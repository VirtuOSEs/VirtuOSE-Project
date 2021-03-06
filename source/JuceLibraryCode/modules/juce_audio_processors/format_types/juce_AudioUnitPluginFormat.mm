/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#if JUCE_PLUGINHOST_AU && JUCE_MAC

} // (juce namespace)

#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUCocoaUIView.h>
#include <CoreAudioKit/AUGenericView.h>
#include <AudioToolbox/AudioUnitUtilities.h>
#include <CoreMIDI/MIDIServices.h>

#if JUCE_SUPPORT_CARBON
 #include <AudioUnit/AudioUnitCarbonView.h>
#endif

namespace juce
{

#include "../../juce_audio_devices/native/juce_MidiDataConcatenator.h"

#if JUCE_SUPPORT_CARBON
 #include "../../juce_gui_extra/native/juce_mac_CarbonViewWrapperComponent.h"
#endif

// Change this to disable logging of various activities
#ifndef AU_LOGGING
 #define AU_LOGGING 1
#endif

#if AU_LOGGING
 #define JUCE_AU_LOG(a) Logger::writeToLog(a);
#else
 #define JUCE_AU_LOG(a)
#endif

namespace AudioUnitFormatHelpers
{
    static int insideCallback = 0;

    String osTypeToString (OSType type)
    {
        const juce_wchar s[4] = { (juce_wchar) ((type >> 24) & 0xff),
                                  (juce_wchar) ((type >> 16) & 0xff),
                                  (juce_wchar) ((type >> 8) & 0xff),
                                  (juce_wchar) (type & 0xff) };
        return String (s, 4);
    }

    OSType stringToOSType (String s)
    {
        if (s.trim().length() >= 4) // (to avoid trimming leading spaces)
            s = s.trim();

        s += "    ";

        return (((OSType) (unsigned char) s[0]) << 24)
             | (((OSType) (unsigned char) s[1]) << 16)
             | (((OSType) (unsigned char) s[2]) << 8)
             |  ((OSType) (unsigned char) s[3]);
    }

    static const char* auIdentifierPrefix = "AudioUnit:";

    String createPluginIdentifier (const AudioComponentDescription& desc)
    {
        jassert (osTypeToString ('abcd') == "abcd"); // agh, must have got the endianness wrong..
        jassert (stringToOSType ("abcd") == (OSType) 'abcd'); // ditto

        String s (auIdentifierPrefix);

        if (desc.componentType == kAudioUnitType_MusicDevice)
            s << "Synths/";
        else if (desc.componentType == kAudioUnitType_MusicEffect
                  || desc.componentType == kAudioUnitType_Effect)
            s << "Effects/";
        else if (desc.componentType == kAudioUnitType_Generator)
            s << "Generators/";
        else if (desc.componentType == kAudioUnitType_Panner)
            s << "Panners/";

        s << osTypeToString (desc.componentType) << ","
          << osTypeToString (desc.componentSubType) << ","
          << osTypeToString (desc.componentManufacturer);

        return s;
    }

    void getNameAndManufacturer (AudioComponent comp, String& name, String& manufacturer)
    {
        CFStringRef cfName;
        if (AudioComponentCopyName (comp, &cfName) == noErr)
        {
            name = String::fromCFString (cfName);
            CFRelease (cfName);
        }

        if (name.containsChar (':'))
        {
            manufacturer = name.upToFirstOccurrenceOf (":", false, false).trim();
            name         = name.fromFirstOccurrenceOf (":", false, false).trim();
        }

        if (name.isEmpty())
            name = "<Unknown>";
    }

    bool getComponentDescFromIdentifier (const String& fileOrIdentifier, AudioComponentDescription& desc,
                                         String& name, String& version, String& manufacturer)
    {
        if (fileOrIdentifier.startsWithIgnoreCase (auIdentifierPrefix))
        {
            String s (fileOrIdentifier.substring (jmax (fileOrIdentifier.lastIndexOfChar (':'),
                                                        fileOrIdentifier.lastIndexOfChar ('/')) + 1));

            StringArray tokens;
            tokens.addTokens (s, ",", String::empty);
            tokens.removeEmptyStrings();

            if (tokens.size() == 3)
            {
                zerostruct (desc);
                desc.componentType         = stringToOSType (tokens[0]);
                desc.componentSubType      = stringToOSType (tokens[1]);
                desc.componentManufacturer = stringToOSType (tokens[2]);

                if (AudioComponent comp = AudioComponentFindNext (0, &desc))
                {
                    getNameAndManufacturer (comp, name, manufacturer);

                    if (manufacturer.isEmpty())
                        manufacturer = tokens[2];

                    if (version.isEmpty())
                    {
                        UInt32 versionNum;

                        if (AudioComponentGetVersion (comp, &versionNum) == noErr)
                        {
                            version << (int) (versionNum >> 16) << "."
                                    << (int) ((versionNum >> 8) & 0xff) << "."
                                    << (int) (versionNum & 0xff);
                        }
                    }

                    return true;
                }
            }
        }

        return false;
    }

    bool getComponentDescFromFile (const String& fileOrIdentifier, AudioComponentDescription& desc,
                                   String& name, String& version, String& manufacturer)
    {
        zerostruct (desc);

        const File file (fileOrIdentifier);
        if (! file.hasFileExtension (".component"))
            return false;

        const char* const utf8 = fileOrIdentifier.toUTF8();

        if (CFURLRef url = CFURLCreateFromFileSystemRepresentation (0, (const UInt8*) utf8,
                                                                    strlen (utf8), file.isDirectory()))
        {
            CFBundleRef bundleRef = CFBundleCreate (kCFAllocatorDefault, url);
            CFRelease (url);

            if (bundleRef != 0)
            {
                CFTypeRef bundleName = CFBundleGetValueForInfoDictionaryKey (bundleRef, CFSTR("CFBundleName"));

                if (bundleName != 0 && CFGetTypeID (bundleName) == CFStringGetTypeID())
                    name = String::fromCFString ((CFStringRef) bundleName);

                if (name.isEmpty())
                    name = file.getFileNameWithoutExtension();

                CFTypeRef versionString = CFBundleGetValueForInfoDictionaryKey (bundleRef, CFSTR("CFBundleVersion"));

                if (versionString != 0 && CFGetTypeID (versionString) == CFStringGetTypeID())
                    version = String::fromCFString ((CFStringRef) versionString);

                CFTypeRef manuString = CFBundleGetValueForInfoDictionaryKey (bundleRef, CFSTR("CFBundleGetInfoString"));

                if (manuString != 0 && CFGetTypeID (manuString) == CFStringGetTypeID())
                    manufacturer = String::fromCFString ((CFStringRef) manuString);

                const short resFileId = CFBundleOpenBundleResourceMap (bundleRef);
                UseResFile (resFileId);

                for (ResourceIndex i = 1; i <= Count1Resources ('thng'); ++i)
                {
                    if (Handle h = Get1IndResource ('thng', i))
                    {
                        HLock (h);
                        const uint32* const types = (const uint32*) *h;

                        if (types[0] == kAudioUnitType_MusicDevice
                             || types[0] == kAudioUnitType_MusicEffect
                             || types[0] == kAudioUnitType_Effect
                             || types[0] == kAudioUnitType_Generator
                             || types[0] == kAudioUnitType_Panner)
                        {
                            desc.componentType = types[0];
                            desc.componentSubType = types[1];
                            desc.componentManufacturer = types[2];

                            if (AudioComponent comp = AudioComponentFindNext (0, &desc))
                                getNameAndManufacturer (comp, name, manufacturer);

                            break;
                        }

                        HUnlock (h);
                        ReleaseResource (h);
                    }
                }

                CFBundleCloseBundleResourceMap (bundleRef, resFileId);
                CFRelease (bundleRef);
            }
        }

        return desc.componentType != 0 && desc.componentSubType != 0;
    }

