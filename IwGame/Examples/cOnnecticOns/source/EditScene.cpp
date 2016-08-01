/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#include "EditScene.h"

#include "IwGameCamera.h"
#include "IwGameInput.h"
#include "IwGame.h"
#include "IwGameAnim.h"
#include "IwGameActorParticles.h"

#include "PlacerActor.h"
#include "GravityPlacerActor.h"
#include "Game.h"
#include "GameScene.h"

//
// XOML paramaters
// ---------------
//

// Actor ID's
// 2 - Blue
// 3 - Green
// 4 - Orange
// 5 - Purple
// 6 - Red
// 7 - Yellow
// 8 - Blue small
// 9 - Green small
// 10 - Orange small
// 11 - Purple small
// 12 - Red small
// 13 - Yellow small
// 14 - Blue large
// 15 - Green large
// 16 - Orange large
// 17 - Purple large
// 18 - Red large
// 19 - Yellow large
// 20 - H block
// 21 - V block
// 22 - H block 45 degrees
// 23 - H block -45 degrees

//
//
//
// EditScene implementation
//
//
//

void EditScene::SaveSceneObjects()
{
	// Get Num COnnecticons
	NumConnecticons = ((CIwGameXomlVariableInt*)IW_GAME_GLOBAL_RESOURCES->getVariableManager()->findVariable("EditorConnecticons"))->NativeValue;

	// Calculate gravity
	GravityPlacerActor* gravity = (GravityPlacerActor*)findActor("GravityDirection");
	int pos_x = (int)gravity->getPosition().x;
	int pos_y = (int)gravity->getPosition().y;
	float d = pos_x * pos_x + pos_y * pos_y;
	d = sqrtf(d);
	if (d < 30.0f)
	{
		Gravity.x = 0;
		Gravity.y = 0;
	}
	else
	{
		Gravity.x = (int)-(pos_x / 10);
		Gravity.y = (int)-(pos_y / 10);
	}

	// Sace scene XML file
	SaveScene();

	// Save editor file
	CIwGameXomlVariableInt* start_round = (CIwGameXomlVariableInt*)IW_GAME_GLOBAL_RESOURCES->getVariableManager()->findVariable("StartRound");
	if (start_round != NULL)
	{
		CIwGameString level_name  = "Editor";
		level_name += start_round->NativeValue;
		level_name += ".bin";
		CIwGameFile file;
		if (file.Open(level_name.c_str(), "wb"))
		{
			// Save NumConnecticons
			file.Write(&NumConnecticons, sizeof(NumConnecticons));

			// Save Gravity actor pos
			file.Write(&pos_x, sizeof(pos_x));
			file.Write(&pos_y, sizeof(pos_y));

			// Save scenes objects
			for (_Iterator it = begin(); it != end(); ++it)
			{
				if ((*it)->getType() >= 2)
				{
					EditSceneObject obj;
					obj.x = (int)(*it)->getPosition().x;
					obj.y = (int)(*it)->getPosition().y;
					obj.type = (*it)->getType();
					file.Write(&obj, sizeof(EditSceneObject));
				}
			}
		}
	}
}

void EditScene::LoadSceneObjects()
{
	CIwGameXomlVariableInt* start_round = (CIwGameXomlVariableInt*)IW_GAME_GLOBAL_RESOURCES->getVariableManager()->findVariable("StartRound");
	if (start_round != NULL)
	{
		CIwGameString level_name  = "Editor";
		level_name += start_round->NativeValue;
		level_name += ".bin";
		CIwGameFile file;
		if (file.Open(level_name.c_str(), "rb"))
		{
			// Load NumConnecticons
			file.Read(&NumConnecticons, sizeof(NumConnecticons));

			// Update XOML variable for NumConnecticons
			CIwGameXomlVariableInt* var = (CIwGameXomlVariableInt*)IW_GAME_GLOBAL_RESOURCES->getVariableManager()->findVariable("EditorConnecticons");
			var->setValue(CIwGameString(NumConnecticons).c_str());

			// Update the text actor to show the correct amuont
			CIwGameScene* hud_scene = GAME->findScene("HUDScene");
			CIwGameActorText* connecticons_label = (CIwGameActorText*)hud_scene->findActor("ConnectButton");
			connecticons_label->setText(var->getValue().c_str());

			// Load Gravity actor pos
			GravityPlacerActor* gravity = (GravityPlacerActor*)findActor("GravityDirection");
			int pos_x = 0, pos_y = 0;
			file.Read(&pos_x, sizeof(pos_x));
			file.Read(&pos_y, sizeof(pos_y));
			gravity->setPosition((float)pos_x, (float)pos_y);

			// Load scenes objects
			do
			{
				EditSceneObject obj;
				if (!file.Read(&obj, sizeof(EditSceneObject)))
					break;
				AddObject(obj.type, obj.x, obj.y);
			}
			while (1 == 1);
		}
	}
}

