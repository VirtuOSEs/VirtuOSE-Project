#include "TSCallback.h"
#include "console/engineAPI.h"
#include "console/simEvents.h"
#include "console/simBase.h"
#include "platform/threads/threadPool.h"
#pragma warning(disable:4996) 

//
//InstrumentStartPlayingCallback CallbackManager::instrumentStartPlayingCallback;
//VelocityChangedCallback CallbackManager::velocityChangedCallback;
//TempoGestureStartCallback CallbackManager::tempoGestureStartCallback;
//TempoGestureEndCallback CallbackManager::tempoGestureEndCallback;

void CallbackManager::instrumentWillPlay(const juce::String& instrumentName, double delayInMillis)
{
  const S32 argc = 3;
  size_t arg1Lenght = instrumentName.length();
  char* arg1 = new char[arg1Lenght];
  instrumentName.toStdString().copy(arg1, arg1Lenght);
  arg1[arg1Lenght] = 0;

  juce::String doubleArg(delayInMillis);
  size_t arg2Lenght = doubleArg.length();
  char * arg2 = new char[arg1Lenght];
  doubleArg.toStdString().copy(arg2, arg2Lenght);
  arg2[arg2Lenght] = 0;

  const char* argv[3] = {"onInstrumentWillPlay",arg1, arg2};
  SimConsoleEvent* evt = new SimConsoleEvent(argc, argv, false);
  Sim::postEvent(Sim::getRootGroup(), evt, Sim::getCurrentTime());
}

void CallbackManager::instrumentStartPlaying(const juce::String& instrumentName)
{
  const S32 argc = 2;
  size_t arg1Lenght = instrumentName.length();
  char* arg1 = new char[arg1Lenght];
  instrumentName.toStdString().copy(arg1, arg1Lenght);
  arg1[arg1Lenght] = 0;

  const char* argv[2] = {"onInstrumentStartPlaying",arg1};
  SimConsoleEvent* evt = new SimConsoleEvent(argc, argv, false);
  Sim::postEvent(Sim::getRootGroup(), evt, Sim::getCurrentTime());

  //Old implementation
  //instrumentStartPlayingCallback.pushNewCall(instrumentName);
  //if (!instrumentStartPlayingCallback.isThreadRunning())
  //{
  //  instrumentStartPlayingCallback.startThread();
  //}
}

void CallbackManager::instrumentStoppedPlay(const juce::String& instrumentName)
{
  const S32 argc = 2;
  size_t arg1Lenght = instrumentName.length();
  char* arg1 = new char[arg1Lenght];
  instrumentName.toStdString().copy(arg1, arg1Lenght);
  arg1[arg1Lenght] = 0;

  const char* argv[2] = {"onInstrumentStoppedPlaying",arg1};
  SimConsoleEvent* evt = new SimConsoleEvent(argc, argv, false);
  Sim::postEvent(Sim::getRootGroup(), evt, Sim::getCurrentTime());
}

void CallbackManager::velocityChanged(float newVelocity)
{
  const S32 argc = 2;
  juce::String arg(newVelocity);
  size_t arg1Lenght = arg.length();
  char * parg = new char[arg1Lenght];
  arg.toStdString().copy(parg, arg1Lenght);
  parg[arg1Lenght] = 0;

  const char* argv[2] = {"onVelocityChanged", parg};

  SimConsoleEvent* evt = new SimConsoleEvent(argc, argv, false);
  Sim::postEvent(Sim::getRootGroup(), evt, Sim::getCurrentTime());

  //Old implementation
  //velocityChangedCallback.pushNewCall(newVelocity);
  //if (!velocityChangedCallback.isThreadRunning())
  //{
  //  velocityChangedCallback.startThread();
  //}
}

void CallbackManager::tempoJustChanged(int newTempo)
{
  const S32 argc = 2;
  juce::String arg(newTempo);
  size_t arg1Lenght = arg.length();
  char * parg = new char[arg1Lenght];
  arg.toStdString().copy(parg, arg1Lenght);
  parg[arg1Lenght] = 0;

  const char* argv[2] = {"onTempoJustChanged", parg};

  SimConsoleEvent* evt = new SimConsoleEvent(argc, argv, false);
  Sim::postEvent(Sim::getRootGroup(), evt, Sim::getCurrentTime());
}

void CallbackManager::tempoGestureStart()
{
  const S32 argc = 1;
  const char* argv[1] = {"onTempoGestureStart"};

  SimConsoleEvent* evt = new SimConsoleEvent(argc, argv, false);
  Sim::postEvent(Sim::getRootGroup(), evt, Sim::getCurrentTime());


  //tempoGestureStartCallback.pushNewCall();
  //if (!tempoGestureStartCallback.isThreadRunning())
  //{
  //  tempoGestureStartCallback.startThread();
  //}
}

