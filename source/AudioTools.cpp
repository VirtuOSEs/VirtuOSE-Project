#include "AudioTools.h"
#include <Objbase.h>
#include "console/engineAPI.h"


namespace JuceModule
{
  // IMPLEM AUDIOTOOLS
AudioTools* AudioTools::singleton = nullptr;

AudioTools::AudioTools()
{
  juce::AudioDeviceManager::AudioDeviceSetup setup;
  setup.inputChannels = 20;
  setup.outputChannels = 20;
  setup.outputDeviceName = "ASIO4ALL v2";
  setup.inputDeviceName = "ASIO4ALL v2";
  setup.sampleRate = 44100.0000;
  setup.bufferSize = 0;
  setup.useDefaultInputChannels = true;
  setup.useDefaultOutputChannels = true;
    
  
  CoInitialize(nullptr);
  juce::String error = deviceManager.initialise (20, 20, nullptr, false, juce::String::empty, &setup);
  Con::errorf(error.toStdString().c_str());

  deviceManager.playTestSound();
}

AudioTools::~AudioTools()
{
  deviceManager.closeAudioDevice();
  juce::MessageManager::deleteInstance();
  CoUninitialize();
}

void AudioTools::initialize()
{
  if (singleton == nullptr)
    singleton = new AudioTools();
}

void AudioTools::deleteInstance()
{
  delete singleton;
  singleton = nullptr;
}

AudioTools& AudioTools::getInstance()
{
  jassert(singleton);
  return *singleton;
}

void AudioTools::generatePlugin(const juce::String& trackName, const juce::String& instrumentName)
{ 
  juce::AudioPluginFormatManager formatManager;
  formatManager.addDefaultFormats();
  juce::KnownPluginList list;
  juce::String errorMessage;
  juce::PluginDescription description;
  description.name = "sfz";
  description.isInstrument = true;
  description.pluginFormatName = "VST";
  description.category = "Instrument";
  description.fileOrIdentifier =  juce::File::getCurrentWorkingDirectory().getChildFile("../sfz.dll").getFullPathName();

  juce::File fxpFile = juce::File::getCurrentWorkingDirectory().getChildFile("../fxp/" + instrumentName + ".fxp").getFullPathName();
  if (!fxpFile.existsAsFile())
  {
    Platform::outputDebugString("Impossible de charger l'instrument demandé");
    return;
  }
  Platform::outputDebugString(juce::String("Chargement de " + instrumentName).toStdString().c_str());

  //Si le plugin correspondant à l'instrument n'existe pas encore
  if (pluginsMap.find(trackName) == pluginsMap.end())
  {
    pluginsMap[trackName] = formatManager.createPluginInstance(description, errorMessage);

    juce::MemoryBlock fxpData;
    fxpFile.loadFileAsData(fxpData);
    juce::VSTPluginFormat::loadFromFXBFile(pluginsMap[trackName], fxpData.getData(), fxpData.getSize());

    playersMap[trackName] = new juce::AudioProcessorPlayer();
    playersMap[trackName]->setProcessor(pluginsMap[trackName]);
    deviceManager.addAudioCallback(playersMap[trackName]);
  }
}

void AudioTools::makePluginPlay(const juce::String& trackName, const juce::MidiMessage& message)
{
  if(pluginsMap.find(trackName) == pluginsMap.end())
    return;

  const juce::ScopedLock sL(criticalSection);
  playersMap[trackName]->handleIncomingMidiMessage(nullptr, message);
}

void AudioTools::disableAudioProcessing()
{
  for (auto plugin = pluginsMap.begin(); plugin != pluginsMap.end(); ++plugin)
  {
    plugin->second->suspendProcessing(true);
    plugin->second->reset();
  }

}

void AudioTools::enableAudioProcessing()
{
  for (auto plugin = pluginsMap.begin(); plugin != pluginsMap.end(); ++plugin)
    plugin->second->suspendProcessing(false);

}
}; // namespace JuceModule