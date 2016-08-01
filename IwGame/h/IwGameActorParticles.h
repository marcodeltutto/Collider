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

#if !defined(_IW_GAME_ACTOR_PARTICLES_H_)
#define _IW_GAME_ACTOR_PARTICLES_H_

#include "IwGx.h"
#include "Iw2D.h"
#include "IwGeom.h"

#include "IwGameUtil.h"
#include "IwGameActor.h"
#include "IwGameSprite.h"
#include "IwGameAnim.h"
#include "IwGameString.h"
#include "IwGameXoml.h"
#include "IwGameBox2d.h"
#include "IwGameShapes.h"
#include "IwGameActions.h"

class CIwGameScene;
class CIwGameActorParticles;

//
//
//
//
// CIwGameActorParticle - A particle
//
//
//
//
struct CIwGameActorParticle
{
	CIwGameActorParticles*	Parent;					// Particles parent emitter
	CIwGameBitmapSprite*	Visual;					// Visual element that represents the particle
	CIwFVec2				Position;				// Position relative to particle system actor
	CIwFVec2				Velocity;				// Velocity of particle
	CIwFVec2				VelocityDamping;		// Velocity damping for particle
	float					Gravity;				// Gravitational acceleration
	CIwFVec2				Scale;					// Scale of particle
	CIwFVec2				ScaleVelocity;			// Scale velocity of particle
	CIwFVec2				ScaleVelocityDamping;	// Scale velocity of particle
	float					Angle;					// Rotation of particle
	float					AngularVelocity;		// Rotation of particle
	float					AngularVelocityDamping;	// Rotation of particle
	CIwFVec4				Colour;					// Particle colour
	CIwFVec4				ColourVelocity;			// Particle colour velocity
	CIwFVec4				ColourVelocityDamping;	// Particle colour velocity damping
	float					Depth;					// Depth of particle
	float					DepthVelocity;			// Depth velocity of particle
	float					DepthVelocityDamping;	// Depth velocity damping of particle
	bool					Active;					// Active state
	bool					Visible;				// Visible state
	float					LifeSpan;				// Total duration of patricle in seconds
	float					SpawnDelay;				// Amount of time to wait before spawning in seconds
	float					LifeTime;				// Current life time in seconds
	int						Lives;					// Number of lives for particle (how time it will be respawned)
	CIwFVec2				OPosition;				// Original Position relative to particle system actor
	CIwFVec2				OVelocity;				// Original Velocity of particle
	CIwFVec2				OScale;					// Original Scale of particle
	CIwFVec2				OScaleVelocity;			// Original Scale velocity of particle
	float					OAngle;					// Original Rotation of particle
	float					OAngularVelocity;		// Original Rotation veloxity of particle
	CIwFVec4				OColour;				// Original Particle colour
	CIwFVec4				OColourVelocity;		// Original Particle colour velocity
	float					ODepth;					// Original Depth of particle
	float					ODepthVelocity;			// Original Depth velocity of particle
	bool					Attached;				// Attached particles will follow the patricle actor

	CIwGameActorParticle() :	Parent(NULL), Visual(NULL), Position(0, 0), Velocity(0, 0), VelocityDamping(1.0f, 1.0f), Scale(1.0f, 1.0f) , ScaleVelocity(0, 0), ScaleVelocityDamping(1.0f, 1.0f), 
								Angle(0), AngularVelocity(0), AngularVelocityDamping(1.0f), Colour(255, 255, 255, 255), ColourVelocity(0, 0, 0, 0), ColourVelocityDamping(1, 1, 1, 1), 
								LifeSpan(2), SpawnDelay(0), LifeTime(0), Lives(-1), Visible(false), Active(true), Gravity(0), Depth(0), DepthVelocity(0),DepthVelocityDamping(1), Attached(true)
	{
	}
	~CIwGameActorParticle();

	void	Update(float dt, float dts);
	void	CopyToOld();
	void	CopyFromOld();

	CIwGameActorParticle* Clone();

};

//
//
//
//
// CIwGameActorParticles - A particle system based actor
//
//
//
//
class CIwGameActorParticles : public CIwGameActor
{
public:
	enum eParticleMode
	{
		PAM_Normal,
		PAM_Random,
	};

