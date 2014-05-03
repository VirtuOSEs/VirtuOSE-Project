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
	
	if(strcmp($orchestrator.options.handedness,"LEFT_HANDEDNESS")==0){
		%obj="leftHand";
	}
	else {
		%obj="rightHand";
	}
	
	%mapTo=%obj.getTargetName(0);
	
	%r=%var;
	%g=1-%var;
	%b=1-%var;
	
	%mapTo.diffuseColor.r=%r;
	%mapTo.diffuseColor.g=%g;
	%mapTo.diffuseColor.b=%b;
	
	
	velocityParticle.colors[0]="1 1 1 1";
	velocityParticle.colors[2]=%r/2 SPC %g/2 SPC %b/2 SPC "0.62";
	velocityParticle.colors[1]=%r SPC %g SPC %b SPC "0.629";
	velocityParticle.colors[3]="0.724 1 0 0";
	velocityParticleNode.position=%obj.position;
	
	//Refresh à trouver
	velocityEmitter.reload();
	%mapTo.reload();
	//particle();
	
}

function liftInstrument(%instrumentName,%y){
	%instrumentName.position.y+=%y;
	echo("Hello");
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
}

function onTempoGestureEnd()
{
  echo("Tempo Gesture End");
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

  //for(%i=0;%i<2;%i+=0.2)
		%instrumentName.schedule(10,"liftInstrument",%instrumentName, 1);

}

function onInstrumentStartPlaying(%instrumentName)
{
  echo(%instrumentName @ " starts playing");
  changeOpacity(%instrumentName,0.5);
}

function onInstrumentStoppedPlaying(%instrumentName)
{
  echo(%instrumentName @ " stops playing");
  changeOpacity(%instrumentName,1);
}

/*function Instrument::liftInstrument(){

}*/
