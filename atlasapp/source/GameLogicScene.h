//
//  GameLogicScene.h
//  GameSceneGL
//
//  Created by Tom McLaughlan on 24/05/2013.
//
//

#ifndef _GAMELOGIC_SCENE_H_
#define _GAMELOGIC_SCENE_H_

#include "Game.h"
//#include "IwGameScene.h"
#include "CzScene.h"
//#include "IwGameUI.h"
#include "CzUIBase.h"
#include "EventManager.h"
//#include "IwGameUIListBox.h"
#include "CzUIListBox.h"
#include "CzUILabel.h"
#include "FileUtils.h"
#include <iostream>

/*
 * The GameLogicScene contains all of the game logic for the event scanning activity
 * It controls the buttons and results of the scanning game, as well as the tutorial
 *
 **/

class GameLogicScene : public CzScene
{
public:
    virtual ~GameLogicScene() {
        /*marco CzXomlVariableBool* checkResults = (CzXomlVariableBool*) this->getVariableManager()->findVariable("CheckResults");
        if(checkResults->NativeValue)
            gameState.toCheck = true;
        */
         GAME->getSaveFile()->insertGameState(gameState);
        FileUtils::writeSaveFile();
        choiceListBox = 0;
        std::cout << "GameLogicScene Destructor" << std::endl;
    }
    
    int		Init(int max_collidables = 128, int max_layers = 10, bool doSleep = true);
	void	Update(float dt);
	void	Draw()
	{
      CzScene::Draw(); // instead of CzScene::Draw()
	}
    void Release() {
        /*marco CzXomlVariableBool* checkResults = (CzXomlVariableBool*) this->getVariableManager()->findVariable("CheckResults");
        if(checkResults->NativeValue)
            gameState.toCheck = true;
        */
        GAME->getSaveFile()->insertGameState(gameState);
        FileUtils::writeSaveFile();
        choiceListBox = 0;
        std::cout << "GameLogicScene Release" << std::endl;
    }
    
    void InitLinks();
    void UpdateCheckResultsButton();
    void UpdateChoices();
    void UpdateTutorialChoices();
    void UpdateMassChoices();
    void CheckSelection();
    void CheckTutorialSelection();
    void CheckMassSelection();
    
    CzUIListBox * choiceListBox;
    CzUILabel * resultLabel;
    CzUILabel * checkButton;
    int currentEvent;
    bool firstUpdate;
    bool postResult;
    
    bool checkResults;
    
    // Implementation of IIwGameXomlResource interface
	int	LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node)
	{
		if (!CzScene::LoadFromXoml(parent, load_children, node))
			return false;
        
		// Add our own custom attribute parsing
		for (CzXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
		{
			unsigned int name_hash = (*it)->getName().getHash();
		}
        
		return true;
	}
    
private:
    GameState gameState;
    int gameId;
    int gameScore;
    bool isTutorial;
    EventManager* eventManager;
//    CIwGameXomlVariableArray* originalChoices;
//    CIwGameXomlVariableArray* currentChoices;
    
    void newGame();
    void loadGame();
};

//
//  GameLogicSceneCreator - Creates an instance the 'GameScene'
//
class GameLogicSceneCreator : public IzXomlClassCreator
{
public:
	GameLogicSceneCreator()
	{
		setClassName("GameLogicScene");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new GameLogicScene(); }
};

#endif /* defined(_GAMELOGIC_SCENE_H_) */
