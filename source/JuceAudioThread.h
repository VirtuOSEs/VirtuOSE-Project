#ifndef JUCE_MODULE_AUDIO_THREAD_H
#define JUCE_MODULE_AUDIO_THREAD_H  
  #include "platform/threads/threadPool.h"

  #define JUCE_MODULE_THREADPOOL_CNT 1

namespace JuceModule
{

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

struct AudioMidiWorkItem : public ThreadPool::WorkItem
{
  typedef ThreadPool::WorkItem Parent;
  U32 mIndex;

  AudioMidiWorkItem(U32 index)
    : mIndex(index), askedToStop(false) {}

  virtual bool isCancellationRequested()
    { return askedToStop;}

  void stop()
    {askedToStop = true;}

protected :
  virtual void execute();
  bool askedToStop;
};

} // namespace JuceModule

#endif
