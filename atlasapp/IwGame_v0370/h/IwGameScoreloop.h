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

#if !defined(_IW_GAME_SCORELOOP_H_)
#define _IW_GAME_SCORELOOP_H_

#include "IwGame.h"
#include "sc_client.h"

struct CIwGameScoreloopUser
{
	SC_UserDetails_h	Details;		// Handle to users details
	CIwGameString		Login;			// Users log in name
	CIwGameString		Email;			// Users email address
	CIwGameString		Nationality;	// Users nationality
	SC_Money_h			Money;			// Handle to users money details
	SC_UserState_t		State;			// Users state
};

class CIwGameScoreloopUserController
{
public:
protected:
	// Properties
public:
	// Properties end

protected:

public:
	CIwGameScoreloopUserController()
	{
	}
	~CIwGameScoreloopUserController()
	{
	}
};

// 
// 
//
//
// CIwGameScoreLoop is a singleton class that handles all Scoreloop interaction
// 
// 
// 
//
#define	IW_GAME_SCORELOOP	CIwGameScoreloop::getInstance()

class CIwGameScoreloop
{
	CDEFINE_SINGLETON(CIwGameScoreloop)

public:
protected:
	// Properties
    SC_Client_h					Client;			// Scoreloop client
    SC_UserController_h			UserController;	// Current session user controller
	CIwGameScoreloopUser		CurrentUser;	// Current users details
	CIwGameString				ErrorString;	// Last error that was reported
	bool						Error;			// True if last operation caused an error
	CIwGameCallback				DoneCallback;	// Callback to be called when done collecting details
	CIwGameCallbackData			DoneCallbackData; // Data to be passed back during the callback
public:
	SC_Client_h					geClient()				{ return Client; }
	const CIwGameScoreloopUser&	getCurrentUser()		{ return CurrentUser; }
	bool						isError() const			{ return Error; }
	CIwGameString&				getErrorString()		{ return ErrorString; }
	void						setError(const char* string)
	{
		if (string != NULL)
		{
			Error = true;
			ErrorString = string;
#if defined(_DEBUG)
			CIwGameError::LogError("Error: ", ErrorString.c_str());
#endif	// (_DEBUG)
		}
		else
			Error = false;
	}
	CIwGameCallbackData			getDoneCallbackData()	{ return DoneCallbackData; }
	// Properties end

protected:

public:
	bool			Init(const char* game_id, const char* game_secret, const char* game_currency, const char* game_language, CIwGameCallback done_callback, CIwGameCallbackData callback_data);
	void			Release();

	// Internal
	void			UpdateUserDetails();
	static void		UserControllerCallback(void* data, SC_Error_t error);

};



#endif // _IW_GAME_SCORELOOP_H_
