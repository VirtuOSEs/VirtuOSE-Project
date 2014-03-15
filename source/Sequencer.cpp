#include "Sequencer.h"
#include <fstream>
#include <sys/stat.h>
#include "AudioTools.h"

namespace JuceModule
{

Sequencer::Sequencer(std::vector<JuceModule::Track::Ptr > tracks, short timeFormat, double tempo)
  : juce::Thread("Sequencer"), 
    tracks(tracks), 
    timeFormat(timeFormat), 
    paused(false), 
    tempo(tempo),
    ticks(0.0), 
    msPerTick(60000.0 / (double)tempo / timeFormat), stopped(false),
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
  updateTracksMsPerTick(msPerTick);
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
  result.setTicksPerQuarterNote(timeFormat);
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
    msPerTick = 60000.0 / (double)tempo / (double)timeFormat;
    microSecPerQuarterNote = msPerTick * timeFormat * 1000.0;
    updateTracksMsPerTick(msPerTick);
  }
  juce::MidiMessage tempoMessage = juce::MidiMessage::tempoMetaEvent(microSecPerQuarterNote);
  {
    const juce::ScopedLock sL(ticksAccess);
    tempoMessage.setTimeStamp(ticks);
  }
  newTempoTrack.addEvent(tempoMessage);
}

void Sequencer::increaseVelocityFactorInPercent(short percentage)
{
  for (unsigned int i = 0; i < tracks.size(); ++i)
  {
    tracks[i]->increaseVelocityFactor(percentage);
  }
}

void Sequencer::decreaseVelocityFactorInPercent(short percentage)
{
  for (unsigned int i = 0; i < tracks.size(); ++i)
  {
    tracks[i]->decreaseVelocityFactor(percentage);
  }
}

void Sequencer::setVelocityAbsolute(float value)
{
  if (value > 1.f)
    value = 1.f;
  else if (value < 0.f)
    value = 0.f;

  for (unsigned int i = 0; i < tracks.size(); ++i)
  {
    tracks[i]->setVelocity(value);
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
  //Tentative d'optimisation de l'accès concurrentiel 
  //à msPerTick et stopped
  double localMsPerTick = 0.0;
  bool localStopped = false;
  for (;;)
  {
    startTime = juce::Time::getMillisecondCounterHiRes();
    //Tentative d'optimisation de l'accès concurrentiel 
    //à msPerTick
    {
      const juce::ScopedLock tempoLock(tempoAccess);
      localMsPerTick = msPerTick;
    }
    wait(localMsPerTick);
    elapsedTime = juce::Time::getMillisecondCounterHiRes() - startTime;
    lag += elapsedTime;

    while (lag >= localMsPerTick)
    {
      { //stoppedAccess scope
        const juce::ScopedLock sL(stoppedAccess);
        localStopped = stopped;
      }

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
          ticks++;
        }
        checkTempoChangeTrack();
        for(int i = 0; i< tracks.size(); ++i)
          tracks[i]->playAtTick(ticks);
      }
      else return;
      lag -= localMsPerTick;
      
    }// end of tempo access (critical section)
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
      msPerTick = midiEvent->message.getTempoMetaEventTickLength(timeFormat) * 1000.0;
      tempo = 60000.0 / msPerTick / (double)timeFormat;
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