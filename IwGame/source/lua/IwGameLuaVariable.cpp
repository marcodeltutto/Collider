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
// LUA_FindVariable variable-name (string), container scene (object, optional)
//
static int LUA_FindVariable(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: variable.find() not enough parameters, expected variable-name (string), container scene (object, optional)");
		lua_pushnil(lua);
		return 1;
	}

	IIwGameXomlResource* scene = NULL;
	const char* name = NULL;

	// Get the variable name
	if (lua_isstring(lua, 1))
		name = lua_tostring(lua, 1);
	if (name == NULL)
	{
        CIwGameError::LogError("Warning: variable.find() Invalid name for Param0");
		lua_pushnil(lua);
		return 1;
	}

	// Get the container scene
	if (count > 1)
	{
		if (lua_isuserdata(lua, 2))
			scene = (IIwGameXomlResource*)lua_touserdata(lua, 2);
		if (scene != NULL && scene->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
		{
			CIwGameError::LogError("Warning: variable.find() expected scene for Param1");
			lua_pushnil(lua);
			return 1;
		}
	}
	else
		scene = (CIwGameScene*)lua->user_data;

	// Locate variable
	CIwGameXomlVariable* var = CIwGameXomlVariableManager::FindVariable(name, scene);
	if (var == NULL)
	{
        CIwGameError::LogError("Warning: variable.find() variable not found - ", name);
		lua_pushnil(lua);
		return 1;
	}

	// Return the variable
	lua_pushlightuserdata(lua, var);

    return 1;
}

//
// LUA_GetVariableSize variable (object)
//
static int LUA_GetVariableSize(lua_State *lua)
{
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: variable.size() not enough parameters, expected variable (object)");
		lua_pushnil(lua);
		return 1;
	}

	// Get variable object
	CIwGameXomlVariable* var = NULL;
	if (lua_isuserdata(lua, 1))
		var = (CIwGameXomlVariable*)lua_touserdata(lua, 1);
	if (var == NULL)
	{
        CIwGameError::LogError("Error: variable.size() Invalid variable for Param0");
		lua_pushnil(lua);
		return 1;
	}

	if (!var->isArray())
		lua_pushinteger(lua, 1);
	else
		lua_pushinteger(lua, ((CIwGameXomlVariableArray*)var)->getSize());

    return 1;
}

//
// LUA_VariableIsArray variable (object)
//
static int LUA_VariableIsArray(lua_State *lua)
{
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: variable.array() not enough parameters, expected variable (object)");
		lua_pushnil(lua);
		return 1;
	}

	// Get variable object
	CIwGameXomlVariable* var = NULL;
	if (lua_isuserdata(lua, 1))
		var = (CIwGameXomlVariable*)lua_touserdata(lua, 1);
	if (var == NULL)
	{
        CIwGameError::LogError("Error: variable.array() Invalid variable for Param0");
		lua_pushnil(lua);
		return 1;
	}

	lua_pushboolean(lua, var->isArray());

    return 1;
}

//
// LUA_GetValue variable (object), index (number, optional)
//
static int LUA_GetValue(lua_State *lua)
{
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: variable.get() not enough parameters, expected variable (object), index (number, optional)");
		lua_pushnil(lua);
		return 1;
	}

	// Get variable object
	CIwGameXomlVariable* var = NULL;
	if (lua_isuserdata(lua, 1))
		var = (CIwGameXomlVariable*)lua_touserdata(lua, 1);
	if (var == NULL)
	{
        CIwGameError::LogError("Error: variable.get() Invalid variable for Param0");
		lua_pushnil(lua);
		return 1;
	}

	// Get variable index
	int index = 0;
	if (lua_gettop(lua) > 1)
	{
		if (lua_isnumber(lua, 2))
			index = (int)lua_tonumber(lua, 2);
		else
		{
			CIwGameError::LogError("Error: variable.get() Invalid value for Param1, expected number");
			lua_pushnil(lua);
			return 1;
		}
	}

	if (var->isArray())
	{
		if (index < 0 || index >= ((CIwGameXomlVariableArray*)var)->getSize())
		{
			CIwGameError::LogError("Error: variable.get() index is out of range");
			lua_pushnil(lua);
			return 1;
		}
		var = ((CIwGameXomlVariableArray*)var)->getElement(index);
	}

	switch (var->Type)
	{
	case VT_String:
	case VT_XML:
		lua_pushstring(lua, (const char*)var->getValue().c_str());
		break;
	case VT_Bool:
		lua_pushboolean(lua, ((CIwGameXomlVariableBool*)var)->NativeValue);
		break;
	case VT_Float:
		lua_pushnumber(lua, (double)((CIwGameXomlVariableFloat*)var)->NativeValue);
		break;
	case VT_Int:
		lua_pushnumber(lua, (double)((CIwGameXomlVariableInt*)var)->NativeValue);
		break;
	case VT_Vec2:
		{
			CIwGameXomlVariableVec2* v = (CIwGameXomlVariableVec2*)var;
			lua_pushvec(lua, v->NativeValue.x, v->NativeValue.y, 0, 0);
		}
		break;
	case VT_Vec3:
		{
			CIwGameXomlVariableVec3* v = (CIwGameXomlVariableVec3*)var;
			lua_pushvec(lua, v->NativeValue.x, v->NativeValue.y, v->NativeValue.y, 0);
		}
		break;
	case VT_Vec4:
		{
			CIwGameXomlVariableVec4* v = (CIwGameXomlVariableVec4*)var;
			lua_pushvec(lua, v->NativeValue.x, v->NativeValue.y, v->NativeValue.y, v->NativeValue.w);
		}
		break;
	case VT_Condition:
		lua_pushboolean(lua, ((CIwGameXomlVariableCondition*)var)->isTrue());
		break;
	default:
		lua_pushnil(lua);
		break;
	}

    return 1;
}

