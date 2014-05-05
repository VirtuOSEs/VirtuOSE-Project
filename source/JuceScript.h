#ifndef JUCE_MODULE_SCRIPT_H
#define JUCE_MODULE_SCRIPT_H

//***********INCLUDE TORQUE ENGINE***************
#include "console/engineAPI.h"

//**********INCLUDE STD LIB*********************
#include <vector>

#include "Sequencer.h"
#include "PlayerTracker.h"
#include "Options.h"

/**
  Bridge between C++ and TorqueScript to 
  play MIDI. This class is accessible through TorqueScript
  (see JuceScript.cpp to see exactly which methods are accessible).
  Early implementation, should evolve a lot.
**/
class Orchestrator : public SimObject
{
public:
  typedef SimObject Parent;
  DECLARE_CONOBJECT( Orchestrator );
  DECLARE_CATEGORY( "Juce Module" );
  DECLARE_DESCRIPTION( "A midi file player." );

  static void initPersistFields();

  Orchestrator();
  Orchestrator(const Options*);
  ~Orchestrator();

  void setSaveFile(const char* filePath);
  void loadMidiFile(const char* filePath);
  int getNumTracks();
  String getInstrumentName(int index);
  String getTrackName(int index);

  void play();
  void pause();
  void stop();

  void increaseVelocityFactor(short percentage);
  void decreaseVelocityFactor(short percentage);

  int getTempo() const;
  void setTempo(juce::uint32 tempo);
  void saveSequence(const char* filePath);

private:
  JuceModule::Sequencer::Ptr sequencer;
  juce::ScopedPointer<PlayerTracker> playerTracker;

  std::vector<JuceModule::Track::Ptr > tracks;
  Options defaultOptions;
  const Options* options;
  juce::String saveFileName;
};


#endif // JUCE_MODULE_SCRIPT_H