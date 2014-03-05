#include "TSCallback.h"
#include "console/engineAPI.h"

// --- TorqueScript Callbacks implementation
IMPLEMENT_GLOBAL_CALLBACK(onTempoGestureStart, void, (), (),
	"Called when the user starts a tempo gesture.\n"
);

IMPLEMENT_GLOBAL_CALLBACK(onTempoGestureEnd, void, (), (),
	"Called when the user ends a tempo gesture.\n"
);

IMPLEMENT_GLOBAL_CALLBACK( onInstrumentStartPlaying, void, (const char* instrumentName), (instrumentName),
   "A callback called by the engine when a track begins to play actual notes.\n"
   "@param instrumentName The name of the instrument which will be played.\n"
  );

IMPLEMENT_GLOBAL_CALLBACK( onInstrumentWillPlay, void, (const char* instrumentName, float delayInMillis), (instrumentName, delayInMillis),
   "A callback called by the engine when a track will soon begin to play.\n"
   "@param instrumentName The name of the instrument which will be played.\n"
   "@param delayInMillis The time in milliseconds before the track begins to play\n"
  );

IMPLEMENT_GLOBAL_CALLBACK( onInstrumentStoppedPlaying, void, (const char* instrumentName), (instrumentName),
   "A callback called by the engine when a track stops to play.\n"
   "@param instrumentName The name of the instrument which will be played.\n"
  );


IMPLEMENT_GLOBAL_CALLBACK(onTempoJustChanged, void, (int newTempo), (newTempo),
	"Called when the the midi sequence tempo is actually changed.\n"
);

IMPLEMENT_GLOBAL_CALLBACK(onVelocityChanged, void, (float newVelocity), (newVelocity),
	"Called when the user changes the velocity.\n"
);


void TempoGestureStartCallback::execute()
{
  onTempoGestureStart_callback();
}

void TempoGestureEndCallback::execute()
{
  onTempoGestureEnd_callback();
}

InstrumentStartPlayingCallback::InstrumentStartPlayingCallback(const juce::String& instrumentName)
  : instrumentName(instrumentName)
{}

void InstrumentStartPlayingCallback::execute()
{
  onInstrumentStartPlaying_callback(instrumentName.toStdString().c_str());
}

InstrumentWillPlayCallback::InstrumentWillPlayCallback(const juce::String& instrumentName, const double delayInMillis)
  :instrumentName(instrumentName), delayInMillis(delayInMillis)
{}

void InstrumentWillPlayCallback::execute()
{
  onInstrumentWillPlay_callback(instrumentName.toStdString().c_str(), static_cast<float>(delayInMillis));
}

InstrumentStoppedPlayingCallback::InstrumentStoppedPlayingCallback(const juce::String& instrumentName)
  : instrumentName(instrumentName)
{}

void InstrumentStoppedPlayingCallback::execute()
{
  onInstrumentStoppedPlaying_callback(instrumentName.toStdString().c_str());
}

TempoJustChangedCallback::TempoJustChangedCallback(const int newTempo)
  : newTempo(newTempo)
{}

void TempoJustChangedCallback::execute()
{
  onTempoJustChanged_callback(newTempo);
}

VelocityChangedCallback::VelocityChangedCallback(const float newVelocity)
  : newVelocity(newVelocity)
{}

void VelocityChangedCallback::execute()
{
  onVelocityChanged_callback(newVelocity);
}