    const char* getCategory (OSType type) noexcept
    {
        switch (type)
        {
            case kAudioUnitType_Effect:
            case kAudioUnitType_MusicEffect:    return "Effect";
            case kAudioUnitType_MusicDevice:    return "Synth";
            case kAudioUnitType_Generator:      return "Generator";
            case kAudioUnitType_Panner:         return "Panner";
            default: break;
        }

        return nullptr;
    }
}

//==============================================================================
class AudioUnitPluginWindowCarbon;
class AudioUnitPluginWindowCocoa;

//==============================================================================
class AudioUnitPluginInstance     : public AudioPluginInstance
{
public:
    AudioUnitPluginInstance (const String& fileOrId)
        : fileOrIdentifier (fileOrId),
          wantsMidiMessages (false),
          producesMidiMessages (false),
          wasPlaying (false),
          prepared (false),
          currentBuffer (nullptr),
          numInputBusChannels (0),
          numOutputBusChannels (0),
          numInputBusses (0),
          numOutputBusses (0),
          audioUnit (nullptr),
          parameterListenerRef (0),
          midiConcatenator (2048)
    {
        using namespace AudioUnitFormatHelpers;

        ++insideCallback;

        JUCE_AU_LOG ("Opening AU: " + fileOrIdentifier);

        if (getComponentDescFromIdentifier (fileOrIdentifier, componentDesc, pluginName, version, manufacturer)
             || getComponentDescFromFile (fileOrIdentifier, componentDesc, pluginName, version, manufacturer))
        {
            if (AudioComponent comp = AudioComponentFindNext (0, &componentDesc))
            {
                AudioComponentInstanceNew (comp, &audioUnit);

                wantsMidiMessages = componentDesc.componentType == kAudioUnitType_MusicDevice
                                 || componentDesc.componentType == kAudioUnitType_MusicEffect;
            }
        }

        --insideCallback;
    }

    ~AudioUnitPluginInstance()
    {
        const ScopedLock sl (lock);

        jassert (AudioUnitFormatHelpers::insideCallback == 0);

        if (parameterListenerRef != 0)
        {
            AUListenerDispose (parameterListenerRef);
            parameterListenerRef = 0;
        }

        if (audioUnit != nullptr)
        {
            if (prepared)
                releaseResources();

            AudioComponentInstanceDispose (audioUnit);
            audioUnit = nullptr;
        }
    }

    void initialise()
    {
        refreshParameterList();
        updateNumChannels();
        producesMidiMessages = canProduceMidiOutput();
        setPluginCallbacks();
        setPlayConfigDetails (numInputBusChannels * numInputBusses,
                              numOutputBusChannels * numOutputBusses, 0, 0);
        setLatencySamples (0);
    }

    //==============================================================================
    // AudioPluginInstance methods:

    void fillInPluginDescription (PluginDescription& desc) const
    {
        desc.name = pluginName;
        desc.descriptiveName = pluginName;
        desc.fileOrIdentifier = AudioUnitFormatHelpers::createPluginIdentifier (componentDesc);
        desc.uid = ((int) componentDesc.componentType)
                    ^ ((int) componentDesc.componentSubType)
                    ^ ((int) componentDesc.componentManufacturer);
        desc.lastFileModTime = Time();
        desc.pluginFormatName = "AudioUnit";
        desc.category = AudioUnitFormatHelpers::getCategory (componentDesc.componentType);
        desc.manufacturerName = manufacturer;
        desc.version = version;
        desc.numInputChannels = getNumInputChannels();
        desc.numOutputChannels = getNumOutputChannels();
        desc.isInstrument = (componentDesc.componentType == kAudioUnitType_MusicDevice);
    }

    void* getPlatformSpecificData()             { return audioUnit; }
    const String getName() const                { return pluginName; }

    bool silenceInProducesSilenceOut() const
    {
        return getTailLengthSeconds() <= 0;
    }

    double getTailLengthSeconds() const
    {
        Float64 tail = 0;
        UInt32 tailSize = sizeof (tail);

        if (audioUnit != nullptr)
            AudioUnitGetProperty (audioUnit, kAudioUnitProperty_TailTime, kAudioUnitScope_Global,
                                  0, &tail, &tailSize);

        return tail;
    }

    bool acceptsMidi() const                    { return wantsMidiMessages; }
    bool producesMidi() const                   { return producesMidiMessages; }

    //==============================================================================
    // AudioProcessor methods:

