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
#include "IwGameScene.h"
#include "IwGameActor.h"
#include "IwGameCamera.h"
#include "IwGameString.h"
#include "IwGameActions.h"
#include "IwGameRender2d.h"
#include "IwGameScript.h"
#include "IwGameScriptLua.h"
#include "IwGameUI.h"
#include "IwGameRemoteReq.h"

//
//
//
//
// CIwGameScene implementation
//
//
//
//
CIwGameScene::~CIwGameScene()
{
	NotifyDestroy();

	// Only delete an unmanaged timeline
	if (Timeline != NULL)
	{
		if (!Timeline->isManaged())
		{
			SAFE_DELETE(Timeline)
		}
	}

	if (Camera != NULL)
	{
		if (!Camera->isManaged())
		{
			SAFE_DELETE(Camera)
		}
	}
	SAFE_DELETE(EventsManager)
	SAFE_DELETE(Modifiers)
	SAFE_DELETE(Collidables)
	clearActors(true);

#if defined(IW_GAME_BOX2D)
	SAFE_DELETE(Box2dWorld)
#endif	// IW_GAME_BOX2D

	SAFE_DELETE(SpriteManager)
	SAFE_DELETE(VariableManager)
	SAFE_DELETE(ActionsManager)
	SAFE_DELETE(TimelinesManager)
	SAFE_DELETE(ProgramManager)
	SAFE_DELETE(ScriptEngine)
	SAFE_DELETE(ResourceManager)

}

void CIwGameScene::setVirtualTransform(int required_width, int required_height, float angle, eIwGameScene_CanvasFit fit, eIwGameScene_CanvasOrigin canvas_origin)
{
	float scale_x = 1.0f, scale_y = 1.0f;
	int screen_width = IwGxGetScreenWidth();
	int screen_height = IwGxGetScreenHeight();

	if (screen_width > screen_height)
		Orientation = Orientation_Landscape;
	else
		Orientation = Orientation_Portrait;

	OriginalSize.x = required_width;
	OriginalSize.y = required_height;
	CanvasFit = fit;
	CanvasOrigin = canvas_origin;

	// If negative size has been specified then base size on a percentage of screen
	if (OriginalSize.x < 0)
		required_width = (screen_width * -OriginalSize.x) / 100;
	if (OriginalSize.y < 0)
		required_height = (screen_height * -OriginalSize.y) / 100;
	
	ScreenSize.x = screen_width;
	ScreenSize.y = screen_height;
	VirtualSize.x = required_width;
	VirtualSize.y = required_height;
	if (fit == CF_None)
	{
		scale_x = scale_y = 1.0f;
	}
	else
	if (fit == CF_Both)
	{
		// Simply scale to fit native screen resolution
		scale_x = (float)screen_width / required_width;
		scale_y = (float)screen_height / required_height;
	}
	else
	{
		// A fixed aspect ensures that the aspect ratio of the scene fits the native screen aspect ratio
		if (fit == CF_Width)
			scale_x = scale_y = (float)screen_width / required_width;
		else
		if (fit == CF_Height)
			scale_x = scale_y = (float)screen_height / required_height;
		else
		if (fit == CF_Best)
		{
			float sx = (float)screen_width / required_width;
			float sy = (float)screen_height / required_height;

			if (sx < sy )
				scale_x = scale_y = sx;
			else
				scale_x = scale_y = sy;
		}
	}

	// Build rotation / scale tranform
	CIwFMat2D scale;
	scale.SetIdentity();
	scale.m[0][0] = scale_x;
	scale.m[1][1] = scale_y;
	VirtualTransform.SetRot(angle * 0.017453293f);
	VirtualTransform.PostMult(scale);

	switch (canvas_origin)
	{
	case CO_Centre:
		{
			// Position scene in middle of screen so that origin is screens centre
			VirtualTransform.t.x = (float)(screen_width / 2);
			VirtualTransform.t.y = (float)(screen_height / 2);
		}
		break;
	case CO_TopLeft:
		{
			VirtualTransform.t.x = 0;
			VirtualTransform.t.y = 0;
		}
		break;
	case CO_Top:
		{
			VirtualTransform.t.x = (float)(screen_width / 2);
			VirtualTransform.t.y = 0;
		}
		break;
	case CO_Left:
		{
			VirtualTransform.t.x = 0;
			VirtualTransform.t.y = (float)(screen_height / 2);
		}
		break;
	}

	// If camera attached then force transform to become dirty so that the scene transform gets rebuilt
	if (Camera != NULL)
	{
		Camera->setTransformDirty(true);
		Camera->Update(0);
		Transform = Camera->getTransform();
		Transform.PostMult(VirtualTransform);

		// Transfer scene transform to sprite manager
		if (SpriteManager != NULL)
		{
			SpriteManager->setTransformNoCamera(VirtualTransform);
			SpriteManager->setTransform(Transform);
		}
	}
	else
	{
		// If no camera attached then just set the scene transform to the virtual transform
		Transform = VirtualTransform;

		// Transfer scene transform to sprite manager
		if (SpriteManager != NULL)
		{
			SpriteManager->setTransformNoCamera(VirtualTransform);
			SpriteManager->setTransform(Transform);
		}
	}

	// Set sprite manager screen centre and centre of projection
	if (SpriteManager != NULL)
	{
		SpriteManager->setScreenCentre((float)(screen_width / 2), (float)(screen_height / 2));
		SpriteManager->setCOP((float)(screen_width / 2), (float)(screen_height / 2));
	}
}

void CIwGameScene::setVirtualTransform(int required_width, int required_height, float angle, bool fix_aspect, bool lock_width, eIwGameScene_CanvasOrigin canvas_origin)
{
	if (!fix_aspect)
		setVirtualTransform(required_width, required_height, angle, CF_Both, canvas_origin);
	else
	{
		if (lock_width)
			setVirtualTransform(required_width, required_height, angle, CF_Width, canvas_origin);
		else
			setVirtualTransform(required_width, required_height, angle, CF_Height, canvas_origin);
	}
}

int	 CIwGameScene::Init(int max_collidables, int max_layers, bool doSleep)
{
	// Allocate collidables list
	Timeline = NULL;
	Collidables = new CIwGameActor*[max_collidables];
	MaxCollidables = max_collidables;
	MaxLayers = max_layers;

	// Allocate managers
	SpriteManager = new CIwGameSpriteManager();
	SpriteManager->Init(max_layers);
	ResourceManager = new CIwGameXomlResourceManager();
	ResourceManager->setParent(this);
	VariableManager = new CIwGameXomlVariableManager();
	VariableManager->setParent(this);
	ActionsManager = new CIwGameActionsManager();
	ActionsManager->setParent(this);
	TimelinesManager = new CIwGameAnimTimelinesManager();
	TimelinesManager->setParent(this);
	ProgramManager = new CIwGameProgramManager();
	ProgramManager->setParent(this);

	Colour.r = 0xff;
	Colour.g = 0xff;
	Colour.b = 0xff;
	Colour.a = 0xff;
	PrevTouchVel.x = 0;
	PrevTouchVel.y = 0;

#if defined(IW_GAME_BOX2D)
	if (PhysicsEnabled)
	{
		Box2dWorld = new CIwGameBox2dWorld();
		Box2dWorld->InitWorld(doSleep);
	}
#endif	// IW_GAME_BOX2D

	return 1;
}

void CIwGameScene::setScriptEngine(CIwGameString& type)
{
	if (ScriptEngine != NULL)
		delete ScriptEngine;
	
	unsigned int engine_hash = type.getHash();
	if (engine_hash == IW_GAME_HASH("lua"))
	{
		CIwGameScriptEngineLua* engine = new CIwGameScriptEngineLua();
		engine->Init(this);
		ScriptEngine = engine;
	}
}


void CIwGameScene::addActor(CIwGameActor *actor)
{
	// Add an actor to the scene
	Actors.push_back(actor);
	actor->setScene(this);
}

void CIwGameScene::removeActor(CIwGameActor* actor, bool instant_delete)
{
	removeLinkedActors(actor);

	if (instant_delete)
	{
		for (_Iterator it = Actors.begin(); it != Actors.end(); ++it)
		{
			if ((*it) == actor)
			{
				Actors.erase(it);
				delete *it;
				return;
			}
		}
	}

	Removals.push_back(actor);
	actor->setDestroyed(true);
}

void CIwGameScene::removeActor(unsigned int name_hash)
{
	// Renove named actor from the scene
	for (_Iterator it = Actors.begin(); it != Actors.end(); ++it)
	{
		if (name_hash == (*it)->getNameHash())
		{
			removeLinkedActors(*it);
			Removals.push_back(*it);
			(*it)->setDestroyed(true);
			break;
		}
	}
}

void CIwGameScene::fixActors(CIwGameActor* parent)
{
/*	// When an actor is deleted it also deletes its visual. In the sprite manager any linked actors are added as children of the parent actors visual
	// When the parent actors visual is deleted, it also deletes all child sprites, so we have to find all actors in the scene that are linked to 
	// the parent and remove their visuals
	for (_Iterator it = Actors.begin(); it != Actors.end(); ++it)
	{
		if ((*it)->getLinkedTo() == parent)
		{
			(*it)->setVisual(NULL);
		}
	}
	*/
}

void CIwGameScene::removeLinkedActors(CIwGameActor* actor)
{
	for (_Iterator it = Actors.begin(); it != Actors.end(); ++it)
	{
		if ((*it)->getLinkedTo() == actor)
		{
			removeLinkedActors(*it);	// Remove any further linked actors
			Removals.push_back(*it);	// Add to removals list
			(*it)->setDestroyed(true);
		}
	}
}

CIwGameActor* CIwGameScene::findActor(const char* name)
{
	return findActor(CIwGameString::CalculateHash(name));
}

