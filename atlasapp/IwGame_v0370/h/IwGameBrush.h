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

#if !defined(_CIW_GAME_BRUSH_H_)
#define _CIW_GAME_BRUSH_H_

#include "IwGx.h"
#include "Iw2D.h"
#include "IwList.h"
#include "IwGameUtil.h"
#include "IwGameXoml.h"
#include "IwGameImage.h"

//
//
//
//
// IIwGameBrush - Base class for all brushes
//
//
//
//
class IIwGameBrush : public IIwGameXomlResource
{
public:
	enum eIwGameBrushType
	{
		BT_None, 
		BT_Solid, 
		BT_Gradient, 
		BT_Image, 
		BT_9Patch, 
	};

	// Properties
protected:
	eIwGameBrushType	BrushType;
public:
	void				setBrushType(eIwGameBrushType type)		{ BrushType = type; }
	eIwGameBrushType	getBrushType() const					{ return BrushType; }
	// Properties End
protected:

public:
	IIwGameBrush() : IIwGameXomlResource()	{ setClassType("brush"); setBrushType(BT_None); }
	virtual ~IIwGameBrush() {}

	// Implementation of IIwGameXomlResource interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//
//
//
// CIwGameBrushSolid - Represents a solid colour brush
//
//
//
//
class CIwGameBrushSolid : public IIwGameBrush
{
	// Properties
protected:
	CIwColour		Colour;
public:
	void			setColour(uint8 r, uint8 g, uint8 b, uint8 a)	{ Colour.r = r; Colour.g = g; Colour.b = b; Colour.a = a; }
	void			setColour(CIwColour& colour)					{ Colour = colour; }
	CIwColour		getColour() const								{ return Colour; }
	// Properties End
protected:

public:
	CIwGameBrushSolid() : IIwGameBrush()	{  setBrushType(BT_Solid); }

};

//
//
//
//
// CIwGameBrushGradient - Represents a gradient colour brush
//
//
//
//
struct CIwGameBrushGradientStop
{
	CIwColour		Colour;
	float			offset;
};

class CIwGameBrushGradient : public IIwGameBrush
{
public:
	// Public access to gradient stops iteration
	typedef CIwList<CIwGameBrushGradientStop*>::iterator _Iterator;
	_Iterator					begin() { return Stops.begin(); }
	_Iterator					end() { return Stops.end(); }
	// Properties
protected:
	CIwList<CIwGameBrushGradientStop*>	Stops;
public:
	void						addStop(CIwGameBrushGradientStop* stop)	{ Stops.push_back(stop); }
	// Properties End
protected:

public:
	CIwGameBrushGradient() : IIwGameBrush()	{ setBrushType(BT_Gradient); }

};

//
//
//
//
// CIwGameBrushImage - Represents an image brush
//
//
//
//
class CIwGameBrushImage : public IIwGameBrush
{
	// Properties
protected:
	CIwGameImage*		Image;
	CIwRect				SrcRect;
public:
	void				setImage(CIwGameImage* image)			{ Image = image; }
	CIwGameImage*		getImage()								{ return Image; }
	void				setSrcRect(int x, int y, int w, int h)	{ SrcRect.x = x; SrcRect.y = y; SrcRect.w = w; SrcRect.h = h; }
	void				setSrcRect(CIwRect& rect)				{ SrcRect = rect; }
	CIwRect				getSrcRect() const						{ return SrcRect; }
	// Properties End
protected:

public:
	CIwGameBrushImage() : IIwGameBrush(), Image(NULL)	{ setBrushType(BT_Image); }

};

//
//
//
//
// CIwGameBrushImage9 - Represents a 9 patch style image brush
//
//
//
//
class CIwGameBrushImage9 : public CIwGameBrushImage
{
	// Properties
protected:
	CIwRect				ScaleArea;
public:
	void				setScalableArea(int x, int y, int w, int h)	{ ScaleArea.x = x; ScaleArea.y = y; ScaleArea.w = w; ScaleArea.h = h; }
	void				setScalableArea(CIwRect &rc)				{ ScaleArea = rc; }
	CIwRect				getScalableArea() const						{ return ScaleArea; }
	// Properties End
protected:

public:
	CIwGameBrushImage9() : CIwGameBrushImage(), ScaleArea(0, 0, 0, 0)	{ setBrushType(BT_9Patch); }

};



//
//  CIwGameBrushCreator - Creates an instance of a brush object
//
class CIwGameBrushCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameBrushCreator()
	{
		setClassName("brush");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new IIwGameBrush(); }
};

#endif	// _CIW_GAME_BRUSH_H_
