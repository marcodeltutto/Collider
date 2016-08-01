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

#include "IwGameBox2d.h"
#include "IwGameActor.h"
#include "IwGameScene.h"
#include "IwGameResources.h"

#if defined(IW_GAME_BOX2D)

// Glopbally defined collision collback used to marshal cross actor collision calls
CIwGameBox2dCcollisionCallback	g_CIwGameBox2dCcollisionCallback;

//
//
//
//
// CIwGameBox2dCcollisionCallback - Box2D collision callback
//
// These methods basically notify each actor that a collision has either started or ended
//
//
//
//
void CIwGameBox2dCcollisionCallback::BeginContact(b2Contact* contact)
{
	CIwGameBox2dBody* bodyUserDataA = (CIwGameBox2dBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	CIwGameBox2dBody* bodyUserDataB = (CIwGameBox2dBody*)contact->GetFixtureB()->GetBody()->GetUserData();
	if (bodyUserDataA != NULL && bodyUserDataB != NULL)
	{
		bodyUserDataA->addCollisionStarted(bodyUserDataB);
		bodyUserDataB->addCollisionStarted(bodyUserDataA);
	}
}
 
void CIwGameBox2dCcollisionCallback::EndContact(b2Contact* contact)
{
	CIwGameBox2dBody* bodyUserDataA = (CIwGameBox2dBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	CIwGameBox2dBody* bodyUserDataB = (CIwGameBox2dBody*)contact->GetFixtureB()->GetBody()->GetUserData();
	if (bodyUserDataA != NULL && bodyUserDataB != NULL)
	{
		bodyUserDataA->addCollisionEnded(bodyUserDataB);
		bodyUserDataB->addCollisionEnded(bodyUserDataA);
	}
}


//
//
//
//
// CIwGameBox2dMaterial implementation
//
//
//
//
bool CIwGameBox2dMaterial::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Process material attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
		{
			setName((*it)->GetValue().c_str());
		}
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Type_Hash)
		{
			unsigned int type_hash = (*it)->GetValue().getHash();
			if (type_hash == CIwGameXomlNames::static_Hash)
				setBodyType(b2_staticBody);
			else
			if (type_hash == CIwGameXomlNames::dynamic_Hash)
				setBodyType(b2_dynamicBody);
			else
			if (type_hash == CIwGameXomlNames::kinematic_Hash)
				setBodyType(b2_kinematicBody);
			else
			{
				CIwGameError::LogError("Warning: XOML - Invalid body type for box2d material - ", (*it)->GetValue().c_str());
			}
		}
		else
		if (name_hash == CIwGameXomlNames::Density_Hash)
		{
			setDensity((*it)->GetValueAsFloat());
		}
		else
		if (name_hash == CIwGameXomlNames::Friction_Hash)
		{
			setFriction((*it)->GetValueAsFloat());
		}
		else
		if (name_hash == CIwGameXomlNames::Restitution_Hash)
		{
			setRestitution((*it)->GetValueAsFloat());
		}
		else
		if (name_hash == CIwGameXomlNames::IsBullet_Hash)
		{
			setBullet((*it)->GetValueAsBool());
		}
		else
		if (name_hash == CIwGameXomlNames::FixedRotation_Hash)
		{
			setFixedRotation((*it)->GetValueAsBool());
		}
		else
		if (name_hash == CIwGameXomlNames::GravityScale_Hash)
		{
			setGravityScale((*it)->GetValueAsFloat());
		}
	}

	// If we are declared inside a scene then material is local to the scene
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

	return true;
}

//
//
//
//
// CIwGameBox2dWorld implementation
//
//
//
//
CIwGameBox2dWorld::~CIwGameBox2dWorld()
{
	SAFE_DELETE(World);
}

void CIwGameBox2dWorld::setGravity(float x, float y)
{
	Gravity.x = x; Gravity.y = y;
	if (World != NULL)
		World->SetGravity(b2Vec2(Gravity.x, Gravity.y));
}