    void prepareToPlay (double newSampleRate, int estimatedSamplesPerBlock)
    {
        if (audioUnit != nullptr)
        {
            releaseResources();
            updateNumChannels();

            Float64 sampleRateIn = 0, sampleRateOut = 0;
            UInt32 sampleRateSize = sizeof (sampleRateIn);
            const Float64 sr = newSampleRate;

            for (int i = 0; i < numInputBusses; ++i)
            {
                AudioUnitGetProperty (audioUnit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Input, i, &sampleRateIn, &sampleRateSize);

                if (sampleRateIn != sr)
                    AudioUnitSetProperty (audioUnit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Input, i, &sr, sizeof (sr));
            }

            for (int i = 0; i < numOutputBusses; ++i)
            {
                AudioUnitGetProperty (audioUnit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Output, i, &sampleRateOut, &sampleRateSize);

                if (sampleRateOut != sr)
                    AudioUnitSetProperty (audioUnit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Output, i, &sr, sizeof (sr));
            }

            UInt32 frameSize = (UInt32) estimatedSamplesPerBlock;
            AudioUnitSetProperty (audioUnit, kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, 0,
                                  &frameSize, sizeof (frameSize));

            setPlayConfigDetails (numInputBusChannels * numInputBusses,
                                  numOutputBusChannels * numOutputBusses,
                                  newSampleRate, estimatedSamplesPerBlock);

            Float64 latencySecs = 0.0;
            UInt32 latencySize = sizeof (latencySecs);
            AudioUnitGetProperty (audioUnit, kAudioUnitProperty_Latency, kAudioUnitScope_Global,
                                  0, &latencySecs, &latencySize);

            setLatencySamples (roundToInt (latencySecs * newSampleRate));

            {
                AudioStreamBasicDescription stream;
                zerostruct (stream); // (can't use "= { 0 }" on this object because it's typedef'ed as a C struct)
                stream.mSampleRate       = sr;
                stream.mFormatID         = kAudioFormatLinearPCM;
                stream.mFormatFlags      = kAudioFormatFlagsNativeFloatPacked | kAudioFormatFlagIsNonInterleaved | kAudioFormatFlagsNativeEndian;
                stream.mFramesPerPacket  = 1;
                stream.mBytesPerPacket   = 4;
                stream.mBytesPerFrame    = 4;
                stream.mBitsPerChannel   = 32;
                stream.mChannelsPerFrame = numInputBusChannels;

                for (int i = 0; i < numInputBusses; ++i)
                    AudioUnitSetProperty (audioUnit, kAudioUnitProperty_StreamFormat,
                                          kAudioUnitScope_Input, i, &stream, sizeof (stream));

                stream.mChannelsPerFrame = numOutputBusChannels;

                for (int i = 0; i < numOutputBusses; ++i)
                    AudioUnitSetProperty (audioUnit, kAudioUnitProperty_StreamFormat,
                                          kAudioUnitScope_Output, i, &stream, sizeof (stream));
            }

            if (numOutputBusses != 0 && numOutputBusChannels != 0)
                outputBufferList.calloc (numOutputBusses, getAudioBufferSizeInBytes());

            zerostruct (timeStamp);
            timeStamp.mSampleTime = 0;
            timeStamp.mHostTime = AudioGetCurrentHostTime();
            timeStamp.mFlags = kAudioTimeStampSampleTimeValid | kAudioTimeStampHostTimeValid;

            currentBuffer = nullptr;
            wasPlaying = false;

            resetBusses();

            prepared = (AudioUnitInitialize (audioUnit) == noErr);
            jassert (prepared);
        }
    }

    void releaseResources()
    {
        if (prepared)
        {
            AudioUnitUninitialize (audioUnit);
            resetBusses();
            AudioUnitReset (audioUnit, kAudioUnitScope_Global, 0);

            outputBufferList.free();
            currentBuffer = nullptr;
            prepared = false;
        }

        incomingMidi.clear();
    }

    void resetBusses()
    {
        for (int i = 0; i < numInputBusses; ++i)   AudioUnitReset (audioUnit, kAudioUnitScope_Input, i);
        for (int i = 0; i < numOutputBusses; ++i)  AudioUnitReset (audioUnit, kAudioUnitScope_Output, i);
    }

    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
    {
        const int numSamples = buffer.getNumSamples();

        if (prepared)
        {
            timeStamp.mHostTime = AudioGetCurrentHostTime();

            for (int i = 0; i < numOutputBusses; ++i)
            {
                AudioBufferList* const abl = getAudioBufferListForBus(i);
                abl->mNumberBuffers = numOutputBusChannels;

                for (int j = 0; j < numOutputBusChannels; ++j)
                {
                    abl->mBuffers[j].mNumberChannels = 1;
                    abl->mBuffers[j].mDataByteSize = sizeof (float) * numSamples;
                    abl->mBuffers[j].mData = buffer.getSampleData (i * numOutputBusChannels + j, 0);
                }
            }

            currentBuffer = &buffer;

            if (wantsMidiMessages)
            {
                const uint8* midiEventData;
                int midiEventSize, midiEventPosition;

                for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (midiEventData, midiEventSize, midiEventPosition);)
                {
                    if (midiEventSize <= 3)
                        MusicDeviceMIDIEvent (audioUnit,
                                              midiEventData[0], midiEventData[1], midiEventData[2],
                                              midiEventPosition);
                    else
                        MusicDeviceSysEx (audioUnit, midiEventData, midiEventSize);
                }

                midiMessages.clear();
            }


            for (int i = 0; i < numOutputBusses; ++i)
            {
                AudioUnitRenderActionFlags flags = 0;
                AudioUnitRender (audioUnit, &flags, &timeStamp, i, numSamples, getAudioBufferListForBus (i));
            }

            timeStamp.mSampleTime += numSamples;
        }
        else
        {
            // Plugin not working correctly, so just bypass..
            for (int i = 0; i < getNumOutputChannels(); ++i)
                buffer.clear (i, 0, buffer.getNumSamples());
        }

        if (producesMidiMessages)
        {
            const ScopedLock sl (midiInLock);
            midiMessages.swapWith (incomingMidi);
            incomingMidi.clear();
        }
    }

    //==============================================================================
    bool hasEditor() const                  { return true; }
    AudioProcessorEditor* createEditor();

    //==============================================================================
    const String getInputChannelName (int index) const
    {
        if (isPositiveAndBelow (index, getNumInputChannels()))
            return "Input " + String (index + 1);

        return String::empty;
    }

    const String getOutputChannelName (int index) const
    {
        if (isPositiveAndBelow (index, getNumOutputChannels()))
            return "Output " + String (index + 1);

        return String::empty;
    }

    bool isInputChannelStereoPair (int index) const    { return isPositiveAndBelow (index, getNumInputChannels()); }
    bool isOutputChannelStereoPair (int index) const   { return isPositiveAndBelow (index, getNumOutputChannels()); }

    //==============================================================================
    int getNumParameters()              { return parameters.size(); }

    float getParameter (int index)
    {
        const ScopedLock sl (lock);

        AudioUnitParameterValue value = 0;

        if (audioUnit != nullptr)
        {
            if (const ParamInfo* p = parameters[index])
            {
                AudioUnitGetParameter (audioUnit,
                                       p->paramID,
                                       kAudioUnitScope_Global, 0,
                                       &value);

                value = (value - p->minValue) / (p->maxValue - p->minValue);
            }
        }

        return value;
    }

    void setParameter (int index, float newValue)
    {
        const ScopedLock sl (lock);

        if (audioUnit != nullptr)
        {
            if (const ParamInfo* p = parameters[index])
            {
                AudioUnitSetParameter (audioUnit, p->paramID, kAudioUnitScope_Global, 0,
                                       p->minValue + (p->maxValue - p->minValue) * newValue, 0);

                sendParameterChangeEvent (index);
            }
        }
    }

    void sendParameterChangeEvent (int index)
    {
        jassert (audioUnit != nullptr);

        const ParamInfo& p = *parameters.getUnchecked (index);

        AudioUnitEvent ev;
        ev.mEventType                        = kAudioUnitEvent_ParameterValueChange;
        ev.mArgument.mParameter.mAudioUnit   = audioUnit;
        ev.mArgument.mParameter.mParameterID = p.paramID;
        ev.mArgument.mParameter.mScope       = kAudioUnitScope_Global;
        ev.mArgument.mParameter.mElement     = 0;

        AUEventListenerNotify (nullptr, nullptr, &ev);
    }

    void sendAllParametersChangedEvents()
    {
        for (int i = 0; i < parameters.size(); ++i)
            sendParameterChangeEvent (i);
    }

    const String getParameterName (int index)
    {
        if (const ParamInfo* p = parameters[index])
            return p->name;

        return String::empty;
    }

