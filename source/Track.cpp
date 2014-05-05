#include "Track.h"
#include "console/engineAPI.h"
#include "AudioTools.h"
#include "TSCallback.h"

namespace JuceModule
{

const double Track::WILL_PLAY_DELAY_MS = 2000.0;
const double Track::DO_NOT_PLAY_DELAY_MS = 3000.0;
const int Track::EXPRESSION_CC = 11;

Track::Track(juce::MidiMessageSequence sequence)
  : sequence(sequence),
    eventIndex(0), 
    trackName(juce::String::empty),
    expressionValue(64),
    expressionChanged(false),
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
      trackName = trackName.removeCharacters(" /");
      if (trackName.indexOfAnyOf("0123456789", 0, true) == 0)
      {
        trackName = trackName.replaceSection(0, 1, "X");
      }
    }
    ++i;
  }
  
  //We consider that the instrument name is in the track name (ex: "2 Horns in Eb")
  instrumentName = extractInstrumentNameFromTrackName(trackName);

  //We instanciate a plugin for this track with the right instrument, based on the instrumentName
  AudioTools::getInstance().generatePlugin(trackName, instrumentName);

  this->sequence.updateMatchedPairs();
}

void Track::restart()
{
  if (eventIndex == 0 || incomingKeyUp.empty())
    return;

  AudioTools::getInstance().enableAudioProcessing();
  for (auto it = incomingKeyUp.begin(); it != incomingKeyUp.end(); ++it)
  {
    juce::MidiMessageSequence::MidiEventHolder* midiEvent = sequence.getEventPointer(*it);
    AudioTools::getInstance().makePluginPlay(trackName, midiEvent->message);
  }
  eventIndex = 0;
  expressionChanged = false;
  incomingKeyUp.clear();
}

bool Track::isFinished() const
{
  return eventIndex >= sequence.getNumEvents();
}

void Track::setExpression(float value)
{
  expressionValue = static_cast<int>(value * 127);
  expressionChanged = true;
}

void Track::playAtTick(double tick)
{
  if (isFinished())
    return;

  juce::MidiMessageSequence::MidiEventHolder* midiEvent = nullptr;
  {
    juce::ScopedLock sl(sequenceAccess);
    midiEvent = sequence.getEventPointer(eventIndex);
  }

  if (expressionChanged.get())
  {
    juce::MidiMessage expressionMessage = juce::MidiMessage::controllerEvent(1, EXPRESSION_CC, expressionValue);
    expressionMessage.setTimeStamp(tick);
    expressionMessage.setChannel(midiEvent->message.getChannel());
    AudioTools::getInstance().makePluginPlay(trackName, expressionMessage);
    sequence.addEvent(expressionMessage);
    expressionChanged = false;
  }

  double timeStamp = midiEvent->message.getTimeStamp();
  while (timeStamp == 0) 
  { 
    ++eventIndex;
    if (isFinished())
      return;
    {
      juce::ScopedLock sl(sequenceAccess);
      midiEvent = sequence.getEventPointer(eventIndex);
    }

    timeStamp = midiEvent->message.getTimeStamp();
  }

  //If it's time, play the event
  while ( timeStamp <= tick)
  {
    bool isNoteOn = midiEvent->message.isNoteOn();
    AudioTools::getInstance().makePluginPlay(trackName, midiEvent->message);
    //Used to track the playing status of the track
    if (isNoteOn)
    {
      int indexKeyUp = 0;
      {
        juce::ScopedLock sl(sequenceAccess);
        indexKeyUp = sequence.getIndexOfMatchingKeyUp(eventIndex);
      }
      //Security
      if (indexKeyUp != -1)
        incomingKeyUp.push_back(indexKeyUp);
    }
 
    //Remove the incomingKeyUp which already came
    //it's not clean, but it works. See std::list::remove_if
    equals eq;
    eq.value2 = eventIndex;
    incomingKeyUp.remove_if(eq);

    ++eventIndex;
    checkPlayingStatus(tick, timeStamp, isNoteOn);

    if (isFinished())
      break;

    midiEvent = nullptr;
    {
      juce::ScopedLock sl(sequenceAccess);
      midiEvent = sequence.getEventPointer(eventIndex);
    }

    timeStamp = midiEvent->message.getTimeStamp();
  }
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

void Track::checkPlayingStatus(double tick, double timeStamp, bool isNoteOn)
{
  double delay = 0.0;
  if (playingStatus == DO_NOT_PLAY && isNoteOn)
  {
    //Special case : begining of a sequence
    if (timeStamp <= tick)
    {
      playingStatus = PLAY;
      CallbackManager::instrumentStartPlaying(trackName);
    }
    else if (timeStamp > tick && (delay = (timeStamp - tick) * msPerTick) < WILL_PLAY_DELAY_MS)
    {
      playingStatus = WILL_PLAY_SOON;
      CallbackManager::instrumentWillPlay(trackName, delay);
    }
  }
  else if (playingStatus == WILL_PLAY_SOON && isNoteOn)
  {
    if (timeStamp <= tick)
    {
      playingStatus = PLAY;
      CallbackManager::instrumentStartPlaying(trackName);
    }
  }
  else if (playingStatus == PLAY)
  {
    if (isFinished() || (incomingKeyUp.empty() && (timeStamp - tick) * msPerTick > DO_NOT_PLAY_DELAY_MS) )
    {
      playingStatus = DO_NOT_PLAY;
      CallbackManager::instrumentStoppedPlay(trackName);
    }
  }
}

}; //namespace JuceModule