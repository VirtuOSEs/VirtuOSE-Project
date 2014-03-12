#ifndef TS_CALLBACK_H
#define TS_CALLBACK_H

#include "JuceLibraryCode\JuceHeader.h"

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
};


#endif // TS_CALLBACK_H