    const String getParameterText (int index)   { return String (getParameter (index)); }

    bool isParameterAutomatable (int index) const
    {
        if (const ParamInfo* p = parameters[index])
            return p->automatable;

        return false;
    }

    //==============================================================================
    int getNumPrograms()
    {
        CFArrayRef presets;
        UInt32 sz = sizeof (CFArrayRef);
        int num = 0;

        if (AudioUnitGetProperty (audioUnit, kAudioUnitProperty_FactoryPresets,
                                  kAudioUnitScope_Global, 0, &presets, &sz) == noErr)
        {
            num = (int) CFArrayGetCount (presets);
            CFRelease (presets);
        }

        return num;
    }

    int getCurrentProgram()
    {
        AUPreset current;
        current.presetNumber = 0;
        UInt32 sz = sizeof (AUPreset);

        AudioUnitGetProperty (audioUnit, kAudioUnitProperty_PresentPreset,
                              kAudioUnitScope_Global, 0, &current, &sz);

        return current.presetNumber;
    }

    void setCurrentProgram (int newIndex)
    {
        AUPreset current;
        current.presetNumber = newIndex;
        current.presetName = CFSTR("");

        AudioUnitSetProperty (audioUnit, kAudioUnitProperty_PresentPreset,
                              kAudioUnitScope_Global, 0, &current, sizeof (AUPreset));

        sendAllParametersChangedEvents();
    }

    const String getProgramName (int index)
    {
        String s;
        CFArrayRef presets;
        UInt32 sz = sizeof (CFArrayRef);

        if (AudioUnitGetProperty (audioUnit, kAudioUnitProperty_FactoryPresets,
                                  kAudioUnitScope_Global, 0, &presets, &sz) == noErr)
        {
            for (CFIndex i = 0; i < CFArrayGetCount (presets); ++i)
            {
                if (const AUPreset* p = (const AUPreset*) CFArrayGetValueAtIndex (presets, i))
                {
                    if (p->presetNumber == index)
                    {
                        s = String::fromCFString (p->presetName);
                        break;
                    }
                }
            }

            CFRelease (presets);
        }

        return s;
    }

    void changeProgramName (int index, const String& newName)
    {
        jassertfalse; // xxx not implemented!
    }

    //==============================================================================
    void getStateInformation (MemoryBlock& destData)
    {
        getCurrentProgramStateInformation (destData);
    }

    void getCurrentProgramStateInformation (MemoryBlock& destData)
    {
        CFPropertyListRef propertyList = 0;
        UInt32 sz = sizeof (CFPropertyListRef);

        if (AudioUnitGetProperty (audioUnit,
                                  kAudioUnitProperty_ClassInfo,
                                  kAudioUnitScope_Global,
                                  0, &propertyList, &sz) == noErr)
        {
            CFWriteStreamRef stream = CFWriteStreamCreateWithAllocatedBuffers (kCFAllocatorDefault, kCFAllocatorDefault);
            CFWriteStreamOpen (stream);

            CFIndex bytesWritten = CFPropertyListWriteToStream (propertyList, stream, kCFPropertyListBinaryFormat_v1_0, 0);
            CFWriteStreamClose (stream);

            CFDataRef data = (CFDataRef) CFWriteStreamCopyProperty (stream, kCFStreamPropertyDataWritten);

            destData.setSize (bytesWritten);
            destData.copyFrom (CFDataGetBytePtr (data), 0, destData.getSize());
            CFRelease (data);

            CFRelease (stream);
            CFRelease (propertyList);
        }
    }

    void setStateInformation (const void* data, int sizeInBytes)
    {
        setCurrentProgramStateInformation (data, sizeInBytes);
    }

    void setCurrentProgramStateInformation (const void* data, int sizeInBytes)
    {
        CFReadStreamRef stream = CFReadStreamCreateWithBytesNoCopy (kCFAllocatorDefault,
                                                                    (const UInt8*) data,
                                                                    sizeInBytes,
                                                                    kCFAllocatorNull);
        CFReadStreamOpen (stream);

        CFPropertyListFormat format = kCFPropertyListBinaryFormat_v1_0;
        CFPropertyListRef propertyList = CFPropertyListCreateFromStream (kCFAllocatorDefault,
                                                                         stream,
                                                                         0,
                                                                         kCFPropertyListImmutable,
                                                                         &format,
                                                                         0);
        CFRelease (stream);

        if (propertyList != 0)
        {
            AudioUnitSetProperty (audioUnit,
                                  kAudioUnitProperty_ClassInfo,
                                  kAudioUnitScope_Global,
                                  0, &propertyList, sizeof (propertyList));

            sendAllParametersChangedEvents();
        }
    }

    void refreshParameterList()
    {
        parameters.clear();

        if (audioUnit != nullptr)
        {
            UInt32 paramListSize = 0;
            AudioUnitGetProperty (audioUnit, kAudioUnitProperty_ParameterList, kAudioUnitScope_Global,
                                  0, 0, &paramListSize);

            if (paramListSize > 0)
            {
                const size_t numParams = paramListSize / sizeof (int);

                HeapBlock<UInt32> ids;
                ids.calloc (numParams);

                AudioUnitGetProperty (audioUnit, kAudioUnitProperty_ParameterList, kAudioUnitScope_Global,
                                      0, ids, &paramListSize);

                for (int i = 0; i < numParams; ++i)
                {
                    AudioUnitParameterInfo info;
                    UInt32 sz = sizeof (info);

                    if (AudioUnitGetProperty (audioUnit,
                                              kAudioUnitProperty_ParameterInfo,
                                              kAudioUnitScope_Global,
                                              ids[i], &info, &sz) == noErr)
                    {
                        ParamInfo* const param = new ParamInfo();
                        parameters.add (param);
                        param->paramID = ids[i];
                        param->minValue = info.minValue;
                        param->maxValue = info.maxValue;
                        param->automatable = (info.flags & kAudioUnitParameterFlag_NonRealTime) == 0;

                        if ((info.flags & kAudioUnitParameterFlag_HasCFNameString) != 0)
                        {
                            param->name = String::fromCFString (info.cfNameString);

                            if ((info.flags & kAudioUnitParameterFlag_CFNameRelease) != 0)
                                CFRelease (info.cfNameString);
                        }
                        else
                        {
                            param->name = String (info.name, sizeof (info.name));
                        }
                    }
                }
            }
        }
    }

    void handleIncomingMidiMessage (void*, const MidiMessage& message)
    {
        const ScopedLock sl (midiInLock);
        incomingMidi.addEvent (message, 0);
    }

    void handlePartialSysexMessage (void*, const uint8*, int, double) {}

private:
    //==============================================================================
    friend class AudioUnitPluginWindowCarbon;
    friend class AudioUnitPluginWindowCocoa;
    friend class AudioUnitPluginFormat;

    AudioComponentDescription componentDesc;
    String pluginName, manufacturer, version;
    String fileOrIdentifier;
    CriticalSection lock;
    bool wantsMidiMessages, producesMidiMessages, wasPlaying, prepared;

