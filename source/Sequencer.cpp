#include "Sequencer.h"

namespace JuceModule
{

// IMPLEM AUDIOTOOLS
AudioTools* AudioTools::singleton = nullptr;

AudioTools::AudioTools()
{
  juce::AudioDeviceManager::AudioDeviceSetup setup;
  setup.inputChannels = 256;
  setup.outputChannels = 256;
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
  description2.pluginFormatName = "VST";
  description2.category = "Instrument";
  description2.fileOrIdentifier =  juce::File::getCurrentWorkingDirectory().getChildFile("../4Front_Piano.dll").getFullPathName();
  plugin = formatManager.createPluginInstance(description2, errorMessage);
  Con::printf(errorMessage.getCharPointer());

  CoInitialize(nullptr);
  juce::String error = deviceManager.initialise (2, 2, nullptr, true, juce::String::empty, &setup);
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

//     IMPLEM TRACK

void Track::run()
{
  int eventIndex = 0;
  while (eventIndex < sequence.getNumEvents())
  {
    juce::MidiMessageSequence::MidiEventHolder* midiEvent = sequence.getEventPointer(eventIndex);
    double timeStamp = midiEvent->message.getTimeStamp();

    if (timeStamp == 0) 
    { 
      eventIndex++;
      continue;
    }

    if ( timeStamp <= clock.getTick())
    {
      AudioTools::getInstance().playMidiMessage(midiEvent->message); 
      eventIndex++;
    }

    //Interrompt la lecture si le thread doit être fermé
    if (threadShouldExit())
      return;
  }
  
}

} // namespace JuceModule