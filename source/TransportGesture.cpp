#include "TransportGesture.h"
#include "console/console.h"

const float TransportGesture::GESTURE_ZONE_RADIUS_MM = 150.0;
const juce::int64 TransportGesture::STOP_GESTURE_TIME_MS = 1500;

TransportGesture::TransportGesture()
  : status(STOP), 
  firstTimeZoneEntered(0),
  zoneStatus(OUT_ZONE)
{
}

bool TransportGesture::checkTransportGesture(const HandsTracker& handsTracker, const nite::Skeleton& skeleton)
{
  if (!calibrateGesture(skeleton))
    return false;

  distanceBetweenHands = handsTracker.currentRightHand - handsTracker.currentLeftHand;

  //Are hands together and in the gesture zone?
  if ( fabs(distanceBetweenHands.len()) < GESTURE_ZONE_RADIUS_MM && gestureZone.isCoordinateInZone(handsTracker.currentRightHand.x, handsTracker.currentRightHand.y) )
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

bool TransportGesture::calibrateGesture(const nite::Skeleton& skeleton)
{
  const nite::SkeletonJoint& neck = skeleton.getJoint(nite::JOINT_NECK);
  if (neck.getPositionConfidence() < 0.5f)
    return false;

  gestureZone.bottom = neck.getPosition().y - 100.0f;
  gestureZone.top = neck.getPosition().y + 100.0f;
  gestureZone.left = neck.getPosition().x - 100.0f;
  gestureZone.right = neck.getPosition().x + 100.0f;

  return true;
}
