// Le script du projet Virtuose
$handSelected="rightHand";
$relache=0;

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

function mainDroiteHit(%x,%y,%z){
	//Epaisseur de 2
	if((%x+2+Spawn.position.x)>=rightHand.position.x && (%x-2+Spawn.position.x)<=rightHand.position.x && (%y+2+Spawn.position.y)>=rightHand.position.y && (%y-2+Spawn.position.y)<=rightHand.position.y && (%z+2+Spawn.position.z)>=rightHand.position.z && (%z-2+Spawn.position.z)<=rightHand.position.z){
		%t=getTimeSinceStart(rightHand.sched);
		if(%t==0){
			//Demarre nouveau schedule
			rightHand.sched=schedule(5000,rightHand.getId(), "Hit");
		}
	}
	else{
		cancel(rightHand.sched);
	}
}

function Hit(){
	echo("Hit");
}

//Fonctions avec la souris
function changeHand(){
	if($relache==1){
		if(strcmp($handSelected,"rightHand")==0){
			$handSelected="leftHand";
		}
		else if(strcmp($handSelected,"leftHand")==0){
			$handSelected="rightHand";
		}
		echo($handSelected);
		$relache=0;
	}
	else {
		$relache=1;
	}
	
}

function handMoveX(%val){
   %x = getMouseAdjustAmount(%val);
   //$handSelected.position.x+=%x;
}

function handMoveY(%val){
   %z = getMouseAdjustAmount(%val);
   //$handSelected.position.z-=%z;
}

function handMoveZ(%val)
{
  /*
	if(%val>0)
		$handSelected.position.y+=0.5;
	else
		$handSelected.position.y-=0.5;
*/
}