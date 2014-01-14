#include "JuceAudioThread.h"

//***********INCLUDE JUCE***************
// WARNING : Disable Juce namespace to avoid name confusion on ThreadPool class
#define DONT_SET_USING_JUCE_NAMESPACE 1
#define JUCE_ASIO 1 //INDISPENSABLE pour faire fonctionner les VST
#include "JuceLibraryCode/JuceHeader.h"

//**********INCLUDE WINDOWS THREAD******
#include <Objbase.h>

//**********INCLUDE TORQUE ENGINE*********
#include "console/engineAPI.h"

namespace JuceModule
{

void AudioMidiWorkItem::execute()
{
  CoInitialize(nullptr);
  juce::AudioDeviceManager deviceManager;
  deviceManager.initialise (2, 2, nullptr, true);
  deviceManager.playTestSound();
  
  juce::AudioPluginFormatManager formatManager;
  formatManager.addDefaultFormats();
  juce::KnownPluginList list;
  juce::String errorMessage;
  juce::PluginDescription description2;
  description2.name = "4Front_Piano";
  description2.pluginFormatName = "VST";
  description2.category = "Instrument";
  description2.fileOrIdentifier =  juce::File::getCurrentWorkingDirectory().getChildFile("../4Front_Piano.dll").getFullPathName();
  juce::ScopedPointer<juce::AudioPluginInstance> plugin = formatManager.createPluginInstance(description2, errorMessage);
  Con::printf(errorMessage.getCharPointer());

  juce::AudioProcessorPlayer player;
  if (plugin != nullptr) 
  {
    player.setProcessor(plugin);
    deviceManager.addAudioCallback(&player);
  }
 
  juce::File myFile = juce::File::getCurrentWorkingDirectory().getChildFile ("../Beethoven-Symphony5-1.mid");
    
  juce::FileInputStream stream(myFile);
  Con::printf(stream.getStatus().getErrorMessage().getCharPointer());

  if (stream.failedToOpen())
  {
    Con::printf("Impossible d'ouvrir le fichier midi");
    jassert(false);
  }
  juce::MidiFile midiFile;
  midiFile.readFrom(stream);
    
  const juce::MidiMessageSequence* sequence = midiFile.getTrack(3);
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