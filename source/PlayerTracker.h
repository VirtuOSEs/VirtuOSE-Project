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
 	void startDetection();
 	void init();
 	float getJointPositionX(char* joint);
 	//float getJointPositionY(char* joint);
 	//float getJointPositionZ(char* joint);

private:
	nite::UserTracker userTracker;
	nite::Status niteRc;
	nite::UserTrackerFrameRef userTrackerFrame;
};

#endif // KINECT_MODULE_SCRIPT_H