CIwGameActor* CIwGameScene::findActor(unsigned int name_hash)
{
	// Find and eturn named actor
	for (_Iterator it = Actors.begin(); it != Actors.end(); ++it)
	{
		if (name_hash == (*it)->getNameHash() && !(*it)->isDestroyed())
			return *it;
	}

	return NULL;
}

CIwGameActor* CIwGameScene::findActor(int type)
{
	// Find and return first actor of specified type
	for (_Iterator it = Actors.begin(); it != Actors.end(); ++it)
	{
		if (type == (*it)->getType() && !(*it)->isDestroyed())
			return *it;
	}
	return NULL;
}

CIwGameActor* CIwGameScene::findClosestActor(int x, int y, int type)
{
	// Find player frog that is closest to point x, y
	CIwGameActor* closest = NULL;
	float closest_d = 99999999.9f;

	for (_Iterator it = begin(); it != end(); ++it)
	{
		if ((*it)->getType() == type && !(*it)->isDestroyed())
		{
			CIwFVec2 pos = (*it)->getPosition();
			float dx = x - pos.x;
			float dy = y - pos.y;
			float d = dx * dx + dy * dy;

			if (d < closest_d)
			{
				closest = *it;
				closest_d = d;
			}
		}
	}

	return closest;
}

CIwGameActor* CIwGameScene::findFurthestActor(int x, int y, int type)
{
	// Find player frog that is closest to point x, y
	CIwGameActor* furthest = NULL;
	float furthest_d = -99999999.9f;

	for (_Iterator it = begin(); it != end(); ++it)
	{
		if ((*it)->getType() == type && !(*it)->isDestroyed())
		{
			CIwFVec2 pos = (*it)->getPosition();
			float dx = x - pos.x;
			float dy = y - pos.y;
			float d = dx * dx + dy * dy;

			if (d > furthest_d)
			{
				furthest = *it;
				furthest_d = d;
			}
		}
	}

	return furthest;
}

void CIwGameScene::clearActors(bool ignore_visuals)
{
	// Remove and delete all actors from the scsne (only delete if actors are not managed)
	for (_Iterator it = Actors.begin(); it != Actors.end(); ++it)
	{
		if (!(*it)->isManaged())
		{
			if (ignore_visuals)		// When a scene closes dowen we dont want actors to delete their own visuals as the sprite manager will auto clean them up when it shuts down
				(*it)->setVisual(NULL);
			delete *it;
		}
	}
	Actors.clear();
}

bool CIwGameScene::addCollideable(CIwGameActor* actor)
{
	if (NextFreeCollidable < MaxCollidables)
		Collidables[NextFreeCollidable++] = actor;
	else
		return false;

	return true;
}

void CIwGameScene::setCamera(CIwGameCamera* camera)
{
	if (Camera == camera)
		return;

	if (Camera != NULL)
	{
		if (!Camera->isManaged())
		{
			SAFE_DELETE(Camera)
		}
	}
	Camera = camera;
	Camera->setTransformDirty(true);
}

void CIwGameScene::UpdateActorResizing()
{
	_Iterator it = begin();
	if (it == end())
		return;

	// Step to last actor
	int size = Actors.size();
	for (int t = 0; t < size - 1; t++, it++);

	// Chck all actors to see if any of them have resized
	for (int t = 0; t < size; t++)
	{
		if ((*it)->getCategory() == CIwGameActor::AC_UI)
		{
			CIwGameUIBase* ui = (CIwGameUIBase*)*it;
			if (ui->getNeedsResize())
			{
				ui->ResizeToContent();
				ui->setNeedsResize(false);
			}
		}
		it--;
	}
}

void CIwGameScene::Update(float dt)
{
	if (!IsActive)
		return;

	int screen_width = IwGxGetScreenWidth();
	int screen_height = IwGxGetScreenHeight();
	if (screen_width != ScreenSize.x || screen_height != ScreenSize.y)
		NotifyOrientationChanged();

	if (TickEnabled)
		ProcessEventActions(CIwGameXomlNames::OnTick_Hash);

	// Update any data bindings
	if (Bindings != NULL)
		UpdateBindings();

	// Update modifiers
	if (Modifiers != NULL)
		Modifiers->Execute(this, dt);

	// Run any scene programs
	ProgramManager->Execute(dt);

#if defined(IW_GAME_BOX2D)
	if (PhysicsEnabled && Box2dWorld != NULL)
	{
		if (ConstantTimeStep == 0)
			Box2dWorld->UpdateWorld(dt);
		else
			Box2dWorld->UpdateWorld(ConstantTimeStep);
	}
#endif	// IW_GAME_BOX2D

	IsDragging = false;

	if (Camera != NULL)
	{
		bool touching = IW_GAME_INPUT->getTouch(0)->active;
		CIwFVec2 cam_pos = Camera->getPosition();
		CIwFVec2 cam_vel = Camera->getVelocity();
		bool changed_x = false;
		bool changed_y = false;

		// Handle camera panning with touch
		if (Parent->isTouchPanEnabled())
		{
			if (Parent->getTouchFocus(0) == NULL || (Parent->getTouchFocus(0) != NULL && Camera->getIgnoreActors()))
			{
				bool pan_x = Camera->isTouchPanX();
				bool pan_y = Camera->isTouchPanY();
				if (pan_x || pan_y)
				{
					// Calculate drag in virtual canvas coordinates
					CIwVec2 raw_drag = IW_GAME_INPUT->getDragDelta(0);
					CIwFVec2 drag = CIwFVec2((float)raw_drag.x, (float)raw_drag.y);
					drag.x = (drag.x / Transform.m[0][0]);
					drag.y = (drag.y / Transform.m[1][1]);
					if (touching)
					{
						//if (drag.x >= -1 && drag.x <= 1)
//							drag.x = 0;
						if (pan_x)
						{
							if (drag.x != 0)
							{
								cam_pos.x -= drag.x;
								IsDragging = true;
							}
							changed_x = true;
							PrevTouchVel.x = -drag.x / 2.0f;
						}
//						if (drag.y >= -1 && drag.y <= 1)
//							drag.y = 0;
						if (pan_y)
						{
							if (drag.y != 0)
							{
								cam_pos.y -= drag.y;
								IsDragging = true;
							}
							changed_y = true;
							PrevTouchVel.y = -drag.y / 2.0f;
						}
						if (changed_x || changed_y)
							Parent->releaseTouchFocus(0, 999999, 999999);	// Use stupidly large values so they are invalid
					}
					else
					if (PrevTouching)
					{
						if (pan_x)
						{
							if (PrevTouchVel.x >= -1 && PrevTouchVel.x <= 1)
								PrevTouchVel.x = 0;
							cam_vel.x = PrevTouchVel.x;
							changed_x = true;
						}
						if (pan_y)
						{
							if (PrevTouchVel.y >= -1 && PrevTouchVel.y <= 1)
								PrevTouchVel.y = 0;
							cam_vel.y = PrevTouchVel.y;
							changed_y = true;
						}
						PrevTouchVel.x = 0;
						PrevTouchVel.y = 0;
					}
				}
			}
		}

		if (Extents.w != 0)
		{
//						CIwFVec2 offs = ScreenToVirtual(ScreenSize.x / 2.0f, ScreenSize.y / 2.0f);
			float min_x = (float)Extents.x;
			float min_y = (float)Extents.y;
			float max_x = (float)(Extents.x + Extents.w);
			float max_y = (float)(Extents.y + Extents.h);

			// Prevent cam from going outside scenes extents
			if (cam_pos.x < min_x)
			{
				cam_pos.x = min_x;
				cam_vel.x = 0;
				changed_x = true;
			}
			else
			if (cam_pos.x > max_x)
			{
				cam_pos.x = max_x;
				cam_vel.x = 0;
				changed_x = true;
			}
			if (cam_pos.y < min_y)
			{
				cam_pos.y = min_y;
				cam_vel.y = 0;
				changed_x = true;
			}
			else
			if (cam_pos.y > max_y)
			{
				cam_pos.y = max_y;
				cam_vel.y = 0;
				changed_x = true;
			}
		}

		// Update cam position and velocity
		if (changed_x)
		{
			Camera->setPosition(cam_pos.x, Camera->getPosition().y);
			Camera->setVelocity(cam_vel.x, Camera->getVelocity().y);
		}
		if (changed_y)
		{
			Camera->setPosition(Camera->getPosition().x, cam_pos.y);
			Camera->setVelocity(Camera->getVelocity().x, cam_vel.y);
		}

		PrevTouching = touching;

		// I need to point out here that we use the concept of dirty transforms to cut down processing when  the camera is not moving
		// If the cameras transform is not dirty (the camera has not moved) then there is no need to force the sprite manager to make 
		// all of its child sprites to rebuild their transforms
		Camera->Update(dt);
		if (Camera->isTransformDirty())
		{
			// Combine the virtual canvas and camera transforms
			Transform = Camera->getTransform();
			Transform.PostMult(VirtualTransform);

			// Transfer scene transform to sprite manager
			if (SpriteManager != NULL)
			{
				SpriteManager->setTransformNoCamera(VirtualTransform);
				SpriteManager->setTransform(Transform);
			}
			Camera->setTransformDirty(false);
		}
	}

	if (ClippingArea.w != 0)
		SpriteManager->setClipRect((float)ClippingArea.x, (float)ClippingArea.y, (float)ClippingArea.w, (float)ClippingArea.h);
	else
		SpriteManager->setClipRect((float)(-VirtualSize.x / 2), (float)(-VirtualSize.y / 2), (float)VirtualSize.x, (float)VirtualSize.y);

	// Process the actors
	for (_Iterator it = begin(); it != end(); ++it)
	{
//CIwGameError::LogError("Processing actor: : ", (*it)->getName().c_str());

		if (!(*it)->Update(dt))
		{
			removeActor(*it);
		}
	}

	UpdateActorResizing();

	// Resolve actor collisions
	for (_Iterator it = begin(); it != end(); ++it)
	{
		if ((*it)->isCollidable())
			(*it)->ResolveCollisions();
	}

	// Remove any deleted actors
	CleanupRemovedActors();

	// Update the animation timeline
	if (Timeline != NULL)
 		Timeline->Update(dt, this);

	// Reset collidables list
	NextFreeCollidable = 0;

	// Reset collision lists
#if defined(IW_GAME_BOX2D)
	for (_Iterator it = begin(); it != end(); ++it)
	{
		CIwGameBox2dBody* body = (*it)->getBox2dBody();
		if (body != NULL)
			body->clearAllCollisions();
	}
#endif	// IW_GAME_BOX2D

	// Update run count
	RunCount++;
}

