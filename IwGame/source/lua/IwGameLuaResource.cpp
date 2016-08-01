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
// LUA_FindResource resource-name (string), resurce_type (string), container scene (object, optional)
//
static int LUA_FindResource(lua_State *lua)
{
	int count = lua_gettop(lua);

	if (count < 2)
	{
        CIwGameError::LogError("Warning: resource.find() not enough parameters, expected resource-name (string), resource_type (string), container scene (object, optional)");
		lua_pushnil(lua);
		return 1;
	}

	IIwGameXomlResource* scene = NULL;
	const char* name = NULL;
	const char* type = NULL;

	// Get the resource name
	if (lua_isstring(lua, 1))
		name = lua_tostring(lua, 1);
	if (name == NULL)
	{
        CIwGameError::LogError("Warning: resource.find() Invalid name for Param0");
		lua_pushnil(lua);
		return 1;
	}

	// Get the resource type
	if (lua_isstring(lua, 2))
		type = lua_tostring(lua, 2);
	if (type == NULL)
	{
        CIwGameError::LogError("Warning: resource.find() Invalid type for Param1");
		lua_pushnil(lua);
		return 1;
	}

	// Get the container scene
	if (count > 2)
	{
		if (lua_isuserdata(lua, 3))
			scene = (IIwGameXomlResource*)lua_touserdata(lua, 3);
		if (scene != NULL && scene->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
		{
			CIwGameError::LogError("Warning: resource.find() expected scene for Param1");
			lua_pushnil(lua);
			return 1;
		}
	}
	else
		scene = (CIwGameScene*)lua->user_data;

	// Locate resource
	IIwGameXomlResource* resource = CIwGameXomlResourceManager::FindResource(name, IW_GAME_HASH(type), scene);
	if (resource == NULL)
	{
        CIwGameError::LogError("Warning: resource.find() resource not found - ", name);
		lua_pushnil(lua);
		return 1;
	}

	// Return the resource
	lua_pushlightuserdata(lua, resource);

    return 1;
}

//
// LUA_Destroy(resource (object))
//
static int LUA_Destroy(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: resource.destroy() not enough parameters, expected object");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the main game object
	CIwGame* game = (CIwGame*)IW_GAME_GLOBAL_RESOURCES->getResourceManager()->getParent();

	// Get the object
	IIwGameXomlResource* object = NULL;
	if (lua_isuserdata(lua, 1))
		object = (IIwGameXomlResource*)lua_touserdata(lua, 1);
	if (object == NULL)
	{
		CIwGameError::LogError("Warning: resource.destroy() Invalid object, expected an object in Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	object->getParent()->removeResource(object);

	lua_pushboolean(lua, true);
	return 1;
}

//
// LUA_DestroyTagged(type-name)
//
static int LUA_DestroyTagged(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: resource.destroyTagged() not enough parameters, expected tag-name");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the type of actor to create
	const char* tag = NULL;
	if (lua_isstring(lua, 1))
		tag = lua_tostring(lua, 1);
	else
	{
        CIwGameError::LogError("Warning: resource.destroyTagged() Invalid parameter, expected tag-name for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}
	unsigned int tag_hash = IW_GAME_HASH(tag);

	IW_GAME_GLOBAL_RESOURCES->getResourceManager()->removeTaggedResources(tag_hash);

	lua_pushboolean(lua, true);
	return 1;
}

static const luaL_Reg g_resourcelib[] =
{
	{"find",			LUA_FindResource}, 
	{"destroy",			LUA_Destroy}, 
	{"destroyTagged",	LUA_DestroyTagged}, 
	{NULL, NULL}
};

LUALIB_API int luaopen_resource(lua_State *L)
{
	luaL_register(L, "resource", g_resourcelib);

	return 1;
}

