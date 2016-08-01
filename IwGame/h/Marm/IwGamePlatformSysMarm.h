// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. You must ensure that this whole notice is present in all files and derivatives, so the reader knows its origin.
// If you use this engine in your product then please ensure that you credit IwGame's usage appropriately. Please see http://www.drmop.com/index.php/iwgame-engine/ for licensing details and support
//
//

#if !defined(_CIW_GAME_PLATFORM_SYS_MARM_H_)
#define _CIW_GAME_PLATFORM_SYS_MARM_H_

#include "IwGamePlatform.h"

class CIwGamePlatformSysMarm : public IIwGamePlatformSys
{
	// Properties
protected:
public:
	// Properties end
public:
	int		Init();
	void	Release();

	// Backlight
	void	BacklightOn();

	// Time
	uint64	getTimeInMs();

	// OS calls
	void	YieldToOS(int time_ms);
	bool	ExecuteViaOS(const char* name, bool exit_app);
	void	RequestQuit();
};


#endif	// _CIW_GAME_PLATFORM_SYS_MARM_H_
