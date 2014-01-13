#include <JuceScript.h>
#include "console/console.h"
#include "console/engineAPI.h"

//***********INCLUDE JUCE***************
// WARNING : Disable Juce namespace to avoid name confusion on ThreadPool class
#define DONT_SET_USING_JUCE_NAMESPACE 1
#include "JuceLibraryCode/JuceHeader.h"
//**********INCLUDE WINDOWS THREAD******
#include <Objbase.h>

namespace JuceModule
{

void AudioMidiWorkItem::execute()
{
  CoInitialize(nullptr);
  juce::AudioDeviceManager deviceManager;
  deviceManager.initialise (256, 256, nullptr, true);
  //deviceManager.playTestSound();
  
  //AudioIODeviceType* device = AudioIODeviceType::createAudioIODeviceType_DirectSound ();
  juce::AudioPluginFormatManager formatManager;
  formatManager.addDefaultFormats();
  juce::KnownPluginList list;
  juce::String errorMessage;
  juce::PluginDescription description2;
  description2.name = "4Front_Piano";
  description2.pluginFormatName = "VST";
  description2.category = "Instrument";
  description2.fileOrIdentifier = "G:/Dev/Apari_Tenpan/Apari_Tenpan.dll";
  juce::ScopedPointer<juce::AudioPluginInstance> plugin = formatManager.createPluginInstance(description2, errorMessage);
  Con::printf(errorMessage.getCharPointer());

  juce::AudioProcessorPlayer player;
  if (plugin != nullptr) 
  {
    player.setProcessor(plugin);
    deviceManager.addAudioCallback(&player);
    player.getMidiMessageCollector().reset(44100);
  }
 
  juce::File myFile("G:/Archives_origines-2013/Projets_Musique/Projet_2/Midi/Piano_premier_mouvement.mid");
    
  juce::FileInputStream stream(myFile);
  Con::printf(stream.getStatus().getErrorMessage().getCharPointer());

  if (stream.failedToOpen())
    Con::printf("Impossible d'ouvrir le fichier midi");
  juce::MidiFile midiFile;
  midiFile.readFrom(stream);
    
  const juce::MidiMessageSequence* sequence = midiFile.getTrack(1);
  double startTime = sequence->getStartTime();

  //std::cout << sequence->getNextIndexAtTime(startTime) << std::endl;
  juce::MidiMessageSequence::MidiEventHolder* midiEvent = sequence->getEventPointer(0);
  //std::cout << midiEvent->message.getMidiNoteName(midiEvent->message.getNoteNumber(), true, true, 3) << std::endl;
  
  double msPerTick = 60000.0 / 120.0 / midiFile.getTimeFormat(); 
  double nextTime;
  double prevTimestamp = 0.0;
  for (int i = 0; i < sequence->getNumEvents(); ++i)
  {
    juce::MidiMessageSequence::MidiEventHolder* midiEvent = sequence->getEventPointer(i);
    nextTime = msPerTick * (midiEvent->message.getTimeStamp() - prevTimestamp);

    juce::Time::waitForMillisecondCounter(juce::Time::getMillisecondCounter() + juce::uint32(nextTime));
    if (midiEvent->message.getTimeStamp() != 0)
    {
      player.handleIncomingMidiMessage(nullptr, midiEvent->message);
          
    }
    prevTimestamp = midiEvent->message.getTimeStamp();
    std::cout << midiEvent->message.getMidiNoteName(midiEvent->message.getNoteNumber(), true, true, 3) << std::endl;

    //Interrompt la lecture si le thread doit être fermé
    if (cancellationPoint())
      break;
  }
  
  deviceManager.closeAudioDevice();
  CoUninitialize();
}

} // namespace JuceModule

DefineEngineFunction( playMIDI, void, (),, "Play a MIDI sequence mothafucka" )
{
  juce::AudioDeviceManager deviceManager;
  deviceManager.initialise (256, 256, nullptr, true);
  //deviceManager.playTestSound();
  
  //AudioIODeviceType* device = AudioIODeviceType::createAudioIODeviceType_DirectSound ();
  juce::AudioPluginFormatManager formatManager;
  formatManager.addDefaultFormats();
  juce::KnownPluginList list;
  juce::String errorMessage;
  juce::PluginDescription description2;
  description2.name = "4Front_Piano";
  description2.pluginFormatName = "VST";
  description2.category = "Instrument";
  description2.fileOrIdentifier = "G:/Dev/Apari_Tenpan/Apari_Tenpan.dll";
  juce::ScopedPointer<juce::AudioPluginInstance> plugin = formatManager.createPluginInstance(description2, errorMessage);
  Con::printf(errorMessage.getCharPointer());

  juce::AudioProcessorPlayer player;
  if (plugin != nullptr) 
  {
    player.setProcessor(plugin);
    deviceManager.addAudioCallback(&player);
  }
 
  juce::File myFile("G:/Archives_origines-2013/Projets_Musique/Projet_2/Midi/Piano_premier_mouvement.mid");
    
  juce::FileInputStream stream(myFile);
  Con::printf(stream.getStatus().getErrorMessage().getCharPointer());

  if (stream.failedToOpen())
    Con::printf("Impossible d'ouvrir le fichier midi");
  juce::MidiFile midiFile;
  midiFile.readFrom(stream);
    
  const juce::MidiMessageSequence* sequence = midiFile.getTrack(1);
  double startTime = sequence->getStartTime();

  //std::cout << sequence->getNextIndexAtTime(startTime) << std::endl;
  juce::MidiMessageSequence::MidiEventHolder* midiEvent = sequence->getEventPointer(0);
  //std::cout << midiEvent->message.getMidiNoteName(midiEvent->message.getNoteNumber(), true, true, 3) << std::endl;
  
  double msPerTick = 60000.0 / 120.0 / midiFile.getTimeFormat(); 
  double nextTime;
  double prevTimestamp = 0.0;
  //for (int i = 0; i < sequence->getNumEvents(); ++i)
  //{
  //  MidiMessageSequence::MidiEventHolder* midiEvent = sequence->getEventPointer(i);
  //  nextTime = msPerTick * (midiEvent->message.getTimeStamp() - prevTimestamp);

  //  Time::waitForMillisecondCounter(Time::getMillisecondCounter() + uint32(nextTime));
  //  if (midiEvent->message.getTimeStamp() != 0)
  //  {
  //    player.handleIncomingMidiMessage(nullptr, midiEvent->message);
  //    
  //  }
  //  prevTimestamp = midiEvent->message.getTimeStamp();
  //  std::cout << midiEvent->message.getMidiNoteName(midiEvent->message.getNoteNumber(), true, true, 3) << std::endl;

  //}

  deviceManager.closeAudioDevice();
}