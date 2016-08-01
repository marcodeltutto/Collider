/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#if !defined(_FLOATER_ACTOR_H_)
#define _FLOATER_ACTOR_H_

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
// FloaterActor - A basic actor that floats upwards and fades out
//
//
class FloaterActor : public CIwGameActorText
{
public:

protected:
	// Properties
	float				FadeSpeed;
	float				ScaleSpeed;
public:
	void				setFadeSpeed(float speed)	{ FadeSpeed = speed; }
	void				setScaleSpeed(float speed)	{ ScaleSpeed = speed; }
	// Properties end
protected:
	float				Fade;
public:
	bool				Init(CIwGameFont* font);
	bool				Update(float dt);

	// Implementation of IIwGameXomlClass interface
	bool			LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  FloaterActorCreator - Creates an instance of a FloaterActor object
//
class FloaterActorCreator : public IIwGameXomlClassCreator
{
public:
	FloaterActorCreator()
	{
		setClassName("FloaterActor");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new FloaterActor(); }
};

#endif // _FLOATER_ACTOR_H_
