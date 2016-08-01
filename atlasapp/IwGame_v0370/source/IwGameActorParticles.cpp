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

#include "IwGameActorParticles.h"
#include "IwGameScene.h"
#include "IwGameCamera.h"
#include "IwGameXoml.h"
#include "IwGameActions.h"


//
//
//
//
//
// CIwGameActorParticle implementation
//
//
//
//
//
void CIwGameActorParticle::Update(float dt, float dts)
{
	if (Active)
	{
		LifeTime += dts;
		if (LifeTime >= SpawnDelay)
		{
			Visible = true;

			// Update velocity
			Position += Velocity * dt;
			Velocity.x *= VelocityDamping.x;
			Velocity.y *= VelocityDamping.y;
			Velocity.y += Gravity * dt;
			Angle += AngularVelocity * dt;
			AngularVelocity *= AngularVelocityDamping;
			Scale += ScaleVelocity * dt;
			ScaleVelocity.x *= ScaleVelocityDamping.x;
			ScaleVelocity.y *= ScaleVelocityDamping.y;
			Colour += ColourVelocity * dt;
			if (Colour.x < 0) Colour.x = 0;
			else
				if (Colour.x > 255) Colour.x = 255;
			if (Colour.y < 0) Colour.y = 0;
			else
				if (Colour.y > 255) Colour.y = 255;
			if (Colour.z < 0) Colour.z = 0;
			else
				if (Colour.z > 255) Colour.z = 255;
			if (Colour.w < 0) Colour.w = 0;
			else
				if (Colour.w > 255) Colour.w = 255;
			ColourVelocity.x *= ColourVelocityDamping.x;
			ColourVelocity.y *= ColourVelocityDamping.y;
			ColourVelocity.z *= ColourVelocityDamping.z;
			ColourVelocity.w *= ColourVelocityDamping.w;
			Depth += DepthVelocity * dt;
			DepthVelocity *= DepthVelocityDamping;

			// Check for timing out
			if (LifeTime >= (LifeSpan + SpawnDelay))
			{
				LifeTime -= (LifeSpan + SpawnDelay);		// Reset life timer
				SpawnDelay = 0;								// Spawn delay should only be used on first spawn
				if (Lives < 0)								// Repeat forever
				{
					Visible = true;
				}
				else
				if (Lives > 0)		// Only reset of enough lives left
				{
					if (Lives == 1)
					{
						// No lives left so hide
						Visible = false;
						Active = false;
					}
					else
						Lives--;
				}

				// Reset the particle
				if (Visible)
				{
/*					if (SpawnDelay == 0)
						Visible = true;
					else
						Visible = false;*/
					CopyFromOld();
					Parent->RegenParticle(this);
				}
			}
		}
		else
		{
			// Not due to spawn yet so hide
			Visible = false;
		}
	}
	else
	{
		// Inactive so hide
		Visible = false;
	}
}

void CIwGameActorParticle::CopyToOld()
{
	OPosition = Position;
	OVelocity = Velocity;
	OScale = Scale;
	OScaleVelocity = ScaleVelocity;
	OAngle = Angle;
	OAngularVelocity = AngularVelocity;
	OColour = Colour;
	OColourVelocity = ColourVelocity;
	ODepth = Depth;
	ODepthVelocity = DepthVelocity;
}

void CIwGameActorParticle::CopyFromOld()
{
	Position = OPosition;
	Velocity = OVelocity;
	Scale = OScale;
	ScaleVelocity = OScaleVelocity;
	Angle = OAngle;
	AngularVelocity = OAngularVelocity;
	Colour = OColour;
	ColourVelocity = OColourVelocity;
	Depth = ODepth;
	DepthVelocity = ODepthVelocity;
}

CIwGameActorParticle* CIwGameActorParticle::Clone()
{
	CIwGameActorParticle* clone = new CIwGameActorParticle();
	*clone = *this;
	clone->Visual = NULL;

	return clone;
}



//
//
//
//
//
// CIwGameActorParticles implementation
//
//
//
//
//

