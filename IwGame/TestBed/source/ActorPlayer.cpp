#include "IwGameActor.h"
#include "IwGameScene.h"

#include "ActorPlayer.h"


//
//
//
// ActorPlayer implementation
//
//
//

ActorPlayer* ActorPlayer::Create(int x, int y, CIwGameScene* scene, CIwGameImage* image, CIwGameAnim* anim)
{
	// Create a new instance
	ActorPlayer* actor = new ActorPlayer();
	if (actor == NULL)
		return NULL;

	// Create an animation timeline to hold our image animation
/*	CIwGameAnimTimeline* timeline = new CIwGameAnimTimeline();

	// Create and set up our face animation
	CIwGameAnimInstance* face_anim = new CIwGameAnimInstance();
	face_anim->setAnimation(anim);
	face_anim->setTarget(actor, "SrcRect");
	timeline->addAnimation(face_anim);
	timeline->play();
*/
	// Create player actor
	actor->setScene(scene);
	actor->Init(image, 36, 40);
//	actor->setTimeline(timeline);
	actor->setPosition((float)x, (float)y);

	// Add player actor to the scene
	scene->addActor(actor);

	return actor;
}


ActorPlayer::~ActorPlayer()
{
}

bool ActorPlayer::Init(CIwGameImage* image, int width, int height)
{
	CIwGameActorImage::Init(image, width, height);

	FadeTimer.setDuration(1000);
	Velocity.x = IwRandMinMax(-100, 100) / 20.0f;
	Velocity.y = IwRandMinMax(-100, 100) / 20.0f;
	AngularVelocity = IwRandMinMax(-100, 100) / 20.0f;

	return true;
}

void ActorPlayer::Reset()
{
	CIwGameActor::Reset();
}

bool ActorPlayer::Update(float dt)
{
	if (Immortal)
	{
		Velocity.x = 0;
		Velocity.y = 0;
		AngularVelocity = 0;
	}
	else
	{
		// If fade timer has timed out then delete this actor
		if (FadeTimer.HasTimedOut())
		{
			return false;	// returning false tells the scene that we no need to be removed from the scene
		}

		// Calculate our opacity from time left on fade timer
		int opacity = (int)FadeTimer.GetTimeLeft() / 2;
		if (opacity > 255) opacity = 255;
		Colour.a = opacity;
	}

	return CIwGameActorImage::Update(dt);
}

bool ActorPlayer::UpdateVisual()
{
	return CIwGameActorImage::UpdateVisual();
}

void ActorPlayer::ResolveCollisions()
{
	// Walk the scenes actors
	for (CIwGameScene::_Iterator it = Scene->begin(); it != Scene->end(); ++it)
	{
		// Only test collision against ball type actors
		if ((*it)->getType() == 1)
		{
			// Chedck for physical collision
			if (CheckCollision(*it))
			{
				// Norify ourselves that we collided with ball actor
				NotifyCollision(*it);
				// Notify ball actor that we collided with it
				(*it)->NotifyCollision(this);
			}
		}
	}
}

void ActorPlayer::NotifyCollision(CIwGameActor* other)
{
}

bool ActorPlayer::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	return false;
}
