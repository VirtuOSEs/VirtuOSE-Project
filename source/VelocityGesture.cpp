#include "VelocityGesture.h"
#include "math/mPoint3.h"

const float VelocityGesture::GESTURE_WIDTH_PERCENTAGE = 50.f/100.f;
const VectorF VelocityGesture::GESTURE_VECTOR = VectorF(0, 1, 0);
const float VelocityGesture::GESTURE_SIMILARITY_THRESHOLD = 0.95f;
const double VelocityGesture::GESTURE_MAX_SPEED_MM_PER_MS = 0.5; 

VelocityGesture::VelocityGesture(const Options& options)
  : velocityDetected(0.5f)
{
  if (options.handedness == LEFT_HANDEDNESS)
  {
    gestureHand = nite::JOINT_RIGHT_HAND;
    gestureElbow = nite::JOINT_RIGHT_ELBOW;
    gestureShoulder = nite::JOINT_RIGHT_SHOULDER;
  }
  else if (options.handedness == RIGHT_HANDEDNESS)
  {
    gestureHand = nite::JOINT_LEFT_HAND;
    gestureElbow = nite::JOINT_LEFT_ELBOW;
    gestureShoulder = nite::JOINT_LEFT_SHOULDER;
  }
}

bool VelocityGesture::checkVelocityGesture(const HandsTracker& handsTracker, const nite::Skeleton& skeleton)
{

  if (! tryToCalibrateGesture(handsTracker, skeleton))
    return false;

  //Detect if current movement is compatible with this gesture
  if (gestureHand == nite::JOINT_LEFT_HAND)
  {
    if (fabs(mDot(handsTracker.leftHandDirection, GESTURE_VECTOR)) < GESTURE_SIMILARITY_THRESHOLD
      || handsTracker.leftHandSpeedMMPerMS > GESTURE_MAX_SPEED_MM_PER_MS)
      return false;
  }
  else if (gestureHand == nite::JOINT_RIGHT_HAND)
  {
    if (fabs(mDot(handsTracker.rightHandDirection, GESTURE_VECTOR)) < GESTURE_SIMILARITY_THRESHOLD
      || handsTracker.rightHandSpeedMMPerMS > GESTURE_MAX_SPEED_MM_PER_MS)
      return false;
  }

  const nite::SkeletonJoint& hand = skeleton.getJoint(gestureHand);

  float handY = hand.getPosition().y;
  float handX = hand.getPosition().x;
  
  if (gestureZone.isCoordinateInZone(handX, handY))
  {
    velocityDetected = (handY - gestureZone.bottom) / gestureZone.length();
    return true;
  }

  return false;
}

bool VelocityGesture::tryToCalibrateGesture(const HandsTracker& handsTracker, const nite::Skeleton& skeleton)
{
  const nite::SkeletonJoint& leftHip = skeleton.getJoint(nite::JOINT_LEFT_HIP);
  const nite::SkeletonJoint& hand = skeleton.getJoint(gestureHand);
  const nite::SkeletonJoint& elbow = skeleton.getJoint(gestureElbow);
  const nite::SkeletonJoint& shoulder = skeleton.getJoint(gestureShoulder);
  
  if (leftHip.getPositionConfidence() < 0.5f 
      || elbow.getPositionConfidence() < 0.5f
      || shoulder.getPositionConfidence() < 0.5f)
    return false;

  const float hipPosition = leftHip.getPosition().y;
  const float headPosition = handsTracker.head.y;
  const float distanceHipToHead = headPosition - hipPosition;

  //Zone starts vertically 10% above the hip and ends 10% above the head
  gestureZone.bottom = hipPosition + distanceHipToHead / 10.f;
  gestureZone.top = headPosition + distanceHipToHead /10.f;

  const Point3F phand(hand.getPosition().x, hand.getPosition().y, hand.getPosition().z);
  const Point3F pelbow(elbow.getPosition().x, elbow.getPosition().y, elbow.getPosition().z);
  const Point3F pshoulder(shoulder.getPosition().x, shoulder.getPosition().y, shoulder.getPosition().z);

  const VectorF handToElbow = phand - pelbow;
  const VectorF elbowToShoulder = pelbow - pshoulder;

  const float distanceHandToShoulder = handToElbow.len() + elbowToShoulder.len();

  //Zone horizontal limit change function of handedness
  if (gestureHand == nite::JOINT_RIGHT_HAND)
  {
    gestureZone.right = pshoulder.x + distanceHandToShoulder * GESTURE_WIDTH_PERCENTAGE;
    gestureZone.left = pshoulder.x;
  }
  else
  {
    gestureZone.right = pshoulder.x;
    gestureZone.left = pshoulder.x - distanceHandToShoulder * GESTURE_WIDTH_PERCENTAGE;
  }
  return true;
}

