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

#if !defined(IW_GAME_BOX2D_H_)
#define IW_GAME_BOX2D_H_

#include "s3e.h"
#include "IwGx.h"
#include "IwGeom.h"

#include "IwGameUtil.h"
#include "IwGameString.h"
#include "IwGameXoml.h"
#include "IwGameShapes.h"

// To disable Box2D support please comment out or undefine IW_GAME_BOX2D
#define	IW_GAME_BOX2D

#if defined(IW_GAME_BOX2D)

#include "Box2D/Box2D.h"

class CIwGameBox2dBody;


//
//
//
//
// CIwGameBox2dCcollisionCallback - Box2D scene collision callback
//
//
//
//
class CIwGameBox2dCcollisionCallback : public b2ContactListener
{
public:
	void				BeginContact(b2Contact* contact);		// Called when one body begins to collide with another
	void				EndContact(b2Contact* contact);			// Called when one body drops colliding with another
};

// Glopbally defined collision collback used to marshal cross actor collision calls
extern CIwGameBox2dCcollisionCallback	g_CIwGameBox2dCcollisionCallback;


//
//
//
//
// CIwGameBox2dCollidable - Represents a collection of begin and end contact collisions. Objects that are part of the physics system usually derive from this class
//
//
//
//
class CIwGameBox2dCollidable
{
public:
	// Properties
protected:
	void*									  UserData;					// Contains user data (usually container actor)
	CIwGameSlotArray<CIwGameBox2dCollidable*> CollisionsStarted;		// List of collidables that have just come into contact with this collidable
	CIwGameSlotArray<CIwGameBox2dCollidable*> CollisionsEnded;			// List of collidables that have just lost contact with this collidable
	CIwGameSlotArray<CIwGameBox2dCollidable*> CollisionContacts;		// List of collidables that this one is in collision with
public:
	virtual void	addCollisionStarted(CIwGameBox2dCollidable* with)
	{
		CollisionsStarted.add(with);
		CollisionContacts.add(with);
	}
	void			clearCollisionsStarted()			{ CollisionsStarted.clear(); }
	int				getCollisionsStartedCount()			{ return CollisionsStarted.count(); }
	virtual void	addCollisionEnded(CIwGameBox2dCollidable* with)
	{
		CollisionsEnded.add(with);
		CollisionContacts.remove(with);
	}
	void			clearCollisionsEnded()				{ CollisionsEnded.clear(); }
	int				getCollisionsEndedCount()			{ return CollisionsEnded.count(); }
	void			clearCollisionContacts()			{ CollisionContacts.clear(); }
	int				getCollisionContactsCount()			{ return CollisionContacts.count(); }

	void			clearAllCollisions()
	{
		CollisionsStarted.clear();
		CollisionsEnded.clear();
	}
	CIwGameSlotArray<CIwGameBox2dCollidable*>& getCollisionsStarted()	{ return CollisionsStarted; }
	CIwGameSlotArray<CIwGameBox2dCollidable*>& getCollisionsEnded()		{ return CollisionsEnded; }
	CIwGameSlotArray<CIwGameBox2dCollidable*>& getCollisionContacts()	{ return CollisionContacts; }
	void				setUserData(void* user_data)	{ UserData = user_data; }
	void*				getUserData()					{ return UserData; }
	// End of properties

	CIwGameBox2dCollidable() : UserData(NULL) {}

	virtual ~CIwGameBox2dCollidable()
	{
		clearAllCollisions();
		CollisionContacts.clear();
	}
};

