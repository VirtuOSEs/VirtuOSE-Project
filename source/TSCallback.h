#ifndef TS_CALLBACK_H
#define TS_CALLBACK_H

#include "JuceLibraryCode\JuceHeader.h"

class InstrumentWillPlayCallback : public juce::Thread
{
public:
  juce::String instrumentName

  virtual void run()
  {
    onInstrumentWillPlay_callback(instrumentName.toStdString().c_str());
  }

};

#endif // TS_CALLBACK_H