void CIwGameBox2dWorld::InitWorld(bool doSleep)
{
	// create Box2D world
	World = new b2World(b2Vec2(Gravity.x, Gravity.y), doSleep);

	// Set up contact collision listener callback (this gets called whever a collision occurs between two bodies)
	World->SetContactListener(&g_CIwGameBox2dCcollisionCallback);
}

void CIwGameBox2dWorld::UpdateWorld(float dt)
{
	if (World != NULL && WorldScale.x != 0)
	{
		// Update the physics engine
		World->Step((dt * FRAME_SPEED_LOCK_MS) / 1000.0f, VelocityIterations, PositionIterations);
	}
}



//
//
//
//
// CIwGameBox2dBody implementation
//
//
//
//

CIwGameBox2dBody::~CIwGameBox2dBody()
{
	ReleaseBody();
}

void CIwGameBox2dBody::setBody(b2Body* body)
{
	// Destroy existing body
	if (Body != NULL)
	{
		World->getWorld()->DestroyBody(Body);
		Body = NULL;
	}

	// Set new body
	Body = body;

	// Reset fixture
	Fixture = NULL;
}

void CIwGameBox2dBody::setBodyMaterial(CIwGameBox2dMaterial* mat)
{
	BodyMaterial = mat;
	if (Fixture != NULL)
	{
		Fixture->SetDensity(mat->getDensity());
		Fixture->SetFriction(mat->getFriction());
		Fixture->SetRestitution(mat->getRestitution());

		if (Body != NULL)
		{
			Body->SetType(mat->getBodyType());
			Body->SetBullet(mat->isBullet());
			Body->SetFixedRotation(mat->isFixedRotation());
			if (mat->getGravityScale() < 1000000.0f)
				Body->SetGravityScale(mat->getGravityScale());
		}
	}
}

void CIwGameBox2dBody::setFixture(CIwGameShape* body_shape, CIwGameBox2dMaterial* body_mat, float com_x, float com_y)
{
	// Destory old fixture
	if (Fixture != NULL)
	{
		if (Body != NULL)
		{
			Body->DestroyFixture(Fixture);
		}
	}

	// Add basic fixture to the body
	b2Vec2 cm;
	cm.x = World->PixelToWorldX(com_x);
	cm.y = World->PixelToWorldX(com_y);
	b2FixtureDef fixture1;
	b2PolygonShape poly_shape;
	b2CircleShape circ_shape;

	switch (body_shape->getShape()->ShapeType)
	{
	case CIwGameGeomShape::ST_Box:
		{
			CIwGameGeomShapeBox* shp = (CIwGameGeomShapeBox*)body_shape->getShape();
			fixture1.shape = &poly_shape;
			poly_shape.SetAsBox(World->PixelToWorldX(shp->Width) / 2.0f, World->PixelToWorldY(shp->Height) / 2.0f, cm, 0);
		}
		break;
	case CIwGameGeomShape::ST_Circle:
		{
			CIwGameGeomShapeCircle* shp = (CIwGameGeomShapeCircle*)body_shape->getShape();
			fixture1.shape = &circ_shape;
			circ_shape.m_radius = World->PixelToWorldX(shp->Radius);
			circ_shape.m_p.x = cm.x;
			circ_shape.m_p.y = cm.y;
		}
		break;
	case CIwGameGeomShape::ST_Polygon:
		{
			CIwGameGeomShapePolygon* shp = (CIwGameGeomShapePolygon*)body_shape->getShape();
			fixture1.shape = &poly_shape;
			poly_shape.m_vertexCount = shp->NumVertices;
			CIwFVec2* points = shp->Vertices;

			b2Vec2 verts[8];
			for (int t = 0; t < shp->NumVertices; t++)
			{
				verts[t].x = World->PixelToWorldX(points->x) + cm.x;
				verts[t].y = World->PixelToWorldY(points->y) + cm.y;
				points++;
			}
			poly_shape.Set(verts, shp->NumVertices);
		}
		break;
	}

	if (body_mat != NULL)
	{
		fixture1.density = body_mat->getDensity();
		fixture1.friction = body_mat->getFriction();
		fixture1.restitution = body_mat->getRestitution();
	}
	else
	{
		fixture1.density = 1.0f;
		fixture1.friction = 1.0f;
		fixture1.restitution = 0.1f;
	}
	Fixture = Body->CreateFixture(&fixture1);
	BodyShape = body_shape;
	BodyMaterial = body_mat;
}

