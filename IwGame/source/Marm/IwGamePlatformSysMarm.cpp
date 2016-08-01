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

#include "Marm/IwGamePlatformSysMarm.h"
#include "s3eOSExec.h"

//
// Marmalade System
//
int		CIwGamePlatformSysMarm::Init()
{
	return 1;
}

void	CIwGamePlatformSysMarm::Release()
{
}

void	CIwGamePlatformSysMarm::BacklightOn()
{
	s3eDeviceBacklightOn();
}

uint64	CIwGamePlatformSysMarm::getTimeInMs()
{
	return s3eTimerGetMs();
}

void	CIwGamePlatformSysMarm::YieldToOS(int time_ms)
{
	s3eDeviceYield(time_ms);
}

bool	CIwGamePlatformSysMarm::ExecuteViaOS(const char* name, bool exit_app)
{
	return s3eOSExecExecute(name, exit_app) == S3E_RESULT_SUCCESS;
}

void	CIwGamePlatformSysMarm::RequestQuit()
{
	s3eDeviceRequestQuit();
}




