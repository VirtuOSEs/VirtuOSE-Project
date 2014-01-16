LISEZ MOI
===========
Faire le git clone dans Torque3D-master\My Projects
Torque3D-master peut s'appeler autrement en fonction de comment vous avez configuré votre truc...

VST SDK
===
Vous devez également télécharger le VST SDK v3.0 sur le site de Steinberg. Le répertoire est là mais je ne l'ai pas partagé pour des raisons de droits (et puis il est super gros).

Oculus SDK
====
L'Oculus SDK doit être à côté de Torque Master. Il est nécessaire de modifier le TorqueEngine pour que ça fonctionne (légère incompatibilité de version):
source/platform/input/oculusVR/oculusVRSensorData.h : ligne 59
<pre>
   void setData(OVR::SensorFusion& data, const F32& maxAxisRadius);//Il faut enlever le const devant la variable data
</pre>
Répercutez le changement dans le fichier .cpp d'implémentation.

Asio SDK
=====
L'ASIO SDK ainsi que les drivers ASIO sont nécessaires. L'ASIO SDK doit être placé dans le répertoire déjà prévu à cet effet dans source/. Vous devez copier-coller les fichiers dans ce dossier afin de respecter la hierarchie (pour vous repérer sur quoi copier et comment il y a déjà un fichier dans ASIO SDK)
En gros sous Windows la sortie audio est quasiment jamais dispo pour ASIO. Il y a une manip pour s'assurer qu'elle l'est  indiquée ici : http://fr.audiofanzine.com/logiciel-musique-divers/michael-tippach/Asio4All-%5BFreeware%5D/pedago/astuces/asio4all-vista-seven-interface-indisponible-5970/. 
Vous aurez normalement au démarrage de VirtuOSE une petite bulle en bas disant "Coucou ASIO$$" (en gros). Si déjà vous avez ça c'est bien. Si vous n'avez pas de bip ni de piano, regardez dans le "panel ASIO" dispo dans la barre des tâches, si votre output est dans l'état indisponible. Si c'est le cas, faut faire la manip du lien.

Config diverse
===
Le VirtuOSE.sln est dans buildFiles/VisualStudio 2010

Pour l'instant le VST utilisé est 4Front Piano. Vous pouvez le trouver facilement sur internet. Il suffit de placer la dll à la racine du projet GitHub et de la renommer 4Front_Piano.dll.

Pour réussir à lire des VST (instruments virtuels) correctement sous Windows, il vous faut les bons drivers :
Téléchargez les drivers ASIO4all et installez-les. Quand vous lancez VirtuOSE vous devez entendre un BIP, puis du piano, une pause, et de nouveau du piano (ça va changer bien sûr).

Pour l'Oculus Rift, il faut le placer à côté de TorqueMaster (ou Torque3D). A la compilation, une erreur vous petera à la gueule et il faut changer le code de l'oculus. Aller dans le .cpp et dans le .h du fichier qui pose problème
et enlevez le const du premier paramètre. Ca devrait résoudre les problèmes.
