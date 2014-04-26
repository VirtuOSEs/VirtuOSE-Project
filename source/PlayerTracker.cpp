#include "console/console.h"
#include <stdio.h>
#include <string.h>
#include <NiteSampleUtilities.h>
#include "PlayerTracker.h"
#include "TSCallback.h"
#include "platform/platform.h"

#define USER_MESSAGE(msg) \
  {Platform::outputDebugString("[%08llu] User #%d:\t%s\n",ts, user.getId(),msg);}

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
  : tempoGesture(Options()),
    expressionGesture(Options())
{
}

PlayerTracker::PlayerTracker(JuceModule::Sequencer::Ptr sequencer, const Options& options)
  : sequencer(sequencer),
    tempoGesture(options),
    expressionGesture(options),
    musicalGestureDetectionActivated(false)
{
  if (!KINECT_DETECTED)
  {
    return;
  }

  niteRc = userTracker.create();
	
  if (niteRc != nite::STATUS_OK)
  {
  Platform::outputDebugString("Couldn't create user tracker\n");
  exit(1);
  }
  userTracker.addNewFrameListener(this);
}

PlayerTracker::~PlayerTracker()
{
  const juce::ScopedLock sL(trackerAccess);
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
  {
    const juce::ScopedLock sL(trackerAccess);
    userTracker.readFrame(&userTrackerFrame);
  }
  if (niteRc != nite::STATUS_OK)
  {
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
      //If hands positions have not enough confidence, this frame is skipped
      if (! handsTracker.update(user.getSkeleton()))
        return;

      if (transportGesture.checkTransportGesture(handsTracker))
      {
        if (transportGesture.getTransportStatus() == TransportGesture::PLAY)
        {
          activateMusicalGestureDetection();
          sequencer->play();
          Platform::outputDebugString("PLAY");
        }
        else if (transportGesture.getTransportStatus() == TransportGesture::PAUSE)
        {
          deactivateMusicalGestureDetection();
          sequencer->pause();
          Platform::outputDebugString("PAUSE");
        }
        else if (transportGesture.getTransportStatus() == TransportGesture::STOP)
        {
          deactivateMusicalGestureDetection();
          sequencer->stop();
          Platform::outputDebugString("STOP");
        }
      }

      //Detect expression changes
      if (musicalGestureDetectionActivated && expressionGesture.checkExpressionGesture(handsTracker, user.getSkeleton()))
      {
        sequencer->setExpression(expressionGesture.getExpressionDetected());
        CallbackManager::expressionChanged(expressionGesture.getExpressionDetected());
        
      }

      //Detect tempo changes
      if (musicalGestureDetectionActivated && tempoGesture.checkTempoGesture(handsTracker, user.getSkeleton()))
      {
        sequencer->setTempo(tempoGesture.getTempo());
        CallbackManager::tempoJustChanged(tempoGesture.getTempo());
      }

      //Send hands position to game (display hands as spheres)

      Sim::postEvent(Sim::getRootGroup(), new HandsMove(handsTracker.torqueCoordinatesLeftHand, handsTracker.torqueCoordinatesRightHand), -1);
     
    }
  } 
}


