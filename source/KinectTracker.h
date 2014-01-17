#ifndef KINECT_MODULE_TRACKER_H
#define KINECT_MODULE_TRACKER_H

#include "console/console.h"
#include "JuceLibraryCode\JuceHeader.h"
#include "NiTE.h"
#include "NiteSampleUtilities.h"

class KinectTracker : public juce::Thread
{
public:
	explicit KinectTracker(nite::HandTracker& handTracker)
		: juce::Thread("KinectThread"), handTracker(handTracker)
	{}

	void run()
	{
		//All hand points and gestures detected in a given frame are encapsulated in HandTrackerFrameRef objects
		nite::HandTrackerFrameRef handTrackerFrame;
		nite::Status niteRc;
		for(;;)
		{
			//niteRc re�oit le status du tracker de la frame du tracker
			niteRc = handTracker.readFrame(&handTrackerFrame);
			if (niteRc != nite::STATUS_OK)
			{
				Con::printf("Get next frame failed\n");
				continue;
			}

			//A chaque frame, on va regarder si une "gestures" a �t� d�tect�
			const nite::Array<nite::GestureData>& gestures = handTrackerFrame.getGestures();//On r�cup�re la liste de toutes les "gestures" d�tect�es dans la frame
			for (int i = 0; i < gestures.getSize(); ++i) //On cherche dans la liste des "gestures" celles qui on �t� effectu�es
			{
				if (gestures[i].isComplete())// Si la "gesture" a �t� effectu�e dans son int�gralit�
				{
					nite::HandId newId;
					handTracker.startHandTracking(gestures[i].getCurrentPosition(), &newId);//On fournit la position dans l'espace au handtracker et on affecte un Id � une main unique
				}
			}

			/************UTILISATION DES GESTURES******************************************************

			Using gestures like this is optional. If your application can know the location of hand positions from 
			some other source (for example, a kiosk application might show a user where to hold his hands), 
			then you can initialize directly without a gesture. It is also possible to write your own computer vision 
			algorithms to locate the position of the hands. For the majority of applications, though, using the 
			gestures described above is the easiest way to initialize hand tracking.

			****************************************************************************************/
		
			//getHands r�cup�re la liste de toutes les mains qui sont track�es
			const nite::Array<nite::HandData>& hands = handTrackerFrame.getHands();
			for (int i = 0; i < hands.getSize(); ++i)
			{
				const nite::HandData& hand = hands[i]; // On cr�e un HandData qui permet d'exploiter les donn�es d'une main dans la liste des mains rep�r�es
				if (hand.isTracking())// Si la main est suivie/track�e
				{
					Con::printf("%d. (%5.2f, %5.2f, %5.2f)\n", hand.getId(), hand.getPosition().x, hand.getPosition().y, hand.getPosition().z);
				}
			}
		}
	}

private:
	nite::HandTracker& handTracker;
};

#endif // KINECT_MODULE_TRACKER_H