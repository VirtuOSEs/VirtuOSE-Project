#ifndef KINECT_MODULE_SCRIPT_H
#define KINECT_MODULE_SCRIPT_H

//***********INCLUDE TORQUE ENGINE***************
#include "console/engineAPI.h"
#include <vector>
#include "NiTE.h"

#include "JuceLibraryCode/JuceHeader.h"
class TempoGesture
{
public:
  enum GestureStatus{NO_GESTURE, IN_ZONE, OUT_ZONE};

  TempoGesture()
    : status(NO_GESTURE),
      gestureCalibrated(false),
      startTime(0.),
      tempo(92),
      yBottom(0.f),
      yTop(150.f),
      timeOut(3000)
  {}

  void calibrateGesture(const nite::SkeletonJoint& torso)
  {
    yBottom = torso.getPosition().y - 160.f;//30cm en dessous du torse
    yTop = yBottom + 150.f;
    gestureCalibrated = true;
    Con::printf("yBottom : %f, yTop : %f", yBottom, yTop);
  }

  juce::int32 getTempo() const
    {return tempo;}

  bool checkTempoGesture(const nite::SkeletonJoint& torso, const nite::SkeletonJoint& rightHand)
  {
    if (!gestureCalibrated)
      calibrateGesture(torso);

    if (rightHand.getPositionConfidence() < 0.5)
      return false;

    float handY = rightHand.getPosition().y;

    //Si on était pas rentré dans la zone une première fois...
    if (status == NO_GESTURE)
    {
      //Suis-je dans la zone ?
      if (handY >= yBottom && handY <= yTop)
      {
        Con::printf("DANS ZONE : DEBUT MOUVEMENT");
        status = IN_ZONE;
        startTime = juce::Time::getMillisecondCounterHiRes();
      }
    }
    //Si au dernier check on était dans la zone, on regarde si on en est sorti...
    else if (status == IN_ZONE)
    {
      //Suis-je sorti de la zone ? On considère qu'on ne sort que "par le haut"
      if (handY > yTop)
      {
        Con::printf("HORS ZONE : SUITE MOUVEMENT");
        status = OUT_ZONE;
      }
      else if (juce::Time::getMillisecondCounterHiRes() - startTime >= timeOut)
      {
        Con::printf("TIMEOUT");
        status = NO_GESTURE;
      }
    }
    //Si au dernier check on était hors de la zone et qu'on rentre à nouveau dedans : nouveau tempo
    else if (status == OUT_ZONE)
    {
      if (handY >= yBottom && handY <= yTop)
      {
        Con::printf("DANS ZONE FIN MOUVEMENT");
        double currentTime = juce::Time::getMillisecondCounterHiRes();
        double elapsedTime = currentTime - startTime;
        tempo = static_cast<juce::int32>(60000 / elapsedTime);
        startTime = currentTime;

        //Pour simplifier pour l'instant on ne peut pas enchainer
        status = NO_GESTURE;
        return true;
      }
      else if (juce::Time::getMillisecondCounterHiRes() - startTime >= timeOut)
      {
        Con::printf("TIMEOUT");
        status = NO_GESTURE;
      }
    }
    return false;
  }

private:
  GestureStatus status;
  bool gestureCalibrated;
  double startTime;
  juce::int32 tempo;
  //---ZONE DELIMITERS
  float yBottom;
  float yTop;
  float timeOut;
};

class PlayerTracker : public SimObject 
{
public:
	typedef SimObject Parent;
  	DECLARE_CONOBJECT( PlayerTracker );
	DECLARE_CATEGORY( "Kinect Module" );
 	DECLARE_DESCRIPTION( "A user tracker using Kinect." );

 	PlayerTracker();
 	~PlayerTracker();

 	void updateUserState(const nite::UserData& user, unsigned long long ts);
 	void readNextFrame();
 	void init();
	void VelocityHandChecker(float hand, float torso);
	void VelocityHandChecker2(float hand, float torso);
	int getVelocityTest();
  bool hasTempoChanged() const;
  juce::int32 getTempo() const;

 	//float getJointPositionX(char* joint);
 	//float getJointPositionY(char* joint);
 	//float getJointPositionZ(char* joint);

private:
	nite::UserTracker userTracker;
	nite::Status niteRc;
	nite::UserTrackerFrameRef userTrackerFrame;
	int velocityTest;
  TempoGesture tempoGesture;
  bool tempoChanged;
};

#endif // KINECT_MODULE_SCRIPT_H