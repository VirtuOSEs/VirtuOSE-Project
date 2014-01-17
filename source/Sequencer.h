#ifndef JUCE_MODULE_SEQUENCER
#define JUCE_MODULE_SEQUENCER

//***********INCLUDE JUCE***************
#include "JuceLibraryCode/JuceHeader.h"

//**********INCLUDE WINDOWS THREAD******
#include <Objbase.h>

//**********INCLUDE TORQUE ENGINE*********
#include "console/engineAPI.h"

namespace JuceModule
{

/** Singleton.
    Contains all important objects for managing sound with JUCE
**/
class AudioTools
{
public:

  /** The thread calling this will be the handler of all the important
      audio objects (sound device, VST plugin...).
      Currently this static method is called by the Torque's module "Juce Module"
      (JuceModule.cpp), so I think audio objects are maintained by the main thread.
      Initialise ASIO device, and VST plugin
  */
  static void initialize();
  static AudioTools& getInstance();
  static void deleteInstance();

  /** 
   Send a midi event to the plugin to play it.
   Supposed to be thread-safe.
  **/
  void playMidiEvent(const juce::MidiMessageSequence::MidiEventHolder* const midiEvent);

private:
  static AudioTools* singleton;
  AudioTools();
  ~AudioTools();
  AudioTools(const AudioTools& other);
  AudioTools& operator=(const AudioTools& other);

  juce::CriticalSection criticalSection;
  juce::AudioDeviceManager deviceManager;
  juce::ScopedPointer<juce::AudioPluginInstance> plugin;
  juce::AudioProcessorPlayer player;
};

/**
 A Track is a WorkItem (Torque Engine object) wich will be passed to a ThreadPool
  (here the JuceModule::AudioMidiThreadPool). 
  A Track goal is to play a juce::MidiMessageSequence.
**/
class Track : public juce::Thread
{
public:

  Track(U32 index, juce::MidiMessageSequence& sequence, short timeFormat)
    : juce::Thread("Track"), askedToStop(false), paused(false), sequence(sequence), timeFormat(timeFormat)
  {
  }

  virtual bool isCancellationRequested()
  {return askedToStop;}

  void stop()
  {askedToStop = true;}

  void pause();
  void play();

protected:
  virtual void run();

  bool askedToStop;
  bool paused;
  juce::MidiMessageSequence& sequence;
  short timeFormat;

};

} // namespace JuceModule

#endif // JUCE_MODULE_SEQUENCER