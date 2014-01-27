
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
{
}
PlayerTracker::~PlayerTracker()
{
}
void PlayerTracker::init(){
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

float PlayerTracker::getJointPositionX(char* joint){
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
}

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


void PlayerTracker::readNextFrame(){

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
		}
		else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED)
		{	
			Con::printf("Tracked \n");
			/*Con::printf("LEFT \n");
			const nite::SkeletonJoint& head = user.getSkeleton().getJoint(nite::JOINT_LEFT_HAND);
			if (head.getPositionConfidence() > .5)
			Con::printf("%d. (%5.2f, %5.2f, %5.2f)\n", user.getId(), head.getPosition().x, head.getPosition().y, head.getPosition().z);
				
			Con::printf("RIGHT \n");
			const nite::SkeletonJoint& head2 = user.getSkeleton().getJoint(nite::JOINT_RIGHT_HAND);
			if (head2.getPositionConfidence() > .5)
			Con::printf("%d. (%5.2f, %5.2f, %5.2f)\n", user.getId(), head2.getPosition().x, head2.getPosition().y, head2.getPosition().z);
			*/
		}
	}
	
}

//-------------Torque Script Bridge

DefineEngineMethod(PlayerTracker, getLHXP, void, (),, "Get X Position of Left Hand")
{
  object->getJointPositionX("left");
}




