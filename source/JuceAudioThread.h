#ifndef JUCE_MODULE_AUDIO_THREAD_H
#define JUCE_MODULE_AUDIO_THREAD_H  
  
#include "platform/threads/threadPool.h"
#include "JuceLibraryCode\JuceHeader.h"

#include "Sequencer.h"

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

  ~AudioMidiWorkItem();
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
  virtual void execute()
  {
    double startTime = sequence.getStartTime();
    double msPerTick = 60000.0 / 120.0 / timeFormat; 
    double nextTime;
    double prevTimestamp = 0.0;


    for (int i = 0; i < sequence.getNumEvents(); ++i)
    {
      juce::MidiMessageSequence::MidiEventHolder* midiEvent = sequence.getEventPointer(i);
      nextTime = msPerTick * (midiEvent->message.getTimeStamp() - prevTimestamp);

      juce::Time::waitForMillisecondCounter(juce::Time::getMillisecondCounter() + juce::uint32(nextTime));
      if (midiEvent->message.getTimeStamp() != 0)
      {
        AudioTools::getInstance().playMidi(midiEvent);
        //player.handleIncomingMidiMessage(nullptr, midiEvent->message);    
      }

      prevTimestamp = midiEvent->message.getTimeStamp();

      //Interrompt la lecture si le thread doit être fermé
      if (cancellationPoint())
        break;
    }
  }

  bool askedToStop;
  juce::MidiMessageSequence& sequence;
  short timeFormat;

};

} // namespace JuceModule

#endif
