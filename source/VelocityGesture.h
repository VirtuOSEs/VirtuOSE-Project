#ifndef KINECT_VELOCITY_GESTURE_H
#define KINECT_VELOCITY_GESTURE_H

#include "JuceLibraryCode/JuceHeader.h"
#include "NiTE.h"
#include "Zone2D.h"
#include "Options.h"
#include "HandsTracker.h"
#include "math/mPoint3.h"
  
class VelocityGesture
{
public:
  ///Width of the gesture zone in percentage of the arm lenght
  static const float GESTURE_WIDTH_PERCENTAGE;
  ///Gesture vector "signature": the gesture move can be modelized by this vector
  static const VectorF GESTURE_VECTOR;
  ///Value between 0 and 1 indicating the level of similarity needed between
  ///the GESTURE_VECTOR and the hand direction to consider that the gesture is
  ///being performed
  static const float GESTURE_SIMILARITY_THRESHOLD;
  ///Max hand speed before hand movement no more considered as a velocity gesture
  static const double GESTURE_MAX_SPEED_MM_PER_MS;

  VelocityGesture(const Options& options);

  bool checkVelocityGesture(const HandsTracker& handsTracker, const nite::Skeleton& skeleton);

  float getVelocityDetected() const
    {return velocityDetected;}

private:
  bool tryToCalibrateGesture(const HandsTracker& handsTracker, const nite::Skeleton& skeleton);
  
  nite::JointType gestureHand;
  nite::JointType gestureElbow;
  nite::JointType gestureShoulder;

  Zone2D gestureZone;
  float velocityDetected;
};

#endif //KINECT_VELOCITY_GESTURE_H