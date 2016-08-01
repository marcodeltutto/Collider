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

#if !defined(_IW_GAME_SCRIPT_LUA_H_)
#define _IW_GAME_SCRIPT_LUA_H_

#include "IwGameScript.h"

// LUA headers
extern "C"
{
   #include "lua.h"
   #include "lstate.h"
   #include "lauxlib.h"
   #include "lualib.h"
}


//
//
//
//
// CIwGameScriptLua - LUA script 
//
//
//
//
class CIwGameScriptEngineLua : public IIwGameScriptEngine
{
public:
	// Proprties
protected:
	lua_State*		Lua;
public:
	// Properties end

protected:
public:

	CIwGameScriptEngineLua() : IIwGameScriptEngine(), Lua(NULL) {}
	~CIwGameScriptEngineLua()
	{
		Release();
	}

	int				Init(CIwGameScene* scene);
	void			Release();
	int				RunScript(const char* script, int script_len);
	int				CallFunction(IIwGameXomlResource* object, CIwGameString* function_name, CIwGameString* param1, CIwGameString* param2, CIwGameString* param3);
	CIwGameXomlProperty getVariable(CIwGameString* var_name);

	// Internal
};




#endif	// _IW_GAME_SCRIPT_LUA_H_