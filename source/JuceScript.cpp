#include <JuceAudioThread.h>
#include <JuceScript.h>
#include "console/console.h"


//**********INCLUDE WINDOWS THREAD******
#include <Objbase.h>

IMPLEMENT_CONOBJECT(MidiPlayer);

MidiPlayer::MidiPlayer()
{
  //L'item est un objet qui contient la m�thode execute() qui va �tre appel�e par le ThreadPool
  sequencer = new JuceModule::AudioMidiWorkItem(10);
}

MidiPlayer::~MidiPlayer()
{
  sequencer->stop();
}

void MidiPlayer::playMidi()
{
  //Ajoute l'item � la file d'attente du ThreadPool pour qu'il soit trait�
  JuceModule::AudioMidiThreadPool::instance()->queueWorkItem(sequencer);
  JuceModule::AudioMidiThreadPool::instance()->flushWorkItems();//je sais pas encore trop pourquoi faut mettre �a
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