#ifndef TS_CALLBACK_H
#define TS_CALLBACK_H

#include "JuceLibraryCode\JuceHeader.h"

namespace CallbackManager
{
  void instrumentWillPlay(const juce::String& trackName, double delayInMillis);
  void instrumentStartPlaying(const juce::String& trackName);
  void instrumentStoppedPlay(const juce::String& trackName);

  void tempoJustChanged(int newTempo);
  void velocityChanged(float newVelocity);
  void expressionChanged(float newExpression);
  void tempoGestureStart();
  void tempoGestureEnd();

  void play();
  void pause();
  void stop();
};


#endif // TS_CALLBACK_H