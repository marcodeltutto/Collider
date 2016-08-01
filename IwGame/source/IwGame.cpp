// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. You must ensure that this whole notice is present in all files and derivatives, so the reader knows its origin.
// If you use this engine in your product then please ensure that you credit IwGame's usage appropriately. Please see http://www.drmop.com/index.php/iwgame-engine/ for licensing details and support
//
//

#include "IwGame.h"
#include "IwGameRender2d.h"
#include "IwGameModifier.h"
#include "IwGameProgram.h"
#include "IwGameUI.h"
#include "IwGameFile.h"
#include "IwGameTemplates.h"
#include "IwGameSettings.h"
#include "IwGameCam.h"
#include "IwGamePlatform.h"

//
//
//
// IwGame implementation
//
//
//
void CIwGame::Init(bool enable_http, bool enable_extensions, bool gl_conmpatible)
{
	setClassType("Game");
	GLCompatible = gl_conmpatible;
	CurrentScene = NULL;
	NextScene = NULL;
	for (int t = 0; t < IW_GAME_MAX_TOUCHES; t++)
	{
		TouchFocus[t] = NULL;
		LastTappedActor[t] = NULL;
	}
	KeyFocus = NULL;
	TouchPanEnabled = true;
	MaxTouches = 2;

	// Retrieve AppEasy settings
	CIwGameSettings::Create();
	IW_GAME_SETTINGS->Init();

	CIwGameError::Init();

	// Initialise 2D renderer
	CIwGameRender2d::Create();
	IW_GAME_RENDER2D->Init();

	// Initialise the http manager (only if requested and not already initialised)
	if (enable_http && IW_GAME_HTTP_MANAGER == NULL)
	{
		CIwGameHttpManager::Create();
		IW_GAME_HTTP_MANAGER->Init();
	}

	// Initialise the input system
	CIwGameInput::Create();
	IW_GAME_INPUT->Init();

	// Initialise the rsource manager (TODO: Remove support)
	PLATFORM_OTHER->InitResManager();

	// Initialise audio system
	CIwGameAudio::Create();
	IW_GAME_AUDIO->Init();

	// Set default background colour
	SetBackgroundColour(0, 0, 0, 0);

	// Get initial time stamp
	LastFrameTime = PLATFORM_SYS->getTimeInMs();

	// Initialise the global resource system
	CIwGameGlobalResources::Create();
	IW_GAME_GLOBAL_RESOURCES->Init(this);

	// Initialise modifiers system
	CIwGameMods::Create();
	IW_GAME_MODS->Init();

	// Initialise programs system
	CIwGamePrograms::Create();
	IW_GAME_PROGRAMS->Init();

	// Init the XOML system
	CIwGameXoml::Create();
	IW_GAME_XOML->Init();
	if (enable_extensions)
		IW_GAME_XOML->InitExtensions();

	// Create system array variables
	CreateSystemArray();

	// Create global UI animations
	CIwGameUIBase::InitPresetAnimations();
}

void CIwGame::CreateSystemArray()
{
	IIwGamePlatformDisplay* display = PLATFORM_DISPLAY;

	// Create the system variable array
	int width = display->getCurrentWidth();
	int height = display->getCurrentHeight();
	CIwGameXomlVariableArrayInt* system = new CIwGameXomlVariableArrayInt();
	system->Init(IW_GAME_SYS_MAX);
	system->Name = "system";
	system->setValue(IW_GAME_SYS_SCREEN_WIDTH, CIwGameString(width).c_str());
	system->setValue(IW_GAME_SYS_SCREEN_HEIGHT, CIwGameString(height).c_str());
	system->setValue(IW_GAME_SYS_SCREEN_SIZE_HINT, CIwGameString((width + height) / 400).c_str());
	system->setValue(IW_GAME_SYS_SCREEN_MODE, CIwGameString(CIwGameUtils::GetGraphicModeIndex(width, height)).c_str());
	system->setValue(IW_GAME_SYS_DEVICE_TYPE, CIwGameString((int)CIwGameUtils::GetDeviceType()).c_str());
	if (IW_GAME_INPUT->isMultiTouch())
		system->setValue(IW_GAME_SYS_HAS_MULTITOUCH, "1");
	else
		system->setValue(IW_GAME_SYS_HAS_MULTITOUCH, "0");
	if (IW_GAME_INPUT->isAccelerometerAvailable())
		system->setValue(IW_GAME_SYS_HAS_ACCELEROMETER, "1");
	else
		system->setValue(IW_GAME_SYS_HAS_ACCELEROMETER, "0");
	if (IW_GAME_INPUT->isCompassAvailable())
		system->setValue(IW_GAME_SYS_HAS_COMPASS, "1");
	else
		system->setValue(IW_GAME_SYS_HAS_COMPASS, "0");
	if (IW_GAME_INPUT->isKeysAvailable())
		system->setValue(IW_GAME_SYS_HAS_KEYS, "1");
	else
		system->setValue(IW_GAME_SYS_HAS_KEYS, "0");
	if (IW_GAME_INPUT->isPointerAvailable())
		system->setValue(IW_GAME_SYS_HAS_POINTER, "1");
	else
		system->setValue(IW_GAME_SYS_HAS_POINTER, "0");
	IW_GAME_GLOBAL_RESOURCES->getVariableManager()->addVariable(system);

	// Create the touch input system array
	CIwGameXomlVariableArrayInt* touches = new CIwGameXomlVariableArrayInt();
	touches->Init(IW_GAME_TOUCHES_MAX);
	touches->Name = "touches";
	for (int t = 0; t < IW_GAME_TOUCHES_MAX; t++)
		touches->setValue(t, "0");
	IW_GAME_GLOBAL_RESOURCES->getVariableManager()->addVariable(touches);

}

