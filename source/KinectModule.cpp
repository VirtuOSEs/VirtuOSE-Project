//********INCLUDE TORQUE ENGINE********
#include "core/module.h"
#include "console/console.h"

//********INCLUDE KINECT************
#include "KinectTracker.h"

MODULE_BEGIN( KinectModule )

  MODULE_INIT_AFTER( JuceModule )
  MODULE_SHUTDOWN_BEFORE( JuceModule )

  KinectTracker* kinectTracker;

  MODULE_INIT
  {
    Con::printf("START NITE INITIALIZE\n");
	  nite::NiTE::initialize();
    kinectTracker = new KinectTracker();
  	kinectTracker->startThread();
  }

  MODULE_SHUTDOWN
  {
    kinectTracker->stopThread(100);
    delete kinectTracker;
    nite::NiTE::shutdown();// On ferme NiTE
  }

MODULE_END;
