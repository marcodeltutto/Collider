#if !defined(_GAME_H_)
#define _GAME_H_

#include "IwGame.h"

#define	GAME	Game::getInstance()

//
//
// Game is a singleton class that acts as a controller for the whole game
// 
// 
//
class Game : public CIwGame
{
	CDEFINE_SINGLETON(Game)

public:

protected:
	//// Properties

public:
	//// Properties end

protected:
	bool			PrevTouched;
	void			LoadResources();

public:
	void			Init(bool enable_http);
	void			Release();
	bool			Update();
	void			Draw();
	void			Save();
	void			Load();

	// Implementation of IIwGameXomlResource interface
	bool				LoadFromXoml(bool load_children, CIwGameXmlNode* node);
	
	// Ad engine test
	void			AdTest_Init();
	void			AdTest_Release();
	void			AdTest_Update();

};

#endif // _GAME_H_
