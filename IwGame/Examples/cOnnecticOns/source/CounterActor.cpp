/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#include "IwGameActor.h"
#include "IwGameScene.h"
#include "IwGameTemplates.h"
#include "IwGameXml.h"

#include "Game.h"
#include "GameScene.h"
#include "CounterActor.h"
#include "ConnectActor.h"


//
// XOML paramaters
// ---------------
// Counter CounterColour
// CounterColour: blue, green, orange, purple, red, yellow
//
// Spawn from Template
// -------------------
// base - Number to add to the name to create unique named counters
// pos - vec2 that specifies spawn position
// depth - visuals 3D depth
// countercolour - Colour of counter (blue, green, orange, purple, red, yellow)
//

//
//
//
// CounterActor implementation
//
//
//
CounterActor::~CounterActor()
{
	if (Particles != NULL)
		Scene->removeActor(Particles);
	if (Connector != NULL)
		Scene->removeActor(Connector);
}

void CounterActor::setSelected(bool selected)
{
	Selected = selected;
}

void CounterActor::BreakLink()
{
	LinkedCounter = NULL;
	RemoveConnector();
	if (Particles != NULL)
	{
		Scene->removeActor(Particles);
		Particles = NULL;
	}
	Selected = false;
}


bool CounterActor::Init(CIwGameImage* image, int width, int height)
{
	if (!CIwGameActorImage::Init(image, width, height))
		return false;

	CounterColour = CC_Blue;
	IsFlying = false;
	IsDying = false;
	Score = 100;
	Selected = false;
	LinkedCounter = NULL;
	Particles = NULL;
	Connector = NULL;
	Health = 1;
	MouthActor = NULL;

	return true;
}

bool CounterActor::Update(float dt)
{
	if (IsDying)
		return false;
	
	if (Particles != NULL)
	{
		Particles->setPosition(Position.x, Position.y);
		Particles->setAngle(Angle);
		Particles->setDepth(Depth);
	}

	if (MouthActor == NULL)
	{
		// Find mouth actor
		for (CIwGameScene::_Iterator it = Scene->begin(); it != Scene->end(); ++it)
		{
			if ((*it)->getLinkedTo() == this)
			{
				MouthActor = *it;
				break;
			}
		}
	}

	return CIwGameActorImage::Update(dt);
}

void CounterActor::RemoveConnector()
{
	if (Connector != NULL)
	{
		Scene->removeActor(Connector);
		Connector = NULL;
		if (LinkedCounter != NULL)
		{
			LinkedCounter->RemoveConnector();
		}
		((GameScene*)Scene)->setConnecticons(((GameScene*)Scene)->getConnecticons() + 1);
	}
}

void CounterActor::OnTapped(int index, int x, int y, bool allow_bubble)
{
	CIwGameActorImage::OnTapped(index, x, y, allow_bubble);

	// Prevent connections when shoot timer has started
	if (((GameScene*)Scene)->hasShootTimerStarted())
		return;

	IW_GAME_AUDIO->PlaySound("select");

	CounterActor* last_selected = ((GameScene*)Scene)->getLastSelectedCounter();

	if (Selected)
	{
		// Break bond with other counter
		if (LinkedCounter != NULL)
			LinkedCounter->BreakLink();
		BreakLink();
		if (last_selected != NULL)
			last_selected->BreakLink();
		((GameScene*)Scene)->setLastSelectedCounter(NULL);
	}
	else
	{
		if (((GameScene*)Scene)->getConnecticons() > 0)
		{
			if (last_selected == NULL)
			{
				// Add particle stream
				Particles = ((GameScene*)Scene)->AddStream(10, Position.x, Position.y, 1.0f, 1.0f, 1, 0);

				// Set this counter as the last selected counter
				((GameScene*)Scene)->setLastSelectedCounter(this);

				// Mark as selected
				Selected = true;
			}
			else
			{
				if (last_selected->getCounterColour() == CounterColour)
				{
					if (last_selected->getLinkedCounter() != NULL)
					{
						// Break bond with other counter
						if (LinkedCounter != NULL)
							LinkedCounter->BreakLink();
						BreakLink();
						last_selected->BreakLink();
						((GameScene*)Scene)->setLastSelectedCounter(NULL);
					}
					else
					{
						// Add particle stream
						Particles = ((GameScene*)Scene)->AddStream(10, Position.x, Position.y, 1.0f, 1.0f, 1, 0);

						// Set this counter as the last selected counter
						((GameScene*)Scene)->setLastSelectedCounter(this);

						// Link the counters
						LinkedCounter = last_selected;
						last_selected->setLinkedCounter(this);
						Connector = ((GameScene*)Scene)->AddConnector(this, last_selected, 1.0f, 1);
						((GameScene*)Scene)->setConnecticons(((GameScene*)Scene)->getConnecticons() - 1);
						IW_GAME_AUDIO->PlaySound("connect");

						// Mark as selected
						Selected = true;
					}
				}
				else
				{
					// Break bond with other counter
					if (LinkedCounter != NULL)
						LinkedCounter->BreakLink();
					BreakLink();
					last_selected->BreakLink();
				}
				((GameScene*)Scene)->setLastSelectedCounter(NULL);
			}
		}
	}
}

