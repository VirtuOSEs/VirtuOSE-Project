#ifndef JUCE_MODULE_SCRIPT_H
#define JUCE_MODULE_SCRIPT_H

#include "console/engineAPI.h"

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
  ThreadSafeRef<JuceModule::AudioMidiWorkItem> sequencer;
};


#endif // JUCE_MODULE_SCRIPT_H