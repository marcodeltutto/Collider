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

#if !defined(_IW_GAME_SCRIPT_H_)
#define _IW_GAME_SCRIPT_H_

#include "s3e.h"
#include "IwList.h"
#include "IwGameXoml.h"

class CIwGameScene;

//
//
//
//
// IIwGameScriptEngine - A script VM
//
//
//
//
class IIwGameScriptEngine
{
public:
	// Proprties
protected:
	CIwGameScene*	Scene;
public:
	CIwGameScene*	getScene()						{ return Scene; }
	// Properties end

protected:
public:

	IIwGameScriptEngine() : Scene(NULL) {}
	virtual ~IIwGameScriptEngine() {}

	virtual int			Init(CIwGameScene* scene) = 0;
	virtual void		Release() = 0;
	virtual int			RunScript(const char* script, int script_len) = 0;
	virtual int			CallFunction(IIwGameXomlResource* object, CIwGameString* function_name, CIwGameString* param1, CIwGameString* param2, CIwGameString* param3) = 0;
	virtual CIwGameXomlProperty getVariable(CIwGameString* var_name) = 0;

	// Internal
};

//
//
//
//
// CIwGameScript - A script
//
//
//
//
class CIwGameScript : public IIwGameXomlResource
{
public:
	enum eIwGameScript_Type
	{
		ST_None,  
		ST_LUA, 
		ST_AngelScript, 
		ST_Javascript, 
		ST_Python, 
	};
	// Proprties
protected:
	eIwGameScript_Type		ScriptType;			// Script type
	char*					Script;				// The actual script
	int						ScriptLength;		// Length of script in bytes
public:
	void					setScriptType(eIwGameScript_Type type)	{ ScriptType = type; }
	eIwGameScript_Type		getScriptType() const					{ return ScriptType; }
	bool					setScript(const char* script, int script_len);
	const char*				getScript() const						{ return Script; }
	int						getScriptLength() const					{ return ScriptLength; }
	// Properties end

protected:
public:

	CIwGameScript() : IIwGameXomlResource(), ScriptType(ST_None), Script(NULL), ScriptLength(0)	{ setClassType("script"); }
	virtual ~CIwGameScript()
	{
		SAFE_DELETE(Script)
	}

	int					Init(const char* script, int script_len, CIwGameScene* scene = NULL);

	// Implementation of IIwGameXomlResource interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	// Internal
};

//
//  CIwGameScriptCreator - Creates an instance of a game script object
//
class CIwGameScriptCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameScriptCreator()
	{
		setClassName("script");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameScript(); }
};


#endif	// _IW_GAME_SCRIPT_H_
