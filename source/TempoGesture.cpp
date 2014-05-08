#include "TempoGesture.h"
#include "TSCallback.h"

const float TempoGesture::GESTURE_WIDTH_PERCENTAGE = 50.f/100.f;
const VectorF TempoGesture::GESTURE_VECTOR = VectorF(0, 1, 0);
const float TempoGesture::GESTURE_SIMILARITY_THRESHOLD = 0.95f;

TempoGesture::TempoGesture(const Options& options)
  : status(NO_GESTURE),
    startTime(0.),
    tempo(options.initialTempo),
    yBottom(0.f),
    yTop(150.f),
    timeOut(3500)
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

  if (status == NO_GESTURE)
  {
    //Am I in the zone and is my hand raising up ?
    if (handY >= yBottom && handY <= yTop && handDirection.y > 0)
    {
      // Con::printf("DANS ZONE : DEBUT MOUVEMENT %f", handY);
      status = IN_ZONE;
      startTime = juce::Time::getMillisecondCounterHiRes();
      CallbackManager::tempoGestureStart();
    }
  }
  else if (status == IN_ZONE)
  {
    //Did I cross the zone meaning the gesture go next step ?
    if (handY > yTop)
    {
      // Con::printf("HORS ZONE : SUITE MOUVEMENT %f", handY);
      status = OUT_ZONE;
    }
    //Leaving the zone by bottom means cancelling the gesture
    else if (handY < yBottom)
    {
      // Con::printf("SORTIE PAR LE BAS : NO GESTURE %f", handY);
      status = NO_GESTURE;
      CallbackManager::tempoGestureEnd();
    }
    else if (juce::Time::getMillisecondCounterHiRes() - startTime >= timeOut)
    {
      // Con::printf("TIMEOUT %f", handY);
      status = NO_GESTURE;
      CallbackManager::tempoGestureEnd();
    }
  }
  //If I were outzone and now I am again in zone: I just finished a tempo gesture!
  else if (status == OUT_ZONE)
  {
    if (handY >= yBottom && handY <= yTop)
    {
    //  Con::printf("DANS ZONE FIN MOUVEMENT %f", handY);
      double currentTime = juce::Time::getMillisecondCounterHiRes();
      double elapsedTime = currentTime - startTime;
        
      juce::int32 tmpTempo = static_cast<juce::int32>(60000 / elapsedTime);
      //Gestion des valeurs aberrantes
      if (tmpTempo > 300) 
      {
      //  Con::printf("VALEUR ABERRANTE: PASSAGE EN NO GESTURE %f", handY);
        status = NO_GESTURE;
        return false;
      }

      tempo = tmpTempo;
      startTime = currentTime;

      CallbackManager::tempoGestureEnd();

      status = NO_GESTURE;
      return true;
    }
    //Gesture timeout
    else if (juce::Time::getMillisecondCounterHiRes() - startTime >= timeOut)
    {
    //  Con::printf("TIMEOUT %f", handY);
      status = NO_GESTURE;
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
