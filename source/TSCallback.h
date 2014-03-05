#ifndef VIRTUOSE_TS_CALLBACK_H
#define VIRTUOSE_TS_CALLBACK_H

#include "platform/threads/ThreadPool.h"
#include "JuceLibraryCode\JuceHeader.h"

class TempoGestureStartCallback : public ThreadPool::WorkItem
{
public:
  virtual void execute();
};

class TempoGestureEndCallback : public ThreadPool::WorkItem
{
public:
  virtual void execute();
};

class InstrumentStartPlayingCallback : public ThreadPool::WorkItem
{
public:
  explicit InstrumentStartPlayingCallback(const juce::String& instrumentName);

  virtual void execute();

private:
  juce::String instrumentName;
};

class InstrumentWillPlayCallback : public ThreadPool::WorkItem
{
public:
  explicit InstrumentWillPlayCallback(const juce::String& instrumentName, const double delayInMillis);
  virtual void execute();

private:
  const juce::String instrumentName;
  const double delayInMillis;
};

class InstrumentStoppedPlayingCallback : public ThreadPool::WorkItem
{
public:
  explicit InstrumentStoppedPlayingCallback(const juce::String& instrumentName);
  virtual void execute();

private:
  const juce::String instrumentName;
};

class TempoJustChangedCallback : public ThreadPool::WorkItem
{
public:
  explicit TempoJustChangedCallback(const int newTempo);
  virtual void execute();

private:
  const int newTempo;
};

class VelocityChangedCallback : public ThreadPool::WorkItem
{
public:
  explicit VelocityChangedCallback(const float newVelocity);
  virtual void execute();

private:
  const float newVelocity;
};

#endif