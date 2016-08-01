/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

//#define ENABLE_ADS
//#define ANDROID_MARKET_LICENSE

#include "s3e.h"
#include "Iw2D.h"
#include "IwGx.h"
#include "IwResManager.h"

#if defined(ENABLE_ADS)
#include "iwGameAdsView.h"
#include "iwGameAdsViewAnimator.h"
#endif	// ENABLE_ADS

#if defined(ANDROID_MARKET_LICENSE)
#include "LicensingCheck.h"
#endif	// ANDROID_MARKET_LICENSE

#include "IwGameFacebook.h"

#include "Game.h"
#include "Common.h"
#include "InertActor.h"
#include "CounterActor.h"
#include "PlacerActor.h"
#include "GravityPlacerActor.h"
#include "SelectorActor.h"
#include "GameScene.h"
#include "LevelSelectScene.h"
#include "ZoneSelectScene.h"
#include "EditScene.h"


CDECLARE_SINGLETON(Game)

//
//
//
// Game singleton implementation
//
//
//
#if defined(ENABLE_ADS)
void Game::AdTest_Init()
{
	int width = IwGxGetScreenWidth();
	int height = IwGxGetScreenHeight();

	// Create ad view
	CIwGameAdsView::Create();

	// Initialise with Application ID (yuo get this from your ad provider)
	IW_GAME_ADS_VIEW->Init("");

	// Set ad provider
	IW_GAME_ADS_VIEW->setAdProvider(CIwGameAds::InnerActive);

	// Set ad request interval in seconds
	IW_GAME_ADS_VIEW->setNewAdInterval(30);

	// Force a request for an initial ad
	IW_GAME_ADS_VIEW->RequestNewAd(CIwGameAds::InnerActive);

	// Set total number of ads visible in te ads view
	IW_GAME_ADS_VIEW->setNumAdsVisible(1);

	// Tell animators to loop
	IW_GAME_ADS_VIEW->setLooped(true);

	// Create and attach an animator that fades the ad in over 1 second, pauses for 7 seconds and then fades the ad back out
	CIwGameAdsViewAnimator* anim = new CIwGameAdsViewAnimator();
	anim->Init();
	anim->setAdViewDataIndex(0);
	anim->setCanvasSize(width, height);
	anim->setInAnim(CIwGameAdsViewAnimator::AnimFadeIn, 1000);
	anim->setOutAnim(CIwGameAdsViewAnimator::AnimFadeOut, 1000);
	anim->setStayDuration(7000);
	IW_GAME_ADS_VIEW->addAnimator(0, anim);

	// Create and attach an animator that sweeps the ad in from the right the over 1,2 seconds, pauses for 7 seconds and then sweeps back out
	anim = new CIwGameAdsViewAnimator();
	anim->Init();
	anim->setAdViewDataIndex(0);
	anim->setCanvasSize(width, height);
	anim->setRestingPosition(0, -height / 8);
	anim->setInAnim(CIwGameAdsViewAnimator::AnimRightSweepIn, 1200);
	anim->setOutAnim(CIwGameAdsViewAnimator::AnimRightSweepOut, 1200);
	anim->setStayDuration(7000);
	IW_GAME_ADS_VIEW->addAnimator(0, anim);

	// Create and attach an animator that scales the ad in over 1.5 seconds, pauses for 7 seconds and then scales back out
	anim = new CIwGameAdsViewAnimator();
	anim->Init();
	anim->setAdViewDataIndex(0);
	anim->setCanvasSize(width, height);
	anim->setInAnim(CIwGameAdsViewAnimator::AnimScaleIn, 1500);
	anim->setOutAnim(CIwGameAdsViewAnimator::AnimScaleOut, 1500);
	anim->setStayDuration(7000);
	IW_GAME_ADS_VIEW->addAnimator(0, anim);

	// Create and attach an animator that rotates the ad in over 1 second, pauses for 7 seconds and then rotates back out
	anim = new CIwGameAdsViewAnimator();
	anim->Init();
	anim->setAdViewDataIndex(0);
	anim->setCanvasSize(width, height);
	anim->setInAnim(CIwGameAdsViewAnimator::AnimSpinIn, 1000);
	anim->setOutAnim(CIwGameAdsViewAnimator::AnimSpinOut, 1000);
	anim->setStayDuration(7000);
	IW_GAME_ADS_VIEW->addAnimator(0, anim);
}

