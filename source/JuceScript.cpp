//#include <JuceAudioThread.h>
#include <JuceScript.h>
#include "console/console.h"

#include "Sequencer.h"

//**********INCLUDE WINDOWS THREAD******
#include <Objbase.h>

IMPLEMENT_CONOBJECT(MidiPlayer);

MidiPlayer::MidiPlayer()
  : fileLoaded(false)
{
}

MidiPlayer::~MidiPlayer()
{
  for (unsigned int i = 0; i < sequencer.size(); ++i)
    sequencer[i]->stop();
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

  for (unsigned int i = 0; i < midiFile.getNumTracks(); ++i)
  {
    sequences.push_back(juce::MidiMessageSequence(*midiFile.getTrack(i)));
  }

  sequencer.resize(sequences.size());

  for (unsigned int i = 0; i < sequencer.size(); ++i)
    sequencer[i] = new JuceModule::Track(11+i, sequences[i], midiFile.getTimeFormat());
  
}

void MidiPlayer::play()
{
  jassert(fileLoaded);
  if (!fileLoaded)
    return;
  //Ajoute l'item à la file d'attente du ThreadPool pour qu'il soit traité
  for (unsigned int i = 0; i < sequencer.size(); ++i)
    sequencer[i]->startThread();
    //JuceModule::AudioMidiThreadPool::instance()->queueWorkItem(sequencer[i]);

 // JuceModule::AudioMidiThreadPool::instance()->flushWorkItems();//je sais pas encore trop pourquoi faut mettre ça
}

void MidiPlayer::pause()
{
 for (unsigned int i = 0; i < sequencer.size(); ++i)
    sequencer[i]->pause();
}

void MidiPlayer::unpause()
{
  for (unsigned int i = 0; i < sequencer.size(); ++i)
    sequencer[i]->play();
}

void MidiPlayer::stop()
{
  jassert(fileLoaded);
  if (!fileLoaded)
    return;
  for (unsigned int i = 0; i < sequencer.size(); ++i)
  {
    sequencer[i]->stopThread(1000);
  }
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

DefineEngineMethod(MidiPlayer, unpause, void, (),, "Unpause a MIDI sequence" )
{
  object->unpause();
}

