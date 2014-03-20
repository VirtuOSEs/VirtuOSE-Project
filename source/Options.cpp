#include "Options.h"

IMPLEMENT_CONOBJECT(Options);

ImplementEnumType( Handedness,
   "User preference for use of a hand.\n"
   "@ref Options\n\n"
   "@ingroup Virtuose" )
   { LEFT_HANDEDNESS, "LEFT_HANDEDNESS",
      "User prefers to use its left hand." },
   { RIGHT_HANDEDNESS, "RIGHT_HANDEDNESS", 
      "User prefers to use its right hand." },
EndImplementEnumType;

ImplementEnumType( RythmUnit,
   "Rythm Unit for the tempo.\n"
   "@ref Options\n\n"
   "@ingroup Virtuose" )
   { WHOLE_NOTE, "WHOLE_NOTE",
      "Rythm unit is a whole note." },
   { HALF_NOTE, "HALF_NOTE", 
      "Rythm unit is a half note." },
   { QUARTER_NOTE, "QUARTER_NOTE", 
      "Rythm unit is a quarter note." },
   { EIGHT_NOTE, "EIGHT_NOTE", 
      "Rythm unit is a eight note." },
EndImplementEnumType;

Options::Options()
  : handedness(RIGHT_HANDEDNESS),
    initialTempo(90),
    rythmUnit(QUARTER_NOTE),
    rythmUnitDotted(false)
{
}

Options::~Options()
{
}


void Options::initPersistFields()
{   
   Parent::initPersistFields();
   addGroup( "Options" );

      addField( "handedness", TypeHandedness, Offset( handedness, Options ), 
         "The type of handedness (left, right) requested by the user." );

      addField( "initialTempo",   TypeS32, Offset( initialTempo, Options ),
         "The initial tempo choosed by the user." );

      addField( "rythmUnit",   TypeRythmUnit, Offset( rythmUnit, Options ),
         "The rythm unit (whole note, half note, quarter note...) choosed by the user." );

      addField( "rythmUnitDotted",   TypeBool, Offset( rythmUnitDotted, Options ),
         "Indicate if the rythm unit is dotted or not." );

   endGroup( "Options" );    
}
