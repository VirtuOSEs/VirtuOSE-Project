#include "core/module.h"
#include "console/console.h"
#include <JuceAudioThread.h>

#include "JuceLibraryCode/JuceHeader.h"

MODULE_BEGIN( JuceModule )

  MODULE_INIT_AFTER( Sim )
  MODULE_SHUTDOWN_BEFORE( Sim )
  MODULE_INIT
  {
    Con::printf("JUCE ON");

    //Créé le thread (en fait un ensemble de Thread mais ici normalement il n'en contient qu'un)
    //qui va gérer audio+midi avec JUCE
    JuceModule::AudioMidiThreadPool::createSingleton();
    JuceModule::AudioTools::initialize();
  }

  MODULE_SHUTDOWN
  {
    // Cleanup code...
    JuceModule::AudioMidiThreadPool::instance()->shutdown();
    JuceModule::AudioMidiThreadPool::instance()->deleteSingleton();
    JuceModule::AudioTools::deleteInstance();
  }

MODULE_END;