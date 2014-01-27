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
    //kinectTracker = new KinectTracker();
  	//kinectTracker->startThread();
  }

  MODULE_SHUTDOWN
  {
    //nite::NiTE::shutdown();// On ferme NiTE
    //delete kinectTracker;
  
  }

MODULE_END;
