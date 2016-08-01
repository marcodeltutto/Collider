// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. The only restriction to its usage is that this header must remain intact and visible in all IwGame engine files.
// If you use this engine in your product, whilst it is not mandatory, a mention of the IwGame engine would be nice or a quick email to let us know where it is being used.
//
//

#include "IwGameInput.h"
#include "IwGameString.h"
#include "s3eCompass.h"

CDECLARE_SINGLETON(CIwGameInput)

// 
//
// Input callback handlers
//
//
//
// HandleMultiTouchButtonCB - For multitouch devices the system will call this callback when the user touches the screen. This callback is called once for each screen touch
// 
void HandleMultiTouchButtonCB(s3ePointerTouchEvent* event)
{
	if (IW_GAME_INPUT == NULL)
		return;

	// Check to see if the touch already exists
//	CIwGameTouch* touch = IW_GAME_INPUT->findTouch(event->m_TouchID);
	CIwGameTouch* touch = IW_GAME_INPUT->getTouch(event->m_TouchID);
	if (touch != NULL)
	{
		// Yes it does, so update the touch information
		touch->prev_active = touch->active;
		touch->active = event->m_Pressed != 0; 
		touch->x = event->m_x;
		touch->y = event->m_y;
		touch->px = event->m_x;
		touch->py = event->m_y;
		touch->id = event->m_TouchID;
	}
}
//
// HandleMultiTouchMotionCB - For multitouch devices the system will call this callback when the user moves their finger on the screen. This callback is called once for each screen touch
// 
void HandleMultiTouchMotionCB(s3ePointerTouchMotionEvent* event)
{
	if (IW_GAME_INPUT == NULL)
		return;

	// Check to see if the touch already exists
//	CIwGameTouch* touch = IW_GAME_INPUT->findTouch(event->m_TouchID);
	CIwGameTouch* touch = IW_GAME_INPUT->getTouch(event->m_TouchID);
	if (touch != NULL)
	{
		// Updates the touches positional information
		touch->x = event->m_x;
		touch->y = event->m_y;
	}
}
//
// HandleSingleTouchButtonCB - The system will call this callback when the user touches the screen
// 
void HandleSingleTouchButtonCB(s3ePointerEvent* event)
{
	if (IW_GAME_INPUT == NULL)
		return;

	CIwGameTouch* touch = IW_GAME_INPUT->getTouch(0);
	touch->prev_active = touch->active;
	touch->active = event->m_Pressed != 0;
	touch->x = event->m_x;
	touch->y = event->m_y;
	touch->px = event->m_x;
	touch->py = event->m_y;
}
//
// HandleSingleTouchMotionCB - The system will call this callback when the user moves their finger on the screen
// 
void HandleSingleTouchMotionCB(s3ePointerMotionEvent* event)
{
	if (IW_GAME_INPUT == NULL)
		return;

	CIwGameTouch* touch = IW_GAME_INPUT->getTouch(0);
	touch->x = event->m_x;
	touch->y = event->m_y;
}

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
		// find a match in all slots (rjt)
		if (Touches[t].id == id)
		{
			return &Touches[t];
		}
	}
	for (int t = 0; t < IW_GAME_MAX_TOUCHES; t++)
	{
		// find first free slot to put touch into (rjt)
		if (!Touches[t].active)
		{
			Touches[t].id = id;
			return &Touches[t];
		}
	}

//	return &Touches[id];
/*    // Attempt to find the touch by its ID and then return it
	// If the touch does not exist then it is recorded in the touches list
	for (int t = 0; t < MAX_TOUCHES; t++)
	{
		if (Touches[t].id == id)
			return &Touches[t];
		if (!Touches[t].active)
		{
            Touches[t].id = id;
			return &Touches[t];
		}
	}*/

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
	FirstTouch = NULL;
	Touched = false;
	Tapped = false;
	BackPressed = false;
	MenuPressed = false;
	CompassActive = false;
	AccelerometerActive = false;


	// Check to see if the device that we are running on supports the pointer
    PointerAvailable = s3ePointerGetInt(S3E_POINTER_AVAILABLE) ? true : false;

	if (PointerAvailable)
	{
		// Clear out the touches array
		for (int t = 0; t < IW_GAME_MAX_TOUCHES; t++)
		{
			Touches[t].active = false;
			Touches[t].id = 0;
		}

		// Determine if the device supports multi-touch
		IsMultiTouch = s3ePointerGetInt(S3E_POINTER_MULTI_TOUCH_AVAILABLE) ? true : false;

		// For multi-touch devices we handle touch and motion events using different callbacks
		if (IsMultiTouch)
		{
			s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, (s3eCallback)HandleMultiTouchButtonCB, NULL);
			s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)HandleMultiTouchMotionCB, NULL);
		}
		else
		{
			s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)HandleSingleTouchButtonCB, NULL);
			s3ePointerRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)HandleSingleTouchMotionCB, NULL);
		}
	}

	// Check to see if the device that we are running on supports the keyboard
    KeysAvailable = ((s3eKeyboardGetInt(S3E_KEYBOARD_HAS_KEYPAD) || s3eKeyboardGetInt(S3E_KEYBOARD_HAS_ALPHA))) ? true : false;

	// Check to see if the device that we are running on supports the on screen keyboard
	OSKeyboardAvailable = s3eOSReadStringAvailable() == S3E_TRUE; 

	// Check accelerometer availability
	if (s3eAccelerometerGetInt(S3E_ACCELEROMETER_AVAILABLE))
		AccelerometerAvailable = true;
	else
		AccelerometerAvailable = false;

	// Check compass availability
	if (s3eCompassAvailable())
		CompassAvailable = true;
	else
		CompassAvailable = false;

	return true; // Pointer support
}

