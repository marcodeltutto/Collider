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
	if (Velocity.x > -0.01f && Velocity.x < 0.01f)
		Velocity.x = 0;
	if (Velocity.y > -0.01f && Velocity.y < 0.01f)
		Velocity.y = 0;

	if (old_pos.x != Position.x || old_pos.y != Position.y)
		TransformDirty = true;

	if (TransformDirty)
	{
		// Update camera transform
		Transform.SetRot(Angle * 0.017453293f);
		Transform.ScaleRot(Scale);
		CIwFVec2 pos(-Position.x, -Position.y);
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

	bool ret = false;
	if (scene != NULL)
		ret = scene->getResourceManager()->addResource(this);
	else
		ret = IW_GAME_GLOBAL_RESOURCES->getResourceManager()->addResource(this);

	if (!ret)
		return false;

	Update(0);

	return true;
}

