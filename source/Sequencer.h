#ifndef JUCE_MODULE_SEQUENCER
#define JUCE_MODULE_SEQUENCER

//***********INCLUDE JUCE***************
#include "JuceLibraryCode/JuceHeader.h"

//**********INCLUDE WINDOWS THREAD******
#include <Objbase.h>

//**********INCLUDE TORQUE ENGINE*********
#include "console/engineAPI.h"

#include <vector>

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
   Send a midi message to the plugin to play it.
   Supposed to be thread-safe.
  **/
  void playMidiMessage(const juce::MidiMessage& message);

  void disableAudioProcessing();
  void enableAudioProcessing();

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
  A Track is a juce::Thread playing a midi track in background.
  A Track goal is to play a juce::MidiMessageSequence.
**/
class Track
{
public:

  Track(U32 index, juce::MidiMessageSequence& sequence)
    : sequence(sequence), eventIndex(0)
  {
  }

  void playAtTick(double tick);
  void restart();

protected:
  juce::MidiMessageSequence& sequence;
  int eventIndex;
};


/**
  Synchronization clock for the Tracks
 **/
class Sequencer : public juce::Thread
{
public:
  Sequencer(std::vector<juce::ScopedPointer<JuceModule::Track> >& tracks, short timeFormat,
            double tempo = 92.0);

  double getTick();

  void setTempo(juce::uint32 tempo);

  void stop();
  void pause();
  void play();

protected:
  virtual void run();

private:
  short timeFormat;
  bool paused;
  bool stopped;
  juce::uint32 tempo;
  double ticks;
  double msPerTick;
  juce::CriticalSection ticksAccess;
  juce::CriticalSection tempoAccess;
  juce::CriticalSection stoppedAccess;

  std::vector<juce::ScopedPointer<JuceModule::Track> >& tracks;
};

} // namespace JuceModule

#endif // JUCE_MODULE_SEQUENCER