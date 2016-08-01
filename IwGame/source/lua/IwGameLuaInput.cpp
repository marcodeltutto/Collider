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

#include "s3eOSExec.h"
#include "IwGameScriptLua.h"
#include "lua/IwGameLuaGen.h"
#include "IwGameActor.h"
#include "IwGameScene.h"
#include "IwGameProgram.h"
#include "IwGameUI.h"
#include "IwGameBrush.h"
#include "IwGame.h"
#include "IwGameTemplates.h"

//
// LUA_isMultiTouch - Determines if multi-touch is supported
//
static int LUA_isMultiTouch(lua_State *lua)
{
	int count = lua_gettop(lua);
	lua_pushboolean(lua, IW_GAME_INPUT->isMultiTouch());
    return 1;
}

//
// LUA_hasCompass - Determines if compass is supported
//
static int LUA_hasCompass(lua_State *lua)
{
	int count = lua_gettop(lua);
	lua_pushboolean(lua, IW_GAME_INPUT->isCompassAvailable());
    return 1;
}

//
// LUA_hasAccelerometer - Determines if accelerometer is supported
//
static int LUA_hasAccelerometer(lua_State *lua)
{
	int count = lua_gettop(lua);
	lua_pushboolean(lua, IW_GAME_INPUT->isAccelerometerAvailable());
    return 1;
}

//
// LUA_hasKeyboard - Determines if keyboard is supported
//
static int LUA_hasKeyboard(lua_State *lua)
{
	int count = lua_gettop(lua);
	lua_pushboolean(lua, IW_GAME_INPUT->isKeysAvailable());
    return 1;
}

//
// LUA_StartCompass - Start the compass
//
static int LUA_StartCompass(lua_State *lua)
{
	int count = lua_gettop(lua);
	lua_pushboolean(lua, IW_GAME_INPUT->startCompass());
    return 1;
}

//
// LUA_StopCompass - Start the compass
//
static int LUA_StopCompass(lua_State *lua)
{
	int count = lua_gettop(lua);
	IW_GAME_INPUT->stopCompass();
	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_getCompassInfo - (returns heading_x, heading_y, heading_z, direction)
//
static int LUA_getCompassInfo(lua_State *lua)
{
	int count = lua_gettop(lua);
	CIwVec3 compass = IW_GAME_INPUT->getCompassHeading();
	lua_pushvec(lua, (float)compass.x, (float)compass.y, (float)compass.z, (float)IW_GAME_INPUT->getCompassDirection());

    return 1;
}

//
// LUA_StartAccelerometer - Start the Accelerometer
//
static int LUA_StartAccelerometer(lua_State *lua)
{
	int count = lua_gettop(lua);
	lua_pushboolean(lua, IW_GAME_INPUT->startAccelerometer());
    return 1;
}

//
// LUA_StopAccelerometer - Stop the keyboard
//
static int LUA_StopAccelerometer(lua_State *lua)
{
	int count = lua_gettop(lua);
	IW_GAME_INPUT->startAccelerometer();
	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_getAccelerometerInfo - (returns heading_x, heading_y, heading_z, direction)
//
static int LUA_getAccelerometerInfo(lua_State *lua)
{
	int count = lua_gettop(lua);
	CIwVec3 accelerometer = IW_GAME_INPUT->getAccelerometerPosition();
	lua_pushvec(lua, (float)accelerometer.x, (float)accelerometer.y, (float)accelerometer.z, 0);
    return 1;
}

//
// LUA_isBackPressed - Returns back button pressed status
//
static int LUA_isBackPressed(lua_State *lua)
{
	int count = lua_gettop(lua);
	lua_pushboolean(lua, IW_GAME_INPUT->isBackPressed());
    return 1;
}

//
// LUA_isMenuPressed - Returns menu button pressed status
//
static int LUA_isMenuPressed(lua_State *lua)
{
	int count = lua_gettop(lua);
	lua_pushboolean(lua, IW_GAME_INPUT->isMenuPressed());
    return 1;
}

//
// LUA_getTextInput - Opens on screen keyboard to get text input from user
//
static int LUA_getTextInput(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: input.textInput() not enough parameters, expected message (string) ");
		lua_pushnil(lua);
		return 1;
	}

	// Get the message to display to user
	const char* message = NULL;
	const char* default_text = NULL;
	if (lua_isstring(lua, 1))
		message = lua_tostring(lua, 1);
	if (message == NULL)
	{
        CIwGameError::LogError("Warning: input.textInput() invalid message for Param0");
		lua_pushnil(lua);
		return 1;
	}

	if (count > 1)
	{
		if (lua_isstring(lua, 2))
			default_text = lua_tostring(lua, 2);
		if (default_text == NULL)
		{
			CIwGameError::LogError("Warning: input.textInput() invalid default_text for Param1");
			lua_pushnil(lua);
			return 1;
		}
	}

	const char* data = IW_GAME_INPUT->showOnScreenKeyboard(message, 0, default_text);
	if (data == NULL)
		lua_pushnil(lua);
	else
		lua_pushstring(lua, data);

    return 1;
}

//
// LUA_getTouchInfo touch-index (number 0 to 4) - (returns x, y, touch_state)
//
static int LUA_getTouchInfo(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: input.touchInfo() not enough parameters, expected touch-index (number)");
		lua_pushnil(lua);
		return 1;
	}

	// Get index
	int index = 0;
	if (count > 0)
	{
		if (lua_isnumber(lua, 1))
			index = (int)lua_tonumber(lua, 1);
		if (index < 0 || index > 4)
		{
			CIwGameError::LogError("Warning: input.touchInfo() touch-index out of range, expected 0 to 4 for Param0");
			lua_pushnil(lua);
			return 1;
		}
	}

	CIwGameTouch* touch = IW_GAME_INPUT->getTouch(index);
	float touched = touch->touched ? 1.0f : 0.0f;
	lua_pushvec(lua, (float)touch->x, (float)touch->y, touched, (float)touch->id);

    return 1;
}

//
// Lua Actions library
//
static const luaL_Reg g_inputlib[] =
{
	{"multitouch",			LUA_isMultiTouch},
	{"compass",				LUA_hasCompass},
	{"accelerometer",		LUA_hasAccelerometer},
	{"keyboard",			LUA_hasKeyboard},
	{"startCompass",		LUA_StartCompass},
	{"stopCompass",			LUA_StopCompass},
	{"getCompass",			LUA_getCompassInfo},
	{"startAccelerometer",	LUA_StartAccelerometer},
	{"stopAccelerometer",	LUA_StopAccelerometer},
	{"getAccelerometer",	LUA_getAccelerometerInfo},
	{"backPressed",			LUA_isBackPressed},
	{"menuPressed",			LUA_isMenuPressed},
	{"textInput",			LUA_getTextInput},
	{"touchInfo",			LUA_getTouchInfo},
	{NULL, NULL}
};

LUALIB_API int luaopen_input(lua_State *L)
{
  luaL_register(L, "input", g_inputlib);
  
  return 1;
}