//
//
//
//
// CIwGameBox2dMaterial - Box2D physics material
//
//
//
//
class CIwGameBox2dMaterial : public IIwGameXomlResource
{
public:

protected:
	// Properties
	b2BodyType			BodyType;			// Physical body type
	float				Density;			// Density of body
	float				Friction;			// Amount of friction to apply
	float				Restitution;		// Bounciness of the object
	bool				Bullet;				// Is bullet type (is set then object will use continuous colilsion detection (slower)) - only use for very fast moviog objects
	bool				FixedRotation;		// Set to true if you do not want an objects raotation to change
	float				GravityScale;		// Specific gravity scale
public:
	void				setBodyType(b2BodyType type)		{ BodyType = type; }
	b2BodyType			getBodyType() const					{ return BodyType; }
	void				setDensity(float density)			{ Density = density; }
	float				getDensity() const					{ return Density; }
	void				setFriction(float friction)			{ Friction = friction; }
	float				getFriction() const					{ return Friction; }
	void				setRestitution(float restitution)	{ Restitution = restitution; }
	float				getRestitution() const				{ return Restitution; }
	void				setBullet(bool bullet)				{ Bullet = bullet; }
	bool				isBullet() const					{ return Bullet; }
	void				setFixedRotation(bool fixed)		{ FixedRotation = fixed; }
	bool				isFixedRotation() const				{ return FixedRotation; }
	void				setGravityScale(float scale)		{ GravityScale = scale; }
	float				getGravityScale() const				{ return GravityScale; }
	// Properties end

public:
	CIwGameBox2dMaterial() : IIwGameXomlResource(), Density(1.0f), Friction(1.0f), Restitution(0.1f), Bullet(false), FixedRotation(false), GravityScale(1000000.0f)  { setClassType("box2dmaterial"); }
	virtual ~CIwGameBox2dMaterial() {}

	// Implementation of IIwGameXomlResource interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  CIwGameBox2dMaterialCreator - Creates an instance of a box2d material object
//
class CIwGameBox2dMaterialCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameBox2dMaterialCreator()
	{
		setClassName("box2dmaterial");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameBox2dMaterial(); }
};

//
//
//
//
// CIwGameBox2dWorld - Box2D physics world
//
//
//
//
class CIwGameBox2dWorld
{
public:

protected:
	// Properties
	b2World*				World;					// Physical world
	CIwFVec2				WorldScale;				// Scaling between physical and viusal worlds (set to 0, 0 to disable physics update)
	int						VelocityIterations;		// Number of internal iterations used when computing velocities
	int						PositionIterations;		// Number of internal iterations used when computing positions
	CIwFVec2				Gravity;				// Scene gravity
	float					TimeStep;				// Physics time step in seconds
public:
	void					setWorldScale(float x, float y)			{ WorldScale.x = x; WorldScale.y = y; }
	CIwFVec2				getWorldScale()							{ return WorldScale; }
	b2World*				getWorld()								{ return World; }
	void					setVelocityIterations(int count)		{ VelocityIterations = count; }
	int						getVelocityIterations() const			{ return VelocityIterations; }
	void					setPositionIterations(int count)		{ PositionIterations = count; }
	int						getPositionIterations() const			{ return PositionIterations; }
	void					setGravity(float x, float y);
	CIwFVec2				getGravity() const						{ return Gravity; }
	// Properties end

public:
	CIwGameBox2dWorld() : World(NULL), WorldScale(10.0f, 10.0f), VelocityIterations(6), PositionIterations(3), Gravity(0.0f, 15.0f) {}
	virtual ~CIwGameBox2dWorld();

	// World initialisation
	virtual void			InitWorld(bool doSleep = true);

	// World update
	virtual void			UpdateWorld(float dt);

	// Box2D world to pixel conversions
	float			WorldToPixelX(float x)						{ return x * WorldScale.x; }
	float			WorldToPixelY(float y)						{ return y * WorldScale.y; }
	float			PixelToWorldX(float x)						{ return x / WorldScale.x; }
	float			PixelToWorldY(float y)						{ return y / WorldScale.y; }

};

//
//
//
//
// CIwGameBox2dJoint - Box2D physics joint
//
//
//
//
enum eIwGameBox2dJointType
{
	JT_Distance, 
	JT_Revolute, 
	JT_Prismatic, 
	JT_Pulley, 
	JT_Wheel, 
};

struct IIwGameBox2dJoint
{
	eIwGameBox2dJointType	Type;				// Type of joint
#if defined (_DEBUG)
	CIwGameString		Name;					// Joint name
#endif	// _DEBUG
	unsigned int		NameHash;				// Joint name hash
	CIwGameBox2dBody*	BodyA;
	CIwGameBox2dBody*	BodyB;
	bool				SelfCollide;

	virtual void		Create(CIwGameBox2dWorld* world) = 0;
	virtual b2Joint*	getJoint() = 0;
	void				setName(const char* name)
	{
#if defined (_DEBUG)
		Name = name;
#endif	// _DEBUG
		NameHash = CIwGameString::CalculateHash(name);
	}
};