    HeapBlock <AudioBufferList> outputBufferList;
    AudioTimeStamp timeStamp;
    AudioSampleBuffer* currentBuffer;
    int numInputBusChannels, numOutputBusChannels, numInputBusses, numOutputBusses;

    AudioUnit audioUnit;
    AUParameterListenerRef parameterListenerRef;

    struct ParamInfo
    {
        UInt32 paramID;
        String name;
        AudioUnitParameterValue minValue, maxValue;
        bool automatable;
    };

    OwnedArray <ParamInfo> parameters;

    MidiDataConcatenator midiConcatenator;
    CriticalSection midiInLock;
    MidiBuffer incomingMidi;

    void setPluginCallbacks()
    {
        if (audioUnit != nullptr)
        {
            {
                AURenderCallbackStruct info;
                zerostruct (info); // (can't use "= { 0 }" on this object because it's typedef'ed as a C struct)

                info.inputProcRefCon = this;
                info.inputProc = renderGetInputCallback;

                for (int i = 0; i < numInputBusses; ++i)
                    AudioUnitSetProperty (audioUnit, kAudioUnitProperty_SetRenderCallback,
                                          kAudioUnitScope_Input, i, &info, sizeof (info));
            }

            if (producesMidiMessages)
            {
                AUMIDIOutputCallbackStruct info;
                zerostruct (info);

                info.userData = this;
                info.midiOutputCallback = renderMidiOutputCallback;

                producesMidiMessages = (AudioUnitSetProperty (audioUnit, kAudioUnitProperty_MIDIOutputCallback,
                                                              kAudioUnitScope_Global, 0, &info, sizeof (info)) == noErr);
            }

            {
                HostCallbackInfo info;
                zerostruct (info);

                info.hostUserData = this;
                info.beatAndTempoProc = getBeatAndTempoCallback;
                info.musicalTimeLocationProc = getMusicalTimeLocationCallback;
                info.transportStateProc = getTransportStateCallback;

                AudioUnitSetProperty (audioUnit, kAudioUnitProperty_HostCallbacks,
                                      kAudioUnitScope_Global, 0, &info, sizeof (info));
            }

            AUListenerCreate (parameterListenerCallback, this, nullptr, nullptr, 0, &parameterListenerRef);

            for (int i = 0; i < parameters.size(); ++i)
            {
                const ParamInfo& p = *parameters.getUnchecked(i);

                AudioUnitParameter paramToAdd;
                paramToAdd.mAudioUnit = audioUnit;
                paramToAdd.mParameterID = p.paramID;
                paramToAdd.mScope = kAudioUnitScope_Global;
                paramToAdd.mElement = 0;

                AUListenerAddParameter (parameterListenerRef, nullptr, &paramToAdd);
            }
        }
    }

    void parameterChanged (const AudioUnitParameter* param, AudioUnitParameterValue newValue)
    {
        for (int i = 0; i < parameters.size(); ++i)
        {
            const ParamInfo& p = *parameters.getUnchecked(i);

            if (p.paramID == param->mParameterID)
            {
                sendParamChangeMessageToListeners (i, (newValue - p.minValue) / (p.maxValue - p.minValue));
                break;
            }
        }
    }

    static void parameterListenerCallback (void* userData, void*, const AudioUnitParameter* param, AudioUnitParameterValue newValue)
    {
        ((AudioUnitPluginInstance*) userData)->parameterChanged (param, newValue);
    }

    //==============================================================================
    OSStatus renderGetInput (AudioUnitRenderActionFlags* ioActionFlags,
                             const AudioTimeStamp* inTimeStamp,
                             UInt32 inBusNumber,
                             UInt32 inNumberFrames,
                             AudioBufferList* ioData) const
    {
        if (currentBuffer != nullptr)
        {
            jassert (inNumberFrames == (UInt32) currentBuffer->getNumSamples()); // if this ever happens, might need to add extra handling

            for (UInt32 i = 0; i < ioData->mNumberBuffers; ++i)
            {
                const int bufferChannel = inBusNumber * numInputBusChannels + i;

                if (bufferChannel < currentBuffer->getNumChannels())
                {
                    memcpy (ioData->mBuffers[i].mData,
                            currentBuffer->getSampleData (bufferChannel, 0),
                            sizeof (float) * inNumberFrames);
                }
                else
                {
                    zeromem (ioData->mBuffers[i].mData,
                             sizeof (float) * inNumberFrames);
                }
            }
        }

        return noErr;
    }

    OSStatus renderMidiOutput (const MIDIPacketList* pktlist)
    {
        if (pktlist != nullptr && pktlist->numPackets)
        {
            const double time = Time::getMillisecondCounterHiRes() * 0.001;
            const MIDIPacket* packet = &pktlist->packet[0];

            for (UInt32 i = 0; i < pktlist->numPackets; ++i)
            {
                midiConcatenator.pushMidiData (packet->data, (int) packet->length, time, (void*) nullptr, *this);
                packet = MIDIPacketNext (packet);
            }
        }

        return noErr;
    }

    OSStatus getBeatAndTempo (Float64* outCurrentBeat, Float64* outCurrentTempo) const
    {
        AudioPlayHead* const ph = getPlayHead();
        AudioPlayHead::CurrentPositionInfo result;

        if (ph != nullptr && ph->getCurrentPosition (result))
        {
            if (outCurrentBeat  != nullptr)    *outCurrentBeat  = result.ppqPosition;
            if (outCurrentTempo != nullptr)    *outCurrentTempo = result.bpm;
        }
        else
        {
            if (outCurrentBeat  != nullptr)    *outCurrentBeat  = 0;
            if (outCurrentTempo != nullptr)    *outCurrentTempo = 120.0;
        }

        return noErr;
    }

    OSStatus getMusicalTimeLocation (UInt32* outDeltaSampleOffsetToNextBeat, Float32* outTimeSig_Numerator,
                                     UInt32* outTimeSig_Denominator, Float64* outCurrentMeasureDownBeat) const
    {
        AudioPlayHead* const ph = getPlayHead();
        AudioPlayHead::CurrentPositionInfo result;

        if (ph != nullptr && ph->getCurrentPosition (result))
        {
            if (outTimeSig_Numerator != nullptr)            *outTimeSig_Numerator   = result.timeSigNumerator;
            if (outTimeSig_Denominator != nullptr)          *outTimeSig_Denominator = result.timeSigDenominator;
            if (outDeltaSampleOffsetToNextBeat != nullptr)  *outDeltaSampleOffsetToNextBeat = 0; //xxx
            if (outCurrentMeasureDownBeat != nullptr)       *outCurrentMeasureDownBeat = result.ppqPositionOfLastBarStart; //xxx wrong
        }
        else
        {
            if (outDeltaSampleOffsetToNextBeat != nullptr)  *outDeltaSampleOffsetToNextBeat = 0;
            if (outTimeSig_Numerator != nullptr)            *outTimeSig_Numerator = 4;
            if (outTimeSig_Denominator != nullptr)          *outTimeSig_Denominator = 4;
            if (outCurrentMeasureDownBeat != nullptr)       *outCurrentMeasureDownBeat = 0;
        }

        return noErr;
    }