void CIwGameScene::CleanupRemovedActors()
{
	for (_Iterator it = Removals.begin(); it != Removals.end(); ++it)
	{
		for (_Iterator it2 = Actors.begin(); it2 != Actors.end(); ++it2)
		{
			if (*it2 == *it)
			{
				CIwGameActor* focus;
				// if actor is games touch focus then release it
				for (int t = 0; t < IW_GAME_MAX_TOUCHES; t++)
				{
					focus = Parent->getTouchFocus(t);
					if (focus != NULL && focus == *it2)
						Parent->setTouchFocus(t, NULL);
				}
				focus = Parent->getKeyFocus();
				if (focus != NULL && focus == *it2)
					Parent->setKeyFocus(NULL);

				// Do not delete managed actors as the pooled object manager will be handling this
				if (!(*it)->isManaged())
					delete *it2;
				else
				{
					// Instead we disable them and mark them as unused so they can be reallocated
					(*it2)->setUsed(false);
					(*it2)->setActive(false);
				}
				Actors.erase(it2);
				break;
			}
		}
	}
	Removals.clear();
}

void CIwGameScene::Draw()
{
	if (!IsVisible)
		return;

	// Transfer actor visual data to sprite manager sprites
	for (_Iterator it = begin(); it != end(); ++it)
		(*it)->UpdateVisual();

	if (ClippingArea.w != 0)
	{
		// Set clipping rectangle (clipping coords are specified in virtual canvas coordinates so we need to transform them to the screen)
		TransformedClipRect[0].x = (float)ClippingArea.x;
		TransformedClipRect[1].x = (float)(ClippingArea.x + ClippingArea.w);
		TransformedClipRect[0].y = (float)ClippingArea.y;
		TransformedClipRect[1].y = (float)(ClippingArea.y + ClippingArea.h);
		if (ClipStatic)
		{
			for (int t = 0; t < 2; t++)
				TransformedClipRect[t] = VirtualTransform.TransformVec(TransformedClipRect[t]);
		}
		else
		{
			for (int t = 0; t < 2; t++)
				TransformedClipRect[t] = Transform.TransformVec(TransformedClipRect[t]);
		}
	}
	else
	{
		TransformedClipRect[0].x = 0;
		TransformedClipRect[0].y = 0;
		TransformedClipRect[1].x = (float)ScreenSize.x;
		TransformedClipRect[1].y = (float)ScreenSize.y;
	}
	SpriteManager->setScreenClipRect(TransformedClipRect[0].x, TransformedClipRect[0].y, (TransformedClipRect[1].x) - (TransformedClipRect[0].x), (TransformedClipRect[1].y) - (TransformedClipRect[0].y));

	// Draw the sprite manager sprites
	SpriteManager->Draw();

	if (ClippingArea.w != 0)
		CIwGameRender2d::ResetClipRect();

}

void CIwGameScene::RestoreClipping()
{
	if (ClippingArea.w != 0)
	{
		CIwGameRender2d::SetCurrentClipRect((int)TransformedClipRect[0].x, (int)TransformedClipRect[0].y, (int)(TransformedClipRect[1].x - TransformedClipRect[0].x), (int)(TransformedClipRect[1].y - TransformedClipRect[0].y));
	}
}

CIwFVec2 CIwGameScene::ScreenToVirtual(float screen_x, float screen_y, bool ignore_translation)
{
	if (!ignore_translation)
	{
		screen_x -= VirtualTransform.t.x;
		screen_y -= VirtualTransform.t.y;
	}

	return CIwFVec2(screen_x / VirtualTransform.m[0][0], screen_y / VirtualTransform.m[1][1]);
}

CIwFVec2 CIwGameScene::VirtualToScreen(float virtual_x, float virtual_y, bool ignore_translation)
{
	if (!ignore_translation)
		return CIwFVec2(virtual_x * VirtualTransform.m[0][0] + VirtualTransform.t.x, virtual_y * VirtualTransform.m[1][1] + VirtualTransform.t.y);
	else
		return CIwFVec2(virtual_x * VirtualTransform.m[0][0], virtual_y * VirtualTransform.m[1][1]);
}

CIwFVec2 CIwGameScene::ScreenToCamera(float pos_x, float pos_y, bool ignore_translation)
{
	CIwFVec2 cam_pos = CIwFVec2(0, 0);
	if (Camera != NULL)
		cam_pos = Camera->getPosition();
	CIwFMat2D& cam_transform = CIwFMat2D::g_Identity;

	if (Camera != NULL)
		cam_transform = Camera->getTransform();

	cam_pos.x = cam_pos.x / cam_transform.m[0][0];
	cam_pos.y = cam_pos.y / cam_transform.m[1][1];

	if (!ignore_translation)
	{
		pos_x -= VirtualTransform.t.x;
		pos_y -= VirtualTransform.t.y;
	}

	CIwFVec2 virtual_pos = CIwFVec2(pos_x / Transform.m[0][0], pos_y / Transform.m[1][1]);

	return CIwFVec2(cam_pos.x + virtual_pos.x, cam_pos.y + virtual_pos.y);
}

void CIwGameScene::ProcessEventActions(unsigned int event_name)
{
	// Find the event
	CIwGameEvent* evt = EventsManager->findEvent(event_name);
	if (evt != NULL)
	{
		// Find actions list
		CIwGameActions* actions = CIwGameActionsManager::FindActions(evt->ActionsName, this);
		if (actions != NULL)
			actions->Execute(this);
	}
}

void CIwGameScene::NotifyCreate()
{
	ProcessEventActions(CIwGameXomlNames::OnCreate_Hash);
}
void CIwGameScene::NotifyDestroy()
{
	ProcessEventActions(CIwGameXomlNames::OnDestroy_Hash);
}
void CIwGameScene::NotifySuspending(CIwGameScene* new_scene)
{
	ProcessEventActions(CIwGameXomlNames::OnSuspend_Hash);
}
void CIwGameScene::NotifyResuming(CIwGameScene* old_scene)
{
	ProcessEventActions(CIwGameXomlNames::OnResume_Hash);
}
void CIwGameScene::NotifyLostFocus(CIwGameScene* new_scene)
{
	ProcessEventActions(CIwGameXomlNames::OnLostFocus_Hash);
}
void CIwGameScene::NotifyGainedFocus(CIwGameScene* old_scene)
{
	ProcessEventActions(CIwGameXomlNames::OnGainedFocus_Hash);
}

void CIwGameScene::Suspend(CIwGameScene* new_scene)
{
	NotifySuspending(this);
	setActive(false);
}

void CIwGameScene::Resume(CIwGameScene* old_scene)
{
	NotifyResuming(this);
	setActive(true);
}

void CIwGameScene::NotifyKeyBack()
{
	ProcessEventActions(CIwGameXomlNames::OnKeyBack_Hash);
}
void CIwGameScene::NotifyKeyMenu()
{
	ProcessEventActions(CIwGameXomlNames::OnKeyMenu_Hash);
}

void CIwGameScene::NotifyOrientationChanged()
{
	eIwGameScene_Orientation old = Orientation;

	ProcessEventActions(CIwGameXomlNames::OnOrientationChange_Hash);

	// Resize scene
	int min = (OriginalSize.x < OriginalSize.y) ? OriginalSize.x : OriginalSize.y;
	int max = (OriginalSize.x > OriginalSize.y) ? OriginalSize.x : OriginalSize.y;
	if (old == Orientation_Landscape)
		setVirtualTransform(min, max, 0, CanvasFit, CanvasOrigin);
	else
	if (old == Orientation_Portrait)
		setVirtualTransform(max, min, 0, CanvasFit, CanvasOrigin);

	// Inform actors of change
	for (_Iterator it = begin(); it != end(); ++it)
	{
		(*it)->OnOrientationChange(old, Orientation);
	}
}

CIwGameActor* CIwGameScene::FindTappedActor(int x, int y)
{
	CIwGameActor* hit = NULL;
	for (_Iterator it = begin(); it != end(); ++it)
	{
		if ((*it)->isTappable())
		{
			if (hit == NULL || (hit->getLayer() <= (*it)->getLayer()))
			{
				CIwGameActor* new_hit = (*it)->HitTest((float)x, (float)y);
				if (new_hit != NULL)
					hit = new_hit;
			}
		}
	}

	return hit;
}

