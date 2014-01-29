//#include <JuceAudioThread.h>
#include <JuceScript.h>
#include "console/console.h"

#include "Sequencer.h"

//**********INCLUDE WINDOWS THREAD******
#include <Objbase.h>

IMPLEMENT_CONOBJECT(Orchestrator);

Orchestrator::Orchestrator()
{
}

Orchestrator::~Orchestrator()
{
  kinectTracker->stopThread(100);
}

void Orchestrator::loadMidiFile(const char* filePath)
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
  std::vector<juce::MidiMessageSequence> sequences;
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

  kinectTracker = new KinectModule::KinectTracker(sequencer);
  kinectTracker->startThread();
  kinectTracker->setPriority(10);
}

void Orchestrator::increaseVelocityFactor(short percentage)
{
  sequencer->increaseVelocityFactorInPercent(percentage);
}

void Orchestrator::decreaseVelocityFactor(short percentage)
{
  sequencer->decreaseVelocityFactorInPercent(percentage);
}

void Orchestrator::saveSequence(const char* filePath)
{
  sequencer->saveSequence(filePath);
}

void Orchestrator::play()
{
  jassert(sequencer);
  if (!sequencer)
    return;

  sequencer->play();
}

void Orchestrator::pause()
{
 if (!sequencer)
    return;
 sequencer->pause();
}

void Orchestrator::stop()
{
  if (!sequencer)
    return;

  sequencer->stop();
}

int Orchestrator::getTempo() const
{
  return static_cast<int>(sequencer->getTempo());
}

void Orchestrator::setTempo(juce::uint32 tempo)
{
  if (!sequencer)
    return;
  sequencer->setTempo(tempo);
}

int Orchestrator::getNumTracks()
  {return tracks.size();}

String Orchestrator::getInstrumentName(int index)
{
  jassert (index < tracks.size());

  return tracks[index]->getInstrumentName().toStdString().c_str();
}

String Orchestrator::getTrackName(int index)
{
  jassert (index < tracks.size());

  return tracks[index]->getTrackName().toStdString().c_str();
}

//-------------Torque Script Bridge

DefineEngineMethod(Orchestrator, loadMidiFile, void, (const char* filePath), , "Load a Midi File. You must call this method before any other on Orchestrator.")
{
  object->loadMidiFile(filePath);
}

DefineEngineMethod(Orchestrator, play, void, (),, "Play a MIDI sequence" )
{
  object->play();
}

DefineEngineMethod(Orchestrator, stop, void, (),, "Stop a MIDI sequence" )
{
  object->stop();
}

DefineEngineMethod(Orchestrator, pause, void, (),, "Pause a MIDI sequence" )
{
  object->pause();
}

DefineEngineMethod(Orchestrator, getTempo, int, (),, "Get the current tempo of the midi sequence")
{
  return object->getTempo();
}

DefineEngineMethod(Orchestrator, setTempo, void, (unsigned int tempo),, "Set a new tempo for the midi sequence")
{
  object->setTempo(tempo);
}

DefineEngineMethod(Orchestrator, saveSequence, void, (const char* filePath),, "Save the modified sequence")
{
  object->saveSequence(filePath);
}

DefineEngineMethod(Orchestrator, increaseVelocity, void, (int percentage),, "Increase velocity of midi notes")
{
  object->increaseVelocityFactor(static_cast<short>(percentage));
}

DefineEngineMethod(Orchestrator, decreaseVelocity, void, (int percentage),, "Decrease velocity of midi notes")
{
  object->decreaseVelocityFactor(static_cast<short>(percentage));
}

DefineEngineMethod(Orchestrator, getNumTracks, int, (),, "Return the number of tracks in the sequence")
{
  return object->getNumTracks();
}

DefineEngineMethod(Orchestrator, getInstrumentName, String, (int index),, "Get names of the instruments in the sequence at the track of index 'index'")
{
  return object->getInstrumentName(index);
}

DefineEngineMethod(Orchestrator, getTrackName, String, (int index),, "Get the track name at index 'index' in the sequence")
{
  return object->getTrackName(index);
}