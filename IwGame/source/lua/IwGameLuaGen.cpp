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
#include "IwGame.h"
#include "IwGamePlatform.h"
#include "IwGameSettings.h"

//
//
// Common functions
//
//
bool ValueToString(lua_State *lua, int stack_index, CIwGameString &string)
{
    if (lua_isstring(lua, stack_index))
        string = lua_tostring(lua, stack_index);
	else
    if (lua_isnumber(lua, stack_index))
        string = (float)lua_tonumber(lua, stack_index);
	else
    if (lua_isboolean(lua, stack_index))
        string = lua_toboolean(lua, stack_index) != 0;
	else
    if (lua_isvec(lua, stack_index))
	{
        const float* v = lua_tovec(lua, stack_index);
		string = *v++;
		string += ",";
		string += *v++;
		string += ",";
		string += *v++;
		string += ",";
		string += *v;
	}
	else
		return false;

	return true;
}

//
// LUA_isType(type-name)
//
static int LUA_isType(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: sys.isType() not enough parameters, expected type-name");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the resource
	IIwGameXomlResource* resource = NULL;
	if (lua_isuserdata(lua, 1))
		resource = (CIwGameScene*)lua_touserdata(lua, 1);
	else
	{
        CIwGameError::LogError("Warning: sys.isTypeOf() Invalid parameter, expected object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the type of actor to create
	const char* type = NULL;
	if (lua_isstring(lua, 2))
		type = lua_tostring(lua, 2);
	else
	{
        CIwGameError::LogError("Warning: sys.isTypeOf() Invalid parameter, expected type-name for Param1");
		lua_pushboolean(lua, false);
		return 1;
	}
	unsigned int type_hash = IW_GAME_HASH(type);

	if (resource->getActualClassTypeHash() == type_hash)
		lua_pushboolean(lua, true);
	else
		lua_pushboolean(lua, false);


	return 1;
}

//
// LUA_isTypeOf(type-name)
//
static int LUA_isTypeOf(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: sys.isTypeOf() not enough parameters, expected type-name");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the resource
	IIwGameXomlResource* resource = NULL;
	if (lua_isuserdata(lua, 1))
		resource = (CIwGameScene*)lua_touserdata(lua, 1);
	else
	{
        CIwGameError::LogError("Warning: sys.isTypeOf() Invalid parameter, expected object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the type of actor to create
	const char* type = NULL;
	if (lua_isstring(lua, 2))
		type = lua_tostring(lua, 2);
	else
	{
        CIwGameError::LogError("Warning: sys.isTypeOf() Invalid parameter, expected type-name for Param1");
		lua_pushboolean(lua, false);
		return 1;
	}
	unsigned int type_hash = IW_GAME_HASH(type);


	if (resource->getClassTypeHash() == type_hash)
		lua_pushboolean(lua, true);
	else
		lua_pushboolean(lua, false);


	return 1;
}

//
// LUA_LaunchURL - Launches a URL
//
static int LUA_LaunchURL(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: sys.launchURL() not enough parameters, expected url (string) ");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the url
	const char* url = NULL;
	if (lua_isstring(lua, 1))
		url = lua_tostring(lua, 1);
	if (url == NULL)
	{
        CIwGameError::LogError("Warning: sys.launchURL(),  invalid url for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	s3eOSExecExecute(url, false);

	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_Exit - Exit the program
//
static int LUA_Exit(lua_State *lua)
{
	int count = lua_gettop(lua);
	s3eDeviceRequestQuit();

	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_getTimeMs - Returns current time in millseconds
//
static int LUA_getTimeMs(lua_State *lua)
{
	int count = lua_gettop(lua);
	lua_pushnumber(lua, (double)s3eTimerGetMs());
    return 1;
}

//
// LUA_getDeviceType - Returns device type
//
static int LUA_getDeviceType(lua_State *lua)
{
	int count = lua_gettop(lua);

	CIwGameUtils::eDeviceType type = CIwGameUtils::GetDeviceType();
	switch (type)
	{
	case CIwGameUtils::DeviceType_iPhone:
		lua_pushstring(lua, "iphone");
		break;
	case CIwGameUtils::DeviceType_iPad:
		lua_pushstring(lua, "ipad");
		break;
	case CIwGameUtils::DeviceType_Android:
		lua_pushstring(lua, "android");
		break;
/*	case CIwGameUtils::DeviceType_Bada:
		break;
	case CIwGameUtils::DeviceType_QNX:
		break;
	case CIwGameUtils::DeviceType_Symbian:
		break;
	case CIwGameUtils::DeviceType_WinMobile:
		break;
	case CIwGameUtils::DeviceType_WebOS:
		break;*/
	case CIwGameUtils::DeviceType_Windows:
		lua_pushstring(lua, "windows");
		break;
/*	case CIwGameUtils::DeviceType_OSX:
		break;*/
	default:
		lua_pushstring(lua, "unsupported");
	}

    return 1;
}

//
// LUA_getLocale - Returns devices language (ISO 639 and ISO 3166 formats, e.g. en_GB)
//
static int LUA_getLocale(lua_State *lua)
{
	int count = lua_gettop(lua);

	const char* locale = s3eDeviceGetString(S3E_DEVICE_LOCALE);
	lua_pushstring(lua, locale);

    return 1;
}

//
// LUA_getFreeMem - Returns current free memory
//
static int LUA_getFreeMem(lua_State *lua)
{
	int count = lua_gettop(lua);
	lua_pushnumber(lua, (double)s3eMemoryGetInt(S3E_MEMORY_FREE));
    return 1;
}

//
// LUA_StartVibrate - Starts vibration - duration (number), priority (number, optional)
//
static int LUA_StartVibrate(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: sys.vibrate() not enough parameters, expected duration (number), priority (number 0 to 255, optional)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get duration
	float duration = 0.5;
	if (lua_isnumber(lua, 1))
		duration = (float)lua_tonumber(lua, 1);
	else
	{
        CIwGameError::LogError("Warning: sys.vibrate(), expected number for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get priority
	float priority = 255.0f;
	if (count > 1)
	{
		if (lua_isnumber(lua, 2))
			priority = (float)lua_tonumber(lua, 2);
		else
		{
	        CIwGameError::LogError("Warning: sys.vibrate(), expected number for Param1");
			lua_pushboolean(lua, false);
			return 1;
		}
	}

	PLATFORM->getOther()->VibrateStart(priority, duration);

	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_StopVibrate - Returns current free memory
//
static int LUA_StopVibrate(lua_State *lua)
{
	int count = lua_gettop(lua);

	PLATFORM->getOther()->VibrateStop();

	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_ChangePowerSaving - command (string (start, stop))
//
static int LUA_ChangePowerSaving(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: media.powerSaving() not enough parameters, expected enable (boolean)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get command
	bool enabled = false;
	if (lua_isboolean(lua, 1))
		enabled = lua_toboolean(lua, 1) != 0;
	else
	{
		CIwGameError::LogError("Warning: media.powerSaving() invalid enable (Param0), expected boolean");
		lua_pushboolean(lua, false);
		return 1;
	}

	IW_GAME_SETTINGS->setPreventPowerSaving(!enabled);

	lua_pushboolean(lua, true);
 
    return 1;
}


//
// Lua AppEasy System library
//
static const luaL_Reg g_syslib[] = {
	{"isType",				LUA_isType}, 
	{"isTypeOf",			LUA_isTypeOf}, 
	{"launchURL",			LUA_LaunchURL}, 
	{"exit",				LUA_Exit}, 
	{"getTimeMs",			LUA_getTimeMs}, 
	{"getDeviceType",		LUA_getDeviceType}, 
	{"getLocale",			LUA_getLocale}, 
	{"getFreeMem",			LUA_getFreeMem}, 
	{"vibrate",				LUA_StartVibrate}, 
	{"stopVibrate",			LUA_StopVibrate}, 
	{"changePowerSaving",	LUA_ChangePowerSaving}, 
	{NULL, NULL}
};

LUALIB_API int luaopen_sys(lua_State *L)
{
	luaL_register(L, "sys", g_syslib);

	return 1;
}


