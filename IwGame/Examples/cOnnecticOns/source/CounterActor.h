/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#if !defined(_COUNTER_ACTOR_H_)
#define _COUNTER_ACTOR_H_

#include "IwGeom.h"

#include "IwGameUtil.h"
#include "IwGameSprite.h"
#include "IwGameAnim.h"
#include "IwGameScene.h"
#include "IwGameActor.h"
#include "IwGameImage.h"
#include "IwGameXoml.h"

#include "GameScene.h"

class ConnectActor;

//
//
// CounterActor - A counter (a bug) actor
//
//
class CounterActor : public CIwGameActorImage
{
public:

protected:
	// Properties
	eCounterColour		CounterColour;				// Colour of counter
	int					Health;						// Health of counter
	bool				IsFlying;					// Is counter currently flying
	bool				IsDying;					// Is counter currently dying
	int					Score;						// Score obtrained for destroying this counter
	bool				Selected;					// Is counter currently selected
	CounterActor*		LinkedCounter;				// Counter that we are linked to
	CIwGameActorParticles* Particles;				// Particles actor
	ConnectActor*		Connector;					// Connecting actor
public:
	eCounterColour		getCounterColour() const				{ return CounterColour; }
	void				setFlying(bool flying)					{ IsFlying = flying; }
	bool				isFlying() const						{ return IsFlying; }
	bool				isDying() const							{ return IsDying; }
	int					getScore() const						{ return Score; }
	void				setSelected(bool selected);
	bool				isSelected() const						{ return Selected; }
	void				setLinkedCounter(CounterActor* counter)	{ LinkedCounter = counter; }
	CounterActor*		getLinkedCounter()						{ return LinkedCounter; }
	void				setConnector(ConnectActor* connector)	{ Connector = connector; }
	ConnectActor*		getConnector()							{ return Connector; }
	// Properties end

protected:
	void				HandleCollisions();
	CIwGameActor*		MouthActor;
public:
	~CounterActor();

	bool				Init(CIwGameImage* image, int width, int height);
	bool				Update(float dt);
	void				Kill(CIwGameActor* other);

	void				ResolveCollisions();
	void				NotifyCollision(CIwGameActor* other);

	void				OnTapped(int index, int x, int y, bool allow_bubble);

	// Implementation of IIwGameXomlClass interface
	bool			LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	static void			Spawn(int counter_num, int pos_x, int pos_y, eCounterColour colour);

	void				BreakLink();
	void				RemoveConnector();
};

//
//  CounterActorCreator - Creates an instance of a CounterActor object
//
class CounterActorCreator : public IIwGameXomlClassCreator
{
public:
	CounterActorCreator()
	{
		setClassName("counter");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CounterActor(); }
};

#endif // _COUNTER_ACTOR_H_
