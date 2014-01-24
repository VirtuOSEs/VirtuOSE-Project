// Le script du projet Virtuose
exec( "scripts/virtuose/baguettes.cs" );
exec( "scripts/virtuose/modifObjects.cs" );

	   
function placeInstruments(%nbInstruments){
	for(%i=0;%i<%nbInstruments;%i++){
		$InstrumentsScene[%i].position.y+=5;
	}
}

function placeInstrumentsType(%type){
	if(strcmp(%type,"pop rock")==0){
		$InstrumentsScene[0]="acousticGuitar";
		$InstrumentsScene[1]="drum";
		$InstrumentsScene[2]="bass";
		$InstrumentsScene[3]="electricGuitar";
		placeInstruments(4);
	}
	else if(strcmp(%type,"classique")==0){
		$InstrumentsScene[0]="piano";
		$InstrumentsScene[1]="flute";
		$InstrumentsScene[2]="trombone";
		$InstrumentsScene[3]="violon";
		$InstrumentsScene[4]="doubleBass";
		$InstrumentsScene[5]="harp";
		$InstrumentsScene[6]="sax";
		$InstrumentsScene[7]="trumpet";


		placeInstruments(8);
	}
}
		
function placeInstrumentsTrack(){
	
	for(%i=0;%i<$midiPlayer.getNumTracks();%i++){
		//echo($midiPlayer.getInstrumentName(%i));
		createInstruments($midiPlayer.getInstrumentName(%i));
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
				   position = "0.0 0.0 0.0";
				   rotation = "1 0 0 0";
				   scale = "20 20 20";
				   name=%instrument;
			   };
		  // }
}