bool CounterActor::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Load the actor from XOML
	if (!CIwGameActorImage::LoadFromXoml(parent, load_children, node))
		return false;

	// Set default counter colour
	CounterColour = CC_Blue;

	// Process InertActor specific attributes
	unsigned int counter_colour_hash = CIwGameString::CalculateHash("countercolour");
	unsigned int health_hash = CIwGameString::CalculateHash("health");
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();
		if (name_hash == counter_colour_hash)
		{
			CIwGameString colour = (*it)->GetValue();
			if (colour == "blue")
			{
				CounterColour = CC_Blue;
			}
			else
			if (colour == "green")
			{
				CounterColour = CC_Green;
			}
			else
			if (colour == "orange")
			{
				CounterColour = CC_Orange;
			}
			else
			if (colour == "purple")
			{
				CounterColour = CC_Purple;
			}
			else
			if (colour == "red")
			{
				CounterColour = CC_Red;
			}
			else
			if (colour == "yellow")
			{
				CounterColour = CC_Yellow;
			}
		}
		else
		if (name_hash == health_hash)
		{
			Health = (*it)->GetValueAsInt();
		}
	}

	// Set correct sprite atlas image coords based on colour of counter
	switch (CounterColour)
	{
	case CC_Blue:
		((CIwGameBitmapSprite*)Visual)->setSrcRect(800, 0, 81, 80);
		break;
	case CC_Green:
		((CIwGameBitmapSprite*)Visual)->setSrcRect(881, 0, 81, 80);
		break;
	case CC_Orange:
		((CIwGameBitmapSprite*)Visual)->setSrcRect(800, 80, 81, 80);
		break;
	case CC_Purple:
		((CIwGameBitmapSprite*)Visual)->setSrcRect(881, 80, 81, 80);
		break;
	case CC_Red:
		((CIwGameBitmapSprite*)Visual)->setSrcRect(800, 160, 81, 80);
		break;
	case CC_Yellow:
		((CIwGameBitmapSprite*)Visual)->setSrcRect(881, 160, 81, 80);
		break;
	}

	return true;
}

