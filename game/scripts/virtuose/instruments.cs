
function initialiseFamilies()
{
  %pattern = "../fxp/Woodwinds/*.fxp";
  %nbInstrus = getFileCount(%pattern);
  $woodwinds[0] = findFirstFile(%pattern);
  $nbWoodwinds = %nbInstrus;
  for(%i = 1; %i < %nbInstrus; %i++)
  {
    %instruName = findNextFile(%pattern);
    %instruName = fileName(%instruName);
    %instruName = strreplace(%instruName, ".fxp", "");
    $woodwinds[%i] = %instruName;
    echo(%instruName);
  }
  
  %pattern = "../fxp/Strings/*.fxp";
  %nbInstrus = getFileCount(%pattern);
  $strings[0] = findFirstFile(%pattern);
  $nbStrings = %nbInstrus;
  for(%i = 1; %i < %nbInstrus; %i++)
  {
    %instruName = findNextFile(%pattern);
    %instruName = fileName(%instruName);
    %instruName = strreplace(%instruName, ".fxp", "");
    $strings[%i] = %instruName;
    echo(%instruName);
  }
  
  %pattern = "../fxp/Horns/*.fxp";
  %nbInstrus = getFileCount(%pattern);
  $horns[0] = findFirstFile(%pattern);
  $nbHorns = %nbInstrus;
  for(%i = 1; %i < %nbInstrus; %i++)
  {
    %instruName = findNextFile(%pattern);
    %instruName = fileName(%instruName);
    %instruName = strreplace(%instruName, ".fxp", "");
    $horns[%i] = %instruName;
    echo(%instruName);
  }
  
  %pattern = "../fxp/Percussions/*.fxp";
  %nbInstrus = getFileCount(%pattern);
  $percussions[0] = findFirstFile(%pattern);
  $nbPercussions = %nbInstrus;
  for(%i = 1; %i < %nbInstrus; %i++)
  {
    %instruName = findNextFile(%pattern);
    %instruName = fileName(%instruName);
    %instruName = strreplace(%instruName, ".fxp", "");
    $percussions[%i] = %instruName;
    echo(%instruName);
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