//
// LUA_SetVariable variable (variable), value (any), index (number)
//
static int LUA_SetVariable(lua_State *lua)
{
	if (lua_gettop(lua) < 2)
	{
        CIwGameError::LogError("Warning: variable.set() not enough parameters, expected variable (variable), value (any), index (number)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the variable
	CIwGameXomlVariable* var = NULL;
	if (lua_isuserdata(lua, 1))
		var = (CIwGameXomlVariable*)lua_touserdata(lua, 1);
	if (var == NULL)
	{
        CIwGameError::LogError("Warning: variable.set() Invalid variable for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the value
	CIwGameString value;
	if (!ValueToString(lua, 2, value))
	{
        CIwGameError::LogError("Warning: variable.set() value (param1) must be a string, number, boolean or vec - ", var->Name.c_str());
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the array (index if supplied)
	int index = -1;
	if (lua_gettop(lua) > 2)
	{
		if (lua_isnumber(lua, 3))
			index = (int)lua_tonumber(lua, 3);
	}

	// Set the value
	if (var->isArray())
	{
		if (index < 0 || index >= ((CIwGameXomlVariableArray*)var)->getSize())
		{
			CIwGameError::LogError("Error: variable.set() index is out of range");
			lua_pushboolean(lua, false);
			return 1;
		}
		var = ((CIwGameXomlVariableArray*)var)->getElement(index);
	}

	var->setValue(value.c_str());

	lua_pushboolean(lua, true);

    return 1;
}

//
// LUA_AddVariable variable (variable), value (any), index (number)
//
static int LUA_AddVariable(lua_State *lua)
{
	if (lua_gettop(lua) < 2)
	{
        CIwGameError::LogError("Warning: variable.add() not enough parameters, expected variable (variable), value (any), index (number)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the variable
	CIwGameXomlVariable* var = NULL;
	if (lua_isuserdata(lua, 1))
		var = (CIwGameXomlVariable*)lua_touserdata(lua, 1);
	if (var == NULL)
	{
        CIwGameError::LogError("Warning: variable.add() Invalid variable for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the value
	CIwGameString value;
	if (!ValueToString(lua, 2, value))
	{
        CIwGameError::LogError("Warning: variable.add() value (param1) must be a string, number, boolean or vec - ", var->Name.c_str());
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the array (index if supplied)
	int index = -1;
	if (lua_gettop(lua) > 2)
	{
		if (lua_isnumber(lua, 3))
			index = (int)lua_tonumber(lua, 3);
	}

	// Set the value
	if (var->isArray())
	{
		if (index < 0 || index >= ((CIwGameXomlVariableArray*)var)->getSize())
		{
			CIwGameError::LogError("Error: variable.set() index is out of range");
			lua_pushboolean(lua, false);
			return 1;
		}
		var = ((CIwGameXomlVariableArray*)var)->getElement(index);
	}

	var->addValue(value.c_str());

	lua_pushboolean(lua, true);

    return 1;
}

//
// LUA_AppendArray variable (variable), value (any), count (number)
//
static int LUA_AppendArray(lua_State *lua)
{
	if (lua_gettop(lua) < 2)
	{
        CIwGameError::LogError("Warning: variable.append() not enough parameters, expected variable (variable), value (any), count (number, optional)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the variable
	CIwGameXomlVariable* var = NULL;
	if (lua_isuserdata(lua, 1))
		var = (CIwGameXomlVariable*)lua_touserdata(lua, 1);
	if (var == NULL || !var->isArray())
	{
        CIwGameError::LogError("Warning: variable.append() Invalid variable for Param0, requires a XOML array variable");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the value
	CIwGameString value;
	if (!ValueToString(lua, 2, value))
	{
        CIwGameError::LogError("Warning: variable.append() value (param1) must be a string, number, boolean or vec - ", var->Name.c_str());
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the array (index if supplied)
	int count = 1;
	if (lua_gettop(lua) > 2)
	{
		if (lua_isnumber(lua, 3))
			count = (int)lua_tonumber(lua, 3);
		else
		{
			CIwGameError::LogError("Warning: variable.append() value (param2) must be a number - ", var->Name.c_str());
			lua_pushboolean(lua, false);
			return 1;
		}
	}

	for (int t = 0; t < count; t++)
	{
		// Set the value
		CIwGameXomlVariable* var2 = NULL;
		switch (var->Type)
		{
		case VT_ArrayString:
			var2 = new CIwGameXomlVariable();
			break;
		case VT_ArrayBool:
			var2 = new CIwGameXomlVariableBool();
			break;
		case VT_ArrayFloat:
			var2 = new CIwGameXomlVariableFloat();
			break;
		case VT_ArrayInt:
			var2 = new CIwGameXomlVariableInt();
			break;
		}
		var2->setValue(value.c_str());

		// Append the variable
		((CIwGameXomlVariableArray*)var)->addElement(var2);
	}

	lua_pushboolean(lua, true);

    return 1;
}

static const luaL_Reg g_variablelib[] =
{
	{ "find",			LUA_FindVariable}, 
	{ "size",			LUA_GetVariableSize}, 
	{ "array",			LUA_VariableIsArray}, 
	{ "get",			LUA_GetValue}, 
	{ "set",			LUA_SetVariable}, 
	{ "add",			LUA_AddVariable}, 
	{ "append",			LUA_AppendArray}, 
	{NULL, NULL}
};

LUALIB_API int luaopen_variable(lua_State *L)
{
	luaL_register(L, "variable", g_variablelib);

	return 1;
}

