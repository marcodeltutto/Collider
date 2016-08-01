/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#if !defined(_ZONE_EDIT_SCENE_H_)
#define _ZONE_EDIT_SCENE_H_

#include "IwGameScene.h"
#include "IwGameActorParticles.h"

struct EditSceneObject
{
	int x;
	int y;
	int type;
};

//
//
// EditScene - Level editor scene
//
//
class EditScene : public CIwGameScene
{
public:

protected:
	// Properties
public:
	// Properties end

protected:
	int					NumConnecticons;
	CIwFVec2			Gravity;

public:
	~EditScene() { SaveSceneObjects(); }
	virtual int			Init(int max_collidables = 128, int max_layers = 10, bool doSleep = true);
	virtual void		Update(float dt);

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	void				AddObject(int id, int x, int y, bool close = false);
	void				SaveSceneObjects();
	void				LoadSceneObjects();

	void				SaveScene();
};

//
//  EditSceneCreator - Creates an instance of a level editor scene object
//
class EditSceneCreator : public IIwGameXomlClassCreator
{
public:
	EditSceneCreator()
	{
		setClassName("EditScene");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new EditScene(); }
};

#endif // _ZONE_EDIT_SCENE_H_
