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

    //Créé le thread (en fait un ensemble de Thread mais ici normalement il n'en contient qu'un)
    //qui va gérer audio+midi avec JUCE
    JuceModule::AudioMidiThreadPool::createSingleton();

    //L'item est un objet qui contient la méthode execute() qui va être appelée par le ThreadPool
    sequencer = new JuceModule::AudioMidiWorkItem(10);

    //Ajoute l'item à la file d'attente du ThreadPool pour qu'il soit traité
    JuceModule::AudioMidiThreadPool::instance()->queueWorkItem(sequencer);
    JuceModule::AudioMidiThreadPool::instance()->flushWorkItems();//je sais pas encore trop pourquoi faut mettre ça
  }

  MODULE_SHUTDOWN
  {
    // Cleanup code...
    sequencer->stop();//On stop le séquenceur midi
    JuceModule::AudioMidiThreadPool::instance()->shutdown();
    JuceModule::AudioMidiThreadPool::instance()->deleteSingleton();
  }

MODULE_END;