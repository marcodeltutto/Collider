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
// LUA_FindTimeline timeline-name (string), scene or actor (object, optional)
//
static int LUA_FindTimeline(lua_State *lua)
{
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: timeline.find() not enough parameters, expected timeline-name (string), scene or actor (object, optional)");
		lua_pushnil(lua);
		return 1;
	}

	IIwGameXomlResource* container = NULL;
	const char* name = NULL;

	// Get the variable name
	if (lua_isstring(lua, 1))
		name = lua_tostring(lua, 1);
	if (name == NULL)
	{
        CIwGameError::LogError("Warning: timeline.find() invalid name for Param0");
		lua_pushnil(lua);
		return 1;
	}

	// Get the container object
	if (lua_isuserdata(lua, 2))
		container = (IIwGameXomlResource*)lua_touserdata(lua, 2);
	if (container != NULL && (container->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && container->getClassTypeHash() != CIwGameXomlNames::Actor_Hash))
	{
        CIwGameError::LogError("Warning: timeline.find() expected scene or actor for Param1");
		lua_pushnil(lua);
		return 1;
	}

	// Locate timeline
	CIwGameAnimTimeline* timeline = CIwGameAnimTimelinesManager::FindTimeline(name, container);
	if (timeline == NULL)
	{
        CIwGameError::LogError("Warning: timeline.find() timeline not found - ", name);
		lua_pushnil(lua);
		return 1;
	}

	// Return the timeline
	lua_pushlightuserdata(lua, timeline);

    return 1;
}

//
// LUA_StopTimeline object (timeline)
//
static int LUA_StopTimeline(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: timeline.stop() not enough parameters, expected timeline (object)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the program
	CIwGameAnimTimeline* timeline = NULL;
	if (lua_isuserdata(lua, 1))
		timeline = (CIwGameAnimTimeline*)lua_touserdata(lua, 1);
	if (timeline == NULL)
	{
        CIwGameError::LogError("Warning: timeline.stop() Invalid timeline object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	timeline->stop();
 
	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_PlayTimeline object (timeline)
//
static int LUA_PlayTimeline(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: timeline.play() not enough parameters, expected timeline (object)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the program
	CIwGameAnimTimeline* timeline = NULL;
	if (lua_isuserdata(lua, 1))
		timeline = (CIwGameAnimTimeline*)lua_touserdata(lua, 1);
	if (timeline == NULL)
	{
        CIwGameError::LogError("Warning: timeline.play() Invalid timeline object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	timeline->play();
 
	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_RestartTimeline object (timeline)
//
static int LUA_RestartTimeline(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: timeline.restart() not enough parameters, expected timeline (object)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the program
	CIwGameAnimTimeline* timeline = NULL;
	if (lua_isuserdata(lua, 1))
		timeline = (CIwGameAnimTimeline*)lua_touserdata(lua, 1);
	if (timeline == NULL)
	{
        CIwGameError::LogError("Warning: timeline.restart() Invalid timeline object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	timeline->restart();
 
	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_PauseTimeline object (timeline)
//
static int LUA_PauseTimeline(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: timeline.pause() not enough parameters, expected timeline (object)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the program
	CIwGameAnimTimeline* timeline = NULL;
	if (lua_isuserdata(lua, 1))
		timeline = (CIwGameAnimTimeline*)lua_touserdata(lua, 1);
	if (timeline == NULL)
	{
        CIwGameError::LogError("Warning: timeline.pause() Invalid timeline object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	timeline->pause();
 
	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_isTimelinePlaying object (timeline)
//
static int LUA_isTimelinePlaying(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: timeline.playing() not enough parameters, expected timeline (object)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	CIwGameAnimTimeline* timeline = NULL;
	if (lua_isuserdata(lua, 1))
		timeline = (CIwGameAnimTimeline*)lua_touserdata(lua, 1);
	if (timeline == NULL)
	{
        CIwGameError::LogError("Warning: timeline.playing() Invalid timeline object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	lua_pushboolean(lua, timeline->isPlaying());
    return 1;
}

static const luaL_Reg g_timelinelib[] =
{
	{ "find",				LUA_FindTimeline}, 
	{ "stop",				LUA_StopTimeline}, 
	{ "play",				LUA_PlayTimeline}, 
	{ "pause",				LUA_PauseTimeline}, 
	{ "restart",			LUA_RestartTimeline}, 
	{ "playing",			LUA_isTimelinePlaying}, 
	{NULL, NULL}
};

LUALIB_API int luaopen_timeline(lua_State *L)
{
	luaL_register(L, "timeline", g_timelinelib);

	return 1;
}
