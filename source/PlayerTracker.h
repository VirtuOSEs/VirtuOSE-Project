#ifndef KINECT_MODULE_SCRIPT_H
#define KINECT_MODULE_SCRIPT_H

//***********INCLUDE TORQUE ENGINE***************
#include "console/engineAPI.h"

#include "T3D/tsStatic.h"
#include "T3D/missionMarker.h"
#include "math/mTransform.h"
#include "platform/threads/ThreadPool.h"

#include <vector>
#include "NiTE.h"

#include "JuceLibraryCode/JuceHeader.h"
class TempoGesture
{
public:
  enum GestureStatus{NO_GESTURE, IN_ZONE, OUT_ZONE};

  TempoGesture()
    : status(NO_GESTURE),
      gestureCalibrated(false),
      startTime(0.),
      tempo(92),
      yBottom(0.f),
      yTop(150.f),
      timeOut(3500)
  {}

  void calibrateGesture(const nite::SkeletonJoint& rightHip)
  {
    if (rightHip.getPositionConfidence() < 0.5f)
      return;
    yBottom = rightHip.getPosition().y;
    yTop = yBottom + 200.f;
    gestureCalibrated = true;
    Con::printf("Tempo gesture : yBottom : %f, yTop : %f", yBottom, yTop);
  }

  juce::int32 getTempo() const
    {return tempo;}

  bool checkTempoGesture(const nite::Skeleton& skeleton)
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
      }
      else if (juce::Time::getMillisecondCounterHiRes() - startTime >= timeOut)
      {
       // Con::printf("TIMEOUT %f", handY);
        status = NO_GESTURE;
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

private:
  GestureStatus status;
  bool gestureCalibrated;
  double startTime;
  juce::int32 tempo;
  //---ZONE DELIMITERS
  float yBottom;
  float yTop;
  float timeOut;
};


/** WorkItem qui modifie la position des mains.
    Envoyée au main thread par PlayerTracker
    **/
class HandsMove : public ThreadPool::WorkItem
{
public:
  HandsMove(Point3F leftHand, Point3F rightHand)
    : leftHand(leftHand), rightHand(rightHand)
  {}

  virtual void execute()
  {
    //Mini optim pour ne faire le test qu'une fois
    if (rightHandSphere == nullptr)
      rightHandSphere = dynamic_cast<TSStatic* > (Sim::findObject("rightHand"));
    if (leftHandSphere == nullptr)
      leftHandSphere = dynamic_cast<TSStatic* > (Sim::findObject("leftHand"));
    if (spawn == nullptr)
    {
      spawn = dynamic_cast<SpawnSphere* > (Sim::findObject("Spawn"));
      MatrixF eyeMatrix;
      spawn->getEyeTransform(&eyeMatrix);
      eyePosition = eyeMatrix.getPosition();
    }
    if (rightHandSphere != nullptr && spawn != nullptr && leftHandSphere != nullptr)
    {
      //inversion de y et z entre Kinect et Torque, le rHandZ est aussi inversé
      MatrixF rhandTransform; rhandTransform.identity();
      rhandTransform.setPosition(Point3F(eyePosition.x + rightHand.x, eyePosition.y - rightHand.z, eyePosition.z + rightHand.y + EYE_OFFSET));
      MatrixF lhandTransform; lhandTransform.identity();
      lhandTransform.setPosition(Point3F(eyePosition.x + leftHand.x, eyePosition.y - leftHand.z, eyePosition.z + leftHand.y + EYE_OFFSET));
      rightHandSphere->setTransform(rhandTransform);
      leftHandSphere->setTransform(lhandTransform);
    }
  }

private:
  Point3F leftHand;
  Point3F rightHand;
  static TSStatic* rightHandSphere;
  static TSStatic* leftHandSphere;
  static SpawnSphere* spawn;
  static Point3F eyePosition;
  static const float EYE_OFFSET;
};


class PlayerTracker : public SimObject, public nite::UserTracker::NewFrameListener
{
public:
	typedef SimObject Parent;
  	DECLARE_CONOBJECT( PlayerTracker );
	DECLARE_CATEGORY( "Kinect Module" );
 	DECLARE_DESCRIPTION( "A user tracker using Kinect." );

 	PlayerTracker();
 	~PlayerTracker();

  //from nite::UserTracker::NewFrameListener
  virtual void onNewFrame(nite::UserTracker& userTracker);

 	void updateUserState(const nite::UserData& user, unsigned long long ts);
 	void readNextFrame();
 	void init();
	void VelocityHandChecker(float hand, float torso);
	void VelocityHandChecker2(float hand, float torso);
	int getVelocityTest();
  bool hasTempoChanged() const;
  void resetTempoChangedFlag();
  juce::int32 getTempo() const;
  float getVelocity();
  void setVelocity(float v);

private:
	nite::UserTracker userTracker;
	nite::Status niteRc;
	nite::UserTrackerFrameRef userTrackerFrame;
	int velocityTest;
  TempoGesture tempoGesture;
  juce::CriticalSection tempoChangedAccess;
  bool tempoChanged;
  float velocity;
};

#endif // KINECT_MODULE_SCRIPT_H