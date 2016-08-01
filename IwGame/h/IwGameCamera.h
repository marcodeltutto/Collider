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

#if !defined(IW_GAME_CAMERA_H_)
#define IW_GAME_CAMERA_H_

#include "s3e.h"
#include "IwGx.h"
#include "IwGeom.h"

#include "IwGameUtil.h"
#include "IwGameXoml.h"
#include "IwGameAnim.h"


//
//
//
//
//  CIwGameCamera - A camera is generally used to display a portion of a game scene, usually tracking some actor
//
//
//
//
class CIwGameCamera : public IIwGameXomlResource
{
public:
	// Properties
protected:
	CIwFMat2D				Transform;		// The combined camera transform
	CIwFVec2				Position;		// Position of camera within scene
	CIwFVec2				Velocity;		// Velocity of camera
	CIwFVec2				VelocityDamping; // Velocity cadmping applied to slow the camera
	float					Scale;			// Cameras scale
	float					Angle;			// Cameras angle
	bool					TransformDirty;	// Marks camera transform needs rebuilding
	unsigned int			TargetHash;		// Hash of  object that the camera is targeting
	bool					TouchPanX;		// If true then the camera will pan along the x-axis with the users touch
	bool					TouchPanY;		// If true then the camera will pan along the y-axis with the users touch
	bool					IgnoreActors;	// if an actor is currently focused then it will usually prevent touch panning, this will disable that
public:
	CIwFMat2D&				getTransform()						{ return Transform; }
	void					setPosition(float x, float y)		{ Position.x = x; Position.y = y; TransformDirty = true; }
	CIwFVec2				getPosition() const					{ return Position; }
	void					setVelocity(float x, float y)		{ Velocity.x = x; Velocity.y = y; }
	CIwFVec2				getVelocity() const					{ return Velocity; }
	void					setVelocityDamping(float x, float y) { VelocityDamping.x = x; VelocityDamping.y = y; }
	CIwFVec2				getVelocityDamping() const			{ return VelocityDamping; }
	void					setScale(float scale)				{ Scale = scale; TransformDirty = true; }
	float					getScale() const					{ return Scale; }
	void					setAngle(float angle)				{ Angle = angle; TransformDirty = true; }
	float					getAngle() const					{ return Angle; }
	void					setTarget(const char* target_name)	{ TargetHash = CIwGameString::CalculateHash(target_name); }
	unsigned int			getTargetHash() const				{ return TargetHash; }
	void					setTransformDirty(bool dirty)		{ TransformDirty = dirty; }
	bool					isTransformDirty() const			{ return TransformDirty; }
	void					setTouchPanX(bool pan)				{ TouchPanX = pan; }
	bool					isTouchPanX() const					{ return TouchPanX; }
	void					setTouchPanY(bool pan)				{ TouchPanY = pan; }
	bool					isTouchPanY() const					{ return TouchPanY; }
	void					setIgnoreActors(bool ignore)		{ IgnoreActors = ignore; }
	bool					getIgnoreActors() const				{ return IgnoreActors; }
	// Properties end

private:

public:
	CIwGameCamera() : IIwGameXomlResource(), Position(0, 0), Scale(1.0f), Angle(0), Velocity(0, 0), VelocityDamping(0.8f, 0.8f), TransformDirty(true), TouchPanX(false), TouchPanY(false), IgnoreActors(true) { setClassType("camera"); }
	virtual ~CIwGameCamera()	{}

	// Updates the camera
	virtual void	Update(float dt);

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parebt, bool load_children, CIwGameXmlNode* node);
};

//
//
//
//
//  CIwGameShapeCreator - Creates an instance of a box shape
//
//
//
//
class CIwGameCameraCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameCameraCreator()
	{
		setClassName("camera");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameCamera(); }
};




#endif // IW_GAME_CAMERA_H_