void CIwGame::UpdateSystemTouches()
{
	CIwGameXomlVariableArrayInt* arr = (CIwGameXomlVariableArrayInt*)IW_GAME_GLOBAL_RESOURCES->getVariableManager()->findVariable("touches");

	// Update system array
	int count = 1;
	if (IW_GAME_INPUT->isMultiTouch())
		count = 5;
	CIwGameString num;
	num.setAutoHash(false);
	int index = 0;
	for (int t = 0; t < 5; t++)
	{
		CIwGameXomlVariableInt* var = (CIwGameXomlVariableInt*)arr->getElement(index++);
		CIwGameTouch* touch = IW_GAME_INPUT->getTouch(t);
		if (touch->x != var->NativeValue)
		{
			var->NativeValue = touch->x;
			num = var->NativeValue;
			var->setValueText(num.c_str());
		}
		var = (CIwGameXomlVariableInt*)arr->getElement(index++);
		if (touch->y != var->NativeValue)
		{
			var->NativeValue = touch->y;
			num = var->NativeValue;
			var->setValueText(num.c_str());
		}
		var = (CIwGameXomlVariableInt*)arr->getElement(index++);
		if ((int)touch->touched != var->NativeValue)
		{
			var->NativeValue = (int)touch->touched;
			num = var->NativeValue;
			var->setValueText(num.c_str());
		}
	}
}

void CIwGame::SetBackgroundColour(uint8 r, uint8 g, uint8 b, uint8 a)
{
	// Set the default background clear colour
	PLATFORM_DISPLAY->setColour(r, b, g, a);
}

void CIwGame::Release()
{
	// Clean up settings
	IW_GAME_SETTINGS->Release();
	CIwGameSettings::Destroy();

	// Reset preset animations
	CIwGameUIBase::ReleasePresetAnimations();

	// Cleans up the modifiers system
	IW_GAME_MODS->Release();
	CIwGameMods::Destroy();

	// Cleans up the programs system
	IW_GAME_PROGRAMS->Release();
	CIwGamePrograms::Destroy();

	// Clean up the XOML system
	IW_GAME_XOML->Release();
	CIwGameXoml::Destroy();

	// Clean up scenes
	for (_Iterator it = Scenes.begin(); it != Scenes.end(); ++it)
		delete *it;
	Scenes.clear();
	SceneRemovals.clear();

	// Shut down global resource system
	IW_GAME_GLOBAL_RESOURCES->Release();
	CIwGameGlobalResources::Destroy();

	// Shut down the input system
	IW_GAME_INPUT->Release();
	CIwGameInput::Destroy();

	// Shut down the resource manager
	PLATFORM_OTHER->ReleaseResManager();

	// Shut down audio
	IW_GAME_AUDIO->Release();
	CIwGameAudio::Destroy();

	// Shut down http manager
	if (IW_GAME_HTTP_MANAGER != NULL)
	{
		IW_GAME_HTTP_MANAGER->Release();
		CIwGameHttpManager::Destroy();
	}

	// Shut down 2D renderer
	IW_GAME_RENDER2D->Release();
	CIwGameRender2d::Destroy();
}

