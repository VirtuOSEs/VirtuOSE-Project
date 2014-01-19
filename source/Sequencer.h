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
  Synchronization clock for the Tracks
 **/
class SynchroClock : public juce::Thread
{
public:
  SynchroClock()
  : juce::Thread("SynchroClock"), paused(false), tempo(90), ticks(0.0), msPerTick(0.0),
    stopped(false)
  {}

  SynchroClock(short timeFormat, double tempo = 90.0)
    : juce::Thread("SynchroClock"), timeFormat(timeFormat), paused(false),
      tempo(120), ticks(0.0), msPerTick(60000.0 / (double)tempo / timeFormat),
      stopped(false)
  {}

  void setTimeFormat(short timeFormat)
  {
    this->timeFormat = timeFormat;
    msPerTick = 60000.0 / (double)tempo / timeFormat;
  }

  double getTick()
  {
    const juce::ScopedLock modifyingTicks(ticksAccess);
    return ticks;
  }

  void setTempo(juce::uint32 tempo)
  {
    const juce::ScopedLock modifyingTempo(tempoAccess);
    this->tempo = tempo;
    msPerTick = 60000.0 / (double)tempo / timeFormat;
  }

  void stop()
  {
    {
      const juce::ScopedLock sL(stoppedAccess);
      stopped = true;
    }
    {
      const juce::ScopedLock sL(ticksAccess);
      ticks = 0;
    }
    AudioTools::getInstance().disableAudioProcessing();
  }

  void pause()
  {
    paused = true;
    AudioTools::getInstance().disableAudioProcessing();
  }

  void play()
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

protected:
  virtual void run()
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

      {
      const juce::ScopedLock sL(stoppedAccess);
        if (!stopped)
        {
          const juce::ScopedLock modifyingTicks(ticksAccess);
          ticks++;
        }
        else return;
      }
    }
  }

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

};

/**
  A Track is a juce::Thread playing a midi track in background.
  A Track goal is to play a juce::MidiMessageSequence.
**/
class Track : public juce::Thread
{
public:

  Track(U32 index, SynchroClock& clock, juce::MidiMessageSequence& sequence)
    : juce::Thread("Track"), clock(clock), sequence(sequence)
  {
  }

protected:
  virtual void run();

  SynchroClock& clock;
  juce::MidiMessageSequence& sequence;
};

} // namespace JuceModule

#endif // JUCE_MODULE_SEQUENCER