void CIwGameActorParticles::RegenParticle(CIwGameActorParticle* particle)
{
	if (PositionMode == PAM_Random)
	{
		particle->Position.x = ((float)rand() * PositionRange.x * 2) / RAND_MAX - PositionRange.x;
		particle->Position.y = ((float)rand() * PositionRange.y * 2) / RAND_MAX - PositionRange.y;
	}
	if (AngleMode == PAM_Random)
	{
		particle->Angle = ((float)rand() * (AngleRange.y - AngleRange.x)) / RAND_MAX + AngleRange.x;
	}
	if (ScaleMode == PAM_Random)
	{
		particle->Scale.x = ((float)rand() * (ScaleRange.y - ScaleRange.x)) / RAND_MAX + ScaleRange.x;
		particle->Scale.y = particle->Scale.x;
	}
	if (DepthMode == PAM_Random)
	{
		particle->Depth = ((float)rand() * (DepthRange.y - DepthRange.x)) / RAND_MAX + DepthRange.x;
	}
	if (VelocityMode == PAM_Random)
	{
		particle->Velocity.x = ((float)rand() * (VelocityRange.y - VelocityRange.x)) / RAND_MAX + VelocityRange.x;
		particle->Velocity.y = ((float)rand() * (VelocityRange.w - VelocityRange.z)) / RAND_MAX + VelocityRange.z;
	}
	if (AngVelocityMode == PAM_Random)
	{
		particle->AngularVelocity = ((float)rand() * (AngVelocityRange.y - AngVelocityRange.x)) / RAND_MAX + AngVelocityRange.x;
	}
	if (ScaleVelocityMode == PAM_Random)
	{
		particle->ScaleVelocity.x = ((float)rand() * (ScaleVelocityRange.y - ScaleVelocityRange.x)) / RAND_MAX + ScaleVelocityRange.x;
		particle->ScaleVelocity.y = particle->ScaleVelocity.x;
	}
	if (DepthVelocityMode == PAM_Random)
	{
		particle->DepthVelocity = ((float)rand() * (DepthVelocityRange.y - DepthVelocityRange.x)) / RAND_MAX + DepthVelocityRange.x;
	}
}

bool CIwGameActorParticles::addParticle(CIwGameActorParticle* particle, CIwRect& src_rect, int count)
{
	// Set particles parent
	particle->Parent = this;

	// Add particle to particles list
	if (!Particles->add(particle))
		return false;

	// Back up properties
	particle->CopyToOld();

	// Apply modes
	RegenParticle(particle);

	// Create a bitmap sprite
	CIwGameBitmapSprite* sprite = new CIwGameBitmapSprite();
	if (sprite == NULL)
		return false;
	sprite->Init();

	// Set particles visual
	particle->Visual = sprite;

	// Set sprite image
	sprite->setImage(Image);
	sprite->setSrcDest(src_rect.x, src_rect.y, src_rect.w, src_rect.h);

	// Add scenes sprite to sprite manager
	Scene->getSpriteManager()->addSprite(sprite);

	// If count was specified then clone this patricle count times
	if (count != 0)
	{
		for (int t = 0; t < count; t++)
		{
			addParticle(particle->Clone(), src_rect);
		}
	}

	return true;
}

bool CIwGameActorParticles::Init(int max_particles)
{
	CIwGameActor::Init();

	// Create dummy sprite (we only need its attributes and do not need to render it)
	Visual = new CIwGameDummySprite();
	Visual->Init();

	// Add dummy sprite to the sprite manager so it can be managed and drawn
	Scene->getSpriteManager()->addSprite(Visual);

	// Create space for particles
	Particles = new CIwGameSlotArray<CIwGameActorParticle*>(max_particles, 4);

	return true;
}

bool CIwGameActorParticles::Update(float dt)
{
	if (!CIwGameActor::Update(dt))
		return false;

	float dts = (dt * FRAME_SPEED_LOCK_MS) / 1000.0f;	// Convert to seconds

	// Update particles
	int num_active = 0;
	for (int t = 0; t < Particles->getSize(); t++)
	{
		CIwGameActorParticle* particle = Particles->element_at(t);
		if (particle != NULL && particle->Active)
		{
			particle->Update(dt, dts);
			num_active++;
		}
	}

	// If all patricles are inactive then remove this actor
	if (num_active == 0)
		return false;

	return true;
}

void CIwGameActorParticles::setParticlesVisibility(bool visible)
{
	// Hide all particle visuals
	for (int t = 0; t < Particles->getSize(); t++)
	{
		CIwGameActorParticle* particle = Particles->element_at(t);
		if (particle != NULL)
		{
			CIwGameBitmapSprite* visual = particle->Visual;
			visual->setVisible(visible);
		}
	}
}

