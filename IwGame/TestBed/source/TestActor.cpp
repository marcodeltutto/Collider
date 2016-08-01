#include "IwGameActor.h"
#include "IwGameScene.h"

#include "TestActor.h"


//
//
//
// TestActor implementation
//
//
//


TestActor::~TestActor()
{
}

bool TestActor::Init(CIwGameImage* image, int width, int height)
{
	CIwGameActorImage::Init(image, width, height);

	return true;
}

void TestActor::Reset()
{
	CIwGameActor::Reset();
}

bool TestActor::Update(float dt)
{
	return CIwGameActorImage::Update(dt);
}

bool TestActor::UpdateVisual()
{
	return CIwGameActorImage::UpdateVisual();
}

void TestActor::ResolveCollisions()
{
}

void TestActor::NotifyCollision(CIwGameActor* other)
{
}

void TestActor::OnTapped(int index, int x, int y, bool allow_bubble)
{
	CIwGameActor::OnTapped(index, x, y, allow_bubble);
}

void TestActor::OnBeginTouch(int index, int x, int y, bool allow_bubble)
{
	CIwGameActor::OnBeginTouch(index, x, y, allow_bubble);
	Colour.a = 0x80;
}

void TestActor::OnEndTouch(int index, int x, int y, bool allow_bubble)
{
	CIwGameActor::OnEndTouch(index, x, y, allow_bubble);
	Colour.a = 0xff;
}


bool TestActor::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Load the actor from XOML
	if (!CIwGameActorImage::LoadFromXoml(parent, load_children, node))
		return false;

	// Process TestActor specific attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();
	}

	return true;
}
