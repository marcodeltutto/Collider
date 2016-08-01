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

	// Create a scene then add it to the game
	CIwGameScene* scene = new CIwGameScene();
	scene->Init();
	scene->setName("MyScene");
	scene->setVirtualTransform(320, 480, 0, false, false);
	addScene(scene);

	// Create and load resource group that contains our font
	CIwGameResourceGroup* group = new CIwGameResourceGroup();
	group->setGroupFilename("Common.group");
	group->Load();
	scene->getResourceManager()->addResource(group);	// Add to scenes resource manager so that it is auto cleaned up

	// Create font
	CIwGameFont* font = new CIwGameFont();
	font->setName("trebuchet_12");
	font->Init("trebuchet_12", group->getResourceGroup());
	scene->getResourceManager()->addResource(font);		// Add to scenes resource manager so that it is auto cleaned up

	// Create a text actor and add it to the scene
	CIwGameActorText* text = new CIwGameActorText();
	scene->addActor(text);
	CIwRect rect = CIwRect(-100, -20, 200, 40); 
	CIwGameString string = "Hello World";
	text->Init(font, rect, string, 0);
	text->setColour(0xff, 0xff, 0xff, 0xff);
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

