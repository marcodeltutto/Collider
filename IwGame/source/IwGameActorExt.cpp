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
		CIwFVec2 offs = TargetA->TransformPoint(OffsetA.x, OffsetA.y);
		pos1.x = offs.x;
		pos1.y = offs.y;
	}
	else
		pos1 = CIwFVec2(OffsetA.x, OffsetA.y);
	if (TargetB != NULL)
	{
		CIwFVec2 offs = TargetB->TransformPoint(OffsetB.x, OffsetB.y);
		pos2.x = offs.x;
		pos2.y = offs.y;
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

	float theta = (atan2f(-dy, dx) * 180.0f) / (float)M_PI;

	setScaleX(d / 100.0f);
	setAngle(theta);
	setPosition((pos1.x + pos2.x) / 2.0f, (pos1.y + pos2.y) / 2.0f);

	return CIwGameActorImage::Update(dt);
}


