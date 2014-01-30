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

	   
function placeInstruments(%nbInstruments){
	for(%i=0;%i<%nbInstruments;%i++){
		$InstrumentsScene[%i].position.y+=5;
	}
}

function createInstruments(%instrument){
				

				datablock StaticShapeData(%instrument) 
			   {   
				   shapeFile = "art/shapes/virtuose/" @ %instrument @ ".dae";
				   junkvar = "helloworld";
			   };
			
			  switch$(%instrument){
			   	case "piano":
			   		%famille = "corde";
			   		$posxcorde ++;
			   		$posx= $posxcorde;
			   	case "harp":
			   		%famille = "corde";	
			   		$posxcorde ++;
			   		$posx= $posxcorde;
			   	case "electricbass":
			   		%famille = "corde";
			   		$posxcorde ++;
			   		$posx= $posxcorde;		
			   	case "Guitar":
			   		%famille = "corde";
			   		$posxcorde ++;
			   		$posx= $posxcorde;	
			   	case "acousticguitar":
			   		%famille = "corde";
			   		$posxcorde ++;
			   		$posx= $posxcorde;		
			   	case "violins":
			   		%famille = "corde";
			   		$posxcorde ++;
			   		$posx= $posxcorde;	
			   	case "cellos":
			   		%famille = "corde";
			   		$posxcorde ++;
			   		$posx= $posxcorde;	
			   	case "violas":
			   		%famille = "corde";
			   		$posxcorde ++;
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
			   	case "bass":
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
			   
			   
			   new StaticShape() 
			   {
				   dataBlock = %instrument;
	
				  position = $posx+%famille.position.x SPC %famille.position.y SPC %famille.position.z;

				   rotation = "1 90 0 0";
				   scale = "1 1 1";
				   name=%instrument;

			   };

			   
}



function placeInstrumentsTrack(){
	for(%i=1;%i<$orchestrator.getNumTracks();%i++){
		//echo($orchestrator.getInstrumentName(%i));
		createInstruments($orchestrator.getInstrumentName(%i));
	}
}

//Initialisation
function virtuoseStart(){
	echo("Virtuose Start");
    leftHand.position = Spawn.position.x-2 SPC Spawn.position.y+5 SPC Spawn.position.z+1;
    rightHand.position = Spawn.position.x+2 SPC Spawn.position.y+5 SPC Spawn.position.z+1;
	soleil.setHidden();
	//placeInstrumentsTrack();
}