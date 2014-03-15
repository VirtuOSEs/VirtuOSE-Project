#ifndef KINECT_TEMPO_GESTURE_H
#define KINECT_TEMPO_GESTURE_H

#include "JuceLibraryCode/JuceHeader.h"
#include "NiTE.h"

class TempoGesture
{
public:
  enum GestureStatus{NO_GESTURE, IN_ZONE, OUT_ZONE};

  TempoGesture();

  juce::int32 getTempo() const
    {return tempo;}

  bool checkTempoGesture(const nite::Skeleton& skeleton);

private:
  void calibrateGesture(const nite::SkeletonJoint& rightHip);
  GestureStatus status;
  bool gestureCalibrated;
  double startTime;
  juce::int32 tempo;
  //---ZONE DELIMITERS
  float yBottom;
  float yTop;
  float timeOut;
};

#endif //KINECT_TEMPO_GESTURE_H