// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. The only restriction to its usage is that this header must remain intact and visible in all IwGame engine files.
// If you use this engine in your product, whilst it is not mandatory, a mention of the IwGame engine would be nice or a quick email to let us know where it is being used.
//
//

#include "IwGameActor.h"
#include "IwGameCamera.h"
#include "IwGameXoml.h"
#include "IwGameActions.h"
#include "IwGame.h"
#include "IwGameBrush.h"
#include "IwGameUI.h"


//
//
//
//
// CIwGameActor implementation
//
//
//
//

CIwGameActor::~CIwGameActor()
{
	// Only delete an unmanaged timeline
	setTimeline(NULL);
	setBindings(NULL);
	setSimpleBinding(NULL);

	// Delete visual if one was attached
//	if (Visual != NULL)
//		Visual->getManager()->removeSprite(Visual);

	// Clean up Box2D body object
#if defined(IW_GAME_BOX2D)
	SAFE_DELETE(Box2dBody)
#endif // IW_GAME_BOX2D_H_

	// Clean-up managers
	SAFE_DELETE(ActionsManager)
	SAFE_DELETE(TimelinesManager)
	SAFE_DELETE(EventsManager)

	// Delete modifiers
	SAFE_DELETE(Modifiers)
}

void CIwGameActor::setTimeline(CIwGameAnimTimeline* timeline)
{
	if (Timeline != NULL)
	{
		if (!Timeline->isManaged())
		{
			SAFE_DELETE(Timeline)
		}
	}
	Timeline = timeline;
}

void CIwGameActor::setBindings(CIwGameXomlBindings* bindings)
{
	if (Bindings != NULL)
	{
		if (!Bindings->isManaged())
		{
			SAFE_DELETE(Bindings)
		}
	}
	Bindings = bindings;
}

void CIwGameActor::setSimpleBinding(CIwGameXomlBinding* binding)
{
	SAFE_DELETE(SimpleBinding)
	SimpleBinding = binding;
}

void CIwGameActor::setCollisionRect(CIwRect& rect)
{
	CollisionRect = rect;
	int dx = rect.w >> 1;
	int dy = rect.h >> 1;
	CollisionSize = dx * dx + dy * dy;
}

void CIwGameActor::setCollisionSize(int size)
{
	CIwRect rc(-size >> 1, -size >> 1, size, size);
	setCollisionRect(rc);
}

void CIwGameActor::setLinkedTo(CIwGameActor* actor)
{
	// Notify old parent that we are removing ourselves
	if (LinkedTo != NULL)
	{
		LinkedTo->LinkChanged(this, true);
		if (Visual != NULL)
			Visual->setLinkedTo(NULL);
	}

	LinkedTo = actor;

	// Notify new actor that we are linking to it
	if (LinkedTo != NULL)
	{
		LinkedTo->LinkChanged(this, false);
		if (Visual != NULL && LinkedTo->getVisual() != NULL)
			Visual->setLinkedTo(LinkedTo->getVisual());
	}
}

void CIwGameActor::setGridPos(int x, int y)
{
	// Here we unlink the actor and then relink it back to the same actor in case it changes grid position (ensures that it gets linked into the correct cell)
	CIwGameActor* actor = LinkedTo;
	setLinkedTo(NULL);
	GridPos = (x << 16) | y;
	setLinkedTo(actor);
}

void CIwGameActor::setPosition(float x, float y)
{
	Position.x = x; Position.y = y;
#if defined(IW_GAME_BOX2D)
	b2Body* body = (Box2dBody == NULL) ? NULL : Box2dBody->getBody();
	if (body != NULL)
	{
		CIwGameBox2dWorld* world = Scene->getBox2dWorld();
		if (world != NULL)
			body->SetTransform(b2Vec2(world->PixelToWorldX(x), world->PixelToWorldY(y)), body->GetAngle());
	}
#endif	// IW_GAME_BOX2D
}

void CIwGameActor::setDocking(eIwGameScene_Docking docking, int width, int height)
{
	CIwVec2 size = Scene->getScreenSize();
	CIwFVec2 pos = CIwFVec2(0, 0);

	switch (docking)
	{
	case DK_Top:
		pos = Scene->ScreenToVirtual(0, 0);
		pos.x = Position.x;
		pos.y += (height + 1) / 2 + Margin.w;
		break;
	case DK_Bottom:
		pos = Scene->ScreenToVirtual(0, (float)size.y);
		pos.x = Position.x;
		pos.y -= (height + 1) / 2 + Margin.h;
		break;
	case DK_Left:
		pos = Scene->ScreenToVirtual(0, 0);
		pos.x += (width + 1) / 2 + Margin.x;
		pos.y = Position.y;
		break;
	case DK_Right:
		pos = Scene->ScreenToVirtual((float)size.x, 0);
		pos.x -= (width + 1) / 2 + Margin.y;
		pos.y = Position.y;
		break;
	case DK_TopLeft:
		pos = Scene->ScreenToVirtual(0, 0);
		pos.x += (width + 1) / 2 + Margin.x;
		pos.y += (height + 1) / 2 + Margin.w;
		break;
	case DK_TopRight:
		pos = Scene->ScreenToVirtual((float)size.x, 0);
		pos.x -= (width + 1) / 2 + Margin.y;
		pos.y += (height + 1) / 2 + Margin.w;
		break;
	case DK_BottomLeft:
		pos = Scene->ScreenToVirtual(0, (float)size.y);
		pos.x += (width + 1) / 2 + Margin.x;
		pos.y -= (height + 1) / 2 + Margin.h;
		break;
	case DK_BottomRight:
		pos = Scene->ScreenToVirtual((float)size.x, (float)size.y);
		pos.x -= (width + 1) / 2 + Margin.y;
		pos.y -= (height + 1) / 2 + Margin.h;
		break;
	}

	setPosition(pos);

	Docking = docking;
}

void CIwGameActor::setDocking(eIwGameScene_Docking docking)
{
	if (Visual == NULL)
	{
		Docking = docking;
		return;
	}

	if (ScreenDocking)
		setDocking(docking, Visual->getDestWidth(), Visual->getDestHeight());
}


void CIwGameActor::setAngle(float angle)
{
	Angle = angle;
#if defined(IW_GAME_BOX2D)
	Angle = -Angle;
	b2Body* body = (Box2dBody == NULL) ? NULL : Box2dBody->getBody();
	if (body != NULL)
	{
		body->SetTransform(body->GetPosition(), (Angle * 3.1415927f) / 180.0f);	// Degrees to radians
	}
#endif	// IW_GAME_BOX2D
}

void CIwGameActor::setVelocity(float x, float y)
{
	Velocity.x = x;  Velocity.y = y;
#if defined(IW_GAME_BOX2D)
	b2Body* body = (Box2dBody == NULL) ? NULL : Box2dBody->getBody();
	if (body != NULL)
	{
		body->SetLinearVelocity(b2Vec2(x, y));
	}
#endif	// IW_GAME_BOX2D
}

void CIwGameActor::setVelocityDamping(float x, float y)
{
	VelocityDamping.x = x;  VelocityDamping.y = y;
#if defined(IW_GAME_BOX2D)
	b2Body* body = (Box2dBody == NULL) ? NULL : Box2dBody->getBody();
	if (body != NULL)
		body->SetLinearDamping(x);
#endif	// IW_GAME_BOX2D
}

void CIwGameActor::setVelocityDamping(float damping)
{
	setVelocityDamping(damping, damping);
}

void CIwGameActor::setAngularVelocity(float velocity)
{
	AngularVelocity = velocity;
#if defined(IW_GAME_BOX2D)
	b2Body* body = (Box2dBody == NULL) ? NULL : Box2dBody->getBody();
	if (body != NULL)
		body->SetAngularVelocity(AngularVelocity);
#endif	// IW_GAME_BOX2D
}

void CIwGameActor::setAngularVelocityDamping(float damping)
{
	AngularVelocityDamping = damping;
#if defined(IW_GAME_BOX2D)
	b2Body* body = (Box2dBody == NULL) ? NULL : Box2dBody->getBody();
	if (body != NULL)
		body->SetAngularDamping(AngularVelocityDamping);
#endif	// IW_GAME_BOX2D
}

void CIwGameActor::setActive(bool active)
{
	if (IsActive == active)
		return;
	IsActive = active;
#if defined(IW_GAME_BOX2D)
	b2Body* body = (Box2dBody == NULL) ? NULL : Box2dBody->getBody();
	if (body != NULL)
		body->SetActive(active);
#endif	// IW_GAME_BOX2D
}

void CIwGameActor::setVisible(bool visible, bool force_set)
{
	if (Visual != NULL)
	{
		if (visible && !IsVisible)
			Visual->forceTransformDirty();
	}
	IsVisible = visible;
}

void CIwGameActor::setCollidable(bool collidable)
{
	IsCollidable = collidable;
#if defined(IW_GAME_BOX2D)
	if (Box2dBody != NULL)
	{
		if (!collidable)
			Box2dBody->setCollisionFlags(0, 0, 0);
		else
		{
			int flags = Box2dBody->getCollisionFlags();
			Box2dBody->setCollisionFlags(flags >> 16, (flags >> 8) & 0xff, flags & 0xff);
		}
	}
#endif	// IW_GAME_BOX2D
}

void CIwGameActor::Init()
{
	OnCreate();
}

void CIwGameActor::Reset()
{
	IsActive = true;
	IsVisible = true;
	IsCollidable = false;
	IsTappable = true;
	IsDraggable = false;
	IsDragging = false;
	IsDragging2 = false;
	WrapPosition = false;
	Type = 0;
	Position.x = 0;
	Position.y = 0;
	Depth = 1.0f;
	Origin.x = 0;
	Origin.y = 0;
	OriginalPosition.x = 0;
	OriginalPosition.y = 0;
	OriginalScale.x = 1.0f;
	OriginalScale.y = 1.0f;
	ScaleX = 1.0f;
	ScaleY = 1.0f;
	OriginalAngle = 0;
	Angle = 0;
	Velocity.x = 0;
	Velocity.y = 0;
	AngularVelocity = 0;
	VelocityDamping.x = 1.0f;
	VelocityDamping.y = 1.0f;
	AngularVelocityDamping = 1.0f;
	CollisionSize = 0;
	CollisionRect.x = 0;
	CollisionRect.y = 0;
	CollisionRect.w = 0;
	CollisionRect.h = 0;
	Colour.r = 0xff;
	Colour.g = 0xff;
	Colour.b = 0xff;
	Colour.a = 0xff;
	OriginalColour.r = 0xff;
	OriginalColour.g = 0xff;
	OriginalColour.b = 0xff;
	OriginalColour.a = 0xff;
	Layer = 0;
	AspectLock = AL_None;
	Destroyed = false;
	TouchIndex = -1;
	Docking = DK_None;
	Margin.x = 0;
	Margin.y = 0;
	Margin.w = 0;
	Margin.h = 0;
	Size.x = 0;
	Size.y = 0;
	GridPos = 0;
	Target = NULL;
	ReceiveEventFromChild = false;
	Category = AC_Generic;
	ScreenDocking = true;
	HoldFocus = false;
	UseParentOpacity = true;
}

bool CIwGameActor::Update(float dt)
{
	if (!IsActive)
		return true;

	// Update any data bindings
	UpdateBindings();

	// Update modifiers
	if (Modifiers != NULL)
		Modifiers->Execute(this, dt);

	if (!IsVisible)
		return true;

	// Update dragging
	if (IsDraggable)
	{
		if (IsDragging)
		{
			CIwVec2 raw_touch_pos  = IW_GAME_INPUT->getTouchedPos(TouchIndex);
			CIwFVec2 pos = Scene->ScreenToCamera((float)raw_touch_pos.x, (float)raw_touch_pos.y);
			setPosition(pos.x + DragAnchor.x, pos.y + DragAnchor.y);
		}
	}

#if defined(IW_GAME_BOX2D)
	b2Body* body = (Box2dBody == NULL) ? NULL : Box2dBody->getBody();
	if (body != NULL)
	{
		// Update actor position and angle to match the physics engine
		float x = body->GetPosition().x;
		float y = body->GetPosition().y;
		CIwGameBox2dWorld* world = Scene->getBox2dWorld();
		if (world != NULL)
		{
			Position.x = world->WorldToPixelX(x);
			Position.y = world->WorldToPixelY(y);
		}
		Angle = (body->GetAngle() * 180.0f) / 3.1415927f;	// Radians to degrees
	}
	else
#endif	// IW_GAME_BOX2D
	{
		// Update velocity
		PreviousPosition = Position;
		Position += Velocity * dt;
		Velocity.x *= VelocityDamping.x;
		Velocity.y *= VelocityDamping.y;
		PreviousAngle = Angle;
		Angle += AngularVelocity * dt;
		AngularVelocity *= AngularVelocityDamping;
	}

	// Wrap actor position with scene extents (if enabled)
	CIwRect extents = Scene->getExtents();
	if (WrapPosition)
	{
#if defined(IW_GAME_BOX2D)
		bool wrapped = false;
		if (Position.x < extents.x)
		{
			Position.x += extents.w;
			wrapped = true;
		}
		else
		if (Position.x >= (extents.x + extents.w))
		{
			Position.x -= extents.w;
			wrapped = true;
		}
		if (Position.y < extents.y)
		{
			Position.y += extents.h;
			wrapped = true;
		}
		else
		if (Position.y >= (extents.y + extents.h))
		{
			Position.y -= extents.h;
			wrapped = true;
		}
		if (wrapped && body != NULL)
		{
			CIwGameBox2dWorld* world = Scene->getBox2dWorld();
			if (world != NULL)
				body->SetTransform(b2Vec2(world->PixelToWorldX(Position.x), world->PixelToWorldX(Position.y)), body->GetAngle());
		}
#else
		if (Position.x < extents.x)
			Position.x += extents.w;
		else
		if (Position.x >= (extents.x + extents.w))
			Position.x -= extents.w;
		if (Position.y < extents.y)
			Position.y += extents.h;
		else
		if (Position.y >= (extents.y + extents.h))
			Position.y -= extents.h;
#endif	// IW_GAME_BOX2D
	}

	// Update the animation timeline
	if (Timeline != NULL)
 		Timeline->Update(dt, this);

	// If this actor is collidable then add it to the scenes potential collidables list
	if (IsCollidable)
		Scene->addCollideable(this);

	return true;
}

