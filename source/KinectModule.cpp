//********INCLUDE TORQUE ENGINE********
#include "core/module.h"
#include "console/console.h"

//********INCLUDE KINECT************
#include "NiTE.h"

#include "PlayerTracker.h"

MODULE_BEGIN( KinectModule )

  MODULE_INIT_AFTER( JuceModule )
  MODULE_SHUTDOWN_BEFORE( JuceModule )

  MODULE_INIT
  {
    Platform::outputDebugString("START NITE INITIALIZE\n");
    //NITE seems to not detect that there is no device present.
    //So we talk directly to OpenNI and initialize NITE only if a device has been found
    openni::Array< openni::DeviceInfo >  deviceInfoList;
    openni::OpenNI::initialize();
    openni::OpenNI::enumerateDevices(&deviceInfoList );

    if (deviceInfoList.getSize() > 0)
    {
      nite::NiTE::initialize();
      Platform::outputDebugString("OpenNI Device found");
      PlayerTracker::KINECT_DETECTED = true;
    }
    else
    {
      Platform::outputDebugString("Unable to find an OpenNI Device (Kinect)");
      PlayerTracker::KINECT_DETECTED = false;
    }
  }

  MODULE_SHUTDOWN
  {
    Platform::outputDebugString("NITE SHUTDOWN");
    nite::NiTE::shutdown();// On ferme NiTE
  }

MODULE_END;
