#include "JuceAudioThread.h"

//***********INCLUDE JUCE***************
#include "JuceLibraryCode/JuceHeader.h"

//**********INCLUDE WINDOWS THREAD******
#include <Objbase.h>

//**********INCLUDE TORQUE ENGINE*********
#include "console/engineAPI.h"

#include <vector>
#include "Sequencer.h"

namespace JuceModule
{

AudioMidiWorkItem::AudioMidiWorkItem(U32 index)
  : mIndex(index), askedToStop(false)
{
  juce::AudioPluginFormatManager formatManager;
  formatManager.addDefaultFormats();
  juce::KnownPluginList list;
  juce::String errorMessage;
  juce::PluginDescription description2;
  description2.name = "4Front_Piano";
  description2.pluginFormatName = "VST";
  description2.category = "Instrument";
  description2.fileOrIdentifier =  juce::File::getCurrentWorkingDirectory().getChildFile("../4Front_Piano.dll").getFullPathName();
  plugin = formatManager.createPluginInstance(description2, errorMessage);
  Con::printf(errorMessage.getCharPointer());
}

AudioMidiWorkItem::~AudioMidiWorkItem()
{
  deviceManager.closeAudioDevice();
  juce::MessageManager::deleteInstance();
  CoUninitialize();
}

void AudioMidiWorkItem::execute()
{
  //Le plugin doit être correctement initialisé, sinon c'est qu'il y a eu un problème
  jassert(plugin);
  CoInitialize(nullptr);
  deviceManager.initialise (2, 2, nullptr, true);
  deviceManager.playTestSound();

  player.setProcessor(plugin);
  deviceManager.addAudioCallback(&player);

  Con::printf(deviceManager.getCurrentAudioDeviceType().toStdString().c_str());
 
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

    //Interrompt la lecture si le thread doit être fermé
    if (cancellationPoint())
      break;
  }
}

} // namespace JuceModule