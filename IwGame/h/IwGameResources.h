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

#if !defined(_IW_GAME_RESOURCES_H_)
#define _IW_GAME_RESOURCES_H_


#include "s3efile.h"
#include "IwGameString.h"
#include "IwGameUtil.h"
#include "IwGameXoml.h"
#include "IwGameScript.h"

class CIwGameActionsManager;
class CIwGameAnimTimelinesManager;
class CIwGameProgramManager;
class IIwGameScriptEngine;

// 
// 
//
//
// CIwGameResourceGroup - Wrapper class to allow Marmalade resource groups to be used as game resources
// 
// 
// 
//
class CIwGameResourceGroup : public IIwGameXomlResource
{
	// Properties
protected:
	CIwGameString					GroupFilename;			// Resource group file name
	CIwResGroup*					ResourceGroup;			// Marmalade resource group
	bool							Loaded;					// Loaded state of resource group
public:
	void							setGroupFilename(const char* group_file_name)	{ GroupFilename = group_file_name; }
	CIwGameString&					getGroupFilename()								{ return GroupFilename; }
	CIwResGroup*					getResourceGroup();
	// Properties End

public:
	CIwGameResourceGroup() : IIwGameXomlResource(), ResourceGroup(NULL), Loaded(false) { setClassType("resourcegroup"); }
	~CIwGameResourceGroup() { Destroy(); }

	bool							Load();
	void							Destroy();

	// Implementation of IIwGameXomlClass interface
	bool							LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//
//
//
//  CIwGameResourceGroupCreator - Creates an instance of a resource group ovject
//
//
//
//
class CIwGameResourceGroupCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameResourceGroupCreator()
	{
		setClassName("resourcegroup");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent);
};


// 
// 
//
//
// CIwGameGlobalResources - Manages application global resources
// 
// 
// 
//
class CIwGameGlobalResources
{
	CDEFINE_SINGLETON(CIwGameGlobalResources)

protected:
	// Properties
	CIwGameXomlResourceManager*		ResourceManager;		// Manages all types of resources
	CIwGameXomlVariableManager*		VariableManager;		// Manages XOML variables
	CIwGameActionsManager*			ActionsManager;			// Manages actions
	CIwGameAnimTimelinesManager*	TimelinesManager;		// Manages timelines
	CIwGameProgramManager*			ProgramManager;			// Manages the execution of a collection of programs
	IIwGameScriptEngine*			ScriptEngine;			// manages a script
public:
	CIwGameXomlResourceManager*		getResourceManager()				{ return ResourceManager; }
	CIwGameXomlVariableManager*		getVariableManager()				{ return VariableManager; }
	CIwGameActionsManager*			getActionsManager()					{ return ActionsManager; }
	CIwGameAnimTimelinesManager*	getTimelinesManager()				{ return TimelinesManager; }
	CIwGameProgramManager*			getProgrameManager()				{ return ProgramManager; }
	IIwGameScriptEngine*			getScriptEngine(CIwGameScript::eIwGameScript_Type type = CIwGameScript::ST_None);
	// Properties end

public:
	void					Init(IIwGameXomlResource* parent);
	void					Release();
};
#define IW_GAME_GLOBAL_RESOURCES		CIwGameGlobalResources::getInstance()


#endif	// _IW_GAME_RESOURCES_H_