bool CIwGameActorParticles::UpdateVisual()
{
	if (!CIwGameActor::UpdateVisual())
	{
		setParticlesVisibility(false);
		return false;
	}

	CIwColour vc = Visual->getColour();
	// Update particle visuals
	for (int t = 0; t < Particles->getSize(); t++)
	{
		CIwGameActorParticle* particle = Particles->element_at(t);
		if (particle != NULL)
		{
			CIwGameBitmapSprite* visual = particle->Visual;

			if (particle->Visible)
			{
				if (LinkedTo != NULL)
					visual->setLinkedTo(LinkedTo->getVisual());
				float depth = Depth + particle->Depth;
				if (depth < 0.1f)
					depth = 0.1f;
				visual->setDepth((iwfixed)(depth * IW_GEOM_ONE));

				// Transform particles position by actors transform
				CIwVec2 pos;
				pos.x = (int)(particle->Position.x * 8);
				pos.y = (int)(particle->Position.y * 8);
				pos = Visual->getTransform().TransformVec(pos);
			
				// Transfor actor properties to the visual
				visual->setPosAngScale(	pos.x, pos.y, 
										(iwangle)((particle->Angle * IW_ANGLE_2PI) / 360.0f), (iwfixed)(particle->Scale.x * IW_GEOM_ONE), (iwfixed)(particle->Scale.y * IW_GEOM_ONE));

				CIwColour c;
				c.r = (uint8)(((int)vc.r * particle->Colour.x) / 255);
				c.g = (uint8)(((int)vc.g * particle->Colour.y) / 255);
				c.b = (uint8)(((int)vc.b * particle->Colour.z) / 255);
				c.a = (uint8)(((int)vc.a * particle->Colour.w) / 255);
				visual->setColour(c);

				// Set visible layer
				visual->setLayer(Layer);
			}
			visual->setVisible(particle->Visible);
		}
	}

	return true;
}

void CIwGameActorParticles::GenerateRandomParticles(int count, CIwRect& src_rect, CIwFVec4& colour, CIwFVec4& colour_velocity, float duration, int repeat_count, float spawn_delay_change, float gravity)
{
	float spawn_delay = 0;

	for (int t = 0; t < count; t++)
	{
		CIwGameActorParticle* p = new CIwGameActorParticle();
		p->LifeSpan = duration;
		p->Lives = repeat_count;
		p->SpawnDelay = spawn_delay;
		p->Gravity = gravity;
		p->Colour = colour;
		p->ColourVelocity = colour_velocity;

		spawn_delay += spawn_delay_change;

		addParticle(p, src_rect);
	}
}
void CIwGameActorParticles::GenerateRandomParticles(int count, CIwGameActorParticle* particle, CIwRect& src_rect, float duration, int repeat_count, float spawn_delay_change)
{
	float spawn_delay = 0;

	for (int t = 0; t < count; t++)
	{
		CIwGameActorParticle* p = particle->Clone();
		p->LifeSpan = duration;
		p->Lives = repeat_count;
		p->SpawnDelay = spawn_delay;

		spawn_delay += spawn_delay_change;

		addParticle(p, src_rect);
	}
}

void CIwGameActorParticles::setVelocityAll(float x, float y)
{
	for (int t = 0; t < Particles->getSize(); t++)
	{
		CIwGameActorParticle* particle = Particles->element_at(t);
		if (particle != NULL)
		{
			particle->Velocity.x = x;
			particle->Velocity.y = y;
		}
	}
}

bool CIwGameActorParticles::UpdateFromAnimation(CIwGameAnimInstance *animation)
{
	if (CIwGameActor::UpdateFromAnimation(animation))
		return true;

	return false;
}

