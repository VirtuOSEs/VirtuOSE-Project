// Le script du projet Virtuose

function test(){
	echo("Position du Spawn");
	echo(Spawn.position);
}

function virtuoseStart(){
	echo("Virtuose Start");
    leftHand.position = Spawn.position.x-2 SPC Spawn.position.y+5 SPC Spawn.position.z+1;
    rightHand.position = Spawn.position.x+2 SPC Spawn.position.y+5 SPC Spawn.position.z+1;
}

function mainDroite(%x, %y, %z){
	rightHand.position = Spawn.position.x+%x SPC Spawn.position.y+%y SPC Spawn.position.z+%z;
}

function mainGauche(%x, %y, %z){
	leftHand.position = Spawn.position.x+%x SPC Spawn.position.y+%y SPC Spawn.position.z+%z;
}