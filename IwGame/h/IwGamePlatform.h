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

#if !defined(_CIW_GAME_PLATFORM_H_)
#define _CIW_GAME_PLATFORM_H_

#include "IwGameTypes.h"
#include "IwGameUtil.h"

class CIwGamePlatform;
class CIwGameAccelerometer;

//
// IIwGamePlatform - Abstract interface that provides access to platform specific hardware
//
// To add a new platforn, derive your own class from IIwGamePlatform and all other platfrom sub systems and implement all methods
// 
//
// NOTES: The platform system will eventually abstract all Marmalade specific platform code into its own class / classes
//
// IIwGamePlatformSys -   Abstract interface to main hardware systen
// IIwGamePlatformDisplay - Abstract interface to display system
// IIwGamePlatformInput - Abstract interface to input system
// IIwGamePlatformAudio - Abstract interface to audio system
// IIwGamePlatformComms - Abstract interface to communications system
// IIwGamePlatformFile -  Abstract interface to file system
// IIwGamePlatformFont -  Abstract interface to font system
// IIwGamePlatformMedia - Abstract interface to media systems
// IIwGamePlatformOther - Abstract interface to other systems not directly related to the above or utility functionality
// IIwGamePlatformUI -	  Abstract interface to UI systems
// 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IIwGamePlatformSys
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IIwGamePlatformSys
{
	// Properties
protected:
	bool		Initialised;
public:
	bool		isInitialised() const	{ return Initialised; }
	// Properties end
public:
	virtual int		Init() = 0;
	virtual void	Release() = 0;

	// Backlight
	virtual void	BacklightOn() = 0;

	// Time
	virtual uint64	getTimeInMs() = 0;

	// OS calls
	virtual void	YieldToOS(int time_ms) = 0;
	virtual bool	ExecuteViaOS(const char* name, bool exit_app) = 0;
	virtual void	RequestQuit() = 0;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IIwGamePlatformDisplay
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IIwGamePlatformDisplay
{
	// Properties
protected:
	bool		Initialised;
public:
	bool		isInitialised() const	{ return Initialised; }
	// Properties end
public:
	// Init
	virtual int		Init(bool use_gl) = 0;
	virtual void	Release() = 0;
	// Parameter query
	virtual int		getCurrentWidth() = 0;
	virtual int		getCurrentHeight() = 0;
	// Display management
	virtual void	setColour(int r, int g, int b, int a) = 0;
	virtual void	Clear() = 0;
	virtual void	Swap() = 0;


};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IIwGamePlatformInput
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IIwGamePlatformInput
{
	// Properties
protected:
	bool		Initialised;
public:
	bool		isInitialised() const	{ return Initialised; }
	// Properties end
public:
	// Init
	virtual int		Init() = 0;
	virtual void	Release() = 0;
	// Feature query
	virtual bool	isPointerAvailable() = 0;
	virtual bool	isMultiTouch() = 0;
	virtual bool	isKeysAvailable() = 0;
	virtual bool	isOSKeyboardAvailable() = 0;
	virtual bool	isAccelerometerAvailable() = 0;
	virtual bool	isCompassAvailable() = 0;
	// Buttons query
	virtual bool	wasBackButtonReleased() = 0;
	virtual bool	wasMenuButtonReleased() = 0;
	// Key query
	virtual bool	isKeyDown(int key) = 0;
	virtual bool	isKeyUp(int key) = 0;
	virtual bool	wasKeyPressed(int key) = 0;
	virtual bool	wasKeyReleased(int key) = 0;
	// Acclerometer
	virtual bool	startAccelerometer() = 0;
	virtual void	stopAccelerometer() = 0;
	virtual void	getAccelerometer(CIwVec3& data) = 0;
	// Compass
	virtual bool	startCompass() = 0;
	virtual void	stopCompass() = 0;
	virtual float	getCompassDirection() = 0;
	virtual bool	getCompassHeading(CIwVec3& data) = 0;
	// OS Keyboard
	virtual const char*	OsKeyboardRead(const char* prompt, const char* default_text, int flags) = 0;

	// Update
	virtual bool	Update() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IIwGamePlatformAudio
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IIwGamePlatformAudio
{
	// Properties
protected:
	bool		Initialised;
public:
	bool		isInitialised() const	{ return Initialised; }
	// Properties end
public:
	virtual int Init() = 0;
	virtual void Release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IIwGamePlatformComms
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IIwGamePlatformComms
{
	// Properties
protected:
	bool		Initialised;
public:
	bool		isInitialised() const	{ return Initialised; }
	// Properties end
public:
	virtual int Init() = 0;
	virtual void Release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IIwGamePlatformFile
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IIwGamePlatformFile
{
	// Properties
protected:
	bool		Initialised;
public:
	bool		isInitialised() const	{ return Initialised; }
	// Properties end
public:
	virtual int		Init() = 0;
	virtual void	Release() = 0;

	virtual CxFile	Open(const char* filename, const char* mode) = 0;
	virtual CxFile	OpenFromMemory(void* memory, uint num_bytes) = 0;
	virtual void	Close(CxFile file) = 0;
	virtual bool	Seek(CxFile file, int offset, CxFileSeekOrigin origin) = 0;
	virtual uint	Read(CxFile file, void* buffer, uint num_bytes) = 0;
	virtual uint	Write(CxFile file, void* buffer, uint num_bytes) = 0;
	virtual bool	Exists(const char* filename) = 0;
	virtual bool	Delete(const char* filename) = 0;
	virtual int		getSize(CxFile file) = 0;
	virtual const char*	getErrorString() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IIwGamePlatformFont
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IIwGamePlatformFont
{
	// Properties
protected:
	bool		Initialised;
public:
	bool		isInitialised() const	{ return Initialised; }
	// Properties end
public:
	virtual int Init() = 0;
	virtual void Release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IIwGamePlatformMedia
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IIwGamePlatformMedia
{
	// Properties
protected:
	bool		Initialised;
public:
	bool		isInitialised() const	{ return Initialised; }
	// Properties end
public:
	virtual int Init() = 0;
	virtual void Release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IIwGamePlatformOther
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IIwGamePlatformOther
{
	// Properties
protected:
	bool		Initialised;
public:
	bool		isInitialised() const	{ return Initialised; }
	// Properties end
public:
	virtual int Init() = 0;
	virtual void Release() = 0;

	// Vibration
	virtual void VibrateStart(float priority, float duration) = 0;
	virtual void VibrateStop() = 0;



	// Marmalade dependencies - TODO: Remove dependency
	virtual void InitResManager() = 0;
	virtual void ReleaseResManager() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IIwGamePlatformUI
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IIwGamePlatformUI
{
	// Properties
protected:
	bool		Initialised;
public:
	bool		isInitialised() const	{ return Initialised; }
	// Properties end
public:
	virtual int Init() = 0;
	virtual void Release() = 0;
};



class IIwGamePlatform
{
public:
	virtual int		Init() = 0;
	virtual void	Release() = 0;

};

#define	PLATFORM			CIwGamePlatform::getInstance()
#define	PLATFORM_AUDIO		PLATFORM->getAudio()
#define	PLATFORM_COMMS		PLATFORM->getComms()
#define	PLATFORM_FILE		PLATFORM->getFile()
#define	PLATFORM_FONT		PLATFORM->getFont()	
#define	PLATFORM_INPUT		PLATFORM->getInput()
#define	PLATFORM_MEDIA		PLATFORM->getMedia()
#define	PLATFORM_OTHER		PLATFORM->getOther()
#define	PLATFORM_SYS		PLATFORM->getSys()
#define	PLATFORM_UI			PLATFORM->getUI()
#define	PLATFORM_DISPLAY	PLATFORM->getDisplay()

class CIwGamePlatform
{
private:
	static CIwGamePlatform* _instance;
	CIwGamePlatform() : Initialised(false), Sys(NULL), Display(NULL), Input(NULL), Audio(NULL), Comms(NULL), File(NULL), Font(NULL), Media(NULL), Other(NULL), UI(NULL), Platform(NULL)  {}
	~CIwGamePlatform() {}
	CIwGamePlatform(const CIwGamePlatform &);
	CIwGamePlatform& operator=(const CIwGamePlatform &);
public:
	static void Create();
	static void Destroy();
	static CIwGamePlatform* getInstance();

	// Properties
protected:
	bool					Initialised;
	IIwGamePlatform*		Platform;
public:
	bool		isInitialised() const			{ return Initialised; }
	IIwGamePlatform*		getPlatform()		{ return Platform; }
	IIwGamePlatformSys*		getSys()			{ return Sys; }
	IIwGamePlatformDisplay*	getDisplay()		{ return Display; }
	IIwGamePlatformInput*	getInput()			{ return Input; }
	IIwGamePlatformAudio*	getAudio()			{ return Audio; }
	IIwGamePlatformComms*	getComms()			{ return Comms; }
	IIwGamePlatformFont*	getFont()			{ return Font; }
	IIwGamePlatformFile*	getFile()			{ return File; }
	IIwGamePlatformMedia*	getMedia()			{ return Media; }
	IIwGamePlatformOther*	getOther()			{ return Other; }
	IIwGamePlatformUI*		getUI()				{ return UI; }
	void setSys(IIwGamePlatformSys* p)			{ Sys = p; }
	void setDisplay(IIwGamePlatformDisplay* p)	{ Display = p; }
	void setInput(IIwGamePlatformInput* p)		{ Input = p; }
	void setAudio(IIwGamePlatformAudio* p)		{ Audio = p; }
	void setComms(IIwGamePlatformComms* p)		{ Comms = p; }
	void setFont(IIwGamePlatformFont* p)		{ Font = p; }
	void setFile(IIwGamePlatformFile* p)		{ File = p; }
	void setMedia(IIwGamePlatformMedia* p)		{ Media = p; }
	void setOther(IIwGamePlatformOther* p)		{ Other = p; }
	void setUI(IIwGamePlatformUI* p)			{ UI = p; }
	// Properties end

protected:
	IIwGamePlatformSys*		Sys;
	IIwGamePlatformDisplay*	Display;
	IIwGamePlatformInput*	Input;
	IIwGamePlatformAudio*	Audio;
	IIwGamePlatformComms*	Comms;
	IIwGamePlatformFile*	File;
	IIwGamePlatformFont*	Font;
	IIwGamePlatformMedia*	Media;
	IIwGamePlatformOther*	Other;
	IIwGamePlatformUI*		UI;

public:
	virtual int		Init(IIwGamePlatform* platform, bool comms, bool use_gl);
	virtual void	Release();

};

#endif	// _CIW_GAME_PLATFORM_H_
