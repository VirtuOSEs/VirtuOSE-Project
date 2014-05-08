#ifndef KINECT_TRANSPORT_GESTURE
#define KINECT_TRANSPORT_GESTURE

#include "JuceLibraryCode/JuceHeader.h"
#include "math/mPoint3.h"
#include "HandsTracker.h"
#include "Zone2D.h"

/**
  * Description of the gesture :
  * Hands together: it's PLAY or PAUSE
  * Hands together more than STOP_GESTURE_TIME millis: it's a STOP
  * @see GESTURE_ZONE_RADIUS_MM
  * @see STOP_GESTURE_TIME
*/
class TransportGesture
{
public:
  /**
   * Zone radius in millimeters
  */
  static const float GESTURE_ZONE_RADIUS_MM;
  static const juce::int64 STOP_GESTURE_TIME_MS;
  
  enum TransportStatus {PLAY, PAUSE, STOP};

  TransportGesture();

  bool checkTransportGesture(const HandsTracker& handsTracker, const nite::Skeleton& skeleton);

  const TransportStatus& getTransportStatus() const
    {return status;}

private:
  bool calibrateGesture(const nite::Skeleton& skeleton);
  enum ZoneStatus {IN_ZONE, OUT_ZONE};

  TransportStatus status;
  juce::int64 firstTimeZoneEntered;
  ZoneStatus zoneStatus;
  Zone2D gestureZone;

  //Light optimisation
  VectorF distanceBetweenHands;
};

#endif //KINECT_TRANSPORT_GESTURE
