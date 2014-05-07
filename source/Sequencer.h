#ifndef JUCE_MODULE_SEQUENCER
#define JUCE_MODULE_SEQUENCER

//***********INCLUDE JUCE***************
#include "JuceLibraryCode/JuceHeader.h"

#include <vector>
#include "Track.h"
#include "Options.h"

namespace JuceModule
{

/**
  The actual objects which play the midi tracks.
  Maintains also a special MidiSequence containing all the tempo change
  events.
 **/
class Sequencer : public juce::Thread, public juce::ReferenceCountedObject
{
public:
  typedef juce::ReferenceCountedObjectPtr<Sequencer> Ptr;
  
  ///Time of wait of the sequencer thread
  static const int TIME_STEP;
  ///Number of milliseconds per minute
  static const double MS_PER_MINUTE;

  Sequencer(std::vector<JuceModule::Track::Ptr > tracks, short ticksPerQuarterNote,
            const Options& options);
  ~Sequencer();

  void saveSequence(const juce::String& filePath);
  double getTick();

  void setTempoTrack(const juce::MidiMessageSequence& tempoTrack);

  juce::uint32 getTempo() const;
  void setTempo(juce::uint32 tempo);
  void setExpression(float value);

  void stop();
  void pause();
  void play();

protected:
  virtual void run();
  void checkTempoChangeTrack();
  inline void updateTracksMsPerTick(double msPerTick);

private:
  inline double computeMsPerTicks();
  inline double computeTickStep();

  short ticksPerQuarterNote;
  std::vector<JuceModule::Track::Ptr > tracks;
  //On lit les tempo event depuis tempoTrack
  juce::MidiMessageSequence tempoTrack;
  //On écrit les tempos event ajoutés par l'utilisateur dans newTempoTrack
  juce::MidiMessageSequence newTempoTrack;
  Options options;

  bool paused;
  bool stopped;
  juce::uint32 tempo;
  double ticks;
  double msPerTick;
  unsigned int tempoTrackIndex;
  double tickStep;//Number of ticks incremented after each timeStep

  juce::CriticalSection ticksAccess;
  juce::CriticalSection tempoAccess;
  juce::CriticalSection stoppedAccess;
};

} // namespace JuceModule

#endif // JUCE_MODULE_SEQUENCER