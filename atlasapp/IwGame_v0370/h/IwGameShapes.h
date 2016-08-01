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

#if !defined(_IW_GAME_SHAPES_H_)
#define _IW_GAME_SHAPES_H_


#include "IwGameUtil.h"
#include "IwGameXoml.h"

// 
// 
//
//
// CIwGameShape - A basic shape class
// 
// 
// 
//

struct CIwGameGeomShape
{
	enum eIwGameGeomShapeType
	{
		ST_None, 
		ST_Box, 
		ST_Circle, 
		ST_Polygon, 
	};

	eIwGameGeomShapeType		ShapeType;			// Type of shape
	virtual ~CIwGameGeomShape() {}
};

struct CIwGameGeomShapeBox : public CIwGameGeomShape
{
	float		Width, Height;						// Dimensions
	CIwGameGeomShapeBox()							{ ShapeType = ST_Box; }
};

struct CIwGameGeomShapeCircle : public CIwGameGeomShape
{
	float		Radius;								// Radius of circle
	CIwGameGeomShapeCircle()						{ ShapeType = ST_Circle; }
};

struct CIwGameGeomShapePolygon : public CIwGameGeomShape
{
	CIwFVec2*	Vertices;							// Radius of circle
	int			NumVertices;						// Number of vertices in the polygon
	CIwGameGeomShapePolygon() : Vertices(NULL)		{ ShapeType = ST_Polygon; }
	~CIwGameGeomShapePolygon()						{ SAFE_DELETE_ARRAY(Vertices) }
};

class CIwGameShape : public IIwGameXomlResource
{
public:
	// Properties
protected:
	CIwGameGeomShape*			Shape;					// Shape data
public:
	void						setShape(CIwGameGeomShape* shape)	{ Shape = shape; }
	CIwGameGeomShape*			getShape()				{ return Shape; }
	// Properties end

public:
	CIwGameShape() : IIwGameXomlResource(), Shape(NULL)	{ setClassType("shape"); }
	virtual ~CIwGameShape()								{ SAFE_DELETE(Shape); }

	// Implementation of IIwGameXomlClass interface
	bool						LoadFromXoml(IIwGameXomlResource* parebt, bool load_children, CIwGameXmlNode* node);
};

//
//
//
//
//  CIwGameShapeCreator - Creates an instance of a shape
//
//
//
//
class CIwGameShapeCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameShapeCreator()
	{
		setClassName("shape");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameShape(); }
};




#endif	// _IW_GAME_SHAPES_H_
