#ifndef SEQUENCER_TRACK_H
#define SEQUENCER_TRACK_H

#include <list>
#include "JuceLibraryCode\JuceHeader.h"

namespace JuceModule
{

/**
  A Track goal is to play a juce::MidiMessageSequence.
  A Track triggers three types of events (TS callbacks) :
  * onInstrumentWillPlay(%instrumentName, %delayInMillis)
  * onInstrumentStartPlaying(%instrumentName)
  * onInstrumentStoppedPlaying(%instrumentName)
**/
class Track : public juce::ReferenceCountedObject
{
public:
  typedef juce::ReferenceCountedObjectPtr<Track> Ptr;

  enum PlayingStatus {DO_NOT_PLAY, PLAY, WILL_PLAY_SOON};
  static const int EXPRESSION_CC;
  static const double WILL_PLAY_DELAY_MS;
  static const double DO_NOT_PLAY_DELAY_MS;

  Track(juce::MidiMessageSequence sequence);

  void playAtTick(double tick);
  void restart();
  juce::MidiMessageSequence getSequence() const;
  bool isFinished() const;

  void setExpression(float value);

  void setMsPerTick(double msPerTick)
    {this->msPerTick = msPerTick;}

  juce::String getTrackName() const
    {return trackName;}

  juce::String getInstrumentName() const
    {return instrumentName;}

protected:
  //See std::list::remove_if
  struct equals
  {
    int value2;
    bool operator() (const int& value1)
    {return value1 == value2;}
  };

  juce::String extractInstrumentNameFromTrackName(const juce::String& trackName);
  void checkPlayingStatus(double tick, double timeStamp, bool isNoteOn);

  juce::MidiMessageSequence sequence;
  int eventIndex;
  juce::String trackName;
  juce::String instrumentName;
  juce::MidiMessage expressionMessage;
  bool expressionChanged;
  PlayingStatus playingStatus;
  double msPerTick;
  juce::CriticalSection sequenceAccess;
  std::list<int> incomingKeyUp;
};

}; //namespace JuceModule

#endif //SEQUENCER_TRACK_H
