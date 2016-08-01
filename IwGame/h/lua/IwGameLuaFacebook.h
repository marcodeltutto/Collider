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

#if !defined(_IW_GAME_LUA_FACEBOOK_H_)
#define _IW_GAME_LUA_FACEBOOK_H_

#include "IwGameScript.h"

// LUA headers
extern "C"
{
   #include "lua.h"
   #include "lauxlib.h"
   #include "lualib.h"
}

LUALIB_API int luaopen_facebook(lua_State *L);



#endif	// _IW_GAME_LUA_FACEBOOK_H_
