/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#include "IwGameActor.h"
#include "IwGameScene.h"

#include "FloaterActor.h"


//
//
//
// InertActor implementation
//
//
//
bool FloaterActor::Init(CIwGameFont* font)
{
	CIwRect rect = CIwRect(-64, -16, 128, 32);
	CIwGameString text;

	if (!CIwGameActorText::Init(font, rect, text, 0))
		return false;

	Fade = 1.0f;
	FadeSpeed = 0.01f;
	ScaleSpeed = 0;

	return true;
}

bool FloaterActor::Update(float dt)
{
	Fade -= FadeSpeed * dt;
	if (Fade < 0)
		Fade = 0;

	Colour.a = (uint8)(255.0f * Fade);

	if (Fade == 0)
		return false;

	return CIwGameActorText::Update(dt);
}

bool FloaterActor::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Load the actor from XOML
	if (!CIwGameActorText::LoadFromXoml(parent, load_children, node))
		return false;

	// Process InertActor specific attributes
/*	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();
	}*/

	return true;
}