void CIwGameBox2dBody::setCollisionFlags(int category_flags, int mask_flags, int collision_group)
{
	if (Fixture != NULL)
	{
		b2Filter f;
		f.categoryBits = category_flags;
		f.maskBits = mask_flags;
		f.groupIndex = collision_group;

		Fixture->SetFilterData(f);
		CollisionFlags = (category_flags << 16) | (mask_flags << 8) | (collision_group);
	}
}

IIwGameBox2dJoint* CIwGameBox2dBody::getJoint(int index)
{
	if (index < 0 || index > Joints.size())
		return false;

	return Joints.element_at(index);
}

IIwGameBox2dJoint* CIwGameBox2dBody::findJoint(unsigned int name_hash)
{
	for (CIwList<IIwGameBox2dJoint*>::iterator it = Joints.begin(); it != Joints.end(); ++it)
	{
		if ((*it)->NameHash == name_hash)
			return *it;
	}

	return NULL;
}

void CIwGameBox2dBody::removeJoint(IIwGameBox2dJoint* joint)
{
	Body->GetWorld()->DestroyJoint((b2Joint*)joint->getJoint());

	for (CIwList<IIwGameBox2dJoint*>::iterator it = Joints.begin(); it != Joints.end(); ++it)
	{
		if (*it == joint)
		{
			delete *it;
			Joints.erase(it);
			break;
		}
	}
}

int	CIwGameBox2dBody::getCollisionCategory() const
{
	if (Fixture == NULL)
		return 0;

	return Fixture->GetFilterData().categoryBits;
}

int	CIwGameBox2dBody::getCollisionMask() const
{
	if (Fixture == NULL)
		return 0;

	return Fixture->GetFilterData().maskBits;
}

int	CIwGameBox2dBody::getCollisionGroup() const
{
	if (Fixture == NULL)
		return 0;

	return Fixture->GetFilterData().groupIndex;
}

void CIwGameBox2dBody::InitBody(CIwGameBox2dWorld* world, CIwGameShape* body_shape, CIwGameBox2dMaterial* body_mat, CIwFVec2* pos, float angle, float com_x, float com_y)
{
	// Set the box2d world
	World = world;

	b2BodyDef body_def;
	if (body_mat != NULL)
	{
		body_def.type = body_mat->getBodyType();
		body_def.position.Set(World->PixelToWorldX(pos->x), World->PixelToWorldX(pos->y));
		body_def.angle = (-angle * 3.1415927f) / 180.0f;			// Degrees to radians
		Body = World->getWorld()->CreateBody(&body_def);	
		Body->SetUserData((void*)this);
		Body->SetBullet(body_mat->isBullet());
		Body->SetFixedRotation(body_mat->isFixedRotation());
		if (body_mat->getGravityScale() < 1000000.0f)
			Body->SetGravityScale(body_mat->getGravityScale());
	}
	else
	{
		body_def.type = b2_staticBody;
		body_def.position.Set(World->PixelToWorldX(pos->x), World->PixelToWorldX(pos->y));
		body_def.angle = (-angle * 3.1415927f) / 180.0f;			// Degrees to radians
		Body = World->getWorld()->CreateBody(&body_def);	
		Body->SetUserData((void*)this);
	}

	setFixture(body_shape, body_mat, com_x, com_y);
}

