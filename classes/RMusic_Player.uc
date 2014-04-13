//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copyright 2005-2008 Dead Cow Studios. All Rights Reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Coder: Raven
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Plays music. Supported audio files:
//   - .flac
//   - .mp2
//   - .mp3
//   - .ogg
//   - .wma
//   - .wav
//   - .ape (only with additional plugin)
class RMusic_Player extends RMusic_Component
				native
				NoUserCreate
				config(RMusicPlayer);
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Config Variables
 */
var globalconfig int RMusic_Volume;		// current volume (global)
var globalconfig bool bAlwaysLoadCodecs;	// loads codecs at startup (global)
var() config string RMusic_PluginsDirectory;	// plugins directory (can be overrided in subclass)
var() config string RMusic_Directory;		// music directory (can be overrided in subclass)
//var() config string RMusic_DefaultPlugins[16];	// default plugins (dsp) which should be load at startup (can be overrided in subclass)
var globalconfig bool bIncludeDebugInfo;	// includes debug info inside log
var globalconfig string DriverName;
var() bool bAuthoritative;			// player controls levelchange. set to false if gametype/console should take controll over RMusic_Player
var() bool bUseCurrentPaths;
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Song fading info
 */
var float FaderUpdateTime;			// fader update time (i.e. how fast fade out music)
var string NextTrack;				// next track (used by UScript music fade simulation)
var bool NextLoop;				// should next track be looped (used by UScript music fade simulation)
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Level switching info
 */
var bool bIsOn;
var PlayerPawn RMusic_LocalPlayer;
var LevelInfo RMusic_CurLevel, RMusic_OldLevel;

var bool bHasDSP;
var float fDSPUpdateTime;
var float fDSPUpdateDelay;
var RMusic_ControllerEx nextEvent;

/**==========================================================
 * NATIVE FUNCTIONS
 **==========================================================*/

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Used to initialize all FMOD functions
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @return	True on success
 */
native simulated final function bool RMusic_Startup();
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Loads plugin (codec/dsp)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input @required     Plugin       Plugin name
 */
native simulated final function bool RMusic_LoadPlugin(string Plugin);

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Unloads plugin (dsp)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input @required     Plugin       Plugin name
 */
native simulated final function RMusic_UnLoadPlugin(string Plugin);
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Sets DSP parameter
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input @required     Plugin       Plugin name
 * @param @input @required     index       parameter index
 * @param @input @required     value       parameter value
 */
native simulated final function RMusic_SetDSPParam(string Plugin, int index, float value);
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Updates FMOD
 */
native simulated final function RMusic_Update();
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Plays song
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input  @required     File          Source file
 * @param @input  @required     Loop          Should song be looped
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @return	True on success
 */
native simulated final function bool RMusic_Play(string File, bool Loop);
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Pauses song
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input  @required     bPause          Pause or not :)
 */
native simulated final function RMusic_Pause(bool bPause);
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Check if song is playing.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @return	true or false
 */
native simulated final function bool RMusic_IsPlaying();
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Changes volume (based on RMusic_Volume value)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @return	True on success
 */
native simulated final function RMusic_SetCfgVolume();
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Changes volume
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input  @required     NewVolume       new volume
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @return	True on success
 */
native simulated final function RMusic_SetVolume(int NewVolume);
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Increments volume
 */
native simulated final function RMusic_IncVolume();
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Decrements volume
 */
native simulated final function RMusic_DecVolume();
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Checks current volume
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @return	Current volume
 */
native simulated final function int RMusic_GetVolume();
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Stops currently played song
 */
native simulated final function RMusic_Stop();
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Shuts down FMOD functions
 */
native simulated final function RMusic_Close();
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Returns total song time or -1
 */
native simulated final function int RMusic_GetTotalTime();
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Returns current song position or -1
 */
native simulated final function int RMusic_GetCurrentTime();
/**==========================================================
 * NON NATIVE FUNCTIONS
 **==========================================================*/

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Sets up RMusicPlayer
 */
