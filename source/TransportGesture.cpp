#include "TransportGesture.h"
#include "console/console.h"

const float TransportGesture::GESTURE_ZONE_RADIUS_MM = 250.0;
const juce::int64 TransportGesture::STOP_GESTURE_TIME_MS = 1500;

TransportGesture::TransportGesture()
  : status(STOP), 
  gestureCalibrated(false), 
  firstTimeZoneEntered(0),
  zoneStatus(OUT_ZONE)
{
}

void TransportGesture::tryToCalibrateGesture(const nite::Skeleton& skeleton)
{
  const nite::SkeletonJoint head = skeleton.getJoint(nite::JOINT_HEAD);
  const nite::SkeletonJoint neck = skeleton.getJoint(nite::JOINT_HEAD);
  if (head.getPositionConfidence() <= 0.6f || head.getPositionConfidence() <= 0.6f)
    return;

  zoneCenter.x = head.getPosition().x;
  zoneCenter.y = head.getPosition().y - (head.getPosition().y - neck.getPosition().y) / 2.f; 
  zoneCenter.z = head.getPosition().z;

  gestureCalibrated = true;
}

bool TransportGesture::checkTransportGesture(const nite::Skeleton& skeleton)
{
  if (!gestureCalibrated)
    tryToCalibrateGesture(skeleton);

  if (!gestureCalibrated)
    return false;

  const nite::SkeletonJoint& leftHand = skeleton.getJoint(nite::JOINT_LEFT_HAND);
  const nite::SkeletonJoint& rightHand = skeleton.getJoint(nite::JOINT_RIGHT_HAND);

  if (leftHand.getPositionConfidence() <= 0.55f || rightHand.getPositionConfidence() <= 0.55f)
    return false;

  const Point3F leftHandPosition(leftHand.getPosition().x, leftHand.getPosition().y, leftHand.getPosition().z);
  const Point3F rightHandPosition(rightHand.getPosition().x, rightHand.getPosition().y, rightHand.getPosition().z);
  const VectorF distanceLeft(leftHandPosition - zoneCenter);
  const VectorF distanceRight(rightHandPosition - zoneCenter);

  //Are hands in the gesture zone ?
  if ( fabs(distanceLeft.len()) < GESTURE_ZONE_RADIUS_MM 
    && fabs(distanceRight.len()) < GESTURE_ZONE_RADIUS_MM)
  {
    //If they already were in zone, check if it's a STOP gesture
    if (zoneStatus == IN_ZONE)
    {
      Platform::outputDebugString("ALREADY IN ZONE");
      juce::int64 elapsedTime = juce::Time::currentTimeMillis() - firstTimeZoneEntered;
      if (elapsedTime >= STOP_GESTURE_TIME_MS)
      {
        status = STOP;
        return true;
      }
      else
      {
        return false;
      }
    }
    //If they were out zone, then it's a play or pause gesture
    else if (zoneStatus == OUT_ZONE)
    {
      Platform::outputDebugString("ENTER ZONE");
      zoneStatus = IN_ZONE;
      firstTimeZoneEntered = juce::Time::currentTimeMillis();

      if (status == PLAY)
      {
        status = PAUSE;
      }
      else if (status == PAUSE || status == STOP)
      {
        status = PLAY;
      }
      return true;
    }
    
  }
  else
  {
    firstTimeZoneEntered = 0;
    zoneStatus = OUT_ZONE;
  }

  return false;
}
