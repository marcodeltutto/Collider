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

#if !defined(_IW_GAME_FACEBOOK_H_)
#define _IW_GAME_FACEBOOK_H_

//
// Classes in this file
//
//	CFacebook						- Facebook wrapper
//

#include "s3e.h"
#include "s3eFacebook.h"

#include "IwGameUtil.h"
#include "IwGameString.h"

#define	IW_GAME_FACEBOOK	CIwGameFacebook::getInstance()

//
//
//
//
//  CIwGameFacebook - Facebook wrapper
//
//
//
//
class CIwGameFacebook
{
	CDEFINE_SINGLETON(CIwGameFacebook)

public:
	enum eFBStatus 
	{
		FB_UNINITIALISED,
		FB_LOGGED_OUT,
		FB_LOGGING_IN,
		FB_LOGGED_IN
	};

	// Properties
private:
	bool			Available;
	CIwGameString	AppID;
	eFBStatus		SessionStatus;
	s3eFBSession*	CurrentSession;
	s3eFBRequest*	Request;
	s3eFBRequest*	GetUID;
	bool			WaitingForCallback;
public:
	// Properties end
	void			setAppID(const char* app_id)				{ AppID = app_id; }
	bool			isAvailable() const							{ return Available; }
	bool			isLoggedIn() const							{ return SessionStatus == FB_LOGGED_IN;; }
	void			setSessionStatus(eFBStatus status)			{ SessionStatus = status; }
	void			setCurrentSession(s3eFBSession* session)	{ CurrentSession = session; }
	void			setRequest(s3eFBRequest* request)			{ Request = request; }
	void			setGetUID(s3eFBRequest* uid)				{ GetUID = uid; }
	void			setWaitingForCallback(bool waiting)			{ WaitingForCallback = waiting; }

private:
	void	WaitForCallback();

public:
	void	Init();
	void	Release();

	bool	Login();
	bool	Logout();
	bool	PostWall(const char* message, const char* link_uri, const char* image_uri, const char* name, const char* description);

	void	DeleteRequest()
	{
		if (Request != NULL)
		{
			s3eFBRequest_Delete(Request);
			Request = NULL;
		}
	}


};


#endif // _IW_GAME_FACEBOOK_H_