bool CIwGameScene::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Process scene attributes
	int				max_colliders = 128;
	int				max_layers = 10;
	CIwFVec2		canvas_size((float)IwGxGetScreenWidth(), (float)IwGxGetScreenHeight());
	bool			fix_aspect = true;
	bool			lock_width = false;
	eIwGameScene_CanvasFit canvas_fit = CF_Best;
	eIwGameScene_CanvasOrigin canvas_origin = CO_Centre;
	bool			current = false;
	CIwGameString	timeline_name;
	CIwGameString	bindings_name;
	CIwGameString	camera_name;
	CIwGameString*	scriptengine = NULL;
	CIwColour		colour;
	bool			batch = false;
	bool			allow_focus = false;
	bool			do_sleep = true;
	bool			canvas_fit_set = false;
	CIwFVec2		gravity = CIwFVec2(0, 0);
	bool			gravity_set = false;
	bool			physics = true;
	CIwFVec2		world_scale = CIwFVec2(10, 10);
	bool			world_scale_set = false;

	colour.Set(0xffffffff);

	// Check and apply style
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();
		if (name_hash == CIwGameXomlNames::Style_Hash)
		{
			if (!(*it)->GetValue().IsEmpty())
			{
				// Find the style
				CIwGameStyle* style = (CIwGameStyle*)IW_GAME_GLOBAL_RESOURCES->getResourceManager()->findResource((*it)->GetValue().c_str(), CIwGameXomlNames::Style_Hash);
				if (style == NULL)
				{
					CIwGameError::LogError("Error: XOML - Could not find named style in the global resources - ", (*it)->GetValue().c_str());
					return false;
				}
				// Apply the style
				CIwGameXoml::ApplyStyle(style, node);
				break;
			}
		}
	}

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
			setName((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Type_Hash)
			setType((*it)->GetValueAsInt());
		else
		if (name_hash == CIwGameXomlNames::Layer_Hash)
			setLayer((*it)->GetValueAsInt());
		else
		if (name_hash == CIwGameXomlNames::CanvasSize_Hash)
		{
			if (!(*it)->GetValueAsPoint(canvas_size))
				CIwGameError::LogError("Warning: XOML - Invalid value for Scene::CanvasSize");
		}
		else
		if (name_hash == CIwGameXomlNames::CanvasFit_Hash)
		{
			canvas_fit_set = true;
			unsigned int hash = (*it)->GetValue().getHash();
			if (hash == CIwGameString::CalculateHash("width"))
				canvas_fit = CF_Width;
			else
			if (hash == CIwGameString::CalculateHash("height"))
				canvas_fit = CF_Height;
			else
			if (hash == CIwGameString::CalculateHash("both"))
				canvas_fit = CF_Both;
			else
			if (hash == CIwGameString::CalculateHash("best"))
				canvas_fit = CF_Best;
			else
				canvas_fit = CF_None;
		}
		else
		if (name_hash == CIwGameXomlNames::CanvasOrigin_Hash)
		{
			unsigned int hash = (*it)->GetValue().getHash();
			if (hash == CIwGameString::CalculateHash("centre"))
				canvas_origin = CO_Centre;
			else
			if (hash == CIwGameString::CalculateHash("topleft"))
				canvas_origin = CO_TopLeft;
			else
			if (hash == CIwGameString::CalculateHash("top"))
				canvas_origin = CO_Top;
			else
			if (hash == CIwGameString::CalculateHash("left"))
				canvas_origin = CO_Left;
		}
		else
		if (name_hash == CIwGameXomlNames::FixAspect_Hash)
			fix_aspect = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::LockWidth_Hash)
			lock_width = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::Extents_Hash)
		{
			CIwRect extents;
			if (!(*it)->GetValueAsRect(extents))
				CIwGameError::LogError("Warning: XOML - Invalid value for Scene::Extents");
			else
				setExtents(extents.x, extents.y, extents.w, extents.h);
		}
		else
		if (name_hash == CIwGameXomlNames::AllowSuspend_Hash)
			setAllowSuspend((*it)->GetValueAsBool());
		else
		if (name_hash == CIwGameXomlNames::Clipping_Hash)
		{
			CIwRect clipping;
			if (!(*it)->GetValueAsRect(clipping))
				CIwGameError::LogError("Warning: XOML - Invalid value for Scene::Clipping");
			else
				setClippingArea(clipping.x, clipping.y, clipping.w, clipping.h);
		}
		else
		if (name_hash == CIwGameXomlNames::ClipStatic_Hash)
		{
			ClipStatic = (*it)->GetValueAsBool();
		}
		else
		if (name_hash == CIwGameXomlNames::Active_Hash)
			setActive((*it)->GetValueAsBool());
		else
		if (name_hash == CIwGameXomlNames::Visible_Hash)
			setVisible((*it)->GetValueAsBool());
		else
		if (name_hash == CIwGameXomlNames::Layers_Hash)
			max_layers = ((*it)->GetValueAsInt());
		else
		if (name_hash == CIwGameXomlNames::Colliders_Hash)
			max_colliders = ((*it)->GetValueAsInt());
		else
		if (name_hash == CIwGameXomlNames::Current_Hash)
			current = ((*it)->GetValueAsBool());
		else
		if (name_hash == CIwGameXomlNames::Timeline_Hash)
			timeline_name = (*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Bindings_Hash)
			bindings_name = (*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Colour_Hash || name_hash == CIwGameXomlNames::Color_Hash)
		{
			if (!(*it)->GetValueAsColour(colour))
				CIwGameError::LogError("Warning: XOML - Invalid value for Scene::Colour");
		}
		else
		if (name_hash == CIwGameXomlNames::Opacity_Hash)
			colour.a = (*it)->GetValueAsInt();
		else
		if (name_hash == CIwGameXomlNames::Camera_Hash)
			camera_name = (*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Batch_Hash)
			batch = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::AllowFocus_Hash)
			allow_focus = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::OnSuspend_Hash)
			EventsManager->addEvent("OnSuspend", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnResume_Hash)
			EventsManager->addEvent("OnResume", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnLostFocus_Hash)
			EventsManager->addEvent("OnLostFocus", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnGainedFocus_Hash)
			EventsManager->addEvent("OnGainedFocus", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnCreate_Hash)
			EventsManager->addEvent("OnCreate", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnDestroy_Hash)
			EventsManager->addEvent("OnDestroy", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnKeyBack_Hash)
			EventsManager->addEvent("OnKeyBack", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnKeyMenu_Hash)
			EventsManager->addEvent("OnKeyMenu", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnOrientationChange_Hash)
			EventsManager->addEvent("OnOrientationChange", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnTick_Hash)
		{
			EventsManager->addEvent("OnTick", (*it)->GetValue().c_str());
			TickEnabled = true;
		}
		else
		if (name_hash == CIwGameXomlNames::ScriptEngine_Hash)
			scriptengine = &(*it)->GetValue();
#if defined(IW_GAME_BOX2D)
		else
		if (name_hash == CIwGameXomlNames::Gravity_Hash)
		{
			if (!(*it)->GetValueAsPoint(gravity))
				CIwGameError::LogError("Warning: XOML - Invalid value for Scene::Gravity");
			else
				gravity_set = true;
		}
		else
		if (name_hash == CIwGameXomlNames::DoSleep_Hash)
			do_sleep = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::WorldScale_Hash)
		{
			if (!(*it)->GetValueAsPoint(world_scale))
				CIwGameError::LogError("Warning: XOML - Invalid value for Scene::WorldScale");
			else
				world_scale_set = true;
		}
		else
		if (name_hash == CIwGameXomlNames::Physics_Hash)
			physics = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::PhysicsTimestep_Hash)
			ConstantTimeStep = (*it)->GetValueAsFloat();
#endif // IW_GAME_BOX2D
	}

	// If scene already exists then load children into the original scene and discard this scene
	CIwGameScene* old_scene = ((CIwGame*)parent)->findScene(NameHash);
	if (old_scene != NULL && !old_scene->isDestroyed())
	{
		if (load_children)
		{
			// Process inner tags
			if (!IW_GAME_XOML->Process(old_scene, node))
			{
				return false;
			}
		}
		IW_GAME_XOML->setExitOnError(false);
		return false;
	}

#if defined(IW_GAME_BOX2D)
	PhysicsEnabled = physics;
#else
	PhysicsEnabled = false;
#endif // IW_GAME_BOX2D

	// Initialise the scene
	Parent = (CIwGame*)parent;
	if (load_children)
	{
		Init(max_colliders, max_layers, do_sleep);
		SpriteManager->setBatching(batch);

		// Set the virtual tansform (called her first as the virtual transform needs to be set up so that actors inside the scene that use docking can have access to the virtual transform)
		if (canvas_fit_set)
			setVirtualTransform((int)canvas_size.x, (int)canvas_size.y, 0, canvas_fit, canvas_origin);
		else
			setVirtualTransform((int)canvas_size.x, (int)canvas_size.y, 0, fix_aspect, lock_width, canvas_origin);

#if defined(IW_GAME_BOX2D)
		if (physics)
		{
			if (gravity_set)
				Box2dWorld->setGravity(gravity.x, gravity.y);
			if (world_scale_set)
				Box2dWorld->setWorldScale(world_scale.x, world_scale.y);
		}
#endif // IW_GAME_BOX2D
	}

	// Set the colour
	Colour = colour;

	// Create script engine
	if (scriptengine != NULL)
		setScriptEngine(*scriptengine);

	if (load_children)
	{
		// Process inner tags
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}

	// Check that we were created from a CIwGame derived class
	if (parent->getClassTypeHash() != CIwGameXomlNames::Game_Hash)
	{
		CIwGameError::LogError("Error: XOML - Scene needs to be declared inside a game type class");
		return false;
	}

	// Search the scenes resources for the specified timeline
	if (!timeline_name.IsEmpty())
	{
		CIwGameAnimTimeline* timeline = CIwGameAnimTimelinesManager::FindTimeline(timeline_name.getHash(), this);
		if (timeline == NULL)
			CIwGameError::LogError("Warning: XOML - Could not find named timeline in the scene or in the global resources - ", timeline_name.c_str());
		else
		{
			setTimeline(timeline);
			timeline->setTargetElement(this);
		}
	}

	// Search the scenes resources for the specified bindings manager
	if (!bindings_name.IsEmpty())
	{
		CIwGameXomlBindings* bindings = (CIwGameXomlBindings*)getResourceManager()->findResource(bindings_name.c_str(), CIwGameXomlNames::Bindings_Hash);
		if (bindings == NULL)
			CIwGameError::LogError("Warning: XOML - Could not find named bindings in the scene or in the global resources - ", bindings_name.c_str());
		else
			setBindings(bindings);
	}

	// Add the scene to the game
	((CIwGame*)parent)->addScene(this, current);

	// Add camera
	if (!camera_name.IsEmpty())
	{
		// Find camera resource
		CIwGameCamera* camera = (CIwGameCamera*)getResourceManager()->findResource(camera_name.c_str(), CIwGameXomlNames::Camera_Hash);
		if (camera == NULL)
		{
			CIwGameError::LogError("Warning: XOML - Could not find named camera in the scene or in the global resources - ", camera_name.c_str());
		}
		setCamera(camera);

		if (Camera != NULL)
		{
			// Note that we set the virtual transform in this 2nd place because a camera has been attached which could change the transform
			if (canvas_fit_set)
				setVirtualTransform((int)canvas_size.x, (int)canvas_size.y, 0, canvas_fit, canvas_origin);
			else
				setVirtualTransform((int)canvas_size.x, (int)canvas_size.y, 0, fix_aspect, lock_width, canvas_origin);
		}
	}

	// If scene is marked as current then switcn to it
	if (current)
	{
		((CIwGame*)parent)->changeScene(this);
	}

	// Set allow focus
	AllowFocus = allow_focus;

	return true;
}

