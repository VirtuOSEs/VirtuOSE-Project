#include "TempoGesture.h"
#include "TSCallback.h"

const float TempoGesture::GESTURE_WIDTH_PERCENTAGE = 50.f/100.f;
const VectorF TempoGesture::GESTURE_VECTOR = VectorF(0, 1, 0);
const float TempoGesture::GESTURE_SIMILARITY_THRESHOLD = 0.95f;
const float TempoGesture::TIME_OUT = 2000.f;

TempoGesture::TempoGesture(const Options& options)
  : status(OUT_ZONE),
    startTime(0.),
    tempo(options.initialTempo),
    yBottom(0.f),
    yTop(150.f),
    gestureStarted(false)
{
  if (options.handedness == LEFT_HANDEDNESS)
    gestureHand = nite::JOINT_LEFT_HAND;
  else if (options.handedness == RIGHT_HANDEDNESS)
    gestureHand = nite::JOINT_RIGHT_HAND;

}

bool TempoGesture::checkTempoGesture(const HandsTracker& handsTracker, const nite::Skeleton& skeleton)
{
  const nite::SkeletonJoint& hand = skeleton.getJoint(gestureHand);
  
  if (!calibrateGesture(skeleton))
    return false;

  VectorF handDirection;
   //Detect if current movement is compatible with this gesture
  if (gestureHand == nite::JOINT_LEFT_HAND)
  {
    handDirection = handsTracker.leftHandDirection;
    if (fabs(mDot(handsTracker.leftHandDirection, GESTURE_VECTOR)) < GESTURE_SIMILARITY_THRESHOLD)
      return false;
  }
  else if (gestureHand == nite::JOINT_RIGHT_HAND)
  {
    handDirection = handsTracker.rightHandDirection;
    if (fabs(mDot(handsTracker.rightHandDirection, GESTURE_VECTOR)) < GESTURE_SIMILARITY_THRESHOLD)
      return false;
  }

  float handY = hand.getPosition().y;

  double currentTime = juce::Time::getMillisecondCounterHiRes();
  double elapsedTime = currentTime - startTime;
  if (gestureStarted)
  { 
    if (elapsedTime >= TIME_OUT)
    {
      gestureStarted = false;
      CallbackManager::tempoGestureEnd();
      return false;
    }
  }

  if (status == OUT_ZONE)
  {
    
    //Am I in the zone and is my hand going down ?
    if (handY >= yBottom && handY <= yTop && handDirection.y < 0)
    {
      status = IN_ZONE;
      if (!gestureStarted)
      {
        CallbackManager::tempoGestureStart();
        startTime = juce::Time::getMillisecondCounterHiRes();
        gestureStarted = true;
      }
      else
      { 
        juce::int32 tmpTempo = static_cast<juce::int32>(60000 / elapsedTime);
        //Gestion des valeurs aberrantes
        if (tmpTempo > 300) 
        {
          gestureStarted = false;
          CallbackManager::tempoGestureEnd();
          return false;
        }

        tempo = tmpTempo;
        startTime = currentTime;
        return true;
      }
    }
  }
  else if (status == IN_ZONE)
  {
    if (handY > yTop || handY < yBottom)
    {
      status = OUT_ZONE;
    }
  }

  return false;
}

bool TempoGesture::calibrateGesture(const nite::Skeleton& skeleton)
{
  const nite::SkeletonJoint& rightHip = skeleton.getJoint(nite::JOINT_RIGHT_HIP);
  const nite::SkeletonJoint& head = skeleton.getJoint(nite::JOINT_HEAD);
  if (rightHip.getPositionConfidence() < 0.5f)
    return false;
  yBottom = rightHip.getPosition().y + (head.getPosition().y - rightHip.getPosition().y) / 3.f;
  yTop = yBottom + 100.f;
  return true;
}