struct CIwGameBox2dJointDistance : public IIwGameBox2dJoint
{
	b2Vec2				AnchorA;			// Body A world anchor pos
	b2Vec2				AnchorB;			// Body B world anchor pos
	float				Length;				// Max length between bosy A and B
	float				Frequency;			// Harmonic oscilation Frequency in Hz
	float				Damping;			// Oscillation damping (1.0 for no oscillation)
	b2DistanceJoint*	Joint;

	CIwGameBox2dJointDistance() : Length(-1), Frequency(-1), Damping(-1) { Type = JT_Distance; }

	// Initialise the joint
	void				Create(CIwGameBox2dWorld* world);
	b2Joint*			getJoint()		{ return Joint; }
};

struct CIwGameBox2dJointRevolute : public IIwGameBox2dJoint
{
	b2Vec2				AnchorA;			// Body A world anchor pos
	b2Vec2				AnchorB;			// Body B world anchor pos
	bool				LimitJoint;			// Imited joint rotation
	bool				MotorEnabled;		// Enable motor
	float				LowerAngle;			// Lower rotation limit
	float				UpperAngle;			// Upper rotation limit
	float				MotorSpeed;			// Motor speed
	float				MaxMotorTorque;		// Max motor torque
	float				ReferenceAngle;		// Reference angle
	b2RevoluteJoint*	Joint;

	CIwGameBox2dJointRevolute() : LimitJoint(false), MotorEnabled(false), ReferenceAngle(0) { Type = JT_Revolute; }

	// Initialise the joint
	void				Create(CIwGameBox2dWorld* world);
	b2Joint*			getJoint()		{ return Joint; }
};

struct CIwGameBox2dJointPrismatic : public IIwGameBox2dJoint
{
	b2Vec2				Axis;				// Axis of movement
	b2Vec2				AnchorA;			// Body A world anchor pos
	b2Vec2				AnchorB;			// Body B world anchor pos
	bool				LimitJoint;			// Limit joint translation
	bool				MotorEnabled;		// Emable motor
	float				LowerTranslation;	// Lower translation limit
	float				UpperTranslation;	// Upper translation limit
	float				MotorSpeed;			// Motor speed
	float				MaxMotorForce;		// Max motor force
	float				ReferenceAngle;		// Reference angle
	b2PrismaticJoint*	Joint;

	CIwGameBox2dJointPrismatic() : LimitJoint(false), MotorEnabled(false), ReferenceAngle(0) { Type = JT_Prismatic; }

	// Initialise the joint
	void				Create(CIwGameBox2dWorld* world);
	b2Joint*			getJoint()		{ return Joint; }
};

struct CIwGameBox2dJointPulley : public IIwGameBox2dJoint
{
	b2Vec2				AnchorA;		// Body A world anchor pos
	b2Vec2				AnchorB;		// Body B world anchor pos
	b2Vec2				GroundAnchorA;	// Body A world ground anchor pos
	b2Vec2				GroundAnchorB;	// Body B world ground anchor pos
	float				LengthA;		// Distance between Body A and Ground Anchor A
	float				LengthB;		// Distance between Body B and Ground Anchor B
	float				Ratio;			// Ratio between rate of change of both sides
	b2PulleyJoint*		Joint;

	CIwGameBox2dJointPulley() : Ratio(1.0f), LengthA(0), LengthB(0) { Type = JT_Pulley; }

	// Initialise the joint
	void				Create(CIwGameBox2dWorld* world);
	b2Joint*			getJoint()		{ return Joint; }
};

struct CIwGameBox2dJointWheel : public IIwGameBox2dJoint
{
	b2Vec2				Axis;			// Axis
	b2Vec2				AnchorA;		// Body A world anchor pos
	b2Vec2				AnchorB;		// Body B world anchor pos
	bool				MotorEnabled;	// Motor enabled
	float				MotorSpeed;		// Motor speed
	float				MaxMotorTorque;	// Max motor torque
	float				Frequency;		// Harmonic oscilation Frequency in Hz
	float				Damping;		// Oscillation damping (1.0 for no oscillation)
	b2WheelJoint*		Joint;

	CIwGameBox2dJointWheel() : MotorEnabled(false) { Type = JT_Wheel; }

	// Initialise the joint
	void				Create(CIwGameBox2dWorld* world);
	b2Joint*			getJoint()		{ return Joint; }
};

