#include "IwGameFacebook.h"



//
//
//
//
// CIwGameFacebook implementation
//
//
//
//
CDECLARE_SINGLETON(CIwGameFacebook)

void CIwGameFacebook::Init()
{
	Available = false;
	SessionStatus = FB_UNINITIALISED;
	CurrentSession = 0;
	Request = 0;

	if (s3eExtFacebookAvailable())
	{
		Available = true;
	}
	else
	{
	}
}

void CIwGameFacebook::Release()
{
	if (Request)
		s3eFBRequest_Delete(Request);

	if (CurrentSession)
		s3eFBTerminate(CurrentSession);
}


static void FBLoginCallback(struct s3eFBSession* session, s3eResult* loginResult, void* userData)
{
    IwTrace(FACEBOOK, ("FBLoginCallback %p %p %p", session, loginResult, userData));
    
    if (*loginResult == S3E_RESULT_SUCCESS)
    {
		IW_GAME_FACEBOOK->setSessionStatus(CIwGameFacebook::FB_LOGGED_IN);
    }
    else
    {
        IW_GAME_FACEBOOK->setSessionStatus(CIwGameFacebook::FB_LOGGED_OUT); // Note: this currently never occurs
    }

    IW_GAME_FACEBOOK->setWaitingForCallback(false);
}

void CIwGameFacebook::WaitForCallback()
{
    WaitingForCallback = true;
    while (WaitingForCallback && !s3eDeviceCheckQuitRequest())
    {
        s3eDeviceYield(100);
    }
}

bool CIwGameFacebook::Login()
{
	if (!Available)
		return false;

	if (!CurrentSession)
		CurrentSession = s3eFBInit(AppID.c_str());

	if (CurrentSession)
	{
		// Log in to Facebook using the session.
		const char* permissions[] = { "read_stream", "publish_stream" };
		s3eFBSession_Login(CurrentSession, FBLoginCallback, NULL, permissions, sizeof(permissions)/sizeof(permissions[0]));

		SessionStatus = FB_LOGGING_IN;

		WaitForCallback();

		s3eBool loggedIn = s3eFBSession_LoggedIn(CurrentSession);
	}
	else
	{
		SessionStatus = FB_UNINITIALISED;
		return false;
	}

	return true;
}

bool CIwGameFacebook::Logout()
{
	s3eFBSession_Logout(CurrentSession);
	SessionStatus = FB_LOGGED_OUT;

	return true;
}

static void FBRequestCallback(struct s3eFBRequest* request, s3eResult* requestResult, void* userData)
{
	if (*requestResult == S3E_RESULT_SUCCESS)
    {
		if (s3eFBRequest_ResponseType(request) == STRING_TYPE)
		{
		}
		else
		{
			// If this returned data then we could process it
		}
    }
	else
	{
		// Request failed
	}

    // Clear our single global request and allow reposting
    IW_GAME_FACEBOOK->DeleteRequest();
}

bool CIwGameFacebook::PostWall(const char* message, const char* link_uri, const char* image_uri, const char* name, const char* description)
{
	if (!Available)
		return false;

	if (!isLoggedIn())
	{
		Login();
	}

	Request = s3eFBRequest_WithMethodName(CurrentSession, "facebook.stream.publish", "POST");

	if (!Request)
	{
		return false;
	}
	else
	{
		// Build JSON attachment
		CIwGameString attachment;
		attachment.setAutoHash(false);
		attachment.allocString(1024);

		attachment += "{ ";
		attachment += "\"name\": \"";
		attachment += (char*)name;
		attachment += "\", ";
		attachment += "\"href\": \"";
		attachment += (char*)link_uri;
		attachment += "\", ";
		attachment += "\"description\": \"";
		attachment += (char*)description;
		attachment += "\", ";
		attachment += "\"media\": [ { \"type\": \"image\", ";
		attachment += "\"src\": \"";
		attachment += (char*)image_uri;
		attachment += "\", ";
		attachment += "\"href\": \"";
		attachment += (char*)link_uri;
		attachment += "\", ";
		attachment += "}] }";


		// Add a value to the request and send
		s3eFBRequest_AddParamString(Request, "message", message);
		s3eFBRequest_AddParamString(Request, "attachment", attachment.c_str());
                
		if (s3eFBRequest_Send(Request, FBRequestCallback, NULL) == S3E_RESULT_ERROR)
		{
			s3eFBRequest_Delete(Request);
			return false;
		}
		else
		{
		}
	}

	return true;
}



