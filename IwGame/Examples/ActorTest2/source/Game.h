/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

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
	bool			LoadFromXoml(bool load_children, CIwGameXmlNode* node);

};



#endif // _GAME_H_
