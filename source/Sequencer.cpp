#include "Sequencer.h"
#include <fstream>
#include <sys/stat.h>

namespace JuceModule
{

// IMPLEM AUDIOTOOLS
AudioTools* AudioTools::singleton = nullptr;

AudioTools::AudioTools()
{
  juce::AudioDeviceManager::AudioDeviceSetup setup;
  setup.inputChannels = 20;
  setup.outputChannels = 20;
  setup.outputDeviceName = "ASIO4ALL v2";
  setup.inputDeviceName = "ASIO4ALL v2";
  setup.sampleRate = 44100.0000;
  setup.bufferSize = 0;
  setup.useDefaultInputChannels = true;
  setup.useDefaultOutputChannels = true;
    
  
  CoInitialize(nullptr);
  juce::String error = deviceManager.initialise (20, 20, nullptr, false, juce::String::empty, &setup);
  Con::errorf(error.toStdString().c_str());

  deviceManager.playTestSound();
}

AudioTools::~AudioTools()
{
  deviceManager.closeAudioDevice();
  juce::MessageManager::deleteInstance();
  CoUninitialize();
}

void AudioTools::initialize()
{
  if (singleton == nullptr)
    singleton = new AudioTools();
}

void AudioTools::deleteInstance()
{
  delete singleton;
  singleton = nullptr;
}

AudioTools& AudioTools::getInstance()
{
  jassert(singleton);
  return *singleton;
}

void AudioTools::generatePlugin(const juce::String& instrument)
{ 
  juce::AudioPluginFormatManager formatManager;
  formatManager.addDefaultFormats();
  juce::KnownPluginList list;
  juce::String errorMessage;
  juce::PluginDescription description;
  description.name = "sfz";
  description.isInstrument = true;
  description.pluginFormatName = "VST";
  description.category = "Instrument";
  description.fileOrIdentifier =  juce::File::getCurrentWorkingDirectory().getChildFile("../sfz.dll").getFullPathName();

  juce::File fxpFile = juce::File::getCurrentWorkingDirectory().getChildFile("../fxp/" + instrument + ".fxp").getFullPathName();
  if (!fxpFile.existsAsFile())
  {
    Con::errorf("Impossible de charger l'instrument demandé");
    return;
  }
  Con::printf(juce::String("Chargement de " + instrument).toStdString().c_str());

  //Si le plugin correspondant à l'instrument n'existe pas encore
  if (pluginsMap.find(instrument) == pluginsMap.end())
  {
    pluginsMap[instrument] = formatManager.createPluginInstance(description, errorMessage);

    juce::MemoryBlock fxpData;
    fxpFile.loadFileAsData(fxpData);
    juce::VSTPluginFormat::loadFromFXBFile(pluginsMap[instrument], fxpData.getData(), fxpData.getSize());

    playersMap[instrument] = new juce::AudioProcessorPlayer();
    playersMap[instrument]->setProcessor(pluginsMap[instrument]);
    deviceManager.addAudioCallback(playersMap[instrument]);
  }
}

void AudioTools::makePluginPlay(const juce::String& instrument, const juce::MidiMessage& message)
{
  if(pluginsMap.find(instrument) == pluginsMap.end())
    return;

  const juce::ScopedLock sL(criticalSection);
  playersMap[instrument]->handleIncomingMidiMessage(nullptr, message);
}

void AudioTools::disableAudioProcessing()
{
  for (auto plugin = pluginsMap.begin(); plugin != pluginsMap.end(); ++plugin)
    plugin->second->suspendProcessing(true);

}

void AudioTools::enableAudioProcessing()
{
  for (auto plugin = pluginsMap.begin(); plugin != pluginsMap.end(); ++plugin)
    plugin->second->suspendProcessing(false);

}

// IMPLEM SEQUENCER

Sequencer::Sequencer(std::vector<JuceModule::Track::Ptr > tracks, short timeFormat, double tempo)
  : juce::Thread("Sequencer"), 
    tracks(tracks), 
    timeFormat(timeFormat), 
    paused(false), 
    tempo(tempo),
    ticks(0.0), 
    msPerTick(60000.0 / (double)tempo / timeFormat), stopped(false)
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
  tempoTrack.addEvent(trackNameEvent);
}

Sequencer::~Sequencer()
{
  signalThreadShouldExit();
  notify();
}

void Sequencer::saveSequence(const juce::String& filePath)
{
  while (isThreadRunning())
    wait(20);
    //Save a midiFile
  juce::MidiFile result;
  //Add the tempo track 
  result.addTrack(tempoTrack);
  
  //Add all the tracks, may be modified
  for (unsigned int i = 0; i < tracks.size(); ++i)
  {
    if (tracks[i]->getTrackName().containsIgnoreCase("symphony"))
      continue;
    result.addTrack(tracks[i]->getSequence());
  }
  result.setTicksPerQuarterNote(timeFormat);
  juce::File midiFile = juce::File::getCurrentWorkingDirectory().getChildFile(filePath);
  if (midiFile.existsAsFile())
    midiFile.deleteFile();
  juce::FileOutputStream midiFileStream(midiFile);
  jassert(result.writeTo(midiFileStream));
}

