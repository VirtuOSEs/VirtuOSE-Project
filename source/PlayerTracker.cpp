
#include "console/console.h"
#include <stdio.h>
#include <string.h>
#include <NiteSampleUtilities.h>
#include "PlayerTracker.h"

IMPLEMENT_CONOBJECT(PlayerTracker);

#define USER_MESSAGE(msg) \
	{Con::printf("[%08llu] User #%d:\t%s\n",ts, user.getId(),msg);}

#define MAX_USERS 10
bool g_visibleUsers[MAX_USERS] = {false};
nite::SkeletonState g_skeletonStates[MAX_USERS] = {nite::SKELETON_NONE};



// HandsMove static constants definition
TSStatic* HandsMove::rightHandSphere = nullptr;
TSStatic* HandsMove::leftHandSphere = nullptr;
SpawnSphere* HandsMove::spawn = nullptr;
Point3F HandsMove::eyePosition = Point3F();
const float HandsMove::EYE_OFFSET = 2.5f;

bool PlayerTracker::KINECT_DETECTED = false;

PlayerTracker::PlayerTracker()
{
}

PlayerTracker::PlayerTracker(JuceModule::Sequencer::Ptr sequencer)
  : sequencer(sequencer)
{
  if (!KINECT_DETECTED)
  {
    return;
  }

  niteRc = userTracker.create();
	
  if (niteRc != nite::STATUS_OK)
  {
  Con::printf("Couldn't create user tracker\n");
  exit(1);
  }
  userTracker.addNewFrameListener(this);
}

PlayerTracker::~PlayerTracker()
{
  if (KINECT_DETECTED) 
    userTracker.removeNewFrameListener(this);
}

void PlayerTracker::updateUserState(const nite::UserData& user, unsigned long long ts)
{
	if (user.isNew())
		USER_MESSAGE("New")
	else if (user.isVisible() && !g_visibleUsers[user.getId()])
		USER_MESSAGE("Visible")
	else if (!user.isVisible() && g_visibleUsers[user.getId()])
		USER_MESSAGE("Out of Scene")
	else if (user.isLost())
		USER_MESSAGE("Lost")

	g_visibleUsers[user.getId()] = user.isVisible();


	if(g_skeletonStates[user.getId()] != user.getSkeleton().getState())
	{
		switch(g_skeletonStates[user.getId()] = user.getSkeleton().getState())
		{
		case nite::SKELETON_NONE:
			USER_MESSAGE("Stopped tracking.")
			break;
		case nite::SKELETON_CALIBRATING:
			USER_MESSAGE("Calibrating...")
			break;
		case nite::SKELETON_TRACKED:
			USER_MESSAGE("Tracking!")
			break;
		case nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE:
		case nite::SKELETON_CALIBRATION_ERROR_HANDS:
		case nite::SKELETON_CALIBRATION_ERROR_LEGS:
		case nite::SKELETON_CALIBRATION_ERROR_HEAD:
		case nite::SKELETON_CALIBRATION_ERROR_TORSO:
			USER_MESSAGE("Calibration Failed... :-|")
			break;
		}
	}
}

void PlayerTracker::onNewFrame(nite::UserTracker& userTracker)
{
    //ONLY FOR Y COORDINATES
  float t_p; // Torso Position
  float lh_p; // Left Hand Position
  float rh_p; // Right Hand Position
  userTracker.readFrame(&userTrackerFrame);
  if (niteRc != nite::STATUS_OK)
  {
    Con::printf("Get next frame failed\n");
    return;
  }
  const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
  for (int i = 0; i < users.getSize(); ++i) {

    const nite::UserData& user = users[i];
    updateUserState(user,userTrackerFrame.getTimestamp());
    if (user.isNew())
    {
      userTracker.startSkeletonTracking(user.getId());
      userTracker.setSkeletonSmoothingFactor(.85f);
    }
    else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED)
    {        
      const nite::SkeletonJoint& lh = user.getSkeleton().getJoint(nite::JOINT_LEFT_HAND);
      if (lh.getPositionConfidence() > .5)
        lh_p=lh.getPosition().y;


      const nite::SkeletonJoint& torso = user.getSkeleton().getJoint(nite::JOINT_TORSO);
      if (torso.getPositionConfidence() > .5)
        t_p=torso.getPosition().y;

      const nite::SkeletonJoint& rh = user.getSkeleton().getJoint(nite::JOINT_RIGHT_HAND);
      if (rh.getPositionConfidence() > .5)
        rh_p=rh.getPosition().y;

      //Detect velocity changes
      if (velocityGesture.checkVelocityGesture(user.getSkeleton()))
      {
        sequencer->setVelocityAbsolute(velocityGesture.getVelocityDetected());
      }

      //Detect tempo changes
      if (tempoGesture.checkTempoGesture(user.getSkeleton()))
      {
        sequencer->setTempo(tempoGesture.getTempo());
      }

      //Send hands position to game (display hands as spheres)
      const nite::SkeletonJoint& head = user.getSkeleton().getJoint(nite::JOINT_HEAD);
      if (rh.getPositionConfidence() > 0.65 && head.getPositionConfidence() > 0.65)
      {
        float rhandX = (rh.getPosition().x - head.getPosition().x) / 150.f;
        float rhandY = (rh.getPosition().y - head.getPosition().y) / 150.f;
        float rhandZ = (rh.getPosition().z - head.getPosition().z) / 150.f;

        float lhandX = (lh.getPosition().x - head.getPosition().x) / 150.f;
        float lhandY = (lh.getPosition().y - head.getPosition().y) / 150.f;
        float lhandZ = (lh.getPosition().z - head.getPosition().z) / 150.f;

        ThreadPool::queueWorkItemOnMainThread(new HandsMove(Point3F(lhandX, lhandY, lhandZ), Point3F(rhandX, rhandY, rhandZ)));
      }
    }
  } 
}


//-------------Torque Script Bridge

DefineEngineMethod(PlayerTracker, getLHXP, void, (),, "Get X Position of Left Hand")
{
  //object->getJointPositionX("left");
}




