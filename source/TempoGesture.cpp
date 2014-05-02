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
  const nite::SkeletonJoint& rightHip = skeleton.getJoint(nite::JOINT_RIGHT_HIP);
  
  if (!calibrateGesture(rightHip))
    return false;

   //Detect if current movement is compatible with this gesture
  if (gestureHand == nite::JOINT_LEFT_HAND)
  {
    if (fabs(mDot(handsTracker.leftHandDirection, GESTURE_VECTOR)) < GESTURE_SIMILARITY_THRESHOLD)
      return false;
  }
  else if (gestureHand == nite::JOINT_RIGHT_HAND)
  {
    if (fabs(mDot(handsTracker.rightHandDirection, GESTURE_VECTOR)) < GESTURE_SIMILARITY_THRESHOLD)
      return false;
  }

  float handY = hand.getPosition().y;

  //Si on était pas rentré dans la zone une première fois...
  if (status == NO_GESTURE)
  {
    //Suis-je dans la zone ?
    if (handY >= yBottom && handY <= yTop)
    {
      // Con::printf("DANS ZONE : DEBUT MOUVEMENT %f", handY);
      status = IN_ZONE;
      startTime = juce::Time::getMillisecondCounterHiRes();
      CallbackManager::tempoGestureStart();
    }
  }
  //Si au dernier check on était dans la zone, on regarde si on en est sorti...
  else if (status == IN_ZONE)
  {
    //Suis-je sorti de la zone ? On considère qu'on ne sort que "par le haut"
    if (handY > yTop)
    {
      // Con::printf("HORS ZONE : SUITE MOUVEMENT %f", handY);
      status = OUT_ZONE;
    }
    //Si on sort par le bas : annulation du geste
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
  //Si au dernier check on était hors de la zone et qu'on rentre à nouveau dedans : nouveau tempo
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

      //Pour simplifier pour l'instant on ne peut pas enchainer
      status = NO_GESTURE;
      return true;
    }
    else if (juce::Time::getMillisecondCounterHiRes() - startTime >= timeOut)
    {
    //  Con::printf("TIMEOUT %f", handY);
      status = NO_GESTURE;
    }
  }
  return false;
}

bool TempoGesture::calibrateGesture(const nite::SkeletonJoint& rightHip)
{
  if (rightHip.getPositionConfidence() < 0.5f)
    return false;
  yBottom = rightHip.getPosition().y;
  yTop = yBottom + 200.f;
}
