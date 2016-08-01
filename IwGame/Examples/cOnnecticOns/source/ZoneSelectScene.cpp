/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#include "ZoneSelectScene.h"

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
// ZoneSelectScene implementation
//
//
//

int ZoneSelectScene::Init(int max_collidables, int max_layers, bool doSleep)
{
	CIwGameScene::Init(max_collidables, max_layers, doSleep);

	return 1;
}

void ZoneSelectScene::Update(float dt)
{
	CIwGameScene::Update(dt);
}


bool ZoneSelectScene::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Load the actor from XOML
	if (!CIwGameScene::LoadFromXoml(parent, load_children, node))
		return false;

	// Update zone completed states
	CIwGameString zone_name;
	for (int t = 0; t < GAME->getMaxZones(); t++)
	{
		if (!GAME->isZoneComplete(t))
		{
			zone_name = "ZoneComplete";
			zone_name += CIwGameString(t + 1); 

			// Find the text actor that holds the completed tick
			for (_Iterator it = begin(); it != end(); ++it)
			{
				if ((*it)->getNameHash() == zone_name.getHash())
				{
					(*it)->setVisible(false);
					break;
				}
			}
		}

		// If previous zone is complete then remove lock
		if (t == 0 || GAME->isZoneComplete(t - 1))
		{
			zone_name = "ZoneLocked";
			zone_name += CIwGameString(t + 1); 

			// Find the text actor that holds the lock image
			for (_Iterator it = begin(); it != end(); ++it)
			{
				if ((*it)->getNameHash() == zone_name.getHash())
				{
					(*it)->setVisible(false);
					break;
				}
			}

			zone_name = "Zone";
			zone_name += CIwGameString(t + 1); 
			// Find the zone selection actor and enable selection
			for (_Iterator it = begin(); it != end(); ++it)
			{
				if ((*it)->getNameHash() == zone_name.getHash())
				{
					(*it)->setTappable(true);
					break;
				}
			}
		}
	}

	// Remove custom zone lock
	{
		zone_name = "ZoneLocked10";

		// Find the text actor that holds the lock image
		for (_Iterator it = begin(); it != end(); ++it)
		{
			if ((*it)->getNameHash() == zone_name.getHash())
			{
				(*it)->setVisible(false);
				break;
			}
		}

		zone_name = "Zone10";
		// Find the zone selection actor and enable selection
		for (_Iterator it = begin(); it != end(); ++it)
		{
			if ((*it)->getNameHash() == zone_name.getHash())
			{
				(*it)->setTappable(true);
				break;
			}
		}
	}

	return true;
}

