//#include <JuceAudioThread.h>
#include <JuceScript.h>
#include "console/console.h"

#include "Sequencer.h"

//**********INCLUDE WINDOWS THREAD******
#include <Objbase.h>

IMPLEMENT_CONOBJECT(MidiPlayer);

MidiPlayer::MidiPlayer()
  : fileLoaded(false), trackStarted(false)
{
}

MidiPlayer::~MidiPlayer()
{
  for (unsigned int i = 0; i < sequencer.size(); ++i)
    sequencer[i]->stopThread(300);
}

void MidiPlayer::loadMidiFile(const char* filePath)
{
  juce::File myFile = juce::File::getCurrentWorkingDirectory().getChildFile (filePath);
    
  juce::FileInputStream stream(myFile);
  Con::printf(stream.getStatus().getErrorMessage().getCharPointer());

  if (stream.failedToOpen())
  {
    Con::printf("Impossible d'ouvrir le fichier midi");
    fileLoaded = false;
  }
  else fileLoaded = true;

  // DEBUG CHECK : the file must be correctly loaded in order to continue
  jassert(fileLoaded);

  juce::MidiFile midiFile;
  midiFile.readFrom(stream);
  clock.setTimeFormat(midiFile.getTimeFormat());

  for (unsigned int i = 0; i < midiFile.getNumTracks(); ++i)
  {
    sequences.push_back(juce::MidiMessageSequence(*midiFile.getTrack(i)));
  }

  sequencer.resize(sequences.size());

  for (unsigned int i = 0; i < sequencer.size(); ++i)
  {
    sequencer[i] = new JuceModule::Track(11+i, clock, sequences[i]);
  }
  
}

void MidiPlayer::play()
{
  jassert(fileLoaded);
  if (!fileLoaded)
    return;

  if (!trackStarted)
  {
    for (unsigned int i = 0; i < sequencer.size(); ++i)
      sequencer[i]->startThread();
    trackStarted = true;
  }
  clock.play();
}

void MidiPlayer::pause()
{
 if (!fileLoaded)
    return;
 clock.pause();
}

void MidiPlayer::stop()
{
  if (!fileLoaded)
    return;

  clock.stop();
  for (unsigned int i = 0; i < sequencer.size(); ++i)
  {
    sequencer[i]->stopThread(20);
  }
  trackStarted = false;
}

void MidiPlayer::setTempo(juce::uint32 tempo)
{
  clock.setTempo(tempo);
}

//-------------Torque Script Bridge

DefineEngineMethod(MidiPlayer, loadMidiFile, void, (const char* filePath), , "Load a Midi File. You must call this method before any other on MidiPlayer.")
{
  object->loadMidiFile(filePath);
}

DefineEngineMethod(MidiPlayer, play, void, (),, "Play a MIDI sequence" )
{
  object->play();
}

DefineEngineMethod(MidiPlayer, stop, void, (),, "Stop a MIDI sequence" )
{
  object->stop();
}

DefineEngineMethod(MidiPlayer, pause, void, (),, "Pause a MIDI sequence" )
{
  object->pause();
}

DefineEngineMethod(MidiPlayer, setTempo, void, (unsigned int tempo),, "Set a new tempo for the midi sequence")
{
  object->setTempo(tempo);
}

