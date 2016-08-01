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

#if !defined(_CIW_GAME_PLATFORM_MARM_H_)
#define _CIW_GAME_PLATFORM_MARM_H_

#include "IwGamePlatform.h"
#include "IwGamePlatformSysMarm.h"
#include "IwGamePlatformDisplayMarm.h"
#include "IwGamePlatformInputMarm.h"
#include "IwGamePlatformAudioMarm.h"
#include "IwGamePlatformCommsMarm.h"
#include "IwGamePlatformFileMarm.h"
#include "IwGamePlatformFontMarm.h"
#include "IwGamePlatformMediaMarm.h"
#include "IwGamePlatformOtherMarm.h"
#include "IwGamePlatformUIMarm.h"

class CIwGamePlatformMarm : public IIwGamePlatform
{
	// Properties
protected:
public:
	// Properties end

protected:
public:
	int		Init();
	void	Release();

};

void	IwGamePlatformMarm_Create();
void	IwGamePlatformMarm_Destroy();


#endif	// _CIW_GAME_PLATFORM_MARM_H_