bool CIwGameScene::PostLoadFromXoml(IIwGameXomlResource* parent, CIwGameXmlNode* node)
{
	// Force update any data bindings
	if (Bindings != NULL)
		UpdateBindings(true);

	NotifyCreate();

	return true;
}

bool CIwGameScene::getProperty(const char* element_name, CIwGameXomlProperty& prop)
{
	return getProperty(IW_GAME_HASH(element_name), prop);
}

bool CIwGameScene::getProperty(unsigned int element_name, CIwGameXomlProperty& prop)
{
	if (Camera != NULL)
	{
		if (element_name == CIwGameXomlNames::Position_Hash)
		{
			prop.Type = PT_Vec2;
			CIwFVec2 v = Camera->getPosition();
			prop.p_vec[0] = v.x;
			prop.p_vec[1] = v.y;
			return true;
		}
		else
		if (element_name == CIwGameXomlNames::PositionX_Hash)
		{
			prop.Type = PT_Float;
			prop.p_float = Camera->getPosition().x;
			return true;
		}
		else
		if (element_name == CIwGameXomlNames::PositionY_Hash)
		{
			prop.Type = PT_Float;
			prop.p_float = Camera->getPosition().y;
			return true;
		}
		else
		if (element_name == CIwGameXomlNames::Angle_Hash)
		{
			prop.Type = PT_Float;
			prop.p_float = Camera->getAngle();
			return true;
		}
		else
		if (element_name == CIwGameXomlNames::Scale_Hash)
		{
			prop.Type = PT_Float;
			prop.p_float = Camera->getScale();
			return true;
		}
	}

	if (element_name == CIwGameXomlNames::Name_Hash)
	{
		prop.Type = PT_String;
		prop.p_data = (void*)Name.c_str();
	}
	else
	if (element_name == CIwGameXomlNames::Type_Hash)
	{
		prop.Type = PT_Int;
		prop.p_int = Type;
	}
	else
	if (element_name == CIwGameXomlNames::Layer_Hash)
	{
		prop.Type = PT_Int;
		prop.p_int = getLayer();
	}
	else
	if (element_name == CIwGameXomlNames::Extents_Hash)
	{
		prop.Type = PT_Vec4;
		prop.p_vec[0] = (float)Extents.x;
		prop.p_vec[1] = (float)Extents.y;
		prop.p_vec[2] = (float)Extents.w;
		prop.p_vec[3] = (float)Extents.h;
	}
	else
	if (element_name == CIwGameXomlNames::AllowSuspend_Hash)
	{
		prop.Type = PT_Bool;
		prop.p_bool = AllowSuspend;
	}
	else
	if (element_name == CIwGameXomlNames::AllowFocus_Hash)
	{
		prop.Type = PT_Bool;
		prop.p_bool = AllowFocus;
	}
	else
	if (element_name == CIwGameXomlNames::ClipStatic_Hash)
	{
		prop.Type = PT_Bool;
		prop.p_bool = ClipStatic;
	}
	else
	if (element_name == CIwGameXomlNames::Active_Hash)
	{
		prop.Type = PT_Bool;
		prop.p_bool = IsActive;
	}
	else
	if (element_name == CIwGameXomlNames::Visible_Hash)
	{
		prop.Type = PT_Bool;
		prop.p_bool = IsVisible;
	}
	else
	if (element_name == CIwGameXomlNames::Current_Hash)
	{
		prop.Type = PT_Bool;
		if (Parent->getCurrentScene() == this)
			prop.p_bool = true;
		else
			prop.p_bool = false;
	}
	else
	if (element_name == CIwGameXomlNames::Colour_Hash || element_name == CIwGameXomlNames::Color_Hash)
	{
		prop.Type = PT_Vec4;
		prop.p_vec[0] = (float)Colour.r;
		prop.p_vec[1] = (float)Colour.g;
		prop.p_vec[2] = (float)Colour.b;
		prop.p_vec[3] = (float)Colour.a;
	}
	else
	if (element_name == CIwGameXomlNames::Opacity_Hash)
	{
		prop.Type = PT_Int;
		prop.p_int = Colour.a;
	}
	else
	if (element_name == CIwGameXomlNames::Timeline_Hash)
	{
		prop.Type = PT_Resource;
		prop.p_data = (void*)Timeline;
	}
	else
	if (element_name == CIwGameXomlNames::Camera_Hash)
	{
		prop.Type = PT_Resource;
		prop.p_data = (void*)Camera;
	}
#if defined(IW_GAME_BOX2D)
	else
	if (element_name == CIwGameXomlNames::Gravity_Hash)
	{
		if (Box2dWorld != NULL)
		{
			prop.Type = PT_Vec2;
			CIwFVec2 v = Box2dWorld->getGravity();
			prop.p_vec[0] = v.x;
			prop.p_vec[1] = v.y;
		}
		else
			return false;
	}
	else
	if (element_name == CIwGameXomlNames::WorldScale_Hash)
	{
		if (Box2dWorld != NULL)
		{
			prop.Type = PT_Vec2;
			CIwFVec2 v = Box2dWorld->getWorldScale();
			prop.p_vec[0] = v.x;
			prop.p_vec[1] = v.y;
		}
		else
			return false;
	}
	else
	if (element_name == CIwGameXomlNames::PhysicsTimestep_Hash)
	{
		if (Box2dWorld != NULL)
		{
			prop.Type = PT_Float;
			prop.p_float = ConstantTimeStep;
		}
		else
			return false;
	}
#endif // IW_GAME_BOX2D
	else
		return false;

	return true;
}

bool CIwGameScene::setProperty(const char* element_name, CIwGameString& data, bool delta)
{
	return setProperty(CIwGameString::CalculateHash(element_name), data, delta);
}

