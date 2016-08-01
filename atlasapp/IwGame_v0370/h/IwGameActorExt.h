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

#if !defined(_IW_GAME_ACTOR_EXT_H_)
#define _IW_GAME_ACTOR_EXT_H_

#include "IwGameActor.h"

//
//
//
//
// CIwGameActorConnector - An actor that connects two points or two actors together like a string
//
//
//
//
class CIwGameActorConnector : public CIwGameActorImage
{
public:

protected:
	// Properties
	CIwGameActor*			TargetA;		// Target actor A
	CIwGameActor*			TargetB;		// Target actor B
	CIwFVec2				OffsetA;		// Target A offset
	CIwFVec2				OffsetB;		// Target B offset
public:
	void					setTargetA(CIwGameActor* target)		{ TargetA = target; }
	CIwGameActor*			getTargetA()							{ return TargetA; }
	void					setTargetB(CIwGameActor* target)		{ TargetB = target; }
	CIwGameActor*			getTargetB()							{ return TargetB; }
	void					setOffsetA(float x, float y)			{ OffsetA.x = x; OffsetA.y = y; }
	CIwFVec2				getOffsetA() const						{ return OffsetA; }
	void					setOffsetB(float x, float y)			{ OffsetB.x = x; OffsetB.y = y; }
	CIwFVec2				getOffsetB() const						{ return OffsetB; }
	bool					setProperty(unsigned int element_name, CIwGameString& data, bool delta);
	// Properties end

public:
	CIwGameActorConnector() : CIwGameActorImage(), TargetA(NULL), TargetB(NULL), OffsetA(0, 0), OffsetB(0, 0) {  setActualClassType("actorconnector"); }

	bool					Update(float dt);

	// Implementation of IIwGameXomlClass interface
	bool					LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  CIwGameActorConnectorCreator - Creates an instance of a CIwGameActorConnctor object
//
class CIwGameActorConnectorCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameActorConnectorCreator()
	{
		setClassName("actorconnector");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameActorConnector(); }
};



#endif // _IW_GAME_ACTOR_EXT_H_
