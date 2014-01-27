#include "KinectTracker.h"

namespace KinectModule
{

bool KinectTracker::kinectEnabled = true;

KinectTracker::KinectTracker(JuceModule::Sequencer& sequencer)
	: juce::Thread("KinectThread"), sequencer(sequencer)
{
  if (kinectEnabled)
    UT.init();
}

void KinectTracker::run()
{
  if (!kinectEnabled)
     return;
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

} // namespace KinectModule