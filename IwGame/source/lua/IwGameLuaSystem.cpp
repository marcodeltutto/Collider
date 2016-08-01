// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. The only restriction to its usage is that this header must remain intact and visible in all IwGame engine files.
// If you use this engine in your product, whilst it is not mandatory, a mention of the IwGame engine would be nice or a quick email to let us know where it is being used.
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
// LUA_SetProperty scene or actor (object), property (string), value (string)
//
static int LUA_SetProperty(lua_State *lua)
{
	if (lua_gettop(lua) < 3)
	{
        CIwGameError::LogError("Warning: LUA_SetProperty not enough parameters, expected scene or actor (object), property (string), value (string)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	IIwGameXomlResource* object = NULL;
	if (lua_isuserdata(lua, 1))
		object = (IIwGameXomlResource*)lua_touserdata(lua, 1);
	if (object == NULL)
	{
        CIwGameError::LogError("Warning: LUA_SetProperty Invalid target object");
		lua_pushboolean(lua, false);
		return 1;
	}

	CIwGameString prop_name, value;
    if (lua_isstring(lua, 2))
        prop_name = lua_tostring(lua, 2);
    else
	{
        CIwGameError::LogError("Warning: LUA_SetProperty property must be a property name - object - ", object->getName().c_str());
		lua_pushboolean(lua, false);
		return 1;
	}

	if (!ValueToString(lua, 3, value))
	{
        CIwGameError::LogError("Warning: LUA_SetProperty value must be a string, number, boolean or vec - object - ", object->getName().c_str());
		lua_pushboolean(lua, false);
		return 1;
	}

	if (object->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		((CIwGameActor*)object)->setProperty(prop_name.getHash(), value, false);
	else
	if (object->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		((CIwGameScene*)object)->setProperty(prop_name.getHash(), value, false);

     lua_pushboolean(lua, true);
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_AddProperty scene or actor (object), property (string), value (string)
//
static int LUA_AddProperty(lua_State *lua)
{
	if (lua_gettop(lua) < 3)
	{
        CIwGameError::LogError("Warning: LUA_AddProperty not enough parameters, expected scene or actor (object), property (string), value (string)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	IIwGameXomlResource* object = NULL;
	if (lua_isuserdata(lua, 1))
		object = (IIwGameXomlResource*)lua_touserdata(lua, 1);
	if (object == NULL)
	{
        CIwGameError::LogError("Warning: LUA_AddProperty Invalid target object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	CIwGameString prop_name, value;
    if (lua_isstring(lua, 2))
        prop_name = lua_tostring(lua, 2);
    else
	{
        CIwGameError::LogError("Warning: LUA_AddProperty property must be a property name (Param1) - object - ", object->getName().c_str());
		lua_pushboolean(lua, false);
		return 1;
	}

	if (!ValueToString(lua, 3, value))
	{
        CIwGameError::LogError("Warning: LUA_AddProperty value must be a string, number, boolean or vec  - object - ", object->getName().c_str());
		lua_pushboolean(lua, false);
		return 1;
	}

	if (object->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		((CIwGameActor*)object)->setProperty(prop_name.getHash(), value, true);
	else
	if (object->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		((CIwGameScene*)object)->setProperty(prop_name.getHash(), value, true);

    lua_pushboolean(lua, true);
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_GetProperty scene or actor (object), property (string)
//
static int LUA_GetProperty(lua_State *lua)
{
	if (lua_gettop(lua) < 2)
	{
        CIwGameError::LogError("Warning: LUA_GetProperty not enough parameters, expected scene or actor (object), property (string)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the target object
	IIwGameXomlResource* object = NULL;
	if (lua_isuserdata(lua, 1))
		object = (IIwGameXomlResource*)lua_touserdata(lua, 1);
	if (object == NULL)
	{
        CIwGameError::LogError("Warning: LUA_GetProperty Invalid target object for Param0");
		lua_pushnil(lua);
		return 1;
	}

	CIwGameString prop_name, value;
    if (lua_isstring(lua, 2))
        prop_name = lua_tostring(lua, 2);
    else
	{
        CIwGameError::LogError("Warning: LUA_GetProperty property must be a property name (Param1) - object - ", object->getName().c_str());
		lua_pushnil(lua);
		return 1;
	}

	CIwGameXomlProperty prop;
	if (object->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
	{
		if (!((CIwGameActor*)object)->getProperty(prop_name.getHash(), prop))
		{
			CIwGameError::LogError("Warning: LUA_GetProperty actor property does not exist - ", prop_name.c_str());
			lua_pushnil(lua);
			return 1;
		}
	}
	else
	if (object->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
	{
		if (!((CIwGameScene*)object)->getProperty(prop_name.getHash(), prop))
		{
			CIwGameError::LogError("Warning: LUA_GetProperty scene property does not exist - ", prop_name.c_str());
			lua_pushnil(lua);
			return 1;
		}
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
	}

    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_GetResource resource-name (string), resurce_type (string), container scene (object, optional)
//
static int LUA_GetResource(lua_State *lua)
{
	int count = lua_gettop(lua);

	if (count < 2)
	{
        CIwGameError::LogError("Warning: LUA_GetResource not enough parameters, expected resource-name (string), resource_type (string), container scene (object, optional)");
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
        CIwGameError::LogError("Warning: LUA_GetResource Invalid name for Param0");
		lua_pushnil(lua);
		return 1;
	}

	// Get the resource type
	if (lua_isstring(lua, 2))
		type = lua_tostring(lua, 2);
	if (type == NULL)
	{
        CIwGameError::LogError("Warning: LUA_GetResource Invalid type for Param1");
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
			CIwGameError::LogError("Warning: LUA_GetResource expected scene for Param1");
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
        CIwGameError::LogError("Warning: LUA_GetResource resource not found - ", name);
		lua_pushnil(lua);
		return 1;
	}

	// Return the resource
	lua_pushlightuserdata(lua, resource);

    return 1;
}

//
// LUA_GetVariable variable-name (string), container scene (object, optional)
//
static int LUA_GetVariable(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: LUA_GetVariable not enough parameters, expected variable-name (string), container scene (object, optional)");
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
        CIwGameError::LogError("Warning: LUA_GetVariable Invalid name for Param0");
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
			CIwGameError::LogError("Warning: LUA_GetVariable expected scene for Param1");
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
        CIwGameError::LogError("Warning: LUA_GetVariable variable not found - ", name);
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
        CIwGameError::LogError("Warning: LUA_GetVariableSize not enough parameters, expected variable (object)");
		lua_pushnil(lua);
		return 1;
	}

	// Get variable object
	CIwGameXomlVariable* var = NULL;
	if (lua_isuserdata(lua, 1))
		var = (CIwGameXomlVariable*)lua_touserdata(lua, 1);
	if (var == NULL)
	{
        CIwGameError::LogError("Error: LUA_GetVariableSize Invalid variable for Param0");
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
// LUA_GetValue variable (object), index (number, optional)
//
static int LUA_GetValue(lua_State *lua)
{
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: LUA_GetValue not enough parameters, expected variable (object), index (number, optional)");
		lua_pushnil(lua);
		return 1;
	}

	// Get variable object
	CIwGameXomlVariable* var = NULL;
	if (lua_isuserdata(lua, 1))
		var = (CIwGameXomlVariable*)lua_touserdata(lua, 1);
	if (var == NULL)
	{
        CIwGameError::LogError("Error: LUA_GetValue Invalid variable for Param0");
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
			CIwGameError::LogError("Error: LUA_GetValue Invalid value for Param1, expected number");
			lua_pushnil(lua);
			return 1;
		}
	}

	if (var->isArray())
	{
		if (index < 0 || index >= ((CIwGameXomlVariableArray*)var)->getSize())
		{
			CIwGameError::LogError("Error: LUA_GetValue index is out of range");
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
        CIwGameError::LogError("Warning: LUA_SetVariable not enough parameters, expected variable (variable), value (any), index (number)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the variable
	CIwGameXomlVariable* var = NULL;
	if (lua_isuserdata(lua, 1))
		var = (CIwGameXomlVariable*)lua_touserdata(lua, 1);
	if (var == NULL)
	{
        CIwGameError::LogError("Warning: LUA_GetVariable Invalid variable for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the value
	CIwGameString value;
	if (!ValueToString(lua, 2, value))
	{
        CIwGameError::LogError("Warning: LUA_SetVariable value (param1) must be a string, number, boolean or vec - object - ", var->Name.c_str());
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
			CIwGameError::LogError("Error: LUA_SetVariable index is out of range");
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
// LUA_GetActions actions-name (string), scene or actor (object, optional)
//
static int LUA_GetActions(lua_State *lua)
{
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: LUA_GetActions not enough parameters, expected actions-name (string), scene or actor (object, optional)");
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
        CIwGameError::LogError("Warning: LUA_GetActions Invalid name for Param0");
		lua_pushnil(lua);
		return 1;
	}

	// Get the container object
	if (lua_isuserdata(lua, 2))
		container = (IIwGameXomlResource*)lua_touserdata(lua, 2);
	if (container != NULL && (container->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && container->getClassTypeHash() != CIwGameXomlNames::Actor_Hash))
	{
        CIwGameError::LogError("Warning: LUA_GetActions expected scene or actor for Param1");
		lua_pushnil(lua);
		return 1;
	}

	// Locate actions
	CIwGameActions* actions = CIwGameActionsManager::FindActions(name, container);
	if (actions == NULL)
	{
        CIwGameError::LogError("Warning: LUA_GetActions actions not found - ", name);
		lua_pushnil(lua);
		return 1;
	}

	// Return the actions
	lua_pushlightuserdata(lua, actions);

    return 1;
}

//
// LUA_GetTimeline timeline-name (string), scene or actor (object, optional)
//
static int LUA_GetTimeline(lua_State *lua)
{
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: LUA_GetTimeline not enough parameters, expected timeline-name (string), scene or actor (object, optional)");
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
        CIwGameError::LogError("Warning: LUA_GetTimeline Invalid name for Param0");
		lua_pushnil(lua);
		return 1;
	}

	// Get the container object
	if (lua_isuserdata(lua, 2))
		container = (IIwGameXomlResource*)lua_touserdata(lua, 2);
	if (container != NULL && (container->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && container->getClassTypeHash() != CIwGameXomlNames::Actor_Hash))
	{
        CIwGameError::LogError("Warning: LUA_GetTimeline expected scene or actor for Param1");
		lua_pushnil(lua);
		return 1;
	}

	// Locate timeline
	CIwGameAnimTimeline* timeline = CIwGameAnimTimelinesManager::FindTimeline(name, container);
	if (timeline == NULL)
	{
        CIwGameError::LogError("Warning: LUA_GetTimeline timeline not found - ", name);
		lua_pushnil(lua);
		return 1;
	}

	// Return the timeline
	lua_pushlightuserdata(lua, timeline);

    return 1;
}

//
// LUA_GetProgram program-name (string), scene (object, optional)
//
static int LUA_GetProgram(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: LUA_GetProgram not enough parameters, expected program-name (string), scene (object, optional)");
		lua_pushnil(lua);
		return 1;
	}

	IIwGameXomlResource* container = NULL;
	const char* name = NULL;

	// Get the programs name
	if (lua_isstring(lua, 1))
		name = lua_tostring(lua, 1);
	if (name == NULL)
	{
        CIwGameError::LogError("Warning: LUA_GetProgram Invalid name for Param0");
		lua_pushnil(lua);
		return 1;
	}

	// Get the container scene
	if (count > 1)
	{
		if (lua_isuserdata(lua, 2))
			container = (IIwGameXomlResource*)lua_touserdata(lua, 2);
		if (container != NULL && container->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
		{
			CIwGameError::LogError("Warning: LUA_GetProgram expected scene for Param1");
			lua_pushnil(lua);
			return 1;
		}
	}
	else
		container = (CIwGameScene*)lua->user_data;

	// Locate program
	CIwGameProgram* program = CIwGameProgramManager::FindProgram(name, container);
	if (program == NULL)
	{
        CIwGameError::LogError("Warning: LUA_GetProgram progran not found - ", name);
		lua_pushnil(lua);
		return 1;
	}

	// Return the program
	lua_pushlightuserdata(lua, program);

    return 1;
}

//
// LUA_GetScene scene-name (string)
//
static int LUA_GetScene(lua_State *lua)
{
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: LUA_GetScene not enough parameters, expected scene-name (string)");
		lua_pushnil(lua);
		return 1;
	}

	const char* name = NULL;

	// Get the scene name
	if (lua_isstring(lua, 1))
		name = lua_tostring(lua, 1);
	if (name == NULL)
	{
        CIwGameError::LogError("Warning: LUA_GetScene Invalid name for Param0");
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
        CIwGameError::LogError("Warning: LUA_GetScene scene not found - ", name);
		lua_pushnil(lua);
		return 1;
	}

	// Return the scene
	lua_pushlightuserdata(lua, scene);

    return 1;
}

//
// LUA_GetActor actor-name (string), scene (object, optional)
//
static int LUA_GetActor(lua_State *lua)
{
	int count = lua_gettop(lua);

	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: LUA_GetActor not enough parameters, expected actor-name (string), scene (object)");
		lua_pushnil(lua);
		return 1;
	}

	IIwGameXomlResource* container = NULL;
	const char* name = NULL;

	// Get the actor name
	if (lua_isstring(lua, 1))
		name = lua_tostring(lua, 1);
	if (name == NULL)
	{
        CIwGameError::LogError("Warning: LUA_GetActor Invalid name for Param0");
		lua_pushnil(lua);
		return 1;
	}

	// Get the container scene
	if (count > 1)
	{
		if (lua_isuserdata(lua, 2))
			container = (IIwGameXomlResource*)lua_touserdata(lua, 2);
		if (container != NULL && container->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
		{
			CIwGameError::LogError("Warning: LUA_GetActor expected scene for Param1");
			lua_pushnil(lua);
			return 1;
		}
	}
	else
		container = (CIwGameScene*)lua->user_data;

	if (container == NULL)
	{
		CIwGameError::LogError("Warning: LUA_GetActor invalid container scene");
		lua_pushnil(lua);
		return 1;
	}

	// Locate actor
	CIwGameActor* actor = ((CIwGameScene*)container)->findActor(name);
	if (actor == NULL)
	{
        CIwGameError::LogError("Warning: LUA_GetActor actor not found - ", name);
		lua_pushnil(lua);
		return 1;
	}

	// Return the actor
	lua_pushlightuserdata(lua, actor);

    return 1;
}

//
// LUA_CreateFromTemplate template (object), container (object), templates parameters (table)
//
static int LUA_CreateFromTemplate(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 2)
	{
        CIwGameError::LogError("Warning: LUA_CreateFromTemplate not enough parameters, expected actor-name (string), scene (object)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the template
	CIwGameTemplate* temp = NULL;
	if (lua_isuserdata(lua, 1))
		temp = (CIwGameTemplate*)lua_touserdata(lua, 1);
	if (temp == NULL || (temp != NULL && temp->getClassTypeHash() != CIwGameXomlNames::Template_Hash))
	{
        CIwGameError::LogError("Warning: LUA_CreateFromTemplate Invalid template for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the scene
	IIwGameXomlResource* container = NULL;
	if (lua_isuserdata(lua, 2))
		container = (CIwGameScene*)lua_touserdata(lua, 2);
	if (container == NULL)
	{
        CIwGameError::LogError("Warning: LUA_CreateFromTemplate Invalid container for Param1");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Create a set of XML attributes that will replace the template parameters
	CIwGameXmlNode* replacements = new CIwGameXmlNode();
	replacements->Managed = false;

	// Table is in the stack at index 't'
	lua_pushnil(lua);  // First key
	while (lua_next(lua, 3) != 0)
	{
		// Get param name and value
		const char* name = lua_tostring(lua, -2);
		const char* value = lua_tostring(lua, -1);

		if (name != NULL && value != NULL)
		{
			// Set base template paramater
			CIwGameXmlAttribute* index_attrib = new CIwGameXmlAttribute();
			index_attrib->Managed = false;
			index_attrib->setName(name);
			index_attrib->setValue(value);
			replacements->AddAttribute(index_attrib);
		}

		// Removes value but keeps key for the next iteration
		lua_pop(lua, 1);
	}

	if (!temp->Instantiate(container, replacements))
	{
#if defined(_DEBUG)
		CIwGameError::LogError("Error: Could not instantiate from items template - ", temp->getName().c_str());
#endif	// _DEBUG
		delete replacements;
		lua_pushboolean(lua, false);
		return 1;
	}
	
	delete replacements;

	lua_pushboolean(lua, true);

    return 1;
}

//
// LUA_isType(type-name)
//
static int LUA_isType(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: LUA_isType not enough parameters, expected type-name");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the resource
	IIwGameXomlResource* resource = NULL;
	if (lua_isuserdata(lua, 1))
		resource = (CIwGameScene*)lua_touserdata(lua, 1);
	else
	{
        CIwGameError::LogError("Warning: LUA_isTypeOf Invalid parameter, expected object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the type of actor to create
	const char* type = NULL;
	if (lua_isstring(lua, 2))
		type = lua_tostring(lua, 2);
	else
	{
        CIwGameError::LogError("Warning: LUA_isTypeOf Invalid parameter, expected type-name for Param1");
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
        CIwGameError::LogError("Warning: LUA_isTypeOf not enough parameters, expected type-name");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the resource
	IIwGameXomlResource* resource = NULL;
	if (lua_isuserdata(lua, 1))
		resource = (CIwGameScene*)lua_touserdata(lua, 1);
	else
	{
        CIwGameError::LogError("Warning: LUA_isTypeOf Invalid parameter, expected object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the type of actor to create
	const char* type = NULL;
	if (lua_isstring(lua, 2))
		type = lua_tostring(lua, 2);
	else
	{
        CIwGameError::LogError("Warning: LUA_isTypeOf Invalid parameter, expected type-name for Param1");
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
// LUA_CreateActor(container (object), parameters)
//
static int LUA_CreateActor(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 2)
	{
        CIwGameError::LogError("Warning: LUA_CreateActor not enough parameters, expected type (string) parent (scene or actor), ......");
		lua_pushnil(lua);
		return 1;
	}

	// Get the type of actor to create
	const char* type = NULL;
	if (lua_isstring(lua, 1))
		type = lua_tostring(lua, 1);
	else
	{
        CIwGameError::LogError("Warning: LUA_CreateActor Invalid actor type for Param0");
		lua_pushnil(lua);
		return 1;
	}

	// Get the parent container
	IIwGameXomlResource* container = NULL;
	if (lua_isuserdata(lua, 2))
		container = (CIwGameScene*)lua_touserdata(lua, 2);
	if (container == NULL || (container->getClassTypeHash() != CIwGameXomlNames::Actor_Hash && container->getClassTypeHash() != CIwGameXomlNames::Scene_Hash))
	{
        CIwGameError::LogError("Warning: LUA_CreateActor Invalid container for Param1");
		lua_pushnil(lua);
		return 1;
	}

	// Get actor / scene
	CIwGameActor* actor = NULL;
	CIwGameScene* scene = NULL;
	if (container->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)container;
	if (container->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
	{
		actor = (CIwGameActor*)container;
		scene = actor->getScene();
	}

	unsigned int type_hash = IW_GAME_HASH(type);
	if (type_hash == CIwGameXomlNames::Icon_Hash)
	{
		CIwGameUIIcon* new_actor = new CIwGameUIIcon();
		scene->addActor(new_actor);

		// Get the brush
		IIwGameBrush *brush = NULL;
		if (lua_isuserdata(lua, 3))
			brush = (IIwGameBrush*)lua_touserdata(lua, 3);
		if (brush == NULL || brush->getClassTypeHash() != CIwGameXomlNames::Brush_Hash)
		{
			CIwGameError::LogError("Warning: LUA_CreateActor Invalid brush in Param2");
			lua_pushnil(lua);
			return 1;
		}

		// Get the width and height
		int width = -100, height = -100;
		if (lua_isnumber(lua, 4))
			width = (int)lua_tonumber(lua, 4);
		else
		{
			CIwGameError::LogError("Warning: LUA_CreateActor Width missing in Param3");
			lua_pushnil(lua);
			return 1;
		}
		if (lua_isnumber(lua, 5))
			height = (int)lua_tonumber(lua, 5);
		else
		{
			CIwGameError::LogError("Warning: LUA_CreateActor Height missing in Param4");
			lua_pushnil(lua);
			return 1;
		}
		new_actor->Init(brush, width, height);
		if (actor != NULL)
		{
			new_actor->setLinkedTo(actor);
			new_actor->setDepth(0);
		}

		lua_pushlightuserdata(lua, new_actor);
	}
	else
	if (type_hash == CIwGameXomlNames::ActorText_Hash)
	{
		CIwGameActorText* new_actor = new CIwGameActorText();
		scene->addActor(new_actor);

		// Get the font
		CIwGameFont *font = NULL;
		if (lua_isuserdata(lua, 3))
			font = (CIwGameFont*)lua_touserdata(lua, 3);
		if (font == NULL || font->getClassTypeHash() != CIwGameXomlNames::Font_Hash)
		{
			CIwGameError::LogError("Warning: LUA_CreateActor Invalid font in Param2");
			lua_pushnil(lua);
			return 1;
		}

		// Get the text
		const char* text = NULL;
		if (lua_isstring(lua, 4))
			text = lua_tostring(lua, 4);
		else
		{
			CIwGameError::LogError("Warning: LUA_CreateActor Text missing in Param3");
			lua_pushnil(lua);
			return 1;
		}

		CIwRect			rect = CIwRect(0, 0, -100, -100);
		CIwGameString t = text;
		new_actor->Init(font, rect, t, 0);
		if (actor != NULL)
		{
			new_actor->setLinkedTo(actor);
			new_actor->setDepth(0);
		}

		lua_pushlightuserdata(lua, new_actor);
	}
	else
	if (type_hash == CIwGameXomlNames::Label_Hash)
	{
		CIwGameUILabel* new_actor = new CIwGameUILabel();
		scene->addActor(new_actor);

		// Get the font
		CIwGameFont *font = NULL;
		if (lua_isuserdata(lua, 3))
			font = (CIwGameFont*)lua_touserdata(lua, 3);
		if (font == NULL || font->getClassTypeHash() != CIwGameXomlNames::Font_Hash)
		{
			CIwGameError::LogError("Warning: LUA_CreateActor Invalid font in Param2");
			lua_pushnil(lua);
			return 1;
		}

		// Get the text
		const char* text = NULL;
		if (lua_isstring(lua, 4))
			text = lua_tostring(lua, 4);
		else
		{
			CIwGameError::LogError("Warning: LUA_CreateActor Text missing in Param3");
			lua_pushnil(lua);
			return 1;
		}

		// Get the brush
		IIwGameBrush *brush = NULL;
		if (lua_isuserdata(lua, 5))
			brush = (IIwGameBrush*)lua_touserdata(lua, 5);
		if (brush == NULL || brush->getClassTypeHash() != CIwGameXomlNames::Brush_Hash)
		{
			CIwGameError::LogError("Warning: LUA_CreateActor Invalid brush in Param4");
			lua_pushnil(lua);
			return 1;
		}

		// Get the width and height
		int width = -100, height = -100;
		if (lua_isnumber(lua, 6))
			width = (int)lua_tonumber(lua, 6);
		else
		{
			CIwGameError::LogError("Warning: LUA_CreateActor Width missing in Param5");
			lua_pushnil(lua);
			return 1;
		}
		if (lua_isnumber(lua, 7))
			height = (int)lua_tonumber(lua, 7);
		else
		{
			CIwGameError::LogError("Warning: LUA_CreateActor Height missing in Param6");
			lua_pushnil(lua);
			return 1;
		}

		CIwRect			rect = CIwRect(0, 0, -100, -100);
		CIwRect			margin = CIwRect(0, 0, 0, 0);
		CIwFVec4		skew = CIwFVec4(0, 0, 0, 0);
		new_actor->Init(brush, width, height);
		if (actor != NULL)
		{
			new_actor->setLinkedTo(actor);
			new_actor->setDepth(0);
		}
		CIwGameString t = text;
		new_actor->InitTextActor(font, rect, t, 0, IW_GX_FONT_ALIGN_CENTRE, IW_GX_FONT_ALIGN_MIDDLE, skew, margin, false);

		lua_pushlightuserdata(lua, new_actor);
	}
	else
		lua_pushnil(lua);

    return 1;
}

//
// LUA_Remove(container (object)
//
static int LUA_Remove(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: LUA_Remove not enough parameters, expected object, parent (scene), ......");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the main game object
	CIwGame* game = (CIwGame*)IW_GAME_GLOBAL_RESOURCES->getResourceManager()->getParent();

	// Get the object
	IIwGameXomlResource* object = NULL;
	if (lua_isuserdata(lua, 1))
		object = (IIwGameXomlResource*)lua_touserdata(lua, 1);
	else
	{
		CIwGameError::LogError("Warning: LUA_Remove Invalid object, expected an object in Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the parent scene
	CIwGameScene* scene = NULL;
	if (lua_gettop(lua) > 1)
	{
		if (lua_isuserdata(lua, 2))
		{
			scene = (CIwGameScene*)lua_touserdata(lua, 2);
			if (scene->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
			{
				CIwGameError::LogError("Warning: LUA_Remove Invalid object, expected a scene in Param1");
				lua_pushboolean(lua, false);
				return 1;
			}
		}
		else
		{
			CIwGameError::LogError("Warning: LUA_Remove Invalid object, expected a scene in Param1");
			lua_pushboolean(lua, false);
			return 1;
		}
	}

	if (object->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		game->removeScene((CIwGameScene*)object);
	else
	if (object->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
	{
		if (scene != NULL)
			scene->removeActor((CIwGameActor*)object);
		else
			((CIwGameActor*)object)->getScene()->removeActor((CIwGameActor*)object);
	}
	else
	if (object->getClassTypeHash() == CIwGameXomlNames::Program_Hash)
	{
		if (scene != NULL)
			scene->getProgrameManager()->removeProgram((CIwGameProgram*)object);
		else
			IW_GAME_GLOBAL_RESOURCES->getProgrameManager()->removeProgram((CIwGameProgram*)object);
	}
	else
	{
		if (scene != NULL)
			scene->getResourceManager()->removeResource(object);
		else
			IW_GAME_GLOBAL_RESOURCES->getResourceManager()->removeResource((CIwGameProgram*)object);
	}

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
        CIwGameError::LogError("Warning: LUA_setCurrentScene not enough parameters, expected scene (object)");
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
        CIwGameError::LogError("Warning: LUA_setCurrentScene Invalid scene for Param0");
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
// LUA_RemoveTaggedResources(type-name)
//
static int LUA_RemoveTaggedResources(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: LUA_RemoveTaggedResources not enough parameters, expected tag-name");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the type of actor to create
	const char* tag = NULL;
	if (lua_isstring(lua, 1))
		tag = lua_tostring(lua, 1);
	else
	{
        CIwGameError::LogError("Warning: LUA_RemoveTaggedResources Invalid parameter, expected tag-name for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}
	unsigned int tag_hash = IW_GAME_HASH(tag);

	IW_GAME_GLOBAL_RESOURCES->getResourceManager()->removeTaggedResources(tag_hash);

	lua_pushboolean(lua, true);
	return 1;
}

//
// LUA_PlaySound sound (object), volume (number), pitch (number), pan (number)
//
static int LUA_PlaySound(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: LUA_PlaySound not enough parameters, expected sound (object) (volume, pitch, pan optional)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the target object
	CIwGameSound* sound = NULL;
	if (lua_isuserdata(lua, 1))
		sound = (CIwGameSound*)lua_touserdata(lua, 1);
	if (sound == NULL || sound->getClassTypeHash() != CIwGameXomlNames::Sound_Hash)
	{
        CIwGameError::LogError("Warning: LUA_PlaySound Invalid sound object for Param0");
		lua_pushnil(lua);
		return 1;
	}

	float volume = 1.0f;
	float pitch = 1.0f;
	float pan = 1.0f;

	if (count > 1)
	{
		if (lua_isnumber(lua, 2))
			volume = (float)lua_tonumber(lua, 2);
		else
		{
			CIwGameError::LogError("Warning: LUA_PlaySound volume must be a number (Param1) - ", sound->getName().c_str());
			lua_pushnil(lua);
			return 1;
		}
	}
	if (count > 2)
	{
		if (lua_isnumber(lua, 3))
			pitch = (float)lua_tonumber(lua, 3);
		else
		{
			CIwGameError::LogError("Warning: LUA_PlaySound pitch must be a number (Param2) - ", sound->getName().c_str());
			lua_pushnil(lua);
			return 1;
		}
	}
	if (count > 3)
	{
		if (lua_isnumber(lua, 4))
			pan = (float)lua_tonumber(lua, 4);
		else
		{
			CIwGameError::LogError("Warning: LUA_PlaySound volume must be a number (Param3) - ", sound->getName().c_str());
			lua_pushnil(lua);
			return 1;
		}
	}

	CIwSoundInst* inst = sound->Play(volume, pitch, pan);

	lua_pushlightuserdata(lua, inst);
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_ChangeSound sound-instance (object), paramater-name (string), value (number)
//
static int LUA_ChangeSound(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 3)
	{
        CIwGameError::LogError("Warning: LUA_ChangeSound not enough parameters, expected sound (object) (volume, pitch, pan optional)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	CIwSoundInst* sound_instance = NULL;
	if (lua_isuserdata(lua, 1))
		sound_instance = (CIwSoundInst*)lua_touserdata(lua, 1);
	if (sound_instance == NULL)
	{
        CIwGameError::LogError("Warning: LUA_ChangeSound,  invalid sound-instance for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get parameter to change
	const char* param = NULL;
	if (lua_isstring(lua, 2))
		param = lua_tostring(lua, 2);
	else
	{
		CIwGameError::LogError("Warning: LUA_ChangeSound, invalid sound parameter (Param1)  ");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get value
	float value;
	if (lua_isnumber(lua, 3))
		value = (float)lua_tonumber(lua, 3);
	else
	{
		CIwGameError::LogError("Warning: LUA_ChangeSound, invalid sound parameter value (Param2)  ");
		lua_pushboolean(lua, false);
		return 1;
	}

	unsigned int param_hash = IW_GAME_HASH(param);
	if (param_hash == CIwGameXomlNames::Volume_Hash)
		sound_instance->SetVol((iwfixed)(value * IW_GEOM_ONE));
	else
	if (param_hash == CIwGameXomlNames::Pitch_Hash)
		sound_instance->SetPitch((iwfixed)(value * IW_GEOM_ONE));
	else
	if (param_hash == CIwGameXomlNames::Pan_Hash)
		sound_instance->SetPan((iwfixed)(value * IW_GEOM_ONE));
	else
	{
		CIwGameError::LogError("Warning: LUA_ChangeSound, invalid sound parameter instance (Param1) - ", param);
		lua_pushboolean(lua, false);
		return 1;
	}

	sound_instance->IsPlaying();

	lua_pushboolean(lua, true);
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_isSoundPlaying sound-instance (object)
//
static int LUA_isSoundPlaying(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: LUA_isSoundPlaying not enough parameters, expected sound-instance (object)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	CIwSoundInst* sound_instance = NULL;
	if (lua_isuserdata(lua, 1))
		sound_instance = (CIwSoundInst*)lua_touserdata(lua, 1);
	if (sound_instance == NULL)
	{
		lua_pushboolean(lua, false);
		return 1;
	}

	lua_pushboolean(lua, sound_instance->IsPlaying());
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_PlayMusic file-name (string)
//
static int LUA_PlayMusic(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: LUA_PlayMusic not enough parameters, expected file-name (string), repeat-count (number, optional) ");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	const char* filename = NULL;
	if (lua_isstring(lua, 1))
		filename = lua_tostring(lua, 1);
	if (filename == NULL)
	{
        CIwGameError::LogError("Warning: LUA_PlayMusic,  invalid music file-name for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	int repeat_count = 0;
	if (count > 1)
	{
		// Get the target object
		const char* filename = NULL;
		if (lua_isnumber(lua, 2))
			repeat_count = (int)lua_tonumber(lua, 2);
		else
		{
			CIwGameError::LogError("Warning: LUA_PlayMusic,  invalid value for repeat-count in Param1");
			lua_pushboolean(lua, false);
			return 1;
		}
	}

	lua_pushboolean(lua, IW_GAME_AUDIO->PlayMusic(filename, repeat_count));
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_StopMusic - Stops music from playing
//
static int LUA_StopMusic(lua_State *lua)
{
	int count = lua_gettop(lua);
	IW_GAME_AUDIO->StopMusic();

	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_PauseMusic - Pauses music
//
static int LUA_PauseMusic(lua_State *lua)
{
	int count = lua_gettop(lua);
	IW_GAME_AUDIO->PauseMusic();

	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_ResumeMusic - resume music
//
static int LUA_ResumeMusic(lua_State *lua)
{
	int count = lua_gettop(lua);
	IW_GAME_AUDIO->ResumeMusic();

	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_isMusicPlaying - Checks if music is currently playing
//
static int LUA_isMusicPlaying(lua_State *lua)
{
	int count = lua_gettop(lua);
	lua_pushboolean(lua, IW_GAME_AUDIO->isMusicPlaying());
    return 1;
}

//
// LUA_isAudioCodecSupported - Checks if music codec is supported
//
static int LUA_isAudioCodecSupported(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: LUA_isAudioCodecSupported not enough parameters, expected codec (string) ");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	const char* codec = NULL;
	if (lua_isstring(lua, 1))
		codec = lua_tostring(lua, 1);
	if (codec == NULL)
	{
        CIwGameError::LogError("Warning: LUA_isAudioCodecSupported,  invalid codec for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	unsigned int codec_hash = IW_GAME_HASH(codec);
	s3eAudioCodec cod;

	if (codec_hash == IW_GAME_HASH("midi"))
		cod = S3E_AUDIO_CODEC_MIDI;
	else
	if (codec_hash == IW_GAME_HASH("mp3"))
		cod = S3E_AUDIO_CODEC_MP3;
	else
	if (codec_hash == IW_GAME_HASH("aac"))
		cod = S3E_AUDIO_CODEC_AAC;
	else
	if (codec_hash == IW_GAME_HASH("aacplus"))
		cod = S3E_AUDIO_CODEC_AACPLUS;
	else
	if (codec_hash == IW_GAME_HASH("qcp"))
		cod = S3E_AUDIO_CODEC_QCP;
	else
	if (codec_hash == IW_GAME_HASH("pcm"))
		cod = S3E_AUDIO_CODEC_PCM;
	else
	if (codec_hash == IW_GAME_HASH("spf"))
		cod = S3E_AUDIO_CODEC_SPF;
	else
	if (codec_hash == IW_GAME_HASH("amr"))
		cod = S3E_AUDIO_CODEC_AMR;
	else
	if (codec_hash == IW_GAME_HASH("mp4"))
		cod = S3E_AUDIO_CODEC_MP4;
	else
	{
        CIwGameError::LogError("Warning: LUA_isAudioCodecSupported,  invalid codec for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	lua_pushboolean(lua, IW_GAME_AUDIO->isMusicCodecSupported(cod));
    return 1;
}

//
// LUA_isVideoCodecSupported - Checks if video codec is supported
//
static int LUA_isVideoCodecSupported(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: LUA_isVideoCodecSupported not enough parameters, expected codec (string) ");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	const char* codec = NULL;
	if (lua_isstring(lua, 1))
		codec = lua_tostring(lua, 1);
	if (codec == NULL)
	{
        CIwGameError::LogError("Warning: LUA_isVideoCodecSupported,  invalid codec for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	unsigned int codec_hash = IW_GAME_HASH(codec);
	bool found = false;
	if (CIwGameVideoPlayer::isCodecSupported(codec_hash, found))
		lua_pushboolean(lua, true);
	else
	{
		if (!found)
			CIwGameError::LogError("Warning: LUA_isVideoCodecSupported,  invalid codec for Param0");

		lua_pushboolean(lua, false);
	}

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
        CIwGameError::LogError("Warning: LUA_LaunchURL not enough parameters, expected url (string) ");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the url
	const char* url = NULL;
	if (lua_isstring(lua, 1))
		url = lua_tostring(lua, 1);
	if (url == NULL)
	{
        CIwGameError::LogError("Warning: LUA_LaunchURL,  invalid url for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	s3eOSExecExecute(url, false);

	lua_pushboolean(lua, true);
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
        CIwGameError::LogError("Warning: LUA_CallActions not enough parameters, expected actions (object), target-scene (object)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	CIwGameActions* actions = NULL;
	if (lua_isuserdata(lua, 1))
		actions = (CIwGameActions*)lua_touserdata(lua, 1);
	if (actions == NULL || actions->getClassTypeHash() != CIwGameXomlNames::Actions_Hash)
	{
        CIwGameError::LogError("Warning: LUA_CallActions Invalid actions object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target
	IIwGameXomlResource* object = NULL;
	if (lua_isuserdata(lua, 2))
		object = (IIwGameXomlResource*)lua_touserdata(lua, 2);
	else
	{
		CIwGameError::LogError("Warning: LUA_CallActions Invalid target, expected scene or actor in Param1");
		lua_pushboolean(lua, false);
		return 1;
	}

	if (object->getClassTypeHash() != CIwGameXomlNames::Actor_Hash && object->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
	{
		CIwGameError::LogError("Warning: LUA_CallActions Invalid target, expected scene or actor in Param1");
		lua_pushboolean(lua, false);
		return 1;
	}

	actions->Execute(object);
 
	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_ChangeProgram object (program), command (stop, start, restart, pause) command_name (for goto)
//
static int LUA_ChangeProgram(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 2)
	{
        CIwGameError::LogError("Warning: LUA_ChangeProgram not enough parameters, expected program (object), command, command-name (string, optional)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the program
	CIwGameProgram* program = NULL;
	if (lua_isuserdata(lua, 1))
		program = (CIwGameProgram*)lua_touserdata(lua, 1);
	if (program == NULL)
	{
        CIwGameError::LogError("Warning: LUA_ChangeProgram Invalid program object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the command
	const char* cmd = NULL;
	if (lua_isstring(lua, 2))
		cmd = lua_tostring(lua, 2);
	else
	{
		CIwGameError::LogError("Warning: LUA_ChangeProgram Invalid command, expected string as Param1");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the command name
	const char* cmd_name = NULL;
	if (count > 2)
	{
		if (lua_isstring(lua, 3))
			cmd_name = lua_tostring(lua, 3);
		if (cmd_name == NULL)
		{
			CIwGameError::LogError("Warning: LUA_ChangeProgram Invalid target command name, expected string as Param2");
			lua_pushboolean(lua, false);
			return 1;
		}
	}

	unsigned int cmd_hash = IW_GAME_HASH(cmd);
	if (cmd_hash == CIwGameXomlNames::Stop_Hash)
		program->stop();
	else
	if (cmd_hash == CIwGameXomlNames::Start_Hash)
		program->start();
	else
	if (cmd_hash == CIwGameXomlNames::Restart_Hash)
		program->restart();
	else
	if (cmd_hash == CIwGameXomlNames::Pause_Hash)
		program->pause();
	else
	if (cmd_hash == CIwGameXomlNames::Next_Hash)
		program->nextCommand();
	else
	if (cmd_hash == CIwGameXomlNames::Priority_Hash)
		program->getManager()->setPriorityProgram(program);
	else
	if (cmd_hash == CIwGameXomlNames::Goto_Hash)
	{
		IIwGameCommandExecutor* command = program->findCommand(cmd_name);
		if (command != NULL)
			command->gotoCommand();
		else
			CIwGameError::LogError("Warning: LUA_ChangeProgram - goto command that does not exist - ", cmd_name);
	}
	else
	{
		CIwGameError::LogError("Warning: LUA_ChangeProgram command not supported in Param1 - ", cmd);
		lua_pushboolean(lua, false);
		return 1;
	}
 
	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_isProgramRunning object (program)
//
static int LUA_isProgramRunning(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: LUA_isProgramRunning not enough parameters, expected program (object), command");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	CIwGameProgram* program = NULL;
	if (lua_isuserdata(lua, 1))
		program = (CIwGameProgram*)lua_touserdata(lua, 1);
	if (program == NULL)
	{
        CIwGameError::LogError("Warning: LUA_isProgramRunning Invalid program object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	lua_pushboolean(lua, program->isRunning());
    return 1;
}

//
// LUA_isProgramPaused object (program)
//
static int LUA_isProgramPaused(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 2)
	{
        CIwGameError::LogError("Warning: LUA_isProgramPaused not enough parameters, expected program (object), command");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	CIwGameProgram* program = NULL;
	if (lua_isuserdata(lua, 1))
		program = (CIwGameProgram*)lua_touserdata(lua, 1);
	if (program == NULL)
	{
        CIwGameError::LogError("Warning: LUA_isProgramPaused Invalid program object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	lua_pushboolean(lua, program->isPaused());
    return 1;
}

//
// LUA_ChangeTimeline object (timeline), command (stop, start, restart, pause)
//
static int LUA_ChangeTimeline(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 2)
	{
        CIwGameError::LogError("Warning: LUA_ChangeTimeline not enough parameters, expected program (object), command");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the program
	CIwGameAnimTimeline* timeline = NULL;
	if (lua_isuserdata(lua, 1))
		timeline = (CIwGameAnimTimeline*)lua_touserdata(lua, 1);
	if (timeline == NULL)
	{
        CIwGameError::LogError("Warning: LUA_ChangeTimeline Invalid timeline object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the command
	const char* cmd = NULL;
	if (lua_isstring(lua, 2))
		cmd = lua_tostring(lua, 2);
	else
	{
		CIwGameError::LogError("Warning: LUA_ChangeTimeline Invalid command, expected string as Param1");
		lua_pushboolean(lua, false);
		return 1;
	}

	unsigned int cmd_hash = IW_GAME_HASH(cmd);
	if (cmd_hash == CIwGameXomlNames::Stop_Hash)
		timeline->stop();
	else
	if (cmd_hash == CIwGameXomlNames::Start_Hash)
		timeline->play();
	else
	if (cmd_hash == CIwGameXomlNames::Restart_Hash)
		timeline->restart();
	else
	if (cmd_hash == CIwGameXomlNames::Pause_Hash)
		timeline->pause();
	else
	{
		CIwGameError::LogError("Warning: LUA_ChangeProgram command not supported in Param1 - ", cmd);
		lua_pushboolean(lua, false);
		return 1;
	}
 
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
        CIwGameError::LogError("Warning: LUA_isTimelinePlaying not enough parameters, expected timeline (object)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	CIwGameAnimTimeline* timeline = NULL;
	if (lua_isuserdata(lua, 1))
		timeline = (CIwGameAnimTimeline*)lua_touserdata(lua, 1);
	if (timeline == NULL)
	{
        CIwGameError::LogError("Warning: LUA_isTimelinePlaying Invalid timeline object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	lua_pushboolean(lua, timeline->isPlaying());
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
// LUA_getTextInput - Opens on screen keyboard to get text input from user
//
static int LUA_getTextInput(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: LUA_getTextInput not enough parameters, expected message (string) ");
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
        CIwGameError::LogError("Warning: LUA_getTextInput,  invalid message for Param0");
		lua_pushnil(lua);
		return 1;
	}

	if (count > 1)
	{
		if (lua_isstring(lua, 2))
			default_text = lua_tostring(lua, 2);
		if (default_text == NULL)
		{
			CIwGameError::LogError("Warning: LUA_getTextInput,  invalid default_text for Param1");
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
        CIwGameError::LogError("Warning: LUA_getTouchInfo not enough parameters, expected touch-index (number) ");
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
			CIwGameError::LogError("Error: LUA_getTouchInfo,  touch-index out of range, expected 0 to 4 for Param0");
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
// LUA_isActorClipped actor (object)
//
static int LUA_isActorClipped(lua_State *lua)
{
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: LUA_isActorClipped not enough parameters, expected actor (object)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the target object
	CIwGameActor* actor = NULL;
	if (lua_isuserdata(lua, 1))
		actor = (CIwGameActor*)lua_touserdata(lua, 1);
	if (actor == NULL || actor->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: LUA_isActorClipped Invalid actor object");
		lua_pushnil(lua);
		return 1;
	}

	lua_pushboolean(lua, actor->getVisual()->isClippedByManager());
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_ActorsOverlap actor1 (object), actor2 (object)
//
static int LUA_ActorsOverlap(lua_State *lua)
{
	if (lua_gettop(lua) < 3)
	{
        CIwGameError::LogError("Warning: LUA_ActorsOverlap not enough parameters, expected actor1 (object), actor2 (object), quick (boolean)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the actor1 object
	CIwGameActor* actor1 = NULL;
	if (lua_isuserdata(lua, 1))
		actor1 = (CIwGameActor*)lua_touserdata(lua, 1);
	if (actor1 == NULL || actor1->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: LUA_ActorsOverlap Invalid actor1 object");
		lua_pushnil(lua);
		return 1;
	}

	// Get the actor2 object
	CIwGameActor* actor2 = NULL;
	if (lua_isuserdata(lua, 2))
		actor2 = (CIwGameActor*)lua_touserdata(lua, 2);
	if (actor2 == NULL || actor2->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: LUA_ActorsOverlap Invalid actor2 object");
		lua_pushnil(lua);
		return 1;
	}

	// Get quick test
	bool quick = true;
	if (lua_isboolean(lua, 3))
		quick = lua_toboolean(lua, 3) != 0;

	if (quick)
		lua_pushboolean(lua, actor1->CheckCollision(actor2));
	else
		lua_pushboolean(lua, actor1->CheckCollisionRect(actor2));
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_getDistanceBetween actor1 (object), actor2 (object)
//
static int LUA_getDistanceBetween(lua_State *lua)
{
	if (lua_gettop(lua) < 2)
	{
        CIwGameError::LogError("Warning: LUA_getDistanceBetween not enough parameters, expected actor1 (object), actor2 (object)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the actor1 object
	CIwGameActor* actor1 = NULL;
	if (lua_isuserdata(lua, 1))
		actor1 = (CIwGameActor*)lua_touserdata(lua, 1);
	if (actor1 == NULL || actor1->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: LUA_getDistanceBetween Invalid actor1 object");
		lua_pushnil(lua);
		return 1;
	}

	// Get the actor2 object
	CIwGameActor* actor2 = NULL;
	if (lua_isuserdata(lua, 2))
		actor2 = (CIwGameActor*)lua_touserdata(lua, 2);
	if (actor2 == NULL || actor2->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: LUA_getDistanceBetween Invalid actor2 object");
		lua_pushnil(lua);
		return 1;
	}

	lua_pushnumber(lua, actor1->getDistanceBetween(actor2));
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_getAngleBetween actor1 (object), actor2 (object)
//
static int LUA_getAngleBetween(lua_State *lua)
{
	if (lua_gettop(lua) < 2)
	{
        CIwGameError::LogError("Warning: LUA_getAngleBetween not enough parameters, expected actor1 (object), actor2 (object)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the actor1 object
	CIwGameActor* actor1 = NULL;
	if (lua_isuserdata(lua, 1))
		actor1 = (CIwGameActor*)lua_touserdata(lua, 1);
	if (actor1 == NULL || actor1->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: LUA_getAngleBetween Invalid actor1 object");
		lua_pushnil(lua);
		return 1;
	}

	// Get the actor2 object
	CIwGameActor* actor2 = NULL;
	if (lua_isuserdata(lua, 2))
		actor2 = (CIwGameActor*)lua_touserdata(lua, 2);
	if (actor2 == NULL || actor2->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: LUA_getAngleBetween Invalid actor2 object");
		lua_pushnil(lua);
		return 1;
	}

	lua_pushnumber(lua, actor1->getAngleBetween(actor2));
 
    // Return the number of result arguments that were passed back to lua
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
// LUA_SceneToScreen scene (object) x (number) y (number) - Returns vector
//
static int LUA_SceneToScreen(lua_State *lua)
{
	if (lua_gettop(lua) < 3)
	{
        CIwGameError::LogError("Warning: LUA_SceneToScreen not enough parameters, expected scene (object) x (number) y (number)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the scene object
	CIwGameScene* scene = NULL;
	if (lua_isuserdata(lua, 1))
		scene = (CIwGameScene*)lua_touserdata(lua, 1);
	if (scene == NULL || scene->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
	{
        CIwGameError::LogError("Warning: LUA_SceneToScreen Invalid scene object");
		lua_pushnil(lua);
		return 1;
	}

	// Get x coord
	float x = 0;
	if (lua_isnumber(lua, 2))
		x = (float)lua_tonumber(lua, 2);
	else
	{
        CIwGameError::LogError("Warning: LUA_SceneToScreen expected a number for Param1");
		lua_pushnil(lua);
		return 1;
	}

	// Get y coord
	float y = 0;
	if (lua_isnumber(lua, 3))
		y = (float)lua_tonumber(lua, 3);
	else
	{
        CIwGameError::LogError("Warning: LUA_SceneToScreen expected a number for Param2");
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
        CIwGameError::LogError("Warning: LUA_ScreenToScene not enough parameters, expected scene (object) x (number) y (number) include_camera (boolean)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the scene object
	CIwGameScene* scene = NULL;
	if (lua_isuserdata(lua, 1))
		scene = (CIwGameScene*)lua_touserdata(lua, 1);
	if (scene == NULL || scene->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
	{
        CIwGameError::LogError("Warning: LUA_ScreenToScene Invalid scene object");
		lua_pushnil(lua);
		return 1;
	}

	// Get x coord
	float x = 0;
	if (lua_isnumber(lua, 2))
		x = (float)lua_tonumber(lua, 2);
	else
	{
        CIwGameError::LogError("Warning: LUA_ScreenToScene expected a number for Param1");
		lua_pushnil(lua);
		return 1;
	}

	// Get y coord
	float y = 0;
	if (lua_isnumber(lua, 3))
		y = (float)lua_tonumber(lua, 3);
	else
	{
        CIwGameError::LogError("Warning: LUA_ScreenToScene expected a number for Param2");
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
			CIwGameError::LogError("Warning: LUA_ScreenToScene expected a boolean for Param3");
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


//
// Lua AppEasy System library
//
static const luaL_Reg g_syslib[] = {
	{"isType",			LUA_isType}, 
	{"isTypeOf",		LUA_isTypeOf}, 
	{"LaunchURL",		LUA_LaunchURL}, 
	{"Exit",			LUA_Exit}, 
	{"getTimeMs",		LUA_getTimeMs}, 
	{"getDeviceType",	LUA_getDeviceType}, 
	{"getLocale",		LUA_getLocale}, 
	{"getFreeMem",		LUA_getFreeMem}, 
	{NULL, NULL}
};

LUALIB_API int luaopen_gen(lua_State *L)
{
  luaL_register(L, "gen", g_genlib);
  
  return 1;
}

