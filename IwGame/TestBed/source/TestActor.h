#if !defined(_TEST_ACTOR_H_)
#define _TEST_ACTOR_H_

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
// TestActor - A test actor
//
//
class TestActor : public CIwGameActorImage
{
public:

protected:
	// Properties
public:
	// Properties end

	TestActor() : CIwGameActorImage() {}
	virtual			~TestActor();
	
	bool			Init(CIwGameImage* image, int width, int height);
	void			Reset();
	bool			Update(float dt);
	bool			UpdateVisual();
	void			ResolveCollisions();
	void			NotifyCollision(CIwGameActor* other);

	void			OnTapped(int index, int x, int y, bool allow_bubble);
	void			OnBeginTouch(int index, int x, int y, bool allow_bubble);
	void			OnEndTouch(int index, int x, int y, bool allow_bubble);

	// Implementation of IIwGameXomlClass interface
	bool			LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  TestActorCreator - Creates an instance of a TestPlayer object
//
class TestActorCreator : public IIwGameXomlClassCreator
{
public:
	TestActorCreator()
	{
		setClassName("testactor");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new TestActor(); }
};

#endif // _TEST_ACTOR_H_
