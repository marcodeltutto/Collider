/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#include "GameScene.h"

#include "IwGameCamera.h"
#include "IwGameInput.h"
#include "IwGame.h"
#include "IwGameActorParticles.h"

#include "Game.h"
#include "InertActor.h"
#include "FloaterActor.h"
#include "CounterActor.h"
#include "ConnectActor.h"

//
// XOML paramaters
// ---------------
//


//
//
//
// GameScene implementation
//
//
//

bool GameScene::isEndOfRound()
{
	if (MaxCounters == 0)
	{
		EndofRound();

		return true;
	}

	return false;
}

int GameScene::Init(int max_collidables, int max_layers, bool doSleep)
{
	CIwGameScene::Init(max_collidables, max_layers, doSleep);

	GameMode = GAM_Normal;
	EndOfRound = false;
	LastSelectedCounter = NULL;
	FirstFrame = true;
	BonusTimer = 11000;
	PrevBonusTimer = -1;

	return 1;
}

void GameScene::Update(float dt)
{
	if (FirstFrame)
	{
		setMaxCounters(MaxCounters);
		UpdateRoundNumber();
		UpdateConnecticons();
		UpdateRecord();
		FirstFrame = false;
	}

	if (IsActive)
	{
		if (ShootTimerStarted)
		{
			ShootTimer += (dt * FRAME_SPEED_LOCK_MS);
			if (ShootTimer > 10000)
			{
				EndofRound();
			}
			UpdateShootTimer();
		}
		else
		{
			if (BonusTimer > 0)
			{
				BonusTimer -= (dt * FRAME_SPEED_LOCK_MS);
				if (BonusTimer < 0)
					BonusTimer = 0;
				UpdateTimer();
			}
		}

		// Check for menu / back buttons
		if (GAME->getCurrentScene() == this)
		{
			if (IW_GAME_INPUT->isBackPressed() || IW_GAME_INPUT->isMenuPressed())
			{
				CIwGameScene* hud = GAME->findScene("HUDScene");
				hud->setActive(false);
				setActive(false);
				CIwGameScene* pause = GAME->findScene("PauseMenu");
				pause->setVisible(true);
				GAME->changeScene(pause);
				GAME->BringSceneToFront(pause);
				CIwGameAnimTimeline* timeline = pause->getTimelinesManager()->findTimeline("fly_in3");
				pause->setTimeline(timeline);
				timeline->restart();
			}
		}
	}

	CIwGameScene::Update(dt);
}


bool GameScene::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Load the actor from XOML
	if (!CIwGameScene::LoadFromXoml(parent, load_children, node))
		return false;

	// Set defaults
	GameMode = GAM_Normal;

	// Process InertActor specific attributes
	unsigned int game_mode_hash = CIwGameString::CalculateHash("gamemode");
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == game_mode_hash)
		{
			CIwGameString game_mode = (*it)->GetValue();
			if (game_mode == "normal")
				GameMode = GAM_Normal;
		}
	}

	// Reset level data
	ResetLevel();

	return true;
}

void GameScene::ResetLevel()
{
	CIwGameXomlVariableInt* var = (CIwGameXomlVariableInt*)VariableManager->findVariable("MaxCounters");
	if (var != NULL)
		MaxCounters = var->NativeValue;

	var = (CIwGameXomlVariableInt*)VariableManager->findVariable("Connecticons");
	if (var != NULL)
		Connecticons = var->NativeValue;

	FirstFrame = true;
	ShootTimer = 0;
	ShootTimerStarted = false;
	BonusTimer = 11000;
	PrevBonusTimer = -1;
}

CIwGameActorParticles* GameScene::AddExplosion(int num_particles, float x, float y, float scale, float depth, int layer, float gravity)
{
	// Create explosion particle actor
	CIwGameActorParticles* actor = new CIwGameActorParticles();
	addActor(actor);
	actor->Init(num_particles);
	actor->setImage((CIwGameImage*)ResourceManager->findResource("sprites1", CIwGameXomlNames::Image_Hash));
	actor->setPosition(x, y);

	// Set random paramaters
	actor->setScaleMode(CIwGameActorParticles::PAM_Random);
	actor->setAngVelocityMode(CIwGameActorParticles::PAM_Random);
	actor->setVelocityMode(CIwGameActorParticles::PAM_Random);

	// Set paramater limits
	CIwFVec2 scale_range(scale, scale + scale / 2);
	actor->setScaleRange(scale_range);
	CIwFVec2 angle_range(-5, 5);
	actor->setAngleRange(angle_range);
	CIwFVec4 vel_range(-5, 5, -5, 5);
	actor->setVelocityRange(vel_range);
	CIwRect src_rect(908, 440, 100, 100);
	CIwFVec4 colour(255, 255, 255, 255);
	CIwFVec4 colour_vel(0, 0, 0, -5);

	// Gnnerate the particles
	actor->GenerateRandomParticles(num_particles, src_rect, colour, colour_vel, 2, 1, 0, gravity);

	return actor;
}

