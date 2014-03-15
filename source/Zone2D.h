#ifndef KINECT_ZONE_2D
#define KINECT_ZONE_2D

struct Zone2D
{
  float bottom;
  float top;
  float left;
  float right;

  //From the perspective of the user, x increase from left to right
  bool isCoordinateInZone(float xPosition, float yPosition) const
  {
    bool verticallyIn = yPosition > bottom && yPosition < top;
    bool horizontallyIn = xPosition > left && xPosition < right;
    return verticallyIn && horizontallyIn;
  }

  float length() const
  {
    return fabs(top - bottom);
  }

  float width() const
  {
    return fabs(left - right);
  }
};

#endif // KINECT_ZONE_2D
