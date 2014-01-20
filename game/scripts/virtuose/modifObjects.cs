// Le script du projet Virtuose

function changeOpacity(%obj,%a){
	
	%mapTo=%obj.getTargetName(0);//
	%mapTo.diffuseColor.a=%a;
	%mapTo.reload();
	
	if(%a>=1)
		%mapTo.translucent=0;
	else
		%mapTo.translucent=1;
}