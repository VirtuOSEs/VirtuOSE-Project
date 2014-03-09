#include "console/console.h"
#include <stdio.h>
#include <string.h>
#include <NiteSampleUtilities.h>
#include "PlayerTracker.h"

#define USER_MESSAGE(msg) \
	{Con::printf("[%08llu] User #%d:\t%s\n",ts, user.getId(),msg);}

#define MAX_USERS 10
bool g_visibleUsers[MAX_USERS] = {false};
nite::SkeletonState g_skeletonStates[MAX_USERS] = {nite::SKELETON_NONE};

IMPLEMENT_GLOBAL_CALLBACK(onTempoJustChanged, void, (int newTempo), (newTempo),
	"Called when the the midi sequence tempo is actually changed.\n"
);

IMPLEMENT_GLOBAL_CALLBACK(onVelocityChanged, void, (float newVelocity), (newVelocity),
	"Called when the user changes the velocity.\n"
);


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
  : sequencer(sequencer), musicalGestureDetectionActivated(false)
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

void PlayerTracker::activateMusicalGestureDetection()
{
  musicalGestureDetectionActivated = true;
}

void PlayerTracker::deactivateMusicalGestureDetection()
{
  musicalGestureDetectionActivated = false;
}

void PlayerTracker::onNewFrame(nite::UserTracker& userTracker)
{
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
      const nite::SkeletonJoint& rh = user.getSkeleton().getJoint(nite::JOINT_RIGHT_HAND);

      //Detect velocity changes
      if (musicalGestureDetectionActivated && velocityGesture.checkVelocityGesture(user.getSkeleton()))
      {
        sequencer->setVelocityAbsolute(velocityGesture.getVelocityDetected());
        onVelocityChanged_callback(velocityGesture.getVelocityDetected());
      }

      //Detect tempo changes
      if (musicalGestureDetectionActivated && tempoGesture.checkTempoGesture(user.getSkeleton()))
      {
        sequencer->setTempo(tempoGesture.getTempo());
        onTempoJustChanged_callback(tempoGesture.getTempo());
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


