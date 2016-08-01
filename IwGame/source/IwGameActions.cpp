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

#include "IwGameActions.h"
#include "IwGameScene.h"
#include "IwGameResources.h"
#include "IwGameActor.h"
#include "IwGame.h"

//#define _DEBUG_ACTIONS

//
//
//
//
// CIwGameActions implementation
//
//
//
//
CIwGameString& CIwGameAction::getParameter1(IIwGameXomlResource* parent)
{
//	if (Variables[0] == NULL)
//		return Params[0];

	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[0], parent);
	if (var != NULL)
		return var->getValue();

	return Params[0];
}
CIwGameString& CIwGameAction::getParameter2(IIwGameXomlResource* parent)
{
	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[1], parent);
	if (var != NULL)
		return var->getValue();

	return Params[1];
}
CIwGameString& CIwGameAction::getParameter3(IIwGameXomlResource* parent)
{
	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[2], parent);
	if (var != NULL)
		return var->getValue();

	return Params[2];
}
CIwGameString& CIwGameAction::getParameter4(IIwGameXomlResource* parent)
{
	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[3], parent);
	if (var != NULL)
		return var->getValue();

	return Params[3];
}
CIwGameString& CIwGameAction::getParameter5(IIwGameXomlResource* parent)
{
	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[4], parent);
	if (var != NULL)
		return var->getValue();

	return Params[4];
}


void CIwGameActions::Execute(IIwGameXomlResource* source)
{
#if defined (_DEBUG_ACTIONS)
	CIwGameError::LogError("Running action group - ", Name.c_str());
#endif // _DEBUG
	if (Conditions != 0)
	{
		// Find the condition variable
		CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(_Conditions, source);
		if (var != NULL)
		{
			bool res = var->isTrue();
			if (Not)
				res = !res;
			if (!res)
				return;
		}
#if defined (_DEBUG)
		else
			CIwGameError::LogError("Warning: condition variable not found - ", _Conditions.c_str());
#endif // _DEBUG
	}

	for (_Iterator it = begin(); it != end(); ++it)
	{
		CIwGameAction*	action = *it;
		unsigned int	method_name = action->getMethodName();
		bool			condition = true;

#if defined (_DEBUG_ACTIONS)
		CIwGameError::LogError("- Running action - ", action->_MethodName.c_str());
#endif // _DEBUG

		// If action has a condition then check that the condition is true before executing the action
		if ((*it)->getConditions() != 0)
		{
			// Find the condition variable
			CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable((*it)->_Conditions, source);

			if (var != NULL)
			{
				condition = var->isTrue();
				if (action->Not)
					condition = !condition;
			}
#if defined (_DEBUG)
			else
				CIwGameError::LogError("Error: condition variable not found - ", (*it)->_Conditions.c_str());
#endif // _DEBUG
		}
		if (condition)
		{
			// Search for action
			for (CIwGameXoml::_ActionsIterator it2 = IW_GAME_XOML->actions_begin(); it2 != IW_GAME_XOML->actions_end(); ++it2)
			{
				if ((*it)->getMethodName() == (*it2)->getActionNameHash())
				{
					// Execute action
					(*it2)->Execute(source, *it);
					break;
				}
			}
		}
	}
}

bool CIwGameActions::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	bool local = true;

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
			setName((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Condition_Hash)
		{
			if (*((*it)->GetValue().c_str()) == '!')
			{
				Not = true;
				_Conditions = (*it)->GetValue().c_str() + 1;
				Conditions = _Conditions.getHash();
			}
			else
				_Conditions = (*it)->GetValue().c_str();
			Conditions = _Conditions.getHash();
		}
		else
		if (name_hash == CIwGameXomlNames::Local_Hash)
			local = (*it)->GetValueAsBool();
	}

	// Load in nodes
	for (CIwGameXmlNode::_Iterator it2 = node->begin(); it2 != node->end(); ++it2)
	{
		unsigned int name_hash = (*it2)->GetName().getHash();
		if (name_hash == CIwGameXomlNames::Action_Hash)
		{
			CIwGameAction* action = new CIwGameAction();
			// Get action data
			for (CIwGameXmlNode::_AttribIterator it = (*it2)->attribs_begin(); it != (*it2)->attribs_end(); ++it)
			{
				unsigned int attrib_hash = (*it)->getName().getHash();

				if (attrib_hash == CIwGameXomlNames::Method_Hash)
				{
					(*it)->GetValue().ToLower();
					action->setMethodName((*it)->GetValue().c_str());
				}
				else
				if (attrib_hash == CIwGameXomlNames::Param1_Hash || attrib_hash == CIwGameXomlNames::P1_Hash)
					action->setParameter1((*it)->GetValue());
				else
				if (attrib_hash == CIwGameXomlNames::Param2_Hash || attrib_hash == CIwGameXomlNames::P2_Hash)
					action->setParameter2((*it)->GetValue());
				else
				if (attrib_hash == CIwGameXomlNames::Param3_Hash || attrib_hash == CIwGameXomlNames::P3_Hash)
					action->setParameter3((*it)->GetValue());
				else
				if (attrib_hash == CIwGameXomlNames::Param4_Hash || attrib_hash == CIwGameXomlNames::P4_Hash)
					action->setParameter4((*it)->GetValue());
				else
				if (attrib_hash == CIwGameXomlNames::Param5_Hash || attrib_hash == CIwGameXomlNames::P5_Hash)
					action->setParameter5((*it)->GetValue());
				else
				if (attrib_hash == CIwGameXomlNames::Condition_Hash)
				{
					if (*((*it)->GetValue().c_str()) == '!')
					{
						action->Not = true;
						action->setConditions((*it)->GetValue().c_str() + 1);
					}
					else
						action->setConditions((*it)->GetValue().c_str());
				}
			}
			// Save action
			Actions.push_back(action);
		}
	}

	// If we are declared inside a scene or actor then actions are local to the scene
	if (parent != NULL && (parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash || parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash))
	{
		if (parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
			((CIwGameScene*)parent)->getActionsManager()->addActions(this);
		else
		{
			if (local)
				((CIwGameActor*)parent)->getActionsManager()->addActions(this);
			else
				((CIwGameActor*)parent)->getScene()->getActionsManager()->addActions(this);
		}
	}
	else
	{
		IW_GAME_GLOBAL_RESOURCES->getActionsManager()->addActions(this);
	}

	return true;
}


