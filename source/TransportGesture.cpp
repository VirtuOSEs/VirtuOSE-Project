#include "TransportGesture.h"
#include "console/console.h"

const float TransportGesture::GESTURE_ZONE_RADIUS_MM = 300.0;
const juce::int64 TransportGesture::TIME_TRESHOLD_BETWEEN_TWO_GESTURES_MS = 2000;

TransportGesture::TransportGesture()
  : status(STOP), gestureCalibrated(false), timer(juce::Time::currentTimeMillis())
{
}

void TransportGesture::tryToCalibrateGesture(const nite::Skeleton& skeleton)
{
  const nite::SkeletonJoint torso = skeleton.getJoint(nite::JOINT_TORSO);
  if (torso.getPositionConfidence() < 0.5f)
    return;

  zoneCenter.x = torso.getPosition().x;
  zoneCenter.y = torso.getPosition().y;
  zoneCenter.z = torso.getPosition().z;

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

  if (leftHand.getPositionConfidence() < 0.5f || rightHand.getPositionConfidence() < 0.5f)
    return false;

  const Point3F leftHandPosition(leftHand.getPosition().x, leftHand.getPosition().y, leftHand.getPosition().z);
  const Point3F rightHandPosition(rightHand.getPosition().x, rightHand.getPosition().y, rightHand.getPosition().z);
  const VectorF distanceLeft(leftHandPosition - zoneCenter);
  const VectorF distanceRight(rightHandPosition - zoneCenter);

  if ( fabs(distanceLeft.len()) < GESTURE_ZONE_RADIUS_MM 
    && fabs(distanceRight.len()) < GESTURE_ZONE_RADIUS_MM
    && juce::Time::currentTimeMillis() - timer > TIME_TRESHOLD_BETWEEN_TWO_GESTURES_MS)
  {
    timer = juce::Time::currentTimeMillis();
    if (status == PLAY)
    {
      status = PAUSE;
      Con::printf("PLAY");
    }
    else if (status == PAUSE || status == STOP)
    {
      status = PLAY;
      Con::printf("PAUSE");
    }
    return true;
  }

  return false;
}
