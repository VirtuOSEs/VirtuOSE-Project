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
#include "Sequencer.h"
#include "TempoGesture.h"
#include "VelocityGesture.h"

class PlayerTracker : public SimObject, public nite::UserTracker::NewFrameListener
{
public:
	typedef SimObject Parent;
  
  //Torque stuff
  DECLARE_CONOBJECT( PlayerTracker );
	DECLARE_CATEGORY( "Kinect Module" );
 	DECLARE_DESCRIPTION( "A user tracker using Kinect." );

  static bool KINECT_DETECTED;
  
  //Should not be called by a programmer. 
  //Exists only because Torque ConObject have to have a default constructor
  PlayerTracker();

 	PlayerTracker(JuceModule::Sequencer::Ptr sequencer);
 	~PlayerTracker();

  //from nite::UserTracker::NewFrameListener
  virtual void onNewFrame(nite::UserTracker& userTracker);

 	void updateUserState(const nite::UserData& user, unsigned long long ts);
 	void readNextFrame();


private:
  JuceModule::Sequencer::Ptr sequencer;
	nite::UserTracker userTracker;
	nite::Status niteRc;
	nite::UserTrackerFrameRef userTrackerFrame;
	int velocityTest;
  TempoGesture tempoGesture;
  VelocityGesture velocityGesture;
  float velocity;
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

#endif // KINECT_MODULE_SCRIPT_H