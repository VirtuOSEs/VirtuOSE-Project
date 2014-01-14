LISEZ MOI
===========
Faire le git clone dans Torque3D-master\My Projects
Torque3D-master peut s'appeler autrement en fonction de comment vous avez configuré votre truc...
Vous devez également télécharger le VST SDK v3.0 sur le site de Steinberg. Le répertoire est là mais je ne l'ai pas partagé pour des raisons de droits (et puis il est super gros).
Idem, je pense pas qu'on puisse partager l'Oculus SDK. Du coup faudra documenter où il faut le mettre dans la structure du projet (ça peut être en dehors de l'arborescence couverte par GitHub c'est pas gênant).

Le VirtuOSE.sln est dans buildFiles/VisualStudio 2010

Pour l'instant le VST utilisé est 4Front Piano. Vous pouvez le trouver facilement sur internet. Il suffit de placer la dll à la racine du projet GitHub et de la renommer 4Front_Piano.dll.

Pour réussir à lire des VST (instruments virtuels) correctement sous Windows, il vous faut les bons drivers :
Téléchargez les drivers ASIO4all et installez-les. Quand vous lancez VirtuOSE vous devez entendre un BIP, puis du piano, une pause, et de nouveau du piano (ça va changer bien sûr).
