#ifndef JUCE_MODULE_SCRIPT_H
#define JUCE_MODULE_SCRIPT_H

//***********INCLUDE TORQUE ENGINE***************
#include "console/engineAPI.h"

//**********INCLUDE STD LIB*********************
#include <vector>

#include "Sequencer.h"

/**
  Bridge between C++ and TorqueScript to 
  play MIDI. This class is accessible through TorqueScript
  (see JuceScript.cpp to see exactly which methods are accessible).
  Early implementation, should evolve a lot.
**/
class MidiPlayer : public SimObject
{
public:
  typedef SimObject Parent;
  DECLARE_CONOBJECT( MidiPlayer );
  DECLARE_CATEGORY( "Juce Module" );
  DECLARE_DESCRIPTION( "A midi file player." );

  MidiPlayer();
  ~MidiPlayer();

  void playMidi();
  void stopMidi();

private:
  std::vector<ThreadSafeRef<JuceModule::Track>> sequencer;
  std::vector<juce::MidiMessageSequence> sequences;
};


#endif // JUCE_MODULE_SCRIPT_H