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

#if !defined(_CIW_GAME_PLATFORM_INPUT_MARM_H_)
#define _CIW_GAME_PLATFORM_INPUT_MARM_H_

#include "IwGamePlatformMarm.h"
#include "IwGameUtil.h"

class CIwGamePlatformInputMarm : public IIwGamePlatformInput
{
	// Properties
protected:
public:
	// Properties end
public:
	// Init
	int		Init();
	void	Release();
	// Feature query
	bool	isPointerAvailable();
	bool	isMultiTouch();
	bool	isKeysAvailable();
	bool	isOSKeyboardAvailable();
	bool	isAccelerometerAvailable();
	bool	isCompassAvailable();
	// Buttons query
	bool	wasBackButtonReleased();
	bool	wasMenuButtonReleased();
	// Key query
	bool	isKeyDown(int key);
	bool	isKeyUp(int key);
	bool	wasKeyPressed(int key);
	bool	wasKeyReleased(int key);
	// Acclerometer
	bool	startAccelerometer();
	void	stopAccelerometer();
	void	getAccelerometer(CIwVec3& data);
	// Compass
	bool	startCompass();
	void	stopCompass();
	float	getCompassDirection();
	bool	getCompassHeading(CIwVec3& data);
	// OS Keyboard
	const char*	OsKeyboardRead(const char* prompt, const char* default_text, int flags);
	// Update
	bool	Update();
};


#endif	// _CIW_GAME_PLATFORM_INPUT_MARM_H_
