// Le script du projet Virtuose
exec( "scripts/virtuose/baguettes.cs" );
exec( "scripts/virtuose/modifObjects.cs" );
exec( "scripts/virtuose/sounds.cs" );

$posx = 0.0;
$posy = 0.0;
$posz = 0.0;
$posxcorde = 0.0;
$posxpercu = 0.0;
$posxbois = 0.0;
$posxcuivre = 0.0;
$posxvent = 0.0;


 //Initialisation
function virtuoseStart(){
	echo("Virtuose is beginning");
	
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
	
	$playParticleNode.position="0 0 0";
	
	if(strcmp($orchestrator.options.handedness,"lEFT_HANDEDNESS")==0){
		$velocityParticleNode.position=rightHand.position;
		$TempoParticleNode.position=leftHand.position;
	}
	else {
		$velocityParticleNode.position=leftHand.position;
		$TempoParticleNode.position=rightHand.position;
	}
	
	placeInstrumentsTrack();
	
}
  
function placeInstruments(%nbInstruments){
	for(%i=0;%i<%nbInstruments;%i++){
		$InstrumentsScene[%i].position.y+=5;
	}
}

function createInstruments(%instrument,%file){
				echo(%instrument SPC %file);
				%nameInstru=%instrument;

				
			  switch$(%file){
			   	case "piano":
			   		%famille = "corde";
			   		$posxcorde +=1.5;
			   		$posx= $posxcorde;
			   	case "harp":
			   		%famille = "corde";	
			   		$posxcorde +=1.5;
			   		$posx= $posxcorde;
			   	case "electricbass":
			   		%famille = "corde";
			   		$posxcorde +=1.5;
			   		$posx= $posxcorde;		
			   	case "Guitar":
			   		%famille = "corde";
			   		$posxcorde +=1.5;
			   		$posx= $posxcorde;	
			   	case "acousticguitar":
			   		%famille = "corde";
			   		$posxcorde +=1.5;
			   		$posx= $posxcorde;		
			   	case "violins":
			   	case "violin":
			   		%famille = "corde";
			   		$posxcorde +=1.5;
			   		$posx= $posxcorde;	
			   	case "cellos":
			   	case "celli":
			   	case "cello":
			   		%famille = "corde";
			   		$posxcorde +=1.5;
			   		$posx= $posxcorde;	
			   	case "violas":
			   	case "viola":
			   		%famille = "corde";
			   		$posxcorde +=1.5;
			   		$posx= $posxcorde;
			   	case "bass":
			   	case "basses":
			   		%famille = "corde";
			   		$posxcorde +=1.5;
			   		$posx= $posxcorde;
			   	case "drum":
			   		%famille = "percussion";
			   		$posxpercu ++;
			   		$posx= $posxpercu;
			   	case "timpani":
			   		%famille = "percussion";
			   		$posxpercu ++;
			   		$posx= $posxpercu;	
			   	case "saxophone":
			   		%famille = "bois";
			   		$posxbois ++;
			   		$posx= $posxbois;
			   	case "oboes":
			   	case "oboe":
			   		%famille = "bois";
			   		$posxbois ++;
			   		$posx= $posxbois;	
			   	case "bassoons":
			   	case "bassoon":
			   		%famille = "bois";
			   		$posxbois ++;
			   		$posx= $posxbois;	
			   	case "trombone":
			   		%famille = "cuivre";
			   		$posxcuivre ++;
			   		$posx= $posxcuivre;
			   	case "trumpets":
			   	case "trumpet":
			   		%famille = "cuivre";
			   		$posxcuivre ++;
			   		$posx= $posxcuivre;
			   	case "flutes":
			   	case "flute":
          case "clarinets":
			   	case "clarinet":
			   		%famille = "vent";
			   		$posxvent ++;
			   		$posx= $posxvent;
			   	case "horns":
			   	case "horn":
			   		%famille = "vent";
			   		$posxvent ++;
			   		$posx= $posxvent;	
			
			   	default:
			   	  echo("Unknown family: " @ %file);

			   }
			   
			   /*datablock StaticShapeData(%instrument) 
			   {   
				   shapeFile = "art/shapes/virtuose/" @ %instrument @ ".dae";
				   junkvar = "helloworld";
				   name=%instrument;
			   };*/
		
		
			   //Handling case where the instrument has not been yet modelised in 3D
			   %file3DName = "art/shapes/virtuose/" @ strlwr(%file) @ ".dae";
			   %fileExist = getFileCount(%file3DName, false);
			   if (%fileExist <= 0)
			   {
			     //We try with an 's' at the end!
			     %file3DName = "art/shapes/virtuose/" @ strlwr(%file) @ "s.dae";
			     %fileExist = getFileCount(%file3DName);
			     if (%fileExist < 0)
			     {
			       %file3DName = "art/shapes/virtuose/timpani.dae";
			     }
			   }
			   echo("Final name : " @ %file3DName);
			   new TSStatic(%nameInstru) 

			   {
					shapeName = %file3DName;

					name=%nameInstru;
					internalName=%nameInstru;
				   //dataBlock = %instrument;
	
					position = $posx+%famille.position.x SPC %famille.position.y SPC %famille.position.z;
					particule="";
				   rotation = "1 90 0 0";
				   scale = "1 1 1";
				   //name=%instrument;
					timeLeft=0;
					distanceLeft=1;
			   };
		   
}

function placeInstrumentsTrack(){
	for(%i=0;%i<$orchestrator.getNumTracks();%i++){
		createInstruments($orchestrator.getTrackName(%i),$orchestrator.getInstrumentName(%i));
	}
}