void CIwGameBox2dBody::ReleaseBody()
{
	// Destroy the body
	if (Body != NULL)
	{
		World->getWorld()->DestroyBody(Body);
		Body = NULL;
	}

	// Remove joints
	for (CIwList<IIwGameBox2dJoint*>::iterator it = Joints.begin(); it != Joints.end(); ++it)
	{
		delete *it;
	}
	Joints.clear();
}

void CIwGameBox2dBody::ApplyForce(float force_x, float force_y, float world_pos_x, float world_pos_y)
{
	if (Body != NULL)
		Body->ApplyForce(b2Vec2(force_x, force_y), b2Vec2(world_pos_x, world_pos_y));
}
void CIwGameBox2dBody::ApplyForceToCenter(float force_x, float force_y)
{
	if (Body != NULL)
		Body->ApplyForceToCenter(b2Vec2(force_x, force_y));
}
void CIwGameBox2dBody::ApplyTorque(float torque)
{
	if (Body != NULL)
		Body->ApplyTorque(torque);
}
void CIwGameBox2dBody::ApplyLinearImpulse(float impulse_x, float impulse_y, float world_pos_x, float world_pos_y)
{
	if (Body != NULL)
		Body->ApplyLinearImpulse(b2Vec2(impulse_x, impulse_y), b2Vec2(world_pos_x, world_pos_y));
}
void CIwGameBox2dBody::ApplyAngularImpulse(float impulse)
{
	if (Body != NULL)
		Body->ApplyAngularImpulse(impulse);
}

//
//
//
//
// CIwGameBox2dJoint implementation
//
//
//
//

void CIwGameBox2dJointDistance::Create(CIwGameBox2dWorld* world)
{
	b2DistanceJointDef jointDef;

	jointDef.bodyA = BodyA->getBody();
	jointDef.bodyB = BodyB->getBody();
	jointDef.localAnchorA = BodyA->getBody()->GetLocalPoint(AnchorA);
	jointDef.localAnchorB = BodyB->getBody()->GetLocalPoint(AnchorB);
	jointDef.collideConnected = SelfCollide;
	if (Length > 0)
		jointDef.length = Length;
	else
		jointDef.length = (AnchorB - AnchorA).Length();
	if (Frequency >= 0)
		jointDef.frequencyHz = Frequency;
	if (Damping >= 0)
		jointDef.dampingRatio = Damping;

	Joint = (b2DistanceJoint*)world->getWorld()->CreateJoint(&jointDef);
}

void CIwGameBox2dJointRevolute::Create(CIwGameBox2dWorld* world)
{
	b2RevoluteJointDef jointDef;

	jointDef.bodyA = BodyA->getBody();
	jointDef.bodyB = BodyB->getBody();
	jointDef.localAnchorA = BodyA->getBody()->GetLocalPoint(AnchorA);
	jointDef.localAnchorB = BodyB->getBody()->GetLocalPoint(AnchorB);
	if (ReferenceAngle == 999999.0f)
		jointDef.referenceAngle = BodyB->getBody()->GetAngle() - BodyA->getBody()->GetAngle();
	else
		jointDef.referenceAngle = (ReferenceAngle * 3.1415927f) / 180.0f;			// Degrees to radians;
	jointDef.collideConnected = SelfCollide;

	if (LimitJoint)
	{
		jointDef.enableLimit = true;
		jointDef.lowerAngle = (LowerAngle * 3.1415927f) / 180.0f;			// Degrees to radians
		jointDef.upperAngle = (UpperAngle * 3.1415927f) / 180.0f;			// Degrees to radians
	}
	if (MotorEnabled)
	{
		jointDef.enableMotor = true;
		jointDef.motorSpeed = MotorSpeed;
		jointDef.maxMotorTorque = MaxMotorTorque;
	}

	Joint = (b2RevoluteJoint*)world->getWorld()->CreateJoint(&jointDef);
}

