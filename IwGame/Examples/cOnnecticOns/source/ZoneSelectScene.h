/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#if !defined(_ZONE_SELECT_SCENE_H_)
#define _ZONE_SELECT_SCENE_H_

#include "IwGameScene.h"
#include "IwGameActorParticles.h"

//
//
// ZoneSelectScene - Zone select scene
//
//
class ZoneSelectScene : public CIwGameScene
{
public:

protected:
	// Properties
public:
	// Properties end

protected:

public:
	virtual int			Init(int max_collidables = 128, int max_layers = 10, bool doSleep = true);
	virtual void		Update(float dt);

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  ZoneSelectSceneCreator - Creates an instance of a level scene object
//
class ZoneSelectSceneCreator : public IIwGameXomlClassCreator
{
public:
	ZoneSelectSceneCreator()
	{
		setClassName("ZoneSelectScene");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new ZoneSelectScene(); }
};

#endif // _ZONE_SELECT_SCENE_H_