void Game::AdTest_Release()
{
	// Clean up ads system
	IW_GAME_ADS_VIEW->Release();
	CIwGameAdsView::Destroy();
}

void Game::AdTest_Update()
{
	CIwGameScene* scene = GAME->findScene("AdScene");
	if (scene != NULL)
	{
		// Update the ads view
		IW_GAME_ADS_VIEW->Update(1.0f);
		IW_GAME_ADS_VIEW->Draw(scene);
	}
}
#endif	// ENABLE_ADS

void Game::Init(bool enable_http)
{
	// Init base game
	CIwGame::Init(enable_http);

	PrevTouched = false;

	srand(time(NULL));

	// Add custom classes to XOML system
	IW_GAME_XOML->addClass(new InertActorCreator());
	IW_GAME_XOML->addClass(new GameSceneCreator());
	IW_GAME_XOML->addClass(new LevelSelectSceneCreator());
	IW_GAME_XOML->addClass(new ZoneSelectSceneCreator());
	IW_GAME_XOML->addClass(new CounterActorCreator());
	IW_GAME_XOML->addClass(new PlacerActorCreator());
	IW_GAME_XOML->addClass(new GravityPlacerActorCreator());
	IW_GAME_XOML->addClass(new SelectorActorCreator());
	IW_GAME_XOML->addClass(new EditSceneCreator());

	// Add custom game actions to XOML system
	for (int t = 0; t < CIwGameXomlAction_Global::Action_Max; t++)
		IW_GAME_XOML->addAction(new GameXomlAction_Global((GameXomlAction_Global::ActionType)t));

	// Load common resources
	IW_GAME_XOML->Process(this, "Common.xml");

	// Load the introduction
	IW_GAME_XOML->Process(this, "Intro.xml");
//	IW_GAME_XOML->Process(this, "Scene1.xml");

#if defined(ENABLE_ADS)
	// Init Ad Test
	AdTest_Init();

	// Create ads scene
	CIwGameScene* ad_scene = new CIwGameScene();
	ad_scene->Init();
	ad_scene->getSpriteManager()->setBatching(false);
	ad_scene->setName("AdScene");
	ad_scene->setVirtualTransform(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, 0, true, false);
	ad_scene->setClippingArea(-VIRTUAL_SCREEN_WIDTH / 2, -VIRTUAL_SCREEN_HEIGHT / 2, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);
	addScene(ad_scene);
#endif	// ENABLE_ADS

	// Locate the audio resource group
	CIwGameResourceGroup* audio_group = (CIwGameResourceGroup*)IW_GAME_GLOBAL_RESOURCES->getResourceManager()->findResource("Audio", CIwGameXomlNames::ResourceGroup_Hash);

	// Set up audio system
//	IW_GAME_AUDIO->setGroup(audio_group->getResourceGroup());

	// Set up default game data
	for (int t = 0; t < MAX_RECORDS; t++)
	{
		Records[t] = 0;
		Completed[t] = false;
	}
	CurrentZone = 0;
	CurrentRound = 0;
	CurrentScore = 0;

	// Get maximum zones
	CIwGameXomlVariableInt* max_zones = (CIwGameXomlVariableInt*)IW_GAME_GLOBAL_RESOURCES->getVariableManager()->findVariable("MaxZones");
	if (max_zones != NULL)
		MaxZones = max_zones->NativeValue;
	else
		MaxZones = 4;

	// Load game data
	LoadRecords();
	LoadCompleted();

#if defined(ANDROID_MARKET_LICENSE)
	// If Android platform then perform licensing check
	if (s3eDeviceGetInt(S3E_DEVICE_OS) == S3E_OS_ID_ANDROID)
	{
		CIwGameLicenseCheck::Create();
		IW_GAME_LICENSE_CHECK->setPublicKey("MH41gh0kJWs4OkjMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4XObOqxpMX4p9tnOV/memiuY8dhn+8cpjUbho+jYcuIBqF04kyPQEhBAE8MKkUR5Cw38R4EJAnZZ4MFyLfrDGFFSZnA4Xn1jRtbedEsx2y4FO/yndZLGqOHkxDF+T3/ZGkFpg1FB8nhfxlFb8m7nDr0SCg3etMzl5w61dmYkZGjD+L5O3jY++mLoEEWjqyLoa0V54b1la20e0Y/mt3UWiL/I7rapagHe8LjkVfArEtjKofcWZnMiN82rcPLbvKW81jCYfovzmOSHYnT73ct60KHieyGGkTH97fjv3iJNQ8vYwd6k3+Uhi+XdTW5D0p9ciPjmQHO6cVI04Kusp24GmwIDAQAB");
		IW_GAME_LICENSE_CHECK->Init();
	}
#endif // ANDROID_MARKET_LICENSE

}

