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

 