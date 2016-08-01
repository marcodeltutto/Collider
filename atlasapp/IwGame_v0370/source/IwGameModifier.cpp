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

#include "IwGameModifier.h"
#include "IwGameScene.h"
#include "IwGameActor.h"

// 
// 
//
//
// CIwGameModifierManager implementation
//
//
//
//

bool CIwGameModifierManager::addModifier(IIwGameModifier* modifier)
{
#if defined(_DEBUG)
	if (findModifier(modifier->getNameHash()))
	{
		CIwGameError::LogError("Warning: Modifier already exists in modifier list - ", modifier->getName().c_str());
		return false;
	}
#endif	//(_DEBUG)
	Modifiers.push_back(modifier);
	modifier->setParent(this);

	return true;
}

bool CIwGameModifierManager::removeModifier(IIwGameModifier* modifier)
{
	for (_Iterator it = Modifiers.begin(); it != Modifiers.end(); ++it)
	{
		if (*it == modifier)
		{
			delete *it;
			Modifiers.erase(it);
			return true;
		}
	}

	return false;
}

IIwGameModifier* CIwGameModifierManager::findModifier(unsigned int name_hash)
{
	for (_Iterator it = Modifiers.begin(); it != Modifiers.end(); ++it)
	{
		if ((*it)->getNameHash() == name_hash)
			return *it;
	}

	return NULL;
}

IIwGameModifier* CIwGameModifierManager::findModifier(const char* name)
{
	return findModifier(CIwGameString::CalculateHash(name));
}

void CIwGameModifierManager::clearModifiers()
{
	// Remove and delete all images from the manager
	for (_Iterator it = Modifiers.begin(); it != Modifiers.end(); ++it)
		delete *it;

	Modifiers.clear();
}

void CIwGameModifierManager::Execute(IIwGameXomlResource* target, float dt)
{
	if (Modifiers.size() == 0)
		return;

	CIwList<IIwGameModifier*>	Removals;

	for (_Iterator it = begin(); it != end(); ++it)
	{
		if ((*it)->isActive())
		{
			if (!(*it)->isInitialised())
			{
				(*it)->InitModifier(target);
				(*it)->setInitialised(true);
			}
			else
			{
				if (!(*it)->UpdateModifier(target, dt))
				{
					(*it)->ReleaseModifier(target);
					Removals.push_back(*it);
				}
			}
		}
	}

	// Remove deleted modifiers
	for (_Iterator it = Removals.begin(); it != Removals.end(); ++it)
		removeModifier(*it);
}

bool CIwGameModifierManager::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash)
	{
		CIwGameError::LogError("Error: XOML - Modifiers list needs to be declared inside an actor or scene");
		return false;
	}
	
	if (parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
	{
		CIwGameActor* actor = (CIwGameActor*)parent;
		actor->setModifiers(this);
	}
	else
	if (parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
	{
		CIwGameScene* scene = (CIwGameScene*)parent;
		scene->setModifiers(this);
	}

	// Process modifiers list attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
		{
			setName((*it)->GetValue().c_str());
		}
	}

	// Prrocess modifiers
	for (CIwGameXmlNode::_Iterator it2 = node->begin(); it2 != node->end(); ++it2)
	{
		unsigned int name_hash = (*it2)->GetName().getHash();
		if (name_hash == CIwGameXomlNames::Modifier_Hash)
		{
			CIwGameString	name;
			CIwGameString	params[4];
			bool			active = true;

			for (CIwGameXmlNode::_AttribIterator it = (*it2)->attribs_begin(); it != (*it2)->attribs_end(); ++it)
			{
				unsigned int attrib_hash = (*it)->getName().getHash();

				if (attrib_hash == CIwGameXomlNames::Name_Hash)
					name = (*it)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Active_Hash)
					active = (*it)->GetValueAsBool();
				else
				if (attrib_hash == CIwGameXomlNames::Param1_Hash)
					params[0] = (*it)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Param2_Hash)
					params[1] = (*it)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Param3_Hash)
					params[2] = (*it)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Param4_Hash)
					params[3] = (*it)->GetValue();
			}

			// Find the modifiers creator
			IIwGameModifierCreator* creator = IW_GAME_MODS->findCreator(name.getHash());
			if (creator != NULL)
			{
				IIwGameModifier* mod = creator->CreateInstance();
				if (mod != NULL)
				{
					mod->setName(name.c_str());
					mod->setActive(active);
					for (int t = 0; t < 4; t++)
						mod->setParameter(t, params[t]);
					addModifier(mod);
				}
			}
			else
				CIwGameError::LogError("Warning: XOML - Modifier not found - ", name.c_str());

		}
	}

	return true;
}

//
// 
// 
//
// CIwGameMods Implementation
// 
// 
// 
//
CDECLARE_SINGLETON(CIwGameMods)

IIwGameModifierCreator* CIwGameMods::findCreator(unsigned int name_hash)
{
	for (_Iterator it = ModifierCreators.begin(); it != ModifierCreators.end(); ++it)
	{
		if ((*it)->getClassNameHash() == name_hash)
			return *it;
	}

	return NULL;
}

IIwGameModifierCreator* CIwGameMods::findCreator(const char* name)
{
	return findCreator(CIwGameString::CalculateHash(name));
}

void CIwGameMods::Init()
{
}

void CIwGameMods::Release()
{
	// Clean up modifier creators
	for (_Iterator it = begin(); it != end(); ++it)
	{
		delete *it;
	}
	ModifierCreators.clear();
}
