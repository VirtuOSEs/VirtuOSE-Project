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
				   position = pos SPC "0.0 0.0";
				   rotation = "1 0 0 0";
				   scale = "20 20 20";
				   name=%instrument;
			   };
			   
			   if(strcmp(%instrument.shapeFile,"")==0){
					%instrument.shapeFile="art/shapes/virtuose/drum.dae";
			   }
			   $pos+=2.0;
			
		  // }
}

function placeInstrumentsTrack(){
	
	for(%i=0;%i<$midiPlayer.getNumTracks();%i++){
		//echo($midiPlayer.getInstrumentName(%i));
		createInstruments($midiPlayer.getInstrumentName(%i));
	}
}