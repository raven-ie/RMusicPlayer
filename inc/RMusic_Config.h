#ifndef _INC_RCONFIG
#define _INC_RCONFIG

class URMusicPlayerConfig : public UObject
{
	DECLARE_CLASS(URMusicPlayerConfig,UObject,CLASS_Config,RMusicPlayer)

	// Configuration.
	FString DriverName;
	FString Output;

	/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	* static constructor (builds configuration menu)
	*/

	void StaticConstructor();
	/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	* returns selected driver
	*/
	int GetDriverNum();
};

#endif