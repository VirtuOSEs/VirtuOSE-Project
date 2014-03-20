#ifndef KINECT_VELOCITY_GESTURE_H
#define KINECT_VELOCITY_GESTURE_H

#include "JuceLibraryCode/JuceHeader.h"
#include "console/engineAPI.h"
#include "NiTE.h"
#include "Zone2D.h"
#include "Options.h"
  
class VelocityGesture
{
public:
  //Width of the gesture zone in percentage of the arm lenght
  static const float GESTURE_WIDTH_PERCENTAGE;

  VelocityGesture(const Options& options);

  bool checkVelocityGesture(const nite::Skeleton& skeleton);

  float getVelocityDetected() const
    {return velocityDetected;}

private:
  void tryToCalibrateGesture(const nite::Skeleton& skeleton);
  
  nite::JointType gestureHand;
  nite::JointType gestureElbow;
  nite::JointType gestureShoulder;

  bool gestureCalibrated;
  Zone2D gestureZone;
  float velocityDetected;
};

#endif //KINECT_VELOCITY_GESTURE_H