bool CIwGameScene::setProperty(unsigned int element_name, CIwGameString& data, bool delta)
{
	float	float_pool[8];

	if (Camera != NULL)
	{
		if (element_name == CIwGameXomlNames::Position_Hash)
		{
			data.GetAsListOfFloat(float_pool);
			if (delta)
			{
				CIwFVec2 pos = Camera->getPosition();
				pos.x += float_pool[0];
				pos.y += float_pool[1];
				Camera->setPosition(pos.x, pos.y);
			}
			else
				Camera->setPosition(float_pool[0], float_pool[1]);
			return true;
		}
		else
		if (element_name == CIwGameXomlNames::PositionX_Hash)
		{
			CIwFVec2 pos = Camera->getPosition();
			if (delta)
			{
				pos.x += data.GetAsFloat();
				Camera->setPosition(pos.x, pos.y);
			}
			else
				Camera->setPosition(data.GetAsFloat(), pos.y);
			return true;
		}
		else
		if (element_name == CIwGameXomlNames::PositionY_Hash)
		{
			CIwFVec2 pos = Camera->getPosition();
			if (delta)
			{
				pos.y += data.GetAsFloat();
				Camera->setPosition(pos.x, pos.y);
			}
			else
				Camera->setPosition(pos.x, data.GetAsFloat());
			return true;
		}
		else
		if (element_name == CIwGameXomlNames::Angle_Hash)
		{
			float d = data.GetAsFloat();
			if (delta)
				Camera->setAngle(Camera->getAngle() + d);
			else
				Camera->setAngle(d);
			return true;
		}
		else
		if (element_name == CIwGameXomlNames::Scale_Hash)
		{
			float d = data.GetAsFloat();
			if (delta)
				Camera->setScale(Camera->getScale() + d);
			else
				Camera->setScale(d);
			return true;
		}
	}

	if (element_name == CIwGameXomlNames::Name_Hash)
		setName(data.c_str());
	else
	if (element_name == CIwGameXomlNames::Type_Hash)
		setType(data.GetAsBool());
	else
	if (element_name == CIwGameXomlNames::Layer_Hash)
	{
		int d = data.GetAsInt();
		if (delta)
			setLayer(Layer + d);
		else
			setLayer(d);
	}
	else
	if (element_name == CIwGameXomlNames::Extents_Hash)
	{
		data.GetAsListOfFloat(float_pool);
		if (delta)
		{
			Extents.x += (int16)float_pool[0];
			Extents.y += (int16)float_pool[1];
			Extents.w += (int16)float_pool[2];
			Extents.h += (int16)float_pool[3];
		}
		else
		{
			Extents.x = (int16)float_pool[0];
			Extents.y = (int16)float_pool[1];
			Extents.w = (int16)float_pool[2];
			Extents.h = (int16)float_pool[3];
		}
	}
	else
	if (element_name == CIwGameXomlNames::AllowSuspend_Hash)
	{
		if (delta)
			setAllowSuspend(!getAllowSuspend());
		else
			setAllowSuspend(data.GetAsBool());
	}
	else
	if (element_name == CIwGameXomlNames::AllowFocus_Hash)
	{
		if (delta)
			setAllowFocus(!getAllowFocus());
		else
			setAllowFocus(data.GetAsBool());
	}
	else
	if (element_name == CIwGameXomlNames::ClipStatic_Hash)
	{
		if (delta)
			ClipStatic = !ClipStatic;
		else
			ClipStatic = data.GetAsBool();
	}
	else
	if (element_name == CIwGameXomlNames::Active_Hash)
	{
		if (delta)
			setActive(!isActive());
		else
			setActive(data.GetAsBool());
	}
	else
	if (element_name == CIwGameXomlNames::Visible_Hash)
	{
		if (delta)
			setVisible(!isVisible());
		else
			setVisible(data.GetAsBool());
	}
	else
	if (element_name == CIwGameXomlNames::Current_Hash)
	{
		if (data.GetAsBool())
			((CIwGame*)Parent)->changeScene(this);
	}
	else
	if (element_name == CIwGameXomlNames::Colour_Hash || element_name == CIwGameXomlNames::Color_Hash)
	{
		data.GetAsListOfFloat(float_pool);
		if (delta)
		{
			Colour.r += (uint8)float_pool[0];
			Colour.g += (uint8)float_pool[1];
			Colour.b += (uint8)float_pool[2];
			Colour.a += (uint8)float_pool[3];
		}
		else
		{
			Colour.r = (uint8)float_pool[0];
			Colour.g = (uint8)float_pool[1];
			Colour.b = (uint8)float_pool[2];
			Colour.a = (uint8)float_pool[3];
		}
	}
	else
	if (element_name == CIwGameXomlNames::Opacity_Hash)
	{
		uint8 opacity = (uint8)data.GetAsInt();
		if (delta)
			Colour.a += opacity;
		else
			Colour.a = opacity;
	}
	else
	if (element_name == CIwGameXomlNames::Timeline_Hash)
	{
		if (!data.IsEmpty())
		{
			CIwGameAnimTimeline* timeline = CIwGameAnimTimelinesManager::FindTimeline(data.getHash(), this);
			if (timeline == NULL)
			{
#if defined(_DEBUG)
				CIwGameError::LogError("Warning: Set Property - Could not find named timeline in the scene or in the global resources - ", data.c_str());
#endif
			}
			if (timeline != NULL && timeline != Timeline)
			{
				setTimeline(timeline);
				timeline->setTargetElement(this);
			}
		}
	}
	else
	if (element_name == CIwGameXomlNames::TimeScale_Hash)
	{
		if (Timeline != NULL)
		{
			float d = data.GetAsFloat();
			if (delta)
				Timeline->setTimeScale(Timeline->getTimeScale() + d);
			else
				Timeline->setTimeScale(d);
		}
	}
	else
	if (element_name == CIwGameXomlNames::Camera_Hash)
	{
		if (!data.IsEmpty())
		{
			CIwGameCamera* camera = (CIwGameCamera*)getResourceManager()->findResource(data.c_str(), CIwGameXomlNames::Camera_Hash);
			if (camera == NULL)
			{
#if defined(_DEBUG)
				CIwGameError::LogError("Warning: Set Property - Could not find named camera in the scene or in the global resources - ", data.c_str());
#endif
			}
			setCamera(camera);
		}
	}
#if defined(IW_GAME_BOX2D)
	else
	if (element_name == CIwGameXomlNames::Gravity_Hash)
	{
		if (Box2dWorld != NULL)
		{
			data.GetAsListOfFloat(float_pool);
			if (delta)
			{
				CIwFVec2 g = Box2dWorld->getGravity();
				Box2dWorld->setGravity(g.x + float_pool[0], g.y + float_pool[1]);
			}
			else
				Box2dWorld->setGravity(float_pool[0], float_pool[1]);
		}
	}
	else
	if (element_name == CIwGameXomlNames::WorldScale_Hash)
	{
		if (Box2dWorld != NULL)
		{
			data.GetAsListOfFloat(float_pool);
			if (delta)
			{
				CIwFVec2 s = Box2dWorld->getWorldScale();
				Box2dWorld->setWorldScale(s.x + float_pool[0], s.y + float_pool[1]);
			}
			else
				Box2dWorld->setWorldScale(float_pool[0], float_pool[1]);
		}
	}
	else
	if (element_name == CIwGameXomlNames::PhysicsTimestep_Hash)
	{
		if (Box2dWorld != NULL)
		{
			float step = data.GetAsFloat();
			if (delta)
				ConstantTimeStep += step;
			else
				ConstantTimeStep = step;
		}
	}
#endif // IW_GAME_BOX2D
	else
	if (element_name == CIwGameXomlNames::OnCreate_Hash)
		EventsManager->addEvent("OnCreate", data.c_str(), true);
	else
	if (element_name == CIwGameXomlNames::OnDestroy_Hash)
		EventsManager->addEvent("OnDestroy", data.c_str(), true);
	else
	if (element_name == CIwGameXomlNames::OnKeyBack_Hash)
		EventsManager->addEvent("OnKeyBack", data.c_str(), true);
	else
	if (element_name == CIwGameXomlNames::OnKeyMenu_Hash)
		EventsManager->addEvent("OnKeyMenu", data.c_str(), true);
	else
	if (element_name == CIwGameXomlNames::OnOrientationChange_Hash)
		EventsManager->addEvent("OnOrientationChange", data.c_str(), true);
	else
	if (element_name == CIwGameXomlNames::OnTick_Hash)
	{
		EventsManager->addEvent("OnTick", data.c_str(), true);
		TickEnabled = true;
	}
	else
	if (element_name == CIwGameXomlNames::Batch_Hash)
	{
		if (SpriteManager != NULL)
			SpriteManager->setBatching(data.GetAsBool());
	}
	else
	if (element_name == CIwGameXomlNames::ScriptEngine_Hash)
	{
		CIwGameString se = data.c_str();
		setScriptEngine(se);
	}
	else
		return false;

	return true;
}

bool CIwGameScene::UpdateFromAnimation(CIwGameAnimInstance *animation)
{
	unsigned int element_name = animation->getTargetPropertyHash();
	bool delta = animation->isDelta();

	if (Camera != NULL)
	{
		if (element_name == CIwGameXomlNames::Position_Hash)
		{
			CIwGameAnimFrameVec2* frame = (CIwGameAnimFrameVec2*)animation->getCurrentData();
			if (delta)
			{
				CIwFVec2 pos = Camera->getPosition();
				pos.x += frame->data.x;
				pos.y += frame->data.y;
				Camera->setPosition(pos.x, pos.y);
			}
			else
				Camera->setPosition(frame->data.x, frame->data.y);
			return true;
		}
		else
		if (element_name == CIwGameXomlNames::Angle_Hash)
		{
			CIwGameAnimFrameFloat* frame = (CIwGameAnimFrameFloat*)animation->getCurrentData();
			if (delta)
				Camera->setAngle(Camera->getAngle() + frame->data);
			else
				Camera->setAngle(frame->data);
			return true;
		}
		else
		if (element_name == CIwGameXomlNames::Scale_Hash)
		{
			CIwGameAnimFrameFloat* frame = (CIwGameAnimFrameFloat*)animation->getCurrentData();
			if (delta)
				Camera->setScale(Camera->getScale() + frame->data);
			else
				Camera->setScale(frame->data);
			return true;
		}
	}
	if (element_name == CIwGameXomlNames::Colour_Hash || element_name == CIwGameXomlNames::Color_Hash)
	{
		CIwGameAnimFrameVec4* frame = (CIwGameAnimFrameVec4*)animation->getCurrentData();
		if (delta)
		{
			Colour.r += (uint8)frame->data.x;
			Colour.g += (uint8)frame->data.y;
			Colour.b += (uint8)frame->data.z;
			Colour.a += (uint8)frame->data.w;
		}
		else
		{
			Colour.r = (uint8)frame->data.x;
			Colour.g = (uint8)frame->data.y;
			Colour.b = (uint8)frame->data.z;
			Colour.a = (uint8)frame->data.w;
		}
		return true;
	}
	else
	if (element_name == CIwGameXomlNames::Opacity_Hash)
	{
		CIwGameAnimFrameFloat* frame = (CIwGameAnimFrameFloat*)animation->getCurrentData();
		if (delta)
			Colour.a += (uint8)frame->data;
		else
			Colour.a = (uint8)frame->data;
	}
	else
	if (element_name == CIwGameXomlNames::Clipping_Hash)
	{
		CIwGameAnimFrameRect* frame = (CIwGameAnimFrameRect*)animation->getCurrentData();
		ClippingArea.x = (uint8)frame->data.x;
		ClippingArea.y = (uint8)frame->data.y;
		ClippingArea.w = (uint8)frame->data.w;
		ClippingArea.h = (uint8)frame->data.h;
		return true;
	}
	else
	if (element_name == CIwGameXomlNames::Visible_Hash)
	{
		CIwGameAnimFrameBool* frame = (CIwGameAnimFrameBool*)animation->getCurrentData();
		IsVisible = frame->data;
		return true;
	}
	else
	if (element_name == CIwGameXomlNames::Timeline_Hash)
	{
		CIwGameAnimFrameString* frame = (CIwGameAnimFrameString*)animation->getCurrentData();
		if (!frame->data.IsEmpty())
		{
			CIwGameAnimTimeline* timeline = CIwGameAnimTimelinesManager::FindTimeline(frame->data.getHash(), this);
			if (timeline != NULL && timeline != Timeline)
			{
				setTimeline(timeline);
				timeline->setTargetElement(this);
			}
		}
		return true;
	}
	else
	if (element_name == CIwGameXomlNames::Camera_Hash)
	{
		CIwGameAnimFrameString* frame = (CIwGameAnimFrameString*)animation->getCurrentData();
		if (!frame->data.IsEmpty())
		{
			// Find camera resource
			CIwGameCamera* camera = (CIwGameCamera*)getResourceManager()->findResource(frame->data.c_str(), CIwGameXomlNames::Camera_Hash);
			if (camera != NULL && camera != Camera)
			{
				setCamera(camera);
			}
		}
		return true;
	}
#if defined(IW_GAME_BOX2D)
	else
	if (element_name == CIwGameXomlNames::PhysicsTimestep_Hash)
	{
		if (Box2dWorld != NULL)
		{
			CIwGameAnimFrameFloat* frame = (CIwGameAnimFrameFloat*)animation->getCurrentData();
			ConstantTimeStep = frame->data;
		}
	}
#endif // IW_GAME_BOX2D

	return false;
}

