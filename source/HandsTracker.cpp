#include "HandsTracker.h"

HandsTracker::HandsTracker()
  : confidenceThreshold(0.65f),
  lastTimeUpdated(0.)  
  {}

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

  double elapsedTime = juce::Time::getMillisecondCounterHiRes() - lastTimeUpdated;
  lastTimeUpdated = juce::Time::getMillisecondCounterHiRes();
  leftHandDirection = currentLeftHand - lastLeftHand;
  leftHandSpeedMMPerMS = leftHandDirection.magnitudeSafe() / elapsedTime;
  leftHandDirection.normalizeSafe();
  rightHandDirection = currentRightHand - lastRightHand;
  rightHandSpeedMMPerMS = rightHandDirection.magnitudeSafe() / elapsedTime;
  rightHandDirection.normalizeSafe();

  torqueCoordinatesLeftHand = (currentLeftHand - head) / 150.f;
  torqueCoordinatesRightHand = (currentRightHand - head) / 150.f;
  
  return true;
}