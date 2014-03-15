#ifndef SEQUENCER_AUDIOTOOLS_H
#define SEQUENCER_AUDIOTOOLS_H

#include "JuceLibraryCode\JuceHeader.h"
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

}; //namespace JuceModule
#endif SEQUENCER_AUDIOTOOLS_H