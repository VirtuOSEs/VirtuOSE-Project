#ifndef VIRTUOSE_HANDS_TRACKER_H
#define VIRTUOSE_HANDS_TRACKER_H

#include "math/mTransform.h"
#include "NiTE.h"
#include "JuceLibraryCode/JuceHeader.h"

/**
  * Helper struct used to group operations
  * about user hands. This struct is internally used
  * by PlayerTracker.
**/
struct HandsTracker
{

  HandsTracker();

  ///Update attribute. Return true if update succeeded,
  ///return false is position confidence was too low and
  ///update was cancelled.
  bool update(const nite::Skeleton& skeleton);

  VectorF leftHandDirection;
  VectorF rightHandDirection;
  float leftHandSpeedMMPerMS;
  float rightHandSpeedMMPerMS;

  Point3F lastLeftHand;///Kinect coordinates
  Point3F lastRightHand;///Kinect coordinates

  Point3F currentLeftHand;///Kinect coordinates
  Point3F currentRightHand;///Kinect coordinates

  Point3F head;///Kinect coordinates

  Point3F torqueCoordinatesLeftHand;
  Point3F torqueCoordinatesRightHand;

  float confidenceThreshold;
  double lastTimeUpdated;
};

#endif // VIRTUOSE_HANDS_TRACKER_H