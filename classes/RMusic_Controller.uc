/**
 * Copyright 2008 Raven
 */
// Controls RMusic_Player.
class RMusic_Controller extends RMusic_Component;

#exec OBJ LOAD FILE="umx\NoMusic.umx" PACKAGE="RMusicPlayer"

var() class<RMusic_Player> PlayerClass;	// Player class (if you want to have own music directory)
var() string RMusic_File;		// Music file to play
var() bool RMusic_PlayAtStartup;	// Should it be played at level start
var() bool RMusic_MuteUMX;		// If true will mute all music in umx files
var() bool RMusic_BroadcastToAll;	// If true, it'll broadcast functions to all players
var() bool RMusic_bUnloadPreviousDSP;	// If true, all DSP plugins will be unloaded
var() bool RMusic_bOwnFadeUpdateTime;	// Should use custom fade time
var() float RMusic_OwnFadeUpdateTime;	// How often fading state will be updated
var() bool RMusic_bUseSaveControl;	// If true, Controller will spawn RMusic_Save actor to restore music in level (usefull only in SinglePlayer)
var string RMusic_DSPPlugins[16];	// DSP plugins to load (not used, left because of compatibility reasons)
var() enum EAction
{
    AC_Play,				// Plays music
    AC_Stop,				// Stops music
    AC_Pause,				// Pauses music
    AC_UnPause,				// Unpasues music
    AC_ShutDown				// Shutdown FMODEX (avoid this one ;) )
} Action;				// Action
var() enum EPlayType
{
    PT_Loop,				// Loops music
    PT_PlayOnce				// Plays once
} RMusic_PlayType;			// Play type
var() enum ERMusicTransition
{
    TRANS_Instanly,			// Instant transition
    TRANS_Fade				// Smooth fade
} RMusic_Transition;			// Transition type

replication
{
	reliable if( Role == ROLE_Authority )
		EVENT_Player;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Sets up all stuff
 */
simulated function BeginPlay()
{
	if(Level.NetMode == NM_Standalone) EVENT_CreateSave();
	if(RMusic_PlayAtStartup)
	{
		//GoToState('RMusic_StartupPlayMusic');
		SetTimer(0.2,true);
	}
} 
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Handle startup music play
 */
simulated function Timer()
{
	EVENT_Player();
	SetTimer(0, false);
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Spawn save controller
 */
function EVENT_CreateSave()
{
	local RMusic_Save RMusic_Save;

        foreach AllActors(class'RMusic_Save',RMusic_Save) break;
        if (RMusic_Save == none)
		RMusic_Save = Spawn(class'RMusic_Save');
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Spawn save controller
 */
function EVENT_Save()
{
	local RMusic_Save RMusic_Save;

        foreach AllActors(class'RMusic_Save',RMusic_Save) break;
        if (RMusic_Save == none)
		RMusic_Save = Spawn(class'RMusic_Save');
	RMusic_Save.SavedController=self;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Used to play music
 */
simulated function EVENT_Player(optional PlayerPawn PP)
{
	local RMusic_Player RMusic_Player;
	local int i;
	local bool bLooped;
	local PlayerPawn RMusic_LocalPlayer;

	if(Level.NetMode == NM_DedicatedServer) return;

	if( RMusic_PlayType == PT_Loop ) bLooped = true;
	else bLooped = false;
	if(PP == none) RMusic_Player = Find_RMusicPlayer(self, PlayerClass);
	else RMusic_Player = Find_RMusicPlayerByPPawn(PP, PlayerClass);

	if(Level.NetMode == NM_Standalone && RMusic_bUseSaveControl) EVENT_Save();

	if( RMusic_Player != none )
	{
		if(PP == none) RMusic_LocalPlayer = Find_LocalPlayer(self);
		else RMusic_LocalPlayer = PP;
		
		RMusic_Player.setEvent(none);

		if( RMusic_MuteUMX ) RMusic_LocalPlayer.ClientSetMusic(Music'RMusicPlayer.null', 0, 0, MTRAN_FastFade);
		if( RMusic_bUnloadPreviousDSP )
		{
			RMusic_Player.RMusic_UnLoadPlugin("ALL");
			RMusic_Player.bHasDSP=false;
		}
		if(RMusic_bOwnFadeUpdateTime && RMusic_OwnFadeUpdateTime > 0) RMusic_Player.FaderUpdateTime=RMusic_OwnFadeUpdateTime;
		switch Action
		{
			case AC_Play:
				if(RMusic_Transition == TRANS_Instanly)
					RMusic_Player.RMusic_Play(RMusic_File,bLooped);
				else
					RMusic_Player.RMusic_PlayStream(RMusic_File,bLooped);
			break;
			case AC_Stop:
				if(RMusic_Transition == TRANS_Instanly)
					RMusic_Player.RMusic_Stop();
				else
					RMusic_Player.GoToState('RMusic_FadeOut');
			break;
			case AC_Pause:
				if(RMusic_Transition == TRANS_Instanly)
					RMusic_Player.RMusic_Pause(true);
				else
					RMusic_Player.GoToState('RMusic_FadeOutPause');
			break;
			case AC_UnPause:
				if(RMusic_Transition == TRANS_Instanly)
					RMusic_Player.RMusic_Pause(false);
				else
					RMusic_Player.GoToState('RMusic_FadeInUnPause');
			break;
			case AC_ShutDown:
				RMusic_Player.RMusic_Close();
			break;
		}
		if( RMusic_Player.bAuthoritative )
		{
			RMusic_Player.RMusic_LocalPlayer = RMusic_LocalPlayer;
			RMusic_Player.RMusic_OldLevel = RMusic_LocalPlayer.Level;
		}
	}
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Used only if RMusic_PlayAtStartup is false
 */
function Trigger( actor Other, pawn EventInstigator )
{
	local Pawn A;
	local PlayerPawn P;

	if( !RMusic_PlayAtStartup )
	{
		if( Level.NetMode == NM_Standalone )
		{
			EVENT_Player();
		}
		else if( Level.NetMode == NM_DedicatedServer || Level.NetMode == NM_ListenServer )
		{
			if( RMusic_BroadcastToAll )
			{
				A = Level.PawnList;
				While ( A != None )
				{
					if ( A.IsA('PlayerPawn') )
					{
						SetOwner(A);
						EVENT_Player(PlayerPawn(A));
					}
					A = A.nextPawn;
				}
			}
			else
			{
				// Only affect the one player.
				P = PlayerPawn(EventInstigator);
				if( P == None )
					return;				
				SetOwner(P);
				EVENT_Player(P);
			}
			if(Owner != none) SetOwner(none);
		}
	}
}

defaultproperties
{
	PlayerClass=class'RMusicPlayer.RMusic_Player'
	RMusic_MuteUMX=true
	RMusic_bUnloadPreviousDSP=false
	RMusic_OwnFadeUpdateTime=0.001

	Texture=texture'RMusicPlayer.Icons.RMusic_Controller'

	bNoDelete=true
	RemoteRole=ROLE_SimulatedProxy
}