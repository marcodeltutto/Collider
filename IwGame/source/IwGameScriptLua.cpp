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
#include "lua/IwGameLuaAction.h"
#include "lua/IwGameLuaActor.h"
#include "lua/IwGameLuaBrush.h"
#include "lua/IwGameLuaCamera.h"
#include "lua/IwGameLuaFacebook.h"
#include "lua/IwGameLuaFont.h"
#include "lua/IwGameLuaHttp.h"
#include "lua/IwGameLuaImage.h"
#include "lua/IwGameLuaInput.h"
#include "lua/IwGameLuaMarket.h"
#include "lua/IwGameLuaMedia.h"
#include "lua/IwGameLuaPhysics.h"
#include "lua/IwGameLuaProgram.h"
#include "lua/IwGameLuaResource.h"
#include "lua/IwGameLuaScene.h"
#include "lua/IwGameLuaShape.h"
#include "lua/IwGameLuaTemplate.h"
#include "lua/IwGameLuaTimeline.h"
#include "lua/IwGameLuaVariable.h"
#include "IwGameActor.h"
#include "IwGameScene.h"



//
//
//
//
// CIwGameScriptEngineLua implementation
//
//
//
//

int CIwGameScriptEngineLua::Init(CIwGameScene* scene)
{
	// Open LUA
	Lua = lua_open();
	if (Lua == NULL)
		return -1;

	// Add LUA libraries
	luaL_openlibs(Lua);

	// Add functions
	luaopen_sys(Lua);
	luaopen_actions(Lua);
	luaopen_actor(Lua);
	luaopen_brush(Lua);
	luaopen_camera(Lua);
	luaopen_facebook(Lua);
	luaopen_font(Lua);
	luaopen_http(Lua);
	luaopen_image(Lua);
	luaopen_input(Lua);
	luaopen_market(Lua);
	luaopen_media(Lua);
	luaopen_physics(Lua);
	luaopen_program(Lua);
	luaopen_resource(Lua);
	luaopen_scene(Lua);
	luaopen_shape(Lua);
	luaopen_template(Lua);
	luaopen_timeline(Lua);
	luaopen_variable(Lua);

	// Set the scene
	Scene = scene;
	Lua->user_data = (void*)scene;

	return 1;
}

void CIwGameScriptEngineLua::Release()
{
	// Shut down the lua state
	if (Lua != NULL)
	{
//		lua_gc(Lua, LUA_GCCOLLECT, 0);
		lua_close(Lua);
		Lua = NULL;
	}
}

int CIwGameScriptEngineLua::RunScript(const char* script, int script_len)
{
	if (Lua == NULL)
	{
		CIwGameError::LogError("Error: No Lua script engine attached to the scene, please using ScriptEngine=\"Lua\" scene tag to attach one");
		return -1;
	}

	// Give each section of code a unique name
/*	static int t = 0;
	CIwGameString name = "scene";
	name += CIwGameString(t);
	t++;*/

	// Pass the string to lua to execute
	if (luaL_loadbuffer(Lua, script, script_len, "main") == 0)
	{
		if (lua_pcall(Lua, 0, 0, 0) != 0)
		{
			// Output the error
			CIwGameError::LogError("LUA ERROR: ------------------------------------------------------------------------------");
			CIwGameError::LogError("LUA ERROR: ", lua_tostring(Lua, -1));
			CIwGameError::LogError("LUA ERROR: ------------------------------------------------------------------------------");

			// Pop error message off the stack
			lua_pop(Lua, 1);
//			lua_close(Lua);
//			Lua = NULL;

			return -1;
		}
	}
	else
	{
		// Output the error
		CIwGameError::LogError("LUA ERROR: ------------------------------------------------------------------------------");
		CIwGameError::LogError("LUA ERROR: ", lua_tostring(Lua, -1));
		CIwGameError::LogError("LUA ERROR: ------------------------------------------------------------------------------");

		// Pop error message off the stack
		lua_pop(Lua, 1);
//		lua_close(Lua);
//		Lua = NULL;

		return -1;
	}

	return 1;
}

