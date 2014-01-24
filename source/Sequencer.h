#ifndef JUCE_MODULE_SEQUENCER
#define JUCE_MODULE_SEQUENCER

//***********INCLUDE JUCE***************
#include "JuceLibraryCode/JuceHeader.h"

//**********INCLUDE WINDOWS THREAD******
#include <Objbase.h>

//**********INCLUDE TORQUE ENGINE*********
#include "console/engineAPI.h"
#include "platform/threads/threadPool.h"

#include <vector>
#include <map>

namespace JuceModule
{

/** Singleton.
    Contains all important objects for managing sound with JUCE
**/
class AudioTools
{
public:

  /** The thread calling this will be the handler of all the important
      audio objects (sound device, VST plugin...).
      Currently this static method is called by the Torque's module "Juce Module"
      (JuceModule.cpp), so I think audio objects are maintained by the main thread.
      Initialise ASIO device, and VST plugin
  */
  static void initialize();
  static AudioTools& getInstance();
  static void deleteInstance();

  ///Generate a plugin associated with the trackName
  void generatePlugin(const juce::String& trackName, const juce::String& instrumentName);
  ///Make the plugin associated with the trackName given in argument plays the message passed as a second argument
  void makePluginPlay(const juce::String& trackName, const juce::MidiMessage& message);

  void disableAudioProcessing();
  void enableAudioProcessing();

private:
  static AudioTools* singleton;
  AudioTools();
  ~AudioTools();
  AudioTools(const AudioTools& other);
  AudioTools& operator=(const AudioTools& other);
 
  std::map<juce::String, juce::ScopedPointer<juce::AudioPluginInstance > > pluginsMap;
  std::map<juce::String, juce::ScopedPointer<juce::AudioProcessorPlayer> > playersMap;
  juce::CriticalSection criticalSection;
  juce::AudioDeviceManager deviceManager;
  juce::AudioProcessorPlayer player;
};

class ChangeOpacity : public ThreadPool::WorkItem
{
public:
  typedef ThreadPool::WorkItem Parent;

  explicit ChangeOpacity(const juce::String& trackName)
    : trackName(trackName) {}

protected:
  virtual void execute();

  juce::String trackName;
};

/**
  A Track goal is to play a juce::MidiMessageSequence.
**/
class Track : public juce::ReferenceCountedObject
{
public:
  typedef juce::ReferenceCountedObjectPtr<Track> Ptr;

  Track(U32 index, juce::MidiMessageSequence& sequence);

  void playAtTick(double tick);
  void restart();
  juce::MidiMessageSequence getSequence() const;

  void increaseVelocityFactor(short percentage)
    {velocityFactor +=  velocityFactor / 100.f * percentage;}

  void decreaseVelocityFactor(short percentage)
    {velocityFactor -= velocityFactor / 100.f * percentage;}

  juce::String getTrackName() const
    {return trackName;}

  juce::String getInstrumentName() const
    {return instrumentName;}

protected:
  juce::String extractInstrumentNameFromTrackName(const juce::String& trackName);

  juce::MidiMessageSequence& sequence;
  int eventIndex;
  juce::String trackName;
  juce::String instrumentName;
  float velocityFactor;
  juce::CriticalSection sequenceAccess;
};


/**
  The actual objects which play the midi tracks.
  Maintains also a special MidiSequence containing all the tempo change
  events.
 **/
class Sequencer : public juce::Thread
{
public:
  Sequencer(std::vector<JuceModule::Track::Ptr > tracks, short timeFormat,
            double tempo = 92.0);
  ~Sequencer();

  void saveSequence(const juce::String& filePath);
  double getTick();

  void setTempoTrack(const juce::MidiMessageSequence& tempoTrack)
    {this->tempoTrack = tempoTrack; newTempoTrack = tempoTrack;}

  juce::uint32 getTempo() const;
  void setTempo(juce::uint32 tempo);
  void increaseVelocityFactorInPercent(short percentage);
  void decreaseVelocityFactorInPercent(short percentage);

  void stop();
  void pause();
  void play();

protected:
  virtual void run();
  void checkTempoChangeTrack();

private:
  short timeFormat;
  std::vector<JuceModule::Track::Ptr > tracks;
  //On lit les tempo event depuis tempoTrack
  juce::MidiMessageSequence tempoTrack;
  //On écrit les tempos event ajoutés par l'utilisateur dans newTempoTrack
  juce::MidiMessageSequence newTempoTrack;

  bool paused;
  bool stopped;
  juce::uint32 tempo;
  double ticks;
  double msPerTick;
  unsigned int tempoTrackIndex;

  juce::CriticalSection ticksAccess;
  juce::CriticalSection tempoAccess;
  juce::CriticalSection stoppedAccess;


};

} // namespace JuceModule

#endif // JUCE_MODULE_SEQUENCER