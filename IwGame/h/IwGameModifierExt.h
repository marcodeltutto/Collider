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

#if !defined(_IW_GAME_MODIFIER_EXT_H_)
#define _IW_GAME_MODIFIER_EXT_H_

#include "IwGameUtil.h"
#include "IwGameModifier.h"
#include "IwGameActor.h"

//
//
//  CIwGameModFollowHeading - Modifier that calculates actors velocity based on heading and speed
//
//	Parameter1 - Heading in degrees (float)
//	Parameter2 - Speed (float)
//
//
class CIwGameModFollowHeading : public IIwGameModifier
{
	// Properties
protected:
	float		Heading;				// Direction
	float		Speed;					// Speed of travel

public:
	void		setHeading(float angle)				{ Heading = angle; }
	float		getHeading() const					{ return Heading; }
	void		setSpeed(float speed)				{ Speed = speed; }
	float		getSpeed() const					{ return Speed; }
	// Properties End

public:
	void InitModifier(IIwGameXomlResource* target)
	{
		Heading = Params[0].GetAsFloat();
		Speed = Params[1].GetAsFloat();
	}
	void ReleaseModifier(IIwGameXomlResource* target)
	{
	}
	bool UpdateModifier(IIwGameXomlResource* target, float dt)
	{
		CIwGameActor* actor = (CIwGameActor*)target;
		float angle = (Heading * 3.1415927f) / 180.0f;

		actor->setVelocity(cosf(Heading) * Speed, sinf(Heading) * Speed);

		return true;
	}
};
class CIwGameModFollowHeadingCreator : public IIwGameModifierCreator
{
public:
	CIwGameModFollowHeadingCreator()
	{
		setClassName("iw_followheading");
	}
	IIwGameModifier* CreateInstance() { return new CIwGameModFollowHeading(); }
};

//
//
//  CIwGameModCollisionNotify - Modifier that adds Box2D collision notifications
//
//	Parameter1 - Collision mask, any actors that pass the mask are potential colliders
//
//
class CIwGameModCollisionNotify : public IIwGameModifier
{
	// Properties
protected:
	int			CollisionMask;					// Colision  mask, any actors that pass the mask are potential colliders

public:
	void		setCollisionMask(int mask)			{ CollisionMask = mask; }
	int			getCollisionMask() const			{ return CollisionMask; }
	// Properties End

public:
	void InitModifier(IIwGameXomlResource* target)
	{
		if (Params[0].IsEmpty())
			CollisionMask = 0;
		else
			CollisionMask = Params[0].GetAsInt();
	}
	void ReleaseModifier(IIwGameXomlResource* target)
	{
	}
	bool UpdateModifier(IIwGameXomlResource* target, float dt);
};
class CIwGameModCollisionNotifyCreator : public IIwGameModifierCreator
{
public:
	CIwGameModCollisionNotifyCreator()
	{
		setClassName("iw_notifycollision");
	}
	IIwGameModifier* CreateInstance() { return new CIwGameModCollisionNotify(); }
};

//
//
//  CIwGameModScript - Modifier that calls a script to modify the object
//
//	Parameter1 - Script function name, any actors that pass the mask are potential colliders
//
//
class CIwGameModScript : public IIwGameModifier
{
	// Properties
protected:
	int			CollisionMask;					// Colision  mask, any actors that pass the mask are potential colliders

public:
	void		setCollisionMask(int mask)			{ CollisionMask = mask; }
	int			getCollisionMask() const			{ return CollisionMask; }
	// Properties End

public:
	void InitModifier(IIwGameXomlResource* target)
	{
	}
	void ReleaseModifier(IIwGameXomlResource* target)
	{
	}
	bool UpdateModifier(IIwGameXomlResource* target, float dt);
};
class CIwGameModScriptCreator : public IIwGameModifierCreator
{
public:
	CIwGameModScriptCreator()
	{
		setClassName("iw_callscript");
	}
	IIwGameModifier* CreateInstance() { return new CIwGameModScript(); }
};




#endif	// _IW_GAME_MODIFIER_EXT_H_
