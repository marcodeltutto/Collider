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

#include "IwGameResources.h"
#include "IwGameUtil.h"
#include "IwGameString.h"
#include "IwGameAnim.h"
#include "IwGameImage.h"
#include "IwGameScene.h"
#include "IwGameActor.h"
#include "IwGameProgram.h"
#include "IwGameAnim.h"
#include "IwGameActions.h"
#include "IwGameScriptLua.h"


//
//
//
//
// CIwGameResourceGroup implementation
//
//
//
//
CIwResGroup* CIwGameResourceGroup::getResourceGroup()
{
	// If group has not been oaded then load it
	if (!Loaded)
		Load();

	return ResourceGroup;
}

bool CIwGameResourceGroup::Load()
{
	if (Loaded)
		return true;

	ResourceGroup = IwGetResManager()->LoadGroup(GroupFilename.c_str());
	if (ResourceGroup != NULL)
	{
		Loaded = true;
		return true;
	}

	return false;
}

void CIwGameResourceGroup::Destroy()
{
	if (Loaded)
	{
		IwGetResManager()->DestroyGroup(ResourceGroup);
		ResourceGroup = NULL;
		Loaded = false;
	}
}

bool CIwGameResourceGroup::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Process Actor attributes
	bool preload = false;
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
		{
			setName((*it)->GetValue().c_str());
		}
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::GroupFile_Hash)
		{
			setGroupFilename((*it)->GetValue().c_str());
		}
		else
		if (name_hash == CIwGameXomlNames::Preload_Hash)
		{
			preload = (*it)->GetValueAsBool();
		}
	}

	// Process inner tags
	if (!IW_GAME_XOML->Process(this, node))
		return false;

	// If we are declared inside a scene then animation is local to the scene
	CIwGameScene* scene = NULL;
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		scene = ((CIwGameActor*)parent)->getScene();
	else
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	bool ret = false;
	if (scene != NULL)
		ret = scene->getResourceManager()->addResource(this);
	else
		ret = IW_GAME_GLOBAL_RESOURCES->getResourceManager()->addResource(this);
	if (!ret)
		return false;

	// Preload the resorce group if requested
	if (preload)
		Load();

	return true;
}

//
//
//
//
// CIwGameResourceGroupCreator implementation
//
//
//
//
IIwGameXomlResource* CIwGameResourceGroupCreator::CreateInstance(IIwGameXomlResource* parent)
{
	return new CIwGameResourceGroup();
}



//
//
//
//
// CIwGameGlobalResources implementation
//
//
//
//

CDECLARE_SINGLETON(CIwGameGlobalResources)

void CIwGameGlobalResources::Init(IIwGameXomlResource* parent)
{
	ResourceManager = new CIwGameXomlResourceManager();
	ResourceManager->setParent(parent);
	VariableManager = new CIwGameXomlVariableManager();
	VariableManager->setParent(parent);
	ActionsManager = new CIwGameActionsManager();
	ActionsManager->setParent(parent);
	TimelinesManager = new CIwGameAnimTimelinesManager();
	TimelinesManager->setParent(parent);
	ProgramManager = new CIwGameProgramManager();
	ProgramManager->setParent(parent);
	ScriptEngine = NULL;
	getScriptEngine(CIwGameScript::ST_LUA);		// TODO: REMOVE eventually - For some reason if we create the global script engine when the script has been loaded froma file the Lua GC throws a wobbler! For now we just create it on init instead
}

void CIwGameGlobalResources::Release()
{
	SAFE_DELETE(ScriptEngine)
	SAFE_DELETE(VariableManager)
	SAFE_DELETE(ActionsManager)
	SAFE_DELETE(TimelinesManager)
	SAFE_DELETE(ResourceManager)
	SAFE_DELETE(ProgramManager)
}

IIwGameScriptEngine* CIwGameGlobalResources::getScriptEngine(CIwGameScript::eIwGameScript_Type type)
{
	if (ScriptEngine != NULL)
		return ScriptEngine;

	switch (type)
	{
	case CIwGameScript::ST_LUA:
		ScriptEngine = new CIwGameScriptEngineLua();
		ScriptEngine->Init(NULL);
		break;
	case CIwGameScript::ST_AngelScript:
		break;
	case CIwGameScript::ST_Javascript:
		break;
	case CIwGameScript::ST_Python:
		break;
	}

	return ScriptEngine;
}
