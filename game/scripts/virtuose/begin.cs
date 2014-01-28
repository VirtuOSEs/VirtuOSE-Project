// Le script du projet Virtuose
exec( "scripts/virtuose/baguettes.cs" );
exec( "scripts/virtuose/modifObjects.cs" );

$posx = 0.0;
$posy = 0.0;
$posz = 0.0;
$signe= 1.0;

$sens="left";
	   
function placeInstruments(%nbInstruments){
	for(%i=0;%i<%nbInstruments;%i++){
		$InstrumentsScene[%i].position.y+=5;
	}
}

function createInstruments(%instrument){
				
		// if(strcmp(%instrument,"batterie")==0){
				datablock StaticShapeData(%instrument) 
			   {   
				   shapeFile = "art/shapes/virtuose/" @ %instrument @ ".dae";   
				   junkvar = "helloworld";
			   };

			   switch$(%instrument){
			   	case "piano":
			   		%famille = "corde";
			   	case "harpe":
			   		%famille = "corde";	
			   	case "contrebass":
			   		%famille = "corde";
			   	case "bassElec":
			   		%famille = "corde";		
			   	case "Guitar":
			   		%famille = "corde";	
			   	case "Guitar2":
			   		%famille = "corde";		
			   	case "violon":
			   		%famille = "corde";	
			   	case "drum":
			   		%famille = "percussion";	
			   	case "saxo":
			   		%famille = "bois";
			   	case "flutetraversiere":
			   		%famille = "bois";	
			   	case "contrebass":
			   		%famille = "bois";	
			   	case "trombone":
			   		%famille = "cuivre";
			   	case "trumpet":
			   		%famille = "cuivre";			

			   }
			   
			   
			   new StaticShape() 
			   {
				   dataBlock = %instrument;
	
				   		position = $signe*$posx+%famille.position.x SPC $posy+%famille.position.y SPC %famille.position.z;

				   rotation = "1 90 0 0";
				   scale = "1 1 1";
				   name=%instrument;

			   };
			   
			   // Create a file stream object for reading
				%fsObject = new FileStreamObject();

				// Open a file for reading
				%fsObject.open(%instrument.shapeFile, "read");

				// Get the status and print it
				%status = %fsObject.getStatus();
				echo(%status);
				if(%status==IOError){
					%instrument.shapeFile="art/shapes/virtuose/drum.dae";
			   }
			   %fsObject.close();

			 /*  $posy-=0.5;
			   $posz+=0.5;
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


			   }*/


			
		  // }
}

function placeInstrumentsTrack(){
	
	for(%i=0;%i<$midiPlayer.getNumTracks();%i++){
		echo($midiPlayer.getInstrumentName(%i));
		//createInstruments($midiPlayer.getInstrumentName(%i));
	}
}