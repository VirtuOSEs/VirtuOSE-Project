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

void AudioTools::suspendAudioProcessing()
{
  plugin->suspendProcessing(true);
}

void AudioTools::enableAudioProcessing()
{
  plugin->suspendProcessing(false);
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
  juce::MidiMessage noteOff;
  for (int i = 0; i < sequence.getNumEvents(); ++i)
  {

    juce::MidiMessageSequence::MidiEventHolder* midiEvent = sequence.getEventPointer(i);
    nextTime = msPerTick * (midiEvent->message.getTimeStamp() - prevTimestamp);
    
    //Boucle d'attente du prochaine évènement midi : pendant l'attente d'un nouvel event il peut
    //se passer plusieurs choses qu'il faut gérer
    juce::uint64 timeWaited = 0;
    juce::uint64 remainingTimeToWait = juce::uint64(nextTime);
    do
    {
      juce::uint64 startTime = juce::Time::currentTimeMillis();
      //Si le thread est réveillé alors qu'il attend une note, c'est qu'on lui demande d'être mis en pause
      if (wait(juce::uint32(remainingTimeToWait)))
      {
        paused = true; 
        AudioTools::getInstance().suspendAudioProcessing();
        timeWaited += juce::Time::currentTimeMillis() - startTime;//Il reste peut-être du temps à attendre
        remainingTimeToWait = nextTime - timeWaited;
      }
      else remainingTimeToWait = 0;

      //Gestion de la pause
      while (paused && !threadShouldExit())
        wait(300);

      AudioTools::getInstance().enableAudioProcessing();

      //Interrompt la lecture si le thread doit être fermé
      if (threadShouldExit())
        return;
    } while (remainingTimeToWait > 0);

    
    if (midiEvent->message.getTimeStamp() != 0)
    {
      AudioTools::getInstance().playMidiMessage(midiEvent->message); 
    }

    prevTimestamp = midiEvent->message.getTimeStamp();

    if (midiEvent->noteOffObject)
      noteOff = midiEvent->noteOffObject->message;

    //Interrompt la lecture si le thread doit être fermé
    if (threadShouldExit())
      return;
  }
}

} // namespace JuceModule