    OSStatus getTransportState (Boolean* outIsPlaying, Boolean* outTransportStateChanged,
                                Float64* outCurrentSampleInTimeLine, Boolean* outIsCycling,
                                Float64* outCycleStartBeat, Float64* outCycleEndBeat)
    {
        AudioPlayHead* const ph = getPlayHead();
        AudioPlayHead::CurrentPositionInfo result;

        if (ph != nullptr && ph->getCurrentPosition (result))
        {
            if (outIsPlaying != nullptr)
                *outIsPlaying = result.isPlaying;

            if (outTransportStateChanged != nullptr)
            {
                *outTransportStateChanged = result.isPlaying != wasPlaying;
                wasPlaying = result.isPlaying;
            }

            if (outCurrentSampleInTimeLine != nullptr)
                *outCurrentSampleInTimeLine = (Float64) result.timeInSamples;

            if (outIsCycling != nullptr)        *outIsCycling = false;
            if (outCycleStartBeat != nullptr)   *outCycleStartBeat = 0;
            if (outCycleEndBeat != nullptr)     *outCycleEndBeat = 0;
        }
        else
        {
            if (outIsPlaying != nullptr)                *outIsPlaying = false;
            if (outTransportStateChanged != nullptr)    *outTransportStateChanged = false;
            if (outCurrentSampleInTimeLine != nullptr)  *outCurrentSampleInTimeLine = 0;
            if (outIsCycling != nullptr)                *outIsCycling = false;
            if (outCycleStartBeat != nullptr)           *outCycleStartBeat = 0;
            if (outCycleEndBeat != nullptr)             *outCycleEndBeat = 0;
        }

        return noErr;
    }

    //==============================================================================
    static OSStatus renderGetInputCallback (void* inRefCon, AudioUnitRenderActionFlags* ioActionFlags,
                                            const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber,
                                            UInt32 inNumberFrames, AudioBufferList* ioData)
    {
        return static_cast <AudioUnitPluginInstance*> (inRefCon)
                    ->renderGetInput (ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, ioData);
    }

    static OSStatus renderMidiOutputCallback (void* userData, const AudioTimeStamp* timeStamp, UInt32 midiOutNum,
                                              const struct MIDIPacketList* pktlist)
    {
        return static_cast <AudioUnitPluginInstance*> (userData)->renderMidiOutput (pktlist);
    }

    static OSStatus getBeatAndTempoCallback (void* inHostUserData, Float64* outCurrentBeat, Float64* outCurrentTempo)
    {
        return static_cast <AudioUnitPluginInstance*> (inHostUserData)
                    ->getBeatAndTempo (outCurrentBeat, outCurrentTempo);
    }

    static OSStatus getMusicalTimeLocationCallback (void* inHostUserData, UInt32* outDeltaSampleOffsetToNextBeat,
                                                    Float32* outTimeSig_Numerator, UInt32* outTimeSig_Denominator,
                                                    Float64* outCurrentMeasureDownBeat)
    {
        return static_cast <AudioUnitPluginInstance*> (inHostUserData)
                    ->getMusicalTimeLocation (outDeltaSampleOffsetToNextBeat, outTimeSig_Numerator,
                                              outTimeSig_Denominator, outCurrentMeasureDownBeat);
    }

    static OSStatus getTransportStateCallback (void* inHostUserData, Boolean* outIsPlaying, Boolean* outTransportStateChanged,
                                               Float64* outCurrentSampleInTimeLine, Boolean* outIsCycling,
                                               Float64* outCycleStartBeat, Float64* outCycleEndBeat)
    {
        return static_cast <AudioUnitPluginInstance*> (inHostUserData)
                    ->getTransportState (outIsPlaying, outTransportStateChanged, outCurrentSampleInTimeLine,
                                         outIsCycling, outCycleStartBeat, outCycleEndBeat);
    }

    //==============================================================================
    size_t getAudioBufferSizeInBytes() const noexcept
    {
        return offsetof (AudioBufferList, mBuffers) + (sizeof (AudioBuffer) * numOutputBusChannels);
    }

    AudioBufferList* getAudioBufferListForBus (int busIndex) const noexcept
    {
        return addBytesToPointer (outputBufferList.getData(), getAudioBufferSizeInBytes() * busIndex);
    }

    int getElementCount (AudioUnitScope scope) const noexcept
    {
        UInt32 count;
        UInt32 countSize = sizeof (count);

        if (AudioUnitGetProperty (audioUnit, kAudioUnitProperty_ElementCount, scope, 0, &count, &countSize) != noErr
             || countSize == 0)
            count = 1;

        return (int) count;
    }

    void updateNumChannels()
    {
        numInputBusses = getElementCount (kAudioUnitScope_Input);
        numOutputBusses = getElementCount (kAudioUnitScope_Output);

        AUChannelInfo supportedChannels [128];
        UInt32 supportedChannelsSize = sizeof (supportedChannels);

        if (AudioUnitGetProperty (audioUnit, kAudioUnitProperty_SupportedNumChannels, kAudioUnitScope_Global,
                                  0, supportedChannels, &supportedChannelsSize) == noErr
             && supportedChannelsSize > 0)
        {
            int explicitNumIns = 0;
            int explicitNumOuts = 0;
            int maximumNumIns = 0;
            int maximumNumOuts = 0;

            for (int i = 0; i < (int) (supportedChannelsSize / sizeof (AUChannelInfo)); ++i)
            {
                const int inChannels  = (int) supportedChannels[i].inChannels;
                const int outChannels = (int) supportedChannels[i].outChannels;

                if (inChannels < 0)
                    maximumNumIns  = jmin (maximumNumIns, inChannels);
                else
                    explicitNumIns = jmax (explicitNumIns, inChannels);

                if (outChannels < 0)
                    maximumNumOuts  = jmin (maximumNumOuts, outChannels);
                else
                    explicitNumOuts = jmax (explicitNumOuts, outChannels);
            }

            if ((maximumNumIns == -1 && maximumNumOuts == -1)  // (special meaning: any number of ins/outs, as long as they match)
                || (maximumNumIns == -2 && maximumNumOuts == -1) // (special meaning: any number of ins/outs, even if they don't match)
                || (maximumNumIns == -1 && maximumNumOuts == -2))
            {
                numInputBusChannels = numOutputBusChannels = 2;
            }
            else
            {
                numInputBusChannels = explicitNumIns;
                numOutputBusChannels = explicitNumOuts;

                if (maximumNumIns == -1 || (maximumNumIns < 0 && explicitNumIns <= -maximumNumIns))
                    numInputBusChannels = 2;

                if (maximumNumOuts == -1 || (maximumNumOuts < 0 && explicitNumOuts <= -maximumNumOuts))
                    numOutputBusChannels = 2;
            }
        }
        else
        {
            // (this really means the plugin will take any number of ins/outs as long
            // as they are the same)
            numInputBusChannels = numOutputBusChannels = 2;
        }
    }