void EditScene::SaveScene()
{
	// Count number of bugs
	int bugs = 0;
	for (_Iterator it = begin(); it != end(); ++it)
	{
		int type = (*it)->getType();
		if (type >= 2 && type <= 19)
			bugs++;
	}

	// Generate a saved XML scene that can be ran by the game engine
	CIwGameString part;
	part.allocString(16384);
	part += "<?xml version=\"1.0\"?><xml>";
	part += "<LoadXOML File=\"PauseMenu.xml\" /><GameScene Name=\"GameScene\" Style=\"GenericSceneStyle\" Camera=\"GameCam\" Timeline=\"scene_fade_in\" Colour=\"64, 128, 200, 0\" Extents=\"-1000, -400, 2000, 800\" Current=\"true\" Gravity=\"";
	part += Gravity.x;
	part += ", ";
	part += Gravity.y;
	part += "\" GameMode=\"normal\" OnGainedFocus=\"GainedFocusAction\" >";
	part += "<Variable Name=\"MaxCounters\" Type=\"int\" Value=\"";
	part += bugs;	// MaxCounters
	part += "\" />";
	part += "<Variable Name=\"Connecticons\" Type=\"int\" Value=\"";
	part += NumConnecticons;	// Connecticons
	part += "\" />";
	part += "<Actions Name=\"GainedFocusAction\"><Action Method=\"BringSceneToFront\" Param1=\"HUDScene\" /></Actions><LoadXOML File=\"Actors.xml\" />";
	part += "<Camera Name=\"GameCam\" Position=\"0, 0\" Angle=\"0\" Scale=\"1.0\" TouchPanX=\"false\" TouchPanY=\"false\" />";
	part += "<Timeline Name=\"scene_fade_in\" AutoPlay=\"true\"><Animation Anim=\"fade_in\" Target=\"Colour\" Repeat=\"1\" StartAtTime=\"0\" /></Timeline>";
	part += "<Timeline Name=\"scene_fade_out\" AutoPlay=\"true\"><Animation Anim=\"quick_fade_out\" Target=\"Colour\" Repeat=\"1\" StartAtTime=\"0\" /></Timeline>";
	part += "<Timeline Name=\"scene_fade_current_round\" AutoPlay=\"true\"><Animation Anim=\"quick_fade_out\" Target=\"Colour\" Repeat=\"1\" StartAtTime=\"0\" OnEnd=\"RestartCurrentRound\" /></Timeline>";
	part += "<Timeline Name=\"scene_fade_next_round\" AutoPlay=\"true\"><Animation Anim=\"quick_fade_out\" Target=\"Colour\" Repeat=\"1\" StartAtTime=\"0\" OnEnd=\"StartNextRound\" /></Timeline>";
	part += "<Timeline Name=\"scene_fade_quit\" AutoPlay=\"true\"><Animation Anim=\"quick_fade_out\" Target=\"Colour\" Repeat=\"1\" StartAtTime=\"0\" OnEnd=\"QuitGame\" /></Timeline>";
	part += "<InertActor Name=\"Background\" Position=\"0, 0\" Size=\"800, 480\" SrcRect=\"0, 0, 800, 480\" Image=\"sprites1\" Depth=\"1.0\" IsTappable=\"false\" HitTes=\"false\" />";
	part += "<Box2DMaterial Name=\"Solid\" Type=\"static\" Density=\"1.0\" Friction=\"1.0\" Restitution=\"0.1\" />";
	part += "<Shape Name=\"WallVShape\" Type=\"box\" width=\"20\" height=\"480\" />";
	part += "<InertActor Name=\"WallL\" Position=\"-410, 0\" Size=\"20, 480\" SrcRect=\"32, 32, 32, 32\" Image=\"sprites1\" Shape=\"WallVShape\" Box2dMaterial=\"Solid\" CollisionFlags=\"1, 1, 1\" Collidable=\"true\" />";
	part += "<InertActor Name=\"WallR\" Position=\"410, 0\" Size=\"20, 480\" SrcRect=\"32, 32, 32, 32\" Image=\"sprites1\" Shape=\"WallVShape\" Box2dMaterial=\"Solid\" CollisionFlags=\"1, 1, 1\" Collidable=\"true\" />";
	part += "<Shape Name=\"WallHShape\" Type=\"box\" width=\"800\" height=\"20\" />";
	part += "<InertActor Name=\"WallT\" Position=\"0, -250\" Size=\"800, 20\" SrcRect=\"32, 32, 32, 32\" Image=\"sprites1\" Shape=\"WallHShape\" Box2dMaterial=\"Solid\" CollisionFlags=\"1, 1, 1\" Collidable=\"true\" />";
	part += "<InertActor Name=\"WallB\" Position=\"0, 250\" Size=\"800, 20\" SrcRect=\"32, 32, 32, 32\" Image=\"sprites1\" Shape=\"WallHShape\" Box2dMaterial=\"Solid\" CollisionFlags=\"1, 1, 1\" Collidable=\"true\" />";

	// Save scenes objects
	int name = 1;
	for (_Iterator it = begin(); it != end(); ++it)
	{
		int type = (*it)->getType();
		if (type >= 2)
		{
			CIwGameString templ;
			int colour = 0;
			if (type >= 2 && type <= 7)		// Normal bug counter
			{
		        templ = "<FromTemplate Template=\"CounterTemp\" base=\"";
				colour = type - 2;
			}
			else
			if (type >= 8 && type <= 13)	// Large bug counter
			{
		        templ = "<FromTemplate Template=\"BigCounterTemp\" base=\"";
				colour = type - 8;
			}
			else
			if (type >= 14 && type <= 19)	// Medium bug counter
			{
		        templ = "<FromTemplate Template=\"MediumCounterTemp\" base=\"";
				colour = type - 14;
			}

			if (type >= 2 && type <= 19)
			{
		        part += templ;
				part += name++;
				part += "\" pos=\"";
				part += (int)(*it)->getPosition().x;
				part += ", ";
				part += (int)(*it)->getPosition().y;
				part += "\" depth=\"1.0\" colour=\"";
				switch (colour)
				{
				case 0: part += "blue"; break;
				case 1: part += "green"; break;
				case 2: part += "orange"; break;
				case 3: part += "purple"; break;
				case 4: part += "red"; break;
				case 5: part += "yellow"; break;
				}
				part += "\" />";
			}
			else
			if (type <= 31)
			{
		        part += "<FromTemplate Template=\"WallTemp\" base=\"";
				part += name++;
				part += "\" pos=\"";
				part += (int)(*it)->getPosition().x;
				part += ", ";
				part += (int)(*it)->getPosition().y;
				part += "\" angle=\"";
				if (type == 21 || type == 27)
					part += "90\"";
				else
				if (type == 22 || type == 28)
					part += "45\"";
				else
				if (type == 23 || type == 29)
					part += "-45\"";
				else
					part += "0\"";
				part += " angvel=\"";
				if (type == 24 || type == 30)
					part += "-2\"";
				else
				if (type == 25 || type == 31)
					part += "2\"";
				else
					part += "0\"";
				if (type == 24 || type == 25)
					part += " material=\"SolidMover\" />";
				else
				if (type >= 20 && type <= 23)
					part += " material=\"Solid\" />";
				else
					part += " material=\"Bouncey\" />";
			}
		}
	}

	part += "</GameScene><LoadXOML File=\"hud.xml\" /></xml>";

	CIwGameXomlVariableInt* start_round = (CIwGameXomlVariableInt*)IW_GAME_GLOBAL_RESOURCES->getVariableManager()->findVariable("StartRound");
	if (start_round != NULL)
	{
		CIwGameString level_name  = "Scene";
		level_name += start_round->NativeValue + 100;
		level_name += ".xml";
		CIwGameFile file;
		if (file.Open(level_name.c_str(), "wb"))
		{
			// Write scene
			file.Write((void*)part.c_str(), part.GetLength());
		}
	}
}