simulated event BeginPlay()
{
	RMusicEvent_Startup();
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Startup event
 */
simulated final function RMusicEvent_Startup()
{
	if(RMusic_Startup())
	{
		bIsOn=true;
//		LoadDefaultPlugins();
		if(bAuthoritative) Enable('Tick');
		else Disable('Tick');
	}
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Loads default plugins
 */
/*
simulated function LoadDefaultPlugins()
{
	local int i;

	for(i=0; i<16; i++)
	{
		if(RMusic_DefaultPlugins[i] != "")
		{
			if(RMusic_LoadPlugin(RMusic_DefaultPlugins[i]))
				if(bIncludeDebugInfo) log("RMusic_Player :: LoadDefaultPlugins :: plugin '"$RMusic_DefaultPlugins[i]$"' loaded.", 'RMusicPlayer');
			else
				if(bIncludeDebugInfo) log("RMusic_Player :: LoadDefaultPlugins :: plugin '"$RMusic_DefaultPlugins[i]$"' can not be loaded.", 'RMusicPlayer');
		}
	}
}
*/
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Mutes player
 */
simulated final function RMusic_Mute()
{
	if( RMusic_GetVolume() > 0 ) RMusic_SetVolume(0);
	else RMusic_SetCfgVolume();
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Fades out music (and stops it)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input  @required     bStop       Should music be stopped
 */
simulated final function RMusic_ProgramFadeOut(optional bool bStop)
{
	local int CurrentVolume;
	CurrentVolume = RMusic_GetVolume();
	if(CurrentVolume > 0)
		RMusic_SetVolume(CurrentVolume-1);
	else
	{
		if(bStop) RMusic_Stop();
		RMusic_SetCfgVolume();
	}
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Fades music in
 */
simulated final function RMusic_ProgramFadeIn()
{
	local int CurrentVolume, Next;
	CurrentVolume = RMusic_GetVolume();
	Next = CurrentVolume;
	Next++;
	if(RMusic_LocalPlayer != none) RMusic_LocalPlayer.BroadCastMessage("Next: "@Next);
	if(CurrentVolume < RMusic_Volume)
		RMusic_SetVolume(Next);
	if(RMusic_LocalPlayer != none) RMusic_LocalPlayer.BroadCastMessage("Is: "@RMusic_GetVolume());
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Plays music stram
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input  @required     File          Source file
 * @param @input  @required     Loop          Should song be looped
 */
simulated final function RMusic_PlayStream(string File, bool Loop)
{
	NextTrack = File;
	NextLoop = Loop;
	if(RMusic_IsPlaying())
	{
		GoToState('RMusic_CrossFade');
	}
	else
	{
		GoToState('RMusic_FadeIn');
	}
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Resets RMusic_Player
 */
simulated final function RMusic_Reset()
{
	bIsOn=false;
	RMusic_Close();
	RMusicEvent_Startup();
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Set next event
 */
simulated function setEvent(RMusic_ControllerEx nEv)
{
	if(nEv == none)
	{
		nextEvent = none;
		if(!bAuthoritative) Disable('Tick');
		return;
	}

	nextEvent = nEv;
	Enable('Tick');
}

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * We need to know if level has changed to stop music
 */
simulated function Tick(float Delta)
{
	if ( RMusic_LocalPlayer == none || !bIsOn ) return;
	
	if(nextEvent != none)
	{
		if( RMusic_GetCurrentTime() >= RMusic_GetTotalTime() )
		{    
			nextEvent.EVENT_Player();
		}
	}

	if(bAuthoritative)
	{
		RMusic_CurLevel = RMusic_LocalPlayer.Player.Console.Viewport.Actor.Level;
	
		if ( RMusic_CurLevel != RMusic_OldLevel )
		{
			if( RMusic_IsPlaying() ) RMusic_Stop();
			RMusic_LocalPlayer = none;
	//		RMusic_Close();
	//		Destroy();
		}
	}
}
/**==========================================================
 * STATES
 **==========================================================*/

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Crossfades two music streams
 */
simulated state RMusic_CrossFade
{
Begin:
	if(NextTrack == "") GoTo('End');
	if(RMusic_IsPlaying()) GoTo('FadeOut');
	else GoTo('FadeIn');
FadeOut:
        if(RMusic_GetVolume() > 0)
	{
		RMusic_DecVolume();
		sleep(FaderUpdateTime);
		GoTo('FadeOut');
	}
	RMusic_Stop();
FadeIn:
	RMusic_Play(NextTrack, NextLoop);
	RMusic_SetVolume(0);
FadeInLoop:
        if(RMusic_GetVolume() < RMusic_Volume)
	{
		RMusic_IncVolume();
		sleep(FaderUpdateTime);
		GoTo('FadeInLoop');
	}
End:
	if(NextTrack != "") NextTrack="";
	else log("ERROR: Track not specified", 'RMusicPlayer');
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Fades in music
 */
simulated state RMusic_FadeIn
{
Begin:
	RMusic_Play(NextTrack, NextLoop);
	RMusic_SetVolume(0);
FadeIn:
        if(RMusic_GetVolume() < RMusic_Volume)
	{
		RMusic_IncVolume();
		sleep(FaderUpdateTime);
		GoTo('FadeIn');
	}
End:
	RMusic_SetCfgVolume();
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Fades out music
 */
simulated state RMusic_FadeOut
{
Begin:
	if(!RMusic_IsPlaying()) GoTo('End');
FadeOut:
        if(RMusic_GetVolume() > 0)
	{
		RMusic_DecVolume();
		sleep(FaderUpdateTime);
		GoTo('FadeOut');
	}
End:
	RMusic_Stop();
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Fades in music and unpauses
 */
simulated state RMusic_FadeInUnPause
{
Begin:
	RMusic_Pause(false);
	RMusic_SetVolume(0);
FadeIn:
        if(RMusic_GetVolume() < RMusic_Volume)
	{
		RMusic_IncVolume();
		sleep(FaderUpdateTime);
		GoTo('FadeIn');
	}
End:
	RMusic_SetCfgVolume();
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Fades out music and pauses
 */
simulated state RMusic_FadeOutPause
{
Begin:
	if(!RMusic_IsPlaying()) GoTo('End');
FadeOut:
        if(RMusic_GetVolume() > 0)
	{
		RMusic_DecVolume();
		sleep(FaderUpdateTime);
		GoTo('FadeOut');
	}
End:
	RMusic_Pause(true);
	RMusic_SetCfgVolume();
}

defaultproperties
{
	FaderUpdateTime=0.001
	RMusic_Directory="../Music/"
	bAlwaysLoadCodecs=true
	RMusic_PluginsDirectory="RMusic_Plugins/"
	bIncludeDebugInfo=false
	bAuthoritative=true

	Texture=texture'RMusicPlayer.Icons.RMusic_Player'
	RMusic_Volume=50
	bStatic=false
	bStasis=false
	bNoDelete=false

	RemoteRole=ROLE_SimulatedProxy
	fDSPUpdateDelay=4
	bUseCurrentPaths=true
}