void CIwGameScene::UpdateBindings(bool force_modified)
{
	for (CIwGameXomlBindings::_Iterator it = Bindings->begin(); it != Bindings->end(); ++it)
	{
		unsigned int element_name = (*it)->Property;
		CIwGameXomlVariable* var = (*it)->Variable;

		if (var->Modified || force_modified)
		{
			if (Camera != NULL)
			{
				if (element_name == CIwGameXomlNames::Position_Hash)
				{
					if (var->Type == VT_Vec2)
					{
						CIwFVec2* data = &((CIwGameXomlVariableVec2*)var)->NativeValue;
						Camera->setPosition(data->x, data->y);
					}
#if defined(_DEBUG)
					else
						CIwGameError::LogError("Warning: Incorrect binding variable type, expected vec2 for Position");
#endif // _DEBUG
				}
				else
				if (element_name == CIwGameXomlNames::Angle_Hash)
				{
					if (var->Type == VT_Float)
						Camera->setAngle(((CIwGameXomlVariableFloat*)var)->NativeValue);
#if defined(_DEBUG)
					else
						CIwGameError::LogError("Warning: Incorrect binding variable type, expected float for Angle");
#endif // _DEBUG
				}
				else
				if (element_name == CIwGameXomlNames::Scale_Hash)
				{
					if (var->Type == VT_Float)
						Camera->setScale(((CIwGameXomlVariableFloat*)var)->NativeValue);
#if defined(_DEBUG)
					else
						CIwGameError::LogError("Warning: Incorrect binding variable type, expected float for Scale");
#endif // _DEBUG
				}
			}
			if (element_name == CIwGameXomlNames::Colour_Hash || element_name == CIwGameXomlNames::Color_Hash)
			{
				if (var->Type == VT_Vec4)
				{
					CIwFVec4* data = &((CIwGameXomlVariableVec4*)var)->NativeValue;
					Colour.r = (uint8)data->x;
					Colour.g = (uint8)data->y;
					Colour.b = (uint8)data->z;
					Colour.a = (uint8)data->w;
				}
#if defined(_DEBUG)
				else
					CIwGameError::LogError("Warning: Incorrect binding variable type, expected vec4 for Colour");
#endif // _DEBUG
			}
			else
			if (element_name == CIwGameXomlNames::Opacity_Hash)
			{
				if (var->Type == VT_Float)
				{
					float data = ((CIwGameXomlVariableFloat*)var)->NativeValue;
					Colour.a = (uint8)data;
				}
#if defined(_DEBUG)
				else
					CIwGameError::LogError("Warning: Incorrect binding variable type, expected float for Opacity");
#endif // _DEBUG
			}
			else
			if (element_name == CIwGameXomlNames::Clipping_Hash)
			{
				if (var->Type == VT_Vec4)
				{
					CIwFVec4* data = &((CIwGameXomlVariableVec4*)var)->NativeValue;
					ClippingArea.x = (int16)data->x;
					ClippingArea.y = (int16)data->y;
					ClippingArea.w = (int16)data->z;
					ClippingArea.h = (int16)data->w;
				}
#if defined(_DEBUG)
				else
					CIwGameError::LogError("Warning: Incorrect binding variable type, expected vec4 for Clipping");
#endif // _DEBUG
			}
			else
			if (element_name == CIwGameXomlNames::Timeline_Hash)
			{
				if (var->Type == VT_String)
				{
					if (!var->getValue().IsEmpty())
					{
						CIwGameAnimTimeline* timeline = CIwGameAnimTimelinesManager::FindTimeline(var->getValue().getHash(), this);
						if (timeline != NULL && timeline != Timeline)
						{
							setTimeline(timeline);
							timeline->setTargetElement(this);
						}
					}
				}
#if defined(_DEBUG)
				else
					CIwGameError::LogError("Warning: Incorrect binding variable type, expected string for Timeline");
#endif // _DEBUG
			}
			else
			if (element_name == CIwGameXomlNames::Binding_Hash)
			{
				if (var->Type == VT_String)
				{
					if (!var->getValue().IsEmpty())
					{
						CIwGameXomlBindings* bindings = (CIwGameXomlBindings*)getResourceManager()->findResource(var->getValue().c_str(), CIwGameXomlNames::Bindings_Hash);
						setBindings(bindings);
						return;	// Bindings have changed so do not update any further bindnigs
					}
				}
#if defined(_DEBUG)
				else
					CIwGameError::LogError("Warning: Incorrect binding variable type, expected string for Bindings");
#endif // _DEBUG
			}
			else
			if (element_name == CIwGameXomlNames::Camera_Hash)
			{
				if (var->Type == VT_String)
				{
					if (!var->getValue().IsEmpty())
					{
						CIwGameCamera* camera = (CIwGameCamera*)getResourceManager()->findResource(var->getValue().c_str(), CIwGameXomlNames::Camera_Hash);
						if (camera != NULL && camera != Camera)
							setCamera(camera);
					}
				}
#if defined(_DEBUG)
				else
					CIwGameError::LogError("Warning: Incorrect binding variable type, expected string for Camera");
#endif // _DEBUG
			}
			else
			if (element_name == CIwGameXomlNames::Type_Hash)
			{
				if (var->Type == VT_Int)
					Type = ((CIwGameXomlVariableInt*)var)->NativeValue;
#if defined(_DEBUG)
				else
					CIwGameError::LogError("Warning: Incorrect binding variable type, expected int for Type");
#endif // _DEBUG
			}
			else
			if (element_name == CIwGameXomlNames::Active_Hash)
			{
				if (var->Type == VT_Bool)
					IsActive = ((CIwGameXomlVariableBool*)var)->NativeValue;
#if defined(_DEBUG)
				else
					CIwGameError::LogError("Warning: Incorrect binding variable type, expected bool for Active");
#endif // _DEBUG
			}
			else
			if (element_name == CIwGameXomlNames::Visible_Hash)
			{
				if (var->Type == VT_Bool)
					IsVisible = ((CIwGameXomlVariableBool*)var)->NativeValue;
#if defined(_DEBUG)
				else
					CIwGameError::LogError("Warning: Incorrect binding variable type, expected bool for Visible");
#endif // _DEBUG
			}
			else
			if (element_name == CIwGameXomlNames::AllowSuspend_Hash)
			{
				if (var->Type == VT_Bool)
					AllowSuspend = ((CIwGameXomlVariableBool*)var)->NativeValue;
#if defined(_DEBUG)
				else
					CIwGameError::LogError("Warning: Incorrect binding variable type, expected bool for AllowSuspend");
#endif // _DEBUG
			}
			else
			if (element_name == CIwGameXomlNames::AllowFocus_Hash)
			{
				if (var->Type == VT_Bool)
					AllowFocus = ((CIwGameXomlVariableBool*)var)->NativeValue;
#if defined(_DEBUG)
				else
					CIwGameError::LogError("Warning: Incorrect binding variable type, expected bool for AllowFocus");
#endif // _DEBUG
			}
		}
	}
}

	
//
//
//
//
//  CIwGameXomlAction_Scenes implementation
//
//
//
//
void CIwGameXomlAction_Scenes::Execute(IIwGameXomlResource* source, CIwGameAction* action)
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
	else
	if (source->getClassTypeHash() == CIwGameXomlNames::Game_Hash)
	{
		game = (CIwGame*)source;
	}
	IIwGameXomlResource* cont = (actor != NULL) ? (IIwGameXomlResource*)actor : (IIwGameXomlResource*)scene;

	if (Type == Action_CallScript)	// Calls a script function
	{
		CIwGameString* p1 = &action->getParameter1(cont);
		CIwGameString* p2 = NULL;
		CIwGameActor* new_actor = NULL;
		CIwGameScene* new_scene = scene;
		if (!action->Params[1].IsEmpty())	// Check for a specific scene or actor
		{
			p2 = &action->getParameter2(cont);
			new_actor = scene->findActor(p2->getHash());
			if (new_actor != NULL)
				actor = new_actor;
			else
			{
				new_scene = game->findScene(p2->getHash());
				if (new_scene == NULL)
				{
					CIwGameError::LogError("Warning: Script target actor or scene not found - ", p2->c_str());
					return;
				}
			}
		}
		CIwGameString* p3 = NULL;
		CIwGameString* p4 = NULL;
		CIwGameString* p5 = NULL;
		if (!action->Params[2].IsEmpty())
			p3 = &action->getParameter3(cont);
		if (!action->Params[3].IsEmpty())
			p4 = &action->getParameter4(cont);
		if (!action->Params[4].IsEmpty())
			p5 = &action->getParameter5(cont);

		if (scene->getScriptEngine() == NULL)
		{
			CIwGameError::LogError("Warning: CallScript - Scene does not contain a script engine");
			return;
		}

		if (p2 != NULL)
		{
			if (new_actor != NULL)
				scene->getScriptEngine()->CallFunction(new_actor, p1, p3, p4, p5);
			else
				scene->getScriptEngine()->CallFunction(new_scene, p1, p3, p4, p5);
		}
		else
		{
			if (actor != NULL)
				scene->getScriptEngine()->CallFunction(actor, p1, p3, p4, p5);
			else
				scene->getScriptEngine()->CallFunction(new_scene, p1, p3, p4, p5);
		}

		return;
	}
	else
	if (Type == Action_Inline)	// Calls script inline
	{
		CIwGameString* p1 = &action->getParameter1(cont);
		if (!action->Params[1].IsEmpty())	// Check for a specific scene
		{
			CIwGameString* p2 = &action->getParameter2(cont);
			scene = game->findScene(p2->getHash());
			if (scene == NULL)
			{
				CIwGameError::LogError("Warning: Inline action - Scene not found - ", p2->c_str());
				return;
			}
			if (scene->getScriptEngine() == NULL)
			{
				CIwGameError::LogError("Warning: Inline action - Scene does not contain a script engine");
				return;
			}

			// Run script in scene script space
			scene->getScriptEngine()->RunScript(p1->c_str(), p1->GetLength());
		}
		else
		{
			// Run script in global script space
			IW_GAME_GLOBAL_RESOURCES->getScriptEngine()->RunScript(p1->c_str(), p1->GetLength());
		}
		return;
	}
	else
	if (Type == Action_AddModifier)
	{
		if (!action->Params[3].IsEmpty())	// Check for a specific scene
		{
			CIwGameScene* new_scene = game->findScene(action->getParameter4(cont).getHash());
			if (new_scene != NULL)
				scene = new_scene;
			else
				CIwGameError::LogError("Warning: AddModifier - Scene was not found, current scene being used - ", action->getParameter4(cont).c_str());
		}
		if (!action->Params[2].IsEmpty())	// Check for a specific actor
		{
			if (scene == NULL)
				CIwGameError::LogError("Error: AddModifier - Scene is invalid");
			else
			{
				CIwGameActor* new_actor = scene->findActor(action->getParameter3(cont).getHash());
				if (new_actor != NULL)
					actor = new_actor;
				else
					CIwGameError::LogError("Warning: AddModifier - Actor was not found - ", action->getParameter3(cont).c_str());
			}
		}

		if (actor != NULL)
		{
			// If actor has no modifier list then create one
			if (actor->getModifiers() == NULL)
				actor->setModifiers(new CIwGameModifierManager());
		}
		else
		if (scene != NULL)
		{
			// If scene has no modifier list then create one
			if (scene->getModifiers() == NULL)
				scene->setModifiers(new CIwGameModifierManager());
		}
		else
		{
			CIwGameError::LogError("Warning: AddModifier - can only be added to actors or scenes - ", action->getParameter1(cont).c_str());
			return;
		}

		// Find the modifiers creator
		IIwGameModifierCreator* creator = IW_GAME_MODS->findCreator(action->getParameter1(cont).getHash());
		if (creator != NULL)
		{
			IIwGameModifier* mod = creator->CreateInstance();
			if (mod != NULL)
			{
				mod->setActive(true);
				mod->setParameter(0, action->getParameter2(cont));
				if (actor != NULL)
				{
					if (!actor->getModifiers()->addModifier(mod))
						delete mod;
				}
				else
				if (scene != NULL)
				{
					if (!scene->getModifiers()->addModifier(mod))
						delete mod;
				}
			}
		}
		else
			CIwGameError::LogError("Warning: Modifier not found and could not be added - ", action->getParameter1(cont).c_str());

		return;
	}
	else
	if (Type == Action_ChangeModifier)
	{
		if (!action->Params[3].IsEmpty())	// Check for a specific scene
		{
			CIwGameScene* new_scene = game->findScene(action->getParameter4(cont).getHash());
			if (new_scene != NULL)
				scene = new_scene;
			else
				CIwGameError::LogError("Warning: ChangeModifier - Scene was not found, current scene being used - ", action->getParameter4(cont).c_str());
		}
		if (!action->Params[2].IsEmpty())	// Check for a specific actor
		{
			if (scene == NULL)
				CIwGameError::LogError("Error: ChangeModifier - Scene is invalid");
			else
			{
				CIwGameActor* new_actor = scene->findActor(action->getParameter3(cont).getHash());
				if (new_actor != NULL)
					actor = new_actor;
				else
					CIwGameError::LogError("Warning: ChangeModifier - Actor was not found - ", action->getParameter3(cont).c_str());
			}
		}

		unsigned int command = action->getParameter2(cont).getHash();
		if (actor != NULL)
		{
			if (actor->getModifiers() != NULL)
			{
				IIwGameModifier* mod = actor->getModifiers()->findModifier(action->getParameter1(cont).getHash());
				if (mod != NULL)
				{
					if (command == CIwGameXomlNames::Activate_Hash)
						mod->setActive(true);
					else
					if (command == CIwGameXomlNames::Deactivate_Hash)
						mod->setActive(false);
					else
					if (command == CIwGameXomlNames::Toggle_Hash)
						mod->setActive(!mod->isActive());
					else
					if (command == CIwGameXomlNames::Remove_Hash)
						actor->getModifiers()->removeModifier(mod);
					else
						CIwGameError::LogError("Warning: Unknown modifier command - ", action->getParameter2(cont).c_str());
				}
				else
					CIwGameError::LogError("Warning: Actor did not contain the modifier - ", action->getParameter1(cont).c_str());
			}
			else
				CIwGameError::LogError("Warning: Actor has no modifiers");
		}
		else
		if (scene != NULL)
		{
			if (scene->getModifiers() != NULL)
			{
				IIwGameModifier* mod = scene->getModifiers()->findModifier(action->getParameter1(cont).getHash());
				if (mod != NULL)
				{
					if (command == CIwGameXomlNames::Activate_Hash)
						mod->setActive(true);
					else
					if (command == CIwGameXomlNames::Deactivate_Hash)
						mod->setActive(false);
					else
					if (command == CIwGameXomlNames::Toggle_Hash)
						mod->setActive(!mod->isActive());
					else
					if (command == CIwGameXomlNames::Remove_Hash)
						scene->getModifiers()->removeModifier(mod);
					else
						CIwGameError::LogError("Warning: Unknown modifier command - ", action->getParameter2(cont).c_str());
				}
				else
					CIwGameError::LogError("Warning: Scene did not contain the modifier - ", action->getParameter1(cont).c_str());
			}
			else
				CIwGameError::LogError("Warning: Scene has no modifiers");
		}
		else
		{
			CIwGameError::LogError("Warning: Modifiers can only be removed from actors or scenes - ", action->getParameter1(cont).c_str());
			return;
		}

		return;
	}
	else
	if (Type == Action_CallActions)
	{
		if (!action->Params[1].IsEmpty())
		{
			CIwGameScene* other_scene = game->findScene(action->getParameter2(cont).getHash());
			if (other_scene == NULL)
				CIwGameError::LogError("Warning: Actions - Other scene was not found - ", action->getParameter2(cont).c_str());
			else
				scene = other_scene;
		}
		if (scene == NULL)
		{
			CIwGameError::LogError("Warning: CallActions needs to be called from inside a scene");
			return;
		}

		CIwGameActions* actions = CIwGameActionsManager::FindActions(action->getParameter1(cont).getHash(), scene);
		if (actions != NULL)
			actions->Execute(scene);
		else
			CIwGameError::LogError("Warning: Actions not found - ", action->getParameter1(cont).c_str());

		return;
	}
	else
	if (Type == Action_SetAllSceneTimelines)
	{
		CIwGameAnimTimeline* timeline = IW_GAME_GLOBAL_RESOURCES->getTimelinesManager()->findTimeline(action->getParameter1(cont).getHash());
		if (timeline != NULL)
		{
			game->SetAllTimelines(timeline);
			timeline->restart();
		}
		else
			CIwGameError::LogError("Warning: Actions - Timeline was not found - ", action->getParameter1(cont).c_str());

		return;
	}
	else
	if (Type == Action_KillAllScenes)
	{
		if (action->Params[0].IsEmpty())
			game->removeAllScenes((CIwGameScene*)NULL);
		else
			game->removeAllScenes(action->getParameter1(cont).getHash());

		return;
	}
	if (Type == Action_RemoteReq)
	{
		CIwGameString* p1 = NULL;
		CIwGameString* p2 = NULL;
		if (action->Params[0].IsEmpty())
		{
			CIwGameError::LogError("Error: Action RemoteReq requires Remote Request resource name as Param1");
			return;
		}
		p1 = &action->getParameter1(cont);
		CIwGameRemoteReq* request = (CIwGameRemoteReq*)scene->getResourceManager()->findResource(p1->c_str(), CIwGameXomlNames::RemoteReq_Hash);
		if (request == NULL)
		{
			CIwGameError::LogError("Warning: XOML - Could not find named remote request resource in the scene or in the global resources - ", p1->c_str());
			return;
		}
		if (!action->Params[1].IsEmpty())
		{
			p2 = &action->getParameter2(cont);
			request->setData(p2->c_str());
		}

		request->MakeRequest();
		return;
	}

	CIwGameScene* new_scene = action->Params[0].IsEmpty() ? scene : game->findScene(action->getParameter1(cont).getHash());
	if (new_scene != NULL)
	{
		switch (Type)
		{
		case Action_ChangeScene:
			game->changeScene(action->getParameter1(cont).getHash());
			break;
		case Action_SuspendScene:
			new_scene->Suspend(NULL);
			break;
		case Action_ResumeScene:
			new_scene->Resume(NULL);
			break;
		case Action_HideScene:
			new_scene->setVisible(false);
			break;
		case Action_ShowScene:
			new_scene->setVisible(true);
			break;
		case Action_KillScene:
			game->removeScene(new_scene);
			break;
		}
	}
	else
		CIwGameError::LogError("Warning: Actions - Scene was not found - ", action->getParameter1(cont).c_str());
}


