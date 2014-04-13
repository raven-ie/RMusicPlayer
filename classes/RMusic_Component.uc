//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copyright 2005-2008 Dead Cow Studios. All Rights Reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Coder: Raven
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Keeps all actors together and stores basic static functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// changelog:
//  v 0.1.1
//    - fixed Trigger function in RMusic_Controller
//    - in RMusic_Player function RMusic_Play stops music (if something is playing), before playing new one
//  v 0.1.0
//    - added new RMusic_Controller actions (AC_Pause and AC_UnPause)
//    - DSP manager moved form RMusic_Controller into RMusic_DSPController
//    - new samples in SDK (integration with gametype and playerpawn)
//  v 0.0.3
//    - fixed fade in/out/corssfade bug
//    - fixed crashes on dedicated server
//    - added DSP related functions into RMusic_Player
//  v 0.0.2 (first public release)
//    - fixed few RMusic_Player bugs
class RMusic_Component extends Actor
				native
				abstract;
				
#exec TEXTURE IMPORT NAME=RMusic_Controller FILE=ico\RMusic_Controller.pcx GROUP="Icons" MIPS=OFF FLAGS=2
#exec TEXTURE IMPORT NAME=RMusic_DSPController FILE=ico\RMusic_DSPController.pcx GROUP="Icons" MIPS=OFF FLAGS=2
#exec TEXTURE IMPORT NAME=RMusic_Player FILE=ico\RMusic_Player.pcx GROUP="Icons" MIPS=OFF FLAGS=2

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Used to find localplayer
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input  @required     Sender     Actor calling this function
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @return	local PlayerPawn
 */
static final simulated function PlayerPawn Find_LocalPlayer(actor Sender)
{
        local PlayerPawn PlayerP;

	if(Sender.Level.NetMode != NM_DedicatedServer)
	{
		foreach Sender.AllActors(class'PlayerPawn',PlayerP)
		{
			if(Viewport(PlayerP.Player) != None) return PlayerP; //we found local player in visibility radius
		}
	}
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Tries to find local RMusic_Player. If RMusic_Player can't
 * be found, function tries to spawn it in Entry level.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input  @required     Sender     Actor calling this function
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @return	RMusic_Player or none
 */
static final simulated function RMusic_Player Find_RMusicPlayer(actor Sender, optional class<RMusic_Player> PlayerClass, optional bool bDoNotSpawn)
{
	local RMusic_Player RMP;
	local PlayerPawn PP;
	
	PP = Find_LocalPlayer(Sender);
	if(PP == none)
	{
		Sender.Log("Can not retrive local PlayerPawn",'RMusicPlayer');
		return none;
	}
	
	if(PP.GetEntryLevel() == none)
	{
		Sender.Log("No Entry level",'RMusicPlayer');
		return none;
	}

	foreach PP.GetEntryLevel().AllActors(class'RMusic_Player', RMP) break;
	
	if(PlayerClass == none) PlayerClass=class'RMusicPlayer.RMusic_Player';

	if (RMP == none && !bDoNotSpawn)
		RMP = PP.GetEntryLevel().Spawn(PlayerClass);
	return RMP;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Tries to find local RMusic_Player. If RMusic_Player can't
 * be found, function tries to spawn it in Entry level.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input  @required     Sender     Actor calling this function
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @return	RMusic_Player or none
 */
//static final simulated function RMusic_Player Find_RMusicPlayerByPPawn(actor Sender, PlayerPawn PP, optional class<RMusic_Player> PlayerClass, optional bool bDoNotSpawn)
static final simulated function RMusic_Player Find_RMusicPlayerByPPawn(PlayerPawn PP, optional class<RMusic_Player> PlayerClass, optional bool bDoNotSpawn)
{
	local RMusic_Player RMP;

	if(PP == none)
	{
		return none;
	}

	if(PP.GetEntryLevel() == none)
	{
		PP.Log("No Entry level",'RMusicPlayer');
		return none;
	}

	foreach PP.GetEntryLevel().AllActors(class'RMusic_Player', RMP) break;

	if(PlayerClass == none) PlayerClass=class'RMusicPlayer.RMusic_Player';

	if (RMP == none && !bDoNotSpawn)
		RMP = PP.GetEntryLevel().Spawn(PlayerClass);
	return RMP;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Tries to detect OS
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input  @required     Sender     Actor calling this function
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @return	OS name
 */
static final simulated function name Find_OS(actor Sender)
{
	local string OSString;

	OSString = Sender.ConsoleCommand("get ini:Engine.Engine.ViewportManager Class");


	if(OSString == "Class'WinDrv.WindowsClient'")
		return 'Windows';
	else if(OSString == "Class'SDLDrv.SDLClient'")
		return 'Linux';
	else if(OSString == "Class'MacDrv.MacClient'")
		return 'MacOS';
	else
		return 'Unknown';
}

defaultproperties
{
	bHidden=true
}