bool CIwGameActor::UpdateVisual()
{
	if (Visual == NULL)
		return false;

	if (!IsVisible || !IsActive)
	{
		Visual->setVisible(false);
		return false;
	}
	else
	{
		// If ww are linked to another actor then check that it is visible
		if (LinkedTo != NULL)
		{
			if (!LinkedTo->isVisible() || !LinkedTo->isActive())
			{
				Visual->setVisible(false);
				return false;
			}
		}
	}

	Visual->setVisible(true);

//	if (LinkedTo != NULL)
//		Visual->setLinkedTo(LinkedTo->getVisual());

	Visual->setDepth((iwfixed)(Depth * IW_GEOM_ONE));
	Visual->setOrigin((int)(Origin.x * 8.0f), (int)(Origin.y * 8.0f));
	// Transfor actor properties to the visual
	Visual->setPosAngScale(	(int)(Position.x * 8.0f), 
							(int)(Position.y * 8.0f), 
							(iwangle)((Angle * IW_ANGLE_2PI) / 360.0f), (iwfixed)(ScaleX * IW_GEOM_ONE), (iwfixed)(ScaleY * IW_GEOM_ONE));

	// Multiply actor colour by scene colour
	CIwColour c = Colour;
	CIwColour c2 = Scene->getColour();
	c.r = (uint8)(((int)c.r * c2.r) / 255);
	c.g = (uint8)(((int)c.g * c2.g) / 255);
	c.b = (uint8)(((int)c.b * c2.b) / 255);
	c.a = (uint8)(((int)c.a * c2.a) / 255);

	Visual->setColour(c, UseParentOpacity);

	// Set visible layer
	Visual->setLayer(Layer);

	return true;
}

void CIwGameActor::removeVisual()
{
	if (Visual != NULL)
	{
		Scene->getSpriteManager()->removeSprite(Visual);
		Visual = NULL;
	}
}

CIwGameActor* CIwGameActor::HitTest(int x, int y)
{
	if (Visual == NULL || !isTappable() || !Visual->isVisibleWithParents())
		return NULL;

	if (Visual->HitTest(x, y))
		return this;		// We return a CIwGameActor so we can return a different actor if this actor hosts child actors

	return NULL;
}

bool CIwGameActor::isOutsideFocusRange(int x, int y, float scale)
{
	if (HoldFocus)
		return false;

	if (Visual == NULL)
		return true;

	return Visual->isOutsideFocusRange(x, y, scale);
}

CIwVec2 CIwGameActor::TransformPoint(float x, float y)
{
	if (Visual != NULL)
		return Visual->TransformPoint(x, y);

	return CIwVec2(0, 0);
}

bool CIwGameActor::CheckCollision(CIwGameActor* other)
{
	// This is just about as simple as colision checks can go. We check to see if the distance between the centre of both objects is less than the sum of the radii of both
	// Note that we do not use a proper distance between two points check because that involves expensive square root calculations, but this works just the same
	// The only thing to be careful of is if your coordinate system is huge and your numbers have a large range, just ensure that the term for d = dx * dx + dy * dy does not fall 
	// out of integer range
	CIwFVec2 other_pos = other->getPosition();
	int		other_collision_size = other->getCollisionSize();
	int		dx = (int)(Position.x - other_pos.x);
	int		dy = (int)(Position.y - other_pos.y);
	int		d_max = (CollisionSize >> 1) + (other_collision_size >> 1);
	int		d = dx * dx + dy * dy;

	d_max *= d_max;
	if (d <= d_max)
		return true;

	return false;
}

CIwGameActor* CIwGameActor::FindEventDependentParent()
{
	// Search down the parent chain to see if any actors allow event bubbling
	CIwGameActor* parent = LinkedTo;
	while (parent != NULL)
	{
		if (parent->canReceiveEventFromChild())
			return parent;

		parent = parent->LinkedTo;
	}

	return NULL;
}

CIwGameActor* CIwGameActor::FindClipper()
{
	if (Visual != NULL)
	{
		if (Visual->affectsClip())
			return this;
	}

	if (LinkedTo != NULL)
		return LinkedTo->FindClipper();

	return NULL;
}

void CIwGameActor::OnTapped(int index, int x, int y, bool allow_bubble)
{
//	if (TouchIndex >= 0)
//		return;

	switch (index)
	{
	case 0:
		ProcessEventActions(CIwGameXomlNames::OnTapped_Hash);
		break;
	case 1:
		ProcessEventActions(CIwGameXomlNames::OnTapped2_Hash);
		break;
	case 2:
		ProcessEventActions(CIwGameXomlNames::OnTapped3_Hash);
		break;
	case 3:
		ProcessEventActions(CIwGameXomlNames::OnTapped4_Hash);
		break;
	case 4:
		ProcessEventActions(CIwGameXomlNames::OnTapped5_Hash);
		break;
	}

	if (allow_bubble)
	{
		// Notify any dependent parents
		CIwGameActor* parent = FindEventDependentParent();
		if (parent != NULL)
			parent->OnTapped(index, x, y, allow_bubble);
	}
}

void CIwGameActor::OnBeginTouch(int index, int x, int y, bool allow_bubble)
{
	if (TouchIndex >= 0)
		return;

	TouchIndex = index;
	IsDragging = true;
	if (IsDraggable)
	{
		CIwFVec2 pos = Scene->ScreenToCamera((float)x, (float)y);
		DragAnchor.x = (int)(Position.x - pos.x);
		DragAnchor.y = (int)(Position.y - pos.y);
	}

	switch (index)
	{
	case 0:
		ProcessEventActions(CIwGameXomlNames::OnBeginTouch_Hash);
		break;
	case 1:
		ProcessEventActions(CIwGameXomlNames::OnBeginTouch2_Hash);
		break;
	case 2:
		ProcessEventActions(CIwGameXomlNames::OnBeginTouch3_Hash);
		break;
	case 3:
		ProcessEventActions(CIwGameXomlNames::OnBeginTouch4_Hash);
		break;
	case 4:
		ProcessEventActions(CIwGameXomlNames::OnBeginTouch5_Hash);
		break;
	}

	if (allow_bubble)
	{
		// Notify any dependent parents
		CIwGameActor* parent = FindEventDependentParent();
		if (parent != NULL)
			parent->OnBeginTouch(index, x, y, allow_bubble);
	}
}

void CIwGameActor::OnEndTouch(int index, int x, int y, bool allow_bubble)
{
	IsDragging = false;

	switch (index)
	{
	case 0:
		ProcessEventActions(CIwGameXomlNames::OnEndTouch_Hash);
		break;
	case 1:
		ProcessEventActions(CIwGameXomlNames::OnEndTouch2_Hash);
		break;
	case 2:
		ProcessEventActions(CIwGameXomlNames::OnEndTouch3_Hash);
		break;
	case 3:
		ProcessEventActions(CIwGameXomlNames::OnEndTouch4_Hash);
		break;
	case 4:
		ProcessEventActions(CIwGameXomlNames::OnEndTouch5_Hash);
		break;
	}

	if (allow_bubble)
	{
		// Notify any dependent parents
		CIwGameActor* parent = FindEventDependentParent();
		if (parent != NULL)
			parent->OnEndTouch(index, x, y, allow_bubble);
	}
	TouchIndex = -1;
}

void CIwGameActor::OnCreate()
{
	ProcessEventActions(CIwGameXomlNames::OnCreate_Hash);
}

void CIwGameActor::OnDestroy()
{
	ProcessEventActions(CIwGameXomlNames::OnDestroy_Hash);
	removeVisual();
	setLinkedTo(NULL);
}

void CIwGameActor::OnOrientationChange(eIwGameScene_Orientation old_orientation, eIwGameScene_Orientation new_orientation)
{
	ProcessEventActions(CIwGameXomlNames::OnOrientationChange_Hash);
}

void CIwGameActor::ProcessEventActions(unsigned int event_name)
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

void CIwGameActor::RemoveBody()
{
	setCollidable(false);
	Velocity.x = 0;
	Velocity.y = 0;
	AngularVelocity = 0;
#if defined(IW_GAME_BOX2D)
	if (Box2dBody != NULL)
	{
		SAFE_DELETE(Box2dBody)
	}
#endif // IW_GAME_BOX2D_H_
}

bool CIwGameActor::setProperty(const char* element_name, CIwGameString& data, bool delta)
{
	return setProperty(CIwGameString::CalculateHash(element_name), data, delta);
}

