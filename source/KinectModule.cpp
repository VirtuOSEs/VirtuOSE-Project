//********INCLUDE TORQUE ENGINE********
#include "core/module.h"
#include "console/console.h"

//********INCLUDE KINECT************
#include <NiTE.h> // Permet d'accéder à l'ensemble des classes de l'API NiTE
//Inclue aussi un ensemble de fcts facilitant la lisibilité du code. Nite.h comprend aussi stdio.h (pour printf)

#include <NiteSampleUtilities.h>
#include "KinectTracker.h"
/*
MODULE_BEGIN( KinectModule )

  MODULE_INIT_AFTER( JuceModule )
  MODULE_SHUTDOWN_BEFORE( JuceModule )

  //Handtracker permet de gerer la reconnaissance des gestes, si on veut utiliser les mains, mieux vaut l'utiliser
  nite::HandTracker handTracker; // -> déclare un handtracker
  KinectTracker* kinectTracker;

  MODULE_INIT
  {
    Con::printf("KINECT ON");
	//La plupart des fct de NiTE retourne un code de statut pour indiquer la réussite ou l'échec de la fonction. le plus souvent, c'est en rapport avec la communication avec le matériel qui peut merder.
	//Utiliser ces codes permet de bien debugguer le prog
	

	//nite contient un type d'enumération Status qui donne accès à toutes les valleurs d'erreurs possibles
	nite::Status niteRc;

	//Initialisation de l'API, sinon ça ne marchera pas
	//A noter que la classe utilisée est NiTE ce qui est différent du namespace nite

	//Ce code combine la déclaration de Status avec un appel à la fonction d'initialisation, et utilise ces lignes suivantes pour checker le statut
	niteRc = nite::NiTE::initialize(); 
	if (niteRc != nite::STATUS_OK)
	{
		 Con::printf("NiTE initialization failed\n");
	}

	niteRc = handTracker.create();//-> connecte l'handtracker à un objet "device" sous-jacent
	//// On peut optionnellement passer en argument un "device" particulier dans le cas de l'utilisation de plusieurs périphériques d'interaction (C'est OpenNI2 qui gère ça)

	////On checke le statut 
	if (niteRc != nite::STATUS_OK)
	{
		 Con::printf("Couldn't create user tracker\n");
	}

	handTracker.startGestureDetection(nite::GESTURE_WAVE); // Repère quand la main bouge d'un coté à l'autre
	handTracker.startGestureDetection(nite::GESTURE_CLICK); // Repère quand la main approche et recule de la caméra
	//Penser à indiquer à l'user ce qu'il doit faire, NiTE ne le fait pas tout seul
	Con::printf("\nWave or click to start tracking your hand...\n");
	kinectTracker = new KinectTracker(handTracker);
	kinectTracker->startThread();

	
  }

  MODULE_SHUTDOWN
  {
	  delete kinectTracker;
    nite::NiTE::shutdown();// On ferme NiTE
  }

MODULE_END;
*/