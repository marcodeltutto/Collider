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
// LUA_SetPropertyScene scene (object), property (string), value (string)
//
static int LUA_SetPropertyScene(lua_State *lua)
{
	if (lua_gettop(lua) < 3)
	{
        CIwGameError::LogError("Warning: scene.set() not enough parameters, expected scene (object), property (string), value (string)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	IIwGameXomlResource* object = NULL;
	if (lua_isuserdata(lua, 1))
		object = (IIwGameXomlResource*)lua_touserdata(lua, 1);
	if (object == NULL || object->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
	{
        CIwGameError::LogError("Warning: scene.set() Invalid target object, expected scene");
		lua_pushboolean(lua, false);
		return 1;
	}

	CIwGameString prop_name, value;
    if (lua_isstring(lua, 2))
        prop_name = lua_tostring(lua, 2);
    else
	{
        CIwGameError::LogError("Warning: scene.set() property must be a property name - object - ", object->getName().c_str());
		lua_pushboolean(lua, false);
		return 1;
	}

	if (!ValueToString(lua, 3, value))
	{
        CIwGameError::LogError("Warning: scene.set() value must be a string, number, boolean or vec - object - ", object->getName().c_str());
		lua_pushboolean(lua, false);
		return 1;
	}

	((CIwGameScene*)object)->setProperty(prop_name.getHash(), value, false);

	lua_pushboolean(lua, true);
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_AddPropertyScene scene (object), property (string), value (string)
//
static int LUA_AddPropertyScene(lua_State *lua)
{
	if (lua_gettop(lua) < 3)
	{
        CIwGameError::LogError("Warning: LUA_AddProperty not enough parameters, expected scene (object), property (string), value (string)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	IIwGameXomlResource* object = NULL;
	if (lua_isuserdata(lua, 1))
		object = (IIwGameXomlResource*)lua_touserdata(lua, 1);
	if (object == NULL || object->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
	{
        CIwGameError::LogError("Warning: scene.add() Invalid target object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	CIwGameString prop_name, value;
    if (lua_isstring(lua, 2))
        prop_name = lua_tostring(lua, 2);
    else
	{
        CIwGameError::LogError("Warning: scene.add() property must be a property name (Param1) - object - ", object->getName().c_str());
		lua_pushboolean(lua, false);
		return 1;
	}

	if (!ValueToString(lua, 3, value))
	{
        CIwGameError::LogError("Warning: scene.add() value must be a string, number, boolean or vec  - object - ", object->getName().c_str());
		lua_pushboolean(lua, false);
		return 1;
	}

	((CIwGameScene*)object)->setProperty(prop_name.getHash(), value, true);

    lua_pushboolean(lua, true);
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_GetPropertyScene scene (object), property (string)
//
static int LUA_GetPropertyScene(lua_State *lua)
{
	if (lua_gettop(lua) < 2)
	{
        CIwGameError::LogError("Warning: scene.get() not enough parameters, expected scene (object), property (string)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the target object
	IIwGameXomlResource* object = NULL;
	if (lua_isuserdata(lua, 1))
		object = (IIwGameXomlResource*)lua_touserdata(lua, 1);
	if (object == NULL || object->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
	{
        CIwGameError::LogError("Warning: scene.get() Invalid target object for Param0");
		lua_pushnil(lua);
		return 1;
	}

	CIwGameString prop_name, value;
    if (lua_isstring(lua, 2))
        prop_name = lua_tostring(lua, 2);
    else
	{
        CIwGameError::LogError("Warning: scene.get() property must be a property name (Param1) - object - ", object->getName().c_str());
		lua_pushnil(lua);
		return 1;
	}

	CIwGameXomlProperty prop;
	if (!((CIwGameScene*)object)->getProperty(prop_name.getHash(), prop))
	{
		CIwGameError::LogError("Warning: LUA_GetProperty scene property does not exist - ", prop_name.c_str());
		lua_pushnil(lua);
		return 1;
	}

	switch (prop.Type)
	{
	case PT_String:
		lua_pushstring(lua, (const char*)prop.p_data);
		break;
	case PT_Bool:
		lua_pushboolean(lua, prop.p_bool);
		break;
	case PT_Float:
		lua_pushnumber(lua, (double)prop.p_float);
		break;
	case PT_Int:
		lua_pushnumber(lua, (double)prop.p_int);
		break;
	case PT_Vec2:
	case PT_Vec3:
	case PT_Vec4:
		lua_pushvec(lua, prop.p_vec[0], prop.p_vec[1], prop.p_vec[2], prop.p_vec[3]);
		break;
	case PT_Resource:
		lua_pushlightuserdata(lua, prop.p_data);
		break;
	default:
		lua_pushnil(lua);
		break;
	}

    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_FindScene scene-name (string)
//
static int LUA_FindScene(lua_State *lua)
{
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: scene.find() not enough parameters, expected scene-name (string)");
		lua_pushnil(lua);
		return 1;
	}

	const char* name = NULL;

	// Get the scene name
	if (lua_isstring(lua, 1))
		name = lua_tostring(lua, 1);
	if (name == NULL)
	{
        CIwGameError::LogError("Warning: scene.find() Invalid name for Param0");
		lua_pushnil(lua);
		return 1;
	}

	// Locate scene
	CIwGameScene* scene = NULL;
	IIwGameXomlResource* parent = IW_GAME_GLOBAL_RESOURCES->getResourceManager()->getParent();
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Game_Hash)
		scene = ((CIwGame*)parent)->findScene(name);
	else
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = ((CIwGameScene*)parent)->getParent()->findScene(name);

	if (scene == NULL)
	{
        CIwGameError::LogError("Warning: scene.find() scene not found - ", name);
		lua_pushnil(lua);
		return 1;
	}

	// Return the scene
	lua_pushlightuserdata(lua, scene);

    return 1;
}

//
// LUA_CreateScene scene-name (string), width (number), height (number), canvas_fit (string), origin (string), physics (boolean), batching (boolean), script-engine (string)
//
static int LUA_CreateScene(lua_State *lua)
{
	if (lua_gettop(lua) < 8)
	{
        CIwGameError::LogError("Warning: scene.create() not enough parameters, scene-name (string), width (number), height (number), canvas_fit (string), origin (string), physics (boolean), batching (boolean), script-engine (string)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the scene name
	const char* name = NULL;
	if (lua_isstring(lua, 1))
		name = lua_tostring(lua, 1);
	if (name == NULL)
	{
        CIwGameError::LogError("Warning: scene.create() Invalid name for Param0, expected string");
		lua_pushnil(lua);
		return 1;
	}

	// Get the scene size
	int width, height;
	if (lua_isnumber(lua, 2))
		width = (int)lua_tonumber(lua, 2);
	else
	{
        CIwGameError::LogError("Warning: scene.create() Invalid width for Param1, expected number");
		lua_pushnil(lua);
		return 1;
	}
	if (lua_isnumber(lua, 3))
		height = (int)lua_tonumber(lua, 3);
	else
	{
        CIwGameError::LogError("Warning: scene.create() Invalid height for Param2, expected number");
		lua_pushnil(lua);
		return 1;
	}

	// Get the scene canvas fit
	const char* canvas_fit = NULL;
	if (lua_isstring(lua, 4))
		canvas_fit = lua_tostring(lua, 4);
	if (canvas_fit == NULL)
	{
        CIwGameError::LogError("Warning: scene.create() Invalid canvas_fit for Param3, expected string");
		lua_pushnil(lua);
		return 1;
	}

	// Get the scene canvas origin
	const char* canvas_org = NULL;
	if (lua_isstring(lua, 5))
		canvas_org = lua_tostring(lua, 5);
	if (canvas_org == NULL)
	{
        CIwGameError::LogError("Warning: scene.create() Invalid canvas_fit for Param4, expected string");
		lua_pushnil(lua);
		return 1;
	}

	unsigned int canvas_fit_hash = IW_GAME_HASH(canvas_fit);
	unsigned int canvas_org_hash = IW_GAME_HASH(canvas_org);
	CIwGameScene::eIwGameScene_CanvasFit fit = CIwGameScene::CF_None;
	if (canvas_fit_hash == IW_GAME_HASH("width"))
		fit = CIwGameScene::CF_Width;
	else
	if (canvas_fit_hash == IW_GAME_HASH("height"))
		fit = CIwGameScene::CF_Height;
	else
	if (canvas_fit_hash == IW_GAME_HASH("both"))
		fit = CIwGameScene::CF_Both;
	else
	if (canvas_fit_hash == IW_GAME_HASH("best"))
		fit = CIwGameScene::CF_Best;
	CIwGameScene::eIwGameScene_CanvasOrigin org = CIwGameScene::CO_Centre;
	if (canvas_org_hash == IW_GAME_HASH("top"))
		org = CIwGameScene::CO_Top;
	else
	if (canvas_org_hash == IW_GAME_HASH("left"))
		org = CIwGameScene::CO_Left;
	else
	if (canvas_org_hash == IW_GAME_HASH("topleft"))
		org = CIwGameScene::CO_TopLeft;

	// Get the scene physics / batching
	bool physics = false, batching = false;
	if (lua_isboolean(lua, 6))
		physics = lua_toboolean(lua, 6) != 0;
	else
	{
        CIwGameError::LogError("Warning: scene.create() Invalid physics for Param5, expected boolean");
		lua_pushnil(lua);
		return 1;
	}
	if (lua_isboolean(lua, 7))
		batching = lua_toboolean(lua, 7) != 0;
	else
	{
        CIwGameError::LogError("Warning: scene.create() Invalid batching for Param6, expected boolean");
		lua_pushnil(lua);
		return 1;
	}

	// Get the scene script engine
	const char* script_engine = NULL;
	if (lua_isstring(lua, 8))
		script_engine = lua_tostring(lua, 8);
	if (script_engine == NULL)
	{
        CIwGameError::LogError("Warning: scene.create() Invalid script-engine for Param7, expected string");
		lua_pushnil(lua);
		return 1;
	}

	CIwGameScene* scene = new CIwGameScene();
	scene->setName(name);
	scene->Init();
	scene->setBatching(batching);
	scene->setVirtualTransform(width, height, 0, fit, org);

	// Create script engine
	CIwGameString se = script_engine;
	if (script_engine != NULL)
		scene->setScriptEngine(se);

	// Add scene to game
	CIwGame* game = (CIwGame*)IW_GAME_GLOBAL_RESOURCES->getResourceManager()->getParent();
	game->addScene(scene, false);

	// Return the scene
	lua_pushlightuserdata(lua, scene);

    return 1;
}

//
// LUA_DestroyScene(scene (object))
//
static int LUA_DestroyScene(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: scene.destroy() not enough parameters, expected scene (object)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the main game object
	CIwGame* game = (CIwGame*)IW_GAME_GLOBAL_RESOURCES->getResourceManager()->getParent();

	// Get the object
	IIwGameXomlResource* object = NULL;
	if (lua_isuserdata(lua, 1))
		object = (IIwGameXomlResource*)lua_touserdata(lua, 1);
	if (object == NULL || object->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
	{
		CIwGameError::LogError("Warning: scene.destroy() Invalid object, expected a scene object in Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	game->removeScene((CIwGameScene*)object);

	lua_pushboolean(lua, true);
	return 1;
}

//
// LUA_setCurrentScene(scene-name (string)) - returns previous scene
//
static int LUA_setCurrentScene(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: scene.setCurrent() not enough parameters, expected scene (object)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the main game object
	CIwGame* game = (CIwGame*)IW_GAME_GLOBAL_RESOURCES->getResourceManager()->getParent();

	// Get the scene
	CIwGameScene* scene = NULL;
	if (lua_isuserdata(lua, 1))
		scene = (CIwGameScene*)lua_touserdata(lua, 1);
	if (scene == NULL || scene->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
	{
        CIwGameError::LogError("Warning: scene.setCurrent() Invalid scene for Param0");
		lua_pushnil(lua);
		return 1;
	}

	CIwGameScene* prev_scene = game->getCurrentScene();
	game->changeScene(scene);

	lua_pushlightuserdata(lua, prev_scene);

    return 1;
}

//
// LUA_getCurrentScene - returns current scene
//
static int LUA_getCurrentScene(lua_State *lua)
{
	int count = lua_gettop(lua);

	// Get the main game object
	CIwGame* game = (CIwGame*)IW_GAME_GLOBAL_RESOURCES->getResourceManager()->getParent();

	lua_pushlightuserdata(lua, game->getCurrentScene());

    return 1;
}

//
// LUA_SceneToScreen scene (object) x (number) y (number) - Returns vector
//
static int LUA_SceneToScreen(lua_State *lua)
{
	if (lua_gettop(lua) < 3)
	{
        CIwGameError::LogError("Warning: scene.toScreen() not enough parameters, expected scene (object) x (number) y (number)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the scene object
	CIwGameScene* scene = NULL;
	if (lua_isuserdata(lua, 1))
		scene = (CIwGameScene*)lua_touserdata(lua, 1);
	if (scene == NULL || scene->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
	{
        CIwGameError::LogError("Warning: scene.toScreen() Invalid scene object");
		lua_pushnil(lua);
		return 1;
	}

	// Get x coord
	float x = 0;
	if (lua_isnumber(lua, 2))
		x = (float)lua_tonumber(lua, 2);
	else
	{
        CIwGameError::LogError("Warning: scene.toScreen() expected a number for Param1");
		lua_pushnil(lua);
		return 1;
	}

	// Get y coord
	float y = 0;
	if (lua_isnumber(lua, 3))
		y = (float)lua_tonumber(lua, 3);
	else
	{
        CIwGameError::LogError("Warning: scene.toScreen() expected a number for Param2");
		lua_pushnil(lua);
		return 1;
	}

	CIwFVec2 screen = scene->VirtualToScreen(x, y);
	lua_pushvec(lua, screen.x, screen.y, 0, 0);
 
    return 1;
}

//
// LUA_ScreenToScene scene (object) x (number) y (number) include_camera (boolean) - Returns vector
//
static int LUA_ScreenToScene(lua_State *lua)
{
	if (lua_gettop(lua) < 3)
	{
        CIwGameError::LogError("Warning: scene.toScene() not enough parameters, expected scene (object) x (number) y (number) include_camera (boolean)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the scene object
	CIwGameScene* scene = NULL;
	if (lua_isuserdata(lua, 1))
		scene = (CIwGameScene*)lua_touserdata(lua, 1);
	if (scene == NULL || scene->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
	{
        CIwGameError::LogError("Warning: scene.toScene() Invalid scene object");
		lua_pushnil(lua);
		return 1;
	}

	// Get x coord
	float x = 0;
	if (lua_isnumber(lua, 2))
		x = (float)lua_tonumber(lua, 2);
	else
	{
        CIwGameError::LogError("Warning: scene.toScene() expected a number for Param1");
		lua_pushnil(lua);
		return 1;
	}

	// Get y coord
	float y = 0;
	if (lua_isnumber(lua, 3))
		y = (float)lua_tonumber(lua, 3);
	else
	{
        CIwGameError::LogError("Warning: scene.toScene() expected a number for Param2");
		lua_pushnil(lua);
		return 1;
	}

	// Get include camera parameter
	bool camera = false;
	if (lua_gettop(lua) > 3)
	{
		if (lua_isboolean(lua, 4))
			camera = lua_toboolean(lua, 4) != 0;
		else
		{
			CIwGameError::LogError("Warning: scene.toScene() expected a boolean for Param3");
			lua_pushnil(lua);
			return 1;
		}
	}

	CIwFVec2 s;
	if (camera)
		s = scene->ScreenToCamera(x, y);
	else
		s = scene->ScreenToVirtual(x, y);
	lua_pushvec(lua, s.x, s.y, 0, 0);
 
    return 1;
}


static const luaL_Reg g_scenelib[] =
{
	{ "set",		LUA_SetPropertyScene}, 
	{ "add",		LUA_AddPropertyScene},
	{ "get",		LUA_GetPropertyScene},
	{ "find",		LUA_FindScene},
	{ "create",		LUA_CreateScene},
	{ "destroy",	LUA_DestroyScene},
	{ "setCurrent", LUA_setCurrentScene},
	{ "getCurrent", LUA_getCurrentScene},
	{ "toScreen",	LUA_SceneToScreen},
	{ "toScene",	LUA_ScreenToScene},
	{NULL, NULL}
};

LUALIB_API int luaopen_scene(lua_State *L)
{
	luaL_register(L, "scene", g_scenelib);

	return 1;
}


