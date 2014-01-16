#ifndef JUCE_MODULE_SEQUENCER
#define JUCE_MODULE_SEQUENCER

//***********INCLUDE JUCE***************
#include "JuceLibraryCode/JuceHeader.h"

//**********INCLUDE WINDOWS THREAD******
#include <Objbase.h>

//**********INCLUDE TORQUE ENGINE*********
#include "console/engineAPI.h"

#include "platform/threads/semaphore.h"

namespace JuceModule
{

/** Singleton.
    Contains all important objects for managing sound with JUCE
**/
class AudioTools
{
public:

  /** Call this from the thread in charge of
      the audio
  */
  static void initialize()
  {
    if (singleton == nullptr)
      singleton = new AudioTools();
  }

  static void deleteInstance()
  {
    delete singleton;
    singleton = nullptr;
  }

  static AudioTools& getInstance()
  {
    jassert(singleton);
    return *singleton;
  }

  void playMidi(const juce::MidiMessageSequence::MidiEventHolder* const midiEvent)
  {
    playerSemaphore.acquire();
    player.handleIncomingMidiMessage(nullptr, midiEvent->message);
    playerSemaphore.release();
  }

  juce::AudioDeviceManager& acquireDeviceManager()
  {
    deviceSemaphore.acquire();
    return deviceManager;
  }

  void releaseDeviceManage()
  {
    deviceSemaphore.release();
  }

  juce::ScopedPointer<juce::AudioPluginInstance> acquirePlugin()
  {
    pluginSemaphore.acquire();
    return plugin;
  }

  void releasePlugin()
  {
    pluginSemaphore.release();
  }

    ~AudioTools()
  {
    deviceManager.closeAudioDevice();
    juce::MessageManager::deleteInstance();
    CoUninitialize();
  }

private:
  static AudioTools* singleton;
  AudioTools(const AudioTools& other);
  AudioTools& operator=(const AudioTools& other);

  AudioTools()
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

  Semaphore deviceSemaphore;
  Semaphore pluginSemaphore;
  Semaphore playerSemaphore;
  juce::AudioDeviceManager deviceManager;
  juce::ScopedPointer<juce::AudioPluginInstance> plugin;
  juce::AudioProcessorPlayer player;
};

class Sequence
{
public:
  explicit Sequence(juce::AudioProcessorPlayer& player, const juce::MidiMessageSequence* sequence, const short timeFormat, double tempo = 120.0)
    : player(player), sequence(sequence), currentMessageIndex(0), lastTimePlayed(0.0), finished(false), timeFormat(timeFormat), tempo(tempo), nextTime(0.0),
    starting(false)
  {}

  void start()
  {
    starting = true;
  }

  void playNextEvent()
  {
    if (starting)
    {
      double msPerTick = 60000.0 / tempo / timeFormat; 
      juce::MidiMessageSequence::MidiEventHolder* midiEvent = sequence->getEventPointer(currentMessageIndex);
      nextTime = msPerTick * (midiEvent->message.getTimeStamp() - lastTimePlayed);
      starting = false;

      if (midiEvent->message.getTimeStamp() != 0)
      {
        player.handleIncomingMidiMessage(nullptr, midiEvent->message);    
      }
      currentMessageIndex++;
      if (currentMessageIndex > sequence->getNumEvents())
        finished = true;
      lastTimePlayed = juce::Time::getMillisecondCounter();

      return;
    }

    double msPerTick = 60000.0 / tempo / timeFormat; 
    juce::MidiMessageSequence::MidiEventHolder* midiEvent = sequence->getEventPointer(currentMessageIndex);
    nextTime = msPerTick * (midiEvent->message.getTimeStamp() - lastTimePlayed);
    if (midiEvent->message.getTimeStamp() != 0)
    {
      player.handleIncomingMidiMessage(nullptr, midiEvent->message);    
    }
    lastTimePlayed = juce::Time::getMillisecondCounter();
    currentMessageIndex++;
    if (currentMessageIndex >= sequence->getNumEvents())
      finished = true;
  }

  double getTimeInMillisBeforeNextEvent()
  {
    return nextTime;
  }

  bool isFinished()
    {return finished;}

private:
  juce::AudioProcessorPlayer& player;
  const juce::MidiMessageSequence* sequence;
  unsigned int currentMessageIndex;
  double lastTimePlayed;
  bool finished;
  const short timeFormat;
  double tempo;
  double nextTime;
  bool starting;
};

} // namespace JuceModule

#endif // JUCE_MODULE_SEQUENCER