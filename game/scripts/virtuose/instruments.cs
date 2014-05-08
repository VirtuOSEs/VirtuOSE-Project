
function initialiseFamilies()
{
  %pattern = "../fxpTest/Woodwinds/*.fxp";
  %nbInstrus = getFileCount(%pattern);
  $woodwinds[0] = findFirstFile(%pattern);
  $nbWoodwinds = %nbInstrus;
  for(%i = 1; %i < %nbInstrus; %i++)
  {
    %instruName = findNextFile(%pattern);
    %instruName = fileName(%instruName);
    %instruName = stripChars(%instruName, ".fxp");
    $woodwinds[%i] = %instruName;
  }
  
  %pattern = "../fxpTest/Strings/*.fxp";
  %nbInstrus = getFileCount(%pattern);
  $strings[0] = findFirstFile(%pattern);
  $nbStrings = %nbInstrus;
  for(%i = 1; %i < %nbInstrus; %i++)
  {
    %instruName = findNextFile(%pattern);
    %instruName = fileName(%instruName);
    %instruName = stripChars(%instruName, ".fxp");
    $strings[%i] = %instruName;
  }
  
  %pattern = "../fxpTest/Horns/*.fxp";
  %nbInstrus = getFileCount(%pattern);
  $horns[0] = findFirstFile(%pattern);
  $nbHorns = %nbInstrus;
  for(%i = 1; %i < %nbInstrus; %i++)
  {
    %instruName = findNextFile(%pattern);
    %instruName = fileName(%instruName);
    %instruName = stripChars(%instruName, ".fxp");
    $horns[%i] = %instruName;
  }
  
  %pattern = "../fxpTest/Percussions/*.fxp";
  %nbInstrus = getFileCount(%pattern);
  $percussions[0] = findFirstFile(%pattern);
  $nbPercussions = %nbInstrus;
  for(%i = 1; %i < %nbInstrus; %i++)
  {
    %instruName = findNextFile(%pattern);
    %instruName = fileName(%instruName);
    %instruName = stripChars(%instruName, ".fxp");
    $percussions[%i] = %instruName;
  }
}

function isWoodwind(%instruName)
{
 for(%i = 0; %i < $nbWoodwinds; %i++)
 {
   if (stricmp(%instruName, $woodwinds[%i]) == 0)
     return true;
 }
 return false;
}

function isHorn(%instruName)
{
 for(%i = 0; %i < $nbHorns; %i++)
 {
   if (stricmp(%instruName, $horns[%i]) == 0)
     return true;
 }
 return false;
}

function isString(%instruName)
{
   for(%i = 0; %i < $nbStrings; %i++)
 {
   if (stricmp(%instruName, $strings[%i]) == 0)
     return true;
 }
 return false;
}

function isPercussion(%instruName)
{
 for(%i = 0; %i < $nbPercussions; %i++)
 {
   if (stricmp(%instruName, $percussions[%i]) == 0)
     return true;
 }
 return false;
}
