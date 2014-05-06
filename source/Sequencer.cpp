#include "Sequencer.h"
#include <fstream>
#include <sys/stat.h>
#include "AudioTools.h"

namespace JuceModule
{

const int Sequencer::TIME_STEP = 3;
const double Sequencer::MS_PER_MINUTE = 60000.0;

double Sequencer::computeTickStep() 
{
  //If dotted whole note : 1 - 2/4, dotted quarter note : 4 - 4/4 => 3 etc
  double rythmUnit = options.rythmUnitDotted ? options.rythmUnit - options.rythmUnit / QUARTER_NOTE : options.rythmUnit;

  double ticksPerMinute = tempo * (double)QUARTER_NOTE / (double)rythmUnit * ticksPerQuarterNote;
  double nbTimeStepPerMinute = MS_PER_MINUTE / (double)TIME_STEP;

  return ticksPerMinute / nbTimeStepPerMinute;
}

double Sequencer::computeMsPerTicks()
{
  //If dotted whole note : 1 - 2/4, dotted quarter note : 4 - 4/4 => 3 etc
  double rythmUnit = options.rythmUnitDotted ? options.rythmUnit - options.rythmUnit / QUARTER_NOTE : options.rythmUnit;
  int ticksPerMinute = tempo * (double)QUARTER_NOTE / (double)rythmUnit * ticksPerQuarterNote;
  return  MS_PER_MINUTE / (double)ticksPerMinute;
}

Sequencer::Sequencer(std::vector<JuceModule::Track::Ptr > tracks, short ticksPerQuarterNote, const Options& options)
  : juce::Thread("Sequencer"), 
    tracks(tracks), 
    ticksPerQuarterNote(ticksPerQuarterNote), 
    options(options),
    paused(false), 
    tempo(options.initialTempo),
    ticks(0.0), 
    stopped(false),
    tempoTrackIndex(0)
{
  //Create the track name meta event to recognize the special tempo sequence
  //See http://home.roadrunner.com/~jgglatt/tech/midifile/example.htm
  unsigned char header[3];
  header[0] = 0xFF;//meta event
  header[1] = 0x03;//track name event

  const char trackName[] = "Tempo Track";
  header[2] = sizeof(trackName);

  juce::MemoryBlock midiData;
  midiData.append(header, sizeof(header));
  midiData.append(trackName, sizeof(trackName));
  juce::MidiMessage trackNameEvent(midiData.getData(), midiData.getSize());
  newTempoTrack.addEvent(trackNameEvent);

  msPerTick = computeMsPerTicks();
  updateTracksMsPerTick(msPerTick);

  tickStep = computeTickStep();
}

Sequencer::~Sequencer()
{
  signalThreadShouldExit();
  notify();
  this->waitForThreadToExit(100);
  stop();
}

void Sequencer::saveSequence(const juce::String& filePath)
{
  jassert(stopped);
  while (isThreadRunning())
    wait(20);
    //Save a midiFile
  juce::MidiFile result;
  //Add the tempo track 
  if (newTempoTrack.getNumEvents() > 1)
    result.addTrack(newTempoTrack);
  
  //Add all the tracks, may be modified
  for (unsigned int i = 0; i < tracks.size(); ++i)
  {
    result.addTrack(tracks[i]->getSequence());
  }
  result.setTicksPerQuarterNote(ticksPerQuarterNote);
  juce::File midiFile = juce::File::getCurrentWorkingDirectory().getChildFile(filePath);
  if (midiFile.existsAsFile())
    midiFile.deleteFile();
  juce::FileOutputStream midiFileStream(midiFile);
  result.writeTo(midiFileStream);
}

double Sequencer::getTick()
{
  const juce::ScopedLock modifyingTicks(ticksAccess);
  return ticks;
}

juce::uint32 Sequencer::getTempo() const
{
  const juce::ScopedLock sL(tempoAccess);
  return tempo;
}

void Sequencer::setTempo(juce::uint32 tempo)
{
  
  int microSecPerQuarterNote = 0;
  const juce::ScopedLock modifyingTempo(tempoAccess);
  {
    const juce::ScopedLock lockTempo(tempoAccess);
    this->tempo = tempo;
    msPerTick = computeMsPerTicks();
    microSecPerQuarterNote = msPerTick * ticksPerQuarterNote * 1000.0;//1000.0 = microsec per millisec
    updateTracksMsPerTick(msPerTick);
  }
  juce::MidiMessage tempoMessage = juce::MidiMessage::tempoMetaEvent(microSecPerQuarterNote);
  {
    const juce::ScopedLock sL(ticksAccess);
    tempoMessage.setTimeStamp(ticks);
    tickStep = computeTickStep();
  }
  newTempoTrack.addEvent(tempoMessage);
}

void Sequencer::setExpression(float value)
{
  if (value > 1.f)
    value = 1.f;
  else if (value < 0.f)
    value = 0.f;

  for (unsigned int i = 0; i < tracks.size(); ++i)
  {
    tracks[i]->setExpression(value);
  }
}

void Sequencer::stop()
{
  {
    const juce::ScopedLock sL(stoppedAccess);
    stopped = true;
    paused = false;
  }
  
  for (int i = 0; i < tracks.size(); ++i)
    tracks[i]->restart();
  
  {
    const juce::ScopedLock sL(ticksAccess);
    ticks = 0;
  }

  tempoTrackIndex = 0;

  //Si on disable l'audio on empeche les NoteOff de restart()
  //d'etre jouees
  //AudioTools::getInstance().disableAudioProcessing();
}

void Sequencer::pause()
{
  {
    const juce::ScopedLock sL(stoppedAccess);
    if (stopped)
      return;
  }

  paused = true;
  AudioTools::getInstance().disableAudioProcessing();
}

void Sequencer::play()
{
  AudioTools::getInstance().enableAudioProcessing();
  {
    const juce::ScopedLock sL(stoppedAccess);
    stopped = false;
  }
  if (!paused)
    startThread();
  else
  {
    paused = false;
    notify();  
  }
}

void Sequencer::run()
{
  {
    const juce::ScopedLock modifyingTicks(ticksAccess);
    ticks = 0; 
  }

  double lag = 0.0;
  double elapsedTime = 0.0;
  double startTime = 0.0;
  bool localStopped = false;
  for (;;)
  {
    startTime = juce::Time::getMillisecondCounterHiRes();
    
    wait(TIME_STEP);

    elapsedTime = juce::Time::getMillisecondCounterHiRes() - startTime;
    lag += elapsedTime;

    while (lag >= TIME_STEP)
    {
       localStopped = stopped;

      //Gestion de la pause
      while (paused && !threadShouldExit() && !localStopped)
        wait(100);

      //Interrompt la lecture si le thread doit être fermé
      if (threadShouldExit())
        return;

      if (!localStopped)
      {
        {
          const juce::ScopedLock modifyingTicks(ticksAccess);
          ticks += tickStep;
        }
        checkTempoChangeTrack();
        for(int i = 0; i< tracks.size(); ++i)
          tracks[i]->playAtTick(ticks);
      }
      else return;
      lag -= TIME_STEP;
      
    }
  }
}

void Sequencer::checkTempoChangeTrack()
{
  if (tempoTrackIndex >= tempoTrack.getNumEvents())
    return;

  const juce::MidiMessageSequence::MidiEventHolder* midiEvent = tempoTrack.getEventPointer(tempoTrackIndex);
  if (midiEvent->message.getTimeStamp() <= ticks)
  {
    if (midiEvent->message.isTempoMetaEvent())
    {
      juce::ScopedLock sL(tempoAccess);
      double rythmUnitModificator = (options.rythmUnit + (options.rythmUnit / 2.0 * options.rythmUnitDotted));
      msPerTick = midiEvent->message.getTempoMetaEventTickLength(ticksPerQuarterNote) * rythmUnitModificator *  1000.0;
      tempo = MS_PER_MINUTE / msPerTick / (double)ticksPerQuarterNote / rythmUnitModificator;
      updateTracksMsPerTick(msPerTick);
    }
    ++tempoTrackIndex;
  }
  
}

void Sequencer::updateTracksMsPerTick(double msPerTick)
{
  for(int i = 0; i< tracks.size(); ++i)
    tracks[i]->setMsPerTick(msPerTick);
}

} // namespace JuceModule