void CIwGameInput::Release()
{
	if (PointerAvailable)
	{
		// Unregister the pointer system callbacks
		if (IsMultiTouch)
		{
			s3ePointerUnRegister(S3E_POINTER_TOUCH_EVENT, (s3eCallback)HandleMultiTouchButtonCB);
			s3ePointerUnRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)HandleMultiTouchMotionCB);
		}
		else
		{
			s3ePointerUnRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)HandleSingleTouchButtonCB);
			s3ePointerUnRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)HandleSingleTouchMotionCB);
		}
	}
}

void CIwGameInput::Update()
{
	// Update the pointer if it is available
	if (PointerAvailable)
		s3ePointerUpdate();

	// Update key system if it is available
	s3eKeyboardUpdate();

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
			}
		}
	}

/*	if (Touched)
	{
		// Update touch position
		DragDelta.x = FirstTouch->x - TouchedPos.x;
		DragDelta.y = FirstTouch->y - TouchedPos.y;
		TouchedPos.x = FirstTouch->x;
		TouchedPos.y = FirstTouch->y;

		// Check to see if touch stopped
		if (!FirstTouch->active)
		{
			// Touch stopped
			Touched = false;
			FirstTouch = NULL;

			// Check for tap
			if (!TapTimer.HasTimedOut())
				Tapped = true;
			TapTimer.Stop();
		}
	}
	else
	{
		// No touch current then check for one
		if (FirstTouch == NULL)
		{
			for (int t = 0; t < MAX_TOUCHES; t++)
			{
				if (Touches[t].active)
				{
					// We found a touch
					FirstTouch = &Touches[t];
					Touched = true;
					TouchedPos.x = FirstTouch->x;
					TouchedPos.y = FirstTouch->y;

					// Start tap timer
					TapTimer.setDuration(TAP_TIME_MS);

					break;
				}
			}
		}
	}*/

	// Update buttons
	BackPressed = false;
	bool back_released = (s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_RELEASED) == S3E_KEY_STATE_RELEASED || (s3eKeyboardGetState(s3eKeyAbsBSK) & S3E_KEY_STATE_RELEASED) == S3E_KEY_STATE_RELEASED;
	if (back_released)
		BackPressed = true;

	MenuPressed = false;
	bool menu_released = (s3eKeyboardGetState(s3eKeyMenu) & S3E_KEY_STATE_RELEASED) == S3E_KEY_STATE_RELEASED || (s3eKeyboardGetState(s3eKeyAbsASK) & S3E_KEY_STATE_RELEASED) == S3E_KEY_STATE_RELEASED;
	if (menu_released)
		MenuPressed = true;

	// Update accelerometer
	if (AccelerometerActive)
	{
		AccelerometerPosition.x = s3eAccelerometerGetX();
		AccelerometerPosition.y = s3eAccelerometerGetY();
		AccelerometerPosition.z = s3eAccelerometerGetZ();
	}

	// Update compass
	if (CompassActive)
	{
		CompassDirection = s3eCompassGet();
		s3eCompassHeading heading = { 0, 0, 0 };
		if (s3eCompassGetHeading(&heading) != S3E_RESULT_SUCCESS)
		{
			CompassHeading.x = heading.m_X;
			CompassHeading.y = heading.m_Y;
			CompassHeading.z = heading.m_Z;
		}
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

bool CIwGameInput::isKeyDown(s3eKey key) const
{
	if (!KeysAvailable)
		return false;

	// Return down state of queried key
	return (s3eKeyboardGetState(key) & S3E_KEY_STATE_DOWN) == S3E_KEY_STATE_DOWN;
}

bool CIwGameInput::isKeyUp(s3eKey key) const
{
	if (!KeysAvailable)
		return false;

	// Return up state of queried key
	return (s3eKeyboardGetState(key) & S3E_KEY_STATE_UP) == S3E_KEY_STATE_UP;
}

bool CIwGameInput::wasKeyPressed(s3eKey key) const
{
	if (!KeysAvailable)
		return false;

	// Return pressed state of queried key
	return (s3eKeyboardGetState(key) & S3E_KEY_STATE_PRESSED) == S3E_KEY_STATE_PRESSED;
}

bool CIwGameInput::wasKeyReleased(s3eKey key) const
{
	if (!KeysAvailable)
		return false;

	// Return released state of queried key
	return (s3eKeyboardGetState(key) & S3E_KEY_STATE_RELEASED) == S3E_KEY_STATE_RELEASED;
}

const char* CIwGameInput::showOnScreenKeyboard(const char* prompt, int flags, const char* default_text)
{
	if (!OSKeyboardAvailable)
		return NULL;

	// Show on screen keyboard and return the input string
	if (default_text != NULL)
		return s3eOSReadStringUTF8WithDefault(prompt, default_text, flags);
	else
		return s3eOSReadStringUTF8(prompt, flags);
}

bool CIwGameInput::startAccelerometer()
{
	AccelerometerActive = false;
	if (AccelerometerAvailable && s3eAccelerometerStart() == S3E_RESULT_SUCCESS)
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
		s3eAccelerometerStop();
		AccelerometerActive = false;
	}
}

bool CIwGameInput::startCompass()
{
	CompassActive = false;
	if (CompassAvailable && s3eCompassStart() == S3E_RESULT_SUCCESS)
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
		s3eCompassStop();
		CompassActive = false;
	}
}



