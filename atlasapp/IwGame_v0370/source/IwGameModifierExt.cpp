// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. The only restriction to its usage is that this header must remain intact and visible in all IwGame engine files.
// If you use this engine in your product, whilst it is not mandatory, a mention of the IwGame engine would be nice or a quick email to let us know where it is being used.
//
//

#include "IwGameModifierExt.h"
#include "IwGameModifier.h"
#include "IwGameScene.h"
#include "IwGameActor.h"

// 
// 
//
// CIwGameModCollisionNotiify implementation
//
//
//

bool CIwGameModCollisionNotify::UpdateModifier(IIwGameXomlResource* target, float dt)
{
	if (target->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
		return true;

	CIwGameActor* actor = (CIwGameActor*)target;
	CIwGameBox2dBody* body = actor->getBox2dBody();
	if (body == NULL)
		return false;

	// Check begin contacts
	CIwGameSlotArray<CIwGameBox2dCollidable*>& started = body->getCollisionsStarted();
	for (int t = 0; t < started.getSize(); t++)
	{
		CIwGameBox2dCollidable* collision = started.element_at(t);
		if (collision != NULL)
		{
			CIwGameActor* actor2 = (CIwGameActor*)collision->getUserData();
			if (actor2 != NULL && (actor2->getType() & CollisionMask) == CollisionMask)
			{
				actor->ProcessEventActions(CIwGameXomlNames::OnCollisionStart_Hash);
				actor->NotifyCollision(actor);
			}
		}
	}

	// Check end contacts
	CIwGameSlotArray<CIwGameBox2dCollidable*>& ended = body->getCollisionsEnded();
	for (int t = 0; t < ended.getSize(); t++)
	{
		CIwGameBox2dCollidable* collision = ended.element_at(t);
		if (collision != NULL)
		{
			CIwGameActor* actor2 = (CIwGameActor*)collision->getUserData();
			if (actor2 != NULL)
				actor->ProcessEventActions(CIwGameXomlNames::OnCollisionEnd_Hash);
		}
	}

	return true;
}