    bool canProduceMidiOutput()
    {
        UInt32 dataSize = 0;
        Boolean isWritable = false;

        if (AudioUnitGetPropertyInfo (audioUnit, kAudioUnitProperty_MIDIOutputCallbackInfo,
                                      kAudioUnitScope_Global, 0, &dataSize, &isWritable) == noErr
             && dataSize != 0)
        {
            CFArrayRef midiArray;

            if (AudioUnitGetProperty (audioUnit, kAudioUnitProperty_MIDIOutputCallbackInfo,
                                      kAudioUnitScope_Global, 0, &midiArray, &dataSize) == noErr)
            {
                bool result = (CFArrayGetCount (midiArray) > 0);
                CFRelease (midiArray);
                return result;
            }
        }

        return false;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioUnitPluginInstance)
};

//==============================================================================
class AudioUnitPluginWindowCocoa    : public AudioProcessorEditor,
                                      public Timer
{
public:
    AudioUnitPluginWindowCocoa (AudioUnitPluginInstance& p, const bool createGenericViewIfNeeded)
        : AudioProcessorEditor (&p),
          plugin (p)
    {
        addAndMakeVisible (&wrapper);

        setOpaque (true);
        setVisible (true);
        setSize (100, 100);

        createView (createGenericViewIfNeeded);
    }

    ~AudioUnitPluginWindowCocoa()
    {
        if (isValid())
        {
            wrapper.setVisible (false);
            removeChildComponent (&wrapper);
            wrapper.setView (nil);
            plugin.editorBeingDeleted (this);
        }
    }

    bool isValid() const        { return wrapper.getView() != nil; }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::white);
    }

    void resized() override
    {
        wrapper.setSize (getWidth(), getHeight());
    }

    void timerCallback() override
    {
        wrapper.resizeToFitView();
        startTimer (jmin (713, getTimerInterval() + 51));
    }

    void childBoundsChanged (Component* child) override
    {
        setSize (wrapper.getWidth(), wrapper.getHeight());
        startTimer (70);
    }

private:
    AudioUnitPluginInstance& plugin;
    NSViewComponent wrapper;

    bool createView (const bool createGenericViewIfNeeded)
    {
        NSView* pluginView = nil;
        UInt32 dataSize = 0;
        Boolean isWritable = false;

        AudioUnitInitialize (plugin.audioUnit);

        if (AudioUnitGetPropertyInfo (plugin.audioUnit, kAudioUnitProperty_CocoaUI, kAudioUnitScope_Global,
                                      0, &dataSize, &isWritable) == noErr
             && dataSize != 0
             && AudioUnitGetPropertyInfo (plugin.audioUnit, kAudioUnitProperty_CocoaUI, kAudioUnitScope_Global,
                                          0, &dataSize, &isWritable) == noErr)
        {
            HeapBlock <AudioUnitCocoaViewInfo> info;
            info.calloc (dataSize, 1);

            if (AudioUnitGetProperty (plugin.audioUnit, kAudioUnitProperty_CocoaUI, kAudioUnitScope_Global,
                                      0, info, &dataSize) == noErr)
            {
                NSString* viewClassName = (NSString*) (info->mCocoaAUViewClass[0]);
                CFStringRef path = CFURLCopyPath (info->mCocoaAUViewBundleLocation);
                NSString* unescapedPath = (NSString*) CFURLCreateStringByReplacingPercentEscapes (0, path, CFSTR (""));
                CFRelease (path);
                NSBundle* viewBundle = [NSBundle bundleWithPath: [unescapedPath autorelease]];
                Class viewClass = [viewBundle classNamed: viewClassName];

                if ([viewClass conformsToProtocol: @protocol (AUCocoaUIBase)]
                     && [viewClass instancesRespondToSelector: @selector (interfaceVersion)]
                     && [viewClass instancesRespondToSelector: @selector (uiViewForAudioUnit: withSize:)])
                {
                    id factory = [[[viewClass alloc] init] autorelease];
                    pluginView = [factory uiViewForAudioUnit: plugin.audioUnit
                                                    withSize: NSMakeSize (getWidth(), getHeight())];
                }

                for (int i = (dataSize - sizeof (CFURLRef)) / sizeof (CFStringRef); --i >= 0;)
                    CFRelease (info->mCocoaAUViewClass[i]);

                CFRelease (info->mCocoaAUViewBundleLocation);
            }
        }

        if (createGenericViewIfNeeded && (pluginView == nil))
        {
            {
                // This forces CoreAudio.component to be loaded, otherwise the AUGenericView will assert
                AudioComponentDescription desc;
                String name, version, manufacturer;
                AudioUnitFormatHelpers::getComponentDescFromIdentifier ("AudioUnit:Output/auou,genr,appl",
                                                                        desc, name, version, manufacturer);
            }

            pluginView = [[AUGenericView alloc] initWithAudioUnit: plugin.audioUnit];
        }

        wrapper.setView (pluginView);

        if (pluginView != nil)
        {
            timerCallback();
            startTimer (70);
        }

        return pluginView != nil;
    }
};

#if JUCE_SUPPORT_CARBON

//==============================================================================
class AudioUnitPluginWindowCarbon   : public AudioProcessorEditor
{
public:
    AudioUnitPluginWindowCarbon (AudioUnitPluginInstance& plugin_)
        : AudioProcessorEditor (&plugin_),
          plugin (plugin_),
          audioComponent (nullptr),
          viewComponent (nullptr)
    {
        addAndMakeVisible (innerWrapper = new InnerWrapperComponent (*this));

        setOpaque (true);
        setVisible (true);
        setSize (400, 300);

        UInt32 propertySize;
        if (AudioUnitGetPropertyInfo (plugin.audioUnit, kAudioUnitProperty_GetUIComponentList,
                                      kAudioUnitScope_Global, 0, &propertySize, NULL) == noErr
             && propertySize > 0)
        {
            HeapBlock<AudioComponentDescription> views (propertySize / sizeof (AudioComponentDescription));

            if (AudioUnitGetProperty (plugin.audioUnit, kAudioUnitProperty_GetUIComponentList,
                                      kAudioUnitScope_Global, 0, &views[0], &propertySize) == noErr)
            {
                audioComponent = AudioComponentFindNext (nullptr, &views[0]);
            }
        }
    }

    ~AudioUnitPluginWindowCarbon()
    {
        innerWrapper = nullptr;

        if (isValid())
            plugin.editorBeingDeleted (this);
    }

    bool isValid() const noexcept           { return audioComponent != nullptr; }

