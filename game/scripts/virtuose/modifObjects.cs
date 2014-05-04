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

function turnOff(){
echo("Hello");
	velocityParticleNode.setActive(false);

}

function particle(){
	
	velocityParticleNode.setHidden(true);	
	velocityParticleNode.setActive(false);
	velocityEmitter.reload();
	velocityParticleNode.setHidden(false);	
	velocityParticleNode.setActive(true);
	velocityEmitter.reload();
}

function changeVelocity(%var){
	
	if(strcmp($orchestrator.options.handedness,"RIGHT_HANDEDNESS")==0){
		%obj=leftHand;
	}
	else {
		%obj=rightHand;
	}
		
		%mapTo=%obj.getTargetName(0);
		
		%r=%var;
		%g=1-%var;
		%b=1-%var;
		
		%mapTo.diffuseColor.r=%r;
		%mapTo.diffuseColor.g=%g;
		%mapTo.diffuseColor.b=%b;
		
		//velocityParticleNode.active=!velocityParticleNode.active;
		
		// velocityParticleNode.setHidden(true);
		// velocityParticleNode.emitter.setHidden(true);
		// velocityParticleNode.emitter.particles[0].setHidden(true);
		
		velocityParticleNode.position=%obj.position;
		
		for(%i=0;%i<4;%i++){
			if(velocityParticleNode.emitter.particles[%i]$="")
				continue;
			velocityParticleNode.emitter.particles[%i].colors[0]="1 1 1 1";
			velocityParticleNode.emitter.particles[%i].colors[1]=%r SPC %g SPC %b SPC "0.629";
			velocityParticleNode.emitter.particles[%i].colors[2]=%r/2 SPC %g/2 SPC %b/2 SPC "0.62";
			velocityParticleNode.emitter.particles[%i].colors[3]="0.724 1 0 0";
			velocityParticleNode.emitter.particles[%i].position=velocityParticleNode.position;
			velocityParticleNode.emitter.particles[%i].reload();
		}
		velocityParticleNode.emitter.reload();
		
		//Refresh à trouver
		%mapTo.reload();
	
		
		echo(%obj.position);
		echo(velocityParticleNode.position);
		echo(velocityParticleNode.emitter.position);
		echo(velocityParticleNode.emitter.particles[0].position);
	//particle();
	
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
	
	%mapTo=%obj.getTargetName(0);
	
	%mapTo.diffuseColor.r=1;
	%mapTo.diffuseColor.g=0;
	%mapTo.diffuseColor.b=0;
	
	%mapTo.reload();
	TempoParticleNode.position=%obj.position;
	TempoParticleNode.active=true;
	%obj.schedule(300,"whiteOut");
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
	velocityParticleNode.setHidden(true);
    changeVelocity(%newExpression);
    velocityParticleNode.setHidden(false);
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
  %instrumentName.liftInstrument(1);
  //changeOpacity(%instrumentName,0.5);
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
