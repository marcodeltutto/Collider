#include "s3e.h"
#include "Iw2D.h"
#include "IwGx.h"
#include "IwResManager.h"

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

	// This is how you increase the size of the XML parsers memory pools
//	CIwGameXmlParser::PoolsDestroy();
//	CIwGameXmlParser::PoolsInit(4096, 4096, 8192);
	// Add custom classes to XOML system
//	IW_GAME_XOML->addClass(new InertActorCreator());

	// Add custom game actions to XOML system
//	for (int t = 0; t < GameXomlAction_Global::Action_Max; t++)
//		IW_GAME_XOML->addAction(new GameXomlAction_Global((GameXomlAction_Global::ActionType)t));

	// Add custom modifiers
//	IW_GAME_MODS->addModifier(new AddThrustModCreator());

	// Load common resuorces
	IW_GAME_XOML->Process(this, "Common.xml");

	// Load the UI style
	IW_GAME_XOML->Process(this, "UIStyle.xml");

	// Load Scen1
	IW_GAME_XOML->Process(this, "Scene1.xml");
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

