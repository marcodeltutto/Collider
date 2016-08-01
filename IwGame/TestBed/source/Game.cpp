#include "s3e.h"
#include "Iw2D.h"
#include "IwGx.h"
#include "IwResManager.h"

#include "iwGameAdsView.h"
#include "iwGameAdsViewAnimator.h"

#include "Game.h"
#include "Common.h"
#include "ActorPlayer.h"
#include "TestActor.h"

CDECLARE_SINGLETON(Game)

//
//
//
// Game singleton implementation
//
//
//

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

void Game::Init(bool enable_http)
{
	// Init base game
	CIwGame::Init(enable_http);

	PrevTouched = false;

	// Add custom classes to XOML system
	IW_GAME_XOML->addClass(new TestActorCreator());

	// Load a few test XONL files
	IW_GAME_XOML->Process(this, "Scene1.xml");

	// Init Ad Test
	AdTest_Init();

	// Create ads scene
	CIwGameScene* ad_scene = new CIwGameScene();
	ad_scene->Init();
	ad_scene->setName("AdScene");
	ad_scene->setVirtualTransform(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, 0, true, false);
	ad_scene->setClippingArea(-VIRTUAL_SCREEN_WIDTH / 2, -VIRTUAL_SCREEN_HEIGHT / 2, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);
	addScene(ad_scene);

	// Locate the audio resource group
	CIwGameResourceGroup* audio_group = (CIwGameResourceGroup*)IW_GAME_GLOBAL_RESOURCES->getResourceManager()->findResource("Audio", CIwGameXomlNames::ResourceGroup_Hash);

	// Set up audio system
	IW_GAME_AUDIO->setGroup(audio_group->getResourceGroup());

	// Locate the scene called GameScene
	CIwGameScene* game_scene = findScene("GameScene");

	// Create a camera and attach it to the scene
	CIwGameCamera* game_camera = new CIwGameCamera();
	game_scene->setCamera(game_camera);

	// Check to see if MP3 codec is supported
	if (IW_GAME_AUDIO->isMusicCodecSupported(S3E_AUDIO_CODEC_MP3))
	{
		// Play some music
//		IW_GAME_AUDIO->PlayMusic("music.mp3");	// Commented out because its incredibly annoying!
	}
}

void Game::Release()
{
	// Release Ad Test
	AdTest_Release();

	// Release game
	CIwGame::Release();
}

bool Game::Update()
{
	if (!CIwGame::Update())
		return false;

	if (IW_GAME_INPUT->getTouchCount() > 0)
	{
		if (!PrevTouched)
		{
			CIwGameScene* scene = findScene("GameScene");
			if (scene != NULL)
			{
				// Get tapped position in our virtual screen space
				CIwFVec2 pos = scene->ScreenToVirtual((float)IW_GAME_INPUT->getTouch(0)->x, (float)IW_GAME_INPUT->getTouch(0)->y);
				// Find our sprite atlas
				CIwGameImage* image = (CIwGameImage*)scene->getResourceManager()->findResource("sprites", CIwGameXomlNames::Image_Hash);
				// Find our manic face animation frames
				CIwGameAnim* anim = (CIwGameAnim*)scene->getResourceManager()->findResource("PlayerImageAnim", CIwGameXomlNames::Animation_Hash);

				// Create 10 player actors
				for (int t = 0; t < 10; t++)
				{
					ActorPlayer::Create((int)pos.x, (int)pos.y, scene, image, anim);
				}

				// Play a sound effect
				IW_GAME_AUDIO->PlaySound("explosion");
			}
		}
		PrevTouched = true;
	}
	else
		PrevTouched = false;

	// Update the Ad test
	AdTest_Update();

	return true;
}

void Game::Draw()
{
	CIwGame::Draw();
}

void Game::Save()
{
}

void Game::Load()
{
}

bool Game::LoadFromXoml(bool load_children, CIwGameXmlNode* node)
{
	// Process inner tags
	if (!IW_GAME_XOML->Process(this, node))
		return false;

	return true;
}
