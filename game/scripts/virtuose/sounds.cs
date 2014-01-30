// Le script du projet Virtuose
$playing=false;
$relache=0;

function playPause(){
	if($relache==1){
		if($playing){
			$orchestrator.pause();
		}
		else if(!$playing){
			$orchestrator.play();
		}
		$playing=!$playing;
		$relache=0;
	}
	else {
		$relache=1;
	}
}