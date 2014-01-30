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

 //Initialisation
function virtuoseStart(){
	echo("Virtuose is beginning");
	HideSun();
    leftHand.position = Spawn.position.x-2 SPC Spawn.position.y+5 SPC Spawn.position.z+1;
    rightHand.position = Spawn.position.x+2 SPC Spawn.position.y+5 SPC Spawn.position.z+1;
	
	//placeInstrumentsTrack();
}
  
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
			   	case "harpe":
			   		%famille = "corde";	
			   		$posxcorde ++;
			   		$posx= $posxcorde;
			   	case "contrebass":
			   		%famille = "corde";
			   		$posxcorde ++;
			   		$posx= $posxcorde;
			   	case "bassElec":
			   		%famille = "corde";
			   		$posxcorde ++;
			   		$posx= $posxcorde;		
			   	case "Guitar":
			   		%famille = "corde";
			   		$posxcorde ++;
			   		$posx= $posxcorde;	
			   	case "Guitar2":
			   		%famille = "corde";
			   		$posxcorde ++;
			   		$posx= $posxcorde;		
			   	case "violon":
			   		%famille = "corde";
			   		$posxcorde ++;
			   		$posx= $posxcorde;	
			   	case "drum":
			   		%famille = "percussion";
			   		$posxpercu ++;
			   		$posx= $posxpercu;	
			   	case "saxo":
			   		%famille = "bois";
			   		$posxbois ++;
			   		$posx= $posxbois;
			   	case "flutetraversiere":
			   		%famille = "bois";
			   		$posxbois ++;
			   		$posx= $posxbois;	
			   	case "contrebass":
			   		%famille = "bois";
			   		$posxbois ++;
			   		$posx= $posxbois;	
			   	case "trombone":
			   		%famille = "cuivre";
			   		$posxcuivre ++;
			   		$posx= $posxcuivre;
			   	case "trumpet":
			   		%famille = "cuivre";
			   		$posxcuivre ++;
			   		$posx= $posxcuivre;			

			   }
			   
			   
			   new StaticShape() 
			   {
				   dataBlock = %instrument;
	
				  position = $posx+%famille.position.x SPC %famille.position.y SPC %famille.position.z;

				   rotation = "1 90 0 0";
				   scale = "1 1 1";
				   name=%instrument;

			   };
			  

			   $posy-=0.5;
			   echo ($sens);
			   	echo($signe*$posx+start.position.x);

			   switch$($sens){
			   	case "right":
			   	$signe=1.0;

			   	$sens = "left";


			   	case "left":
			   	$signe=-1.0;
			   	$posx+=2.0;

			   	$sens="right";


			   }

}

function placeInstrumentsTrack(){
	for(%i=0;%i<$orchestrator.getNumTracks();%i++){
		echo($orchestrator.getInstrumentName(%i));
		//createInstruments($orchestrator.getInstrumentName(%i));
	}
}