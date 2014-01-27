#ifndef JUCE_MODULE_SCRIPT_H
#define JUCE_MODULE_SCRIPT_H

//***********INCLUDE TORQUE ENGINE***************
#include "console/engineAPI.h"

//**********INCLUDE STD LIB*********************
#include <vector>

#include "KinectTracker.h"
#include "Sequencer.h"

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

  Orchestrator();
  ~Orchestrator();

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
  juce::ScopedPointer<JuceModule::Sequencer> sequencer;
  juce::ScopedPointer<KinectModule::KinectTracker> kinectTracker;

  std::vector<JuceModule::Track::Ptr > tracks;
  std::vector<juce::MidiMessageSequence> sequences;
};


#endif // JUCE_MODULE_SCRIPT_H