void CIwGameBox2dJointPrismatic::Create(CIwGameBox2dWorld* world)
{
	b2PrismaticJointDef jointDef;

	jointDef.bodyA = BodyA->getBody();
	jointDef.bodyB = BodyB->getBody();
	jointDef.localAnchorA = BodyA->getBody()->GetLocalPoint(AnchorA);
	jointDef.localAnchorB = BodyB->getBody()->GetLocalPoint(AnchorB);
	if (ReferenceAngle == 999999.0f)
		jointDef.referenceAngle = BodyB->getBody()->GetAngle() - BodyA->getBody()->GetAngle();
	else
		jointDef.referenceAngle = (ReferenceAngle * 3.1415927f) / 180.0f;			// Degrees to radians;
	jointDef.collideConnected = SelfCollide;
	jointDef.localAxisA = BodyA->getBody()->GetLocalVector(Axis);

	if (LimitJoint)
	{
		jointDef.enableLimit = true;
		jointDef.lowerTranslation = LowerTranslation;
		jointDef.upperTranslation = UpperTranslation;
	}
	if (MotorEnabled)
	{
		jointDef.enableMotor = true;
		jointDef.motorSpeed = MotorSpeed;
		jointDef.maxMotorForce = MaxMotorForce;
	}

	Joint = (b2PrismaticJoint*)world->getWorld()->CreateJoint(&jointDef);
}

void CIwGameBox2dJointPulley::Create(CIwGameBox2dWorld* world)
{
	b2PulleyJointDef jointDef;

	jointDef.bodyA = BodyA->getBody();
	jointDef.bodyB = BodyB->getBody();
	jointDef.localAnchorA = BodyA->getBody()->GetLocalPoint(AnchorA);
	jointDef.localAnchorB = BodyB->getBody()->GetLocalPoint(AnchorB);
	jointDef.groundAnchorA = GroundAnchorA;
	jointDef.groundAnchorB = GroundAnchorB;
	if (LengthA > 0)
		jointDef.lengthA = LengthA;
	else
		jointDef.lengthA = (AnchorA - GroundAnchorA).Length();
	if (LengthB > 0)
		jointDef.lengthB = LengthB;
	else
		jointDef.lengthB = (AnchorB - GroundAnchorB).Length();
	jointDef.ratio = Ratio;
	jointDef.collideConnected = SelfCollide;

	Joint = (b2PulleyJoint*)world->getWorld()->CreateJoint(&jointDef);
}

void CIwGameBox2dJointWheel::Create(CIwGameBox2dWorld* world)
{
	b2WheelJointDef jointDef;

	jointDef.bodyA = BodyA->getBody();
	jointDef.bodyB = BodyB->getBody();
	jointDef.localAnchorA = BodyA->getBody()->GetLocalPoint(AnchorA);
	jointDef.localAnchorB = BodyB->getBody()->GetLocalPoint(AnchorB);
	jointDef.collideConnected = SelfCollide;
	jointDef.dampingRatio = Damping;
	jointDef.frequencyHz = Frequency;
	jointDef.localAxisA = BodyA->getBody()->GetLocalVector(Axis);

	if (MotorEnabled)
	{
		jointDef.enableMotor = true;
		jointDef.motorSpeed = MotorSpeed;
		jointDef.maxMotorTorque = MaxMotorTorque;
	}

	Joint = (b2WheelJoint*)world->getWorld()->CreateJoint(&jointDef);
}


