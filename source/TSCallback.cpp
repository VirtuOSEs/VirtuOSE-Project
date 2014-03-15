#include "TSCallback.h"
#include "console/engineAPI.h"
#include "console/simEvents.h"
#include "console/simBase.h"
#include "platform/threads/threadPool.h"

#pragma warning(disable:4996) 

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
  Sim::postEvent(Sim::getRootGroup(), evt, -1);
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
  Sim::postEvent(Sim::getRootGroup(), evt, -1);
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
  Sim::postEvent(Sim::getRootGroup(), evt, -1);
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
  Sim::postEvent(Sim::getRootGroup(), evt, -1);
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
  Sim::postEvent(Sim::getRootGroup(), evt, -1);
}

void CallbackManager::tempoGestureStart()
{
  const S32 argc = 1;
  const char* argv[1] = {"onTempoGestureStart"};

  SimConsoleEvent* evt = new SimConsoleEvent(argc, argv, false);
  Sim::postEvent(Sim::getRootGroup(), evt, -1);
}

void CallbackManager::tempoGestureEnd()
{
  const S32 argc = 1;
  const char* argv[1] = {"onTempoGestureEnd"};

  SimConsoleEvent* evt = new SimConsoleEvent(argc, argv, false);
  Sim::postEvent(Sim::getRootGroup(), evt, -1);
}

