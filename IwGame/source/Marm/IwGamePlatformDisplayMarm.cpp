// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. The only restriction to its usage is that this header must remain intact and visible in all IwGame engine Displays.
// If you use this engine in your product then please ensure that you credit IwGame's usage appropriately. Please see http://www.drmop.com/index.php/iwgame-engine/ for licensing details and support
//
//

#include "Marm/IwGamePlatformDisplayMarm.h"
#include "IwGx.h"
#include "IwGL.h"
#include "Iw2D.h"


//
// Marmalade Display system
//
int		CIwGamePlatformDisplayMarm::Init(bool use_gl)
{
	IwGxInit();
    Iw2DInit();
	Iw2DSetUseMipMapping(false);

	if (use_gl)
		IwGLInit();
	UseGL = use_gl;

	return 1;
}

void	CIwGamePlatformDisplayMarm::Release()
{
	if (UseGL)
		IwGLTerminate();
	Iw2DTerminate();
	IwGxTerminate();
}

int		CIwGamePlatformDisplayMarm::getCurrentWidth()
{
	return IwGxGetScreenWidth();
}

int		CIwGamePlatformDisplayMarm::getCurrentHeight()
{
	return IwGxGetScreenHeight();
}

void	CIwGamePlatformDisplayMarm::setColour(int r, int g, int b, int a)
{
	IwGxSetColClear(r, g, b, a);
}

void	CIwGamePlatformDisplayMarm::Clear()
{
	// Clear the screen
	if (UseGL)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else
		IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);
}

void	CIwGamePlatformDisplayMarm::Swap()
{
	// Flush IwGx
	IwGxFlush();

	// Display the rendered frame
	if (UseGL)
		IwGLSwapBuffers();
	else
		IwGxSwapBuffers();
}






