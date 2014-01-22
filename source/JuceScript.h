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

  void loadMidiFile(const char* filePath);
  void play();
  void pause();
  void unpause();
  void stop();

  void setTempo(juce::uint32 tempo);
  void saveSequence(const char* filePath);

private:
  JuceModule::Sequencer* sequencer;
  std::vector<JuceModule::Track::Ptr > tracks;
  std::vector<juce::MidiMessageSequence> sequences;
};


#endif // JUCE_MODULE_SCRIPT_H