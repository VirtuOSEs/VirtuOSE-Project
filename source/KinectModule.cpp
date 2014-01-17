//********INCLUDE TORQUE ENGINE********
#include "core/module.h"
#include "console/console.h"

//********INCLUDE KINECT************
#include <NiTE.h> // Permet d'acc�der � l'ensemble des classes de l'API NiTE
//Inclue aussi un ensemble de fcts facilitant la lisibilit� du code. Nite.h comprend aussi stdio.h (pour printf)

#include <NiteSampleUtilities.h>
#include "KinectTracker.h"
/*
MODULE_BEGIN( KinectModule )

  MODULE_INIT_AFTER( JuceModule )
  MODULE_SHUTDOWN_BEFORE( JuceModule )

  //Handtracker permet de gerer la reconnaissance des gestes, si on veut utiliser les mains, mieux vaut l'utiliser
  nite::HandTracker handTracker; // -> d�clare un handtracker
  KinectTracker* kinectTracker;

  MODULE_INIT
  {
    Con::printf("KINECT ON");
	//La plupart des fct de NiTE retourne un code de statut pour indiquer la r�ussite ou l'�chec de la fonction. le plus souvent, c'est en rapport avec la communication avec le mat�riel qui peut merder.
	//Utiliser ces codes permet de bien debugguer le prog
	

	//nite contient un type d'enum�ration Status qui donne acc�s � toutes les valleurs d'erreurs possibles
	nite::Status niteRc;

	//Initialisation de l'API, sinon �a ne marchera pas
	//A noter que la classe utilis�e est NiTE ce qui est diff�rent du namespace nite

	//Ce code combine la d�claration de Status avec un appel � la fonction d'initialisation, et utilise ces lignes suivantes pour checker le statut
	niteRc = nite::NiTE::initialize(); 
	if (niteRc != nite::STATUS_OK)
	{
		 Con::printf("NiTE initialization failed\n");
	}

	niteRc = handTracker.create();//-> connecte l'handtracker � un objet "device" sous-jacent
	//// On peut optionnellement passer en argument un "device" particulier dans le cas de l'utilisation de plusieurs p�riph�riques d'interaction (C'est OpenNI2 qui g�re �a)

	////On checke le statut 
	if (niteRc != nite::STATUS_OK)
	{
		 Con::printf("Couldn't create user tracker\n");
	}

	handTracker.startGestureDetection(nite::GESTURE_WAVE); // Rep�re quand la main bouge d'un cot� � l'autre
	handTracker.startGestureDetection(nite::GESTURE_CLICK); // Rep�re quand la main approche et recule de la cam�ra
	//Penser � indiquer � l'user ce qu'il doit faire, NiTE ne le fait pas tout seul
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