/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#if !defined(_INERT_ACTOR_H_)
#define _INERT_ACTOR_H_

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
// InertActor - An inert actor that has no real functionality of its own
//
//
class InertActor : public CIwGameActorImage
{
public:

protected:
	// Properties
public:
	// Properties end

	// Implementation of IIwGameXomlClass interface
	bool			LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  InertActorCreator - Creates an instance of a InertActor object
//
class InertActorCreator : public IIwGameXomlClassCreator
{
public:
	InertActorCreator()
	{
		setClassName("InertActor");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new InertActor(); }
};

#endif // _INERT_ACTOR_H_