bool CIwGameActor::setProperty(unsigned int element_name, CIwGameString& data, bool delta)
{
	float	float_pool[8];

	if (element_name == CIwGameXomlNames::Position_Hash)
	{
		data.GetAsListOfFloat(float_pool);
		if (delta)
			setPosition(Position.x + float_pool[0], Position.y + float_pool[1]);
		else
			setPosition(float_pool[0], float_pool[1]);
	}
	else
	if (element_name == CIwGameXomlNames::PositionX_Hash)
	{
		if (delta)
			setPosition(Position.x + data.GetAsFloat(), Position.y);
		else
			setPosition(data.GetAsFloat(), Position.y);
	}
	else
	if (element_name == CIwGameXomlNames::PositionY_Hash)
	{
		if (delta)
			setPosition(Position.x, Position.y + data.GetAsFloat());
		else
			setPosition(Position.x, data.GetAsFloat());
	}
	else
	if (element_name == CIwGameXomlNames::Depth_Hash)
	{
		float d = data.GetAsFloat();
		if (delta)
			setDepth(Depth + d);
		else
			setDepth(d);
	}
	else
	if (element_name == CIwGameXomlNames::Origin_Hash)
	{
		data.GetAsListOfFloat(float_pool);
		if (delta)
		{
			Origin.x += float_pool[0];
			Origin.y += float_pool[1];
		}
		else
		{
			Origin.x = float_pool[0];
			Origin.y = float_pool[1];
		}
	}
	else
	if (element_name == CIwGameXomlNames::Velocity_Hash)
	{
		data.GetAsListOfFloat(float_pool);
		if (delta)
			setVelocity(Velocity.x + float_pool[0], Velocity.y + float_pool[1]);
		else
			setVelocity(float_pool[0], float_pool[1]);
	}
	else
	if (element_name == CIwGameXomlNames::Angle_Hash)
	{
		float d = data.GetAsFloat();
		if (delta)
			setAngle(Angle + d);
		else
			setAngle(d);
	}
	else
	if (element_name == CIwGameXomlNames::AngularVelocity_Hash)
	{
		float d = data.GetAsFloat();
		if (delta)
			setAngularVelocity(AngularVelocity + d);
		else
			setAngularVelocity(d);
	}
	else
	if (element_name == CIwGameXomlNames::Scale_Hash)
	{
		data.GetAsListOfFloat(float_pool);
		if (delta)
		{
			ScaleX += float_pool[0];
			ScaleY += float_pool[1];
		}
		else
		{
			ScaleX = float_pool[0];
			ScaleY = float_pool[1];
		}
	}
	else
	if (element_name == CIwGameXomlNames::ScaleX_Hash)
	{
		float d = data.GetAsFloat();
		if (delta)
			ScaleX += d;
		else
			ScaleX = d;
	}
	else
	if (element_name == CIwGameXomlNames::ScaleY_Hash)
	{
		float d = data.GetAsFloat();
		if (delta)
			ScaleY += d;
		else
			ScaleY = d;
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
	if (element_name == CIwGameXomlNames::Layer_Hash)
	{
		if (delta)
			Layer = data.GetAsInt();
		else
			Layer += data.GetAsInt();
	}
	else
	if (element_name == CIwGameXomlNames::Visible_Hash)
	{
		if (delta)
			IsVisible = !IsVisible;
		else
			IsVisible = data.GetAsBool();
	}
	else
	if (element_name == CIwGameXomlNames::Collidable_Hash)
	{
		if (delta)
			setCollidable(!IsCollidable);
		else
			setCollidable(data.GetAsBool());
	}
	else
	if (element_name == CIwGameXomlNames::Draggable_Hash)
	{
		if (delta)
			setDraggable(!IsDraggable);
		else
			setDraggable(data.GetAsBool());
	}
	else
	if (element_name == CIwGameXomlNames::HitTest_Hash)
	{
		if (delta)
			IsTappable = !IsTappable;
		else
			IsTappable = data.GetAsBool();
	}
	else
	if (element_name == CIwGameXomlNames::Binding_Hash)
	{
		if (!data.IsEmpty())
		{
			CIwGameString var_name, prop_name;
			int index = -1;
			if (data.SplitPropVarIndex(prop_name, var_name, index))
			{
				CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(var_name, Scene);
				if (var == NULL)
					CIwGameError::LogError("Warning: setProperty - Bound simple binding variable does not exist - ", var_name.c_str());
				else
				{
					SAFE_DELETE(SimpleBinding)

					SimpleBinding = new CIwGameXomlBinding();
					SimpleBinding->Index = index;
#if defined(_DEBUG)
					SimpleBinding->_Property = prop_name;
#endif // _DEBUG
					SimpleBinding->Property = prop_name.getHash();
					SimpleBinding->Variable = var;
				}
			}
			else
				CIwGameError::LogError("Warning: setProperty - Invalid format of simple binding, format should be [property]variable:array_index - ", data.c_str());
		}
	}
	else
	if (element_name == CIwGameXomlNames::Timeline_Hash)
	{
		if (!data.IsEmpty())
		{
			CIwGameAnimTimeline* timeline = CIwGameAnimTimelinesManager::FindTimeline(data.getHash(), this);
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
#if defined(IW_GAME_BOX2D_H_)
	else
	if (element_name == CIwGameXomlNames::Torque_Hash)
	{
		if (Box2dBody != NULL)
			Box2dBody->ApplyTorque(data.GetAsFloat());
	}
	else
	if (element_name == CIwGameXomlNames::AngularImpulse_Hash)
	{
		if (Box2dBody != NULL)
			Box2dBody->ApplyAngularImpulse(data.GetAsFloat());
	}
	else
	if (element_name == CIwGameXomlNames::Force_Hash)
	{
		if (Box2dBody != NULL)
		{
			data.GetAsListOfFloat(float_pool);
			Box2dBody->ApplyForce(float_pool[0], float_pool[1], Box2dBody->getWorld()->PixelToWorldX((Position.x + float_pool[2])), Box2dBody->getWorld()->PixelToWorldY(Position.y + float_pool[3]));
		}
	}
	else
	if (element_name == CIwGameXomlNames::LinearImpulse_Hash)
	{
		if (Box2dBody != NULL)
		{
			data.GetAsListOfFloat(float_pool);
			Box2dBody->ApplyLinearImpulse(float_pool[0], float_pool[1], Box2dBody->getWorld()->PixelToWorldX(Position.x + float_pool[2]), Box2dBody->getWorld()->PixelToWorldY(Position.y + float_pool[3]));
		}
	}
#endif // IW_GAME_BOX2D_H_
	else
		return false;

	return true;
}

bool CIwGameActor::UpdateFromAnimation(CIwGameAnimInstance *animation)
{
	unsigned int element_name = animation->getTargetPropertyHash();
	bool delta = animation->isDelta();

	if (element_name == CIwGameXomlNames::Position_Hash)
	{
		CIwGameAnimFrameVec2* frame = (CIwGameAnimFrameVec2*)animation->getCurrentData();
		if (delta)
			setPosition(OriginalPosition.x + frame->data.x, OriginalPosition.y + frame->data.y);
		else
			setPosition(frame->data.x, frame->data.y);
	}
	else
	if (element_name == CIwGameXomlNames::PositionX_Hash)
	{
		CIwGameAnimFrameFloat* frame = (CIwGameAnimFrameFloat*)animation->getCurrentData();
		if (delta)
			setPosition(OriginalPosition.x + frame->data, Position.y);
		else
			setPosition(frame->data, Position.y);
	}
	else
	if (element_name == CIwGameXomlNames::PositionY_Hash)
	{
		CIwGameAnimFrameFloat* frame = (CIwGameAnimFrameFloat*)animation->getCurrentData();
		if (delta)
			setPosition(Position.x, OriginalPosition.y + frame->data);
		else
			setPosition(Position.y, frame->data);
	}
	else
	if (element_name == CIwGameXomlNames::Depth_Hash)
	{
		CIwGameAnimFrameFloat* frame = (CIwGameAnimFrameFloat*)animation->getCurrentData();
		if (delta)
			setDepth(Depth + frame->data);
		else
			setDepth(frame->data);
	}
	else
	if (element_name == CIwGameXomlNames::Origin_Hash)
	{
		CIwGameAnimFrameVec2* frame = (CIwGameAnimFrameVec2*)animation->getCurrentData();
		if (delta)
		{
			Origin.x += frame->data.x;
			Origin.y += frame->data.y;
		}
		else
		{
			Origin.x = frame->data.x;
			Origin.y = frame->data.y;
		}
	}
	else
	if (element_name == CIwGameXomlNames::Velocity_Hash)
	{
		CIwGameAnimFrameVec2* frame = (CIwGameAnimFrameVec2*)animation->getCurrentData();
		if (delta)
			setVelocity(Velocity.x + frame->data.x, Velocity.y + frame->data.y);
		else
			setVelocity(frame->data.x, frame->data.y);
	}
	else
	if (element_name == CIwGameXomlNames::Angle_Hash)
	{
		CIwGameAnimFrameFloat* frame = (CIwGameAnimFrameFloat*)animation->getCurrentData();
		if (delta)
			setAngle(OriginalAngle + frame->data);
		else
			setAngle(frame->data);
	}
	else
	if (element_name == CIwGameXomlNames::AngularVelocity_Hash)
	{
		CIwGameAnimFrameFloat* frame = (CIwGameAnimFrameFloat*)animation->getCurrentData();
		if (delta)
			setAngularVelocity(AngularVelocity + frame->data);
		else
			setAngularVelocity(frame->data);
	}
	else
	if (element_name == CIwGameXomlNames::Scale_Hash)
	{
		CIwGameAnimFrameVec2* frame = (CIwGameAnimFrameVec2*)animation->getCurrentData();
		if (delta)
		{
			ScaleX = OriginalScale.x + frame->data.x;
			ScaleY = OriginalScale.y + frame->data.y;
		}
		else
		{
			ScaleX = frame->data.x;
			ScaleY = frame->data.y;
		}
	}
	else
	if (element_name == CIwGameXomlNames::ScaleX_Hash)
	{
		CIwGameAnimFrameFloat* frame = (CIwGameAnimFrameFloat*)animation->getCurrentData();
		if (delta)
			ScaleX = OriginalScale.x + frame->data;
		else
			ScaleX = frame->data;
	}
	else
	if (element_name == CIwGameXomlNames::ScaleY_Hash)
	{
		CIwGameAnimFrameFloat* frame = (CIwGameAnimFrameFloat*)animation->getCurrentData();
		if (delta)
			ScaleY = OriginalScale.y + frame->data;
		else
			ScaleY = frame->data;
	}
	else
	if (element_name == CIwGameXomlNames::Colour_Hash || element_name == CIwGameXomlNames::Color_Hash)
	{
		CIwGameAnimFrameVec4* frame = (CIwGameAnimFrameVec4*)animation->getCurrentData();
		if (delta)
		{
			Colour.r = OriginalColour.r + (uint8)frame->data.x;
			Colour.g = OriginalColour.g + (uint8)frame->data.y;
			Colour.b = OriginalColour.b + (uint8)frame->data.z;
			Colour.a = OriginalColour.a + (uint8)frame->data.w;
		}
		else
		{
			Colour.r = (uint8)frame->data.x;
			Colour.g = (uint8)frame->data.y;
			Colour.b = (uint8)frame->data.z;
			Colour.a = (uint8)frame->data.w;
		}
	}
	else
	if (element_name == CIwGameXomlNames::Opacity_Hash)
	{
		CIwGameAnimFrameFloat* frame = (CIwGameAnimFrameFloat*)animation->getCurrentData();
		if (delta)
			Colour.a = OriginalColour.a + (uint8)frame->data;
		else
			Colour.a = (uint8)frame->data;
	}
	else
	if (element_name == CIwGameXomlNames::Layer_Hash)
	{
		CIwGameAnimFrameFloat* frame = (CIwGameAnimFrameFloat*)animation->getCurrentData();
		Layer = (int)frame->data;
	}
	else
	if (element_name == CIwGameXomlNames::Visible_Hash)
	{
		CIwGameAnimFrameBool* frame = (CIwGameAnimFrameBool*)animation->getCurrentData();
		IsVisible = frame->data;
	}
	else
	if (element_name == CIwGameXomlNames::HitTest_Hash)
	{
		CIwGameAnimFrameBool* frame = (CIwGameAnimFrameBool*)animation->getCurrentData();
		IsTappable = frame->data;
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
	}
	else
	if (element_name == CIwGameXomlNames::TimeScale_Hash)
	{
		if (Timeline != NULL)
		{
			CIwGameAnimFrameFloat* frame = (CIwGameAnimFrameFloat*)animation->getCurrentData();
			if (delta)
				Timeline->setTimeScale(Timeline->getTimeScale() + frame->data);
			else
				Timeline->setTimeScale(frame->data);
		}
	}
	else
		return false;

	return true;
}

bool CIwGameActor::UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var)
{
	if (!var->Modified || var->getValue().IsEmpty())
		return false;

	if (element_name == CIwGameXomlNames::Position_Hash)
	{
		if (var->Type == VT_Vec2)
		{
			CIwFVec2* data = &((CIwGameXomlVariableVec2*)var)->NativeValue;
			setPosition(data->x, data->y);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected vec2 for Position");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::PositionX_Hash)
	{
		if (var->Type == VT_Float)
			setPosition(((CIwGameXomlVariableFloat*)var)->NativeValue, Position.y);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected float for PositionX");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::PositionY_Hash)
	{
		if (var->Type == VT_Float)
			setPosition(Position.x, ((CIwGameXomlVariableFloat*)var)->NativeValue);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected float for PositionY");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::Origin_Hash)
	{
		if (var->Type == VT_Vec2)
		{
			CIwFVec2* data = &((CIwGameXomlVariableVec2*)var)->NativeValue;
			setOrigin(data->x, data->y);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected vec2 for Origin");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::Angle_Hash)
	{
		if (var->Type == VT_Float)
			setAngle(((CIwGameXomlVariableFloat*)var)->NativeValue);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected float for Angle");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::Scale_Hash)
	{
		if (var->Type == VT_Float)
			setScale(((CIwGameXomlVariableFloat*)var)->NativeValue);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected float for Scale");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::ScaleX_Hash)
	{
		if (var->Type == VT_Float)
			setScaleX(((CIwGameXomlVariableFloat*)var)->NativeValue);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected float for ScaleX");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::ScaleY_Hash)
	{
		if (var->Type == VT_Float)
			setScaleY(((CIwGameXomlVariableFloat*)var)->NativeValue);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected float for ScaleY");
#endif // _DEBUG
	}
	else
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
	if (element_name == CIwGameXomlNames::Velocity_Hash)
	{
		if (var->Type == VT_Vec2)
		{
			CIwFVec2* data = &((CIwGameXomlVariableVec2*)var)->NativeValue;
			setVelocity(data->x, data->y);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected vec2 for Velocity");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::AngularVelocity_Hash)
	{
		if (var->Type == VT_Float)
			setAngularVelocity(((CIwGameXomlVariableFloat*)var)->NativeValue);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected float for AngularVelocity");
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
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected string for timeline");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::Binding_Hash)
	{
		if (var->Type == VT_String)
		{
			if (!var->getValue().IsEmpty())
			{
				CIwGameXomlBindings* bindings = (CIwGameXomlBindings*)Scene->getResourceManager()->findResource(var->getValue().c_str(), CIwGameXomlNames::Bindings_Hash);
				setBindings(bindings);
			}
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected string for Binding");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::Binding_Hash)
	{
		if (var->Type == VT_String)
		{
			if (!var->getValue().IsEmpty())
			{
				CIwGameString var_name, prop_name;
				int index = -1; 
				if (var->getValue().SplitPropVarIndex(prop_name, var_name, index))
				{
					CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(var_name, Scene);
					if (var == NULL)
						CIwGameError::LogError("Warning: UpdateBinding - Bound simple binding variable does not exist - ", var_name.c_str());
					else
					{
						SAFE_DELETE(SimpleBinding)

						SimpleBinding = new CIwGameXomlBinding();
						SimpleBinding->Index = index;
#if defined(_DEBUG)
						SimpleBinding->_Property = prop_name;
#endif // _DEBUG
						SimpleBinding->Property = prop_name.getHash();
						SimpleBinding->Variable = var;
					}
				}
				else
					CIwGameError::LogError("Warning: UpdateBinding - Invalid format of simple binding, format should be [property]variable:array_index - ", var->getValue().c_str());
			}
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: UpdateBinding - Incorrect simple binding variable type, expected string for simple binding");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::Depth_Hash)
	{
		if (var->Type == VT_Float)
			Depth = ((CIwGameXomlVariableFloat*)var)->NativeValue;
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected float for Depth");
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
	if (element_name == CIwGameXomlNames::Collidable_Hash)
	{
		if (var->Type == VT_Bool)
			setCollidable(((CIwGameXomlVariableBool*)var)->NativeValue);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected bool for Collidable");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::Draggable_Hash)
	{
		if (var->Type == VT_Bool)
			setDraggable(((CIwGameXomlVariableBool*)var)->NativeValue);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected bool for Draggable");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::HitTest_Hash)
	{
		if (var->Type == VT_Bool)
			setTappable(((CIwGameXomlVariableBool*)var)->NativeValue);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected bool for HitTest");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::TimeScale_Hash)
	{
		if (var->Type == VT_Float)
		{
			if (Timeline != NULL)
				Timeline->setTimeScale(((CIwGameXomlVariableFloat*)var)->NativeValue);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected float for TimeScale");
#endif // _DEBUG
	}
	else
		return false;

	return true;
}

void CIwGameActor::UpdateBindings(bool force_modified)
{
	// Update complex bindings
	if (Bindings != NULL)
	{
		for (CIwGameXomlBindings::_Iterator it = Bindings->begin(); it != Bindings->end(); ++it)
		{
			unsigned int element_name = (*it)->Property;
			CIwGameXomlVariable* var = (*it)->Variable;

			if (var != NULL)
			{
				if (var->isArray() && (*it)->Index >= 0)
					var = ((CIwGameXomlVariableArray*)var)->getElement((*it)->Index);
			}

			if (var != NULL)
			{
				if (force_modified)
					var->Modified = true;
				UpdateBinding(element_name, var);
			}
		}
	}

	// Update simple binding
	if (SimpleBinding != NULL)
	{
		CIwGameXomlVariable* var = SimpleBinding->Variable;

		if (var != NULL)
		{
			if (var->isArray() && SimpleBinding->Index >= 0)
				var = ((CIwGameXomlVariableArray*)var)->getElement(SimpleBinding->Index);
		}

		if (var != NULL)
		{
			if (force_modified)
				var->Modified = true;
			UpdateBinding(SimpleBinding->Property, var);
		}
	}
}

bool CIwGameActor::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - Actor needs to be declared inside a scene or an actor");
		return false;
	}

	// Add the actor to the scene
	if (parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
	{
		Scene = ((CIwGameActor*)parent)->getScene();
	}
	else
		Scene = (CIwGameScene*)parent;

	Scene->addActor(this);

	// Check and apply style
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();
		if (name_hash == CIwGameXomlNames::Style_Hash)
		{
			if (!(*it)->GetValue().IsEmpty())
			{
				// Find the style
				CIwGameStyle* style = (CIwGameStyle*)Scene->getResourceManager()->findResource((*it)->GetValue().c_str(), CIwGameXomlNames::Style_Hash);
				if (style == NULL)
				{
					CIwGameError::LogError("Error: XOML - Could not find named style in the scene or in the global resources - ", (*it)->GetValue().c_str());
					return false;
				}
				// Apply the style
				CIwGameXoml::ApplyStyle(style, node);
			}
		}
	}

	// Process Actor attributes
	CIwGameString*	box2dmaterial_name = NULL, *shape_name = NULL;
	CIwGameString*	bindings_name = NULL, *simple_binding_name = NULL;
	CIwFVec2		com(0, 0);
	CIwFVec3		collision_flags(0, 0, 0);
	bool			sensor = false;
	bool			vd_set = false;
	bool			avd_set = false;
	bool			is_active = true;
	bool			depth_set = false;
	float			ang_vel = 0;
	CIwFVec2		vel  = CIwFVec2(0.0f, 0.0f);
	eIwGameScene_Docking docking = DK_None;
	bool			docking_set = false;
	CIwGameString*	linked_to = NULL;
	CIwGameString*	target = NULL;
	CIwGameString*	condition = NULL;

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
		{
			setName((*it)->GetValue().c_str());
		}
		else
		if (name_hash == CIwGameXomlNames::Type_Hash)
			setType((*it)->GetValueAsInt());
		else
		if (name_hash == CIwGameXomlNames::Position_Hash)
		{
			if (!(*it)->GetValueAsPoint(Position))
				CIwGameError::LogError("Warning: XOML - Invalid value for Actor::Position, expected vec2");
			setOriginalPosition(Position.x, Position.y);
		}
		else
		if (name_hash == CIwGameXomlNames::Origin_Hash)
		{
			if (!(*it)->GetValueAsPoint(Origin))
				CIwGameError::LogError("Warning: XOML - Invalid value for Actor::Origin, expected vec2");
		}
		else
		if (name_hash == CIwGameXomlNames::Velocity_Hash)
		{
			if (!(*it)->GetValueAsPoint(vel))
				CIwGameError::LogError("Warning: XOML - Invalid value for Actor::Velocity, expected vec2");
		}
		else
		if (name_hash == CIwGameXomlNames::VelocityDamping_Hash)
		{
			if (!(*it)->GetValueAsPoint(VelocityDamping))
				CIwGameError::LogError("Warning: XOML - Invalid value for Actor::VelocityDamping, expected vec2");
			else
				vd_set = true;
		}
		else
		if (name_hash == CIwGameXomlNames::Angle_Hash)
		{
			Angle = (*it)->GetValueAsFloat();
			OriginalAngle = Angle;
		}
		else
		if (name_hash == CIwGameXomlNames::AngularVelocity_Hash)
			ang_vel = (*it)->GetValueAsFloat();
		else
		if (name_hash == CIwGameXomlNames::AngularVelocityDamping_Hash)
		{
			AngularVelocityDamping = (*it)->GetValueAsFloat();
			avd_set = true;
		}
		else
		if (name_hash == CIwGameXomlNames::Scale_Hash)
		{
			ScaleX = ScaleY = (*it)->GetValueAsFloat();
			setOriginalScale(ScaleX, ScaleY);
		}
		else
		if (name_hash == CIwGameXomlNames::ScaleX_Hash)
		{
			ScaleX = (*it)->GetValueAsFloat();
			setOriginalScale(ScaleX, 1.0f);
		}
		else
		if (name_hash == CIwGameXomlNames::ScaleY_Hash)
		{
			ScaleY = (*it)->GetValueAsFloat();
			setOriginalScale(1.0f, ScaleY);
		}
		else
		if (name_hash == CIwGameXomlNames::Colour_Hash || name_hash == CIwGameXomlNames::Color_Hash)
		{
			if (!(*it)->GetValueAsColour(Colour))
				CIwGameError::LogError("Warning: XOML - Invalid value for Actor::Colour, expected colour");
			OriginalColour = Colour;
		}
		else
		if (name_hash == CIwGameXomlNames::Opacity_Hash)
		{
			Colour.a = (*it)->GetValueAsInt();
			OriginalColour.a = Colour.a;
		}
		else
		if (name_hash == CIwGameXomlNames::Layer_Hash)
			Layer = (*it)->GetValueAsInt();
		else
		if (name_hash == CIwGameXomlNames::Active_Hash)
			is_active = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::Visible_Hash)
			IsVisible = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::Docking_Hash)
		{
			docking_set = true;
			unsigned int hash = (*it)->GetValue().getHash();
			if (hash == CIwGameString::CalculateHash("top"))
				docking = DK_Top;
			else
			if (hash == CIwGameString::CalculateHash("left"))
				docking = DK_Left;
			else
			if (hash == CIwGameString::CalculateHash("right"))
				docking = DK_Right;
			else
			if (hash == CIwGameString::CalculateHash("bottom"))
				docking = DK_Bottom;
			else
			if (hash == CIwGameString::CalculateHash("topleft"))
				docking = DK_TopLeft;
			else
			if (hash == CIwGameString::CalculateHash("topright"))
				docking = DK_TopRight;
			else
			if (hash == CIwGameString::CalculateHash("bottomleft"))
				docking = DK_BottomLeft;
			else
			if (hash == CIwGameString::CalculateHash("bottomright"))
				docking = DK_BottomRight;
			else
				docking = DK_None;
		}
		else
		if (name_hash == CIwGameXomlNames::Margin_Hash)
		{
			if (!(*it)->GetValueAsRect(Margin))
				CIwGameError::LogError("Warning: XOML - Invalid value for Actor::Margin, expected rect");
		}
		else
		if (name_hash == CIwGameXomlNames::Collidable_Hash)
			IsCollidable = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::Draggable_Hash)
			IsDraggable = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::HitTest_Hash)
			IsTappable = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::CollisionSize_Hash)
			setCollisionSize((*it)->GetValueAsInt());
		else
		if (name_hash == CIwGameXomlNames::CollisionRect_Hash)
		{
			CIwRect rect;
			if (!(*it)->GetValueAsRect(rect))
				CIwGameError::LogError("Warning: XOML - Invalid value for Actor::CollisionRect, expected rect");
			setCollisionRect(rect);
		}
		else
		if (name_hash == CIwGameXomlNames::WrapPosition_Hash)
			WrapPosition = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::Binding_Hash)
			simple_binding_name = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Bindings_Hash)
			bindings_name = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Depth_Hash)
		{
			Depth = (*it)->GetValueAsFloat();
			depth_set = true;
		}
		else
		if (name_hash == CIwGameXomlNames::OnTapped_Hash)
			EventsManager->addEvent("OnTapped", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnTapped2_Hash)
			EventsManager->addEvent("OnTapped2", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnTapped3_Hash)
			EventsManager->addEvent("OnTapped3", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnTapped4_Hash)
			EventsManager->addEvent("OnTapped4", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnTapped5_Hash)
			EventsManager->addEvent("OnTapped5", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnBeginTouch_Hash)
			EventsManager->addEvent("OnBeginTouch", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnBeginTouch2_Hash)
			EventsManager->addEvent("OnBeginTouch2", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnBeginTouch3_Hash)
			EventsManager->addEvent("OnBeginTouch3", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnBeginTouch4_Hash)
			EventsManager->addEvent("OnBeginTouch4", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnBeginTouch5_Hash)
			EventsManager->addEvent("OnBeginTouch5", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnEndTouch_Hash)
			EventsManager->addEvent("OnEndTouch", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnEndTouch2_Hash)
			EventsManager->addEvent("OnEndTouch2", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnEndTouch3_Hash)
			EventsManager->addEvent("OnEndTouch3", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnEndTouch4_Hash)
			EventsManager->addEvent("OnEndTouch4", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnEndTouch5_Hash)
			EventsManager->addEvent("OnEndTouch5", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnCreate_Hash)
			EventsManager->addEvent("OnCreate", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnDestroy_Hash)
			EventsManager->addEvent("OnDestroy", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnOrientationChange_Hash)
			EventsManager->addEvent("OnOrientationChange", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnCollisionStart_Hash)
			EventsManager->addEvent("OnCollisionStart", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnCollisionEnd_Hash)
			EventsManager->addEvent("OnCollisionEnd", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::LinkedTo_Hash)
			linked_to = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::GridPos_Hash)
		{
			CIwFVec2 pos = CIwFVec2(0, 0);
			if (!(*it)->GetValueAsPoint(pos))
				CIwGameError::LogError("Warning: XOML - Invalid value for Actor::GridPos, expected vec2");
			else
				GridPos = ((int)pos.x << 16) | (int)pos.y;
		}
		else
		if (name_hash == CIwGameXomlNames::Target_Hash)
			target = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::UseParentOpacity_Hash)
			UseParentOpacity = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::Bubbling_Hash)
			ReceiveEventFromChild = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::Condition_Hash)
			condition = &(*it)->GetValue();
#if defined(IW_GAME_BOX2D)
		else
		if (name_hash == CIwGameXomlNames::Box2dMaterial_Hash)
			box2dmaterial_name = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Shape_Hash)
			shape_name = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::COM_Hash)
		{
			if (!(*it)->GetValueAsPoint(com))
				CIwGameError::LogError("Warning: XOML - Invalid value for Actor::COM, expected vec2");
		}
		else
		if (name_hash == CIwGameXomlNames::Sensor_Hash)
			sensor = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::CollisionFlags_Hash)
		{
			if (!(*it)->GetValueAsPoint3(collision_flags))
				CIwGameError::LogError("Warning: XOML - Invalid value for Actor::CollisionFlags, expected vec3");
		}
#endif // IW_GAME_BOX2D
	}

	if (condition != NULL)
	{
		// Find the condition variable
		bool condition_not = false;
		CIwGameXomlVariable* var = NULL;
		if (*(condition->c_str()) == '!')
		{
			condition_not = true;
			CIwGameString cond = condition->c_str() + 1;
			var = CIwGameXomlVariable::GetVariable(cond, Scene);
		}
		else
			var = CIwGameXomlVariable::GetVariable(*condition, Scene);
		if (var != NULL)
		{
			bool res = var->isTrue();
			if (condition_not)
				res = !res;
			if (!res)
			{
				Scene->removeActor(this, true);
				IW_GAME_XOML->setExitOnError(false);	// We prevent the XOML system from exiting with error as this is not an error but we need derived actors to believe that it is so they do not process children
				return false;
			}
		}
#if defined (_DEBUG)
		else
			CIwGameError::LogError("Warning: condition variable not found - ", condition->c_str());
#endif // _DEBUG
	}

	// Set up LinkedTo actor
	if (linked_to != NULL && !linked_to->IsEmpty())
	{
		CIwGameActor* actor = Scene->findActor(linked_to->getHash());
		if (actor != NULL)
			setLinkedTo(actor);
		else
			CIwGameError::LogError("Warning: XOML - Linked actor not found - ", linked_to->c_str());
	}
	else
	if (parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		setLinkedTo((CIwGameActor*)parent);	// Because we were defined inside another actor tag

	// If an actor is linked to another and no depth value has been set then it should be set to 0 as depth values for linked actors are accumulative
	if (LinkedTo != NULL && !depth_set)
		Depth = 0;

	// Search the scenes resources for the specified bindings manager
	if (bindings_name != NULL && !bindings_name->IsEmpty())
	{
		CIwGameXomlBindings* bindings = (CIwGameXomlBindings*)Scene->getResourceManager()->findResource(bindings_name->c_str(), CIwGameXomlNames::Bindings_Hash);
		if (bindings == NULL)
			CIwGameError::LogError("Error: XOML - Could not find named bindings in the scene or in the global resources - ", bindings_name->c_str());
		setBindings(bindings);
	}

	// Set up simple binding
	if (simple_binding_name != NULL && !simple_binding_name->IsEmpty())
	{
		CIwGameString var_name, prop_name;
		int index = -1;
		if (simple_binding_name->SplitPropVarIndex(prop_name, var_name, index))
		{
			CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(var_name, Scene);
			if (var == NULL)
				CIwGameError::LogError("Warning: XOML - Bound simple binding variable does not exist - ", var_name.c_str());
			else
			{
				SimpleBinding = new CIwGameXomlBinding();
				SimpleBinding->Index = index;
#if defined(_DEBUG)
				SimpleBinding->_Property = prop_name;
#endif // _DEBUG
				SimpleBinding->Property = prop_name.getHash();
				SimpleBinding->Variable = var;
			}
		}
		else
			CIwGameError::LogError("Warning: XOML - Invalid format of simple binding, format should be [property]variable:array_index - ", simple_binding_name->c_str());
	}

#if defined(IW_GAME_BOX2D)
	// Check for shape and set if present
	CIwGameShape* shape = NULL;
	if (shape_name != NULL && !shape_name->IsEmpty())
	{
		shape = (CIwGameShape*)Scene->getResourceManager()->findResource(shape_name->c_str(), CIwGameXomlNames::Shape_Hash);
		if (shape == NULL)
		{
			CIwGameError::LogError("Error: XOML - Could not find named shape in the scene or in the global resources - ", shape_name->c_str());
		}
	}

	// Check for box2d material and set if present
	if (box2dmaterial_name != NULL && !box2dmaterial_name->IsEmpty())
	{
		CIwGameBox2dMaterial* material = (CIwGameBox2dMaterial*)Scene->getResourceManager()->findResource(box2dmaterial_name->c_str(), CIwGameXomlNames::Box2dMaterial_Hash);
		if (material == NULL)
		{
			CIwGameError::LogError("Error: XOML - Could not find named material in the scene or in the global resources - ", box2dmaterial_name->c_str());
		}

		if (shape == NULL)
		{
			CIwGameError::LogError("Warning: XOML - Cannot attach Box2d physics to an actor that has no shape");
		}
		else
		{
			Angle = -Angle;
			Box2dBody = new CIwGameBox2dBody();
			if (shape != NULL)
				Box2dBody->setBodyShape(shape);
			Box2dBody->InitBody(Scene->getBox2dWorld(), shape, material, &Position, Angle, com.x, com.y);
			if (!vd_set)
			{
				VelocityDamping.x = 0;
				VelocityDamping.y = 0;
			}
			if (!avd_set)
				AngularVelocityDamping = 0;
			setVelocityDamping(VelocityDamping.x, VelocityDamping.y);
			setAngularVelocityDamping(AngularVelocityDamping);
			Box2dBody->setAsSensor(sensor);
			Box2dBody->setCollisionFlags((int)collision_flags.x, (int)collision_flags.y, (int)collision_flags.z);
			Box2dBody->setUserData(this);
		}
	}
	setVelocity(vel.x, vel.y);
	setAngularVelocity(ang_vel);

#endif // IW_GAME_BOX2D
	setActive(is_active);

	// Set dockign mode
	if (docking_set)
		setDocking(docking);

	// Load children
	if (load_children)
	{
		// Process inner tags
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}

	// Find target
	if (target != NULL && !target->IsEmpty())
	{
		Target = Scene->findActor(target->getHash());
		if (Target == NULL)
			CIwGameError::LogError("Warning: XOML - Could not find target actor - ", target->c_str());
	}

	return true;
}

bool CIwGameActor::PostLoadFromXoml(IIwGameXomlResource* parent, CIwGameXmlNode* node)
{
	CIwGameString*	timeline_name = NULL;

	// We process timelines after the actor an its children hav been loaded as they may be children of the actor
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Timeline_Hash)
			timeline_name = &(*it)->GetValue();

	}

	// Search the timeline resources for the specified timeline
	if (timeline_name != NULL && !timeline_name->IsEmpty())
	{
		CIwGameAnimTimeline* timeline = CIwGameAnimTimelinesManager::FindTimeline(timeline_name->getHash(), this);
		if (timeline == NULL)
			CIwGameError::LogError("Warning: XOML - Could not find named timeline in the scene or in the global resources - ", timeline_name->c_str());
		else
		{
			setTimeline(timeline);
			timeline->setTargetElement(this);
		}
	}

	// Force update any data bindings
	UpdateBindings(true);

	return true;
}

void CIwGameActor::CalculateSizes(int& width, int& height)
{
	// Calculate size on parent ot screen if size not specified
	if (LinkedTo != NULL)
	{
		// If negative size has been specified and part of a linked actor then base size on a percentage of parent
		if (width < 0)
			width = (LinkedTo->getSize().x * -width) / 100;
		if (height < 0)
			height = (LinkedTo->getSize().y * -height) / 100;;
	}
	else
	{
		// If negative size has been specified then base size on a percentage of virtual canvas
		if (width < 0)
			width = ((Scene->getScreenSize().x * IW_GEOM_ONE) / Scene->getVirtualTransform().m[0][0] * -width) / 100;
		if (height < 0)
			height = ((Scene->getScreenSize().y * IW_GEOM_ONE) / Scene->getVirtualTransform().m[1][1] * -height) / 100;
	}
}

void CIwGameActor::CalculateMargins(CIwRect& margin)
{
	// Calculate size on parent ot screen if size not specified
	if (LinkedTo != NULL)
	{
		// If negative margin has been specified and part of a linked actor then base margin on a percentage of parent
		if (margin.x < 0)
			Margin.x = (LinkedTo->getSize().x * -margin.x) / 100;
		if (margin.y < 0)
			Margin.y = (LinkedTo->getSize().x * -margin.y) / 100;
		if (margin.w < 0)
			Margin.w = (LinkedTo->getSize().y * -margin.w) / 100;
		if (margin.h < 0)
			Margin.h = (LinkedTo->getSize().y * -margin.h) / 100;
	}
	else
	{
		// If negative nargin has been specified then base margin on a percentage of virtual canvas
		if (margin.x < 0)
			Margin.x = ((Scene->getScreenSize().x * IW_GEOM_ONE) / Scene->getVirtualTransform().m[0][0] * -margin.x) / 100;
		if (margin.y < 0)
			Margin.y = ((Scene->getScreenSize().x * IW_GEOM_ONE) / Scene->getVirtualTransform().m[0][0] * -margin.y) / 100;
		if (margin.w < 0)
			Margin.w = ((Scene->getScreenSize().y * IW_GEOM_ONE) / Scene->getVirtualTransform().m[1][1] * -margin.w) / 100;
		if (margin.h < 0)
			Margin.h = ((Scene->getScreenSize().y * IW_GEOM_ONE) / Scene->getVirtualTransform().m[1][1] * -margin.h) / 100;
	}
}

void CIwGameActor::UpdateMargins()
{
	if (LinkedTo != NULL)
	{
		if (OriginalMargin.x < 0)
			Margin.x = (LinkedTo->getSize().x * -OriginalMargin.x) / 100;
		if (OriginalMargin.y < 0)
			Margin.y = (LinkedTo->getSize().x * -OriginalMargin.y) / 100;
		if (OriginalMargin.w < 0)
			Margin.w = (LinkedTo->getSize().y * -OriginalMargin.w) / 100;
		if (OriginalMargin.h < 0)
			Margin.h = (LinkedTo->getSize().y * -OriginalMargin.h) / 100;
	}
	else
	{
		if (OriginalMargin.x < 0)
			Margin.x = ((Scene->getScreenSize().x * IW_GEOM_ONE) / Scene->getVirtualTransform().m[0][0] * -OriginalMargin.x) / 100;
		if (OriginalMargin.y < 0)
			Margin.y = ((Scene->getScreenSize().x * IW_GEOM_ONE) / Scene->getVirtualTransform().m[0][0] * -OriginalMargin.y) / 100;
		if (OriginalMargin.w < 0)
			Margin.w = ((Scene->getScreenSize().y * IW_GEOM_ONE) / Scene->getVirtualTransform().m[1][1] * -OriginalMargin.w) / 100;
		if (OriginalMargin.h < 0)
			Margin.h = ((Scene->getScreenSize().y * IW_GEOM_ONE) / Scene->getVirtualTransform().m[1][1] * -OriginalMargin.h) / 100;
	}
}

//
//
//
//
//
// CIwGameActorImage implementation
//
//
//
//
//
bool CIwGameActorImage::InitWithImage(CIwGameImage* image, int width, int height)
{
	// Create sprite
	CIwGameBitmapSprite* sprite = new CIwGameBitmapSprite();
	if (sprite == NULL)
		return false;
	sprite->Init();

	// Set sprite as visual
	Visual = sprite;

	OriginalSize.x = width;
	OriginalSize.y = height;
	OriginalMargin = Margin;
	CalculateSizes(width, height);
	CalculateMargins(Margin);

	// Add sprite to the sprite manager so it can be managed and drawn
	Scene->getSpriteManager()->addSprite(sprite);

	// Set-up LinkedTo
	if (LinkedTo != NULL)
		Visual->setLinkedTo(LinkedTo->getVisual());

	// Set sprite image
	sprite->setImage(image);
	SetSrcRect(0, 0, width, height);
	SetDestSize(width, height);

	return true;
}

bool CIwGameActorImage::InitWithBrush(IIwGameBrush* brush, int width, int height)
{
	// Calculate size on parent ot screen if size not specified
	OriginalSize.x = width;
	OriginalSize.y = height;
	OriginalMargin = Margin;
	CalculateSizes(width, height);
	CalculateMargins(Margin);

	if (brush->getBrushType() == IIwGameBrush::BT_Image)
	{
		// Create sprite
		CIwGameBitmapSprite* sprite = new CIwGameBitmapSprite();
		if (sprite == NULL)
			return false;
		sprite->Init();
			
		// Set sprite image
		sprite->setFromBrush((CIwGameBrushImage*)brush);

		// Set sprite as visual
		Visual = sprite;

		// Set sprite size
		SetDestSize(width, height);

		// Add sprite to the sprite manager so it can be managed and drawn
		Scene->getSpriteManager()->addSprite(sprite);

		// Set-up LinkedTo
		if (LinkedTo != NULL)
			Visual->setLinkedTo(LinkedTo->getVisual());

	}
	else
	if (brush->getBrushType() == IIwGameBrush::BT_9Patch)
	{
		// Create sprite
		CIwGameBitmapSprite9* sprite = new CIwGameBitmapSprite9();
		if (sprite == NULL)
			return false;
		sprite->Init();
			
		// Set UV's from brush
		sprite->setFromBrush((CIwGameBrushImage9*)brush);

		// Set sprite as visual
		Visual = sprite;

		// Set sprite size
		SetDestSize(width, height);

		// Add sprite to the sprite manager so it can be managed and drawn
		Scene->getSpriteManager()->addSprite(sprite);

		// Set-up LinkedTo
		if (LinkedTo != NULL)
			Visual->setLinkedTo(LinkedTo->getVisual());
	}
	else
	{
		// Set destination size
		SetDestSize(width, height);
	}

	return true;
}

bool CIwGameActorImage::Init(IIwGameBrush* brush, int width, int height)
{
	CIwGameActor::Init();

	// Create sprite
	if (!InitWithBrush(brush, width, height))
		return false;

	return true;
}

bool CIwGameActorImage::Init(CIwGameImage* image, int width, int height)
{
	CIwGameActor::Init();

	// Create sprite
	if (!InitWithImage(image, width, height))
		return false;

	return true;
}

bool CIwGameActorImage::Init(CIwGameImage* image, CIwRect* src_rect)
{
	CIwGameActor::Init();

	// Create sprite
	if (!InitWithImage(image, src_rect->w, src_rect->h))
		return false;
	SetSrcRect(src_rect->x, src_rect->y, src_rect->w, src_rect->h);

	return true;
}

bool CIwGameActorImage::Init(CIwGameImage* image, int x, int y, int w, int h)
{
	CIwGameActor::Init();

	// Create sprite
	if (!InitWithImage(image, w, h))
		return false;
	SetSrcRect(x, y, w, h);

	return true;
}


void CIwGameActorImage::SetSrcDest(int x, int y, int w, int h)
{
	SetSrcRect(x, y, w, h);
	SetDestSize(w, h);
}

void CIwGameActorImage::SetSrcRect(int x, int y, int w, int h)
{
	if (Visual != NULL)
		((CIwGameBitmapSprite*)Visual)->setSrcRect(x, y, w, h);
}

void CIwGameActorImage::SetDestSize(int w, int h)
{
	// If apsect is locked then adjust none locked axis to fit parent or scene container aspect ratio
	if (AspectLock == AL_X)
	{
		CIwVec2 size;
		size.x = ((CIwGameBitmapSprite*)Visual)->getSrcWidth();
		size.y = ((CIwGameBitmapSprite*)Visual)->getSrcHeight();
		size = Scene->getScreenSize();
		h = (size.x * w) / size.y;
	}
	else
	if (AspectLock == AL_Y)
	{
		CIwVec2 size;
		size.x = ((CIwGameBitmapSprite*)Visual)->getSrcWidth();
		size.y = ((CIwGameBitmapSprite*)Visual)->getSrcHeight();
		w = (size.y * h) / size.x;
	}
	Size.x = w;
	Size.y = h;
	if (Visual != NULL)
		((CIwGameBitmapSprite*)Visual)->setDestSize(w, h);
	if (Docking != DK_None)
		setDocking(Docking);
}

void CIwGameActorImage::SetImage(CIwGameImage* image)
{
	if (Visual == NULL)
	{
		CIwGameBitmapSprite* sprite = new CIwGameBitmapSprite();
		sprite->Init();

		// Set sprite image
		sprite->setImage(image);

		// Set sprite as visual
		Visual = sprite;

		// Add sprite to the sprite manager so it can be managed and drawn
		Scene->getSpriteManager()->addSprite(sprite);
	}
	else
	{
		CIwGameBitmapSprite* sprite = (CIwGameBitmapSprite*)Visual;

		// Set sprite image
		sprite->setImage(image);
	}
}

void CIwGameActorImage::SetFromBrush(IIwGameBrush* brush)
{
	if (brush->getBrushType() == IIwGameBrush::BT_Image || brush->getBrushType() == IIwGameBrush::BT_9Patch)
	{
		CIwGameBrushImage* b = (CIwGameBrushImage*)brush;
		SetImage(b->getImage());
		CIwRect rc = b->getSrcRect();
		SetSrcRect(rc.x, rc.y, rc.w, rc.h);
		if (OriginalSize.x == 0)
			Size.x = rc.w;
		if (OriginalSize.y == 0)
			Size.y = rc.h;
		SetDestSize(Size.x, Size.y);
	}
}


bool CIwGameActorImage::UpdateVisual()
{
	if (!CIwGameActor::UpdateVisual())
		return false;

	return true;
}

bool CIwGameActorImage::setProperty(unsigned int element_name, CIwGameString& data, bool delta)
{
	if (CIwGameActor::setProperty(element_name, data, delta))
		return true;

	int		int_pool[8];
	float	float_pool[8];

	if (element_name == CIwGameXomlNames::Size_Hash)
	{
		data.GetAsListOfInt(int_pool);
		if (delta)
		{
			int width = Visual->getDestWidth();
			int height = Visual->getDestHeight();
			width += int_pool[0];
			height += int_pool[1];
			SetDestSize(width, height);
		}
		else
			SetDestSize(int_pool[0], int_pool[1]);
	}
	else
	if (element_name == CIwGameXomlNames::SrcRect_Hash)
	{
		data.GetAsListOfInt(int_pool);
		if (delta)
		{
			CIwRect rect = ((CIwGameBitmapSprite*)Visual)->getSrcRect();
			rect.x += int_pool[0];
			rect.y += int_pool[1];
			rect.w += int_pool[2];
			rect.h += int_pool[3];
			((CIwGameBitmapSprite*)Visual)->setSrcRect(&rect);
		}
		else
			((CIwGameBitmapSprite*)Visual)->setSrcRect(int_pool[0], int_pool[1], int_pool[2], int_pool[3]);
	}
	else
	if (element_name == CIwGameXomlNames::Image_Hash)
	{
		if (data.IsEmpty())
			SetImage(NULL);
		else
		{
			CIwGameImage* image = (CIwGameImage*)Scene->getResourceManager()->findResource(data.getHash(), CIwGameXomlNames::Image_Hash);
			if (image != NULL)
				SetImage(image);
			else
				CIwGameError::LogError("Warning: setProperty image not found in resources - ", data.c_str());
		}
	}
	else
	if (element_name == CIwGameXomlNames::Brush_Hash)
	{
		if (data.IsEmpty())
			SetImage(NULL);
		else
		{
			CIwGameBrushImage* brush = (CIwGameBrushImage*)Scene->getResourceManager()->findResource(data.getHash(), CIwGameXomlNames::Brush_Hash);
			if (brush != NULL)
				SetFromBrush(brush);
			else
				CIwGameError::LogError("Warning: setProperty brush not found in resources - ", data.c_str());
		}
	}
	else
	if (element_name == CIwGameXomlNames::FlipX_Hash)
	{
		if (Visual != NULL)
		{
			int flip_y = ((CIwGameBitmapSprite*)Visual)->getImageTransform() & IW_2D_IMAGE_TRANSFORM_FLIP_Y;
			if (data.GetAsBool())
				((CIwGameBitmapSprite*)Visual)->setImageTransform((CIw2DImageTransform)(flip_y | IW_2D_IMAGE_TRANSFORM_FLIP_X));
			else
				((CIwGameBitmapSprite*)Visual)->setImageTransform((CIw2DImageTransform)flip_y);
		}
	}
	else
	if (element_name == CIwGameXomlNames::FlipY_Hash)
	{
		if (Visual != NULL)
		{
			int flip_x = ((CIwGameBitmapSprite*)Visual)->getImageTransform() & IW_2D_IMAGE_TRANSFORM_FLIP_X;
			if (data.GetAsBool())
				((CIwGameBitmapSprite*)Visual)->setImageTransform((CIw2DImageTransform)(flip_x | IW_2D_IMAGE_TRANSFORM_FLIP_Y));
			else
				((CIwGameBitmapSprite*)Visual)->setImageTransform((CIw2DImageTransform)flip_x);
		}
	}
	else
	if (element_name == CIwGameXomlNames::Skew_Hash)
	{
		if (Visual != NULL)
		{
			data.GetAsListOfFloat(float_pool);
			if (delta)
			{
				CIwRect skew = Visual->getSkew();
				skew.x += (int)(float_pool[0] * 8.0f);
				skew.y += (int)(float_pool[1] * 8.0f);
				skew.w += (int)(float_pool[2] * 8.0f);
				skew.h += (int)(float_pool[3] * 8.0f);
				Visual->setSkew(skew.x, skew.y, skew.w, skew.h);
			}
			else
			{
				Visual->setSkew((int)(float_pool[0] * 8.0f), (int)(float_pool[1] * 8.0f), (int)(float_pool[2] * 8.0f), (int)(float_pool[3] * 8.0f));
			}
		}
	}
	else
	if (element_name == CIwGameXomlNames::AlphaMode_Hash)
	{
		if (Visual != NULL)
		{
			unsigned int alpha_mode_hash = data.getHash();

			if (alpha_mode_hash == IW_GAME_HASH("none"))
				Visual->setAlphaMode(CIwMaterial::ALPHA_NONE);
			else
			if (alpha_mode_hash == IW_GAME_HASH("half"))
				Visual->setAlphaMode(CIwMaterial::ALPHA_HALF);
			else
			if (alpha_mode_hash == IW_GAME_HASH("add"))
				Visual->setAlphaMode(CIwMaterial::ALPHA_ADD);
			else
			if (alpha_mode_hash == IW_GAME_HASH("sub"))
				Visual->setAlphaMode(CIwMaterial::ALPHA_SUB);
			else
			if (alpha_mode_hash == IW_GAME_HASH("blend"))
				Visual->setAlphaMode(CIwMaterial::ALPHA_BLEND);
#if defined(_DEBUG)
			else
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorImage::AlphaMode");
#endif
		}
	}
	else
		return false;

	return true;
}

bool CIwGameActorImage::UpdateFromAnimation(CIwGameAnimInstance *animation)
{
	if (CIwGameActor::UpdateFromAnimation(animation))
		return true;

	bool delta = animation->isDelta();

	unsigned int element_name = animation->getTargetPropertyHash();

	if (element_name == CIwGameXomlNames::Size_Hash)
	{
		if (Visual != NULL)
		{
			CIwGameAnimFrameVec2* frame = (CIwGameAnimFrameVec2*)animation->getCurrentData();
			if (delta)
			{
				int w = Visual->getDestWidth() + (int)frame->data.x;
				int h = Visual->getDestHeight() + (int)frame->data.y;
				SetDestSize(w, h);
			}
			else
				SetDestSize((int)frame->data.x, (int)frame->data.y);
		}
	}
	else
	if (element_name == CIwGameXomlNames::Brush_Hash)
	{
		if (Visual != NULL)
		{
			CIwGameAnimFrameString* frame = (CIwGameAnimFrameString*)animation->getCurrentData();
			IIwGameBrush* brush = (IIwGameBrush*)Scene->getResourceManager()->findResource(frame->data.getHash(), CIwGameXomlNames::Brush_Hash);
			if (brush != NULL)
				SetFromBrush(brush);
#if defined (_DEBUG)
			else
				CIwGameError::LogError("Warning: Brush in animation could not be found - ", frame->data.c_str());
#endif
		}
	}
	else
	if (element_name == CIwGameXomlNames::SrcRect_Hash)
	{
		if (Visual != NULL)
		{
			CIwGameAnimFrameRect* frame = (CIwGameAnimFrameRect*)animation->getCurrentData();
			if (delta)
			{
				CIwGameBitmapSprite* vis = (CIwGameBitmapSprite*)Visual;
				CIwRect rc = vis->getSrcRect();
				rc.x += frame->data.x;
				rc.y += frame->data.y;
				rc.w += frame->data.w;
				rc.h += frame->data.h;
			}
			else
				((CIwGameBitmapSprite*)Visual)->setSrcRect(&frame->data);
		}
	}
	else
	if (element_name == CIwGameXomlNames::Skew_Hash)
	{
		if (Visual != NULL)
		{
			CIwGameAnimFrameVec4* frame = (CIwGameAnimFrameVec4*)animation->getCurrentData();
			if (delta)
			{
				CIwRect skew = Visual->getSkew();
				skew.x += (int)(frame->data.x * 8.0f);
				skew.y += (int)(frame->data.y * 8.0f);
				skew.w += (int)(frame->data.z * 8.0f);
				skew.h += (int)(frame->data.w * 8.0f);
				Visual->setSkew(skew.x, skew.y, skew.w, skew.h);
			}
			else
				Visual->setSkew((int)(frame->data.x * 8.0f), (int)(frame->data.y * 8.0f), (int)(frame->data.z * 8.0f), (int)(frame->data.w * 8.0f));
		}
	}
	else
		return false;

	return true;
}

bool CIwGameActorImage::UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var)
{
	if (!var->Modified || var->getValue().IsEmpty())
		return false;
	if (CIwGameActor::UpdateBinding(element_name, var))
		return true;

	if (element_name == CIwGameXomlNames::Size_Hash)
	{
		if (var->Type == VT_Vec2)
		{
			CIwFVec2* data = &((CIwGameXomlVariableVec2*)var)->NativeValue;
			SetDestSize((int)data->x, (int)data->y);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected vec2 for Size");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::SrcRect_Hash)
	{
		if (var->Type == VT_Vec4)
		{
			CIwFVec4* data = &((CIwGameXomlVariableVec4*)var)->NativeValue;
			SetSrcRect((int)data->x, (int)data->y, (int)data->z, (int)data->w);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected vec4 for SrcRect");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::Image_Hash)
	{
		if (var->Type == VT_String)
		{
			CIwGameImage* image = (CIwGameImage*)Scene->getResourceManager()->findResource(var->getValue().c_str(), CIwGameXomlNames::Image_Hash);
#if defined(_DEBUG)
			if (image == NULL)
				CIwGameError::LogError("Warning: Bound image could not be found - ", var->getValue().c_str());
#endif // _DEBUG
			CIwGameBitmapSprite* sprite = (CIwGameBitmapSprite*)Visual;
			if (sprite != NULL && sprite->getImage() != image)
				sprite->setImage(image);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected string for Image");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::Brush_Hash)
	{
		if (var->Type == VT_String)
		{
			IIwGameBrush* brush = (IIwGameBrush*)Scene->getResourceManager()->findResource(var->getValue().c_str(), CIwGameXomlNames::Brush_Hash);
			if (brush != NULL)
			{
				if (brush->getBrushType() == IIwGameBrush::BT_Image)
				{
					CIwGameBitmapSprite* sprite = (CIwGameBitmapSprite*)Visual;
					if (sprite != NULL && sprite->getBrush() != brush)
						sprite->setFromBrush((CIwGameBrushImage*)brush);
				}
#if defined(_DEBUG)
				else
					CIwGameError::LogError("Warning: Bound brush should be an image brush - ", var->getValue().c_str());
#endif // _DEBUG
			}
#if defined(_DEBUG)
			else
				CIwGameError::LogError("Warning: Bound brush could not be found - ", var->getValue().c_str());
#endif // _DEBUG
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected string for Brush");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::Skew_Hash)
	{
		if (var->Type == VT_Vec4)
		{
			if (Visual != NULL)
			{
				CIwGameXomlVariableVec4* v = (CIwGameXomlVariableVec4*)var;
				Visual->setSkew((int)(v->NativeValue.x * 8.0f), (int)(v->NativeValue.y * 8.0f), (int)(v->NativeValue.z * 8.0f), (int)(v->NativeValue.w * 8.0f));
			}
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected vec4 for Skew");
#endif // _DEBUG
	}
	else
		return false;

	return true;
}

bool CIwGameActorImage::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - Actor needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameActor::LoadFromXoml(parent, false, node))
		return false;

	// Process ActorImage specific attributes
	CIwGameString*	image_name = NULL;
	CIwGameString*	brush_name = NULL;
	CIwFVec2		size = CIwFVec2(0, 0);
	CIwRect			src_rect(0, 0, 0, 0);
	int				image_transform = IW_2D_IMAGE_TRANSFORM_NONE;
	CIwFVec4		skew = CIwFVec4(0, 0, 0, 0);
	bool			before = true;
	bool			filter = true;
	CIwMaterial::AlphaMode alpha_mode = CIwMaterial::ALPHA_BLEND;

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Image_Hash)
			image_name = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Size_Hash)
		{
			if (!(*it)->GetValueAsPoint(size))
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorImage::Size expected vec2");
		}
		else
		if (name_hash == CIwGameXomlNames::SrcRect_Hash)
		{
			if (!(*it)->GetValueAsRect(src_rect))
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorImage::SrcRect expected rect");
		}
		else
		if (name_hash == CIwGameXomlNames::FlipX_Hash)
		{
			if ((*it)->GetValueAsBool())
				image_transform |= IW2D_FLIP_X_F;
		}
		else
		if (name_hash == CIwGameXomlNames::FlipY_Hash)
		{
			if ((*it)->GetValueAsBool())
				image_transform |= IW2D_FLIP_Y_F;
		}
		else
		if (name_hash == CIwGameXomlNames::Brush_Hash || name_hash == CIwGameXomlNames::Background_Hash)
		{
			brush_name = &(*it)->GetValue();
		}
		else
		if (name_hash == CIwGameXomlNames::Skew_Hash)
		{
			if (!(*it)->GetValueAsPoint4(skew))
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorImage::Skew expected vec4");
		}
		else
		if (name_hash == CIwGameXomlNames::BeforeChildren_Hash)
			before = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::Filter_Hash)
			filter = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::AlphaMode_Hash)
		{
			unsigned int alpha_mode_hash = (*it)->GetValue().getHash();

			if (alpha_mode_hash == IW_GAME_HASH("none"))
				alpha_mode = CIwMaterial::ALPHA_NONE;
			else
			if (alpha_mode_hash == IW_GAME_HASH("half"))
				alpha_mode = CIwMaterial::ALPHA_HALF;
			else
			if (alpha_mode_hash == IW_GAME_HASH("add"))
				alpha_mode = CIwMaterial::ALPHA_ADD;
			else
			if (alpha_mode_hash == IW_GAME_HASH("sub"))
				alpha_mode = CIwMaterial::ALPHA_SUB;
			else
			if (alpha_mode_hash == IW_GAME_HASH("blend"))
				alpha_mode = CIwMaterial::ALPHA_BLEND;
			else
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorImage::AlphaMode");
		}
		else
		if (name_hash == CIwGameXomlNames::AspectLock_Hash)
		{
			unsigned int aspect_hash = (*it)->GetValue().getHash();

			if (aspect_hash == IW_GAME_HASH("x"))
				AspectLock = AL_X;
			else
			if (aspect_hash == IW_GAME_HASH("y"))
				AspectLock = AL_Y;
			else
			if (aspect_hash == IW_GAME_HASH("none"))
				AspectLock = AL_None;
			else
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorImage::AspectLock expected x, y, or none");
		}
	}

	CIwGameBrushImage* brush = NULL;
	CIwGameImage* image = NULL;
	if (brush_name != NULL)
	{
		brush = (CIwGameBrushImage*)Scene->getResourceManager()->findResource(brush_name->getHash(), CIwGameXomlNames::Brush_Hash);
		if (brush != NULL)
		{
			image = brush->getImage();
			src_rect = brush->getSrcRect();
		}
		else
			CIwGameError::LogError("Warning: Could not find named brush in the scene or in the global resources - ", brush_name->c_str());
	}
	else
	{
		// Search the scenes resources for the specified image
		if (image_name != NULL)
		{
			image = (CIwGameImage*)Scene->getResourceManager()->findResource(image_name->c_str(), CIwGameXomlNames::Image_Hash);
			if (image == NULL)
			{
				CIwGameError::LogError("Warning: XOML - Could not find named image in the scene or in the global resources - ", image_name->c_str());
			}
		}
	}

	// If src_rect is specified and no size then take szie from the src_rect
	if (src_rect.w != 0 && size.x == 0)
		size.x = src_rect.w;
	if (src_rect.h != 0 && size.y == 0)
		size.y = src_rect.h;

	// Initialise the image actor
	CIwGameBitmapSprite* visual;
	if (brush != NULL)
	{
		Init(brush, (int)size.x, (int)size.y);
		visual = (CIwGameBitmapSprite*)Visual;;
	}
	else
	{
		Init(image, (int)size.x, (int)size.y);
		visual = (CIwGameBitmapSprite*)Visual;;
		if (src_rect.w != 0)
		{
			visual->setSrcRect(&src_rect);
		}
	}
	visual->setImageTransform((CIw2DImageTransform)image_transform);
	Visual->setSkew((int)(skew.x * 8.0f), (int)(skew.y * 8.0f), (int)(skew.z * 8.0f), (int)(skew.w * 8.0f));
	Visual->setBeforeChildren(before);
	Visual->setFilter(filter);
	Visual->setAlphaMode(alpha_mode);

	// Set docking
	if (Docking != DK_None)
		setDocking(Docking);

	// Process inner tags
	if (load_children)
	{
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}

	return true;
}

void CIwGameActorImage::OnOrientationChange(eIwGameScene_Orientation old_orientation, eIwGameScene_Orientation new_orientation)
{
	CIwGameActor::OnOrientationChange(old_orientation, new_orientation);

	CIwVec2		size = OriginalSize;
	
	UpdateMargins();
	if (LinkedTo != NULL)
	{
		if (OriginalSize.x < 0)
			size.x = (LinkedTo->getSize().x * -OriginalSize.x) / 100;
		if (OriginalSize.y < 0)
			size.y = (LinkedTo->getSize().y * -OriginalSize.y) / 100;

		if (size.x == 0)
			size.x = LinkedTo->getSize().x;
		if (size.y == 0)
			size.y = LinkedTo->getSize().y;
	}
	else
	{
		// If negative size has been specified then base size on a percentage of virtual canvas
		if (OriginalSize.x < 0)
			size.x = ((Scene->getScreenSize().x * IW_GEOM_ONE) / Scene->getVirtualTransform().m[0][0] * -OriginalSize.x) / 100;
		if (OriginalSize.y < 0)
			size.y = ((Scene->getScreenSize().y * IW_GEOM_ONE) / Scene->getVirtualTransform().m[1][1] * -OriginalSize.y) / 100;
	}

	SetDestSize(size.x, size.y);
}

//
//
//
//
//
// CIwGameActorText implementation
//
//
//
//
//

void CIwGameActorText::setText(const char* text)
{
	CIwGameTextSprite* visual = (CIwGameTextSprite*)Visual;
	if (visual != NULL)
	{
		visual->setText(text);

		// Generate the prepared fnot data
		CIwGameTextSprite* sprite = (CIwGameTextSprite*)visual;
		if (sprite->getFont()->isFontLoaded())
		{
			sprite->RebuildText();

			// Resize rect to fit the text coverage
			if (AutoHeight)
			{
				CIwRect rect = sprite->getRect();
				CIwGxFontPreparedData* font_data = sprite->getPreparedData();
				int height = font_data->GetHeight();
				rect.h = height;
				setRect(rect);
			}
		}
	}
}

void CIwGameActorText::setRect(CIwRect& rect)
{
	CIwGameTextSprite* visual = (CIwGameTextSprite*)Visual;
	if (visual != NULL)
		visual->setRect(rect);
	Size.x = rect.w;
	Size.y = rect.h;
	if (Docking != DK_None)
		setDocking(Docking);
}

void CIwGameActorText::setFlags(IwGxFontFlags flags)
{
	CIwGameTextSprite* visual = (CIwGameTextSprite*)Visual;
	if (visual != NULL)
		visual->setFlags(flags);
}
void CIwGameActorText::setFont(CIwGameFont* font)
{
	CIwGameTextSprite* visual = (CIwGameTextSprite*)Visual;
	if (visual != NULL)
		visual->setFont(font);
}

void CIwGameActorText::setAlignH(IwGxFontAlignHor align)
{
	CIwGameTextSprite* visual = (CIwGameTextSprite*)Visual;
	if (visual != NULL)
		visual->setAlignH(align);
}

void CIwGameActorText::setAlignV(IwGxFontAlignVer align)
{
	CIwGameTextSprite* visual = (CIwGameTextSprite*)Visual;
	if (visual != NULL)
		visual->setAlignV(align);
}

bool CIwGameActorText::Init(CIwGameFont* font, CIwRect& rect, CIwGameString& text, int flags)
{
	CIwGameActor::Init();

	OriginalSize.x = rect.w;
	OriginalSize.y = rect.h;
	OriginallRect = rect;
	OriginalMargin = Margin;
	CalculateMargins(Margin);
	if (LinkedTo != NULL)
	{
		// If negative size has been specified and part of a linked actor then base size on a percentage of parent
		if (rect.w < 0)
		{
			rect.w = (LinkedTo->getSize().x * -rect.w) / 100;
			rect.x = -rect.w / 2;
		}
		if (rect.h < 0)
		{
			rect.h = (LinkedTo->getSize().y * -rect.h) / 100;;
			rect.y = -rect.h / 2;
		}
	}
	else
	{
		// If negative size has been specified then base size on a percentage of virtual canvas
		if (rect.w < 0)
		{
			rect.w = ((Scene->getScreenSize().x * IW_GEOM_ONE) / Scene->getVirtualTransform().m[0][0] * -rect.w) / 100;
			rect.x = -rect.w / 2;
		}
		if (rect.h < 0)
		{
			rect.h = ((Scene->getScreenSize().y * IW_GEOM_ONE) / Scene->getVirtualTransform().m[1][1] * -rect.h) / 100;
			rect.y = -rect.h / 2;
		}
	}

	Size.x = rect.w;
	Size.y = rect.h;

	// Adjust for text margin
	rect.x += Margin.x;
	rect.y += Margin.w;
	rect.w -= Margin.x + Margin.y;
	rect.h -= Margin.w + Margin.h;

	// Create sprite
	CIwGameTextSprite* sprite = new CIwGameTextSprite();
	if (sprite == NULL)
		return false;
	sprite->Init();

	// Set sprite parammeters
	sprite->setFont(font);
	sprite->setRect(rect);
	sprite->setFlags((IwGxFontFlags)flags);
	sprite->setDestSize(rect.w, rect.h);
	sprite->setText(text.c_str());

	// Set sprite as visual
	Visual = sprite;

	// Add sprite to the sprite manager so it can be managed and drawn
	Scene->getSpriteManager()->addSprite(sprite);

	// Set-up LinkedTo
	if (LinkedTo != NULL)
		Visual->setLinkedTo(LinkedTo->getVisual());

	// Handle docking
	if (Docking != DK_None)
		setDocking(Docking);

	return true;
}

bool CIwGameActorText::UpdateVisual()
{
	if (!CIwGameActor::UpdateVisual())
		return false;

	if (!FontLoaded && AutoHeight && Visual != NULL)
	{
		CIwGameTextSprite* sprite = (CIwGameTextSprite*)Visual;
		if (sprite->getFont()->isFontLoaded())
		{
			sprite->RebuildText();

			// Resize rect to fit the text coverage
			if (AutoHeight)
			{
				CIwRect rect = sprite->getRect();
				CIwGxFontPreparedData* font_data = sprite->getPreparedData();
				int height = font_data->GetHeight();
				rect.h = height;
				setRect(rect);
			}
			FontLoaded = true;
		}
	}

	return true;
}

bool CIwGameActorText::setProperty(unsigned int element_name, CIwGameString& data, bool delta)
{
	if (CIwGameActor::setProperty(element_name, data, delta))
		return true;

	float float_pool[8];

	if (element_name == CIwGameXomlNames::Text_Hash)
		setText(data.c_str());
	else
	if (element_name == CIwGameXomlNames::Font_Hash)
	{
		CIwGameFont* font = (CIwGameFont*)Scene->getResourceManager()->findResource(data.getHash(), CIwGameXomlNames::Font_Hash);
		if (font != NULL)
			setFont(font);
		else
			CIwGameError::LogError("Warning: setProperty font not found in resources - ", data.c_str());
	}
	else
	if (element_name == CIwGameXomlNames::AlignH_Hash)
	{
		IwGxFontAlignHor alignh = IW_GX_FONT_ALIGN_CENTRE;

		unsigned int align_hash = data.getHash();
		if (align_hash == CIwGameXomlNames::left_Hash)
			alignh = IW_GX_FONT_ALIGN_LEFT;
		else
		if (align_hash == CIwGameXomlNames::right_Hash)
			alignh = IW_GX_FONT_ALIGN_RIGHT;
		setAlignH(alignh);
	}
	else
	if (element_name == CIwGameXomlNames::AlignV_Hash)
	{
		IwGxFontAlignVer alignv = IW_GX_FONT_ALIGN_MIDDLE;

		unsigned int align_hash = data.getHash();
		if (align_hash == CIwGameXomlNames::top_Hash)
			alignv = IW_GX_FONT_ALIGN_TOP;
		else
		if (align_hash == CIwGameXomlNames::bottom_Hash)
			alignv = IW_GX_FONT_ALIGN_BOTTOM;
		setAlignV(alignv);
	}
	else
	if (element_name == CIwGameXomlNames::Wrap_Hash)
	{
		if (data.GetAsBool())
			setFlags(IW_GX_FONT_DEFAULT_F);
		else
			setFlags(IW_GX_FONT_NOWRAP_F);
	}
	else
	if (element_name == CIwGameXomlNames::Skew_Hash)
	{
		if (Visual != NULL)
		{
			data.GetAsListOfFloat(float_pool);
			if (delta)
			{
				CIwRect skew = Visual->getSkew();
				skew.x += (int)(float_pool[0] * 8.0f);
				skew.y += (int)(float_pool[1] * 8.0f);
				skew.w += (int)(float_pool[2] * 8.0f);
				skew.h += (int)(float_pool[3] * 8.0f);
				Visual->setSkew(skew.x, skew.y, skew.w, skew.h);
			}
			else
				Visual->setSkew((int)(float_pool[0] * 8.0f), (int)(float_pool[1] * 8.0f), (int)(float_pool[2] * 8.0f), (int)(float_pool[3] * 8.0f));
		}
	}
	else
	if (element_name == CIwGameXomlNames::AlphaMode_Hash)
	{
		if (Visual != NULL)
		{
			unsigned int alpha_mode_hash = data.getHash();

			if (alpha_mode_hash == IW_GAME_HASH("none"))
				Visual->setAlphaMode(CIwMaterial::ALPHA_NONE);
			else
			if (alpha_mode_hash == IW_GAME_HASH("half"))
				Visual->setAlphaMode(CIwMaterial::ALPHA_HALF);
			else
			if (alpha_mode_hash == IW_GAME_HASH("add"))
				Visual->setAlphaMode(CIwMaterial::ALPHA_ADD);
			else
			if (alpha_mode_hash == IW_GAME_HASH("sub"))
				Visual->setAlphaMode(CIwMaterial::ALPHA_SUB);
			else
			if (alpha_mode_hash == IW_GAME_HASH("blend"))
				Visual->setAlphaMode(CIwMaterial::ALPHA_BLEND);
#if defined(_DEBUG)
			else
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorText::AlphaMode");
#endif
		}
	}
	else
		return false;

	return true;
}

bool CIwGameActorText::UpdateFromAnimation(CIwGameAnimInstance *animation)
{
	if (CIwGameActor::UpdateFromAnimation(animation))
		return true;

	bool delta = animation->isDelta();

	unsigned int element_name = animation->getTargetPropertyHash();

	if (element_name == CIwGameXomlNames::Skew_Hash)
	{
		if (Visual != NULL)
		{
			CIwGameAnimFrameVec4* frame = (CIwGameAnimFrameVec4*)animation->getCurrentData();
			if (delta)
			{
				CIwRect skew = Visual->getSkew();
				skew.x += (int)(frame->data.x * 8.0f);
				skew.y += (int)(frame->data.y * 8.0f);
				skew.w += (int)(frame->data.z * 8.0f);
				skew.h += (int)(frame->data.w * 8.0f);
				Visual->setSkew(skew.x, skew.y, skew.w, skew.h);
			}
			else
				Visual->setSkew((int)(frame->data.x * 8.0f), (int)(frame->data.y * 8.0f), (int)(frame->data.z * 8.0f), (int)(frame->data.w * 8.0f));
		}
	}
	else
		return false;

	return true;
}

bool CIwGameActorText::UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var)
{
	if (!var->Modified || var->getValue().IsEmpty())
		return false;
	if (CIwGameActor::UpdateBinding(element_name, var))
		return true;

	if (element_name == CIwGameXomlNames::Text_Hash)
	{
		if (Visual != NULL && ((CIwGameTextSprite*)Visual)->getText().getHash() != var->getValue().getHash())
			setText(var->getValue().c_str());
	}
	else
	if (element_name == CIwGameXomlNames::rect_Hash)
	{
		if (var->Type == VT_Vec4)
		{
			CIwFVec4* data = &((CIwGameXomlVariableVec4*)var)->NativeValue;
			CIwRect rc((int)data->x, (int)data->y, (int)data->z, (int)data->w);
			setRect(rc);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected vec4 for SrcRect");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::Skew_Hash)
	{
		if (var->Type == VT_Vec4)
		{
			if (Visual != NULL)
			{
				CIwGameXomlVariableVec4* v = (CIwGameXomlVariableVec4*)var;
				Visual->setSkew((int)(v->NativeValue.x * 8.0f), (int)(v->NativeValue.y * 8.0f), (int)(v->NativeValue.z * 8.0f), (int)(v->NativeValue.w * 8.0f));
			}
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected vec4 for Skew");
#endif // _DEBUG
	}
	else
		return false;

	return true;
}

bool CIwGameActorText::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - Actor needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameActor::LoadFromXoml(parent, false, node))
		return false;

	// Process Actor Font specific attributes
	CIwGameString*	font_name = NULL;
	CIwGameString*	text = NULL;
	CIwRect			rect = CIwRect(0, 0, -100, -100);
	IwGxFontAlignHor alignh = IW_GX_FONT_ALIGN_CENTRE;
	IwGxFontAlignVer alignv = IW_GX_FONT_ALIGN_MIDDLE;
	int				flags = 0;
	CIwFVec4		skew = CIwFVec4(0, 0, 0, 0);
	bool			before = true;
	bool			filter = true;
	CIwMaterial::AlphaMode alpha_mode = CIwMaterial::ALPHA_BLEND;

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Font_Hash)
			font_name = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::rect_Hash)
		{
			if (!(*it)->GetValueAsRect(rect))
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorText::Rect");
		}
		else
		if (name_hash == CIwGameXomlNames::Text_Hash)
			text = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::AlignH_Hash)
		{
			unsigned int align_hash = (*it)->GetValue().getHash();
			if (align_hash == CIwGameXomlNames::left_Hash)
				alignh = IW_GX_FONT_ALIGN_LEFT;
			else
			if (align_hash == CIwGameXomlNames::centre_Hash)
				alignh = IW_GX_FONT_ALIGN_CENTRE;
			else
			if (align_hash == CIwGameXomlNames::right_Hash)
				alignh = IW_GX_FONT_ALIGN_RIGHT;
		}
		else
		if (name_hash == CIwGameXomlNames::AlignV_Hash)
		{
			unsigned int align_hash = (*it)->GetValue().getHash();
			if (align_hash == CIwGameXomlNames::top_Hash)
				alignv = IW_GX_FONT_ALIGN_TOP;
			else
			if (align_hash == CIwGameXomlNames::middle_Hash)
				alignv = IW_GX_FONT_ALIGN_MIDDLE;
			else
			if (align_hash == CIwGameXomlNames::bottom_Hash)
				alignv = IW_GX_FONT_ALIGN_BOTTOM;
		}
		else
		if (name_hash == CIwGameXomlNames::Wrap_Hash)
		{
			if ((*it)->GetValueAsBool())
				flags |= IW_GX_FONT_DEFAULT_F;
			else
				flags |= IW_GX_FONT_NOWRAP_F;
		}
		else
		if (name_hash == CIwGameXomlNames::Italic_Hash)
		{
			if ((*it)->GetValueAsBool())
				flags |= IW_GX_FONT_ITALIC_F;
		}
		else
		if (name_hash == CIwGameXomlNames::Underline_Hash)
		{
			if ((*it)->GetValueAsBool())
				flags |= IW_GX_FONT_UNDERLINE_F;
		}
		else
		if (name_hash == CIwGameXomlNames::Skew_Hash)
		{
			if (!(*it)->GetValueAsPoint4(skew))
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorText::Skew");
		}
		else
		if (name_hash == CIwGameXomlNames::BeforeChildren_Hash)
			before = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::Filter_Hash)
			filter = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::AutoHeight_Hash)
			AutoHeight = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::AlphaMode_Hash)
		{
			unsigned int aspect_hash = (*it)->GetValue().getHash();

			if (aspect_hash == IW_GAME_HASH("none"))
				alpha_mode = CIwMaterial::ALPHA_NONE;
			else
			if (aspect_hash == IW_GAME_HASH("half"))
				alpha_mode = CIwMaterial::ALPHA_HALF;
			else
			if (aspect_hash == IW_GAME_HASH("add"))
				alpha_mode = CIwMaterial::ALPHA_ADD;
			else
			if (aspect_hash == IW_GAME_HASH("sub"))
				alpha_mode = CIwMaterial::ALPHA_SUB;
			else
			if (aspect_hash == IW_GAME_HASH("blend"))
				alpha_mode = CIwMaterial::ALPHA_BLEND;
			else
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorImage::AlphaMode");
		}
	}

	// Search the scenes resources for the specified font
	CIwGameFont* font = NULL;
	if (font_name != NULL)
	{
		font = (CIwGameFont*)Scene->getResourceManager()->findResource(font_name->c_str(), CIwGameXomlNames::Font_Hash);
		if (font == NULL)
			CIwGameError::LogError("Warning: XOML - Could not find named font in the scene or in the global resources - ", font_name->c_str());
	}

	// Initialise the image actor
	if (text == NULL)
	{
		CIwGameString text;
		Init(font, rect, text, flags);
	}
	else
		Init(font, rect, *text, flags);

	// Set alignment
	setAlignH(alignh);
	setAlignV(alignv);

	// Set skewing
	Visual->setSkew((int)(skew.x * 8.0f), (int)(skew.y * 8.0f), (int)(skew.z * 8.0f), (int)(skew.w * 8.0f));
	Visual->setBeforeChildren(before);
	Visual->setFilter(filter);
	Visual->setAlphaMode(alpha_mode);

	// Set docking
	if (Docking != DK_None)
		setDocking(Docking);

	// Process inner tags
	if (load_children)
	{
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}

	return true;
}

