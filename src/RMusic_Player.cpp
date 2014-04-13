/**
 * Copyright 2008 Raven
 */
#include "RMusicPlayerPrivate.h"

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Bunch of global variabls used by RMusic_Player
 */
FMOD::System     *RMusicPlayer_system;
FMOD::Sound      *RMusicPlayer_sound;
FMOD::Channel    *RMusicPlayer_channel = 0;
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * DSP plugin support
 */
struct DSP_Plugins_S
{
	FString DSPPlugin;
	int DSPIndex;
	FMOD::DSP *DSPHandle;
	float DSPParams[32];
};
DSP_Plugins_S DSP_Plugins[16];

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * static constructor (builds configuration menu)
 */
void ARMusic_Player::StaticConstructor()
{
	guard(ARMusic_Player::StaticConstructor);
	//Sound driver configuration
	FMOD::System     *local_system;
	FMOD_RESULT result;
	result = FMOD::System_Create(&local_system);

	int i, numdrivers;	
	UEnum* SoundDrivers = new( GetClass(), TEXT("SoundDriver") )UEnum( NULL );
    
	result = local_system->getNumDrivers(&numdrivers);

	for (i=0; i < numdrivers; i++)
	{
		char StringA[256];
		result = local_system->getDriverInfo(i, StringA, 256, 0);
		new(SoundDrivers->Names) FName(ANSI_TO_TCHAR(StringA));
	}       

	new(GetClass(), TEXT("SoundDriver"), RF_Public)UByteProperty (CPP_PROPERTY( DriverName ), TEXT("MusicPlayer"), CPF_Config, SoundDrivers );
	
	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * returns selected driver
 */
int ARMusic_Player::GetDriverNum()
{
	guard(ARMusic_Player::GetDriverNum);
	//Sound driver configuration
	FMOD::System     *local_system;
	FMOD_RESULT result;
	result = FMOD::System_Create(&local_system);

	int i, numdrivers;
	int result_driver = (int)-1;
	FString CrrentDriverF;

	result = local_system->getNumDrivers(&numdrivers);

	for (i=0; i < numdrivers; i++)
	{
		char StringA[256];
		result = local_system->getDriverInfo(i, StringA, 256, 0);
		CrrentDriverF = ANSI_TO_TCHAR(StringA);
		CrrentDriverF = RemoveSpaces( *CrrentDriverF );		
		
		if(CrrentDriverF == DriverName) 
		{
			result_driver=i;
			break;
		}
	}

	return result_driver;
	unguard;
}

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Used to initialize all FMOD functions
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @return	True on success
 */
void ARMusic_Player::execRMusic_Startup(SCRIPT_STACK)
{
	guard(ARMusic_Player::execRMusic_Startup);
	P_FINISH;
	FMOD_RESULT result;
	unsigned int version;
	// creates object
	result = FMOD::System_Create(&RMusicPlayer_system);
	if(result != FMOD_OK)
	{
		if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: FMOD error :: RMusic_Startup :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
		RESULT_CAST(UBOOL, Result) = false;
	}
	else if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Startup :: System created"));
	// tries to get fmodex version
	if(RMusicPlayer_system != NULL)
	{
		result = RMusicPlayer_system->getVersion(&version);
		if(result != FMOD_OK)
		{
			if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: FMOD error :: RMusic_Startup :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			RESULT_CAST(UBOOL, Result) = false;
		}
		// version check
		if (version < FMOD_VERSION && bIncludeDebugInfo)
		{
			debugf(NAME_Init, TEXT("RMusic_Player :: FMOD error :: RMusic_Startup :: wrong fmodex.dll version. RMusic_Player requires version %i"), FMOD_VERSION);
		}
		RMusicPlayer_system->setDriver(Cast<URMusicPlayerConfig>(URMusicPlayerConfig::StaticClass()->GetDefaultObject() )->GetDriverNum());
		FString SelOut=Cast<URMusicPlayerConfig>(URMusicPlayerConfig::StaticClass()->GetDefaultObject() )->Output;

		if( SelOut == FString(TEXT("DirectSound")) )
			RMusicPlayer_system->setOutput(FMOD_OUTPUTTYPE_DSOUND);
		else if( SelOut == FString(TEXT("WindowsMultimediaWaveOut")) )
			RMusicPlayer_system->setOutput(FMOD_OUTPUTTYPE_WINMM);
		else if( SelOut == FString(TEXT("ASIO")) )
			RMusicPlayer_system->setOutput(FMOD_OUTPUTTYPE_ASIO);

		//loads additional codecs
		if(bAlwaysLoadCodecs)
		{
			//sets plugin directory
			result = RMusicPlayer_system->setPluginPath(TCHAR_TO_ANSI( *AddSlashes(RMusic_PluginsDirectory) ));
			if(result != FMOD_OK)
			{
				if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: FMOD error  :: RMusic_Startup ::(%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );    	
			}
			else
			{
				HANDLE hFile; //handle for file
				WIN32_FIND_DATA FileInformation; // file informations
				TCHAR* strPattern;  // search pattern

				strPattern = ANSI_TO_TCHAR ( TCHAR_TO_ANSI( *ConnectAddSlashes(RMusic_PluginsDirectory, FString(TEXT("codec_*.dll"))) ) ); // we search for FMODEX codecs

				//simple search function
				hFile = ::FindFirstFile(strPattern, &FileInformation);
				if(hFile != INVALID_HANDLE_VALUE)
				{
					do
					{
						if(appStricmp(FileInformation.cFileName,TEXT(".")) && appStricmp(FileInformation.cFileName,TEXT("..")))
						{
							if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: Plugin Found :: %s"), FileInformation.cFileName );
							result = RMusicPlayer_system->loadPlugin(TCHAR_TO_ANSI(FileInformation.cFileName), 0, 0);
							if (result != FMOD_OK)
							{
								if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: FMOD error  :: RMusic_Startup ::(%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
								RESULT_CAST(UBOOL, Result) = false;
							}    
							else
							{
								if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Play :: Plugin %s loaded"), FileInformation.cFileName);
								RESULT_CAST(UBOOL, Result) = true;
							}
						}
					}
					while(::FindNextFile(hFile, &FileInformation) == TRUE);
					::FindClose(hFile);
				}
			}
		}

		// fmodex initialize
		result = RMusicPlayer_system->init(1, FMOD_INIT_NORMAL, 0);
		if(result != FMOD_OK)
		{
			if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: FMOD error  :: RMusic_Startup ::(%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			RESULT_CAST(UBOOL, Result) = false;
		}
		else if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Startup :: System initialized"));
		// everything is initialized, so we can return true
		RESULT_CAST(UBOOL, Result) = true;
	}
	else
		RESULT_CAST(UBOOL, Result) = false;
	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Loads plugin (codec/dsp)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input @required     Plugin       Plugin name
 */
void ARMusic_Player::execRMusic_LoadPlugin(SCRIPT_STACK)
{
	guard(Plugin::execRMusic_LoadPlugin);
	P_GET_STR( Plugin );
	P_FINISH;

	RESULT_CAST(UBOOL, Result) = false;
	
	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Unloads plugin (codec/dsp)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input @required     Plugin       Plugin name
 */
void ARMusic_Player::execRMusic_UnLoadPlugin(SCRIPT_STACK)
{
	guard(Plugin::execRMusic_UnLoadPlugin);
	P_GET_STR( Plugin );
	P_FINISH;

	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Sets DSP parameter
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input @required     index       parameter index
 * @param @input @required     value       parameter value
 */
void ARMusic_Player::execRMusic_SetDSPParam(SCRIPT_STACK)
{
	guard(ARMusic_Player::RMusic_SetDSPParam);
	P_GET_STR( Plugin );
	P_GET_INT( index );
	P_GET_FLOAT( value );	
	P_FINISH; 
	DSP_Plugins_S FoundPlugin;	
	FMOD_RESULT result;

	if(RMusicPlayer_system != NULL)
	{
		int i;		
		for(i = 0; i < 16; i++)
		{
			if(DSP_Plugins[i].DSPPlugin == Plugin)
			{
				FoundPlugin = DSP_Plugins[i];
				break;
			}
		}		
		if (FoundPlugin.DSPHandle != NULL)
		{
			debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_SetDSPParam :: DSP FOund"));
			int RDSPNumParams;
			result = FoundPlugin.DSPHandle->getNumParameters(&RDSPNumParams);		
			if (result != FMOD_OK && bIncludeDebugInfo)
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_SetDSPParam :: FMOD error  :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			}
			if( FoundPlugin.DSPPlugin != FString(TEXT("")) && index < RDSPNumParams )
			{
				float min;
				float max;
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_SetDSPParam :: Creating DSP"));

				result = FoundPlugin.DSPHandle->getParameterInfo(index, 0, 0, 0, 0, &min, &max);

				if (result != FMOD_OK && bIncludeDebugInfo)
				{
					debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_SetDSPParam :: FMOD error  ::  (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
				}

				if(value > max) value=max;
				if(value < min) value=min;
				result = FoundPlugin.DSPHandle->setParameter(index, value);

				if (result != FMOD_OK && bIncludeDebugInfo)
				{
					debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_SetDSPParam :: FMOD error  :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
				}
			}
		}
		if (bIncludeDebugInfo)
		{
			debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_SetDSPParam :: FMOD error  :: DSPHandle not found"));
		}
	}
	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Updates FMOD
 */
void ARMusic_Player::execRMusic_Update(SCRIPT_STACK)
{
	guard(ARMusic_Player::RMusic_Update);
	P_FINISH; 
	if(RMusicPlayer_system != NULL)
	{    
		RMusicPlayer_system->update();
	}
	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Loads plugin (codec/dsp)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input @required     bPause          Pause or not :)
 */
void ARMusic_Player::execRMusic_Pause(SCRIPT_STACK)
{
	guard(Plugin::execRMusic_Pause);
	P_GET_UBOOL( bPause );
	P_FINISH;
	FMOD_RESULT result;
   
	if(RMusicPlayer_channel != NULL)
	{
		if(bPause)
			result = RMusicPlayer_channel->setPaused(true);
		else
			result = RMusicPlayer_channel->setPaused(false);

		if(bIncludeDebugInfo)
		{
			if (result != FMOD_OK)
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Pause :: FMOD error  :: RMusic_Pause :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			}
			else
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Pause :: Music paused %g"), bPause );
			}
		}
	}
	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Plays song
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input  @required     File          Source file
 * @param @input  @required     Loop          Should song be looped
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @return	True on success
 */
void ARMusic_Player::execRMusic_Play(SCRIPT_STACK)
{
	guard(ARMusic_Player::execRMusic_Play);
	P_GET_STR( File );
	P_GET_UBOOL( Loop );
	P_FINISH;
	FMOD_RESULT result;
	
	if(RMusicPlayer_system != NULL)
	{
		//we have to check out if music isn't already playing
		if(RMusicPlayer_channel != NULL && RMusicPlayer_sound != NULL)
		{
			bool RMusic_isPlaying=false;	//false by default

			result = RMusicPlayer_channel->isPlaying(&RMusic_isPlaying);

			if (result != FMOD_OK && bIncludeDebugInfo)
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Play :: FMOD error  :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			}
			//if music is playing
			if(RMusic_isPlaying)
			{
				// releases sound
				result = RMusicPlayer_sound->release();
				if(bIncludeDebugInfo)
				{
					if (result != FMOD_OK)
					{
						debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Play :: FMOD error  :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
					}
					else
					{
						debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Play :: Sound released"));
					}
				}
			}
		}
		//creating stream
		if( Loop )
			result = RMusicPlayer_system->createStream(TCHAR_TO_ANSI(*ConnectAddSlashes(RMusic_Directory,File)), FMOD_HARDWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &RMusicPlayer_sound);
		else
			result = RMusicPlayer_system->createStream(TCHAR_TO_ANSI(*ConnectAddSlashes(RMusic_Directory,File)), FMOD_HARDWARE | FMOD_LOOP_OFF | FMOD_2D, 0, &RMusicPlayer_sound);
		
		if( result != FMOD_OK && bUseCurrentPaths )
		{
			if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Play :: file not found in path '%s'. Scanning GSys paths for music."), RMusic_Directory);
			FString R,Fn;
			UBOOL bFound = 0;

			// Gets target file.
			for(int i=0; i<GSys->Paths.Num(); i++ )
			{
				if( GSys->Paths(i).InStr(TEXT("*.umx")) != -1)
				{
					R = GSys->Paths(i);
					R = FStrReplace( R, FString(TEXT("*.umx")), FString(TEXT("")) );
					R = AddSlashes(R);
					R = R+File;
					if( GFileManager->FindFiles(ANSI_TO_TCHAR(TCHAR_TO_ANSI(*R)), 1, 0 ).Num() )
					{
						bFound = 1;
						Fn = R;
						break;
					}
				}
			}
			if( bFound )
			{
				if( Loop )
					result = RMusicPlayer_system->createStream(TCHAR_TO_ANSI(*Fn), FMOD_HARDWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &RMusicPlayer_sound);
				else
					result = RMusicPlayer_system->createStream(TCHAR_TO_ANSI(*Fn), FMOD_HARDWARE | FMOD_LOOP_OFF | FMOD_2D, 0, &RMusicPlayer_sound);
				RESULT_CAST(UBOOL, Result) = true;
			}
			else
			{
				if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Play :: Scanning GSys paths completed. File not found '%s'."), File);
				RESULT_CAST(UBOOL, Result) = false;
			}
		}
		//playing stream only on success
		if(result != FMOD_OK)
		{
			debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Play :: FMOD error :: %s"), File);
			if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Play :: FMOD error :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			RESULT_CAST(UBOOL, Result) = false;
		}
		else
		{
			if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Play :: Stream created from file %s"), ConnectAddSlashes(RMusic_Directory,File));

			result = RMusicPlayer_system->playSound(FMOD_CHANNEL_FREE, RMusicPlayer_sound, false, &RMusicPlayer_channel);
			if(result != FMOD_OK)
			{
				if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: FMOD error  :: RMusic_Play :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
				RESULT_CAST(UBOOL, Result) = false;
			}
			else
			{
				if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Play :: Playing sound"));
				float Volume;

				//Volume = RMusic_Volume/100.0f;
				Volume = (float)RMusic_Volume/(float)100;

				result = RMusicPlayer_channel->setVolume(Volume);
				if(bIncludeDebugInfo)
				{
					if (result != FMOD_OK)
					{
						debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Play :: FMOD error :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
					}
					else
					{
						debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Play :: Volume is %g"), Volume );
					}
				}
			}
		}
	}
	else
	{
		RESULT_CAST(UBOOL, Result) = false;
	}

	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Check if song is playing.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @return	true or false
 */
void ARMusic_Player::execRMusic_IsPlaying(SCRIPT_STACK)
{
	guard(ARMusic_Player::execRMusic_IsPlaying);
	P_FINISH; 
	FMOD_RESULT result;
	bool RMusic_isPlaying=false;

	if(RMusicPlayer_channel != NULL)
	{
		result = RMusicPlayer_channel->isPlaying(&RMusic_isPlaying);

		if (result != FMOD_OK && bIncludeDebugInfo)
		{
			debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_IsPlaying :: FMOD error :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
		}
	
		RESULT_CAST(UBOOL, Result) = RMusic_isPlaying;
	}
	else
	{
		RESULT_CAST(UBOOL, Result) = false;
	}

	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Changes volume (based on RMusic_Volume value)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @return	True on success
 */
void ARMusic_Player::execRMusic_SetCfgVolume(SCRIPT_STACK)
{
	guard(ARMusic_Player::execRMusic_SetCfgVolume);
	P_FINISH;
	FMOD_RESULT result;
    
	if(RMusicPlayer_channel != NULL)
	{
		float Volume;
	
		Volume = (float)RMusic_Volume/(float)100;

		result = RMusicPlayer_channel->setVolume(Volume);
	
		if(bIncludeDebugInfo)
		{
			if (result != FMOD_OK)
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_SetCfgVolume :: FMOD error  :: RMusic_SetCfgVolume :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			}
			else
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_SetCfgVolume :: Volume is %g"), Volume );
			}
		}
	}
	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Changes volume
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @param @input  @required     NewVolume       new volume
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @return	True on success
 */
void ARMusic_Player::execRMusic_SetVolume(SCRIPT_STACK)
{
	guard(ARMusic_Player::execRMusic_SetVolume);
	P_GET_INT( NewVolume );
	P_FINISH;
	FMOD_RESULT result;

	if(RMusicPlayer_channel != NULL)
	{
		if(bIncludeDebugInfo)
		{
			float Volume2;
			RMusicPlayer_channel->getVolume(&Volume2);

			debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_SetVolume :: inserted %d"), NewVolume );
			debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_SetVolume :: Old %g"), Volume2 );
		}
		float Volume;
	
		Volume = (float)NewVolume/(float)100;

		result = RMusicPlayer_channel->setVolume(Volume);
	
		if(bIncludeDebugInfo)
		{
			if (result != FMOD_OK)
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_SetVolume :: FMOD error :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			}
			else
			{
				float TrueVolume;
				RMusicPlayer_channel->getVolume(&TrueVolume);
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_SetVolume :: Volume should be %g and is %f :: (%d) %s"), Volume, TrueVolume, result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			}
		}
	}
	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Checks current volume
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @return	Current volume
 */
void ARMusic_Player::execRMusic_GetVolume(SCRIPT_STACK)
{
	guard(ARMusic_Player::execRMusic_GetVolume);
	P_FINISH;
	FMOD_RESULT result;

	if(RMusicPlayer_channel != NULL)
	{
		float Volume;
		int TrueVolume;

		result = RMusicPlayer_channel->getVolume(&Volume);

		if (result != FMOD_OK && bIncludeDebugInfo)
		{
			debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_GetVolume :: FMOD error  :: RMusic_GetVolume :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
		}

		Volume = Volume * 100;

		TrueVolume = (int) Volume;
	
		RESULT_CAST(int, Result) = TrueVolume;
	}
	else
	{
		RESULT_CAST(int, Result) = (int)0;
	}

	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Increments volume
 */
void ARMusic_Player::execRMusic_IncVolume(SCRIPT_STACK)
{
	guard(ARMusic_Player::execRMusic_IncVolume);
	P_FINISH;
	FMOD_RESULT result;

	if(RMusicPlayer_channel != NULL)
	{
		float CurrentVolume;
		RMusicPlayer_channel->getVolume(&CurrentVolume);
		CurrentVolume+=(float)0.01;

		result = RMusicPlayer_channel->setVolume(CurrentVolume);
	
		if(bIncludeDebugInfo)
		{
			if (result != FMOD_OK)
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_IncVolume :: FMOD error :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			}
			else
			{
				float TrueVolume;
				RMusicPlayer_channel->getVolume(&TrueVolume);
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_IncVolume :: Volume should be %g and is %f :: (%d) %s"), CurrentVolume, TrueVolume, result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			}
		}
	}
	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Decrements volume
 */
void ARMusic_Player::execRMusic_DecVolume(SCRIPT_STACK)
{
	guard(ARMusic_Player::execRMusic_DecVolume);
	P_FINISH;
	FMOD_RESULT result;

	if(RMusicPlayer_channel != NULL)
	{
		float CurrentVolume;
		RMusicPlayer_channel->getVolume(&CurrentVolume);
		CurrentVolume-=(float)0.01;

		result = RMusicPlayer_channel->setVolume(CurrentVolume);
	
		if(bIncludeDebugInfo)
		{
			if (result != FMOD_OK)
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_DecVolume :: FMOD error :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			}
			else
			{
				float TrueVolume;
				RMusicPlayer_channel->getVolume(&TrueVolume);
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_DecVolume :: Volume should be %g and is %f :: (%d) %s"), CurrentVolume, TrueVolume, result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			}
		}
	}
	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Stops currently played song
 */
void ARMusic_Player::execRMusic_Stop(SCRIPT_STACK)
{
	guard(ARMusic_Player::execRMusic_Stop);
	P_FINISH;
	FMOD_RESULT result;

	if(RMusicPlayer_sound != NULL)
	{
		// releases sound
		result = RMusicPlayer_sound->release();
		if(bIncludeDebugInfo)
		{
			if (result != FMOD_OK)
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Stop :: FMOD error  :: RMusic_Stop :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			}
			else
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Stop :: Sound released"));
			}
		}
	}

	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Shuts down FMOD functions
 */
void ARMusic_Player::execRMusic_Close(SCRIPT_STACK)
{
	guard(ARMusic_Player::execRMusic_Close);
	P_FINISH;
	FMOD_RESULT result;

	// releases sound
	if(RMusicPlayer_sound != NULL)
	{
		result = RMusicPlayer_sound->release();
		if(bIncludeDebugInfo)
		{
			if (result != FMOD_OK)
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Stop :: FMOD error  :: RMusic_Stop :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			}
			else
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Stop :: Sound released"));
			}
		}
	}
	// closes fmodex
	if(RMusicPlayer_sound != NULL)
	{
		result = RMusicPlayer_system->close();
		if(bIncludeDebugInfo)
		{
			if (result != FMOD_OK)
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Close :: FMOD error  :: RMusic_Close :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			}
			else
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Close :: FMODEX closed"));
			}
		}
	}
	// releases fmodex
	if(RMusicPlayer_sound != NULL)
	{
		result = RMusicPlayer_system->release();
		if(bIncludeDebugInfo)
		{
			if (result != FMOD_OK)
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Close :: FMOD error  :: RMusic_Close :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
			}
			else
			{
				debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_Close :: FMODEX released"));
			}
		}
	}
	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Returns total song time or -1
 */
void ARMusic_Player::execRMusic_GetTotalTime(SCRIPT_STACK)
{
	guard(ARMusic_Player::execRMusic_GetTotalTime);
	P_GET_STR( File );
	P_GET_UBOOL( Loop );
	P_FINISH;
	FMOD_RESULT result;
	unsigned int length = -1;

	if(RMusicPlayer_system != NULL)
	{
		//we have to check out if music isn't already playing
		if(RMusicPlayer_channel != NULL && RMusicPlayer_sound != NULL)
		{
			result = RMusicPlayer_sound->getLength(&length, FMOD_TIMEUNIT_MS);
			if (result != FMOD_OK)
			{
				if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_GetTotalTime :: FMOD error  :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
				length = (int) -1;
			}
			else
			{
				if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_GetTotalTime :: current time :: %i"), length);
			}					
		}
	}
	*(INT*)Result  = length;	
	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Returns current song position or -1
 */
void ARMusic_Player::execRMusic_GetCurrentTime(SCRIPT_STACK)
{
	guard(ARMusic_Player::execRMusic_GetCurrentTime);
	P_GET_STR( File );
	P_GET_UBOOL( Loop );
	P_FINISH;
	FMOD_RESULT result;
	unsigned int length = -1;

	if(RMusicPlayer_system != NULL)
	{
		//we have to check out if music isn't already playing
		if(RMusicPlayer_channel != NULL && RMusicPlayer_sound != NULL)
		{
			result = RMusicPlayer_channel->getPosition(&length, FMOD_TIMEUNIT_MS);
			if (result != FMOD_OK)
			{
				if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_GetCurrentTime :: FMOD error  :: (%d) %s"), result, ANSI_TO_TCHAR(FMOD_ErrorString(result)) );
				length = (int) -1;
				return;
			}
			else
			{
				if(bIncludeDebugInfo) debugf(NAME_Init, TEXT("RMusic_Player :: RMusic_GetCurrentTime :: current time :: %i"), length);
			}
		}
	}
	*(INT*)Result  = length;	
	unguard;
}

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * class implementation
 */
IMPLEMENT_CLASS(ARMusic_Component);
IMPLEMENT_CLASS(ARMusic_Player);