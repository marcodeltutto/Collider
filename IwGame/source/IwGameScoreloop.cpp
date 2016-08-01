/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engine as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#include "IwGameScoreloop.h"
#include "IwGameUtil.h"

CDECLARE_SINGLETON(CIwGameScoreloop)

//
//
//
// CIwGameScoreLoop singleton implementation
//
//
//

void CIwGameScoreloop::UserControllerCallback(void* data, SC_Error_t error)
{
    CIwGameScoreloop* self = static_cast<CIwGameScoreloop *>(data);
    if (error != SC_OK)
	{
		self->setError("Request failed");
		if (self->DoneCallback != NULL)
			self->DoneCallback(self, self->getDoneCallbackData());
		return;
    }

    self->UpdateUserDetails();
}

void CIwGameScoreloop::UpdateUserDetails()
{
	SC_User_h user = SC_Session_GetUser(SC_Client_GetSession(Client));
	CurrentUser.Login = SC_String_GetData(SC_User_GetLogin(user));
	CurrentUser.Email = SC_String_GetData(SC_User_GetEmail(user));
	SC_UserList_h buddies = SC_User_GetBuddyUsers(user);
	unsigned int buddiesCount = SC_UserList_GetUsersCount(buddies);
	SC_Money_h balance = SC_User_GetBalance(user);
	unsigned long balanceAmount = SC_Money_GetAmount(balance);

	if (DoneCallback != NULL)
		DoneCallback(this, getDoneCallbackData());
}

bool CIwGameScoreloop::Init(const char* game_id, const char* game_secret, const char* game_currency, const char* game_language, CIwGameCallback done_callback, CIwGameCallbackData callback_data)
{
	Client = NULL;
	UserController = NULL;
	DoneCallback = done_callback;
	DoneCallbackData = callback_data;

	// Create the Scoreloop client
    PAL_InitData_t palInitData;
	SC_Error_t error = SC_Client_New(&Client, &palInitData,	game_id, game_secret, game_currency, game_language);
	if (error != SC_OK)
	{
		setError("Failed to create SC_Client");
		if (DoneCallback != NULL)
			DoneCallback(this, DoneCallbackData);
		return false;
	}

	// Create session UserController
    error = SC_Client_CreateUserController(Client, &UserController, &UserControllerCallback, this);
    if (error != SC_OK)
	{
        UserController = NULL;
		setError("Failed to create SC_UserController");
		if (DoneCallback != NULL)
			DoneCallback(this, DoneCallbackData);
		return false;
    }

	return true;
}

void CIwGameScoreloop::Release()
{
	// Release user controller
    SC_UserController_Release(UserController);

	// Release Scoreloop client
	SC_Client_Release(Client);
}