bool CIwGameActorParticles::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - Actor needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameActor::LoadFromXoml(parent, false, node))
		return false;

	// Process ActorImage specific attributes
	CIwGameString	image_name;
	CIwRect			src_rect(0, 0, 0, 0);

	CIwFVec2 size;
	unsigned int pos_mode_hash = CIwGameString::CalculateHash("posmode");
	unsigned int ang_mode_hash = CIwGameString::CalculateHash("angmode");
	unsigned int scale_mode_hash = CIwGameString::CalculateHash("scalemode");
	unsigned int depth_mode_hash = CIwGameString::CalculateHash("depthmode");
	unsigned int vel_mode_hash = CIwGameString::CalculateHash("velmode");
	unsigned int vang_mode_hash = CIwGameString::CalculateHash("velangmode");
	unsigned int vscale_mode_hash = CIwGameString::CalculateHash("velscalemode");
	unsigned int vdepth_mode_hash = CIwGameString::CalculateHash("veldepthmode");
	unsigned int position_range_hash = CIwGameString::CalculateHash("positionrange");
	unsigned int angle_range_hash = CIwGameString::CalculateHash("anglerange");
	unsigned int scale_range_hash = CIwGameString::CalculateHash("scalerange");
	unsigned int depth_range_hash = CIwGameString::CalculateHash("depthrange");
	unsigned int vel_range_hash = CIwGameString::CalculateHash("velrange");
	unsigned int angvel_range_hash = CIwGameString::CalculateHash("angvelrange");
	unsigned int scalevel_range_hash = CIwGameString::CalculateHash("scalevelrange");
	unsigned int depthvel_range_hash = CIwGameString::CalculateHash("depthvelrange");
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Image_Hash)
		{
			image_name = (*it)->GetValue();
		}
		else
		if (name_hash == CIwGameXomlNames::Size_Hash)
		{
			if (!(*it)->GetValueAsPoint(size))
			{
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::Size");
			}
		}
		else
		if (name_hash == pos_mode_hash)
		{
			if ((*it)->GetValue() == "random")
				PositionMode = PAM_Random;
		}
		else
		if (name_hash == ang_mode_hash)
		{
			if ((*it)->GetValue() == "random")
				AngleMode = PAM_Random;
		}
		else
		if (name_hash == scale_mode_hash)
		{
			if ((*it)->GetValue() == "random")
				ScaleMode = PAM_Random;
		}
		else
		if (name_hash == depth_mode_hash)
		{
			if ((*it)->GetValue() == "random")
				DepthMode = PAM_Random;
		}
		else
		if (name_hash == vel_mode_hash)
		{
			if ((*it)->GetValue() == "random")
				VelocityMode = PAM_Random;
		}
		else
		if (name_hash == vang_mode_hash)
		{
			if ((*it)->GetValue() == "random")
				AngVelocityMode = PAM_Random;
		}
		else
		if (name_hash == vscale_mode_hash)
		{
			if ((*it)->GetValue() == "random")
				ScaleVelocityMode = PAM_Random;
		}
		else
		if (name_hash == vdepth_mode_hash)
		{
			if ((*it)->GetValue() == "random")
				DepthVelocityMode = PAM_Random;
		}
		else
		if (name_hash == position_range_hash)
		{
			if (!(*it)->GetValueAsPoint(PositionRange))
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::PositionRange");
		}
		else
		if (name_hash == angle_range_hash)
		{
			if (!(*it)->GetValueAsPoint(AngleRange))
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::AngleRange");
		}
		else
		if (name_hash == scale_range_hash)
		{
			if (!(*it)->GetValueAsPoint(ScaleRange))
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::ScaleRange");
		}
		else
		if (name_hash == depth_range_hash)
		{
			if (!(*it)->GetValueAsPoint(DepthRange))
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::DepthRange");
		}
		else
		if (name_hash == vel_range_hash)
		{
			if (!(*it)->GetValueAsPoint4(VelocityRange))
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::VelocityRange");
		}
		else
		if (name_hash == angvel_range_hash)
		{
			if (!(*it)->GetValueAsPoint(AngVelocityRange))
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::AngVelocityRange");
		}
		else
		if (name_hash == scalevel_range_hash)
		{
			if (!(*it)->GetValueAsPoint(ScaleVelocityRange))
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::ScaleVelocityRange");
		}
		else
		if (name_hash == depthvel_range_hash)
		{
			if (!(*it)->GetValueAsPoint(DepthVelocityRange))
				CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::DepthVelocityRange");
		}
	}

	// Search the scenes resources for the specified image
	CIwGameImage* image = (CIwGameImage*)Scene->getResourceManager()->findResource(image_name.c_str(), CIwGameXomlNames::Image_Hash);
	if (image == NULL)
	{
		CIwGameError::LogError("Error: XOML - Could not find named image in the scene or in the global resources - ", image_name.c_str());
		return false;
	}
	setImage(image);

	// Calculate number of particles
	int num_particles = node->Children.size();
	if (num_particles == 0)
	{
		CIwGameError::LogError("Warning: XOML - Patricle actor has no particles");
		return true;
	}

	// Initialise the actor
	Init(num_particles);

	// Prrocess particles
	for (CIwGameXmlNode::_Iterator it2 = node->begin(); it2 != node->end(); ++it2)
	{
		CIwGameActorParticle* particle = new CIwGameActorParticle();

		unsigned int name_hash = (*it2)->GetName().getHash();
		if (name_hash == CIwGameXomlNames::Particle_Hash)
		{
			int count = 0;

			// Get particle data
			for (CIwGameXmlNode::_AttribIterator it = (*it2)->attribs_begin(); it != (*it2)->attribs_end(); ++it)
			{
				unsigned int attrib_hash = (*it)->getName().getHash();

				if (attrib_hash == CIwGameXomlNames::Count_Hash)
				{
					count = (*it)->GetValueAsInt();
				}
				else
				if (attrib_hash == CIwGameXomlNames::Position_Hash)
				{
					if (!(*it)->GetValueAsPoint(particle->Position))
						CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::Position");
				}
				else
				if (attrib_hash == CIwGameXomlNames::Velocity_Hash)
				{
					if (!(*it)->GetValueAsPoint(particle->Velocity))
						CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::Velocity");
				}
				else
				if (attrib_hash == CIwGameXomlNames::VelocityDamping_Hash)
				{
					if (!(*it)->GetValueAsPoint(particle->VelocityDamping))
						CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::VelocityDamping");
				}
				else
				if (attrib_hash == CIwGameXomlNames::Angle_Hash)
					particle->Angle = (*it)->GetValueAsFloat();
				else
				if (attrib_hash == CIwGameXomlNames::AngularVelocity_Hash)
					particle->AngularVelocity = (*it)->GetValueAsFloat();
				else
				if (attrib_hash == CIwGameXomlNames::AngularVelocityDamping_Hash)
				{
					particle->AngularVelocityDamping = (*it)->GetValueAsFloat();
				}
				else
				if (attrib_hash == CIwGameXomlNames::Depth_Hash)
					particle->Depth = (*it)->GetValueAsFloat();
				else
				if (attrib_hash == CIwGameXomlNames::DepthVelocity_Hash)
				{
					particle->DepthVelocity = (*it)->GetValueAsFloat();
				}
				else
				if (attrib_hash == CIwGameXomlNames::DepthVelocityDamping_Hash)
				{
					particle->DepthVelocityDamping = (*it)->GetValueAsFloat();
				}
				else
				if (attrib_hash == CIwGameXomlNames::Scale_Hash)
				{
					if (!(*it)->GetValueAsPoint(particle->Scale))
						CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::Scale");
				}
				else
				if (attrib_hash == CIwGameXomlNames::ScaleVelocity_Hash)
				{
					if (!(*it)->GetValueAsPoint(particle->Scale))
						CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::ScaleVelocity");
				}
				else
				if (attrib_hash == CIwGameXomlNames::ScaleVelocityDamping_Hash)
				{
					if (!(*it)->GetValueAsPoint(particle->Scale))
						CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::ScaleVelocityDamping");
				}
				else
				if (attrib_hash == CIwGameXomlNames::SrcRect_Hash)
				{
					if (!(*it)->GetValueAsRect(src_rect))
					{
						CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticle::SrcRect");
					}
				}
				else
				if (attrib_hash == CIwGameXomlNames::Colour_Hash || attrib_hash == CIwGameXomlNames::Color_Hash)
				{
					if (!(*it)->GetValueAsPoint4(particle->Colour))
						CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::Colour");
				}
				else
				if (attrib_hash == CIwGameXomlNames::ColourVelocity_Hash)
				{
					if (!(*it)->GetValueAsPoint4(particle->ColourVelocity))
						CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::ColourVelocity");
				}
				else
				if (attrib_hash == CIwGameXomlNames::ColourVelocityDamping_Hash)
				{
					if (!(*it)->GetValueAsPoint4(particle->ColourVelocityDamping))
						CIwGameError::LogError("Warning: XOML - Invalid value for ActorParticles::ColourVelocityDamping");
				}
				else
				if (attrib_hash == CIwGameXomlNames::Repeat_Hash)
				{
					particle->Lives = (*it)->GetValueAsInt();
				}
				else
				if (attrib_hash == CIwGameXomlNames::SpawnDelay_Hash)
				{
					particle->SpawnDelay = (*it)->GetValueAsFloat();
				}
				else
				if (attrib_hash == CIwGameXomlNames::Duration_Hash)
				{
					particle->LifeSpan = (*it)->GetValueAsFloat();
				}
				else
				if (attrib_hash == CIwGameXomlNames::Active_Hash)
				{
					particle->Active = (*it)->GetValueAsBool();
				}
				else
				if (attrib_hash == CIwGameXomlNames::Visible_Hash)
				{
					particle->Visible = (*it)->GetValueAsBool();
				}
				else
				if (attrib_hash == CIwGameXomlNames::Gravity_Hash)
				{
					particle->Gravity = (*it)->GetValueAsFloat() / 100.0f;
				}
			}
			addParticle(particle, src_rect, count);
		}
	}

	Size.x = (int)size.x;
	Size.y = (int)size.y;

	// Set dockign mode
	if (Docking != DK_None)
		setDocking(Docking);

	// Process inner tags
	if (load_children)
	{
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}

	return true;
}

