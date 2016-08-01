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

#include "GravityPlacerActor.h"
#include "EditScene.h"


//
//
//
// GravityPlacerActor implementation
//
//
//

bool GravityPlacerActor::Init(CIwGameImage* image, int width, int height)
{
	if (!CIwGameActorImage::Init(image, width, height))
		return false;

	Dragging = false;

	return true;
}

bool GravityPlacerActor::Init(CIwGameImage* image, int x, int y, int w, int h)
{
	if (!CIwGameActorImage::Init(image, x, y, w, h))
		return false;

	Dragging = false;

	return true;
}


bool GravityPlacerActor::Update(float dt)
{
	// Position the marker at the curser
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

	// Point the marker to the world centre
	float d = Position.x * Position.x + Position.y * Position.y;
	d = sqrtf(d);
	if (d < 30.0f) d = 30.0f;
	setScale(d / 100.0f);
	setAngle((float)(atan2f(Position.y, -Position.x) * 180.0f) / M_PI + 90.0f);

	// Update gravity text
	CIwGameActorText* text = (CIwGameActorText*)Scene->findActor("GravityDirectionText");
	if (text != NULL)
	{
		if (d <= 30)
		{
			text->setText("0, 0");
		}
		else
		{
			int dx = (int)-(Position.x / 10);
			int dy = (int)-(Position.y / 10);
			CIwGameString coords = dx;
			coords += ", ";
			coords += dy;
			text->setText(coords.c_str());
		}
	}

	return CIwGameActorImage::Update(dt);
}


void GravityPlacerActor::OnBeginTouch(int index, int x, int y, bool allow_bubble)
{
	CIwGameActorImage::OnBeginTouch(index, x, y, allow_bubble);

	Dragging = true;
}

void GravityPlacerActor::OnEndTouch(int index, int x, int y, bool allow_bubble)
{
	CIwGameActorImage::OnEndTouch(index, x, y, allow_bubble);

	Dragging = false;

}


bool GravityPlacerActor::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
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
