#include <JuceAudioThread.h>
#include <JuceScript.h>
#include "console/console.h"


//**********INCLUDE WINDOWS THREAD******
#include <Objbase.h>

IMPLEMENT_CONOBJECT(MidiPlayer);

MidiPlayer::MidiPlayer()
{
  //L'item est un objet qui contient la méthode execute() qui va être appelée par le ThreadPool
  sequencer = new JuceModule::AudioMidiWorkItem(10);
}

MidiPlayer::~MidiPlayer()
{
  sequencer->stop();
}

void MidiPlayer::playMidi()
{
  //Ajoute l'item à la file d'attente du ThreadPool pour qu'il soit traité
  JuceModule::AudioMidiThreadPool::instance()->queueWorkItem(sequencer);
  JuceModule::AudioMidiThreadPool::instance()->flushWorkItems();//je sais pas encore trop pourquoi faut mettre ça
}

void MidiPlayer::stopMidi()
{
  sequencer->stop();
}


DefineEngineMethod(MidiPlayer, startMIDI, void, (),, "Play a MIDI sequence" )
{
  object->playMidi();
}

DefineEngineMethod(MidiPlayer, stopMIDI, void, (),, "Stop a MIDI sequence" )
{
  object->stopMidi();
}