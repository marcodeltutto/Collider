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

#include "Marm/IwGamePlatformMarm.h"
#include "IwUtil.h"

void	IwGamePlatformMarm_Create()
{
    IwUtilInit();

	CIwGamePlatform::Create();
	PLATFORM->Init(new CIwGamePlatformMarm(), true, false);
}

void	IwGamePlatformMarm_Destroy()
{
	PLATFORM->Release();
	CIwGamePlatform::Destroy();

	IwUtilTerminate();

}

int CIwGamePlatformMarm::Init()
{
	PLATFORM->setSys(new CIwGamePlatformSysMarm());
	PLATFORM->setDisplay(new CIwGamePlatformDisplayMarm());
	PLATFORM->setInput(new CIwGamePlatformInputMarm());
	PLATFORM->setAudio(new CIwGamePlatformAudioMarm());
	PLATFORM->setComms(new CIwGamePlatformCommsMarm());
	PLATFORM->setFile(new CIwGamePlatformFileMarm());
	PLATFORM->setFont(new CIwGamePlatformFontMarm());
	PLATFORM->setMedia(new CIwGamePlatformMediaMarm());
	PLATFORM->setOther(new CIwGamePlatformOtherMarm());
	PLATFORM->setUI(new CIwGamePlatformUIMarm());

	return 1;
}

void CIwGamePlatformMarm::Release()
{
}