void CallbackManager::tempoGestureEnd()
{
  const S32 argc = 1;
  const char* argv[1] = {"onTempoGestureEnd"};

  SimConsoleEvent* evt = new SimConsoleEvent(argc, argv, false);
  Sim::postEvent(Sim::getRootGroup(), evt, Sim::getCurrentTime());

  //tempoGestureEndCallback.pushNewCall();
  //if (!tempoGestureEndCallback.isThreadRunning())
  //{
  //  tempoGestureEndCallback.startThread();
  //}
}

//void InstrumentStartPlayingCallback::run()
//{
//  while (!instrumentsNames.empty())
//  {
//    const char* argv[2] = {"onInstrumentStartPlaying", instrumentsNames.front().toStdString().c_str()};
//
//    SimConsoleEvent* evt = new SimConsoleEvent(2, argv, false);
//    Sim::postEvent(Sim::getRootGroup(), evt, Sim::getCurrentTime());
//    //onInstrumentStartPlaying_callback(instrumentsNames.front().toStdString().c_str());
//    //Concurrent access
//    {
//      const juce::ScopedLock sL(modifyQueue);
//      instrumentsNames.pop();
//    }
//  }
//}
//
//void InstrumentWillPlayCallback::run()
//{
//  onInstrumentWillPlay_callback(instrumentName.toStdString().c_str(), static_cast<float>(delay));
//}
//
//void InstrumentStoppedPlayingCallback::run()
//{
//  onInstrumentStoppedPlaying_callback(instrumentName.toStdString().c_str());
//}
//
//void VelocityChangedCallback::run()
//{
//  while (!callQueue.empty())
//  {
//
//    juce::String arg(callQueue.front());
//    char * parg = new char[arg.length() + 1];
//    arg.toStdString().copy(parg, arg.length());
//    parg[arg.length()] = 0;
//
//    const char* argv[2] = {"onVelocityChanged", parg};
//
//    SimConsoleEvent* evt = new SimConsoleEvent(2, argv, false);
//    Sim::postEvent(Sim::getRootGroup(), evt, Sim::getCurrentTime());
//    //onVelocityChanged_callback(callQueue.front());
//
//    //Concurrent access
//    {
//      const juce::ScopedLock sL(modifyQueue);
//      callQueue.pop();
//    }
//  }
//}
//
//void TempoGestureStartCallback::run()
//{
//  while (callCount > 0)
//  {
//    onTempoGestureStart_callback();
//    const juce::ScopedLock sL(countAccess);
//    --callCount;
//  }
//}
//
//void TempoGestureEndCallback::run()
//{
//  while (callCount > 0)
//  {
//    onTempoGestureEnd_callback();
//    const juce::ScopedLock sL(countAccess);
//    --callCount;
//  }
//}

// --- TorqueScript Callbacks implementation

//IMPLEMENT_GLOBAL_CALLBACK( onInstrumentStartPlaying, void, (const char* instrumentName), (instrumentName),
//   "A callback called by the engine when a track begins to play actual notes.\n"
//   "@param instrumentName The name of the instrument which will be played.\n"
//  );
//
//IMPLEMENT_GLOBAL_CALLBACK( onInstrumentWillPlay, void, (const char* instrumentName, float delayInMillis), (instrumentName, delayInMillis),
//   "A callback called by the engine when a track will soon begin to play.\n"
//   "@param instrumentName The name of the instrument which will be played.\n"
//   "@param delayInMillis The time in milliseconds before the track begins to play\n"
//  );
//
//IMPLEMENT_GLOBAL_CALLBACK( onInstrumentStoppedPlaying, void, (const char* instrumentName), (instrumentName),
//   "A callback called by the engine when a track stops to play.\n"
//   "@param instrumentName The name of the instrument which will be played.\n"
//  );
//
//IMPLEMENT_GLOBAL_CALLBACK(onVelocityChanged, void, (float newVelocity), (newVelocity),
//	"Called when the user changes the velocity.\n"
//);
//IMPLEMENT_GLOBAL_CALLBACK(onTempoJustChanged, void, (int newTempo), (newTempo),
//	"Called when the the midi sequence tempo is actually changed.\n"
//);
//
//IMPLEMENT_GLOBAL_CALLBACK(onTempoGestureStart, void, (), (),
//	"Called when the user starts a tempo gesture.\n"
//);
//
//IMPLEMENT_GLOBAL_CALLBACK(onTempoGestureEnd, void, (), (),
//	"Called when the user ends a tempo gesture.\n"
//
//);

