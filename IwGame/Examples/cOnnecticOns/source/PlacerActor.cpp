/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#include "IwGameActor.h"
#include "IwGameScene.h"
#include "IwGameInput.h"
#include "IwGameScene.h"
#include "IwGameCamera.h"

#include "PlacerActor.h"


//
//
//
// PlacerActor implementation
//
//
//

bool PlacerActor::Init(CIwGameImage* image, int width, int height)
{
	if (!CIwGameActorImage::Init(image, width, height))
		return false;

	Dragging = false;

	return true;
}

bool PlacerActor::Init(CIwGameImage* image, int x, int y, int w, int h)
{
	if (!CIwGameActorImage::Init(image, x, y, w, h))
		return false;

	Dragging = false;

	return true;
}


bool PlacerActor::Update(float dt)
{
	if (Dragging)
	{
		CIwVec2 touch_pos  = IW_GAME_INPUT->getTouchedPos(0);
		CIwFVec2 cam_pos = Scene->getCamera()->getPosition();
		CIwFMat2D& cam_transform = Scene->getCamera()->getTransform();
		cam_pos.x = cam_pos.x / cam_transform.m[0][0];
		cam_pos.y = cam_pos.y / cam_transform.m[1][1];

		CIwFVec2 touch_pos2 = Scene->ScreenToVirtual(touch_pos.x, touch_pos.y);

		setPosition(cam_pos.x + touch_pos2.x, cam_pos.y + touch_pos2.y);
	}

	return CIwGameActorImage::Update(dt);
}


void PlacerActor::OnBeginTouch(int index, int x, int y, bool allow_bubble)
{
	CIwGameActorImage::OnBeginTouch(index, x, y, allow_bubble);

	Dragging = true;
}

void PlacerActor::OnEndTouch(int index, int x, int y, bool allow_bubble)
{
	CIwGameActorImage::OnEndTouch(index, x, y, allow_bubble);

	Dragging = false;

	// If user dropped us at bottom of screen then he wanted to delete us
	if (Position.y >= 220)
		Scene->removeActor(this);

}


bool PlacerActor::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Load the actor from XOML
	if (!CIwGameActorImage::LoadFromXoml(parent, load_children, node))
		return false;

	// Process InertActor specific attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();
	}

	return true;
}
