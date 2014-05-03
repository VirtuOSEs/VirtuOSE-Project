#ifndef TS_CALLBACK_H
#define TS_CALLBACK_H

#include "JuceLibraryCode\JuceHeader.h"

class CallbackManager
{
public:
  static void instrumentWillPlay(const juce::String& trackName, double delayInMillis);
  static void instrumentStartPlaying(const juce::String& trackName);
  static void instrumentStoppedPlay(const juce::String& trackName);

  static void tempoJustChanged(int newTempo);
  static void velocityChanged(float newVelocity);
  static void expressionChanged(float newExpression);
  static void tempoGestureStart();
  static void tempoGestureEnd();

private:
  CallbackManager(){}
};


#endif // TS_CALLBACK_H