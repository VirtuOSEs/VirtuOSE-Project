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
  description2.name = "4Front_Piano";
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

void AudioTools::playMidiEvent(const juce::MidiMessageSequence::MidiEventHolder* const midiEvent)
{
  const juce::ScopedLock threadLock(criticalSection);
  player.handleIncomingMidiMessage(nullptr, midiEvent->message);
}

//     IMPLEM TRACK

void Track::pause()
  {paused = true;}

void Track::play()
  {paused = false;}

void Track::run()
{
  //double startTime = sequence.getStartTime();
  double msPerTick = 60000.0 / 120.0 / timeFormat; 
  double nextTime;
  double prevTimestamp = 0.0;

  for (int i = 0; i < sequence.getNumEvents(); ++i)
  {
    while (paused)
    {
      wait(300);
    }

    juce::MidiMessageSequence::MidiEventHolder* midiEvent = sequence.getEventPointer(i);
    nextTime = msPerTick * (midiEvent->message.getTimeStamp() - prevTimestamp);

    wait(juce::uint32(nextTime));
    if (midiEvent->message.getTimeStamp() != 0)
    {
      AudioTools::getInstance().playMidiEvent(midiEvent); 
    }

    prevTimestamp = midiEvent->message.getTimeStamp();

    //Interrompt la lecture si le thread doit être fermé
    if (threadShouldExit())
      break;
  }
}

} // namespace JuceModule