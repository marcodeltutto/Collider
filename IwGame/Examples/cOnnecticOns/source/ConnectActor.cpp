/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#include "IwGameActor.h"
#include "IwGameScene.h"

#include "ConnectActor.h"
#include "CounterActor.h"


//
//
//
// ConnectActor implementation
//
//
//

bool ConnectActor::Init(CIwGameImage* image, int width, int height)
{
	if (!CIwGameActorImage::Init(image, width, height))
		return false;

	Actor1 = NULL;
	Actor2 = NULL;
	IsTappable = false;
	IsCollidable = false;

	return true;
}

bool ConnectActor::Update(float dt)
{
	UpdateConnector();
	return CIwGameActorImage::Update(dt);
}

void ConnectActor::UpdateConnector()
{
	if (Actor1 == NULL || Actor2 == NULL)
		return;

	CIwFVec2 pos1 = Actor1->getPosition();
	CIwFVec2 pos2 = Actor2->getPosition();

	// Calculate how long the connecting string is
	float dx = pos2.x - pos1.x;
	float dy = pos2.y - pos1.y;
	float d = dx * dx + dy * dy;
	d = sqrtf(d);
	if (d < 1.0f) d = 1.0f;
	if (d > 1000.0f)
		d = 1.0f;

	// Calculate angle between both actors
	float theta = (atan2f(dy, dx) * 180.0f) / M_PI;

	ScaleX = d / 100.0f;
	Angle = theta;

	Position.x = (pos1.x + pos2.x) / 2;
	Position.y = (pos1.y + pos2.y) / 2;

}

bool ConnectActor::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
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