double Sequencer::getTick()
{
  const juce::ScopedLock modifyingTicks(ticksAccess);
  return ticks;
}

void Sequencer::setTempo(juce::uint32 tempo)
{
  
  const juce::ScopedLock modifyingTempo(tempoAccess);
  this->tempo = tempo;
  msPerTick = 60000.0 / (double)tempo / timeFormat;
  juce::MidiMessage tempoMessage = juce::MidiMessage::tempoMetaEvent(msPerTick * timeFormat);
  tempoTrack.addEvent(tempoMessage);
}

void Sequencer::stop()
{
  {
    const juce::ScopedLock sL(stoppedAccess);
    stopped = true;
  }
  {
    const juce::ScopedLock sL(ticksAccess);
    ticks = 0;
  }
  for (int i = 0; i < tracks.size(); ++i)
    tracks[i]->restart();

  //Si on disable l'audio on empeche les NoteOff de restart()
  //d'etre jouees
  //AudioTools::getInstance().disableAudioProcessing();
}

void Sequencer::pause()
{
  paused = true;
  AudioTools::getInstance().disableAudioProcessing();
}

void Sequencer::play()
{
  AudioTools::getInstance().enableAudioProcessing();
  stopped = false;
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
  for (;;)
  {
    startTime = juce::Time::getMillisecondCounterHiRes();
    wait(msPerTick);
    elapsedTime = juce::Time::getMillisecondCounterHiRes() - startTime;
    lag += elapsedTime;

    while (lag >= msPerTick)
    {
      //Gestion de la pause
      while (paused && !threadShouldExit())
        wait(100);

      //Interrompt la lecture si le thread doit être fermé
      if (threadShouldExit())
        return;

      { //stoppedAccess scope
        const juce::ScopedLock sL(stoppedAccess);
        if (!stopped)
        {
          const juce::ScopedLock modifyingTicks(ticksAccess);
          ticks++;
        
          for(int i = 0; i< tracks.size(); ++i)
            tracks[i]->playAtTick(ticks);
        }
        else return;
      }//stoppedAccess scope

      lag -= msPerTick;
    }//end of while (lag >= msPerTick)
  }
}

//     IMPLEM TRACK

Track::Track(U32 index, juce::MidiMessageSequence& sequence)
  : sequence(sequence), eventIndex(0), trackName(juce::String::empty)
{
  int i = 0;
  while (i < sequence.getNumEvents() && trackName == juce::String::empty)
  {
    juce::MidiMessage& message = sequence.getEventPointer(i)->message;
    if (message.isTrackNameEvent())
    {
      trackName = message.getTextFromTextMetaEvent();
    }
    ++i;
  }
  instrumentName = extractInstrumentNameFromTrackName(trackName);
  AudioTools::getInstance().generatePlugin(instrumentName);
}

void Track::restart()
{
  if (eventIndex == 0)
    return;

  //Tentative d'annuler le bug de la note tenue lors d'un stop() play()
  int i = eventIndex;
  while (i < sequence.getNumEvents())
  {
    juce::MidiMessageSequence::MidiEventHolder* midiEvent = sequence.getEventPointer(i);
    if (midiEvent->message.isNoteOff())
      AudioTools::getInstance().makePluginPlay(instrumentName, midiEvent->message);
    ++i;
  }

  eventIndex = 0;
}

void Track::playAtTick(double tick)
{
  if (eventIndex >= sequence.getNumEvents())
    return;

  juce::MidiMessageSequence::MidiEventHolder* midiEvent = nullptr;
  {
    juce::ScopedLock sl(sequenceAccess);
    midiEvent = sequence.getEventPointer(eventIndex);
  }

  double timeStamp = midiEvent->message.getTimeStamp();

  if (timeStamp == 0) 
  { 
    eventIndex++;
    return;
  }

  if ( timeStamp <= tick)
  {
    AudioTools::getInstance().makePluginPlay(instrumentName, midiEvent->message);
    eventIndex++;
  }
}

juce::MidiMessageSequence Track::getSequence() const
{
  juce::ScopedLock sL(sequenceAccess);
  return sequence;

}

juce::String Track::extractInstrumentNameFromTrackName(const juce::String& trackName)
{
  //On regarde les noms des fichiers .fxp dans le répertoire fxp
  //Si la trackName contient un de ces noms, c'est l'instrument qu'on cherche !
  juce::Array<juce::File> fxpFiles;
  juce::File fxpDirectory = juce::File::getCurrentWorkingDirectory().getChildFile("../fxp").getFullPathName();
  fxpDirectory.findChildFiles(fxpFiles, juce::File::findFiles, false, "*.fxp");
  for(int i = 0; i < fxpFiles.size(); ++i)
  {
    if (trackName.containsIgnoreCase(fxpFiles[i].getFileNameWithoutExtension()))
      return fxpFiles[i].getFileNameWithoutExtension();
  }
  return juce::String::empty;
}

} // namespace JuceModule