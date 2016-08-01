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

#if !defined(_CIW_GAME_PLATFORM_DISPLAY_MARM_H_)
#define _CIW_GAME_PLATFORM_DISPLAY_MARM_H_

#include "IwGamePlatformMarm.h"

class CIwGamePlatformDisplayMarm : public IIwGamePlatformDisplay
{
	// Properties
protected:
	bool			UseGL;
public:
	// Properties end
public:
	// Init
	int				Init(bool use_gl);
	void			Release();
	// Parameter query
	int				getCurrentWidth();
	int				getCurrentHeight();
	// Display management
	void			setColour(int r, int g, int b, int a);
	void			Clear();
	void			Swap();

};


#endif	// _CIW_GAME_PLATFORM_DISPLAY_MARM_H_
