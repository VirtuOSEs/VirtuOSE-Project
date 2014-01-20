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

Kinect : Kinect SDK + OpenNI2 + NiTE
====
Vous devez récuperer :
   <ul>
      <li>Le Kinect SDK officiel, disponible sur le site de Microsoft</li>
      <li>L'OpenNI SDK, version 2, disponible sans inscription sur le site de PrimeSense</li>
      <li>Le NiTE SDK, disponible avec inscription sur le site de PrimeSese</li>
   </ul>
   
Vous receverez par mail une archive contenant la plupart des .exe d'installations.

   <ol>
      <li>En premier lieu, installez le Kinect SDK.</li>
      <li>Ensuite, installez OpenNI. Par défaut l'installation se fera dans le dossier Program Files/OpenNI2 (32 bits).</li>
      <li>De la même façon, installez NiTE. Par défaut l'installation se fera dans le dossier Program Files/PrimeSense/NiTE2 (32 bits).</li>
   </ol>
   
   <strong>Il faut ensuite copier le contenu des dossiers (y compris le dossier source) de Program Files/PrimeSense/NiTE2/Redist et Program Files/OpenNI2/Redist et coller le tout à la racine du dossier game de VirtuOSE. <br/>
   Il faut copier le fichier NiteSampleUtilities.h depuis Program Files /PrimeSense/ NiTE2/ Samples/SimpleHandTracker et le coller dans rogram Files /PrimeSense/NiTE2/Include</strong>
   
Dernières Étapes
   <ul>
   <li>Ajoutez ../../../source/OpenNI2/Include et ../../../source/NiTE2/Include dans les include du projet VirtuOSE DLL</li>
   <li>Copiez les dossiers Program Files/PrimseSense/NiTe2 et Program Files/OpenNI2 dans le dossier source de VirtuOSE</li>
   </ul>
   
Asio SDK
=====
L'ASIO SDK ainsi que les drivers ASIO sont nécessaires. L'ASIO SDK doit être placé dans le répertoire déjà prévu à cet effet dans source/. Vous devez copier-coller les fichiers dans ce dossier afin de respecter la hierarchie (pour vous repérer sur quoi copier et comment il y a déjà un fichier dans ASIO SDK)
En gros sous Windows la sortie audio est quasiment jamais dispo pour ASIO. Il y a une manip pour s'assurer qu'elle l'est  indiquée ici : http://fr.audiofanzine.com/logiciel-musique-divers/michael-tippach/Asio4All-%5BFreeware%5D/pedago/astuces/asio4all-vista-seven-interface-indisponible-5970/. 
Vous aurez normalement au démarrage de VirtuOSE une petite bulle en bas disant "Coucou ASIO$$" (en gros). Si déjà vous avez ça c'est bien. Si vous n'avez pas de bip ni de piano, regardez dans le "panel ASIO" dispo dans la barre des tâches, si votre output est dans l'état indisponible. Si c'est le cas, faut faire la manip du lien.

Config diverse
===
Le VirtuOSE.sln est dans buildFiles/VisualStudio 2010

Pour l'instant le VST utilisé est sfz qu'on trouve <a href="http://www.cakewalk.com/Download/sfz.aspx">ici</a>. Il suffit de placer la dll à la racine du projet GitHub. Il vous faut aussi le <a href="http://sso.mattiaswestlund.net/"> Sonatina Symphonic Orchestra</a>. Config de Sonatina à venir.

Pour réussir à lire des VST (instruments virtuels) correctement sous Windows, il vous faut les bons drivers :
Téléchargez les drivers ASIO4all et installez-les. Quand vous lancez VirtuOSE vous devez entendre un BIP, puis du piano, une pause, et de nouveau du piano (ça va changer bien sûr).

Pour l'Oculus Rift, il faut le placer à côté de TorqueMaster (ou Torque3D). A la compilation, une erreur vous petera à la gueule et il faut changer le code de l'oculus. Aller dans le .cpp et dans le .h du fichier qui pose problème
et enlevez le const du premier paramètre. Ca devrait résoudre les problèmes.
