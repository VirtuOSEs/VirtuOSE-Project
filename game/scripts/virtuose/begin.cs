// Le script du projet Virtuose
exec( "scripts/virtuose/baguettes.cs" );
exec( "scripts/virtuose/modifObjects.cs" );
exec( "scripts/virtuose/sounds.cs" );
exec( "scripts/virtuose/instruments.cs" );

$posx = 0.0;
$posy = 0.0;
$posz = 0.0;
$posxstring = 0.0;
$posxpercu = 0.0;
$posxhorn = 0.0;
$posxwoodwind = 0.0;
$posxsolo = 0.0;

 //Initialisation
function virtuoseStart(){
  echo("Virtuose is beginning");
  MLAAfx.toggle();
  initialiseFamilies();
  $posx = 0.0;
  $posy = 0.0;
  $posz = 0.0;
  $posxstring = 0.0;
  $posxpercu = 0.0;
  $posxhorn = 0.0;
  $posxwoodwind = 0.0;
  $posxsolo = 0.0;
	
	//DataBlock Particle
   datablock ParticleEmitterNodeData(SimpleEmitterNodeData)
	{
    timeMultiple = 1.0;
	};

	%options = new Options();
	
  %options.initialTempo=tempo.getText();
  %options.rythmUnit=battue.getText();
  %options.rythmUnitDotted=dotted.isStateOn();
	if(leftHandCtrl.getValue()==1)
			%options.handedness = LEFT_HANDEDNESS;
	else
			%options.handedness = RIGHT_HANDEDNESS;
	
	$orchestrator = new Orchestrator()
  {
    options = %options;
  };
  $orchestrator.setSaveFile("../tracks/" @ $vsaveFile);
  $orchestrator.loadMidiFile("../tracks/" @ $vfileName);

	//HideSun();
	//HideSun();
  leftHand.position = Spawn.position.x-2 SPC Spawn.position.y+5 SPC Spawn.position.z+1;
  rightHand.position = Spawn.position.x+2 SPC Spawn.position.y+5 SPC Spawn.position.z+1;
	
	$TempoParticleNode=new ParticleEmitterNode(TempoParticleNode) {
         active = "0";
         emitter = "TempoEmitter";
         velocity = "1";
         dataBlock = "SimpleEmitterNodeData";
         position = "0.98659 24.9451 6.40856";
         rotation = "1 0 0 0";
         scale = "1 1 1";
         canSave = "1";
         canSaveDynamicFields = "1";
      };
      $velocityParticleNode=new ParticleEmitterNode(velocityParticleNode) {
         active = "0";
         emitter = "velocityEmitter";
         velocity = "1";
         dataBlock = "SimpleEmitterNodeData";
         position = "-3.01341 24.9451 6.40856";
         rotation = "1 0 0 0";
         scale = "1 1 1";
         canSave = "1";
         canSaveDynamicFields = "1";
      };
	$playParticleNode=new ParticleEmitterNode(playParticleNode) {
         active = "0";
         emitter = "playEmitter";
         velocity = "1";
         dataBlock = "SimpleEmitterNodeData";
         position = "0 0 0";
         rotation = "1 0 0 0";
         scale = "1 1 1";
         canSave = "1";
         canSaveDynamicFields = "1";
      };
	
	if(strcmp($orchestrator.options.handedness,LEFT_HANDEDNESS)==0){
		$velocityParticleNode.position=rightHand.position;
		$TempoParticleNode.position=leftHand.position;
	}
	else {
		$velocityParticleNode.position=leftHand.position;
		$TempoParticleNode.position=rightHand.position;
	}
		
	%mapTo=leftHand.getTargetName(0);
	%mapTo.diffuseColor.r=0.5;
	%mapTo.diffuseColor.g=0.5;
	%mapTo.diffuseColor.b=0.5;
	%mapTo.reload();
	
	%mapTo=rightHand.getTargetName(0);
	%mapTo.diffuseColor.r=0.5;
	%mapTo.diffuseColor.g=0.5;
	%mapTo.diffuseColor.b=0.5;
	%mapTo.reload();
	
	placeInstrumentsTrack();
	
	
}
  
function placeInstruments(%nbInstruments){
	for(%i=0;%i<%nbInstruments;%i++){
		$InstrumentsScene[%i].position.y+=5;
	}
}

function createInstruments(%instrument,%file){

				%nameInstru=%instrument;
				if (isWoodwind(%file))
				{
				  %family = "woodwind";
				$posxwoodind ++;
			   	$posx= $posxwoodind;	
				}
				else if (isHorn(%file))
				{
          %family = "horn";
          $posxhorn ++;
          $posx= $posxhorn;
				}
				else if (isString(%file))
				{
          %family = "string";
          $posxstring +=1.5;
          $posx= $posxstring;
				}
				else if (isSoloInstrument(%file))
				{
				  %family = "solo";
				  $posxsolo = $posxsolo + 2;
				  $posx = $posxsolo;
				}
				else// if (isPercussion(%file))
				{
					  %family = "percussion";
					  $posxpercu ++;
					  $posx= $posxpercu;
				}
				
			   //Handling case where the instrument has not been yet modelised in 3D
			   %file3DName = "art/shapes/virtuose/" @ strlwr(%file) @ ".dae";
			   %fileExist = getFileCount(%file3DName, false);
			   if (%fileExist <= 0)
			   {
			     //We try with an 's' at the end!
			     %file3DName = "art/shapes/virtuose/" @ strlwr(%file) @ "s.dae";
			     %fileExist = getFileCount(%file3DName, false);
			     if (%fileExist <= 0)
			     {
			       echo("File doesn't exist");
			       %file3DName = "art/shapes/virtuose/violins.dae";//Default shape
			     }
			   }
			   echo(%nameInstru);
			   echo("Final name : " @ %file3DName);
			   echo(%family @ ":" @ %family.position);
			   new TSStatic(%nameInstru) 

			   {
					shapeName = %file3DName;

					name=%nameInstru;
					internalName=%nameInstru;
				   //dataBlock = %instrument;
	
					position = $posx+%family.position.x SPC %family.position.y SPC %family.position.z;
					particule="";
				   rotation = "1 90 0 0";
				   scale = "1 1 1";
				   //name=%instrument;
					timeLeft=0;
					distanceLeft=1;
					hightOrigin=%family.position.z;
					wasPlaying=0;
			   };
}

function placeInstrumentsTrack(){
	for(%i=0;%i<$orchestrator.getNumTracks();%i++){
		createInstruments($orchestrator.getTrackName(%i),$orchestrator.getInstrumentName(%i));
		echo("");
	}
}