// Le script du projet Virtuose

function placeInstrument(%nbInstruments,%Instruments){
	for(%i=0;%i<%nbInstruments;%i++){
		echo(Instruments[%i]);
	}
}

function placeInstrument(%type){
	if(%type=="pop rock"){
		%Instruments[0]="guitare";
		%Instruments[1]="batterie";
		%Instruments[2]="basse";
		%Instruments[3]="chant";
		%Instruments[4]="guitare";
		placeInstrument(5,%Instruments);
	}
	else if(%type=="classique"){
		%Instruments[0]="violon";
		%Instruments[1]="flute";
		%Instruments[2]="cymbale";
		%Instruments[3]="piano";
		placeInstrument(4,%Instruments);
	}
}