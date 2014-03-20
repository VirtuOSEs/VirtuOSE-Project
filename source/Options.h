#ifndef VIRTUOSE_OPTIONS_H
#define VIRTUOSE_OPTIONS_H

#include "console/engineAPI.h"
#include "console/dynamicTypes.h"

enum Handedness
{
  LEFT_HANDEDNESS,
  RIGHT_HANDEDNESS
};

enum RythmUnit
{
  WHOLE_NOTE = 1,
  HALF_NOTE = 2,
  QUARTER_NOTE = 4,
  EIGHT_NOTE = 8
};

DefineEnumType(Handedness);

/**
  * The Options class is used to store information
  * about user preferences. It is exposed to TorqueScript
  * as a SimObject whith user preferences as static fields
  * for a simple use. Example of use in TorqueScript:
  * $options = new Options();
  * $options.initialTempo = 120;
  * $options.handedNess = LEFT_HAND;
  * This class is used by Orchestrator to initialize all values
  * and behaviors according to user preferences.
  * @see Orchestrator
  * @see Handedness
  * @see RythmUnit
 */
class Options : public SimObject
{
public:
  typedef SimObject Parent;

  DECLARE_CONOBJECT( Options );
  DECLARE_CATEGORY( "Virtuose" );
  DECLARE_DESCRIPTION( "Object encapsulating user preferences" );

  static void initPersistFields();

  Options();
  ~Options();

  Handedness handedness;
  RythmUnit rythmUnit;
  bool rythmUnitDotted;

  int initialTempo;
};

#endif