// 
// 
//
//
// CIwGameActionsManager implementation
//
//
//
//

void CIwGameActionsManager::addActions(CIwGameActions* actions)
{
#if defined(IW_REMOVE_DUPLICATES)
	CIwGameActions* acts = findActions(actions->getNameHash());
	if (acts != NULL)
	{
		CIwGameError::LogError("Warning: Actions already exists, actions list was replaced - ", actions->getName().c_str());
		removeActions(acts);
	}
#endif

	Actions.push_back(actions);
}

void CIwGameActionsManager::removeActions(CIwGameActions* actions)
{
	removeActions(actions->getNameHash());
}

void CIwGameActionsManager::removeActions(unsigned int name_hash)
{
	// Renove named Actions from the manager
	for (_Iterator it = Actions.begin(); it != Actions.end(); ++it)
	{
		if (name_hash == (*it)->getNameHash())
		{
			delete *it;
			Actions.erase(it);
			break;
		}
	}
}

CIwGameActions* CIwGameActionsManager::findActions(unsigned int name_hash)
{
	for (_Iterator it = Actions.begin(); it != Actions.end(); ++it)
	{
		if ((*it)->getNameHash() == name_hash)
			return *it;
	}

	return NULL;
}

CIwGameActions* CIwGameActionsManager::findActions(const char* name)
{
	return findActions(CIwGameString::CalculateHash(name));
}

void CIwGameActionsManager::clearActionss()
{
	// Remove and delete all actions from the manager
	for (_Iterator it = Actions.begin(); it != Actions.end(); ++it)
		delete *it;
	Actions.clear();
}

CIwGameActions* CIwGameActionsManager::FindActions(const char* name, IIwGameXomlResource* container)
{
	return FindActions(IW_GAME_HASH(name), container);
}

CIwGameActions* CIwGameActionsManager::FindActions(unsigned int name_hash, IIwGameXomlResource* container)
{
	CIwGameActions* action = NULL;
	
	if (container != NULL)
	{
		// If container class is an actor then check the actors scenes actions manager
		if (container->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		{
			action = ((CIwGameActor*)container)->getActionsManager()->findActions((name_hash));

			// If not found in the actor then check the actors scene
			if (action == NULL)
				container = ((CIwGameActor*)container)->getScene();
		}

		// If container class is a scene then check the scenes actions manager
		if (action == NULL && container->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
			action = ((CIwGameScene*)container)->getActionsManager()->findActions((name_hash));
	}

	// If the actions has no container or the actions was not found in the supplied container then check the global actions manager
	if (action == NULL)
		action = (CIwGameActions*)IW_GAME_GLOBAL_RESOURCES->getActionsManager()->findActions((name_hash));

	return action;
}

CIwGameActions*	CIwGameActionsManager::FindActions(const char* name, CIwGameScene* container, const char* actor_name)
{
	return FindActions(IW_GAME_HASH(name), container, IW_GAME_HASH(actor_name));
}

CIwGameActions* CIwGameActionsManager::FindActions(unsigned int name_hash, CIwGameScene* scene, unsigned int actor_hash)
{
	CIwGameActions* action = NULL;
	
	if (scene != NULL)
	{
		CIwGameActor* actor = scene->findActor(actor_hash);
		if (actor != NULL)
			action = actor->getActionsManager()->findActions((name_hash));
		else
			action = scene->getActionsManager()->findActions((name_hash));
	}

	if (action == NULL)
		action = (CIwGameActions*)IW_GAME_GLOBAL_RESOURCES->getActionsManager()->findActions((name_hash));

	return action;
}

