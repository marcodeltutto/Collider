/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#if !defined(_GRAVITY_PLACER_ACTOR_H_)
#define _GRAVITY_PLACER_ACTOR_H_

#include "IwGeom.h"

#include "IwGameUtil.h"
#include "IwGameSprite.h"
#include "IwGameAnim.h"
#include "IwGameScene.h"
#include "IwGameActor.h"
#include "IwGameImage.h"
#include "IwGameXoml.h"


//
//
// GravityPlacerActor - A gravity direction actor that can be dragged around and placed
//
//
class GravityPlacerActor : public CIwGameActorImage
{
public:

protected:
	// Properties
protected:
	bool				Dragging;
public:
	// Properties end
	bool		Init(CIwGameImage* image, int width, int height);
	bool		Init(CIwGameImage* image, int x, int y, int w, int h);
	bool		Update(float dt);

	void		OnBeginTouch(int index, int x, int y, bool allow_bubble);
	void		OnEndTouch(int index, int x, int y, bool allow_bubble);

	// Implementation of IIwGameXomlClass interface
	bool			LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  GravityPlacerActorCreator
//
class GravityPlacerActorCreator : public IIwGameXomlClassCreator
{
public:
	GravityPlacerActorCreator()
	{
		setClassName("gravityplaceractor");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new GravityPlacerActor(); }
};

#endif // _GRAVITY_PLACER_ACTOR_H_
