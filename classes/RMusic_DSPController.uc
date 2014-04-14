/**
 * Copyright 2008 Raven
 */
// Adds/deletes DSP.
class RMusic_DSPController extends RMusic_Component;

var() class<RMusic_Player> PlayerClass;	// Player class (if you want to have own music directory)
var() bool RMusic_BroadcastToAll;	// If true, it'll broadcast functions to all players
var() bool RMusic_bUnload;		// Unloads all DSP plugins
var() string RMusic_UnloadPlugin;	// plugin to unload
var() string RMusic_DSPPlugin;		// DSP plugins to load (not used, left because of compatibility reasons)
var() bool RMusic_bUpdateParameters;	// If true will update all parameters
var() float RMusic_DSPParameters[32];	// DSP parameters

replication
{
	reliable if( Role==ROLE_Authority && RMusic_BroadcastToAll)
		EVENT_AddDSP;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Used to play music
 */
simulated function EVENT_AddDSP()
{
	local RMusic_Player RMusic_Player;
	local int i;
	local bool bHasDSP;

	if(Level.NetMode == NM_DedicatedServer) return;

	RMusic_Player = Find_RMusicPlayer(self, PlayerClass);

	if( RMusic_Player != none )
	{
		if(RMusic_bUnload)
		{
			if(RMusic_UnloadPlugin == "") RMusic_Player.RMusic_UnLoadPlugin("ALL");
			else RMusic_Player.RMusic_UnLoadPlugin(RMusic_UnloadPlugin);

		}
		if(RMusic_DSPPlugin != "")
		{
			bHasDSP = RMusic_Player.RMusic_LoadPlugin(RMusic_DSPPlugin);
			if(bHasDSP && RMusic_bUpdateParameters)
			{
				for(i=0; i<32; i++)
				{
					RMusic_Player.RMusic_SetDSPParam(RMusic_DSPPlugin, i, RMusic_DSPParameters[i]);
				}
			}
		}
	}
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Used only if RMusic_PlayAtStartup is false
 */
simulated function Trigger( actor Other, pawn EventInstigator )
{
	EVENT_AddDSP();
}

defaultproperties
{
	RMusic_bUnload=true
	PlayerClass=class'RMusicPlayer.RMusic_Player'
	Texture=texture'RMusicPlayer.Icons.RMusic_DSPController'

	bNoDelete=true
	RemoteRole=ROLE_SimulatedProxy
}