	// Properties
protected:
	CIwGameImage*								Image;				// Image used by the particles
	CIwGameSlotArray<CIwGameActorParticle*>*	Particles;			// Particles in this system
	eParticleMode								PositionMode;		// Mode of operatiuon for position
	eParticleMode								AngleMode;			// Mode of operatiuon for angle
	eParticleMode								ScaleMode;			// Mode of operatiuon for scale
	eParticleMode								DepthMode;			// Mode of operatiuon for depth
	eParticleMode								VelocityMode;		// Mode of operatiuon for velocity
	eParticleMode								AngVelocityMode;	// Mode of operatiuon for angular velocity
	eParticleMode								ScaleVelocityMode;	// Mode of operatiuon for scale velocity
	eParticleMode								DepthVelocityMode;	// Mode of operatiuon for depth velocity
	CIwFVec2									PositionRange;		// Range of spawn positions
	CIwFVec2									AngleRange;			// Range of spawn angles - x = min angle, y = max angle
	CIwFVec2									ScaleRange;			// Range of scales - x = min scale, y = max scale
	CIwFVec2									DepthRange;			// Range of depth - x = min depth, y = max depth
	CIwFVec4									VelocityRange;		// Range of spawn velocities - min_x, max_y, min_y, max_y
	CIwFVec2									AngVelocityRange;	// Range of angular velocities - x = min ang velocity, y = max ang velocity
	CIwFVec2									ScaleVelocityRange;	// Range of scale velocities - x = min scale velocity, y = max scale velocity
	CIwFVec2									DepthVelocityRange;	// Range of depth velocities - x = min depth velocity, y = max depth velocity

public:
	bool			addParticle(CIwGameActorParticle* particle, CIwRect& src_rect, int count = 0, float spawn_delay = 0.0f);
	void			removeParticle(CIwGameActorParticle* particle)	{ Particles->remove(particle); delete particle; }
	CIwGameImage*	getImage()										{ return Image; }
	void			setImage(CIwGameImage* image)					{ Image = image; }
	void			setPositionMode(eParticleMode mode)				{ PositionMode = mode; }
	void			setAngleMode(eParticleMode mode)				{ AngleMode = mode; }
	void			setScaleMode(eParticleMode mode)				{ ScaleMode = mode; }
	void			setVelocityMode(eParticleMode mode)				{ VelocityMode = mode; }
	void			setAngVelocityMode(eParticleMode mode)			{ AngVelocityMode = mode; }
	void			setScaleVelocityMode(eParticleMode mode)		{ ScaleVelocityMode = mode; }
	void			setDepthMode(eParticleMode mode)				{ DepthMode = mode; }
	void			setDepthVelocityMode(eParticleMode mode)		{ DepthVelocityMode = mode; }
	void			setPositionRange(CIwFVec2& range)				{ PositionRange = range; }
	void			setAngleRange(CIwFVec2& range)					{ AngleRange = range; }
	void			setScaleRange(CIwFVec2& range)					{ ScaleRange = range; }
	void			setDepthRange(CIwFVec2& range)					{ DepthRange = range; }
	void			setVelocityRange(CIwFVec4& range)				{ VelocityRange = range; }
	void			setAngVelocityRange(CIwFVec2& range)			{ AngVelocityRange = range; }
	void			setScaleVelocityRange(CIwFVec2& range)			{ ScaleVelocityRange = range; }
	void			setDepthVelocityRange(CIwFVec2& range)			{ DepthVelocityRange = range; }
	// Properties end
protected:
	void			setParticlesVisibility(bool visible);
public:
	CIwGameActorParticles() :	CIwGameActor(), Particles(NULL), Image(NULL), PositionMode(PAM_Normal), AngleMode(PAM_Normal), ScaleMode(PAM_Normal), VelocityMode(PAM_Normal), 
								AngVelocityMode(PAM_Normal), ScaleVelocityMode(PAM_Normal), DepthMode(PAM_Normal), DepthVelocityMode(PAM_Normal), DepthRange(0, 0), 
								PositionRange(0, 0), AngleRange(0, 360), ScaleRange(1.0, 1.0), VelocityRange(0, 0, 0, 0), AngVelocityRange(0, 0), ScaleVelocityRange(0, 0), DepthVelocityRange(0, 0)  {}
	virtual ~CIwGameActorParticles()
	{
		for (int t = 0; t < Particles->getSize(); t++)
		{
			CIwGameActorParticle* particle = Particles->element_at(t);
			if (particle != NULL)
				delete particle;
		}
		SAFE_DELETE(Particles)
	}

	bool				Init(int max_particles);
	bool				Update(float dt);
	bool				UpdateVisual();

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	// Implementation of IIwGameAnimTarget interface
	bool				UpdateFromAnimation(CIwGameAnimInstance *animation);

	// Generates a bunch of randmo particles using the ranges and modes previously set
	void				GenerateRandomParticles(int count, CIwRect& src_rect, CIwFVec4& colour, CIwFVec4& colour_velocity, float duration, int repeat_count = 0, float spawn_delay_change = 0, float gravity = 10.0f);
	// Generates a bunch of randmo particles using the ranges and modes previously set, using patricle as a basis
	void				GenerateRandomParticles(int count, CIwGameActorParticle* particle, CIwRect& src_rect, float duration, int repeat_count = 0, float spawn_delay_change = 0);

	// Utility
	void				setVelocityAll(float x, float y);

	// Internal
	void				RegenParticle(CIwGameActorParticle* particle);
};

//
//  CIwGameActorParticlesCreator - Creates an instance of a particles based actor object
//
class CIwGameActorParticlesCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameActorParticlesCreator()
	{
		setClassName("actorparticles");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameActorParticles(); }
};


#endif // _IW_GAME_ACTOR_PARTICLES_H_
