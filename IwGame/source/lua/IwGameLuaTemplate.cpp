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
// LUA_CreateFromTemplate template (object), container (object), templates parameters (table)
//
static int LUA_CreateFromTemplate(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 2)
	{
        CIwGameError::LogError("Warning: template.from() not enough parameters, expected actor-name (string), scene (object), templates parameters (table)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the template
	CIwGameTemplate* temp = NULL;
	if (lua_isuserdata(lua, 1))
		temp = (CIwGameTemplate*)lua_touserdata(lua, 1);
	if (temp == NULL || (temp != NULL && temp->getClassTypeHash() != CIwGameXomlNames::Template_Hash))
	{
        CIwGameError::LogError("Warning: template.from() Invalid template for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the scene
	IIwGameXomlResource* container = NULL;
	if (lua_isuserdata(lua, 2))
		container = (CIwGameScene*)lua_touserdata(lua, 2);
	if (container == NULL)
	{
        CIwGameError::LogError("Warning: template.from() Invalid container for Param1");
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
		CIwGameError::LogError("Error: template.from() could not instantiate from items template - ", temp->getName().c_str());
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
// LUA_DestroyTemplate(template (object))
//
static int LUA_DestroyTemplate(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: template.destroy() not enough parameters, expected template (object)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the object
	IIwGameXomlResource* object = NULL;
	if (lua_isuserdata(lua, 1))
		object = (IIwGameXomlResource*)lua_touserdata(lua, 1);
	if (object == NULL || object->getClassTypeHash() != CIwGameXomlNames::Template_Hash)
	{
		CIwGameError::LogError("Warning: template.destroy() Invalid object, expected a template in Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	object->getParent()->removeResource(object);

	lua_pushboolean(lua, true);
	return 1;
}

//
// LUA_FindTemplate template-name (string), container scene (object, optional)
//
static int LUA_FindTemplate(lua_State *lua)
{
	int count = lua_gettop(lua);

	if (count < 1)
	{
        CIwGameError::LogError("Warning: template.find() not enough parameters, expected template-name (string), container scene (object, optional)");
		lua_pushnil(lua);
		return 1;
	}

	IIwGameXomlResource* scene = NULL;
	const char* name = NULL;

	// Get the resource name
	if (lua_isstring(lua, 1))
		name = lua_tostring(lua, 1);
	if (name == NULL)
	{
        CIwGameError::LogError("Warning: template.find() Invalid name for Param0");
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
			CIwGameError::LogError("Warning: template.find() expected scene for Param1");
			lua_pushnil(lua);
			return 1;
		}
	}
	else
		scene = (CIwGameScene*)lua->user_data;

	// Locate resource
	IIwGameXomlResource* resource = CIwGameXomlResourceManager::FindResource(name, CIwGameXomlNames::Template_Hash, scene);
	if (resource == NULL)
	{
        CIwGameError::LogError("Warning: template.find() template not found - ", name);
		lua_pushnil(lua);
		return 1;
	}

	// Return the resource
	lua_pushlightuserdata(lua, resource);

    return 1;
}


static const luaL_Reg g_templatelib[] =
{
	{ "find",		LUA_FindTemplate}, 
	{ "destroy",	LUA_DestroyTemplate}, 
	{ "from",		LUA_CreateFromTemplate}, 
	{NULL, NULL}
};

LUALIB_API int luaopen_template(lua_State *L)
{
	luaL_register(L, "template", g_templatelib);

	return 1;
}
