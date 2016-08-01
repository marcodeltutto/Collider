/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#include "LevelSelectScene.h"

#include "IwGameCamera.h"
#include "IwGameInput.h"
#include "IwGame.h"
#include "IwGameActorParticles.h"

#include "Game.h"

//
// XOML paramaters
// ---------------
//


//
//
//
// LevelSelectScene implementation
//
//
//

int LevelSelectScene::Init(int max_collidables, int max_layers, bool doSleep)
{
	CIwGameScene::Init(max_collidables, max_layers, doSleep);

	return 1;
}

void LevelSelectScene::Update(float dt)
{
	CIwGameScene::Update(dt);
}


bool LevelSelectScene::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Load the actor from XOML
	if (!CIwGameScene::LoadFromXoml(parent, load_children, node))
		return false;

	// Get current zone
	int zone = 0;
	CIwGameXomlVariableInt* zone_var = (CIwGameXomlVariableInt*)IW_GAME_GLOBAL_RESOURCES->getVariableManager()->findVariable("Zone");
	if (zone_var != NULL)
		zone = zone_var->NativeValue;

	// Update record scores and hide tick if level not complete
	CIwGameString round_name;
	CIwGameString record;
	CIwGameString score;
	CIwGameString lock_name;
	for (int t = 0; t < MAX_RECORDS; t++)
	{
		record = "Record";
		record += CIwGameString(t + 1); 

		// Find the text actor that holds the record score
		for (_Iterator it = begin(); it != end(); ++it)
		{
			if ((*it)->getNameHash() == record.getHash())
			{
				score = GAME->getRecord(t + (zone - 1) * 10);
				((CIwGameActorText*)*it)->setText(score.c_str());
				break;
			}
		}

		if (!GAME->isCompleted((zone - 1) * 10 + t))
		{
			round_name = "LevelComplete";
			round_name += CIwGameString(t + 1); 

			// Find the text actor that holds the completed tick
			for (_Iterator it = begin(); it != end(); ++it)
			{
				if ((*it)->getNameHash() == round_name.getHash())
				{
					(*it)->setVisible(false);
					break;
				}
			}
		}
	}

	return true;
}