CIwGameActorParticles* GameScene::AddStream(int num_particles, float x, float y, float scale, float depth, int layer, float gravity)
{
	// Create stream particle actor
	CIwGameActorParticles* actor = new CIwGameActorParticles();
	addActor(actor);
	actor->Init(num_particles);
	actor->setImage((CIwGameImage*)ResourceManager->findResource("sprites1", CIwGameXomlNames::Image_Hash));
	actor->setPosition(x, y);
	CIwRect src_rect(800, 291, 68, 65);
	CIwFVec4 colour(255, 255, 255, 128);
	CIwFVec4 colour_vel(0, 0, 0, -3);

	// CReate and add particles
	float spawn_delay = 0;
	for (int t = 0; t < num_particles; t++)
	{
		CIwGameActorParticle* p = new CIwGameActorParticle();
		p->LifeSpan = 1;
		p->Lives = -1;
		p->SpawnDelay = spawn_delay;
		p->Gravity = gravity;
		p->Colour = colour;
		p->ColourVelocity = colour_vel;
		p->DepthVelocity = -0.01f;

		actor->addParticle(p, src_rect);
		spawn_delay += (float)1.0f / num_particles;
	}

	return actor;
}

FloaterActor* GameScene::AddScoreFloater(int score, float x, float y, float scale, float depth, int layer)
{
	CIwGameFont* font = (CIwGameFont*)ResourceManager->findResource("trebuchet_12", CIwGameXomlNames::Font_Hash);
	if (font == NULL)
		return NULL;

	FloaterActor* floater = new FloaterActor();
	addActor(floater);
	floater->Init(font);
	floater->setText(CIwGameString(score).c_str());
	floater->setColour(0, 0, 0, 255);
	CIwRect rc(0, 0, 100, 50);
	floater->setRect(rc);
	floater->setScale(scale);
	floater->setLayer(layer);
	floater->setDepth(depth);
	floater->setPosition(x, y);
	floater->setVelocity(0, -2);
	floater->setFadeSpeed(0.005f);

	return floater;
}

ConnectActor* GameScene::AddConnector(CounterActor* actor1, CounterActor* actor2, float depth, int layer)
{
	ConnectActor* connect = new ConnectActor();
	addActor(connect);
	CIwGameImage* image = (CIwGameImage*)ResourceManager->findResource("sprites1", CIwGameXomlNames::Image_Hash);
	connect->Init(image, 100, 8);
	connect->setActor1(actor1);
	connect->setActor2(actor2);
	CIwRect src_rect(868, 291, 100, 8);
	CIwGameBitmapSprite* visual = (CIwGameBitmapSprite*)connect->getVisual();
	visual->setSrcRect(&src_rect);

	connect->setColour(255, 255, 255, 255);
	connect->setLayer(layer);
	connect->setDepth(depth);
	connect->UpdateConnector();

	return connect;
}


bool GameScene::CheckforEndOfRound()
{
	if (EndOfRound)
		return true;

	if (isEndOfRound())
	{
		EndOfRound = true;
		return true;
	}

	return false;
}

void GameScene::EndofRound()
{
	EndOfRound = true;

	// Stop shoot count down tiomer
	stopShootTimer();

	// Load out of frogs scene
	IW_GAME_XOML->Process((IIwGameXomlResource*)Parent, "LevelCleared.xml");

	// Set score and record score
	CIwGameScene* scene = GAME->findScene("LevelComplete");
	if (scene != NULL)
	{
		// Add on bonus for (remaining connecticons + 1) * bonus timer
		int bonus = 0;
		if (MaxCounters == 0)
			bonus = (Connecticons + 1) * (int)(BonusTimer / 1000) * 10;
		GAME->setCurrentScore(GAME->getCurrentScore() + bonus);

		// Set the round score
		CIwGameActorText* text = (CIwGameActorText*)scene->findActor("RoundScore");
		if (text != NULL)
			text->setText(CIwGameString(GAME->getCurrentScore()).c_str());
		text = (CIwGameActorText*)scene->findActor("BonusScore");
		if (text != NULL)
			text->setText(CIwGameString(bonus).c_str());

		// Show failed or success actor
		if (MaxCounters == 0)
		{
			CIwGameActor* actor = (CIwGameActor*)scene->findActor("RoundComplete");
			if (actor != NULL)
				actor->setVisible(true);
			GAME->setCompleted((GAME->getCurrentZone() - 1) * 10 + GAME->getCurrentRound() - 1, true);
		}
		else
		{
			CIwGameActor* actor = (CIwGameActor*)scene->findActor("RoundFailed");
			if (actor != NULL)
				actor->setVisible(true);
		}

		// If we got a record score then show record score animation
		int level = (GAME->getCurrentZone() - 1) * 10 + GAME->getCurrentRound();
		if (GAME->setRecord(level - 1, GAME->getCurrentScore()))
		{
			CIwGameActor* actor = (CIwGameActor*)scene->findActor("RecordParticles1");
			if (actor != NULL)
				actor->setVisible(true);
			actor = (CIwGameActor*)scene->findActor("RecordParticles2");
			if (actor != NULL)
				actor->setVisible(true);
		}

	}

	// PLay end of round tune
	IW_GAME_AUDIO->PlayMusic("eor.mp3", 1);

}

