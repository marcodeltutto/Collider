#include "s3e.h"
#include "Iw2D.h"
#include "IwGx.h"
#include "IwGL.h"

#include "IwResManager.h"

#include "Game.h"
#include "IwGxScene.h"
#include "GameLogicScene.h"
#include "LiveScene.h"
#include "LandingScreen.h"
#include "WebBrowser.h"
#include "ClassificationGame.h"
#include "GAMarmalade.h"
#include "EventManager.h"
#include "s3eDevice.h"
#include <iostream>

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
  std::cout << "MARCO here 1." << std::endl;
  // Init base game
  CzApp::Init(enable_http, false);
  
  std::cout << "MARCO here 2." << std::endl;
  // This is how you increase the size of the XML parsers memory pools
  //	CIwGameXmlParser::PoolsDestroy();
  //	CIwGameXmlParser::PoolsInit(4096, 4096, 8192);
  // Add custom classes to XOML system
  //IW_GAME_XOML->addClass(new IwGxSceneCreator());
  CZ_XOML->addClass(new LiveSceneCreator());
  CZ_XOML->addClass(new ABoxFromXMLCreator());
  CZ_XOML->addClass(new ACylinderFromXMLCreator());
  CZ_XOML->addClass(new EndcapFromXMLCreator());
  CZ_XOML->addClass(new EventFromXMLCreator());
  CZ_XOML->addClass(new IwGxSceneCreator());
  CZ_XOML->addClass(new ClassificationGameCreator());
  CZ_XOML->addClass(new GameLogicSceneCreator());
  CZ_XOML->addClass(new LandingScreenCreator());
  CZ_XOML->addClass(new WebBrowserCreator());
  
  // Add custom game actions to XOML system
  //for (int t = 0; t < GameXomlAction_Global::Action_Max; t++)
  //IW_GAME_XOML->addAction(new GameXomlAction_Global((GameXomlAction_Global::ActionType)t));
  
  // Add custom modifiers
  //IW_GAME_MODS->addModifier(new AddThrustModCreator());
  
  // Initialise the http manager
  CzHttpManager::Create();
  CZ_HTTP_MANAGER->Init();
  
  GAMarmalade::init("UA-42555369-3", "12345678", "fictional.com");
  
  saveFile = FileUtils::loadSaveFile();
  
  //Setup Input to Facebook App
  //CIwGameFacebook::Create();
  //IW_GAME_FACEBOOK->Init();
  //IW_GAME_FACEBOOK->setAppID("141331202732781");
  
  eventManager = new EventManager();
  
  // Load common resuorces
  CZ_XOML->Process(this, "Common.xml");
  
  // Load the UI style
  CZ_XOML->Process(this, "UIStyle.xml");
  
  // Load Scene1
  //IW_GAME_XOML->Process(this, "Scene1.xml");
  
  // these contain the rotation and zoom configuration for the event display
  // defined here and referred to by pointers in the event display itself, allowing the state to be remembered
  rotMatrix.SetIdentity();
  zoom = 0.7;
  
  // Load SceneGL
  //IW_GAME_XOML->Process(this, "SceneGL.xml");
  CZ_XOML->Process(this, "hunt_higgs_game.xml");
  
}

void Game::Release()
{
  //Release Facebook API resources
  /*if (IW_GAME_FACEBOOK != NULL)
   {
   IW_GAME_FACEBOOK->Release();
   CIwGameFacebook::Destroy();
   }*/
  
  
  if (CZ_HTTP_MANAGER != NULL) {
    CZ_HTTP_MANAGER->Release();
    CzHttpManager::Destroy();
  }
  
  // Release game
  CzApp::Release();
  
  GAMarmalade::end();
  
  FileUtils::writeSaveFile();
  
  delete saveFile;
  delete eventManager;
}

bool Game::Update()
{
  
  
  //std::cout <<"!!!!!! Game::Update() !!!!!!!! isBackDown = "<<IW_GAME_INPUT->isKeyDown(s3eKeyAbsBSK)<<"\n";
  CZ_HTTP_MANAGER->Update();
  
  if (!CzApp::Update()){
    //std::cout <<"!!!!!!!!!!!!!!!!!!!! Game::Update() about to return false!!!\n";
    //CIwGameError::LogError("!!!!!!!!!!!!!!!!!!!! Game::Update() about to return false!!!");
    return false;
  }
  
  return true;
}

void Game::Draw()
{
  CzApp::Draw();
}

EventManager * Game::getEventManager() {
  return eventManager;
}

SaveFile * Game::getSaveFile() {
  return saveFile;
}

int Game::LoadFromXoml(bool load_children, CzXmlNode* node)
{
  // Process inner tags
  if (!CZ_XOML->Process(this, node))
    return false;
  
  return true;
}

