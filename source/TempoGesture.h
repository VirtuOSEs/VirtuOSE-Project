#ifndef KINECT_TEMPO_GESTURE_H
#define KINECT_TEMPO_GESTURE_H

#include "JuceLibraryCode/JuceHeader.h"
#include "NiTE.h"
#include "Options.h"
#include "HandsTracker.h"

/**
   Triggers the following TS Callbacks:
   * onTempoGestureStart()
   * onTempoGestureEnd()
  */
class TempoGesture
{
public:
  enum ZoneStatus{IN_ZONE, OUT_ZONE};

    ///Width of the gesture zone in percentage of the arm lenght
  static const float GESTURE_WIDTH_PERCENTAGE;
  ///Gesture vector "signature": the gesture move can be modelized by this vector
  static const VectorF GESTURE_VECTOR;
  ///Value between 0 and 1 indicating the level of similarity needed between
  ///the GESTURE_VECTOR and the hand direction to consider that the gesture is
  ///being performed
  static const float GESTURE_SIMILARITY_THRESHOLD;
  static const float TIME_OUT;
  
  TempoGesture(const Options& options);

  juce::int32 getTempo() const
    {return tempo;}

  bool checkTempoGesture(const HandsTracker& handsTracker, const nite::Skeleton& skeleton);

private:
  bool calibrateGesture(const nite::Skeleton& skeleton);
  nite::JointType gestureHand; 
  ZoneStatus status;
  bool gestureStarted;
  double startTime;
  juce::int32 tempo;
  //---ZONE DELIMITERS
  float yBottom;
  float yTop;

};

#endif //KINECT_TEMPO_GESTURE_H