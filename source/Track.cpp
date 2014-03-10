#include "Track.h"
#include "console/engineAPI.h"
#include "AudioTools.h"
#include "TSCallback.h"

//La fonction TorqueScript pour changer l'opacité d'un objet
//IMPLEMENT_GLOBAL_CALLBACK( changeOpacity, void, ( const char* instrumentName, float opacity ), ( instrumentName, opacity ),
//   "A callback called by the engine when a a track will be played soon.\n"
//   "@param name The name of the instrument which will be played.\n"
//  );

namespace JuceModule
{

const double Track::WILL_PLAY_DELAY_MS = 2000.0;
const double Track::DO_NOT_PLAY_DELAY_MS = 5000.0;

Track::Track(juce::MidiMessageSequence sequence)
  : sequence(sequence),
    eventIndex(0), 
    trackName(juce::String::empty),
    velocityFactor(1),
    velocity(0.5f),
    velocityChanged(false),
    playingStatus(DO_NOT_PLAY),
    msPerTick(0)
{
  //Look for the track name meta event in the entire sequence (should be at the beginning)
  int i = 0;
  while (i < sequence.getNumEvents() && trackName == juce::String::empty)
  {
    juce::MidiMessage& message = sequence.getEventPointer(i)->message;
    if (message.isTrackNameEvent())
    {
      trackName = message.getTextFromTextMetaEvent();
    }
    ++i;
  }
  //We consider that the instrument name is in the track name (ex: "2 Horns in Eb")
  instrumentName = extractInstrumentNameFromTrackName(trackName);

  //We instanciate a plugin for this track with the right instrument, based on the instrumentName
  AudioTools::getInstance().generatePlugin(trackName, instrumentName);
}

void Track::restart()
{
  if (eventIndex == 0)
    return;

  //Tentative d'annuler le bug de la note tenue lors d'un stop() play()
  //Ca marche mais c'est bourrin
  int i = eventIndex;
  while (i < sequence.getNumEvents())
  {
    juce::MidiMessageSequence::MidiEventHolder* midiEvent = sequence.getEventPointer(i);
    if (midiEvent->message.isNoteOff())
      AudioTools::getInstance().makePluginPlay(trackName, midiEvent->message);
    ++i;
  }

  eventIndex = 0;
}

void Track::setVelocity(float value)
{
  jassert(velocity >= 0.f && velocity <= 1.f);
  velocity = value;
  velocityChanged = true;
}

void Track::playAtTick(double tick)
{
  if (eventIndex >= sequence.getNumEvents())
    return;

  juce::MidiMessageSequence::MidiEventHolder* midiEvent = nullptr;
  {
    juce::ScopedLock sl(sequenceAccess);
    midiEvent = sequence.getEventPointer(eventIndex);
  }

  double timeStamp = midiEvent->message.getTimeStamp();

  if (timeStamp == 0) 
  { 
    eventIndex++;
    return;
  }

  //If it's time, play the event
  if ( timeStamp <= tick)
  {
    if (velocityChanged)
      midiEvent->message.setVelocity(velocity);
    AudioTools::getInstance().makePluginPlay(trackName, midiEvent->message);
    //Used to track the playing status of the track
    if (midiEvent->message.isNoteOn())
    {
      {
        juce::ScopedLock sl(sequenceAccess);
        incomingKeyUp.push_back(sequence.getIndexOfMatchingKeyUp(eventIndex));
      }
    }
    eventIndex++;
  }

  //Remove the incomingKeyUp which already came
  //it's not clean, but it works. See std::list::remove_if
  equals eq;
  eq.value2 = eventIndex;
  incomingKeyUp.remove_if(eq);

  checkPlayingStatus(tick, timeStamp);
}

juce::MidiMessageSequence Track::getSequence() const
{
  juce::ScopedLock sL(sequenceAccess);
  return sequence;

}

juce::String Track::extractInstrumentNameFromTrackName(const juce::String& trackName)
{
  //On regarde les noms des fichiers .fxp dans le répertoire fxp
  //Si la trackName contient un de ces noms, c'est l'instrument qu'on cherche !
  juce::Array<juce::File> fxpFiles;
  juce::File fxpDirectory = juce::File::getCurrentWorkingDirectory().getChildFile("../fxp").getFullPathName();
  fxpDirectory.findChildFiles(fxpFiles, juce::File::findFiles, false, "*.fxp");
  for(int i = 0; i < fxpFiles.size(); ++i)
  {
    if (trackName.containsIgnoreCase(fxpFiles[i].getFileNameWithoutExtension()))
      return fxpFiles[i].getFileNameWithoutExtension();
  }
  return juce::String::empty;
}

void Track::checkPlayingStatus(double tick, double timeStamp)
{
  double delay = 0.0;
  if (playingStatus == DO_NOT_PLAY)
  {
    //Special case : begining of a sequence
    if (timeStamp <= tick)
    {
      playingStatus = PLAY;
      CallbackManager::instrumentStartPlaying(instrumentName);
    }
    else if (timeStamp > tick && (delay = (timeStamp - tick) * msPerTick) < WILL_PLAY_DELAY_MS)
    {
      playingStatus = WILL_PLAY_SOON;
      CallbackManager::instrumentWillPlay(instrumentName, delay);
    }
  }
  else if (playingStatus == WILL_PLAY_SOON)
  {
    if (timeStamp <= tick)
    {
      playingStatus = PLAY;
      CallbackManager::instrumentStartPlaying(instrumentName);
    }
  }
  else if (playingStatus == PLAY)
  {
    if (incomingKeyUp.empty() && (timeStamp - tick) * msPerTick > DO_NOT_PLAY_DELAY_MS)
    {
      playingStatus = DO_NOT_PLAY;
      CallbackManager::instrumentStoppedPlay(instrumentName);
    }
  }
}

}; //namespace JuceModule