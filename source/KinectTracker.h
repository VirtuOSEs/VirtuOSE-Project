#ifndef KINECT_MODULE_TRACKER_H
#define KINECT_MODULE_TRACKER_H

#include "console/console.h"
#include "JuceLibraryCode\JuceHeader.h"
#include "PlayerTracker.h"
#include "Sequencer.h"

namespace KinectModule
{

class KinectTracker : public juce::Thread
{
public:
  //Helper function to allow devs without Kinect to..dev !
  static bool kinectEnabled;
  static void enableKinect()
    {kinectEnabled = true;}
  static void disableKinect()
    {kinectEnabled = false;}

	explicit KinectTracker(JuceModule::Sequencer& sequencer);
	void run();

private:
	PlayerTracker UT;
  JuceModule::Sequencer& sequencer;
};

} //namespace KinectModule

#endif // KINECT_MODULE_TRACKER_H