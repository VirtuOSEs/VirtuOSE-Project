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
			   		%famille = "corde";
			   		$posxcorde +=1.5;
			   		$posx= $posxcorde;	
			   	case "cellos":
			   		%famille = "corde";
			   		$posxcorde +=1.5;
			   		$posx= $posxcorde;	
			   	case "violas":
			   		%famille = "corde";
			   		$posxcorde +=1.5;
			   		$posx= $posxcorde;
			   	case "bass":
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
			   		%famille = "bois";
			   		$posxbois ++;
			   		$posx= $posxbois;	
			   	case "bassoons":
			   		%famille = "bois";
			   		$posxbois ++;
			   		$posx= $posxbois;	
			   	case "trombone":
			   		%famille = "cuivre";
			   		$posxcuivre ++;
			   		$posx= $posxcuivre;
			   	case "trumpets":
			   		%famille = "cuivre";
			   		$posxcuivre ++;
			   		$posx= $posxcuivre;
			   	case "flutes":
			   		%famille = "vent";
			   		$posxvent ++;
			   		$posx= $posxvent;
			   	case "clarinets":
			   		%famille = "vent";
			   		$posxvent ++;
			   		$posx= $posxvent;
			   	case "horns":
			   		%famille = "vent";
			   		$posxvent ++;
			   		$posx= $posxvent;	
			
		

			   }
			   
			   /*datablock StaticShapeData(%instrument) 
			   {   
				   shapeFile = "art/shapes/virtuose/" @ %instrument @ ".dae";
				   junkvar = "helloworld";
				   name=%instrument;
			   };*/
			   
			   new TSStatic(%nameInstru) 
			   {
					shapeName = "art/shapes/virtuose/" @ %file @ ".dae";

					name=%nameInstru;
					internalName=%nameInstru;
				   //dataBlock = %instrument;
	
				  position = $posx+%famille.position.x SPC %famille.position.y SPC %famille.position.z;

				   rotation = "1 90 0 0";
				   scale = "1 1 1";
				   //name=%instrument;
					
			   };
		   
}

function placeInstrumentsTrack(){
	for(%i=0;%i<$orchestrator.getNumTracks();%i++){
		//echo($orchestrator.getTrackName(%i));
		//echo($orchestrator.getInstrumentName(%i));
		createInstruments($orchestrator.getTrackName(%i),$orchestrator.getInstrumentName(%i));
		
	}
}