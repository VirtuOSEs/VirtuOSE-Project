#ifndef TS_CALLBACK_H
#define TS_CALLBACK_H

#include "JuceLibraryCode\JuceHeader.h"
#include <queue>

class CallbackManager
{
public:
  static void instrumentWillPlay(const juce::String& instrumentName, double delayInMillis);
  static void instrumentStartPlaying(const juce::String& instrumentName);
  static void instrumentStoppedPlay(const juce::String& instrumentName);

  static void tempoJustChanged(int newTempo);
  static void velocityChanged(float newVelocity);
  static void tempoGestureStart();
  static void tempoGestureEnd();

private:
  CallbackManager(){}
  //static InstrumentStartPlayingCallback instrumentStartPlayingCallback;
  //static VelocityChangedCallback velocityChangedCallback;
  //static TempoGestureStartCallback tempoGestureStartCallback;
  //static TempoGestureEndCallback tempoGestureEndCallback;
};

/*
  Thread objets wrapping TorqueScript callbacks
*/


//class InstrumentStartPlayingCallback : public juce::Thread
//{
//public:
//  InstrumentStartPlayingCallback()
//    : juce::Thread("instrumentStartPlaying")
//  {}
//
//  virtual void run();
//
//  void pushNewCall(const juce::String& instrumentName)
//  {
//    const juce::ScopedLock sL(modifyQueue);
//    instrumentsNames.push(instrumentName);
//  }
//
//private:
//  std::queue<juce::String> instrumentsNames;
//  juce::CriticalSection modifyQueue;
//};
//
//class InstrumentWillPlayCallback : public juce::Thread
//{
//public:
//  juce::String instrumentName;
//  double delay;
//
//  InstrumentWillPlayCallback()
//    : juce::Thread("instrumentWillPlay")
//  {}
//
//  virtual void run();
//};
//
//class InstrumentStoppedPlayingCallback : public juce::Thread
//{
//public:
//  juce::String instrumentName;
//
//  InstrumentStoppedPlayingCallback()
//    : juce::Thread("instrumentStoppedPlaying")
//  {}
//
//  virtual void run();
//
//};
//
//
//class VelocityChangedCallback : public juce::Thread
//{
//public:
//  VelocityChangedCallback()
//    : juce::Thread("velocityChanged")
//  {}
//
//  virtual void run();
//
//  void pushNewCall(float newVelocity)
//  {
//    const juce::ScopedLock sL(modifyQueue);
//    callQueue.push(newVelocity);
//  }
//
//private:
//  std::queue<float> callQueue;
//  juce::CriticalSection modifyQueue;
//};
//
//class TempoGestureStartCallback : public juce::Thread
//{
//public:
//  typedef juce::Thread Parent;
//
//  TempoGestureStartCallback()
//    : Parent("tempoGestureStart"), callCount(0)
//  {}
//
//  virtual void run();
//
//  void pushNewCall()
//  {
//    juce::ScopedLock sL(countAccess);
//    ++callCount;
//  }
//
//private:
//  int callCount;
//  juce::CriticalSection countAccess;
//};
//
//class TempoGestureEndCallback : public juce::Thread
//{
//public:
//  typedef juce::Thread Parent;
//
//  TempoGestureEndCallback()
//    : Parent("tempoGestureEnd"), callCount(0)
//  {}
//
//  virtual void run();
//
//  void pushNewCall()
//  {
//    juce::ScopedLock sL(countAccess);
//    ++callCount;
//  }
//
//private:
//  int callCount;
//  juce::CriticalSection countAccess;
//};

//class TempoJustChangedCallback : public juce::Thread
//{
//public:
//  int newTempo;
//  
//  virtual void run();
//};


#endif // TS_CALLBACK_H