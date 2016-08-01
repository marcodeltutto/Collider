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

#include "IwGameScript.h"
#include "IwGameScene.h"
#include "IwGameActor.h"

//
//
//
//
// CIwGameScript implementation
//
//
//
//

bool CIwGameScript::setScript(const char* script, int script_len)
{
/*	SAFE_DELETE(Script)
	Script = new char [script_len];
	if (Script == NULL)
		return false;
	memcpy(Script, script, script_len);
	ScriptLength = script_len;*/

	return true;
}


int CIwGameScript::Init(const char* script, int script_len, CIwGameScene* scene)
{
//	if (!setScript(script, script_len))
//		return 1;

	if (scene == NULL)
	{
		IIwGameXomlResource* parent = Parent->getParent();
		if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
			scene = (CIwGameScene*)parent;
		else
		{
			if (IW_GAME_GLOBAL_RESOURCES->getScriptEngine(ScriptType) != NULL)
				return IW_GAME_GLOBAL_RESOURCES->getScriptEngine(ScriptType)->RunScript(script, script_len);
//				return IW_GAME_GLOBAL_RESOURCES->getScriptEngine(ScriptType)->RunScript(Script, ScriptLength);
		}
	}
	
	// Run the script
	if (scene != NULL && scene->getScriptEngine() != NULL)
		return scene->getScriptEngine()->RunScript(script, script_len);
//		return scene->getScriptEngine()->RunScript(Script, ScriptLength);

	return -1;
}

bool CIwGameScript::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Process image attributes
	CIwGameString* name = NULL;
	CIwGameString* file = NULL;
	CIwGameString* condition = NULL;

	CIwGameScene* scene = NULL;
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		scene = ((CIwGameActor*)parent)->getScene();
	else
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
		{
			name = &(*it)->GetValue();
			setName(name->c_str());
		}
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::File_Hash)
			file = &(*it)->GetValue();
		else
 		if (name_hash == CIwGameXomlNames::Type_Hash)
		{
			unsigned int type_hash = (*it)->GetValue().getHash();

			if (type_hash == IW_GAME_HASH("lua"))
				setScriptType(ST_LUA);
			else
				CIwGameError::LogError("Warning: Unsipported script format - ", (*it)->GetValue().c_str());
		}
		else
		if (name_hash == CIwGameXomlNames::Condition_Hash)
			condition = &(*it)->GetValue();
	}

	if (condition != NULL)
	{
		// Find the condition variable
		bool condition_not = false;
		CIwGameXomlVariable* var = NULL;
		if (*(condition->c_str()) == '!')
		{
			condition_not = true;
			CIwGameString cond = condition->c_str() + 1;
			var = CIwGameXomlVariable::GetVariable(cond, scene);
		}
		else
			var = CIwGameXomlVariable::GetVariable(*condition, scene);
		if (var != NULL)
		{
			bool res = var->isTrue();
			if (condition_not)
				res = !res;
			if (!res)
			{
				IW_GAME_XOML->setExitOnError(false);
				return false;
			}
		}
#if defined (_DEBUG)
		else
			CIwGameError::LogError("Warning: condition variable not found - ", condition->c_str());
#endif // _DEBUG 
	}

	// If we are declared inside a scene then script is local to the scene
	if (scene != NULL)
		return scene->getResourceManager()->addResource(this);
	else
		return IW_GAME_GLOBAL_RESOURCES->getResourceManager()->addResource(this);

	return true;
}




