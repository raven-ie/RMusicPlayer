/**
 * Copyright 2008 Raven
 */
// Controls RMusic_Player.
class RMusic_ControllerEx extends RMusic_Controller;

var() RMusic_ControllerEx NextController;

simulated function EVENT_Player(optional PlayerPawn PP)
{
	local RMusic_Player RMusic_Player;

	Super.EVENT_Player(PP);

	if(PP == none) RMusic_Player = Find_RMusicPlayer(self, PlayerClass);
	else RMusic_Player = Find_RMusicPlayerByPPawn(PP, PlayerClass);
	if( RMusic_Player != none ) RMusic_Player.setEvent(NextController);
}


defaultproperties
{
	//DO NOT CHANGE!!
	RMusic_PlayType=PT_PlayOnce
}