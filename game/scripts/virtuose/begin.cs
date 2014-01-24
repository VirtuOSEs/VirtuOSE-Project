// Le script du projet Virtuose
exec( "scripts/virtuose/baguettes.cs" );
exec( "scripts/virtuose/modifObjects.cs" );

$pos = 0.0;
	   
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
			   
			   new StaticShape() 
			   {
				   dataBlock = %instrument;
				   position = pos+Spawn.position.x SPC Spawn.position.y SPC Spawn.position.z;
				   rotation = "1 0 0 0";
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
			   $pos+=2.0;
			
		  // }
}

function placeInstrumentsTrack(){
	
	for(%i=0;%i<$midiPlayer.getNumTracks();%i++){
		//echo($midiPlayer.getInstrumentName(%i));
		createInstruments($midiPlayer.getInstrumentName(%i));
	}
}