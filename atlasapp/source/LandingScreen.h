//
//  GameLogicScene.h
//  GameSceneGL
//
//  Created by Tom McLaughlan on 24/05/2013.
//
//

#ifndef _LANDING_SCREEN_H_
#define _LANDING_SCREEN_H_

#include "Game.h"
//#include "IwGameScene.h"
#include "CzScene.h"
//#include "IwGameUI.h"
#include "CzUIBase.h"
#include "EventManager.h"
//#include "IwGameUIListBox.h"
#include "CzUIListBox.h"
#include "FileUtils.h"
#include <iostream>
#include <vector>
#include <string>

/*
 * This scene controls the main Landing Screen
 * The main functionality of the class is to handle downloads
 * Most other things are inherited from CIwGameScene
 **/
static void facebookCallback(struct s3eFBDialog* dialog, s3eResult* streamResult, void* userData);

class s3eFBSession;

class LandingScreen : public CzScene
{
public:
    virtual ~LandingScreen() {}
    
    int		Init(int max_collidables = 128, int max_layers = 10, bool doSleep = true);
	void	Update(float dt);
	void	Draw()
	{
		CzScene::Draw();
	}
    
    void InitLinks();
    void CheckForDownloads();
    void UpdateDownloadButtons();
    void UpdateSelectionScreen();
    
    bool downloaded;
    bool toupdate;
    
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
    CzScene* selectionScene;
    CzScene* congratsScene;
    CzScene* menuScene;
    std::vector<std::string> av;
    CzXomlVariableManager* varMan;
    CzXomlVariable* gameId_var;
    CzXomlVariableBool* resetGame;
    EventManager* eventManager;
    int currentGameId;
    s3eFBSession* fbSession;
    
};

//
//  LandingScreenCreator - Creates an instance the 'LandingScreen'
//
class LandingScreenCreator : public IzXomlClassCreator
{
public:
	LandingScreenCreator()
	{
		setClassName("LandingScreen");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new LandingScreen(); }
};

#endif /* defined(_LANDING_SCREEN_H_) */
