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
#include "IwGameActor.h"
#include "IwGameScene.h"
#include "IwGameProgram.h"
#include "IwGameUI.h"
#include "IwGameBrush.h"
#include "IwGame.h"
#include "IwGameTemplates.h"

//
// LUA_SetProperty actor (object), property (string), value (string)
//
static int LUA_SetPropertyActor(lua_State *lua)
{
	if (lua_gettop(lua) < 3)
	{
        CIwGameError::LogError("Warning: actor.set() not enough parameters, expected actor (object), property (string), value (string)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	IIwGameXomlResource* object = NULL;
	if (lua_isuserdata(lua, 1))
		object = (IIwGameXomlResource*)lua_touserdata(lua, 1);
	if (object == NULL || object->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: actor.set() Invalid target object, expected actor");
		lua_pushboolean(lua, false);
		return 1;
	}

	CIwGameString prop_name, value;
    if (lua_isstring(lua, 2))
        prop_name = lua_tostring(lua, 2);
    else
	{
        CIwGameError::LogError("Warning: actor.set() property must be a property name - object - ", object->getName().c_str());
		lua_pushboolean(lua, false);
		return 1;
	}

	if (!ValueToString(lua, 3, value))
	{
        CIwGameError::LogError("Warning: actor.set() value must be a string, number, boolean or vec - object - ", object->getName().c_str());
		lua_pushboolean(lua, false);
		return 1;
	}
CIwGameError::LogError("Setting property to - ", value.c_str());

	((CIwGameActor*)object)->setProperty(prop_name.getHash(), value, false);

	lua_pushboolean(lua, true);
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_AddProperty actor (object), property (string), value (string)
//
static int LUA_AddPropertyActor(lua_State *lua)
{
	if (lua_gettop(lua) < 3)
	{
        CIwGameError::LogError("Warning: actor.add() not enough parameters, expected scene or actor (object), property (string), value (string)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	IIwGameXomlResource* object = NULL;
	if (lua_isuserdata(lua, 1))
		object = (IIwGameXomlResource*)lua_touserdata(lua, 1);
	if (object == NULL || object->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: actor.add() Invalid target object for Param0, expected actor");
		lua_pushboolean(lua, false);
		return 1;
	}

	CIwGameString prop_name, value;
    if (lua_isstring(lua, 2))
        prop_name = lua_tostring(lua, 2);
    else
	{
        CIwGameError::LogError("Warning: actor.add() property must be a property name (Param1) - object - ", object->getName().c_str());
		lua_pushboolean(lua, false);
		return 1;
	}

	if (!ValueToString(lua, 3, value))
	{
        CIwGameError::LogError("Warning: actor.add() value must be a string, number, boolean or vec  - object - ", object->getName().c_str());
		lua_pushboolean(lua, false);
		return 1;
	}

	((CIwGameActor*)object)->setProperty(prop_name.getHash(), value, true);

    lua_pushboolean(lua, true);
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_GetProperty actor (object), property (string)
//
static int LUA_GetPropertyActor(lua_State *lua)
{
	if (lua_gettop(lua) < 2)
	{
        CIwGameError::LogError("Warning: actor.get() not enough parameters, expected scene or actor (object), property (string)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the target object
	IIwGameXomlResource* object = NULL;
	if (lua_isuserdata(lua, 1))
		object = (IIwGameXomlResource*)lua_touserdata(lua, 1);
	if (object == NULL || object->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: actor.get() Invalid target object for Param0, expected actor");
		lua_pushnil(lua);
		return 1;
	}

	CIwGameString prop_name, value;
    if (lua_isstring(lua, 2))
        prop_name = lua_tostring(lua, 2);
    else
	{
        CIwGameError::LogError("Warning: actor.get() property must be a property name (Param1) - object - ", object->getName().c_str());
		lua_pushnil(lua);
		return 1;
	}

	CIwGameXomlProperty prop;
	if (!((CIwGameActor*)object)->getProperty(prop_name.getHash(), prop))
	{
		CIwGameError::LogError("Warning: actor.get() actor property does not exist - ", prop_name.c_str());
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
// LUA_FindActor actor-name (string), scene (object, optional)
//
static int LUA_FindActor(lua_State *lua)
{
	int count = lua_gettop(lua);

	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: actor.find() not enough parameters, expected actor-name (string), scene (object, optional)");
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
        CIwGameError::LogError("Warning: actor.find() Invalid name for Param0");
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
			CIwGameError::LogError("Warning: actor.find() expected scene for Param1");
			lua_pushnil(lua);
			return 1;
		}
	}
	else
		container = (CIwGameScene*)lua->user_data;

	if (container == NULL)
	{
		CIwGameError::LogError("Warning: actor.find() invalid container scene");
		lua_pushnil(lua);
		return 1;
	}

	// Locate actor
	CIwGameActor* actor = ((CIwGameScene*)container)->findActor(name);
	if (actor == NULL)
	{
        CIwGameError::LogError("Warning: actor.find() actor not found - ", name);
		lua_pushnil(lua);
		return 1;
	}

	// Return the actor
	lua_pushlightuserdata(lua, actor);

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
        CIwGameError::LogError("Warning: actor.create() not enough parameters, expected type (string) parent (scene or actor), ......");
		lua_pushnil(lua);
		return 1;
	}

	// Get the type of actor to create
	const char* type = NULL;
	if (lua_isstring(lua, 1))
		type = lua_tostring(lua, 1);
	else
	{
        CIwGameError::LogError("Warning: actor.create() Invalid actor type for Param0");
		lua_pushnil(lua);
		return 1;
	}

	// Get the parent container
	IIwGameXomlResource* container = NULL;
	if (lua_isuserdata(lua, 2))
		container = (CIwGameScene*)lua_touserdata(lua, 2);
	if (container == NULL || (container->getClassTypeHash() != CIwGameXomlNames::Actor_Hash && container->getClassTypeHash() != CIwGameXomlNames::Scene_Hash))
	{
        CIwGameError::LogError("Warning: actor.create() Invalid container for Param1");
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
			CIwGameError::LogError("Warning: actor.create() Invalid brush for Param2");
			lua_pushnil(lua);
			return 1;
		}

		// Get the width and height
		int width = -100, height = -100;
		if (lua_isnumber(lua, 4))
			width = (int)lua_tonumber(lua, 4);
		else
		{
			CIwGameError::LogError("Warning: actor.create() Width missing for Param3");
			lua_pushnil(lua);
			return 1;
		}
		if (lua_isnumber(lua, 5))
			height = (int)lua_tonumber(lua, 5);
		else
		{
			CIwGameError::LogError("Warning: actor.create() Height missing for Param4");
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
			CIwGameError::LogError("Warning: actor.create() Invalid font for Param2");
			lua_pushnil(lua);
			return 1;
		}

		// Get the text
		const char* text = NULL;
		if (lua_isstring(lua, 4))
			text = lua_tostring(lua, 4);
		else
		{
			CIwGameError::LogError("Warning: actor.create() Text missing for Param3");
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
			CIwGameError::LogError("Warning: actor.create() Invalid font for Param2");
			lua_pushnil(lua);
			return 1;
		}

		// Get the text
		const char* text = NULL;
		if (lua_isstring(lua, 4))
			text = lua_tostring(lua, 4);
		else
		{
			CIwGameError::LogError("Warning: actor.create() Text missing for Param3");
			lua_pushnil(lua);
			return 1;
		}

		// Get the brush
		IIwGameBrush *brush = NULL;
		if (lua_isuserdata(lua, 5))
			brush = (IIwGameBrush*)lua_touserdata(lua, 5);
		if (brush == NULL || brush->getClassTypeHash() != CIwGameXomlNames::Brush_Hash)
		{
			CIwGameError::LogError("Warning: actor.create() Invalid brush for Param4");
			lua_pushnil(lua);
			return 1;
		}

		// Get the width and height
		int width = -100, height = -100;
		if (lua_isnumber(lua, 6))
			width = (int)lua_tonumber(lua, 6);
		else
		{
			CIwGameError::LogError("Warning: actor.create() Width missing for Param5");
			lua_pushnil(lua);
			return 1;
		}
		if (lua_isnumber(lua, 7))
			height = (int)lua_tonumber(lua, 7);
		else
		{
			CIwGameError::LogError("Warning: actor.create() Height missing for Param6");
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
	{
		CIwGameError::LogError("Warning: actor.create() Cannot create actor of type - ", type);
		lua_pushnil(lua);
	}

    return 1;
}

//
// LUA_DestroyActor(container (object)
//
static int LUA_DestroyActor(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: actor.destroy() not enough parameters, expected actor (object)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the main game object
	CIwGame* game = (CIwGame*)IW_GAME_GLOBAL_RESOURCES->getResourceManager()->getParent();

	// Get the object
	IIwGameXomlResource* object = NULL;
	if (lua_isuserdata(lua, 1))
		object = (IIwGameXomlResource*)lua_touserdata(lua, 1);
	if (object == NULL || object->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Warning: actor.destroy() Invalid object, expected an actor for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	((CIwGameActor*)object)->getScene()->removeActor((CIwGameActor*)object);

	lua_pushboolean(lua, true);
	return 1;
}

//
// LUA_isActorClipped actor (object)
//
static int LUA_isActorClipped(lua_State *lua)
{
	if (lua_gettop(lua) < 1)
	{
        CIwGameError::LogError("Warning: actor.clipped() not enough parameters, expected actor (object)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the target object
	CIwGameActor* actor = NULL;
	if (lua_isuserdata(lua, 1))
		actor = (CIwGameActor*)lua_touserdata(lua, 1);
	if (actor == NULL || actor->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: actor.clipped() Invalid actor object for Param0");
		lua_pushnil(lua);
		return 1;
	}

	lua_pushboolean(lua, actor->getVisual()->isClippedByManager());
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_ActorsOverlaps actor1 (object), actor2 (object)
//
static int LUA_ActorsOverlaps(lua_State *lua)
{
	if (lua_gettop(lua) < 3)
	{
        CIwGameError::LogError("Warning: actor.overlaps() not enough parameters, expected actor1 (object), actor2 (object), quick (boolean)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the actor1 object
	CIwGameActor* actor1 = NULL;
	if (lua_isuserdata(lua, 1))
		actor1 = (CIwGameActor*)lua_touserdata(lua, 1);
	if (actor1 == NULL || actor1->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: actor.overlaps() Invalid actor1 object for Param0");
		lua_pushnil(lua);
		return 1;
	}

	// Get the actor2 object
	CIwGameActor* actor2 = NULL;
	if (lua_isuserdata(lua, 2))
		actor2 = (CIwGameActor*)lua_touserdata(lua, 2);
	if (actor2 == NULL || actor2->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: actor.overlaps() Invalid actor2 object for Param1");
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
// LUA_getDistanceBetweenActors actor1 (object), actor2 (object)
//
static int LUA_getDistanceBetweenActors(lua_State *lua)
{
	if (lua_gettop(lua) < 2)
	{
        CIwGameError::LogError("Warning: actor.distance() not enough parameters, expected actor1 (object), actor2 (object)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the actor1 object
	CIwGameActor* actor1 = NULL;
	if (lua_isuserdata(lua, 1))
		actor1 = (CIwGameActor*)lua_touserdata(lua, 1);
	if (actor1 == NULL || actor1->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: actor.distance() Invalid actor1 object");
		lua_pushnil(lua);
		return 1;
	}

	// Get the actor2 object
	CIwGameActor* actor2 = NULL;
	if (lua_isuserdata(lua, 2))
		actor2 = (CIwGameActor*)lua_touserdata(lua, 2);
	if (actor2 == NULL || actor2->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: actor.distance() Invalid actor2 object");
		lua_pushnil(lua);
		return 1;
	}

	lua_pushnumber(lua, actor1->getDistanceBetween(actor2));
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_getAngleBetweenActors actor1 (object), actor2 (object)
//
static int LUA_getAngleBetweenActors(lua_State *lua)
{
	if (lua_gettop(lua) < 2)
	{
        CIwGameError::LogError("Warning: actor.anglediff() not enough parameters, expected actor1 (object), actor2 (object)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the actor1 object
	CIwGameActor* actor1 = NULL;
	if (lua_isuserdata(lua, 1))
		actor1 = (CIwGameActor*)lua_touserdata(lua, 1);
	if (actor1 == NULL || actor1->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: actor.anglediff() Invalid actor1 object for Param0");
		lua_pushnil(lua);
		return 1;
	}

	// Get the actor2 object
	CIwGameActor* actor2 = NULL;
	if (lua_isuserdata(lua, 2))
		actor2 = (CIwGameActor*)lua_touserdata(lua, 2);
	if (actor2 == NULL || actor2->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: actor.anglediff() Invalid actor2 object for Param1");
		lua_pushnil(lua);
		return 1;
	}

	lua_pushnumber(lua, actor1->getAngleBetween(actor2));
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_getStartContacts actor (object) collision-mask (number)
//
static int LUA_getStartContacts(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: actor.getStartContacts() not enough parameters, expected actor (object), collision-mask (number, optional)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the actor object
	CIwGameActor* actor = NULL;
	if (lua_isuserdata(lua, 1))
		actor = (CIwGameActor*)lua_touserdata(lua, 1);
	if (actor == NULL || actor->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: actor.getStartContacts() Invalid actor object for Param0");
		lua_pushnil(lua);
		return 1;
	}

	unsigned int collision_mask = 0;
	if (count > 1)
	{
		if (lua_isnumber(lua, 2))
			collision_mask = (unsigned int)lua_tonumber(lua, 2);
		else
		{
			CIwGameError::LogError("Warning: actor.getStartContacts() Invalid collision-mask for Param1, expected number");
			lua_pushnil(lua);
			return 1;
		}
	}

	CIwGameBox2dBody* body = actor->getBox2dBody();
	if (body == NULL)
	{
		lua_pushnil(lua);
		return 1;
	}

	CIwGameSlotArray<CIwGameBox2dCollidable*>& started = body->getCollisionsStarted();

	int col_count = 0;
	for (int t = 0; t < started.getSize(); t++)
	{
		CIwGameBox2dCollidable* collision = started.element_at(t);
		if (collision != NULL)
		{
			CIwGameActor* actor2 = (CIwGameActor*)collision->getUserData();
			if (actor2 != NULL && (actor2->getType() & collision_mask) == collision_mask)
				col_count++;
		}
	}
	if (col_count == 0)
	{
		lua_pushnil(lua);
		return 1;
	}
	
	lua_createtable(lua, col_count, 0);
	int new_table = lua_gettop(lua);
    int index = 1;

	// Check begin contacts
	for (int t = 0; t < started.getSize(); t++)
	{
		CIwGameBox2dCollidable* collision = started.element_at(t);
		if (collision != NULL)
		{
			CIwGameActor* actor2 = (CIwGameActor*)collision->getUserData();
			if (actor2 != NULL && (actor2->getType() & collision_mask) == collision_mask)
			{
				lua_pushlightuserdata(lua, actor2);
				lua_rawseti(lua, new_table, index++);
			}
		}
	}

    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_getEndContacts actor (object) collision-mask (number)
//
static int LUA_getEndContacts(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: actor.getEndContacts() not enough parameters, expected actor (object), collision-mask (number, optional)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the actor object
	CIwGameActor* actor = NULL;
	if (lua_isuserdata(lua, 1))
		actor = (CIwGameActor*)lua_touserdata(lua, 1);
	if (actor == NULL || actor->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: actor.getEndContacts() Invalid actor object for Param0");
		lua_pushnil(lua);
		return 1;
	}

	unsigned int collision_mask = 0;
	if (count > 1)
	{
		if (lua_isnumber(lua, 2))
			collision_mask = (unsigned int)lua_tonumber(lua, 2);
		else
		{
			CIwGameError::LogError("Warning: actor.getEndContacts() Invalid collision-mask for Param1, expected number");
			lua_pushnil(lua);
			return 1;
		}
	}

	CIwGameBox2dBody* body = actor->getBox2dBody();
	if (body == NULL)
	{
		lua_pushnil(lua);
		return 1;
	}

	CIwGameSlotArray<CIwGameBox2dCollidable*>& ended = body->getCollisionsEnded();

	int col_count = 0;
	for (int t = 0; t < ended.getSize(); t++)
	{
		CIwGameBox2dCollidable* collision = ended.element_at(t);
		if (collision != NULL)
		{
			CIwGameActor* actor2 = (CIwGameActor*)collision->getUserData();
			if (actor2 != NULL && (actor2->getType() & collision_mask) == collision_mask)
				col_count++;
		}
	}
	if (col_count == 0)
	{
		lua_pushnil(lua);
		return 1;
	}
	
	lua_createtable(lua, col_count, 0);
	int new_table = lua_gettop(lua);
    int index = 1;

	// Check end contacts
	for (int t = 0; t < ended.getSize(); t++)
	{
		CIwGameBox2dCollidable* collision = ended.element_at(t);
		if (collision != NULL)
		{
			CIwGameActor* actor2 = (CIwGameActor*)collision->getUserData();
			if (actor2 != NULL && (actor2->getType() & collision_mask) == collision_mask)
			{
				lua_pushlightuserdata(lua, actor2);
				lua_rawseti(lua, new_table, index++);
			}
		}
	}

    return 1;
}

//
// LUA_getChildrenActor actor (object)
//
static int LUA_getChildrenActor(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: actor.children() not enough parameters, expected actor (object)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the actor object
	CIwGameActor* actor = NULL;
	if (lua_isuserdata(lua, 1))
		actor = (CIwGameActor*)lua_touserdata(lua, 1);
	if (actor == NULL || actor->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: actor.children() Invalid actor object for Param0");
		lua_pushnil(lua);
		return 1;
	}

	// Count number of children
	int child_count = 0;
	CIwGameScene* scene = actor->getScene();
	for (CIwGameScene::_Iterator it = scene->begin(); it != scene->end(); ++it)
		if ((*it)->getLinkedTo() == actor)
			child_count++;
	if (child_count == 0)
	{
		lua_pushnil(lua);
		return 1;
	}

	lua_createtable(lua, child_count, 0);
	int new_table = lua_gettop(lua);
    int index = 1;

	// Add any actors that link to this actor
	for (CIwGameScene::_Iterator it = scene->begin(); it != scene->end(); ++it)
	{
		if ((*it)->getLinkedTo() == actor)
		{
			lua_pushlightuserdata(lua, *it);
			lua_rawseti(lua, new_table, index++);
		}
	}

    return 1;
}

//
// LUA_changeTimelineActor actor (object), command (play, stop. pause, restart) - Returns true if success
//
static int LUA_changeTimelineActor(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 2)
	{
        CIwGameError::LogError("Warning: actor.changeTimeline() not enough parameters, expected actor (object), command (play, stop. pause, restart)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the actor object
	CIwGameActor* actor = NULL;
	if (lua_isuserdata(lua, 1))
		actor = (CIwGameActor*)lua_touserdata(lua, 1);
	if (actor == NULL || actor->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
        CIwGameError::LogError("Warning: actor.changeTimeline() Invalid actor object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	const char* command = NULL;
	if (lua_isstring(lua, 2))
		actor = (CIwGameActor*)lua_tostring(lua, 2);
	else
	{
        CIwGameError::LogError("Warning: actor.changeTimeline() Invalid command for Param1, expected string");
		lua_pushboolean(lua, false);
		return 1;
	}

	if (actor->getTimeline() == NULL)
	{
        CIwGameError::LogError("Warning: actor.changeTimeline() actor has no timeline assigned");
		lua_pushboolean(lua, false);
		return 1;
	}

	unsigned int hash = IW_GAME_HASH(command);
	if (hash == CIwGameXomlNames::Play_Hash)
		actor->getTimeline()->play();
	else
	if (hash == CIwGameXomlNames::Stop_Hash)
		actor->getTimeline()->stop();
	else
	if (hash == CIwGameXomlNames::Pause_Hash)
		actor->getTimeline()->pause();
	else
	if (hash == CIwGameXomlNames::Restart_Hash)
		actor->getTimeline()->restart();
	else
	{
        CIwGameError::LogError("Warning: actor.changeTimeline() invalid command - ", command);
		lua_pushnil(lua);
		return 1;
	}

	lua_pushboolean(lua, true);

    return 1;
}

//
// Lua library
//
static const luaL_Reg g_actorlib[] =
{
	{"set",					LUA_SetPropertyActor}, 
	{"add",					LUA_AddPropertyActor}, 
	{"get",					LUA_GetPropertyActor}, 
	{"find",				LUA_FindActor}, 
	{"create",				LUA_CreateActor}, 
	{"destroy",				LUA_DestroyActor}, 
	{"clipped",				LUA_isActorClipped}, 
	{"overlaps",			LUA_ActorsOverlaps}, 
	{"distance",			LUA_getDistanceBetweenActors}, 
	{"anglediff",			LUA_getAngleBetweenActors}, 
	{"getStartContacts",	LUA_getStartContacts}, 
	{"getEndContacts",		LUA_getEndContacts}, 
	{"children",			LUA_getChildrenActor}, 
	{"changeTimeline",		LUA_changeTimelineActor}, 
	{NULL, NULL}
};

LUALIB_API int luaopen_actor(lua_State *L)
{
	luaL_register(L, "actor", g_actorlib);
  
	return 1;
}
