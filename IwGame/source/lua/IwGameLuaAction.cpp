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

#include "IwGameScriptLua.h"
#include "lua/IwGameLuaGen.h"
#include "IwGameActions.h"
#include "IwGameScene.h"
#include "IwGameActor.h"

//
// LUA_FindActions actions-name (string), scene or actor (object, optional)
//
static int LUA_FindActions(lua_State *lua)
{
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: actions.get() not enough parameters, expected actions-name (string), scene or actor (object, optional)");
		lua_pushnil(lua);
		return 1;
	}

	IIwGameXomlResource* container = NULL;
	const char* name = NULL;

	// Get the actions name
	if (lua_isstring(lua, 1))
		name = lua_tostring(lua, 1);
	if (name == NULL)
	{
        CIwGameError::LogError("Warning: actions.get() Invalid name for Param0");
		lua_pushnil(lua);
		return 1;
	}

	// Get the container object
	if (lua_isuserdata(lua, 2))
		container = (IIwGameXomlResource*)lua_touserdata(lua, 2);
	if (container != NULL && (container->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && container->getClassTypeHash() != CIwGameXomlNames::Actor_Hash))
	{
        CIwGameError::LogError("Warning: actions.get() expected scene or actor for Param1");
		lua_pushnil(lua);
		return 1;
	}

	// Locate actions
	CIwGameActions* actions = CIwGameActionsManager::FindActions(name, container);
	if (actions == NULL)
	{
        CIwGameError::LogError("Warning: actions.get() actions not found - ", name);
		lua_pushnil(lua);
		return 1;
	}

	// Return the actions
	lua_pushlightuserdata(lua, actions);

    return 1;
}

//
// LUA_CallActions object (actions), target (scene or actor)
//
static int LUA_CallActions(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 2)
	{
        CIwGameError::LogError("Warning: actions.call() not enough parameters, expected actions (object), target-scene (object)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	CIwGameActions* actions = NULL;
	if (lua_isuserdata(lua, 1))
		actions = (CIwGameActions*)lua_touserdata(lua, 1);
	if (actions == NULL || actions->getClassTypeHash() != CIwGameXomlNames::Actions_Hash)
	{
        CIwGameError::LogError("Warning: actions.call() Invalid actions object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target
	IIwGameXomlResource* object = NULL;
	if (lua_isuserdata(lua, 2))
		object = (IIwGameXomlResource*)lua_touserdata(lua, 2);
	else
	{
		CIwGameError::LogError("Warning: actions.call() Invalid target, expected scene or actor in Param1");
		lua_pushboolean(lua, false);
		return 1;
	}

	if (object->getClassTypeHash() != CIwGameXomlNames::Actor_Hash && object->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
	{
		CIwGameError::LogError("Warning: actions.call() Invalid target, expected scene or actor in Param1");
		lua_pushboolean(lua, false);
		return 1;
	}

	actions->Execute(object);
 
	lua_pushboolean(lua, true);
    return 1;
}

//
// Lua Actions library
//
static const luaL_Reg g_actionslib[] =
{
	{ "find",		LUA_FindActions }, 
	{ "call",		LUA_CallActions }, 
	{NULL, NULL}
};

LUALIB_API int luaopen_actions(lua_State *L)
{
  luaL_register(L, "actions", g_actionslib);
  
  return 1;
}