void Game::resetRecords()
{
	for (int t = 0; t < MAX_RECORDS; t++)
	{
		Records[t] = 0;
		Completed[t] = false;
	}
	Save();
}

bool Game::isZoneComplete(int zone) const
{
	for (int t = zone * 10; t < zone * 10 + 10; t++)
	{
		if (!Completed[t])
			return false;
	}

	return true;
}

bool Game::isAllZonesComplete() const
{
	for (int t = 0; t < MaxZones * 10; t++)
	{
		if (!Completed[t])
			return false;
	}

	return true;
}

void Game::Release()
{
	if (IW_GAME_FACEBOOK != NULL)
	{
		IW_GAME_FACEBOOK->Release();
		CIwGameFacebook::Destroy();
	}

#if defined(ANDROID_MARKET_LICENSE)
	// If Android platform then shut down licensing check
	if (s3eDeviceGetInt(S3E_DEVICE_OS) == S3E_OS_ID_ANDROID)
	{
		IW_GAME_LICENSE_CHECK->Release();
		CIwGameLicenseCheck::Destroy();
	}
#endif // ANDROID_MARKET_LICENSE

#if defined(ENABLE_ADS)
	// Release Ad Test
	AdTest_Release();
#endif	// ENABLE_ADS

	// Release game
	CIwGame::Release();
}

bool Game::Update()
{
	if (!CIwGame::Update())
		return false;

#if defined(ENABLE_ADS)
	// Update the Ad test
	AdTest_Update();
#endif	// ENABLE_ADS

	return true;
}

void Game::Draw()
{
	CIwGame::Draw();
}

void Game::SaveRecords()
{
	// Save records
	CIwGameFile file;
	if (file.Open("\\records.dat", "wb"))
	{
		file.Write((void*)Records, MAX_RECORDS * sizeof(int));
	}
}

void Game::LoadRecords()
{
	// Load records
	CIwGameFile file;
	if (file.Open("\\records.dat", "rb"))
	{
		file.Read((void*)Records, file.getFileSize());
	}
}

void Game::SaveCompleted()
{
	// Save round completed states
	CIwGameFile file;
	if (file.Open("\\completed.dat", "wb"))
	{
		file.Write((void*)Completed, MAX_RECORDS * sizeof(bool));
	}
}

void Game::LoadCompleted()
{
	// Load round completed states
	CIwGameFile file;
	if (file.Open("\\completed.dat", "rb"))
	{
		file.Read((void*)Completed, file.getFileSize());
	}
}

bool Game::LoadFromXoml(bool load_children, CIwGameXmlNode* node)
{
	// Process inner tags
	if (!IW_GAME_XOML->Process(this, node))
		return false;

	return true;
}

void Game::setCurrentScore(int score)
{
	// Update current score
	CurrentScore = score;

	// Update score visual
	GameScene* scene = (GameScene*)findScene("GameScene");
	if (scene != NULL)
	{
		scene->UpdateScore();
	}
}

bool Game::setRecord(int index, int score)
{
	if (score > Records[index])
	{
		Records[index] = score;
		SaveRecords();
		return true;
	}

	return false;
}

bool Game::setCompleted(int index, bool completed)
{
	if (completed != Completed[index])
	{
		Completed[index] = completed;
		SaveCompleted();
		return true;
	}

	return false;
}

void Game::StartRound(int round)
{
	// Get current zone
	CIwGameXomlVariableInt* zone = (CIwGameXomlVariableInt*)IW_GAME_GLOBAL_RESOURCES->getVariableManager()->findVariable("Zone");
	if (zone == NULL)
	{
		// TODO: Notify player that they have completed all zones
		return;
	}

	// Set round data
	CurrentZone = zone->NativeValue;
	CurrentRound = round;
	CurrentScore = 0;

	// Load the rounds scene file
	CIwGameString round_file = "Scene";
	round_file += (CurrentZone - 1) * 10 + round;
	round_file += ".xml";
	IW_GAME_XOML->Process(this, round_file.c_str());
}



