#include "TransportGesture.h"
#include "console/console.h"

const float TransportGesture::GESTURE_ZONE_RADIUS_MM = 100.0;
const juce::int64 TransportGesture::STOP_GESTURE_TIME_MS = 1500;

TransportGesture::TransportGesture()
  : status(STOP), 
  gestureCalibrated(false), 
  firstTimeZoneEntered(0),
  zoneStatus(OUT_ZONE)
{
}

bool TransportGesture::checkTransportGesture(const nite::Skeleton& skeleton)
{
  const nite::SkeletonJoint& leftHand = skeleton.getJoint(nite::JOINT_LEFT_HAND);
  const nite::SkeletonJoint& rightHand = skeleton.getJoint(nite::JOINT_RIGHT_HAND);

  if (leftHand.getPositionConfidence() <= 0.55f || rightHand.getPositionConfidence() <= 0.55f)
    return false;

  leftHandPosition.x = leftHand.getPosition().x; leftHandPosition.y = leftHand.getPosition().y; leftHandPosition.z = leftHand.getPosition().z;
  rightHandPosition.x = rightHand.getPosition().x; rightHandPosition.y = rightHand.getPosition().y; rightHandPosition.z = rightHand.getPosition().z;
  distanceBetweenHands = rightHandPosition - leftHandPosition;

  //Are hands together ?
  if ( fabs(distanceBetweenHands.len()) < GESTURE_ZONE_RADIUS_MM)
  {
    //If they already were in zone, check if it's a STOP gesture
    if (zoneStatus == IN_ZONE)
    {
      juce::int64 elapsedTime = juce::Time::currentTimeMillis() - firstTimeZoneEntered;
      if (elapsedTime >= STOP_GESTURE_TIME_MS && status != STOP)
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