void CIwGameActorText::OnOrientationChange(eIwGameScene_Orientation old_orientation, eIwGameScene_Orientation new_orientation)
{
	CIwGameActor::OnOrientationChange(old_orientation, new_orientation);

//	CIwRect		rect = ((CIwGameTextSprite*)Visual)->getRect();
	CIwRect		rect = OriginallRect;

	UpdateMargins();
	if (LinkedTo != NULL)
	{
		// If negative size has been specified and part of a linked actor then base size on a percentage of parent
		if (OriginalSize.x < 0)
		{
			rect.w = (LinkedTo->getSize().x * -OriginalSize.x) / 100;
			rect.x = -rect.w / 2;
		}
		if (OriginalSize.y < 0)
		{
			rect.h = (LinkedTo->getSize().y * -OriginalSize.y) / 100;;
			rect.y = -rect.h / 2;
		}

		if (rect.w == 0)
			rect.w = LinkedTo->getSize().x;
		if (rect.h == 0)
			rect.h = LinkedTo->getSize().y;
	}
	else
	{
		// If negative size has been specified then base size on a percentage of virtual canvas
		if (OriginalSize.x < 0)
		{
			rect.w = ((Scene->getScreenSize().x * IW_GEOM_ONE) / Scene->getVirtualTransform().m[0][0] * -OriginalSize.x) / 100;
			rect.x = -rect.w / 2;
		}
		if (OriginalSize.y < 0)
		{
			rect.h = ((Scene->getScreenSize().y * IW_GEOM_ONE) / Scene->getVirtualTransform().m[1][1] * -OriginalSize.y) / 100;
			rect.y = -rect.h / 2;
		}
	}

	// Adjust for text margin
	rect.x += Margin.x;
	rect.y += Margin.w;
	rect.w -= Margin.x + Margin.y;
	rect.h -= Margin.w + Margin.h;
	setRect(rect);

	// Resize rect to fit the text coverage
	if (AutoHeight && Visual != NULL)
	{
		CIwGameTextSprite* sprite = (CIwGameTextSprite*)Visual;
		sprite->RebuildText();
		CIwGxFontPreparedData* font_data = sprite->getPreparedData();
		int height = font_data->GetHeight();
		rect.h = height;
		setRect(rect);
	}
}

