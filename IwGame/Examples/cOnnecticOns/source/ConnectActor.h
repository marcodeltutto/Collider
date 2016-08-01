/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#if !defined(_CONNECT_ACTOR_H_)
#define _CONNECT_ACTOR_H_

#include "IwGeom.h"

#include "IwGameUtil.h"
#include "IwGameSprite.h"
#include "IwGameAnim.h"
#include "IwGameScene.h"
#include "IwGameActor.h"
#include "IwGameImage.h"
#include "IwGameXoml.h"

class CounterActor;

//
//
// ConnectActor - A connector actor that has connects two actors usnig a string
//
//
class ConnectActor : public CIwGameActorImage
{
public:

protected:
	// Properties
	CounterActor*		Actor1;			// First linked actor
	CounterActor*		Actor2;			// Second linked actor

public:
	void				setActor1(CounterActor* actor)			{ Actor1 = actor; }
	CounterActor*		getActor1()								{ return Actor1; }
	void				setActor2(CounterActor* actor)			{ Actor2 = actor; }
	CounterActor*		getActor2()								{ return Actor2; }
	// Properties end
public:
	bool				Init(CIwGameImage* image, int width, int height);
	bool				Update(float dt);

	// Implementation of IIwGameXomlClass interface
	bool			LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	// Utility
	void				UpdateConnector();
};

//
//  ConnectActorCreator - Creates an instance of a InertActor object
//
class ConnectActorCreator : public IIwGameXomlClassCreator
{
public:
	ConnectActorCreator()
	{
		setClassName("ConnectActor");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new ConnectActor(); }
};

#endif // _CONNECT_ACTOR_H_
