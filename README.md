LISEZ MOI
===========
Faire le git clone dans Torque3D-master\My Projects
Torque3D-master peut s'appeler autrement en fonction de comment vous avez configuré votre truc...
Vous devez également télécharger le VST SDK v3.0 sur le site de Steinberg. Le répertoire est là mais je ne l'ai pas partagé pour des raisons de droits (et puis il est super gros).

Oculus SDK
L'Oculus SDK doit être à côté de Torque Master. Il est nécessaire de modifier le TorqueEngine pour que ça fonctionne (légère incompatibilité de version):
source/platform/input/oculusVR/oculusVRSensorData.h : ligne 59
<pre>
   void setData(OVR::SensorFusion& data, const F32& maxAxisRadius);//Il faut enlever le const devant la variable data
</pre>
Répercutez le changement dans le fichier .cpp d'implémentation.

Asio SDK
L'ASIO SDK ainsi que les drivers ASIO sont nécessaires. L'ASIO SDK doit être placé dans le répertoire déjà prévu à cet effet dans source/. Vous devez copier-coller les fichiers dans ce dossier afin de respecter la hierarchie (pour vous repérer sur quoi copier et comment il y a déjà un fichier dans ASIO SDK)

Le VirtuOSE.sln est dans buildFiles/VisualStudio 2010

Pour l'instant le VST utilisé est 4Front Piano. Vous pouvez le trouver facilement sur internet. Il suffit de placer la dll à la racine du projet GitHub et de la renommer 4Front_Piano.dll.

Pour réussir à lire des VST (instruments virtuels) correctement sous Windows, il vous faut les bons drivers :
Téléchargez les drivers ASIO4all et installez-les. Quand vous lancez VirtuOSE vous devez entendre un BIP, puis du piano, une pause, et de nouveau du piano (ça va changer bien sûr).