//
//
//
//
// CIwGameBox2dJoints - Box2D physics joints XOML command
//
//
//
//
class CIwGameBox2dJoints : public IIwGameXomlResource
{
public:
	CIwGameBox2dJoints() { setClassType("joints"); }

	// Implementation of IIwGameXomlResource interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  CIwGameBox2dJointsCreator - Creates an instance of Box2DJoints object
//
class CIwGameBox2dJointsCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameBox2dJointsCreator()
	{
		setClassName("joints");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameBox2dJoints(); }
};


//
//
//
//
// CIwGameBox2dBody - Box2D physics body
//
//
//
//
class CIwGameBox2dBody :  public CIwGameBox2dCollidable
{
public:

protected:
	// Properties
	CIwGameBox2dWorld*			World;					// World that this body is attached to
	b2Body*						Body;					// Physical body
	b2Fixture*					Fixture;				// The physical fixture (managed by Box2D)
	CIwGameBox2dMaterial*		BodyMaterial;			// Physical body attributes that are applied to this actors body
	CIwGameShape*				BodyShape;				// Physical body shape
	CIwList<IIwGameBox2dJoint*>	Joints;					// Physical joints
	int							CollisionFlags;			// Collision flags
public:
	void				setWorld(CIwGameBox2dWorld* world)		{ World = world; }
	CIwGameBox2dWorld*	getWorld()								{ return World; }
	void				setBody(b2Body* body);					// Setting directly will destroy fixture
	b2Body*				getBody()								{ return Body; }
	void				setFixture(CIwGameShape* body_shape, CIwGameBox2dMaterial* body_mat, float com_x, float com_y);
	b2Fixture*			getFixture()							{ return Fixture; }
	void				setBodyMaterial(CIwGameBox2dMaterial* mat);	
	CIwGameBox2dMaterial* getBodyMaterial()						{ return BodyMaterial; }
	void				setBodyShape(CIwGameShape* body_shape)	{ BodyShape = BodyShape; }
	CIwGameShape*		getBodyShape()							{ return BodyShape; }
	CIwList<IIwGameBox2dJoint*>& getJointsList()				{ return Joints; }
	void				addJoint(IIwGameBox2dJoint* joint)		{ Joints.push_back(joint); }
	IIwGameBox2dJoint*	getJoint(int index);
	IIwGameBox2dJoint*	findJoint(unsigned int name_hash);
	void				removeJoint(IIwGameBox2dJoint* joint);
	void				setAsSensor(bool enable)				{ if (Fixture != NULL) Fixture->SetSensor(enable); }
	bool				isSensor() const						{ if (Fixture != NULL) return Fixture->IsSensor(); else return false; }
	void				setCollisionFlags(int category_flags, int mask_flags, int collision_group);
	int					getCollisionFlags()	const				{ return CollisionFlags; }
	int					getCollisionCategory() const;
	int					getCollisionMask() const;
	int					getCollisionGroup() const;
	void				SetAwake(bool awake)					{ Body->SetAwake(awake); }
	int					IsAwake() const							{ return Body->IsAwake(); }
	void				SetActive(bool active)					{ Body->SetActive(active); }
	int					IsActive() const						{ return Body->IsActive(); }
	// Properties end

public:
	CIwGameBox2dBody() : CIwGameBox2dCollidable(), World(NULL), Body(NULL), Fixture(NULL), BodyMaterial(NULL), BodyShape(NULL), CollisionFlags(0)			{}
	virtual ~CIwGameBox2dBody();

	// Initialise the physical body
	void				InitBody(CIwGameBox2dWorld* world, CIwGameShape* body_shape, CIwGameBox2dMaterial* body_mat, CIwFVec2* pos, float angle, float com_x, float com_y);

	void				ReleaseBody();

	// Force application
	void				ApplyForce(float force_x, float force_y, float world_pos_x, float world_pos_y);
	void				ApplyForceToCenter(float force_x, float force_y);
	void				ApplyTorque(float torque);
	void				ApplyLinearImpulse(float impulse_x, float impulse_y, float world_pos_x, float world_pos_y);
	void				ApplyAngularImpulse(float impulse);
};


#endif	// IW_GAME_BOX2D


#endif // IW_GAME_BOX2D_H_