bool CIwGameBox2dJoints::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - Joints list needs to be declared inside an actor");
		return false;
	}
	CIwGameActor* actor = (CIwGameActor*)parent;

	for (CIwGameXmlNode::_Iterator it = node->begin(); it != node->end(); ++it)
	{
		unsigned int name_hash = (*it)->GetName().getHash();
		if (name_hash == CIwGameXomlNames::Joint_Hash)
		{
			unsigned int	type = 0;
			CIwGameString	actor_b;
			CIwFVec2		offset_a(0, 0), offset_b(0, 0), axis(0, 1.0f), ground_a(0, 0), ground_b(0, 0);
			bool			self_collide = false;
			float			length = -1, frequency = -1, damping = -1, length_a = -1, length_b = -1, ratio = 1;
			bool			motor = false, limit = false;
			float			lower_limit = 0, upper_limit = 0, max_torque = 0, max_force = 0, motor_speed = 0, ref_angle = 999999.0f;

			// Get joint data
			for (CIwGameXmlNode::_AttribIterator it3 = (*it)->attribs_begin(); it3 != (*it)->attribs_end(); ++it3)
			{
				unsigned int attrib_hash = (*it3)->getName().getHash();

				if (attrib_hash == CIwGameXomlNames::Type_Hash)
				{
					type = (*it3)->GetValue().getHash();
				}
				else
				if (attrib_hash == CIwGameXomlNames::ActorB_Hash)
				{
					actor_b = (*it3)->GetValue();
				}
				else
				if (attrib_hash == CIwGameXomlNames::OffsetA_Hash)
				{
					if (!(*it3)->GetValueAsPoint(offset_a))
					{
						CIwGameError::LogError("Warning: XOML - OffsetA should be a vec2");
					}
				}
				else
				if (attrib_hash == CIwGameXomlNames::OffsetB_Hash)
				{
					if (!(*it3)->GetValueAsPoint(offset_b))
					{
						CIwGameError::LogError("Warning: XOML - OffsetB should be a vec2");
					}
				}
				else
				if (attrib_hash == CIwGameXomlNames::GroundA_Hash)
				{
					if (!(*it3)->GetValueAsPoint(ground_a))
					{
						CIwGameError::LogError("Warning: XOML - GroundA should be a vec2");
					}
				}
				else
				if (attrib_hash == CIwGameXomlNames::GroundB_Hash)
				{
					if (!(*it3)->GetValueAsPoint(ground_b))
					{
						CIwGameError::LogError("Warning: XOML - GroundB should be a vec2");
					}
				}
				else
				if (attrib_hash == CIwGameXomlNames::SelfCollide_Hash)
					self_collide = (*it3)->GetValueAsBool();
				else
				if (attrib_hash == CIwGameXomlNames::Length_Hash)
					length = (*it3)->GetValueAsFloat();
				else
				if (attrib_hash == CIwGameXomlNames::LengthA_Hash)
					length_a = (*it3)->GetValueAsFloat();
				else
				if (attrib_hash == CIwGameXomlNames::LengthB_Hash)
					length_b = (*it3)->GetValueAsFloat();
				else
				if (attrib_hash == CIwGameXomlNames::Frequency_Hash)
					frequency = (*it3)->GetValueAsFloat();
				else
				if (attrib_hash == CIwGameXomlNames::Damping_Hash)
					damping = (*it3)->GetValueAsFloat();
				else
				if (attrib_hash == CIwGameXomlNames::MotorEnabled_Hash)
					motor = (*it3)->GetValueAsBool();
				else
				if (attrib_hash == CIwGameXomlNames::LimitJoint_Hash)
					limit = (*it3)->GetValueAsBool();
				else
				if (attrib_hash == CIwGameXomlNames::LowerLimit_Hash)
					lower_limit = (*it3)->GetValueAsFloat();
				else
				if (attrib_hash == CIwGameXomlNames::UpperLimit_Hash)
					upper_limit = (*it3)->GetValueAsFloat();
				else
				if (attrib_hash == CIwGameXomlNames::MotorSpeed_Hash)
					motor_speed = (*it3)->GetValueAsFloat();
				else
				if (attrib_hash == CIwGameXomlNames::MaxMotorTorque_Hash)
					max_torque = (*it3)->GetValueAsFloat();
				else
				if (attrib_hash == CIwGameXomlNames::MaxMotorForce_Hash)
					max_force = (*it3)->GetValueAsFloat();
				else
				if (attrib_hash == CIwGameXomlNames::ReferenceAngle_Hash)
					ref_angle = (*it3)->GetValueAsFloat();
				else
				if (attrib_hash == CIwGameXomlNames::Ratio_Hash)
					ratio = (*it3)->GetValueAsFloat();
				else
				if (attrib_hash == CIwGameXomlNames::Axis_Hash)
				{
					if (!(*it3)->GetValueAsPoint(axis))
					{
						CIwGameError::LogError("Warning: XOML - Axis should be a vec2");
					}
				}
			}

			CIwGameBox2dWorld* world = actor->getBox2dBody()->getWorld();
			CIwGameActor* actorb = actor->getScene()->findActor(actor_b.c_str());
			if (actorb != NULL)
			{
				if (type == CIwGameXomlNames::Distance_Hash)
				{
					CIwGameBox2dJointDistance* joint = new CIwGameBox2dJointDistance();
					joint->BodyA = actor->getBox2dBody();
					joint->BodyB = actorb->getBox2dBody();
					b2Vec2 anch_a = joint->BodyA->getBody()->GetWorldCenter();
					anch_a.x += world->PixelToWorldX(offset_a.x);
					anch_a.y += world->PixelToWorldY(offset_a.y);
					b2Vec2 anch_b = joint->BodyB->getBody()->GetWorldCenter();
					anch_b.x += world->PixelToWorldX(offset_b.x);
					anch_b.y += world->PixelToWorldY(offset_b.y);
					joint->AnchorA = anch_a;
					joint->AnchorB = anch_b;
					joint->SelfCollide = self_collide;
					joint->Damping = damping;
					joint->Frequency = frequency;
					if (length > 0)
						joint->Length = world->PixelToWorldX((length));
					joint->Create(world);
					actor->getBox2dBody()->addJoint(joint);
				}
				else
				if (type == CIwGameXomlNames::Revolute_Hash)
				{
					CIwGameBox2dJointRevolute* joint = new CIwGameBox2dJointRevolute();
					joint->BodyA = actor->getBox2dBody();
					joint->BodyB = actorb->getBox2dBody();
					b2Vec2 anch_a = joint->BodyA->getBody()->GetWorldCenter();
					anch_a.x += world->PixelToWorldX(offset_a.x);
					anch_a.y += world->PixelToWorldY(offset_a.y);
					b2Vec2 anch_b = joint->BodyB->getBody()->GetWorldCenter();
					anch_b.x += world->PixelToWorldX(offset_b.x);
					anch_b.y += world->PixelToWorldY(offset_b.y);
					joint->AnchorA = anch_a;
					joint->AnchorB = anch_b;
					joint->SelfCollide = self_collide;
					joint->LimitJoint = limit;
					joint->MotorEnabled = motor;
					joint->LowerAngle = lower_limit;
					joint->UpperAngle = upper_limit;
					joint->MotorSpeed = motor_speed;
					joint->MaxMotorTorque = max_torque;
					joint->ReferenceAngle = ref_angle;
					joint->Create(world);
					actor->getBox2dBody()->addJoint(joint);
				}
				else
				if (type == CIwGameXomlNames::Prismatic_Hash)
				{
					CIwGameBox2dJointPrismatic* joint = new CIwGameBox2dJointPrismatic();
					joint->BodyA = actor->getBox2dBody();
					joint->BodyB = actorb->getBox2dBody();
					b2Vec2 anch_a = joint->BodyA->getBody()->GetWorldCenter();
					anch_a.x += world->PixelToWorldX(offset_a.x);
					anch_a.y += world->PixelToWorldY(offset_a.y);
					b2Vec2 anch_b = joint->BodyB->getBody()->GetWorldCenter();
					anch_b.x += world->PixelToWorldX(offset_b.x);
					anch_b.y += world->PixelToWorldY(offset_b.y);
					joint->AnchorA = anch_a;
					joint->AnchorB = anch_b;
					joint->Axis.x = axis.x;
					joint->Axis.y = axis.y;
					joint->SelfCollide = self_collide;
					joint->LimitJoint = limit;
					joint->MotorEnabled = motor;
					joint->LowerTranslation = world->PixelToWorldY(lower_limit);
					joint->UpperTranslation = world->PixelToWorldY(upper_limit);
					joint->MotorSpeed = motor_speed;
					joint->MaxMotorForce = max_force;
					joint->ReferenceAngle = ref_angle;
					joint->Create(world);
					actor->getBox2dBody()->addJoint(joint);
				}
				else
				if (type == CIwGameXomlNames::Pulley_Hash)
				{
					CIwGameBox2dJointPulley* joint = new CIwGameBox2dJointPulley();
					joint->BodyA = actor->getBox2dBody();
					joint->BodyB = actorb->getBox2dBody();
					b2Vec2 anch_a = joint->BodyA->getBody()->GetWorldCenter();
					anch_a.x += world->PixelToWorldX(offset_a.x);
					anch_a.y += world->PixelToWorldY(offset_a.y);
					b2Vec2 anch_b = joint->BodyB->getBody()->GetWorldCenter();
					anch_b.x += world->PixelToWorldX(offset_b.x);
					anch_b.y += world->PixelToWorldY(offset_b.y);
					b2Vec2 grounda = joint->BodyA->getBody()->GetWorldCenter();
					grounda.x += world->PixelToWorldX(ground_a.x);
					grounda.y += world->PixelToWorldY(ground_a.y);
					b2Vec2 groundb = joint->BodyB->getBody()->GetWorldCenter();
					groundb.x += world->PixelToWorldX(ground_b.x);
					groundb.y += world->PixelToWorldY(ground_b.y);
					joint->AnchorA = anch_a;
					joint->AnchorB = anch_b;
					joint->GroundAnchorA = grounda;
					joint->GroundAnchorB = groundb;
					if (length_a > 0)
						joint->LengthA = length_a;
					if (length_b > 0)
						joint->LengthB = length_b;
					joint->Ratio = ratio;
					joint->SelfCollide = self_collide;
					joint->Create(world);
					actor->getBox2dBody()->addJoint(joint);
				}
				else
				if (type == CIwGameXomlNames::Wheel_Hash)
				{
					CIwGameBox2dJointWheel* joint = new CIwGameBox2dJointWheel();
					joint->BodyA = actor->getBox2dBody();
					joint->BodyB = actorb->getBox2dBody();
					b2Vec2 anch_a = joint->BodyA->getBody()->GetWorldCenter();
					anch_a.x += world->PixelToWorldX(offset_a.x);
					anch_a.y += world->PixelToWorldY(offset_a.y);
					b2Vec2 anch_b = joint->BodyB->getBody()->GetWorldCenter();
					anch_b.x += world->PixelToWorldX(offset_b.x);
					anch_b.y += world->PixelToWorldY(offset_b.y);
					joint->Axis.x = axis.x;
					joint->Axis.y = axis.y;
					joint->AnchorA = anch_a;
					joint->AnchorB = anch_b;
					joint->SelfCollide = self_collide;
					joint->MotorEnabled = motor;
					joint->MotorSpeed = motor_speed;
					joint->MaxMotorTorque = max_torque;
					joint->Damping = damping;
					joint->Frequency = frequency;
					joint->Create(world);
					actor->getBox2dBody()->addJoint(joint);
				}
			}
			else
			{
				CIwGameError::LogError("Warning: XOML - ActorB does not exist in the scene - ", actor_b.c_str());
			}

		}
	}

	IW_GAME_XOML->setExitOnError(false);
	return false;
}




#endif	// IW_GAME_BOX2D


