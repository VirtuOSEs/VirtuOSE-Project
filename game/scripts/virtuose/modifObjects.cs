// Le script du projet Virtuose

function changeOpacity(%obj,%a){
	// return;
	%mapTo=%obj.getTargetName(0);
	%mapTo.diffuseColor.a=%a;
	%mapTo.reload();
	
	// if(%a>=1)
		// %mapTo.translucent=0;
	// else
		// %mapTo.translucent=1;
}

function decrementOpacity(%obj){
	%mapTo=%obj.getTargetName(0);
	
	while(%mapTo.diffuseColor.a>0){
		changeOpacity(%obj,%mapTo.diffuseColor.a-0.01);
	}
	
}

function hide(%obj){
	%obj.hidden=!%obj.hidden;
}

function changeTempo(%r, %g, %b){
	if(strcmp($orchestrator.options.handedness,"LEFT_HANDEDNESS")==0){
		%obj="rightHand";	
	}
	else {
		%obj="leftHand";
	}
	
	%mapTo=%obj.getTargetName(0);
	%mapTo.diffuseColor.r=%r;
	%mapTo.diffuseColor.g=%g;
	%mapTo.diffuseColor.b=%b;
}

function changeVelocity(%var){
	
	if(strcmp($orchestrator.options.handedness,"RIGHT_HANDEDNESS")==0){
		%obj=leftHand;
	}
	else {
		%obj=rightHand;
	}
	
		if($particuleVeloTmp!$="") return;
		
		$particuleVeloTmp=$velocityParticleNode.clone();
		$particuleVeloTmp.active=true;
		%mapTo=%obj.getTargetName(0);
		
		%r=%var;
		%g=1-%var;
		%b=1-%var;
		
		%mapTo.diffuseColor.r=%r;
		%mapTo.diffuseColor.g=%g;
		%mapTo.diffuseColor.b=%b;
		
		//$velocityParticleNode.active=!$velocityParticleNode.active;
		
		// $velocityParticleNode.setHidden(true);
		//$velocityParticleNode.emitter.setHidden(true);
		// $velocityParticleNode.emitter.particles[0].setHidden(true);
		
		$particuleVeloTmp.position=%obj.position.x SPC %obj.position.y SPC %obj.position.z-1;
		//$velocityParticleNode.setScale($velocityParticleNode.getScale());
		
		for(%i=0;%i<4;%i++){
			if($particuleVeloTmp.emitter.particles[%i]$="")
				continue;
			$particuleVeloTmp.emitter.particles[%i].colors[0]="1 1 1 1";
			$particuleVeloTmp.emitter.particles[%i].colors[1]=%r SPC %g SPC %b SPC "0.629";
			$particuleVeloTmp.emitter.particles[%i].colors[2]=%r/2 SPC %g/2 SPC %b/2 SPC "0.62";
			$particuleVeloTmp.emitter.particles[%i].colors[3]="0.724 1 0 0";
			$particuleVeloTmp.emitter.particles[%i].position=$velocityParticleNode.position;
			$particuleVeloTmp.emitter.particles[%i].reload();
		}
		$particuleVeloTmp.emitter.reload();
		

		//Refresh à trouver
		%mapTo.reload();
	
		
		/*echo(%obj.position);
		echo($velocityParticleNode.position);
		echo($velocityParticleNode.emitter.position);
		echo($velocityParticleNode.emitter.particles[0].position);*/
	//particle();
		%obj.schedule(500,"deleteParticle");
		
}

function TSStatic::deleteParticle(){
	$particuleVeloTmp.delete();
	$particuleVeloTmp="";
	//echo("Boom");
}

function TSStatic::liftInstrument(%instrumentName,%z){
	%instrumentName.position=%instrumentName.position.x SPC %instrumentName.position.y SPC (%instrumentName.position.z+%z);
}

function light(){
	instruLight.hidden=!instruLight.hidden;
}

function HideSun(){
	soleil.setHidden(!soleil.hidden);	
}

function onTempoGestureStart()
{
  echo("Tempo Gesture Start");
  if(strcmp($orchestrator.options.handedness,"lEFT_HANDEDNESS")==0){
		%obj="leftHand";
	}
	else {
		%obj="rightHand";
	}
	
	%mapTo=%obj.getTargetName(0);
	
	%mapTo.diffuseColor.r=0;
	%mapTo.diffuseColor.g=0;
	%mapTo.diffuseColor.b=1;
	
	%mapTo.reload();
}

function TSStatic::whiteOut(){
	if(strcmp($orchestrator.options.handedness,"lEFT_HANDEDNESS")==0){
		%obj="leftHand";
	}
	else {
		%obj="rightHand";
	}
	
	%mapTo=%obj.getTargetName(0);
	
	%mapTo.diffuseColor.r=0.5;
	%mapTo.diffuseColor.g=0.5;
	%mapTo.diffuseColor.b=0.5;
	
	%mapTo.reload();
	$particuleTempoTmp.delete();	
}

function onTempoGestureEnd()
{
  echo("Tempo Gesture End");
  if(strcmp($orchestrator.options.handedness,"lEFT_HANDEDNESS")==0){
		%obj="leftHand";
	}
	else {
		%obj="rightHand";
	}
	
	$particuleTempoTmp=$TempoParticleNode.clone();
		
	%mapTo=%obj.getTargetName(0);
	
	%mapTo.diffuseColor.r=1;
	%mapTo.diffuseColor.g=0;
	%mapTo.diffuseColor.b=0;
	
	%mapTo.reload();
	$particuleTempoTmp.position=%obj.position;
	$particuleTempoTmp.active=true;
	%obj.schedule(100,"whiteOut");
}


function onTempoJustChanged(%newTempo)
{
  echo("Tempo just changed " @ %newTempo);
}

function onVelocityChanged(%newVelocity)
{
   echo("Velocity just changed " @ %newVelocity); 
		
	changeVelocity(%newVelocity);
}

function onExpressionChanged(%newExpression)
{
    changeVelocity(%newExpression);
}

function onInstrumentWillPlay(%instrumentName, %delayInMillis) 
{
  echo(%instrumentName @ " will play in " @ %delayInMillis);
	%nombreAppels=100;
  for(%i=0;%i<%delayInMillis;%i=%i+%delayInMillis/%nombreAppels){
		%instrumentName.schedule(%i,"liftInstrument", 1/(%nombreAppels));
	}

}

function onInstrumentStartPlaying(%instrumentName)
{
  echo(%instrumentName @ " starts playing");
}

function onInstrumentStoppedPlaying(%instrumentName)
{
  echo(%instrumentName @ " stops playing");
  //changeOpacity(%instrumentName,1);
  %delayInMillis=2000;
  %nombreAppels=100;
  for(%i=0;%i<%delayInMillis;%i=%i+%delayInMillis/%nombreAppels){
		%instrumentName.schedule(%i,"liftInstrument", -1/(%nombreAppels));
	}
}