bool CIwGame::Update()
{
	IIwGamePlatformDisplay* display = PLATFORM_DISPLAY;

	// If power saving mode disabled then turn back light on
	if (IW_GAME_SETTINGS->getPreventPowerSaving())
		PLATFORM_SYS->BacklightOn();

	// Calculate how long the last game frame took (capped at 60 and 10 fps) - We use this to scale all of our trasient variables that rely upon time so that everything moves at the same rate
	// regardless of our devices frame rate
	float dt = (float)(PLATFORM_SYS->getTimeInMs() - LastFrameTime) / FRAME_SPEED_LOCK_MS;
	if (dt < 0.00001f) dt = 0.00001f;
	if (dt > 10.0f) dt = 10.0f;
	LastFrameTime = PLATFORM_SYS->getTimeInMs();

	// Clear the screen
	display->Clear();

	// Update input system
	IW_GAME_INPUT->Update();
	UpdateSystemTouches();

	// Check for device orientation change
	int screen_width = display->getCurrentWidth();
	int screen_height = display->getCurrentHeight();
	if (screen_width != ScreenSize.x || screen_height != ScreenSize.y)
	{
		CIwGameUIBase::UpdatePresetAnimations();
		ScreenSize.x = screen_width;
		ScreenSize.y = screen_height;
	}

	// Update all global variables
	IW_GAME_GLOBAL_RESOURCES->getVariableManager()->Update();

	// Update all scene local variables
	for (_Iterator it = Scenes.begin(); it != Scenes.end(); ++it)
		(*it)->getVariableManager()->Update();

	// Run any global programs
	IW_GAME_GLOBAL_RESOURCES->getProgrameManager()->Execute(dt);

	// Check for scene change
	// Notify scenes that there is a change of circumstances
	if (CurrentScene != NextScene)
	{
		if (CurrentScene != NULL)
		{
			CurrentScene->NotifyLostFocus(NextScene);
			if (CurrentScene->getAllowSuspend())
				CurrentScene->NotifySuspending(NextScene);
		}
		if (NextScene != NULL)
		{
			NextScene->NotifyGainedFocus(CurrentScene);
			if (NextScene->getAllowSuspend())
				NextScene->NotifyResuming(CurrentScene);
		}
		CurrentScene = NextScene;
	}

	// Update all scenes that are not suspended
	// Note that because actions taken in scenes can change the order of scenes in the Scenes list we need to create a local termporary list that does not get updated
	SceneList.clear();
	for (_Iterator it = Scenes.begin(); it != Scenes.end(); ++it)
		SceneList.add(*it);
	for (int t = 0; t < SceneList.getSize(); t++)
	{
		CIwGameScene* s = SceneList.element_at(t);
		if (s != NULL)
		{
			if (CurrentScene == s)
				s->Update(dt);
			else
			{
				if (!s->getAllowSuspend())
					s->Update(dt);
			}
		}
	}

	// Handle touch event
	if (CurrentScene != NULL)
		ProcessEvents(CurrentScene);

	// Process back key event for key focus actor
	if (KeyFocus != NULL && IW_GAME_INPUT->isKeysAvailable())
	{
		if (IW_GAME_INPUT->isBackPressed())
			KeyFocus->OnBackKey();
		if (IW_GAME_INPUT->isMenuPressed())
			KeyFocus->OnMenuKey();
	}

	// Update http manager
	if (IW_GAME_HTTP_MANAGER != NULL)
	{
		IW_GAME_HTTP_MANAGER->Update();
	}

	CIwGameAnimTimeline::UpdateRunCount();

	// Update audio
	IW_GAME_AUDIO->Update();

	// Remove any scenes that got deleted last frame
	for (_Iterator it = SceneRemovals.begin(); it != SceneRemovals.end(); ++it)
	{
		for (_Iterator it2 = Scenes.begin(); it2 != Scenes.end(); ++it2)
		{
			if (*it2 == *it)
			{
				// If scene was current scene and had touch or key focus then reset the focus
				if (CurrentScene == *it2)
				{
					for (int t = 0; t < IW_GAME_MAX_TOUCHES; t++)
					{
						if (TouchFocus[t] != NULL && TouchFocus[t]->getScene() == CurrentScene)
							TouchFocus[t] = NULL;
					}
					if (KeyFocus != NULL && KeyFocus->getScene() == CurrentScene)
						KeyFocus = NULL;
					CurrentScene = NULL;
				}
				if (NextScene == *it2)
					NextScene = NULL;
				delete *it2;
				Scenes.erase(it2);
				break;
			}
		}
	}
	SceneRemovals.clear();

	return true;
}

CIwGameActor* CIwGame::FindTappedActor(int x, int y)
{
	CIwGameActor* hit_actor = NULL;
	CIwGameScene* hit = NULL;
	for (_Iterator it = Scenes.begin(); it != Scenes.end(); ++it)
	{
		if (CurrentScene == *it ||  (*it)->getAllowFocus())
		{
			if (hit == NULL || (hit->getLayer() <= (*it)->getLayer()))
			{
				if (!(*it)->isDragging())
				{
					CIwGameActor* tapped_actor = (*it)->FindTappedActor(x, y);
					CIwGameScene* new_hit = *it;
					if (tapped_actor != NULL)
					{
						hit_actor = tapped_actor;
						hit = *it;
					}
				}
			}
		}
	}

	return hit_actor;
}

void CIwGame::releaseTouchFocus(int index, int x, int y, bool allow_bubble)
{
	if (TouchFocus[index] != NULL)
	{
		if (TouchFocus[index]->getTouchIndex() == index)
		{
			TouchFocus[index]->OnEndTouch(index, x, y, allow_bubble);
			TouchFocus[index] = NULL;
		}
	}
}