void CounterActor::Spawn(int counter_num, int pos_x, int pos_y, eCounterColour colour)
{
	// Find game scene that contains our counter player template
	GameScene* scene = (GameScene*)GAME->findScene("GameScene");
	if (scene != NULL)
	{
		// Find our counter or counter player tremplate
		CIwGameTemplate* temp = (CIwGameTemplate*)scene->getResourceManager()->findResource("CounterTemp", "template");
		if (temp != NULL)
		{
			// Create a set of XML attributes that will replace the template parameters
			CIwGameXmlNode* replacements = new CIwGameXmlNode();
			replacements->Managed = false;
			CIwGameXmlAttribute* attrib;

			// Set base template paramater
			attrib = new CIwGameXmlAttribute();
			attrib->Managed = false;
			attrib->setName("base");
			attrib->setValue(CIwGameString(counter_num).c_str());
			replacements->AddAttribute(attrib);

			// Set position template paramater
			attrib = new CIwGameXmlAttribute();
			attrib->Managed = false;
			attrib->setName("pos");
			CIwGameString pos = pos_x;
			pos += ", ";
			pos += pos_y;
			attrib->setValue(pos.c_str());
			replacements->AddAttribute(attrib);

			// Set depth template paramater
			attrib = new CIwGameXmlAttribute();
			attrib->Managed = false;
			attrib->setName("depth");
			attrib->setValue("1.0");
			replacements->AddAttribute(attrib);

			// Set counter colour template paramater
			attrib = new CIwGameXmlAttribute();
			attrib->Managed = false;
			attrib->setName("countercolour");
			switch (colour)
			{
			case CC_Blue:
				attrib->setValue("blue");
				break;
			case CC_Green:
				attrib->setValue("green");
				break;
			case CC_Orange:
				attrib->setValue("orange");
				break;
			case CC_Purple:
				attrib->setValue("purple");
				break;
			case CC_Red:
				attrib->setValue("red");
				break;
			case CC_Yellow:
				attrib->setValue("yellow");
				break;
			}
			replacements->AddAttribute(attrib);

			// Instantiate the template
			temp->Instantiate(scene, replacements);

			// Clean up replacement attributes
			delete replacements;
		}
	}
}

void CounterActor::ResolveCollisions()
{
	if (IsDying || Box2dBody == NULL)
		return;

	// Check begin contacts
	CIwGameSlotArray<CIwGameBox2dCollidable*>& started = Box2dBody->getCollisionsStarted();

	for (int t = 0; t < started.getSize(); t++)
	{
		CIwGameBox2dCollidable* collision = started.element_at(t);
		if (collision != NULL)
		{
			CIwGameActor* actor = (CIwGameActor*)collision->getUserData();
			if (actor != NULL)
			{
				actor->NotifyCollision(this);
			}
		}
	}
}

static char g_aRandomCollisionSounds[] = 
{
	0, 3, 2, 1, 2, 1, 0, 3, 3, 1, 0, 3, 2, 1, 2, 0
};
static int g_NextRandomCollisionSounds = 0;

void CounterActor::NotifyCollision(CIwGameActor* other)
{
	CounterActor* counter = (CounterActor*)other;

	if (MouthActor != NULL)
	{
		MouthActor->getTimeline()->restart();
	}

	switch (g_aRandomCollisionSounds[g_NextRandomCollisionSounds])
	{
	case 0:
		IW_GAME_AUDIO->PlaySound("collide2");
		break;
	case 1:
		IW_GAME_AUDIO->PlaySound("collide3");
		break;
	case 2:
		IW_GAME_AUDIO->PlaySound("collide4");
		break;
	case 3:
		IW_GAME_AUDIO->PlaySound("collide5");
		break;
	}
	g_NextRandomCollisionSounds++;
	if (g_NextRandomCollisionSounds >= 16)
		g_NextRandomCollisionSounds = 0;

	if (counter->getCounterColour() == CounterColour)
	{
		counter->Kill(this);
	}
}

void CounterActor::Kill(CIwGameActor* other)
{
	Health--;
	if (Health > 0)
		return;

	// Spawn an explosion
	CIwGameActorParticles* p = ((GameScene*)Scene)->AddExplosion(10, Position.x, Position.y, 0.5f, 1.0f, 1, 0);
	p->setVelocity(0, -2.0f);
	IW_GAME_AUDIO->PlaySound("explosion");

	// Remove physics body
//	RemoveBody();
//	setCollidable(false);

	// Start die timeline
	if (Timeline != NULL)
		Timeline->restart();

	// Set as dying
	IsDying = true;

	// Update score
	int multiplier = 1;
	GAME->setCurrentScore(GAME->getCurrentScore() + (Score * multiplier));

	// Generator score floater
	((GameScene*)Scene)->AddScoreFloater(Score * multiplier, Position.x, Position.y, 1.0f, 1.0f, 2);

	// Update counter count
	((GameScene*)Scene)->setMaxCounters(((GameScene*)Scene)->getMaxCounters() - 1);

	// Check for end of round
	((GameScene*)Scene)->CheckforEndOfRound();

	// Destroy any connecticons
	if (Connector != NULL)
	{
		Scene->removeActor(Connector);
		Connector = NULL;
	}
}


