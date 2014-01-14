#ifndef JUCE_MODULE_AUDIO_THREAD_H
#define JUCE_MODULE_AUDIO_THREAD_H  
  
#include "platform/threads/threadPool.h"
#include "JuceLibraryCode\JuceHeader.h"

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
  AudioMidiWorkItem();
  ~AudioMidiWorkItem();
  typedef ThreadPool::WorkItem Parent;
  U32 mIndex;

  AudioMidiWorkItem(U32 index);

  virtual bool isCancellationRequested()
    { return askedToStop;}

  void stop()
    {askedToStop = true;}

protected :
  virtual void execute();
  bool askedToStop;
  juce::AudioDeviceManager deviceManager;
  juce::AudioProcessorPlayer player;
  juce::ScopedPointer<juce::AudioPluginInstance> plugin;
};

} // namespace JuceModule

#endif
