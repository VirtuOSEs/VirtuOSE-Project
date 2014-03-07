#include "TempoGesture.h"

IMPLEMENT_GLOBAL_CALLBACK(onTempoGestureStart, void, (), (),
	"Called when the user starts a tempo gesture.\n"
);

IMPLEMENT_GLOBAL_CALLBACK(onTempoGestureEnd, void, (), (),
	"Called when the user ends a tempo gesture.\n"

);

// TempoGesture implem
TempoGesture::TempoGesture()
  : status(NO_GESTURE),
    gestureCalibrated(false),
    startTime(0.),
    tempo(92),
    yBottom(0.f),
    yTop(150.f),
    timeOut(3500)
{}

bool TempoGesture::checkTempoGesture(const nite::Skeleton& skeleton)
{
  const nite::SkeletonJoint& rightHand = skeleton.getJoint(nite::JOINT_RIGHT_HAND);
  const nite::SkeletonJoint& rightHip = skeleton.getJoint(nite::JOINT_RIGHT_HIP);

  if (!gestureCalibrated)
    calibrateGesture(rightHip);

  if (rightHand.getPositionConfidence() < 0.5)
    return false;

  float handY = rightHand.getPosition().y;

  //Si on était pas rentré dans la zone une première fois...
  if (status == NO_GESTURE)
  {
    //Suis-je dans la zone ?
    if (handY >= yBottom && handY <= yTop)
    {
      // Con::printf("DANS ZONE : DEBUT MOUVEMENT %f", handY);
      status = IN_ZONE;
      startTime = juce::Time::getMillisecondCounterHiRes();
      onTempoGestureStart_callback();
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
      onTempoGestureEnd_callback();
    }
    else if (juce::Time::getMillisecondCounterHiRes() - startTime >= timeOut)
    {
      // Con::printf("TIMEOUT %f", handY);
      status = NO_GESTURE;
      onTempoGestureEnd_callback();
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
        
      //Gestion des valeurs aberrantes
      if (elapsedTime < 288) //Equivaut à peu près à tempo = 208 (on bat à la blanche)
      {
      //  Con::printf("VALEUR ABERRANTE: PASSAGE EN NO GESTURE %f", handY);
        status = NO_GESTURE;
        return false;
      }
      tempo = static_cast<juce::int32>(120000 / elapsedTime);//On bat à la blanche
      startTime = currentTime;

      onTempoGestureEnd_callback();

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

void TempoGesture::calibrateGesture(const nite::SkeletonJoint& rightHip)
{
  if (rightHip.getPositionConfidence() < 0.5f)
    return;
  yBottom = rightHip.getPosition().y;
  yTop = yBottom + 200.f;
  gestureCalibrated = true;
  Con::printf("Tempo gesture : yBottom : %f, yTop : %f", yBottom, yTop);
}