void CIwGame::ProcessTouch(CIwGameScene *scene, int index)
{
	CIwGameTouch* touch_info = IW_GAME_INPUT->getTouch(index);
	CIwVec2 pos(touch_info->x, touch_info->y);

	// Handle begin touch event
	if (TouchFocus[index] == NULL)
	{
		if (!touch_info->prev_active && touch_info->active)
		{
			CIwGameActor* tapped_actor = FindTappedActor(pos.x, pos.y);
			if (tapped_actor != NULL)
			{
				tapped_actor->OnBeginTouch(index, pos.x, pos.y, true);
				TouchFocus[index] = tapped_actor;
			}
		}
	}
	else
	{
		// If user moves finger too far frmo actor then release it
		if (TouchFocus[index]->isOutsideFocusRange((float)pos.x, (float)pos.y, 1.0f))
		{
			releaseTouchFocus(index, pos.x, pos.y, true);
		}
	}

	// Handle end touch event (only actor that received begin touch event needs to handle this)
	if (touch_info->prev_active && !touch_info->active)
	{
		if (TouchFocus[index] != NULL)
		{
			if (TouchFocus[index]->HitTest((float)pos.x, (float)pos.y))
			{
				TouchFocus[index]->OnTapped(index, pos.x, pos.y, true);
				LastTappedActor[index] = TouchFocus[index];
			}
			TouchFocus[index]->OnEndTouch(index, pos.x, pos.y, true);
		}
		TouchFocus[index] = NULL;
	}
}

void CIwGame::ProcessEvents(CIwGameScene *scene)
{
	if (scene == NULL)
		return;

	LastTappedActor[0] = NULL;
	LastTappedActor[1] = NULL;
	if (!scene->isActive() || !scene->isVisible())
	{
		return;
	}

	// Update keys
	if (IW_GAME_INPUT->isBackPressed())
		scene->NotifyKeyBack();
	if (IW_GAME_INPUT->isMenuPressed())
		scene->NotifyKeyMenu();

	for (int t = 0; t < MaxTouches; t++)
		ProcessTouch(scene, t);
}

void CIwGame::Draw()
{
	IW_GAME_RENDER2D->Begin();

	// Calculate max layers, also draw any scenes that have negative layers
	int max_layers = 0;
	for (_Iterator it = Scenes.begin(); it != Scenes.end(); ++it)
	{
		if (!(*it)->isDestroyed())
		{
			int layer = (*it)->getLayer();
			if (layer < 0)
				(*it)->Draw();
			else
			if (layer < IW_GAME_MAX_SCENE_LAYERS && layer > max_layers)
				max_layers = layer;
		}
	}

	// Draw all scenes in layer order
	int num_scenes = Scenes.size();
	for (int t = 0; t <= max_layers; t++)
	{
		for (_Iterator it = Scenes.begin(); it != Scenes.end(); ++it)
		{
			if (!(*it)->isDestroyed())
			{
				if ((*it)->getLayer() == t)
					(*it)->Draw();
			}
		}
	}
	// Draw any scenes that have layers that are out of range last
	for (_Iterator it = Scenes.begin(); it != Scenes.end(); ++it)
	{
		if (!(*it)->isDestroyed())
		{
			int layer = (*it)->getLayer();
			if (layer >= IW_GAME_MAX_SCENE_LAYERS)
				(*it)->Draw();
		}
	}

	// Take care of any post draw rendering
	PostDraw();

	IW_GAME_RENDER2D->End();

	// Swap display buffers
	PLATFORM_DISPLAY->Swap();

	// Yield to the operating system
	PLATFORM_SYS->YieldToOS(0);
}

void CIwGame::PostDraw()
{
}

void CIwGame::addScene(CIwGameScene *scene, bool bring_to_front)
{
	Scenes.push_back(scene);
	scene->setParent(this);

	// If another scene has focus then bring it back to the top of the scene stack
	if (CurrentScene != NULL && !bring_to_front)
	{
		BringSceneToFront(CurrentScene);
	}
}

void CIwGame::removeScene(CIwGameScene* scene)
{
	// Mark scene for removal
	 if (!scene->isDestroyed())
	 {
		SceneRemovals.push_back(scene);
		scene->setDestroyed(true);
		scene->PreDestroy();
	 }
}

void CIwGame::removeScene(unsigned int name_hash)
{
	// Find the scene
	for (_Iterator it = Scenes.begin(); it != Scenes.end(); ++it)
	{
		if ((*it)->getNameHash() == name_hash && !(*it)->isDestroyed())
		{
			// Mark scene for removal
			SceneRemovals.push_back(*it);
			(*it)->setDestroyed(true);
			(*it)->PreDestroy();
			break;
		}
	}
}

void CIwGame::removeAllScenes(CIwGameScene* exclude_scene)
{
	// Mark scenes for removal
	for (_Iterator it = Scenes.begin(); it != Scenes.end(); ++it)
	{
		if (*it != exclude_scene)
		{
			// Mark scene for removal
			SceneRemovals.push_back(*it);
			(*it)->setDestroyed(true);
			(*it)->PreDestroy();
		}
	}
}

void CIwGame::removeAllScenes(unsigned int exclude_name_hash)
{
	removeAllScenes(findScene(exclude_name_hash));
}

