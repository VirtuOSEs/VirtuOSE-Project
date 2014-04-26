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

function changeColor(%obj,%var){
	%mapTo=%obj.getTargetName(0);
	
	%r=%var;
	%g=1-%var;
	%b=1-%var;
	
	%mapTo.diffuseColor.r=%r;
	%mapTo.diffuseColor.g=%g;
	%mapTo.diffuseColor.b=%b;
	
	// %r=%newVelocity;
	// %g=1-%newVelocity;
	// %b=1-%newVelocity;
	
    rightHandParticle.colors[0]="1 1 1 1";
	rightHandParticle.colors[2]=%r/2 SPC %g/2 SPC %b/2 SPC "0.62";
	rightHandParticle.colors[1]=%r SPC %g SPC %b SPC "0.629";
	rightHandParticle.colors[3]="0.724 1 0 0";
	
	%mapTo.reload();
	
}

function particle(){
	rightHandParticleNode.setActive(!rightHandParticleNode.active);
	leftHandParticleNode.setActive(!leftHandParticleNode.active);
	
	// leftHandParticle.colors[0]="1 1 1 1";
	// leftHandParticle.colors[1]="0 1 0.627 0.62";
	// leftHandParticle.colors[2]="0 0.071 1 0.629";
	// leftHandParticle.colors[3]="0.724 1 0 0";
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
		
	changeColor("rightHand",%newVelocity);
}

function onInstrumentWillPlay(%instrumentName, %delayInMillis) 
{
  echo(%instrumentName @ " will play in " @ %delayInMillis);
}

function onInstrumentStartPlaying(%instrumentName)
{
  echo(%instrumentName @ " starts playing");
}

function onInstrumentStoppedPlaying(%instrumentName)
{
  echo(%instrumentName @ " stops playing");
}