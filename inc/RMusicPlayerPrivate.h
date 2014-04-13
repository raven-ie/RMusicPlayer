#ifndef _INC_RPRIV
#define _INC_RPRIV
	#include <cstdlib>
	#include <string>
	#include <iostream>
	#include <vector>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <math.h>
	#include <conio.h>
	#include <Windows.h>

	#include "Engine.h"

	/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	* If you forget this You will have error like this:
	* '(...) inconsistent dll linkage.  dllexport assumed.(...)'
	*/
	#define RMUSICPLAYER_API DLL_EXPORT

	#ifndef RMUSICPLAYER_API
	#define RMUSICPLAYER_API DLL_IMPORT
	#endif

	#define RESULT_CAST(type, var) *reinterpret_cast< type * >(var)
	#define SCRIPT_STACK FFrame &Stack, void* Result

	#include "FOutputDeviceFile.h"
	#define debugf GLog->Logf

	#include "RMusicPlayerClasses.h"

	#include "fmod.hpp"
	#include "fmod_errors.h"

	#include "RMusic_Utils.h"
	#include "RMusic_Config.h"
#endif