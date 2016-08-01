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

#if !defined(_IW_GAME_INPUT_H_)
#define _IW_GAME_INPUT_H_

#include "IwGamePlatform.h"
#include "IwGameUtil.h"

#define IW_GAME_MAX_TOUCHES		10
#define IW_GAME_TAP_TIME_MS		200
#define IW_GAME_TAP_SENSITIVITY	(16 * 16 + 16 * 16)

#define	IW_GAME_INPUT	(CIwGameInput::getInstance())

//
// eIwGameInputTypeHint is passed to showOnScreenKeyboard() to hint at what type of keyboard entry to show
//
enum eIwGameInputTypeHint
{
	ITH_Text		= 0, 
	ITH_Number		= 1, 
	ITH_Password	= 2, 
	ITH_Email		= 3, 
	ITH_URL			= 4
};

//
//
// CIwGameTouch - Represents a single touch
//
//
struct CIwGameTouch
{
public:
    int			x, y;			// Touch position
    int			px, py;			// Previous touch position
    int			dx, dy;			// Delta position
	bool		touched;
    bool		active;			// Touch active state
    bool		prev_active;	// Touch active state (last frame)
    int			id;				// ID of touch - The system tracks multiple touches by assigning each one a unique ID

	CIwGameTouch() : active(false), prev_active(false), id(-1), touched(false) {}
};


//
//
// CIwGameInput - The IwGameInput class is responsible for handling all keyboard and pointer input
//
//
class CIwGameInput
{
	CDEFINE_SINGLETON(CIwGameInput)

	// Properties
private:
	bool			PointerAvailable;				// true if a pointer is present
	bool			KeysAvailable;					// true if a key input is present
	bool			OSKeyboardAvailable;			// true if on screen keyboard is available
	bool			AccelerometerAvailable;			// true if accelerometer is available
	bool			CompassAvailable;				// true if compass is available
	bool			IsMultiTouch;					// true if multitouch is enabled
	CIwGameTouch	Touches[IW_GAME_MAX_TOUCHES];	// List of potential touches
	CIwGameTouch*	FirstTouch;						// First touch that was made
	CIwVec2			DragDelta;						// Amount dragged
	CIwVec2			TouchedPos;						// Position user touched initially
	bool			Touched;						// true when user is touching the screen
	bool			Tapped;							// true when user taps screen (cleared in next update)
	bool			BackPressed;					// Back key pressed state
	bool			MenuPressed;					// Menu key pressed state
	bool			AccelerometerActive;			// Active state of accelerometer
	bool			CompassActive;					// Active state of compass
	CIwVec3			AccelerometerReference;			// Accelerometer reference position
	CIwVec3			AccelerometerPosition;			// Current accelerometer position
	CIwVec3			CompassHeading;					// Current compass heading
	float			CompassDirection;				// Current compass direction
public:
	// Availability query
	bool			isPointerAvailable() const		{ return PointerAvailable; }	// Returns availability of the pointer
	bool			isKeysAvailable() const			{ return KeysAvailable; }		// Returns availability of keys
	bool			isOSKeyboardAvailable() const	{ return OSKeyboardAvailable; }	// Returns availability of on screen keyboard
	bool			isAccelerometerAvailable() const { return AccelerometerAvailable; } // Returns availability of accelerometer
	bool			isCompassAvailable() const		{ return CompassAvailable; }	// Returns true if compass is available
	bool			isMultiTouch() const			{ return IsMultiTouch; }		// Returns multitouch capability

	// Pointer
	CIwGameTouch*	getTouchByID(int id);											// Returns the touch identified by its id
	CIwGameTouch*	getTouch(int index)				{ if (index < 0 || index >= IW_GAME_MAX_TOUCHES) return NULL; else return &Touches[index]; }		// Gets a specific touch
	CIwGameTouch*	getFirstTouch()					{ return FirstTouch; }			// Returns last touch that was made
	CIwGameTouch*	findTouch(int id);												// Finds a specific touch by its id
	int				getTouchCount() const;											// Get number of touches this frame
	bool			hasTapped() const				{ return Tapped; }				// Returns tapped status
	bool			isTouching(int index) const		{ return Touches[index].touched; }
	CIwVec2			getTouchedPos(int index) const	{ return CIwVec2(Touches[index].x, Touches[index].y); }
	CIwVec2			getDragDelta(int index) const	{ return CIwVec2(Touches[index].dx, Touches[index].dy); }
	bool			isDragging(int index) const;

	// keys / Buttons
	bool			isKeyDown(int key) const;										// Tests if a key is down
	bool			isKeyUp(int key) const;											// Tests if a key is up
	bool			wasKeyPressed(int key) const;									// Tests if a key was pressed
	bool			wasKeyReleased(int key) const;									// Tests if a key was released
	const char*		showOnScreenKeyboard(const char* prompt, int flags = 0, const char* default_text = NULL);
	bool			isBackPressed()					{ return BackPressed; }
	void			resetBackPressed()				{ BackPressed = false; }
	bool			isMenuPressed()					{ return MenuPressed; }
	void			resetMenuPressed()				{ MenuPressed = false; }
	s3eKey			getLastKey() const				{ return s3eKeyboardAnyKey(); }

	// Accelerometer
	bool			startAccelerometer();											// Start accelerometer input
	void			stopAccelerometer();											// Stop accelerometer input
	void			setAccelerometerReference();									// Sets the current accelerometer position as a reference posisition 
	CIwVec3			getAccelerometerPosition() const { return AccelerometerPosition; }	// Get current accelerometer position
	CIwVec3			getAccelerometerOffset() const { return AccelerometerPosition - AccelerometerReference; }	// Get current accelerometer offset from the reference position

	// Compass
	bool			startCompass();													// Start compass input
	void			stopCompass();													// Stop compass input
	CIwVec3			getCompassHeading() const { return CompassHeading; }			// Get current compass heading
	float			getCompassDirection() const { return CompassDirection; }		// Get current compass direction (0 to 360 degrees)

	// Properties end

private:
	CIwGameTimer	TapTimer;
	int				TouchID;

public:
	bool			Init();							// Initialises the input system (returns true if pointer is supported)
	void			Release();						// Releases data used by the input system
	void			Update();						// Updates the input system, called every frame
};




#endif	// _IW_GAME_INPUT_H_