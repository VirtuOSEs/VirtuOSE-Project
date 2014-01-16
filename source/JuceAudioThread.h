#ifndef JUCE_MODULE_AUDIO_THREAD_H
#define JUCE_MODULE_AUDIO_THREAD_H  
  
#include "platform/threads/threadPool.h"

#define JUCE_MODULE_THREADPOOL_CNT 1

namespace JuceModule
{

/**
  This class is a ThreadPool wich goal is to handle the WorkItems (here they are JuceModule::Track)
  which play the MIDI Tracks of the JuceModule::MidiPlayer.
  Each Track is added to this ThreadPool in order to be executed in parrallel with the others.
  Today, this ThreadPool manage just one thread, but it may evolve with future needs.
**/
class AudioMidiThreadPool : public ThreadPool, public ManagedSingleton<AudioMidiThreadPool>
{
public:
  typedef ThreadPool Parent;

  AudioMidiThreadPool()
    : Parent("AudioMidiThreadPool", JUCE_MODULE_THREADPOOL_CNT)
  {
  }

  static const char* getSingletonName()
  {
    return "AudioMidiThreadPool";
  }
};

} // namespace JuceModule

#endif
