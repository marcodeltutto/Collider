// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. You must ensure that this whole notice is present in all files and derivatives, so the reader knows its origin.
// If you use this engine in your product then please ensure that you credit IwGame's usage appropriately. Please see http://www.drmop.com/index.php/iwgame-engine/ for licensing details and support
//
//

#include "IwGameModifierExt.h"
#include "IwGameModifier.h"
#include "IwGameScene.h"
#include "IwGameActor.h"
#include "IwGameScript.h"

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
				actor->NotifyCollision(actor2);
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
			if (actor2 != NULL && (actor2->getType() & CollisionMask) == CollisionMask)
				actor->ProcessEventActions(CIwGameXomlNames::OnCollisionEnd_Hash);
		}
	}

	return true;
}

// 
// 
//
// CIwGameModScript implementation
//
//
//

bool CIwGameModScript::UpdateModifier(IIwGameXomlResource* target, float dt)
{
	CIwGameActor* actor = NULL;
	CIwGameScene* scene = NULL;
	
	if (target->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		actor = (CIwGameActor*)target;
	else
	if (target->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)target;

	CIwGameString *p2 = NULL;
	CIwGameString *p3 = NULL;
	CIwGameString *p4 = NULL;

	if (!Params[1].IsEmpty())
		p2 = &Params[1];
	if (!Params[2].IsEmpty())
		p3 = &Params[2];
	if (!Params[3].IsEmpty())
		p4 = &Params[3];

	if (actor != NULL)
		actor->getScene()->getScriptEngine()->CallFunction(actor, &Params[0], p2, p3, p4);
	else
		scene->getScriptEngine()->CallFunction(scene, &Params[0], p2, p3, p4);

	return true;
}




