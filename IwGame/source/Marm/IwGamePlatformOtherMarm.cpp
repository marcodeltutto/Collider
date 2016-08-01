// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. The only restriction to its usage is that this header must remain intact and visible in all IwGame engine Others.
// If you use this engine in your product then please ensure that you credit IwGame's usage appropriately. Please see http://www.drmop.com/index.php/iwgame-engine/ for licensing details and support
//
//

#include "Marm/IwGamePlatformOtherMarm.h"
#include "IwResManager.h"


//
// Marmalade Other systems
//
int		CIwGamePlatformOtherMarm::Init()
{
	return 1;
}

void	CIwGamePlatformOtherMarm::Release()
{
}

void	CIwGamePlatformOtherMarm::VibrateStart(float priority, float duration)
{
	s3eVibraVibrate((int)(priority * 255.0f), (int)(duration * 1000.0f));
}

void	CIwGamePlatformOtherMarm::VibrateStop()
{
	s3eVibraStop();
}

void	CIwGamePlatformOtherMarm::InitResManager()
{
	IwResManagerInit();
}

void	CIwGamePlatformOtherMarm::ReleaseResManager()
{
	IwResManagerTerminate();
}