int EditScene::Init(int max_collidables, int max_layers, bool doSleep)
{
	CIwGameScene::Init(max_collidables, max_layers, doSleep);
	NumConnecticons = 2;
	return 1;
}

void EditScene::Update(float dt)
{
	if (IsActive)
	{
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


bool EditScene::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Load the actor from XOML
	if (!CIwGameScene::LoadFromXoml(parent, load_children, node))
		return false;


	// Load scebe objects
	LoadSceneObjects();

	return true;
}

void EditScene::AddObject(int id, int x, int y, bool close)
{
	PlacerActor *actor = new PlacerActor();
	addActor(actor);
	actor->setType(id);

	CIwGameImage* sprites = (CIwGameImage*)ResourceManager->findResource("sprites1", CIwGameXomlNames::Image_Hash);
	switch (id)
	{
	case 2:	// Blue bug
		actor->Init(sprites, 800, 0, 81, 80);
		break;
	case 3:	// Green bug
		actor->Init(sprites, 881, 0, 81, 80);
		break;
	case 4:	// Orange bug
		actor->Init(sprites, 800, 80, 81, 80);
		break;
	case 5:	// Purple bug
		actor->Init(sprites, 881, 80, 81, 80);
		break;
	case 6:	// Red bug
		actor->Init(sprites, 800, 160, 81, 80);
		break;
	case 7:	// Yellow bug
		actor->Init(sprites, 881, 160, 81, 80);
		break;
	case 8:	// Blue bug (large)
		actor->Init(sprites, 800, 0, 81, 80);
		actor->SetDestSize(120, 120);
		break;
	case 9:	// Green bug (large)
		actor->Init(sprites, 881, 0, 81, 80);
		actor->SetDestSize(120, 120);
		break;
	case 10: // Orange bug (large)
		actor->Init(sprites, 800, 80, 81, 80);
		actor->SetDestSize(120, 120);
		break;
	case 11: // Purple bug (large)
		actor->Init(sprites, 881, 80, 81, 80);
		actor->SetDestSize(120, 120);
		break;
	case 12: // Red bug (large)
		actor->Init(sprites, 800, 160, 81, 80);
		actor->SetDestSize(120, 120);
		break;
	case 13: // Yellow bug (large)
		actor->Init(sprites, 881, 160, 81, 80);
		actor->SetDestSize(120, 120);
		break;
	case 14: // Blue bug (small)
		actor->Init(sprites, 800, 0, 81, 80);
		actor->SetDestSize(60, 60);
		break;
	case 15: // Green bug (small)
		actor->Init(sprites, 881, 0, 81, 80);
		actor->SetDestSize(60, 60);
		break;
	case 16: // Orange bug (small)
		actor->Init(sprites, 800, 80, 81, 80);
		actor->SetDestSize(60, 60);
		break;
	case 17: // Purple bug (small)
		actor->Init(sprites, 881, 80, 81, 80);
		actor->SetDestSize(60, 60);
		break;
	case 18: // Red bug (small)
		actor->Init(sprites, 800, 160, 81, 80);
		actor->SetDestSize(60, 60);
		break;
	case 19: // Yellow bug (small)
		actor->Init(sprites, 881, 160, 81, 80);
		actor->SetDestSize(60, 60);
		break;
	case 20: // H wall (solid)
		actor->Init(sprites, 800, 240, 138, 51);
		break;
	case 21: // V wall (solid)
		actor->Init(sprites, 800, 240, 138, 51);
		actor->setAngle(90);
		break;
	case 22: // H wall - 45 degrees (solid)
		actor->Init(sprites, 800, 240, 138, 51);
		actor->setAngle(-45);
		break;
	case 23:// H wall + 45 degrees (solid)
		actor->Init(sprites, 800, 240, 138, 51);
		actor->setAngle(45);
		break;
	case 24:// H wall spin anti (solid)
		actor->Init(sprites, 800, 240, 138, 51);
		actor->setAngularVelocity(-2);
		break;
	case 25:// H wall spin clock (solid)
		actor->Init(sprites, 800, 240, 138, 51);
		actor->setAngularVelocity(2);
		break;
	case 26: // H wall (dyhamic)
		actor->Init(sprites, 800, 240, 138, 51);
		actor->setColour(255, 120, 120, 255);
		break;
	case 27: // V wall (dyhamic)
		actor->Init(sprites, 800, 240, 138, 51);
		actor->setAngle(90);
		actor->setColour(255, 120, 120, 255);
		break;
	case 28: // H wall - 45 degrees (dyhamic)
		actor->Init(sprites, 800, 240, 138, 51);
		actor->setAngle(-45);
		actor->setColour(255, 120, 120, 255);
		break;
	case 29:// H wall + 45 degrees (dyhamic)
		actor->Init(sprites, 800, 240, 138, 51);
		actor->setAngle(45);
		actor->setColour(255, 120, 120, 255);
		break;
	case 30:// H wall spin anti (dyhamic)
		actor->Init(sprites, 800, 240, 138, 51);
		actor->setAngularVelocity(-2);
		actor->setColour(255, 120, 120, 255);
		break;
	case 31:// H wall spin clock (dyhamic)
		actor->Init(sprites, 800, 240, 138, 51);
		actor->setAngularVelocity(2);
		actor->setColour(255, 120, 120, 255);
		break;
	}
	actor->setPosition((float)x, (float)y);

	if (close)
	{
		CIwGameScene* scene = GAME->findScene("ObjectSelect");
		if (scene != NULL)
		{
			CIwGameAnimTimeline* timeline = scene->getTimelinesManager()->findTimeline("fly_out");
			if (timeline != NULL)
			{
				scene->setTimeline(timeline);
				timeline->restart();
			}
		}
	}
}