//
//
//
//
//  CIwGameXomlAction_Actors implementation
//
//
//
//
void CIwGameXomlAction_Actors::Execute(IIwGameXomlResource* source, CIwGameAction* action)
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
	IIwGameXomlResource* cont = (actor != NULL) ? (IIwGameXomlResource*)actor : (IIwGameXomlResource*)scene;

	if (Type == Action_SetProperty || Type == Action_AddProperty)
	{
		if (!action->Parameter4.IsEmpty())	// Check for a specific scene
		{
			CIwGameScene* new_scene = game->findScene(action->getParameter4(cont).getHash());
			if (new_scene != NULL)
				scene = new_scene;
			else
				CIwGameError::LogError("Warning: Set(Add)Property - Scene was not found - ", action->getParameter4(cont).c_str());
		}
		if (!action->Parameter3.IsEmpty())	// Check for a specific actor
		{
			CIwGameActor* new_actor = scene->findActor(action->getParameter3(cont).getHash());
			if (new_actor != NULL)
				actor = new_actor;
			else
				CIwGameError::LogError("Warning: Set(Add)Property - Actor was not found - ", action->getParameter3(cont).c_str());
		}
		actor->setProperty(action->getParameter1().getHash(), action->getParameter2(cont), Type == Action_AddProperty);
		return;
	}

	CIwGameActor* new_actor = action->Parameter1.IsEmpty() ? actor : scene->findActor(action->getParameter1(cont).getHash());
	if (new_actor != NULL)
	{
		switch (Type)
		{
		case Action_HideActor:
			new_actor->setVisible(false);
			break;
		case Action_ShowActor:
			new_actor->setVisible(true);
			break;
		case Action_ActivateActor:
			new_actor->setActive(true);
			break;
		case Action_DeactivateActor:
			new_actor->setActive(false);
			break;
		case Action_KillActor:
			scene->removeActor(new_actor);
			break;
		case Action_SetKeyFocus:
			{
				if (new_actor->getCategory() != CIwGameActor::AC_UI)
					CIwGameError::LogError("Warning: SetKeyFocus - Actor needs to be a UI actor to receive key focus");
				else
					game->setKeyFocus((CIwGameUIBase*)actor);
			}
			break;
		}
	}
	else
		CIwGameError::LogError("Warning: Actions - Actor was not found - ", action->getParameter1().c_str());
}




