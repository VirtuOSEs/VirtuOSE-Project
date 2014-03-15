#ifndef KINECT_TRANSPORT_GESTURE
#define KINECT_TRANSPORT_GESTURE

#include "JuceLibraryCode/JuceHeader.h"
#include "NiTE.h"
#include "math/mPoint3.h"

class TransportGesture
{
public:
  /**
   * Zone radius in millimeters
  */
  static const float GESTURE_ZONE_RADIUS_MM;
  static const juce::int64 TIME_TRESHOLD_BETWEEN_TWO_GESTURES_MS;
  
  enum TransportStatus {PLAY, PAUSE, STOP};

  TransportGesture();

  bool checkTransportGesture(const nite::Skeleton& skeleton);

  const TransportStatus& getTransportStatus() const
    {return status;}

private:
  void tryToCalibrateGesture(const nite::Skeleton& skeleton);

  TransportStatus status;
  VectorF zoneCenter;
  bool gestureCalibrated;
  juce::int64 timer;

};

#endif //KINECT_TRANSPORT_GESTURE
