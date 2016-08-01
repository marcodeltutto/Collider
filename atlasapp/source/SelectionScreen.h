//
//  GameLogicScene.h
//  GameSceneGL
//
//  Created by Tom McLaughlan on 24/05/2013.
//
//

#ifndef _SELECTION_SCREEN_H_
#define _SELECTION_SCREEN_H_

#include "Game.h"
#include "IwGameScene.h"
#include "IwGameUI.h"
#include "EventManager.h"
#include "IwGameUIListBox.h"
#include "FileUtils.h"
#include <iostream>
#include <vector>
#include <string>

/*
 * This scene controls the main Landing Screen
 * The main functionality of the class is to handle downloads
 * Most other things are inherited from CIwGameScene
 **/

class LandingScreen : public CIwGameScene
{
public:
    virtual ~LandingScreen() {}
    
    int		Init(int max_collidables = 128, int max_layers = 10, bool doSleep = true);
	void	Update(float dt);
	void	Draw()
	{
		CIwGameScene::Draw();
	}
    
    void InitLinks();
    void CheckForDownloads();
    void UpdateDownloadButtons();
    bool downloaded;
    bool toupdate;
    
    // Implementation of IIwGameXomlResource interface
	bool	LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
	{
		if (!CIwGameScene::LoadFromXoml(parent, load_children, node))
			return false;
        
		// Add our own custom attribute parsing
		for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
		{
			unsigned int name_hash = (*it)->getName().getHash();
		}
        
		return true;
	}
    
private:
    CIwGameScene* selectionScene;
    std::vector<std::string> av;
    
};

//
//  LandingScreenCreator - Creates an instance the 'LandingScreen'
//
class LandingScreenCreator : public IIwGameXomlClassCreator
{
public:
	LandingScreenCreator()
	{
		setClassName("LandingScreen");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new LandingScreen(); }
};

#endif /* defined(_SELECTION_SCREEN_H_) */
