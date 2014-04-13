/**
 * Copyright 2008 Raven
 */
#include "RMusicPlayerPrivate.h"

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* IMPORTANT!!
* UCC generated header implementation
* This file was created to glue UnrealScript and C++ code together
*/

#define NAMES_ONLY
#define AUTOGENERATE_NAME(name) RMUSICPLAYER_API FName RMUSICPLAYER_##name;
#define AUTOGENERATE_FUNCTION(cls,idx,name) IMPLEMENT_FUNCTION(cls,idx,name)
#include "RMusicPlayerClasses.h"
#undef AUTOGENERATE_FUNCTION
#undef AUTOGENERATE_NAME
#undef NAMES_ONLY

void RegisterNames()
{
	static INT Registered = 0;
	if (!Registered++)
	{
		#define NAMES_ONLY
		#define AUTOGENERATE_NAME(name) extern RMUSICPLAYER_API FName RMUSICPLAYER_##name; RMUSICPLAYER_##name=FName(TEXT(#name),FNAME_Intrinsic);
		#define AUTOGENERATE_FUNCTION(cls,idx,name)
		#include "RMusicPlayerClasses.h"
		#undef DECLARE_NAME
		#undef NAMES_ONLY
	}
}

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Implementing package
*/
IMPLEMENT_PACKAGE(RMusicPlayer);