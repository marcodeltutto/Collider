/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#include "Game.h"


CDECLARE_SINGLETON(Game)

//
//
//
// Game singleton implementation
//
//
//

void Game::Init(bool enable_http)
{
	// Init base game
	CIwGame::Init(enable_http);

	// Load and run the HelloWorld scene
	IW_GAME_XOML->Process(this, "HelloWorld.xml");
}

void Game::Release()
{
	// Release game
	CIwGame::Release();
}

bool Game::Update()
{
	if (!CIwGame::Update())
		return false;

	return true;
}

void Game::Draw()
{
	CIwGame::Draw();
}


bool Game::LoadFromXoml(bool load_children, CIwGameXmlNode* node)
{
	// Process inner tags
	if (!IW_GAME_XOML->Process(this, node))
		return false;

	return true;
}

