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
    
  juce::AudioPluginFormatManager formatManager;
  formatManager.addDefaultFormats();
  juce::KnownPluginList list;
  juce::String errorMessage;
  juce::PluginDescription description2;
  description2.name = "4Front Piano";
  description2.isInstrument = true;
  description2.pluginFormatName = "VST";
  description2.category = "Instrument";
  description2.fileOrIdentifier =  juce::File::getCurrentWorkingDirectory().getChildFile("../sfz.dll").getFullPathName();
  plugin = formatManager.createPluginInstance(description2, errorMessage);
  Con::printf(errorMessage.getCharPointer());
  
  CoInitialize(nullptr);
  juce::String error = deviceManager.initialise (20, 20, nullptr, false, juce::String::empty, &setup);
  Con::printf(error.toStdString().c_str());

  deviceManager.playTestSound();
  player.setProcessor(plugin);
  deviceManager.addAudioCallback(&player);
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

void AudioTools::playMidiMessage(const juce::MidiMessage& message)
{
  const juce::ScopedLock threadLock(criticalSection);
  player.handleIncomingMidiMessage(nullptr, message);
}

void AudioTools::disableAudioProcessing()
{
  plugin->suspendProcessing(true);
}

void AudioTools::enableAudioProcessing()
{
  plugin->suspendProcessing(false);
}

// IMPLEM SEQUENCER

Sequencer::Sequencer(std::vector<JuceModule::Track::Ptr > tracks, short timeFormat, double tempo)
  : juce::Thread("Sequencer"), tracks(tracks), timeFormat(timeFormat), paused(false), tempo(tempo),
    ticks(0.0), msPerTick(60000.0 / (double)tempo / timeFormat), stopped(false)
{}

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

  AudioTools::getInstance().disableAudioProcessing();
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

  for (;;)
  {
    wait(msPerTick);

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

  }
}

//     IMPLEM TRACK

void Track::restart()
  {eventIndex = 0;}

void Track::playAtTick(double tick)
{
    juce::MidiMessageSequence::MidiEventHolder* midiEvent = sequence.getEventPointer(eventIndex);
    double timeStamp = midiEvent->message.getTimeStamp();

    if (timeStamp == 0) 
    { 
      eventIndex++;
      return;
    }

    if ( timeStamp <= tick)
    {
      //AudioTools::getInstance().playMidiMessage(midiEvent->message); 
      AudioTools::getInstance().makePluginPlay(instrumentName, midiEvent->message);
      eventIndex++;
    }
}

} // namespace JuceModule