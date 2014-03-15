#ifndef KINECT_TRANSPORT_GESTURE
#define KINECT_TRANSPORT_GESTURE

#include "JuceLibraryCode/JuceHeader.h"
#include "NiTE.h"
#include "math/mPoint3.h"

/**
  * Description of the gesture :
  * Hands together in the same small area near torso : it's PLAY or PAUSE
  * Hands together in the small area near torso more than STOP_GESTURE_TIME millis : it's a STOP
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

  bool checkTransportGesture(const nite::Skeleton& skeleton);

  const TransportStatus& getTransportStatus() const
    {return status;}

private:
  enum ZoneStatus {IN_ZONE, OUT_ZONE};

  void tryToCalibrateGesture(const nite::Skeleton& skeleton);

  TransportStatus status;
  VectorF zoneCenter;
  bool gestureCalibrated;
  juce::int64 firstTimeZoneEntered;
  ZoneStatus zoneStatus;

};

#endif //KINECT_TRANSPORT_GESTURE
