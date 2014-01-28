#ifndef KINECT_MODULE_SCRIPT_H
#define KINECT_MODULE_SCRIPT_H

//***********INCLUDE TORQUE ENGINE***************
#include "console/engineAPI.h"
#include <vector>
#include "NiTE.h"


class PlayerTracker : public SimObject 
{
public:
	typedef SimObject Parent;
  	DECLARE_CONOBJECT( PlayerTracker );
	DECLARE_CATEGORY( "Kinect Module" );
 	DECLARE_DESCRIPTION( "A user tracker using Kinect." );

 	PlayerTracker();
 	~PlayerTracker();

 	void updateUserState(const nite::UserData& user, unsigned long long ts);
 	void readNextFrame();
 	void init();
	void VelocityHandChecker(float hand, float torso);
	void VelocityHandChecker2(float hand, float torso);
	int getVelocityTest();
 	//float getJointPositionX(char* joint);
 	//float getJointPositionY(char* joint);
 	//float getJointPositionZ(char* joint);

private:
	nite::UserTracker userTracker;
	nite::Status niteRc;
	nite::UserTrackerFrameRef userTrackerFrame;
	int velocityTest;
};

#endif // KINECT_MODULE_SCRIPT_H