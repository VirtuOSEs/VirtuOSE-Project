
#include "console/console.h"
#include <stdio.h>
#include <string.h>
#include <NiteSampleUtilities.h>
#include "PlayerTracker.h"

IMPLEMENT_CONOBJECT(PlayerTracker);

#define MAX_USERS 10
bool g_visibleUsers[MAX_USERS] = {false};
nite::SkeletonState g_skeletonStates[MAX_USERS] = {nite::SKELETON_NONE};

#define USER_MESSAGE(msg) \
	{Con::printf("[%08llu] User #%d:\t%s\n",ts, user.getId(),msg);}


PlayerTracker::PlayerTracker()
  : tempoChanged(false)
{
}
PlayerTracker::~PlayerTracker()
{
}

void PlayerTracker::init(){
	Con::printf("START NITE INITIALIZE\n");
	nite::NiTE::initialize();

	niteRc = userTracker.create();
	
	if (niteRc != nite::STATUS_OK)
	{
		Con::printf("Couldn't create user tracker\n");
		exit(1);
	}
	Con::printf("END NITE INITIALIZE\n");
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

int PlayerTracker::getVelocityTest()
{
	return velocityTest;
}

void PlayerTracker::VelocityHandChecker(float hand, float torso){
	float position=hand;
	float epsilon=100;
	float max_y=400;
	float min_y=-400;

	if((position-torso>=max_y-epsilon)&&(position-torso<=max_y+epsilon))
		velocityTest=1;
	else if((position+torso>=min_y-epsilon)&&(position+torso<=min_y+epsilon))
		velocityTest=-1;
	else
		velocityTest=0;
}

void VelocityHandChecker2(float hand, float torso){
	float position=hand;
	float epsilon=100;
	float zero = torso;
	float max_y=450-torso;
	float min_y=-450+torso;
}

/*float PlayerTracker::getJointPositionX(char* joint){
	const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
	for (int i = 0; i < users.getSize(); ++i) {
	
		const nite::UserData& user = users[i];
		updateUserState(user,userTrackerFrame.getTimestamp());
		if (user.isNew())
			{
				userTracker.startSkeletonTracking(user.getId());
			}

			else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED)
			{
				if(strcmp(joint,"left")==0){
					const nite::SkeletonJoint& head = user.getSkeleton().getJoint(nite::JOINT_LEFT_HAND);
					if (head.getPositionConfidence() > .5)
						return head.getPosition().x;
					else
						return head.getPosition().x;
				}	

				if(strcmp(joint,"right")==0){
					const nite::SkeletonJoint& head = user.getSkeleton().getJoint(nite::JOINT_RIGHT_HAND);
						if (head.getPositionConfidence() > .5)
						return head.getPosition().x;
					else
						return head.getPosition().x;
				}
			}
	}
}*/

/*float  PlayerTracker::getJointPositionY(char* joint){
	const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
	const nite::UserData& user = users[0];
	if(strcmp(joint,"left")==0){
		const nite::SkeletonJoint& head = user.getSkeleton().getJoint(nite::JOINT_LEFT_HAND);
		if (head.getPositionConfidence() > .5)
			return head.getPosition().y;
		else
			return head.getPosition().y;
	}	

	if(strcmp(joint,"right")==0){
		const nite::SkeletonJoint& head = user.getSkeleton().getJoint(nite::JOINT_RIGHT_HAND);
			if (head.getPositionConfidence() > .5)
			return head.getPosition().y;
		else
			return head.getPosition().y;
	}
}

float  PlayerTracker::getJointPositionZ(char* joint){
	const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
	const nite::UserData& user = users[0];
	updateUserState(user,userTrackerFrame.getTimestamp());
	if(strcmp(joint,"left")==0){
		const nite::SkeletonJoint& head = user.getSkeleton().getJoint(nite::JOINT_LEFT_HAND);
		if (head.getPositionConfidence() > .5)
			return head.getPosition().z;
		else
			return head.getPosition().z;
	}	

	if(strcmp(joint,"right")==0){

		const nite::SkeletonJoint& head = user.getSkeleton().getJoint(nite::JOINT_RIGHT_HAND);
			if (head.getPositionConfidence() > .5)
			return head.getPosition().z;
		else
			return head.getPosition().z;
	}
}*/

#include "T3D/tsStatic.h"
#include "T3D/missionMarker.h"
#include "math/mTransform.h"
void PlayerTracker::readNextFrame(){

  //ONLY FOR Y COORDINATES
  int t_p; // Torso Position
  int lh_p; // Left Hand Position
  int rh_p; // Right Hand Position

  niteRc = userTracker.readFrame(&userTrackerFrame);
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

      VelocityHandChecker(lh_p,t_p);

      if (tempoGesture.checkTempoGesture(user.getSkeleton()))
        tempoChanged = true;
      else
        tempoChanged = false;
     
      if (rh.getPositionConfidence() > 0.5 && torso.getPositionConfidence() > 0.5)
      {
        float rhandX = (rh.getPosition().x - torso.getPosition().x) / 100.f;
        float rhandY = (rh.getPosition().y - torso.getPosition().y) / 100.f;
        float rhandZ = (rh.getPosition().z - torso.getPosition().z) / 100.f;

        float lhandX = (lh.getPosition().x - torso.getPosition().x) / 100.f;
        float lhandY = (lh.getPosition().y - torso.getPosition().y) / 100.f;
        float lhandZ = (lh.getPosition().z - torso.getPosition().z) / 100.f;

        TSStatic* rightHandSphere = dynamic_cast<TSStatic* > (Sim::findObject("rightHand"));
        TSStatic* leftHandSphere = dynamic_cast<TSStatic* > (Sim::findObject("leftHand"));
        SpawnSphere* spawn = dynamic_cast<SpawnSphere* > (Sim::findObject("Spawn"));
        if (rightHandSphere != nullptr && spawn != nullptr && leftHandSphere != nullptr)
        {
          MatrixF eyeMatrix;
          spawn->getEyeTransform(&eyeMatrix);
          Point3F eyePosition = eyeMatrix.getPosition();
          //inversion de y et z entre Kinect et Torque, le rHandZ est aussi inversé
          TransformF rhandTransform(Point3F(eyePosition.x + rhandX, eyePosition.y - rhandZ, eyePosition.z + rhandY), AngAxisF());
          TransformF lhandTransform(Point3F(eyePosition.x + lhandX, eyePosition.y - lhandZ, eyePosition.z + lhandY), AngAxisF());
          rightHandSphere->setTransform(rhandTransform.getMatrix());
          leftHandSphere->setTransform(lhandTransform.getMatrix());
        }
      }

    }
  } 

}

bool PlayerTracker::hasTempoChanged() const
{
  return tempoChanged;
}
juce::int32 PlayerTracker::getTempo() const
{
  return tempoGesture.getTempo();
}


//-------------Torque Script Bridge

DefineEngineMethod(PlayerTracker, getLHXP, void, (),, "Get X Position of Left Hand")
{
  //object->getJointPositionX("left");
}




