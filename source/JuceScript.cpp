//#include <JuceAudioThread.h>
#include <JuceScript.h>
#include "console/console.h"

#include "Sequencer.h"

//**********INCLUDE WINDOWS THREAD******
#include <Objbase.h>

IMPLEMENT_CONOBJECT(MidiPlayer);

MidiPlayer::MidiPlayer()
{
}

MidiPlayer::~MidiPlayer()
{
  delete sequencer;
}

void MidiPlayer::loadMidiFile(const char* filePath)
{
  juce::File myFile = juce::File::getCurrentWorkingDirectory().getChildFile (filePath);
    
  juce::FileInputStream stream(myFile);
  Con::printf(stream.getStatus().getErrorMessage().getCharPointer());

  if (stream.failedToOpen())
  {
    Con::printf("Impossible d'ouvrir le fichier midi");
    // DEBUG CHECK : the file must be correctly loaded in order to continue
    jassert(false);
  }

  juce::MidiFile midiFile;
  midiFile.readFrom(stream);

  juce::MidiMessageSequence tempoTrack;
  for (unsigned int i = 0; i < midiFile.getNumTracks(); ++i)
  {
    if (midiFile.getTrack(i)->getEventPointer(0)->message.isTrackNameEvent())
    {
      if (midiFile.getTrack(i)->getEventPointer(0)->message.getTextFromTextMetaEvent() == "Tempo Track")
      {
        tempoTrack = *midiFile.getTrack(i);
        continue;
      }
    }
    sequences.push_back(juce::MidiMessageSequence(*midiFile.getTrack(i)));
  }

  tracks.resize(sequences.size());

  for (unsigned int i = 0; i < tracks.size(); ++i)
  {
    tracks[i] = new JuceModule::Track(11+i, sequences[i]);
  }

  sequencer = new JuceModule::Sequencer(tracks, midiFile.getTimeFormat());
  sequencer->setTempoTrack(tempoTrack);
}

void MidiPlayer::saveSequence(const char* filePath)
{
  sequencer->saveSequence(filePath);
}

void MidiPlayer::play()
{
  jassert(sequencer);
  if (!sequencer)
    return;

  sequencer->play();
}

void MidiPlayer::pause()
{
 if (!sequencer)
    return;
 sequencer->pause();
}

void MidiPlayer::stop()
{
  if (!sequencer)
    return;

  sequencer->stop();
}

void MidiPlayer::setTempo(juce::uint32 tempo)
{
  if (!sequencer)
    return;
  sequencer->setTempo(tempo);
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

DefineEngineMethod(MidiPlayer, saveSequence, void, (const char* filePath),, "Save the modified sequence")
{
  object->saveSequence(filePath);
}