    //==============================================================================
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black);
    }

    void resized() override
    {
        if (innerWrapper != nullptr)
            innerWrapper->setSize (getWidth(), getHeight());
    }

    //==============================================================================
    bool keyStateChanged (bool) override         { return false; }
    bool keyPressed (const KeyPress&) override   { return false; }

    //==============================================================================
    AudioUnit getAudioUnit() const      { return plugin.audioUnit; }

    AudioUnitCarbonView getViewComponent()
    {
        if (viewComponent == nullptr && audioComponent != nullptr)
            AudioComponentInstanceNew (audioComponent, &viewComponent);

        return viewComponent;
    }

    void closeViewComponent()
    {
        if (viewComponent != nullptr)
        {
            JUCE_AU_LOG ("Closing AU GUI: " + plugin.getName());

            AudioComponentInstanceDispose (viewComponent);
            viewComponent = nullptr;
        }
    }

private:
    //==============================================================================
    AudioUnitPluginInstance& plugin;
    AudioComponent audioComponent;
    AudioUnitCarbonView viewComponent;

    //==============================================================================
    class InnerWrapperComponent   : public CarbonViewWrapperComponent
    {
    public:
        InnerWrapperComponent (AudioUnitPluginWindowCarbon& owner_)
            : owner (owner_)
        {
        }

        ~InnerWrapperComponent()
        {
            deleteWindow();
        }

        HIViewRef attachView (WindowRef windowRef, HIViewRef rootView)
        {
            JUCE_AU_LOG ("Opening AU GUI: " + owner.plugin.getName());

            AudioUnitCarbonView carbonView = owner.getViewComponent();

            if (carbonView == 0)
                return 0;

            Float32Point pos = { 0, 0 };
            Float32Point size = { 250, 200 };

            HIViewRef pluginView = 0;

            AudioUnitCarbonViewCreate (carbonView,
                                       owner.getAudioUnit(),
                                       windowRef,
                                       rootView,
                                       &pos,
                                       &size,
                                       (ControlRef*) &pluginView);

            return pluginView;
        }

        void removeView (HIViewRef)
        {
            owner.closeViewComponent();
        }

    private:
        AudioUnitPluginWindowCarbon& owner;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InnerWrapperComponent)
    };

    friend class InnerWrapperComponent;
    ScopedPointer<InnerWrapperComponent> innerWrapper;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioUnitPluginWindowCarbon)
};

#endif

//==============================================================================
AudioProcessorEditor* AudioUnitPluginInstance::createEditor()
{
    ScopedPointer<AudioProcessorEditor> w (new AudioUnitPluginWindowCocoa (*this, false));

    if (! static_cast <AudioUnitPluginWindowCocoa*> (w.get())->isValid())
        w = nullptr;

   #if JUCE_SUPPORT_CARBON
    if (w == nullptr)
    {
        w = new AudioUnitPluginWindowCarbon (*this);

        if (! static_cast <AudioUnitPluginWindowCarbon*> (w.get())->isValid())
            w = nullptr;
    }
   #endif

    if (w == nullptr)
        w = new AudioUnitPluginWindowCocoa (*this, true); // use AUGenericView as a fallback

    return w.release();
}


//==============================================================================
//==============================================================================
AudioUnitPluginFormat::AudioUnitPluginFormat()
{
}

AudioUnitPluginFormat::~AudioUnitPluginFormat()
{
}

void AudioUnitPluginFormat::findAllTypesForFile (OwnedArray <PluginDescription>& results,
                                                 const String& fileOrIdentifier)
{
    if (! fileMightContainThisPluginType (fileOrIdentifier))
        return;

    PluginDescription desc;
    desc.fileOrIdentifier = fileOrIdentifier;
    desc.uid = 0;

    try
    {
        ScopedPointer <AudioPluginInstance> createdInstance (createInstanceFromDescription (desc));

        if (AudioUnitPluginInstance* const auInstance = dynamic_cast <AudioUnitPluginInstance*> (createdInstance.get()))
        {
            auInstance->fillInPluginDescription (desc);
            results.add (new PluginDescription (desc));
        }
    }
    catch (...)
    {
        // crashed while loading...
    }
}

AudioPluginInstance* AudioUnitPluginFormat::createInstanceFromDescription (const PluginDescription& desc)
{
    if (fileMightContainThisPluginType (desc.fileOrIdentifier))
    {
        ScopedPointer <AudioUnitPluginInstance> result (new AudioUnitPluginInstance (desc.fileOrIdentifier));

        if (result->audioUnit != nullptr)
        {
            result->initialise();
            return result.release();
        }
    }

    return nullptr;
}

StringArray AudioUnitPluginFormat::searchPathsForPlugins (const FileSearchPath& /*directoriesToSearch*/,
                                                          const bool /*recursive*/)
{
    StringArray result;
    AudioComponent comp = nullptr;

    for (;;)
    {
        AudioComponentDescription desc;
        zerostruct (desc);

        comp = AudioComponentFindNext (comp, &desc);

        if (comp == nullptr)
            break;

        AudioComponentGetDescription (comp, &desc);

        if (desc.componentType == kAudioUnitType_MusicDevice
             || desc.componentType == kAudioUnitType_MusicEffect
             || desc.componentType == kAudioUnitType_Effect
             || desc.componentType == kAudioUnitType_Generator
             || desc.componentType == kAudioUnitType_Panner)
        {
            const String s (AudioUnitFormatHelpers::createPluginIdentifier (desc));
            DBG (s);
            result.add (s);
        }
    }

    return result;
}

bool AudioUnitPluginFormat::fileMightContainThisPluginType (const String& fileOrIdentifier)
{
    AudioComponentDescription desc;

    String name, version, manufacturer;
    if (AudioUnitFormatHelpers::getComponentDescFromIdentifier (fileOrIdentifier, desc, name, version, manufacturer))
        return AudioComponentFindNext (nullptr, &desc) != nullptr;

    const File f (File::createFileWithoutCheckingPath (fileOrIdentifier));

    return f.hasFileExtension (".component")
             && f.isDirectory();
}

String AudioUnitPluginFormat::getNameOfPluginFromIdentifier (const String& fileOrIdentifier)
{
    AudioComponentDescription desc;
    String name, version, manufacturer;
    AudioUnitFormatHelpers::getComponentDescFromIdentifier (fileOrIdentifier, desc, name, version, manufacturer);

    if (name.isEmpty())
        name = fileOrIdentifier;

    return name;
}

bool AudioUnitPluginFormat::pluginNeedsRescanning (const PluginDescription& desc)
{
    AudioComponentDescription newDesc;
    String name, version, manufacturer;

    return ! (AudioUnitFormatHelpers::getComponentDescFromIdentifier (desc.fileOrIdentifier, newDesc,
                                                                      name, version, manufacturer)
               && version == desc.version);
}

bool AudioUnitPluginFormat::doesPluginStillExist (const PluginDescription& desc)
{
    if (desc.fileOrIdentifier.startsWithIgnoreCase (AudioUnitFormatHelpers::auIdentifierPrefix))
        return fileMightContainThisPluginType (desc.fileOrIdentifier);

    return File (desc.fileOrIdentifier).exists();
}

FileSearchPath AudioUnitPluginFormat::getDefaultLocationsToSearch()
{
    return FileSearchPath();
}

#undef JUCE_AU_LOG

#endif