//
//
//
//
//  GameXomlAction_Global implementation
//
//
//
//
void GameXomlAction_Global::Execute(IIwGameXomlResource* source, CIwGameAction* action)
{
	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	CIwGameActor* actor = NULL;

	if (source->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
	{
		scene = (CIwGameScene*)source;
		game = scene->getParent();
	}
	else
	if (source->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
	{
		actor = (CIwGameActor*)source;
		scene = actor->getScene();
		game = scene->getParent();
	}

	switch (Type)
	{
	case Action_StartRound:
		{
			CIwGameXomlVariableInt* start_round = (CIwGameXomlVariableInt*)IW_GAME_GLOBAL_RESOURCES->getVariableManager()->findVariable("StartRound");
			if (start_round != NULL)
			{
				// Start the round
				GAME->StartRound(start_round->NativeValue);
			}
		}
		break;
	case Action_NextRound:
		{
			CIwGameXomlVariableInt* start_round = (CIwGameXomlVariableInt*)IW_GAME_GLOBAL_RESOURCES->getVariableManager()->findVariable("StartRound");
			if (start_round != NULL)
			{
				if (start_round->NativeValue == 10 && GAME->isAllZonesComplete())	// Check for game complete
				{
					IW_GAME_XOML->Process(GAME, "GameComplete.xml");
				}
				else
				if (start_round->NativeValue == 10 && (!GAME->isZoneComplete(GAME->getCurrentZone() - 1)))	// Check for locked zone
				{
					IW_GAME_XOML->Process(GAME, "ZoneLocked.xml");
				}
				else
				{
					// Update zone
					int round = start_round->NativeValue + 1;
					if (round > 10)
					{
						round = 1;
						GAME->setCurrentZone(GAME->getCurrentZone() + 1);
						CIwGameXomlVariableInt* zone = (CIwGameXomlVariableInt*)IW_GAME_GLOBAL_RESOURCES->getVariableManager()->findVariable("Zone");
						if (zone != NULL)
							zone->setValue(CIwGameString(GAME->getCurrentZone()).c_str());
					}

					// Update StartRound
					start_round->setValue(CIwGameString(round).c_str());

					// Start the round
					GAME->StartRound(round);
				}
			}
		}
		break;
	case Action_Shoot:
		{
			GameScene* scene = (GameScene*)GAME->findScene("GameScene");
			if (scene != NULL)
			{
				((GameScene*)scene)->Shoot();
			}
		}
		break;
	case Action_ResetRecords:
		{
			GAME->resetRecords();
		}
		break;
	case Action_PostFacebook:
		{
			if (IW_GAME_FACEBOOK == NULL)
			{
				CIwGameFacebook::Create();
				IW_GAME_FACEBOOK->Init();
				IW_GAME_FACEBOOK->setAppID("Put your own App ID here");
			}

			if (IW_GAME_FACEBOOK->isAvailable())
			{
				CIwGameString message = "I just scored ";
				message += GAME->getCurrentScore();
				message += " points on round ";
				message += GAME->getCurrentRound();
				message += " (zone ";
				message += GAME->getCurrentZone();
				message += ") playing cOnnecticOns!";
				if (s3eDeviceGetInt(S3E_DEVICE_OS) == S3E_OS_ID_ANDROID)
					IW_GAME_FACEBOOK->PostWall(message.c_str(), "http://appliter.com/apps/connecticons/connecticons.aspx", "http://appliter.com/apps/connecticons/icon_135x135.png", "cOnnecticOns Android", "Get your copy of this amazing physics puzzler game on your Android today");
				else
				if (s3eDeviceGetInt(S3E_DEVICE_OS) == S3E_OS_ID_IPHONE)
					IW_GAME_FACEBOOK->PostWall(message.c_str(), "http://appliter.com/apps/connecticons/connecticons.aspx", "http://appliter.com/apps/connecticons/icon_135x135.png", "cOnnecticOns iPhone", "Get your copy of this amazing physics puzzler game on your iPhone, iPad and iPod today");
			}
		}
		break;
	}
}
