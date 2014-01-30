#include "KinectTracker.h"

namespace KinectModule
{

bool KinectTracker::kinectEnabled = true;

KinectTracker::KinectTracker(JuceModule::Sequencer::Ptr sequencer)
	: juce::Thread("KinectThread"), sequencer(sequencer)
{
  if (kinectEnabled)
  {
    UT.init();
  }
}

void KinectTracker::run()
{
  if (!kinectEnabled)
     return;
  UT.setVelocity(0.5f);
  Con::printf("START NITE DECTECTION\n");
	Con::printf("\nStart moving around to GET detected...\n(PSI pose may be required for skeleton calibration, depending on the configuration)\n");
	for(;;)
	{
    wait(200);
    if (threadShouldExit())
      return;
		//UT.readNextFrame();

		sequencer->setVelocityAbsolute(UT.getVelocity());
    if (UT.hasTempoChanged())
    {
      //until next tempo event, UT.hasTempoChanged will return false
      UT.resetTempoChangedFlag();
      //Con::printf("Tempo changed : new Tempo %d", UT.getTempo());
      sequencer->setTempo(UT.getTempo());
    }
  }
}

float KinectTracker::getHandX()
{
  return 0.f;
}

} // namespace KinectModule