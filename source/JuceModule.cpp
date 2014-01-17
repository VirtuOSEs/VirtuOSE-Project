//********INCLUDE TORQUE ENGINE********
#include "core/module.h"
#include "console/console.h"

//********INCLUDE JUCE************
#include "JuceLibraryCode/JuceHeader.h"

#include "Sequencer.h"

MODULE_BEGIN( JuceModule )

  MODULE_INIT_AFTER( Sim )
  MODULE_SHUTDOWN_BEFORE( Sim )
  MODULE_INIT
  {
    Con::printf("JUCE ON");
    //Initiliaze the object holding audio device and VST
    JuceModule::AudioTools::initialize();
  }

  MODULE_SHUTDOWN
  {
    JuceModule::AudioTools::deleteInstance();
  }

MODULE_END;