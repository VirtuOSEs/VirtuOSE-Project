// Le script du projet Virtuose

$nbAppels=100; //Nombre d'appels pour les schedule (plus c'est grand, plus le mouvement est realiste)
$play=0; // 0 : pause, 1 : play

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
	if(strcmp($orchestrator.options.handedness,LEFT_HANDEDNESS)==0){
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
	
	if(strcmp($orchestrator.options.handedness,RIGHT_HANDEDNESS)==0){
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
		
		$particuleVeloTmp.position=%obj.position.x SPC %obj.position.y SPC %obj.position.z-1;
		
		
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
}

function TSStatic::liftInstrument(%instrumentName,%z,%timeLeft,%distanceLeft){
	%instrumentName.position=%instrumentName.position.x SPC %instrumentName.position.y SPC (%instrumentName.position.z+%z);
	%instrumentName.timeLeft=%timeLeft;
	%instrumentName.distanceLeft=%distanceLeft;
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
  if(strcmp($orchestrator.options.handedness,lEFT_HANDEDNESS)==0){
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
	if(strcmp($orchestrator.options.handedness,lEFT_HANDEDNESS)==0){
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
  if(strcmp($orchestrator.options.handedness,lEFT_HANDEDNESS)==0){
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
  echo("Expression: " @ %newExpression);
    changeVelocity(%newExpression);
}

function onInstrumentWillPlay(%instrumentName, %delayInMillis) 
{
$play=1;
  echo(%instrumentName @ " will play in " @ %delayInMillis);
	  %w=1;
	  %distance=%instrumentName.distanceLeft;
	  
	  if(%distance==0 || %delayInMillis==0)
		return;
		
  for(%i=0;%i<%delayInMillis;%i=%i+%delayInMillis/$nbAppels){
		// if(%distance-%w/($nbAppels)<0)
			// break;
		%instrumentName.sched[%w]=%instrumentName.schedule(%i,"liftInstrument", %distance/($nbAppels),%delayInMillis-%i,%distance-(%w*%distance)/($nbAppels));
		%w++;
	}
	%instrumentName.sched[0]=%w-1; // Le premier indique le nombre de schedule appelés
}

function onInstrumentStartPlaying(%instrumentName)
{
$play=1;
  echo(%instrumentName @ " starts playing");
  %pos=%instrumentName.position.x SPC %instrumentName.position.y SPC %instrumentName.position.z;
  
  %particle=$playParticleNode.clone();
  %particle.position=%pos;
  %particle.active=true;
  
  %instrumentName.particle=%particle;
  %instrumentName.distanceLeft=0;
  %instrumentName.timeLeft=0;
}

function onInstrumentStoppedPlaying(%instrumentName)
{
$play=1;
  echo(%instrumentName @ " stops playing");
  //changeOpacity(%instrumentName,1);
  if(%instrumentName.particle!$="")
	%instrumentName.particle.delete();
	
  %delayInMillis=2000;
	%distance=%instrumentName.position.z-%instrumentName.hightOrigin;
	echo(%instrumentName.position.z SPC %instrumentName.hightOrigin);
	if(%distance <=0) return;
	
  for(%i=0;%i<%delayInMillis;%i=%i+%delayInMillis/$nbAppels){
		%instrumentName.schedule(%i,"liftInstrument", -%distance/($nbAppels),0,1);
		
	}
%instrumentName.distanceLeft=1;
}

function onPlay()
{
if($play!=0) return;
$play=1;
  echo("PLAY");
  
  for(%i=0;%i<$orchestrator.getNumTracks();%i++){
	 onInstrumentWillPlay($orchestrator.getTrackName(%i),$orchestrator.getTrackName(%i).timeLeft);
		 if($orchestrator.getTrackName(%i).wasPlaying==1){
			echo($orchestrator.getTrackName(%i));
			  $orchestrator.getTrackName(%i).particle=$playParticleNode.clone();
			  $orchestrator.getTrackName(%i).particle.position=$orchestrator.getTrackName(%i).position.x SPC $orchestrator.getTrackName(%i).position.y SPC $orchestrator.getTrackName(%i).position.z;
			  $orchestrator.getTrackName(%i).particle.active=true;
			  
		 }
	 }
}

function onPause()
{
if($play!=1) return;
$play=0;
  echo("PAUSE");
  	for(%i=0;%i<$orchestrator.getNumTracks();%i++){
	 for(%j=1;%j<=$orchestrator.getTrackName(%i).sched[0];%j++){
		cancel($orchestrator.getTrackName(%i).sched[%j]);
		$orchestrator.getTrackName(%i).sched[%j]="";
	  }
	  $orchestrator.getTrackName(%i).sched[0]=0;
	  if($orchestrator.getTrackName(%i).particle!$=""){
		$orchestrator.getTrackName(%i).particle.delete();
		$orchestrator.getTrackName(%i).wasPlaying=1;
		$orchestrator.getTrackName(%i).particle="";
		
		}
	  else {
		$orchestrator.getTrackName(%i).wasPlaying=0;
	  }
  }
  
  %mapTo=leftHand.getTargetName(0);
	%mapTo.diffuseColor.r=0.5;
	%mapTo.diffuseColor.g=0.5;
	%mapTo.diffuseColor.b=0.5;
	%mapTo.reload();
	
	%mapTo=rightHand.getTargetName(0);
	%mapTo.diffuseColor.r=0.5;
	%mapTo.diffuseColor.g=0.5;
	%mapTo.diffuseColor.b=0.5;
	%mapTo.reload();
	
}

function onStop()
{
  echo("STOP");
	$posx = 0.0;
	$posy = 0.0;
	$posz = 0.0;
	$posxcorde = 0.0;
	$posxpercu = 0.0;
	$posxbois = 0.0;
	$posxcuivre = 0.0;
	$posxvent = 0.0;
}
