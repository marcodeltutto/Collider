#if !defined(_GAME_H_)
#define _GAME_H_

//#include "IwGame.h"
#include "CzApp.h"
#include "IwGeomFMat.h" //marco addition

#define	GAME	Game::getInstance()

class EventManager;
class SaveFile;

//
//
// Game is a singleton class that acts as a controller for the whole game
// 
// 
//
class Game : public CzApp
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
        
    EventManager* eventManager;
    EventManager* getEventManager();
    SaveFile* saveFile;
    SaveFile* getSaveFile();
    CIwFMat rotMatrix;
    float zoom;

	// Implementation of IIwGameXomlResource interface
	int				LoadFromXoml(bool load_children, CzXmlNode* node);
	
};


#endif // _GAME_H_
