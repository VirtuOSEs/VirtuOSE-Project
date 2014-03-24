#ifndef VIRTUOSE_HANDS_TRACKER_H
#define VIRTUOSE_HANDS_TRACKER_H

#include "math/mTransform.h"
#include "NiTE.h"

/**
  * Helper struct used to group operations
  * about user hands. This struct is internally used
  * by PlayerTracker.
**/
struct HandsTracker
{

  HandsTracker()
    : confidenceThreshold(0.65f)
  {}

  ///Update attribute. Return true if update succeeded,
  ///return false is position confidence was too low and
  ///update was cancelled.
  bool update(const nite::Skeleton& skeleton);

  VectorF leftHandDirection;
  VectorF rightHandDirection;

  Point3F lastLeftHand;///Kinect coordinates
  Point3F lastRightHand;///Kinect coordinates

  Point3F currentLeftHand;///Kinect coordinates
  Point3F currentRightHand;///Kinect coordinates

  Point3F head;///Kinect coordinates

  Point3F torqueCoordinatesLeftHand;
  Point3F torqueCoordinatesRightHand;

  float confidenceThreshold;
};

#endif // VIRTUOSE_HANDS_TRACKER_H