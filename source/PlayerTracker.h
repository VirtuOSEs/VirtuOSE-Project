#ifndef KINECT_MODULE_SCRIPT_H
#define KINECT_MODULE_SCRIPT_H

//***********INCLUDE TORQUE ENGINE***************
#include "console/engineAPI.h"
#include "console/simEvents.h"

#include "T3D/tsStatic.h"
#include "T3D/missionMarker.h"
#include "math/mTransform.h"
#include "platform/threads/ThreadPool.h"

#include <vector>
#include "NiTE.h"

#include "JuceLibraryCode/JuceHeader.h"
#include "Sequencer.h"
#include "TempoGesture.h"
#include "ExpressionGesture.h"
#include "TransportGesture.h"
#include "Options.h"
#include "HandsTracker.h"


class PlayerTracker : public nite::UserTracker::NewFrameListener
{
public:
  static bool KINECT_DETECTED;
  
  //Should not be called by a programmer. 
  //Exists only because Torque ConObject must have a default constructor
  PlayerTracker();

 	PlayerTracker(JuceModule::Sequencer::Ptr sequencer, const Options& options);
 	~PlayerTracker();

  //from nite::UserTracker::NewFrameListener
  virtual void onNewFrame(nite::UserTracker& userTracker);
  void activateMusicalGestureDetection();
  void deactivateMusicalGestureDetection();

 	void updateUserState(const nite::UserData& user, unsigned long long ts);

private:
  JuceModule::Sequencer::Ptr sequencer;
	nite::UserTracker userTracker;
	nite::Status niteRc;
	nite::UserTrackerFrameRef userTrackerFrame;
  HandsTracker handsTracker;

  TempoGesture tempoGesture;
  ExpressionGesture expressionGesture;
  TransportGesture transportGesture;

  float velocity;
  bool musicalGestureDetectionActivated;
  juce::CriticalSection trackerAccess;
};

/** SimEvent modifying hands representation position.
    Sended to main thread by PlayerTracker
    **/
class HandsMove : public SimEvent
{
public:
  HandsMove(Point3F leftHand, Point3F rightHand)
    : leftHand(leftHand), rightHand(rightHand)
  {}

  virtual void process(SimObject *object)
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