#ifndef KINECT_VELOCITY_GESTURE_H
#define KINECT_VELOCITY_GESTURE_H

#include "JuceLibraryCode/JuceHeader.h"
#include "console/engineAPI.h"
#include "NiTE.h"
#include "Zone2D.h"

class VelocityGesture
{
public:
  //Width of the gesture zone in percentage of the arm lenght
  static const float GESTURE_WIDTH_PERCENTAGE;

  VelocityGesture();

  bool checkVelocityGesture(const nite::Skeleton& skeleton);

  float getVelocityDetected() const
    {return velocityDetected;}

private:
  void tryToCalibrateGesture(const nite::Skeleton& skeleton);

  bool gestureCalibrated;
  Zone2D gestureZone;
  float velocityDetected;
};

#endif //KINECT_VELOCITY_GESTURE_H