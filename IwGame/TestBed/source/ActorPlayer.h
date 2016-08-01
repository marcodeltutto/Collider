#if !defined(_ACTOR_PLAYER_H_)
#define _ACTOR_PLAYER_H_

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
// ActorPlayer - A player character actor
//
//
class ActorPlayer : public CIwGameActorImage
{
public:

protected:
	// Properties
	CIwGameTimer	FadeTimer;
	bool			Immortal;
public:
	void			setImmortal(bool immortal)	{ Immortal = immortal; }
	// Properties end

	ActorPlayer() : CIwGameActorImage(), Immortal(false) {}
	virtual			~ActorPlayer();
	
	bool			Init(CIwGameImage* image, int width, int height);
	void			Reset();
	bool			Update(float dt);
	bool			UpdateVisual();
	void			ResolveCollisions();
	void			NotifyCollision(CIwGameActor* other);

	static ActorPlayer* Create(int x, int y, CIwGameScene* scene, CIwGameImage* image, CIwGameAnim* anim);

	// Implementation of IIwGameXomlClass interface
	bool			LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

#endif // _ACTOR_PLAYER_H_
