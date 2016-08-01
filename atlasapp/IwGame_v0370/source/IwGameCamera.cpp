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

#include "IwGameCamera.h"
#include "IwGameResources.h"
#include "IwGameScene.h"
#include "IwGameActor.h"

//
//
//
//
// CIwGameCamera implementation
//
//
//
//

void CIwGameCamera::Update(float dt)
{
	CIwFVec2 old_pos = Position;
	// Apply camera velocity
	Position += Velocity * dt;
	Velocity.x *= VelocityDamping.x;
	Velocity.y *= VelocityDamping.y;
	if (Velocity.x > -0.1f && Velocity.x < 0.1f)
		Velocity.x = 0;
	if (Velocity.y > -0.1f && Velocity.y < 0.1f)
		Velocity.y = 0;

	if (old_pos.x != Position.x || old_pos.y != Position.y)
		TransformDirty = true;

	if (TransformDirty)
	{
		// Update camera transform
		Transform.SetRot((iwangle)((Angle * IW_ANGLE_2PI) / 360.0f));
		Transform.ScaleRot((iwfixed)(Scale * IW_GEOM_ONE));
		CIwSVec2 pos((int)(-Position.x * 8.0f), (int)(-Position.y * 8.0f));
		Transform.SetTrans(pos);
	}
}

bool CIwGameCamera::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Process Actor attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
			setName((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Position_Hash)
		{
			if (!(*it)->GetValueAsPoint(Position))
			{
				CIwGameError::LogError("Warning: XOML - Invalid value for Camera::Position");
			}
		}
		else
		if (name_hash == CIwGameXomlNames::Angle_Hash)
			Angle = (*it)->GetValueAsFloat();
		else
		if (name_hash == CIwGameXomlNames::Scale_Hash)
			Scale = (*it)->GetValueAsFloat();
		else
		if (name_hash == CIwGameXomlNames::Target_Hash)
			setTarget((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::VelocityDamping_Hash)
		{
			if (!(*it)->GetValueAsPoint(VelocityDamping))
				CIwGameError::LogError("Warning: XOML - Invalid value for Camera::VelocityDamping");
		}
		else
		if (name_hash == CIwGameXomlNames::TouchPanX_Hash)
			setTouchPanX((*it)->GetValueAsBool());
		else
		if (name_hash == CIwGameXomlNames::TouchPanY_Hash)
			setTouchPanY((*it)->GetValueAsBool());
		else
		if (name_hash == CIwGameXomlNames::IgnoreActors_Hash)
			setIgnoreActors((*it)->GetValueAsBool());
	}

	// If we are declared inside a scene then shape is local to the scene
	CIwGameScene* scene = NULL;
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		scene = ((CIwGameActor*)parent)->getScene();
	else
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	if (scene != NULL)
		scene->getResourceManager()->addResource(this);
	else
		IW_GAME_GLOBAL_RESOURCES->getResourceManager()->addResource(this);

	Update(0);

	return true;
}

