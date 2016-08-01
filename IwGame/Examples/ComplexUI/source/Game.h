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
	// Properties

public:
	// Properties end

protected:

public:
	void			Init(bool enable_http);
	void			Release();
	bool			Update();
	void			Draw();

	// Implementation of IIwGameXomlResource interface
	bool				LoadFromXoml(bool load_children, CIwGameXmlNode* node);
	
};


#endif // _GAME_H_
