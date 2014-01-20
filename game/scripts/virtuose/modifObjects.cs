// Le script du projet Virtuose

function changeOpacity(%obj,%a){
	echo(%obj.color);
	%obj.color=%obj.color.r SPC %obj.color.g SPC %obj.color.b SPC %a;
}