CIwGameScene* CIwGame::findScene(unsigned int name_hash)
{
	for (_Iterator it = Scenes.begin(); it != Scenes.end(); ++it)
	{
//		if ((*it)->getNameHash() == name_hash && !(*it)->isDestroyed())
		if ((*it)->getNameHash() == name_hash && !(*it)->isDestroyed())
		{
			return *it;
		}
	}

	return NULL;
}

CIwGameScene* CIwGame::findScene(const char* name)
{
	return findScene(CIwGameString::CalculateHash(name));
}

CIwGameScene* CIwGame::findScene(int type)
{
	for (_Iterator it = Scenes.begin(); it != Scenes.end(); ++it)
	{
		if ((*it)->getType() == type)
		{
			return *it;
		}
	}

	return NULL;
}

CIwGameScene* CIwGame::getScene(int index)
{
	if (index < 0 || index >= Scenes.size())
		return NULL;

	return Scenes.element_at(index);
}

void CIwGame::clearScenes()
{
	CIwGameError::LogError("ERROR: CIwGame::clearScenes() is deprecated please use CIwGame::removeAllScenes() instead");

/*	for (_Iterator it = Scenes.begin(); it != Scenes.end(); ++it)
		delete *it;

	Scenes.clear();*/
}

void CIwGame::changeScene(CIwGameScene *new_scene)
{
	NextScene = new_scene;
}

bool CIwGame::changeScene(unsigned int name_hash)
{
	NextScene = findScene(name_hash);
	if (NextScene == NULL)
		return false;

	return true;
}

void CIwGame::BringSceneToFront(CIwGameScene* scene)
{
	for (_Iterator it = begin(); it != end(); it++)
	{
		if (*it == scene)
		{
			Scenes.erase(it);
			Scenes.push_back(scene);
			break;
		}
	}
}

void CIwGame::DisableFocus()
{
	for (_Iterator it = begin(); it != end(); it++)
	{
		(*it)->setAllowFocus(false);
	}
}

void CIwGame::SetAllTimelines(CIwGameAnimTimeline* timeline)
{
	for (_Iterator it = begin(); it != end(); it++)
	{
		if (!(*it)->isDestroyed())
		{
			(*it)->setTimeline(timeline);
		}
	}
}

bool CIwGame::LoadFromXoml(IIwGameXomlResource* parebt, bool load_children, CIwGameXmlNode* node)
{
	// Process inner tags
	IW_GAME_XOML->Process(this, node);

	return true;
}

//
//
//
//
// CIwGameCreator implementation
//
//
//
//
IIwGameXomlResource* CIwGameCreator::CreateInstance(IIwGameXomlResource* parent)
{
	return NULL;
}