int CIwGameScriptEngineLua::CallFunction(IIwGameXomlResource* object, CIwGameString* function_name, CIwGameString* param1, CIwGameString* param2, CIwGameString* param3)
{
	if (Lua == NULL)
	{
		// Check global script engine
		IIwGameScriptEngine* se = IW_GAME_GLOBAL_RESOURCES->getScriptEngine();
		if (se != this && se != NULL)
			return se->CallFunction(object, function_name, param1, param2, param3);

		CIwGameError::LogError("Error: No Lua script engine attached to the scene, trying to call script function - ", function_name->c_str());
		return -1;
	}

	if (function_name == NULL || function_name->c_str() == NULL)
	{
		CIwGameError::LogError("Error: Invalid Lua script function name");
		return -1;
	}

	int count = lua_gettop(Lua);
	int ret = 0;

	int pcount = 1;
	lua_getglobal(Lua, function_name->c_str());				// Push function name that we want to call onto the stack
	if (lua_isnil(Lua, -1))
	{
		lua_pop(Lua, 1);

		// Check global script engine
		IIwGameScriptEngine* se = IW_GAME_GLOBAL_RESOURCES->getScriptEngine();
		if (se != this && se != NULL)
			return se->CallFunction(object, function_name, param1, param2, param3);

		CIwGameString error = "Error: function ";
		error += function_name->c_str();
		error += " does not exist";
		CIwGameError::LogError(error.c_str());
		return ret;
	}

	lua_pushlightuserdata(Lua, (void*)object);				// Push the reference object onto the stack
	if (param1 != NULL)
	{
		lua_pushstring(Lua, param1->c_str());
		pcount++;
	}
	if (param2 != NULL)
	{
		lua_pushstring(Lua, param2->c_str());
		pcount++;
	}
	if (param3 != NULL)
	{
		lua_pushstring(Lua, param3->c_str());
		pcount++;
	}
 
	if (lua_pcall(Lua, pcount, 1, 0) != 0)
	{
		CIwGameString error = "Error calling script - ";
		error += function_name->c_str();
		error += "(";
		if (param1 != NULL)
		{
			error += param1->c_str();
			error += ", ";
		}
		if (param2 != NULL)
		{
			error += param2->c_str();
			error += ", ";
		}
		if (param3 != NULL)
			error += param3->c_str();
		error += ") - ";
		error += lua_tostring(Lua, -1);
		CIwGameError::LogError(error.c_str());
	}
	if (lua_isnumber(Lua, -1))
	{
		// Get the result returned from the LUA function
		ret = (int)lua_tonumber(Lua, -1);
	}
	else
	{
		CIwGameString error = "Warning: function ";
		error += function_name->c_str();
		error += " should return a number";
		CIwGameError::LogError(error.c_str());
		ret = 0;
	}

	lua_pop(Lua, 1);

	return ret;
}

CIwGameXomlProperty CIwGameScriptEngineLua::getVariable(CIwGameString* var_name)
{
	CIwGameXomlProperty p;

	lua_getglobal(Lua, var_name->c_str());				// Push variable name onto stack
	if (lua_isnil(Lua, -1))
	{
		// Check global script engine
		IIwGameScriptEngine* se = IW_GAME_GLOBAL_RESOURCES->getScriptEngine();
		if (se != this && se != NULL)
		{
			lua_pop(Lua, 1);
			return se->getVariable(var_name);
		}

		// Output the error
		CIwGameError::LogError("LUA ERROR: ------------------------------------------------------------------------------");
		CIwGameError::LogError("LUA ERROR: ", lua_tostring(Lua, -1));
		CIwGameError::LogError("LUA ERROR: ------------------------------------------------------------------------------");

		lua_pop(Lua, 1);
	}
	else
	{
		int type = lua_type(Lua, -1);

		switch (type)
		{
		case LUA_TBOOLEAN:
			p.Type = PT_Bool;
			p.p_bool = lua_toboolean(Lua, -1) != 0;
			break;
		case LUA_TNUMBER:
			p.Type = PT_Float;
			p.p_float = (float)lua_tonumber(Lua, -1);
			break;
		case LUA_TVEC:
			{
				p.Type = PT_Vec4;
				const float* vec = lua_tovec(Lua, -1);
				p.p_vec[0] = *vec;
				p.p_vec[1] = *(vec + 1);
				p.p_vec[2] = *(vec + 2);
				p.p_vec[3] = *(vec + 3);
			}
			break;
		case LUA_TSTRING:
			p.Type = PT_String;
			p.p_data = (void*)lua_tostring(Lua, -1);
			break;
/*		case LUA_TLIGHTUSERDATA:
			p.Type = PT_Resource;
			p.p_data = (void*)lua_touserdata(Lua, -1);
			break;
		case LUA_TFUNCTION:
			p.Type = PT_Resource;
			p.p_data = (void*)lua_topointer(Lua, -1);
			break;
		case LUA_TTABLE:
			p.Type = PT_Resource;
			p.p_data = (void*)lua_topointer(Lua, -1);
			break;*/
		default:
			CIwGameError::LogError("Warning: Lua variable type not supported - ", CIwGameString(type).c_str());
		}
	}

	lua_pop(Lua, 1);

	return p;
}

