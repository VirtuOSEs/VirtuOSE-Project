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
    //NITE seems to not detect that there is no device present.
    //So we talk directly to OpenNI and initialize NITE only if a device has been found
    openni::Array< openni::DeviceInfo >  deviceInfoList;
    openni::OpenNI::initialize();
    openni::OpenNI::enumerateDevices(&deviceInfoList );

    if (deviceInfoList.getSize() > 0)
    {
      KinectModule::KinectTracker::enableKinect();
      nite::NiTE::initialize();
      Con::printf("OpenNI Device found");
    }
    else
    {
      Con::errorf("Unable to find an OpenNI Device (Kinect)");
      KinectModule::KinectTracker::disableKinect();
    }
  }

  MODULE_SHUTDOWN
  {
    Con::printf("NITE SHUTDOWN");
    nite::NiTE::shutdown();// On ferme NiTE
  }

MODULE_END;
