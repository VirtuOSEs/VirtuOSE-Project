// Le script du projet Virtuose
exec( "scripts/virtuose/baguettes.cs" );
exec( "scripts/virtuose/modifObjects.cs" );

function placeInstruments(%nbInstruments){
	for(%i=0;%i<%nbInstruments;%i++){
		echo($InstrumentsScene[%i]);
	}
}

function placeInstrumentsType(%type){
	if(strcmp(%type,"pop rock")==0){
		$InstrumentsScene[0]="guitare";
		$InstrumentsScene[1]="batterie";
		$InstrumentsScene[2]="basse";
		$InstrumentsScene[3]="chant";
		$InstrumentsScene[4]="guitare";
		placeInstruments(5);
	}
	else if(strcmp(%type,"classique")==0){
		$InstrumentsScene[0]="violon";
		$InstrumentsScene[1]="flute";
		$InstrumentsScene[2]="cymbale";
		$InstrumentsScene[3]="piano";

		placeInstruments(4);
	}
}