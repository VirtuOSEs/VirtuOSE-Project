#include "core/module.h"
#include "console/console.h"
#include <JuceScript.h>


MODULE_BEGIN( JuceModule )

  MODULE_INIT_AFTER( Sim )
  MODULE_SHUTDOWN_BEFORE( Sim )
  ThreadSafeRef<JuceModule::AudioMidiWorkItem> sequencer;
  MODULE_INIT
  {
    Con::printf("JUCE ON");

    //Cr�� le thread (en fait un ensemble de Thread mais ici normalement il n'en contient qu'un)
    //qui va g�rer audio+midi avec JUCE
    JuceModule::AudioMidiThreadPool::createSingleton();

    //L'item est un objet qui contient la m�thode execute() qui va �tre appel�e par le ThreadPool
    sequencer = new JuceModule::AudioMidiWorkItem(10);

    //Ajoute l'item � la file d'attente du ThreadPool pour qu'il soit trait�
    JuceModule::AudioMidiThreadPool::instance()->queueWorkItem(sequencer);
    JuceModule::AudioMidiThreadPool::instance()->flushWorkItems();//je sais pas encore trop pourquoi faut mettre �a
  }

  MODULE_SHUTDOWN
  {
    // Cleanup code...
    sequencer->stop();//On stop le s�quenceur midi
    JuceModule::AudioMidiThreadPool::instance()->shutdown();
    JuceModule::AudioMidiThreadPool::instance()->deleteSingleton();
  }

MODULE_END;