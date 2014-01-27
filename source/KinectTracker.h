#ifndef KINECT_MODULE_TRACKER_H
#define KINECT_MODULE_TRACKER_H

#include "console/console.h"
#include "JuceLibraryCode\JuceHeader.h"
#include "PlayerTracker.h"

class KinectTracker : public juce::Thread
{
public:
	explicit KinectTracker()
		: juce::Thread("KinectThread")
	{
    UT.init();
  }

	void run()
	{
    Con::printf("START NITE DECTECTION\n");
	  Con::printf("\nStart moving around to GET detected...\n(PSI pose may be required for skeleton calibration, depending on the configuration)\n");
	  for(;;)
	  {
		  UT.readNextFrame();
      wait(10);
      if (threadShouldExit())
        return;
    }
	}

private:
	PlayerTracker UT;
};

#endif // KINECT_MODULE_TRACKER_H