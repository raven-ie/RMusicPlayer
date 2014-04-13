/*===========================================================================
    C++ class definitions exported from UnrealScript.
    This is automatically generated by the tools.
    DO NOT modify this manually! Edit the corresponding .uc files instead!
===========================================================================*/
#if _MSC_VER
#pragma pack (push,4)
#endif

#ifndef RMUSICPLAYER_API
#define RMUSICPLAYER_API DLL_IMPORT
#endif

#ifndef NAMES_ONLY
#define AUTOGENERATE_NAME(name) extern RMUSICPLAYER_API FName RMUSICPLAYER_##name;
#define AUTOGENERATE_FUNCTION(cls,idx,name)
#endif


#ifndef NAMES_ONLY


class RMUSICPLAYER_API ARMusic_Component : public AActor
{
public:
    DECLARE_CLASS(ARMusic_Component,AActor,0,RMusicPlayer)
    NO_DEFAULT_CONSTRUCTOR(ARMusic_Component)
};


class RMUSICPLAYER_API ARMusic_Player : public ARMusic_Component
{
public:
    INT RMusic_Volume;
    BITFIELD bAlwaysLoadCodecs:1 GCC_PACK(4);
    FStringNoInit RMusic_PluginsDirectory GCC_PACK(4);
    FStringNoInit RMusic_Directory;
    BITFIELD bIncludeDebugInfo:1 GCC_PACK(4);
    FStringNoInit DriverName GCC_PACK(4);
    BITFIELD bAuthoritative:1 GCC_PACK(4);
    BITFIELD bUseCurrentPaths:1;
    FLOAT FaderUpdateTime GCC_PACK(4);
    FStringNoInit NextTrack;
    BITFIELD NextLoop:1 GCC_PACK(4);
    BITFIELD bIsOn:1;
    class APlayerPawn* RMusic_LocalPlayer GCC_PACK(4);
    class ALevelInfo* RMusic_CurLevel;
    class ALevelInfo* RMusic_OldLevel;
    BITFIELD bHasDSP:1 GCC_PACK(4);
    FLOAT fDSPUpdateTime GCC_PACK(4);
    FLOAT fDSPUpdateDelay;
    class ARMusic_ControllerEx* nextEvent;
    DECLARE_FUNCTION(execRMusic_GetCurrentTime);
    DECLARE_FUNCTION(execRMusic_GetTotalTime);
    DECLARE_FUNCTION(execRMusic_Close);
    DECLARE_FUNCTION(execRMusic_Stop);
    DECLARE_FUNCTION(execRMusic_GetVolume);
    DECLARE_FUNCTION(execRMusic_DecVolume);
    DECLARE_FUNCTION(execRMusic_IncVolume);
    DECLARE_FUNCTION(execRMusic_SetVolume);
    DECLARE_FUNCTION(execRMusic_SetCfgVolume);
    DECLARE_FUNCTION(execRMusic_IsPlaying);
    DECLARE_FUNCTION(execRMusic_Pause);
    DECLARE_FUNCTION(execRMusic_Play);
    DECLARE_FUNCTION(execRMusic_Update);
    DECLARE_FUNCTION(execRMusic_SetDSPParam);
    DECLARE_FUNCTION(execRMusic_UnLoadPlugin);
    DECLARE_FUNCTION(execRMusic_LoadPlugin);
    DECLARE_FUNCTION(execRMusic_Startup);
    DECLARE_CLASS(ARMusic_Player,ARMusic_Component,0|CLASS_Config,RMusicPlayer)
    #include "ARMusic_Player.h"
};

#endif

AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_GetCurrentTime);
AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_GetTotalTime);
AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_Close);
AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_Stop);
AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_GetVolume);
AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_DecVolume);
AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_IncVolume);
AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_SetVolume);
AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_SetCfgVolume);
AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_IsPlaying);
AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_Pause);
AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_Play);
AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_Update);
AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_SetDSPParam);
AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_UnLoadPlugin);
AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_LoadPlugin);
AUTOGENERATE_FUNCTION(ARMusic_Player,-1,execRMusic_Startup);

#ifndef NAMES_ONLY
#undef AUTOGENERATE_NAME
#undef AUTOGENERATE_FUNCTION
#endif NAMES_ONLY

#if _MSC_VER
#pragma pack (pop)
#endif