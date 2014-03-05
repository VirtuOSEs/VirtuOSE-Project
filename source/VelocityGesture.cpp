#include "VelocityGesture.h"
#include "math/mPoint3.h"

const float VelocityGesture::GESTURE_WIDTH_PERCENTAGE = 50.f/100.f;

VelocityGesture::VelocityGesture()
  : gestureCalibrated(false),
      velocityDetected(0.5f)
{
}

bool VelocityGesture::checkVelocityGesture(const nite::Skeleton& skeleton)
{
  if (!gestureCalibrated)
  {
    tryToCalibrateGesture(skeleton);
  }
  //Still not calibrated ?
  if (!gestureCalibrated)
    return false;

  const nite::SkeletonJoint& hand = skeleton.getJoint(nite::JOINT_LEFT_HAND);
  if (hand.getPositionConfidence() < 0.5f)
    return false;

  float handY = hand.getPosition().y;
  float handX = hand.getPosition().x;

  if (gestureZone.isCoordinateInZone(handX, handY))
  {
    velocityDetected = (handY - gestureZone.bottom) / gestureZone.length();
    return true;
  }

  return false;
}

void VelocityGesture::tryToCalibrateGesture(const nite::Skeleton& skeleton)
{
  const nite::SkeletonJoint& head = skeleton.getJoint(nite::JOINT_HEAD);
  const nite::SkeletonJoint& leftHip = skeleton.getJoint(nite::JOINT_LEFT_HIP);
  const nite::SkeletonJoint& leftHand = skeleton.getJoint(nite::JOINT_LEFT_HAND);
  const nite::SkeletonJoint& leftElbow = skeleton.getJoint(nite::JOINT_LEFT_ELBOW);
  const nite::SkeletonJoint& leftShoulder = skeleton.getJoint(nite::JOINT_LEFT_SHOULDER);
  
  if (leftHip.getPositionConfidence() < 0.5f || head.getPositionConfidence() < 0.5f
      || leftHand.getPositionConfidence() < 0.5f || leftElbow.getPositionConfidence() < 0.5f
      || leftShoulder.getPositionConfidence() < 0.5f)
    return;

  const float hipPosition = leftHip.getPosition().y;
  const float headPosition = head.getPosition().y;
  const float distanceHipToHead = headPosition - hipPosition;

  //Zone starts vertically 10% above the hip and ends 10% above the head
  gestureZone.bottom = hipPosition + distanceHipToHead / 10.f;
  gestureZone.top = headPosition + distanceHipToHead /10.f;

  const Point3F hand(leftHand.getPosition().x, leftHand.getPosition().y, leftHand.getPosition().z);
  const Point3F elbow(leftElbow.getPosition().x, leftElbow.getPosition().y, leftElbow.getPosition().z);
  const Point3F shoulder(leftShoulder.getPosition().x, leftShoulder.getPosition().y, leftShoulder.getPosition().z);

  const VectorF handToElbow = hand - elbow;
  const VectorF elbowToShoulder = elbow - shoulder;

  const float distanceHandToShoulder = handToElbow.len() + elbowToShoulder.len();
  gestureZone.left = shoulder.x;
  gestureZone.right = shoulder.x + distanceHandToShoulder * GESTURE_WIDTH_PERCENTAGE;
  Con::printf("Head x : %f", head.getPosition().x);
  Con::printf("Zone left : %f, Zone Right : %f, Distance hand to shoulder : %f", gestureZone.left, gestureZone.right, distanceHandToShoulder);
  gestureCalibrated = true;
}