//
//
//
//
//  CIwGameXomlAction_Global implementation
//
//
//
//
void CIwGameXomlAction_Global::Execute(IIwGameXomlResource* source, CIwGameAction* action)
{
	if (source == NULL)
		return;

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
	else
	if (source->getClassTypeHash() == CIwGameXomlNames::Game_Hash)
	{
		game = (CIwGame*)source;
	}
	IIwGameXomlResource* cont = (actor != NULL) ? (IIwGameXomlResource*)actor : (IIwGameXomlResource*)scene;

	switch (Type)
	{
	case Action_PlayTimeline:
	case Action_StopTimeline:
	case Action_SetTimeline:
	case Action_PauseTimeline:
		{
			CIwGameAnimTimeline* timeline = NULL;
			if (!action->Params[1].IsEmpty())
			{
				// If user provided second paremater then take that as a target actor or scene
				IIwGameXomlResource* other = scene->findActor(action->getParameter2(cont).getHash());
				if (other == NULL)
				{
					actor = NULL;
					scene = game->findScene(action->getParameter2(cont).getHash());
				}
				else
				{
					actor = (CIwGameActor*)other;
					scene = actor->getScene();
				}
			}
			if (!action->Params[0].IsEmpty())
			{
				if (actor != NULL)
					timeline = CIwGameAnimTimelinesManager::FindTimeline(action->getParameter1(cont).getHash(), actor);
				else
					timeline = CIwGameAnimTimelinesManager::FindTimeline(action->getParameter1(cont).getHash(), scene);
			}

			if (actor != NULL)
			{
				if (timeline == NULL)
				{
					if (Type == Action_SetTimeline)
					{
						actor->setTimeline(NULL);
						return;
					}
					else
						timeline = actor->getTimeline();
				}
				if (timeline == NULL)
				{
					CIwGameError::LogError("Warning: Actions - Actor timeline was not found - ", action->getParameter1(cont).c_str());
					return;
				}
				if (Type == Action_SetTimeline)
				{
					actor->setTimeline(timeline);
					timeline->restart();
				}
			}
			else
			if (scene != NULL)
			{
				if (timeline == NULL)
				{
					if (Type == Action_SetTimeline)
					{
						scene->setTimeline(NULL);
						return;
					}
					else
						timeline = scene->getTimeline();
				}
				if (timeline == NULL)
				{
					CIwGameError::LogError("Warning: Actions - Scene timeline was not found - ", action->getParameter1(cont).c_str());
					return;
				}
				if (Type == Action_SetTimeline)
				{
					scene->setTimeline(timeline);
					timeline->restart();
				}
			}
			else
			{
				CIwGameError::LogError("Warning: Actions - No valid target scene or actor - ", action->getParameter1(cont).c_str());
				return;
			}

			if (Type == Action_PlayTimeline)
			{
//				timeline->restart();
				timeline->play();
			}
			else
			if (Type == Action_StopTimeline)
				timeline->stop();
			else
			if (Type == Action_PauseTimeline)
				timeline->pause();
		}
		break;
	case Action_PlaySound:
		if (!action->Params[0].IsEmpty())
			IW_GAME_AUDIO->PlaySound(action->getParameter1(cont).getHash());
		break;
	case Action_PlayMusic:
		if (!action->Params[0].IsEmpty())
		{
			if (action->Params[1].IsEmpty())
				IW_GAME_AUDIO->PlayMusic(action->getParameter1(cont).c_str(), 1);
			else
				IW_GAME_AUDIO->PlayMusic(action->getParameter1(cont).c_str(), action->getParameter2(cont).GetAsInt());
		}
		break;
	case Action_StopMusic:
		IW_GAME_AUDIO->StopMusic();
		break;
	case Action_LoadXOML:
		{
			if (!action->Params[0].IsEmpty())
			{
				IIwGameXomlResource* parent = NULL;
				if (!action->Params[1].IsEmpty())
					parent = scene->getResourceManager()->findResource(action->getParameter2(cont).getHash(), CIwGameXomlNames::Scene_Hash);
				if (parent == NULL)
					parent = game;
				IW_GAME_XOML->Process(parent, action->getParameter1(cont).c_str());
			}
		}
		break;
	case Action_FromTemplate:
		{
			if (!action->Params[0].IsEmpty())
			{
				if (!action->Params[2].IsEmpty())
				{
					CIwGameScene* target_scene = game->findScene(action->getParameter3(cont).getHash());
					CIwGameXomlLoad::CreateInstance(scene, target_scene, &action->getParameter1(cont), &action->getParameter2(cont));
				}
				else
					CIwGameXomlLoad::CreateInstance(scene, scene, &action->getParameter1(cont), &action->getParameter2(cont));
			}
		}
		break;
	case Action_SetVariable:
		{
			if (!action->Params[2].IsEmpty())
			{
				CIwGameScene* new_scene = game->findScene(action->getParameter3(cont).c_str());
				if (new_scene != NULL)
					scene = new_scene;
#if defined (_DEBUG)
				else
					CIwGameError::LogError("Warning: Actions - SetVariable - Invalid scene - ", action->Params[2].c_str());
#endif
			}

			// Set the variables value
			CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(action->Params[0], scene);
			if (var != NULL)
			{
				CIwGameString& p2 = action->getParameter2(cont);
				unsigned int hash = p2.getHash();
				if (hash == CIwGameXomlNames::Rand_Hash || hash == CIwGameXomlNames::RandChar_Hash)
				{
					CIwGameString rnd;
					rnd.setAutoHash(false);
					CIwGameString& p4 = action->getParameter4(cont);
					CIwGameString& p5 = action->getParameter5(cont);
					float min, max;

					if (hash == CIwGameXomlNames::Rand_Hash)
					{
						min = p4.GetAsFloat();
						max = p5.GetAsFloat();
					}
					else
					{
						min = (float)p4[0];
						max = (float)p5[0];
					}

					if (var->isArray())
					{
						CIwGameXomlVariableArray* arr = (CIwGameXomlVariableArray*)var;
						for (int t = 0; t < arr->getSize(); t++)
						{
							CIwGameXomlVariable* var2 = arr->getElement(t);
							float num = min + (((float)rand() * (max - min)) / RAND_MAX);

							if (hash == CIwGameXomlNames::Rand_Hash)
							{
								if (var2->Type == VT_Bool)
									rnd = (bool)(num > 0.5f);
								else
								if (var2->Type == VT_Int)
									rnd = (int)num;
								else
									rnd = num;
							}
							else
								rnd = (char)num;
							var2->setValue(rnd.c_str());
						}
					}
					else
					{
						float num = min + (((float)rand() * (max - min)) / RAND_MAX);
						if (hash == CIwGameXomlNames::Rand_Hash)
						{
							if (var->Type == VT_Bool)
								rnd = (bool)(num > 0.5f);
							else
							if (var->Type == VT_Int)
								rnd = (int)num;
							else
								rnd = num;
						}
						else
							rnd = (char)num;
						var->setValue(rnd.c_str());
					}
				}
				else
					var->setValue(p2.c_str());
			}
#if defined (_DEBUG)
			else
				CIwGameError::LogError("Warning: Actions - SetVariable - Invalid target variable - ", action->Params[0].c_str());
#endif
		}
		break;
	case Action_AddVariable:
		{
			if (!action->Params[3].IsEmpty())
			{
				CIwGameScene* new_scene = game->findScene(action->getParameter4(cont).c_str());
				if (new_scene != NULL)
					scene = new_scene;
#if defined (_DEBUG)
				else
					CIwGameError::LogError("Warning: Actions - AddVariable - Invalid scene - ", action->getParameter4(cont).c_str());
#endif
			}

			// Set the variables value
			CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(action->Params[0], scene);
			if (var != NULL)
			{
				if (action->Params[2].IsEmpty())
					var->addValue(action->getParameter2(cont).c_str());
				else
					var->addValue(action->getParameter2(cont).c_str(), action->getParameter3(cont).c_str());
			}
#if defined (_DEBUG)
			else
				CIwGameError::LogError("Warning: Actions - AddVariable - Invalid target variable - ", action->Params[0].c_str());
#endif
		}
		break;
	case Action_Launch:
		{
			if (!action->Params[0].IsEmpty())
				PLATFORM_SYS->ExecuteViaOS(action->getParameter1(cont).c_str(), false);
		}
		break;
	case Action_SetBGColour:
		{
			int components[8];
			if (action->getParameter1(cont).GetAsListOfInt(components) == 4)
				game->SetBackgroundColour((uint8)components[0], (uint8)components[1], (uint8)components[2], (uint8)components[3]);
			else
				CIwGameError::LogError("Warning: Actions - SetBGColour - Incorrect component count - ", action->getParameter1(cont).c_str());
		}
		break;
	case Action_SetCurrentScene:
		{
			CIwGameScene* scene = game->findScene(action->getParameter1().getHash());
			if (scene != NULL)
			{
				game->changeScene(scene);
				game->BringSceneToFront(scene);
			}
			else
				CIwGameError::LogError("Warning: Actions - SetCurrentScene - scene not found - ", action->getParameter1(cont).c_str());
		}
		break;
	case Action_BringtSceneToFront:
		{
			CIwGameScene* scene = game->findScene(action->getParameter1().getHash());
			if (scene != NULL)
				game->BringSceneToFront(scene);
			else
				CIwGameError::LogError("Warning: Actions - BringtSceneToFront - scene not found - ", action->getParameter1(cont).c_str());
		}
		break;
	case Action_EnterValue:	// EnterValue Message To User, Variable to place value in
		{
			CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(action->Params[1], scene);
			if (var != NULL)
			{
				const char* str = IW_GAME_INPUT->showOnScreenKeyboard(action->getParameter1(cont).c_str(), 0, (action->getParameter3(cont) == "0") ? NULL : var->getValue().c_str());
				if (str != NULL)
					var->setValue(str);
			}
			else
				CIwGameError::LogError("Warning: Actions - Enter Value - Target variable not found - ", action->getParameter2(cont).c_str());
		}
		break;
	case Action_UpdateText:		// UpdateText Name Of Text Actor, Variable to get value from, Scene where text actor lives (optional)
		{
			if (!action->Params[2].IsEmpty())
				scene = game->findScene(action->getParameter3(cont).c_str());

			CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(action->Params[1], scene);
			if (var != NULL)
			{
				CIwGameActorText* text = (CIwGameActorText*)scene->findActor(action->getParameter1(cont).c_str());
				if (text->getActualClassTypeHash() ==  CIwGameXomlNames::ActorText_Hash)
				{
					text->setText(var->getValue().c_str());
				}
				else
					CIwGameError::LogError("Error: Actions - Update Text - Target actor is not a text based actor - ", action->getParameter1(cont).c_str());
			}
			else
				CIwGameError::LogError("Warning: Actions - Update Text - Source variable not found - ", action->getParameter2(cont).c_str());
		}
		break;
	case Action_Exit:				// Exits the app
		PLATFORM_SYS->RequestQuit();
		break;
	case Action_RemoveResource:		// Removes a specific resource (name, type)
		{
			if (!action->Params[0].IsEmpty())
			{
				// Remove from global resource manager
				IIwGameXomlResource* res = IW_GAME_GLOBAL_RESOURCES->getResourceManager()->findResource(action->getParameter1(cont).c_str(), action->getParameter2(cont).c_str(), false);
				if (res != NULL)
					IW_GAME_GLOBAL_RESOURCES->getResourceManager()->removeResource(res);
			}
		}
		break;
	case Action_RemoveResources:	// Removes a collection of tagged resources (tag)
		{
			if (!action->Params[0].IsEmpty())
			{
				// Remove from global resource manager
				IW_GAME_GLOBAL_RESOURCES->getResourceManager()->removeTaggedResources(action->getParameter1(cont).getHash());
			}
		}
		break;
	case Action_ChangeProgram:		// Changes a running program
		{
			if (!action->Params[2].IsEmpty())
				scene = game->findScene(action->getParameter3(cont).c_str());

			CIwGameProgram* program = NULL;
			if (scene != NULL)
			{
				if (!action->Params[0].IsEmpty())
				{
					program = scene->getProgrameManager()->findProgram(action->getParameter1(cont).c_str());
					if (program == NULL)
						program = IW_GAME_GLOBAL_RESOURCES->getProgrameManager()->findProgram(action->getParameter1(cont).c_str());
				}
				else
				{
					// If no program specified then target priority program
					program = scene->getProgrameManager()->getPriorityProgram();
				}
			}

			if (program != NULL)
			{
				unsigned int command_hash = action->getParameter2(cont).getHash();
				if (command_hash == CIwGameXomlNames::Start_Hash)
					program->start();
				else
				if (command_hash == CIwGameXomlNames::Restart_Hash)
					program->restart();
				else
				if (command_hash == CIwGameXomlNames::Stop_Hash)
					program->stop();
				else
				if (command_hash == CIwGameXomlNames::Pause_Hash)
					program->pause();
				else
				if (command_hash == CIwGameXomlNames::Next_Hash)
					program->gotoNextCommand();
				else
				if (command_hash == CIwGameXomlNames::Priority_Hash)
					program->getManager()->setPriorityProgram(program);
				else
				if (command_hash == CIwGameXomlNames::Goto_Hash)
				{
					IIwGameCommandExecutor* command = program->findCommand(action->getParameter4(cont).getHash());
					if (command != NULL)
						command->gotoCommand();
					else
						CIwGameError::LogError("Warning: Actions - ChangeProgram - goto command that does not exist - ", action->getParameter4(cont).c_str());
				}
			}
			else
				CIwGameError::LogError("Warning: Actions - ChangeProgram - No valid program found - ", action->getParameter1(cont).c_str());
		}
		break;
	case Action_DebugText:
		{
			CIwGameError::LogError(action->getParameter1(cont).c_str(), action->getParameter2(cont).c_str());
		}
		break;
	case Action_LoadFile:	// File, blocking, filename (optional), scene (optional)
		{
			IIwGameXomlResource* parent = NULL;
			if (!action->Params[3].IsEmpty())
				scene = game->findScene(action->getParameter4(cont).getHash());
			if (!action->Params[0].IsEmpty())
			{
				if (scene == NULL)
					CIwGameError::LogError("Error: Action_LoadFile - Scene is invalid - ", action->getParameter1(cont).c_str());
				else
				{
					CIwGameDataFile* file = (CIwGameDataFile*)scene->getResourceManager()->findResource(action->getParameter1(cont).getHash(), CIwGameXomlNames::File_Hash);
					if (file != NULL)
					{
						if (!action->Params[2].IsEmpty())
						{
							// New filename was supplied so load new file contents into file
							file->Init(action->getParameter3(cont).c_str(), true, action->getParameter2(cont).GetAsBool());
						}
						else
							file->Load(action->getParameter2(cont).GetAsBool());
					}
					else
						CIwGameError::LogError("Warning: File resource could not be found - ", action->getParameter1(cont).c_str());
				}
			}
		}
		break;
	case Action_CallGlobalScript:
		{
			CIwGameString* p1 = &action->getParameter1(cont);
			CIwGameString* p3 = NULL;
			CIwGameString* p4 = NULL;
			CIwGameString* p5 = NULL;
			if (!action->Params[2].IsEmpty())
				p3 = &action->getParameter3(cont);
			if (!action->Params[3].IsEmpty())
				p4 = &action->getParameter4(cont);
			if (!action->Params[4].IsEmpty())
				p5 = &action->getParameter5(cont);

			CIwGameScene* scene = NULL;
			if (!action->Params[1].IsEmpty())
				scene = game->findScene(action->getParameter2().getHash());
			if (scene != NULL)
				IW_GAME_GLOBAL_RESOURCES->getScriptEngine()->CallFunction(scene, p1, p3, p4, p5);
			else
				IW_GAME_GLOBAL_RESOURCES->getScriptEngine()->CallFunction(game, p1, p3, p4, p5);
		}
		break;
	case Action_ChangeVideoCam:
		{
			CIwGameString* p1 = &action->getParameter1(cont);
			CIwGameString* p2 = NULL;
			if (!action->Params[1].IsEmpty())
				p2 = &action->getParameter2(cont);
			if (p2 == NULL)
			{
				CIwGameError::LogError("Warning: VideoCam function not supplied - ", action->getParameter1(cont).c_str());
				return;
			}
			CIwGameVideoCam* cam = (CIwGameVideoCam*)CIwGameXomlResourceManager::FindResource(p1->getHash(), CIwGameXomlNames::VideoCam_Hash, scene);
			if (cam != NULL)
			{
				unsigned int hash = p2->getHash();
				if (hash == CIwGameXomlNames::Start_Hash)
					cam->Start();
				else
				if (hash == CIwGameXomlNames::Stop_Hash)
					cam->Stop();
				else
					CIwGameError::LogError("Warning: VideoCam function not supported - ", action->getParameter1(cont).c_str());
			}
			else
				CIwGameError::LogError("Warning: VideoCam resource was not found - ", action->getParameter1(cont).c_str());
		}
		break;
	}
}
