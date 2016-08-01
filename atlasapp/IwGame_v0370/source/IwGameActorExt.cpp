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

#include "IwGameActorExt.h"
#include "IwGameScene.h"

//
//
//
// CIwGameActorConnector implementation
//
//
//

bool CIwGameActorConnector::setProperty(unsigned int element_name, CIwGameString& data, bool delta)
{
	if (CIwGameActor::setProperty(element_name, data, delta))
		return true;

	float float_pool[8];

	if (element_name == CIwGameXomlNames::TargetA_Hash)
	{
		CIwGameActor* actor = Scene->findActor(data.getHash());
		setTargetA(actor);
		return true;
	}
	else
	if (element_name == CIwGameXomlNames::TargetB_Hash)
	{
		CIwGameActor* actor = Scene->findActor(data.getHash());
		setTargetB(actor);
		return true;
	}
	else
	if (element_name == CIwGameXomlNames::OffsetA_Hash)
	{
		data.GetAsListOfFloat(float_pool);
		setOffsetA(float_pool[0], float_pool[1]);
		return true;
	}
	else
	if (element_name == CIwGameXomlNames::OffsetB_Hash)
	{
		data.GetAsListOfFloat(float_pool);
		setOffsetB(float_pool[0], float_pool[1]);
		return true;
	}

	return false;
}

bool CIwGameActorConnector::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Load the actor from XOML
	if (!CIwGameActorImage::LoadFromXoml(parent, load_children, node))
		return false;

	// Process InertActor specific attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::TargetA_Hash)
		{
			TargetA = Scene->findActor((*it)->GetValue().c_str());
			if (TargetA == NULL)
				CIwGameError::LogError("Warning: XOML - Could not find TargetA actor in scene - ", (*it)->GetValue().c_str());
		}
		else
		if (name_hash == CIwGameXomlNames::TargetB_Hash)
		{
			TargetB = Scene->findActor((*it)->GetValue().c_str());
			if (TargetB == NULL)
				CIwGameError::LogError("Warning: XOML - Could not find TargetB actor in scene - ", (*it)->GetValue().c_str());
		}
		else
		if (name_hash == CIwGameXomlNames::OffsetA_Hash)
		{
			if (!(*it)->GetValueAsPoint(OffsetA))
				CIwGameError::LogError("Warning: XOML - Invalid value for Vec2 ActorConnector::OffsetA");
		}
		else
		if (name_hash == CIwGameXomlNames::OffsetB_Hash)
		{
			if (!(*it)->GetValueAsPoint(OffsetB))
				CIwGameError::LogError("Warning: XOML - Invalid value for Vec2 ActorConnector::OffsetB");
		}
	}

	return true;
}

bool CIwGameActorConnector::Update(float dt)
{
	CIwFVec2 pos1 = CIwFVec2(0, 0); 
	CIwFVec2 pos2 = CIwFVec2(0, 0); 
	
	if (TargetA != NULL)
	{
		CIwVec2 offs = TargetA->TransformPoint(OffsetA.x * 8.0f, OffsetA.y * 8.0f);
		pos1.x = offs.x / 8.0f;
		pos1.y = offs.y / 8.0f;
	}
	else
		pos1 = CIwFVec2(OffsetA.x, OffsetA.y);
	if (TargetB != NULL)
	{
		CIwVec2 offs = TargetB->TransformPoint(OffsetB.x * 8.0f, OffsetB.y * 8.0f);
		pos2.x = offs.x / 8.0f;
		pos2.y = offs.y / 8.0f;
	}
	else
		pos2 = CIwFVec2(OffsetB.x, OffsetB.y);

	float dx = pos2.x - pos1.x;
	float dy = pos2.y - pos1.y;
	float d = dx * dx + dy * dy;
	d = sqrtf(d);
	if (d < 1.0f) d = 1.0f;
	if (d > 10000.0f)
		d = 1.0f;

	float theta = (atan2f(-dy, dx) * 180.0f) / M_PI;

	setScaleX(d / 100.0f);
	setAngle(theta);
	setPosition((pos1.x + pos2.x) / 2, (pos1.y + pos2.y) / 2);

	return CIwGameActorImage::Update(dt);
}


