//********INCLUDE TORQUE ENGINE********
#include "core/module.h"
#include "console/console.h"

//********INCLUDE KINECT************
#include "KinectTracker.h"

MODULE_BEGIN( KinectModule )

  MODULE_INIT_AFTER( JuceModule )
  MODULE_SHUTDOWN_BEFORE( JuceModule )

  MODULE_INIT
  {
    Con::printf("START NITE INITIALIZE\n");
	  nite::NiTE::initialize();
  }

  MODULE_SHUTDOWN
  {
    nite::NiTE::shutdown();// On ferme NiTE
  }

MODULE_END;
