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
	{}

	void run()
	{
		UT.init();
		UT.startDetection();
	}

private:
	PlayerTracker UT;
};

#endif // KINECT_MODULE_TRACKER_H