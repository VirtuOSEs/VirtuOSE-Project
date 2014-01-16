#include <JuceAudioThread.h>
#include <JuceScript.h>
#include "console/console.h"

#include "Sequencer.h"

//**********INCLUDE WINDOWS THREAD******
#include <Objbase.h>

IMPLEMENT_CONOBJECT(MidiPlayer);

MidiPlayer::MidiPlayer()
{
  //L'item est un objet qui contient la méthode execute() qui va être appelée par le ThreadPool
  //sequencer = new JuceModule::AudioMidiWorkItem(10);

  //TEST 
  juce::File myFile = juce::File::getCurrentWorkingDirectory().getChildFile ("../Beethoven-Symphony5-1.mid");
    
  juce::FileInputStream stream(myFile);
  Con::printf(stream.getStatus().getErrorMessage().getCharPointer());

  if (stream.failedToOpen())
  {
    Con::printf("Impossible d'ouvrir le fichier midi");
    jassert(false);
  }
  juce::MidiFile midiFile;
  midiFile.readFrom(stream);

  //sequence = *(midiFile.getTrack(3));

  for (unsigned int i = 0; i < midiFile.getNumTracks(); ++i)
  {
    sequences.push_back(juce::MidiMessageSequence(*midiFile.getTrack(i)));
  }
  /*sequences.resize(1);
  sequences.push_back(*midiFile.getTrack(3));*/
  sequencer.resize(sequences.size());

  for (unsigned int i = 0; i < sequencer.size(); ++i)
    sequencer[i] = new JuceModule::Track(11+i, sequences[i], midiFile.getTimeFormat());
}

MidiPlayer::~MidiPlayer()
{
  //sequencer->stop();
  for (unsigned int i = 0; i < sequencer.size(); ++i)
    sequencer[i]->stop();
}

void MidiPlayer::play()
{
  //TEST

  //Ajoute l'item à la file d'attente du ThreadPool pour qu'il soit traité
  for (unsigned int i = 0; i < sequencer.size(); ++i)
    JuceModule::AudioMidiThreadPool::instance()->queueWorkItem(sequencer[i]);

 // JuceModule::AudioMidiThreadPool::instance()->flushWorkItems();//je sais pas encore trop pourquoi faut mettre ça
}

void MidiPlayer::stop()
{
  //TEST
  for (unsigned int i = 0; i < sequencer.size(); ++i)
    sequencer[i]->stop();

  //sequencer->stop();
}


DefineEngineMethod(MidiPlayer, play, void, (),, "Play a MIDI sequence" )
{
  object->play();
}

DefineEngineMethod(MidiPlayer, stop, void, (),, "Stop a MIDI sequence" )
{
  object->stop();
}