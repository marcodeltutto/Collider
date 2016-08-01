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

#include "IwGamePlatform.h"

CDECLARE_SINGLETON(CIwGamePlatform)

int CIwGamePlatform::Init(IIwGamePlatform* platform, bool comms, bool use_gl)
{
	Platform = platform;
	Platform->Init();
	if (Sys != NULL)
		Sys->Init();
	if (Display != NULL)
		Display->Init(use_gl);
	if (File != NULL)
		File->Init();
	if (Input != NULL)
		Input->Init();
	if (Audio != NULL)
		Audio->Init();
	if (comms && Comms != NULL)
		Comms->Init();
	if (Font != NULL)
		Font->Init();
	if (Media != NULL)
		Media->Init();
	if (Other != NULL)
		Other->Init();
	if (UI != NULL)
		UI->Init();
	Initialised = true;

	return 1;
}

void CIwGamePlatform::Release()
{
	if (UI != NULL)
	{
		UI->Release();
		delete UI;
		UI = NULL;
	}
	if (Other != NULL)
	{
		Other->Release();
		delete Other;
		Other = NULL;
	}
	if (Media != NULL)
	{
		Media->Release();
		delete Media;
		Media = NULL;
	}
	if (Font != NULL)
	{
		Font->Release();
		delete Font;
		Font = NULL;
	}
	if (Comms != NULL)
	{
		Comms->Release();
		delete Comms;
		Comms = NULL;
	}
	if (Audio != NULL)
	{
		Audio->Release();
		delete Audio;
		Audio = NULL;
	}
	if (Input != NULL)
	{
		Input->Release();
		delete Input;
		Input = NULL;
	}
	if (File != NULL)
	{
		File->Release();
		delete File;
		File = NULL;
	}
	if (Display != NULL)
	{
		Display->Release();
		delete Display;
		Display = NULL;
	}
	if (Sys != NULL)
	{
		Sys->Release();
		delete Sys;
		Sys = NULL;
	}
	Platform->Release();
	SAFE_DELETE(Platform);
	Initialised = false;
}