void GameScene::UpdateScore()
{
	// Find the HUD scene
	CIwGameScene* scene = Parent->findScene("HUDScene");
	if (scene != NULL)
	{
		// Find the player front count text actor
		CIwGameActorText* text = (CIwGameActorText*)scene->findActor("Score");
		if (text != NULL)
		{
			text->setText(CIwGameString(GAME->getCurrentScore()).c_str());
			text->getTimeline()->restart();
		}
	}
}

void GameScene::UpdateRoundNumber()
{
	// Find the HUD scene
	CIwGameScene* scene = Parent->findScene("HUDScene");
	if (scene != NULL)
	{
		// Find the player front count text actor
		CIwGameActorText* text = (CIwGameActorText*)scene->findActor("Round");
		if (text != NULL)
		{
			int index = (GAME->getCurrentZone() - 1) * 10 + GAME->getCurrentRound();
			text->setText(CIwGameString(index).c_str());
		}
	}
}

void GameScene::UpdateConnecticons()
{
	// Find the HUD scene
	CIwGameScene* scene = Parent->findScene("HUDScene");
	if (scene != NULL)
	{
		// Find the player front count text actor
		CIwGameActorText* text = (CIwGameActorText*)scene->findActor("Connecticons");
		if (text != NULL)
		{
			text->setText(CIwGameString(Connecticons).c_str());
			text->getTimeline()->restart();
		}
	}
}

void GameScene::UpdateRecord()
{
	// Find the HUD scene
	CIwGameScene* scene = Parent->findScene("HUDScene");
	if (scene != NULL)
	{
		// Find the player front count text actor
		CIwGameActorText* text = (CIwGameActorText*)scene->findActor("RecordScore");
		if (text != NULL)
		{
			int index = (GAME->getCurrentZone() - 1) * 10 + GAME->getCurrentRound() - 1;
			text->setText(CIwGameString(GAME->getRecord(index)).c_str());
		}
	}
}

void GameScene::UpdateTimer()
{
	int new_time = (int)(BonusTimer / 1000);
	if (new_time == PrevBonusTimer)
		return;

	// Find the HUD scene
	CIwGameScene* scene = Parent->findScene("HUDScene");
	if (scene != NULL)
	{
		// Find the player front count text actor
		CIwGameActorText* text = (CIwGameActorText*)scene->findActor("Timer");
		if (text != NULL)
		{
			text->setText(CIwGameString(new_time).c_str());
			text->getTimeline()->restart();
		}
	}
	PrevBonusTimer = new_time;
}

void GameScene::ShowShootTimer()
{
	// Find the HUD scene
	CIwGameScene* scene = Parent->findScene("HUDScene");
	if (scene != NULL)
	{
		// Find the player front count text actor
		CIwGameActorText* text = (CIwGameActorText*)scene->findActor("CountDownTimer");
		if (text != NULL)
		{
			text->setVisible(true);
		}
	}
}

void GameScene::UpdateShootTimer()
{
	// Find the HUD scene
	CIwGameScene* scene = Parent->findScene("HUDScene");
	if (scene != NULL)
	{
		// Find the player front count text actor
		CIwGameActorText* text = (CIwGameActorText*)scene->findActor("CountDownTimer");
		if (text != NULL)
		{
			int time = ShootTimer;
			if (time > 10000) time = 10000;
			time = (10999 - time) / 1000;
			text->setText(CIwGameString(time).c_str());
		}
	}
}
void GameScene::Shoot()
{
	for (_Iterator it = begin(); it != end(); ++it)
	{
		CounterActor* counter = (CounterActor*)*it;
		if (counter->getType() == ACT_Counter)
		{
			if (counter->getLinkedCounter() != NULL)
			{
				float dx = counter->getLinkedCounter()->getPosition().x - counter->getPosition().x;
				float dy = counter->getLinkedCounter()->getPosition().y - counter->getPosition().y;
				counter->setVelocity(dx / 10.0f, dy / 10.0f);

			}
		}
	}

	// Start the shoot timer
	startShootTimer();

	// Hide the start button
	CIwGameScene* scene = Parent->findScene("HUDScene");
	if (scene != NULL)
	{
		// Find the statr button and hide it
		CIwGameActor* button = (CIwGameActor*)scene->findActor("StartButton");
		if (button != NULL)
			button->setVisible(false);
	}

}
