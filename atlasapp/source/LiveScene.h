//
//  LiveScene.h
//  GameSceneGL
//
//  Created by Tom McLaughlan on 24/05/2013.
//
//

#ifndef _LIVE_SCENE_H_
#define _LIVE_SCENE_H_

#include "Game.h"
//#include "IwGameScene.h"
#include "CzScene.h"
//#include "IwGameUI.h"
#include "CzUIBase.h"
#include "CzUILabel.h"
#include "EventManager.h"
#include "FileUtils.h"
#include <iostream>

/*
 * The LiveScene contains a few methods to set event date/time, load new live stream events, etc.
 *
 **/

class LiveScene : public CzScene
{
public:
    virtual ~LiveScene() {}
    
    int		Init(int max_collidables = 128, int max_layers = 10, bool doSleep = true);
	void	Update(float dt);
	void	Draw()
	{
		CzScene::Draw();
	}
    
    void InitLinks();
  
    // Implementation of IIwGameXomlResource interface
	int	LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node)
	{
		if (!CzScene::LoadFromXoml(parent/*Parent*/, load_children, node))
			return false;
        
		// Add our own custom attribute parsing
		for (CzXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
		{
			unsigned int name_hash = (*it)->getName().getHash();
		}
        
		return true;
	}
    
private:
    EventManager* eventManager;
    CzUILabel* refreshButton;
    CzUILabel* nextButton;
    CzUILabel* prevButton;
    CzUILabel* dateLabel;
};

//
//  LiveSceneCreator - Creates an instance the 'GameScene'
//
class LiveSceneCreator : public IzXomlClassCreator
{
public:
	LiveSceneCreator()
	{
		setClassName("LiveScene");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new LiveScene(); }
};

#endif /* defined(_LIVE_SCENE_H_) */
