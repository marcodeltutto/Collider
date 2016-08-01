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

#include "IwGameInput.h"
#include "IwGameString.h"

CDECLARE_SINGLETON(CIwGameInput)


//
//
// CIwGameInput implementation
//
//
CIwGameTouch*	CIwGameInput::findTouch(int id)
{
	if (!PointerAvailable)
		return NULL;

	if (id < 0 || id >= IW_GAME_MAX_TOUCHES)
		id = 0;

	for (int t = 0; t < IW_GAME_MAX_TOUCHES; t++)
	{
		// find a match in all slots
		if (Touches[t].id == id)
		{
			return &Touches[t];
		}
	}
	for (int t = 0; t < IW_GAME_MAX_TOUCHES; t++)
	{
		// find first free slot to put touch into
		if (!Touches[t].active)
		{
			Touches[t].id = id;
			return &Touches[t];
		}
	}

	return NULL;
}

CIwGameTouch*	CIwGameInput::getTouchByID(int id)
{
	if (!PointerAvailable)
		return NULL;

	// Find touch by its ID and return it
	for (int t = 0; t < IW_GAME_MAX_TOUCHES; t++)
	{
//		if (Touches[t].active && Touches[t].id == id)
		if (Touches[t].id == id)
			return &Touches[t];
	}

	return NULL;
}

int CIwGameInput::getTouchCount() const
{
	if (!PointerAvailable)
		return 0;

	// Return the total number of active touches
	int count = 0;
	for (int t = 0; t < IW_GAME_MAX_TOUCHES; t++)
	{
		if (Touches[t].active)
            count++;
	}

	return count;
}

bool CIwGameInput::Init()
{
	IIwGamePlatformInput* input = PLATFORM->getInput();
	FirstTouch = NULL;
	Touched = false;
	Tapped = false;
	BackPressed = false;
	MenuPressed = false;
	CompassActive = false;
	AccelerometerActive = false;

	// Check to see if the device that we are running on supports the pointer
	PointerAvailable = input->isPointerAvailable();

	if (PointerAvailable)
	{
		// Clear out the touches array
		for (int t = 0; t < IW_GAME_MAX_TOUCHES; t++)
		{
			Touches[t].active = false;
			Touches[t].id = 0;
		}

		// Determine if the device supports multi-touch
		IsMultiTouch = input->isMultiTouch();
	}

	input->Init();

	// Check to see if the device that we are running on supports the keyboard
	KeysAvailable = input->isKeysAvailable();

	// Check to see if the device that we are running on supports the on screen keyboard
	OSKeyboardAvailable = input->isOSKeyboardAvailable(); 

	// Check accelerometer availability
	if (input->isAccelerometerAvailable())
		AccelerometerAvailable = true;
	else
		AccelerometerAvailable = false;

	// Check compass availability
	if (input->isCompassAvailable())
		CompassAvailable = true;
	else
		CompassAvailable = false;

	return true; // Pointer support
}

void CIwGameInput::Release()
{
	PLATFORM->getInput()->Release();
}

void CIwGameInput::Update()
{
	IIwGamePlatformInput* input = PLATFORM->getInput();

	input->Update();

	Tapped = false;

	DragDelta.x = 0;
	DragDelta.y = 0;
	for (int t = 0; t < IW_GAME_MAX_TOUCHES; t++)
	{
		Touches[t].dx = 0;
		Touches[t].dy = 0;
		if (Touches[t].touched)
		{
			Touches[t].dx = Touches[t].x - Touches[t].px;
			Touches[t].dy = Touches[t].y - Touches[t].py;
			Touches[t].px = Touches[t].x;
			Touches[t].py = Touches[t].y;

			if (!Touches[t].active)
				Touches[t].touched = false;
		}
		else
		{
			if (Touches[t].active)
			{
				Touches[t].touched = true;
				Touches[t].px = Touches[t].x;
				Touches[t].py = Touches[t].y;
				Tapped = true;
			}
		}
	}

	// Update buttons
	BackPressed = false;
	bool back_released = input->wasBackButtonReleased();
	if (back_released)
		BackPressed = true;

	MenuPressed = false;
	bool menu_released = input->wasMenuButtonReleased();
	if (menu_released)
		MenuPressed = true;

	// Update accelerometer
	if (AccelerometerActive)
		input->getAccelerometer(AccelerometerPosition);

	// Update compass
	if (CompassActive)
	{
		CompassDirection = input->getCompassDirection();
		input->getCompassHeading(CompassHeading);
	}

}

bool CIwGameInput::isDragging(int index) const
{
	if (Touches[index].touched)
	{
		if (Touches[index].dx != 0 || Touches[index].dy != 0)
			return true;
	}

	return false;
}

bool CIwGameInput::isKeyDown(int key) const
{
	return PLATFORM->getInput()->isKeyDown(key);
}

bool CIwGameInput::isKeyUp(int key) const
{
	return PLATFORM->getInput()->isKeyUp(key);
}

bool CIwGameInput::wasKeyPressed(int key) const
{
	return PLATFORM->getInput()->wasKeyPressed(key);
}

bool CIwGameInput::wasKeyReleased(int key) const
{
	return PLATFORM->getInput()->wasKeyReleased(key);
}

const char* CIwGameInput::showOnScreenKeyboard(const char* prompt, int flags, const char* default_text)
{
	return PLATFORM->getInput()->OsKeyboardRead(prompt, default_text, flags);
}

bool CIwGameInput::startAccelerometer()
{
	AccelerometerActive = false;
	if (AccelerometerAvailable && PLATFORM->getInput()->startAccelerometer())
	{
		AccelerometerActive = true;
		return true;
	}

	return false;
}
void CIwGameInput::stopAccelerometer()
{
	if (AccelerometerActive)
	{
		PLATFORM->getInput()->stopAccelerometer();
		AccelerometerActive = false;
	}
}

bool CIwGameInput::startCompass()
{
	CompassActive = false;
	if (CompassAvailable && PLATFORM->getInput()->startCompass())
	{
		CompassActive = true;
		return true;
	}

	return false;
}
void CIwGameInput::stopCompass()
{
	if (CompassActive)
	{
		PLATFORM->getInput()->stopCompass();
		CompassActive = false;
	}
}



