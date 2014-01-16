//********INCLUDE TORQUE ENGINE********
#include "core/module.h"
#include "console/console.h"

//********INCLUDE JUCE************
#include "JuceLibraryCode/JuceHeader.h"

#include "JuceAudioThread.h"
#include "Sequencer.h"

MODULE_BEGIN( JuceModule )

  MODULE_INIT_AFTER( Sim )
  MODULE_SHUTDOWN_BEFORE( Sim )
  MODULE_INIT
  {
    Con::printf("JUCE ON");

    //Cr�� le thread (en fait un ensemble de Thread mais ici normalement il n'en contient qu'un)
    //qui va g�rer audio+midi avec JUCE
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