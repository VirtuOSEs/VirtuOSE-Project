#include "HandsTracker.h"

bool HandsTracker::update(const nite::Skeleton& skeleton)
{
  if (skeleton.getJoint(nite::JOINT_RIGHT_HAND).getPositionConfidence() < confidenceThreshold
    || skeleton.getJoint(nite::JOINT_LEFT_HAND).getPositionConfidence() < confidenceThreshold
    || skeleton.getJoint(nite::JOINT_HEAD).getPositionConfidence() < confidenceThreshold)
    return false;

  lastLeftHand = currentLeftHand;
  lastRightHand = currentRightHand;

  currentLeftHand.x = skeleton.getJoint(nite::JOINT_LEFT_HAND).getPosition().x;
  currentLeftHand.y = skeleton.getJoint(nite::JOINT_LEFT_HAND).getPosition().y;
  currentLeftHand.z = skeleton.getJoint(nite::JOINT_LEFT_HAND).getPosition().z;

  currentRightHand.x = skeleton.getJoint(nite::JOINT_RIGHT_HAND).getPosition().x;
  currentRightHand.y = skeleton.getJoint(nite::JOINT_RIGHT_HAND).getPosition().y;
  currentRightHand.z = skeleton.getJoint(nite::JOINT_RIGHT_HAND).getPosition().z;

  head.x = skeleton.getJoint(nite::JOINT_HEAD).getPosition().x;
  head.y = skeleton.getJoint(nite::JOINT_HEAD).getPosition().y;
  head.z = skeleton.getJoint(nite::JOINT_HEAD).getPosition().z;

  leftHandDirection = currentLeftHand - lastLeftHand;
  rightHandDirection = currentRightHand - lastRightHand;

  torqueCoordinatesLeftHand = (currentLeftHand - head) / 150.f;
  torqueCoordinatesRightHand = (currentRightHand - head) / 150.f;
  
  return true;
}