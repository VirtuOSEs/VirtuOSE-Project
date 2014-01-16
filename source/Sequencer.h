#ifndef JUCE_MODULE_SEQUENCER
#define JUCE_MODULE_SEQUENCER

//***********INCLUDE JUCE***************
#include "JuceLibraryCode/JuceHeader.h"

//**********INCLUDE WINDOWS THREAD******
#include <Objbase.h>

//**********INCLUDE TORQUE ENGINE*********
#include "console/engineAPI.h"
#include "platform/threads/threadPool.h"
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
      the audio. Initialise ASIO device, and VST plugin
  */
  static void initialize();
  static AudioTools& getInstance();
  static void deleteInstance();

  void playMidi(const juce::MidiMessageSequence::MidiEventHolder* const midiEvent);
  
  juce::AudioDeviceManager& acquireDeviceManager();
  void releaseDeviceManager();

  juce::ScopedPointer<juce::AudioPluginInstance> acquirePlugin();
  void releasePlugin();

private:
  static AudioTools* singleton;
  AudioTools();
  ~AudioTools();
  AudioTools(const AudioTools& other);
  AudioTools& operator=(const AudioTools& other);

  Semaphore deviceSemaphore;
  Semaphore pluginSemaphore;
  Semaphore playerSemaphore;
  juce::AudioDeviceManager deviceManager;
  juce::ScopedPointer<juce::AudioPluginInstance> plugin;
  juce::AudioProcessorPlayer player;
};

/**
 A Track is a WorkItem (Torque Engine object) wich will be passed to a ThreadPool
  (here the JuceModule::AudioMidiThreadPool). 
  A Track goal is to play a juce::MidiMessageSequence.
**/
class Track : public ThreadPool::WorkItem
{
public:
  typedef ThreadPool::WorkItem Parent;
  U32 mIndex;

  Track(U32 index, juce::MidiMessageSequence& sequence, short timeFormat)
    : mIndex(index), askedToStop(false), sequence(sequence), timeFormat(timeFormat)
  {
  }

  virtual bool isCancellationRequested()
  {return askedToStop;}

  void stop()
  {askedToStop = true;}

protected:
  virtual void execute();

  bool askedToStop;
  juce::MidiMessageSequence& sequence;
  short timeFormat;

};

} // namespace JuceModule

#endif // JUCE_MODULE_SEQUENCER