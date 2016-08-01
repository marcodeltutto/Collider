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

#if !defined(_CIW_GAME_SPRITE_H_)
#define _CIW_GAME_SPRITE_H_

#include "s3e.h"
#include "Iw2D.h"

#include "IwGameAnim.h"
#include "IwGameImage.h"
#include "IwGameFont.h"
#include "IwGameBrush.h"

class CIwGameSpriteManager;

//
//
//
// CIwGameSprite - A sprite is the visual representation of an on screen game object
//
// Can be thought of as an interface rather than a concrete class as other sprite types are created from this
//
//
class CIwGameSprite
{
public:
	enum eIwGameSpriteType
	{
		ST_None, 
		ST_Image, 
		ST_Text, 
		ST_Dummy, 
		ST_9Patch, 
	};

	// Provide public access to iteration of the sprite list
	typedef CIwList<CIwGameSprite*>::iterator	Iterator;
	Iterator		begin() { return Children.begin(); }
	Iterator		end()	{ return Children.end(); }

	/// Properties
protected:
	CIwGameSprite*			Parent;					// Parent sprite
	CIwList<CIwGameSprite*>	Children;				// List of children sprites
	eIwGameSpriteType		SpriteType;				// Type of sprite
	CIwGameSpriteManager*	Manager;				// Parent sprite manager
	int						Width, Height;			// Destination width and height (used to represent the visible extents of the sprite on screen)
	iwfixed					Depth;					// Depth
	CIwMat2D				Transform;				// Transform
	CIwVec2					Position;				// Position of the sprite
	CIwVec2					Origin;					// Origin of sprite (0, 0 is sprites centre)
	CIwRect					Skew;					// 4 vertex skew offsets (3 bit fixed)
	iwangle					Angle;					// Rotation of sprite (IW_ANGLE_2PI = 360 degrees)
	iwfixed					ScaleX;					// X axis scale of sprite (IW_GEOM_ONE = 1.0)
	iwfixed					ScaleY;					// X axis scale of sprite (IW_GEOM_ONE = 1.0)
	CIwColour				Colour;					// Colour of sprite
	bool					Visible;				// Sprites visible state
	bool					Filter;					// Uses filtering if true
	bool					Pooled;					// Tells system if we belong to a sprite pool or not
	bool					InUse;					// Used in a memory pooling system to mark this sprite as in use
	bool					BeforeChildren;			// When true th parent sprite will be drawn before all children otherwise after
	int						Layer;					// Depth layer
	CIwGameSprite*			LinkedTo;				// When a sprite is linked to another it takes on that sprites transform etc..
	IIwGameBrush*			Brush;					// Current brush assigned to this sprite
	int						VertexCount;			// Number of vertices that make up the sprite
	CIwRect					ClipRect;				// Local clipping rect used to clip this sprites children
	CIwMaterial::AlphaMode	AlphaMode;				// Alpha combine mode
public:
	void			setParent(CIwGameSprite* parent)			{ Parent = parent; }
	CIwGameSprite*	getParent()									{ return Parent; }
	void			addChild(CIwGameSprite* sprite);
	void			removeChild(CIwGameSprite* sprite, bool delete_sprites = true);
	void			setSpriteType(eIwGameSpriteType type)		{ SpriteType = type; }
	eIwGameSpriteType	getSpriteType() const					{ return SpriteType; }
	void			setManager(CIwGameSpriteManager* manager)	{ Manager = manager; TransformDirty = true; }
	CIwGameSpriteManager* getManager()							{ return Manager; }
	void			setDestSize(int width, int height)
	{
		Width = width;
		Height = height;
		TransformDirty = true;
	}
	void			setDepth(iwfixed depth)		{ if (depth != Depth) TransformDirty = true; Depth = depth; }
	int				getDestWidth() const		{ return Width; }
	int				getDestHeight() const		{ return Height; }
	iwfixed			getDepth() const			{ return Depth; }
	void			setPosAngScale(int x, int y, iwangle angle, iwfixed scale)
	{
		if (x != Position.x || y != Position.y || angle != Angle || scale != ScaleX || scale != ScaleY)
		{
			Position.x = x;
			Position.y = y;
			Angle = angle;
			ScaleX = ScaleY = scale;
			TransformDirty = true;
		}
	}
	void			setPosAngScale(int x, int y, iwangle angle, iwfixed scale_x, iwfixed scale_y)
	{
		if (x != Position.x || y != Position.y || angle != Angle || scale_x != ScaleX || scale_y != ScaleY)
		{
			Position.x = x;
			Position.y = y;
			Angle = angle;
			ScaleX = scale_x;
			ScaleY = scale_y;
			TransformDirty = true;
		}
	}
	void			setPosition(int x, int y)
	{
		if (x != Position.x || y != Position.y)
		{
			Position.x = x;
			Position.y = y;
			TransformDirty = true;
		}
	}
	CIwVec2			getPosition() const			{ return Position; }
	void			setOrigin(int x, int y)
	{
		if (x != Origin.x || y != Origin.y)
		{
			Origin.x = x;
			Origin.y = y;
			TransformDirty = true;
		}
	}
	CIwVec2			getOrigin() const			{ return Origin; }
	void			setSkew(int x1, int x2, int y1, int y2)
	{
		if (Skew.x != x1 || Skew.y != x2 || Skew.w != y1 || Skew.h != y2)
		{
			Skew.x = x1;
			Skew.y = x2;
			Skew.w = y1;
			Skew.h = y2;
			TransformDirty = true;
		}
	}
	CIwRect			getSkew() const			{ return Skew; }
	void			setAngle(iwangle angle)
	{
		if (angle != Angle)
		{
			Angle = angle;
			TransformDirty = true;
		}
	}
	void			setAngle(float angle)
	{
		Angle = (iwangle)(angle * IW_ANGLE_2PI);
		TransformDirty = true;
	}
	iwangle			getAngle() const			{ return Angle; }		
	void			setScale(iwfixed scale)
	{
		if (scale != ScaleX || scale != ScaleY)
		{
			ScaleX = scale;
			ScaleY = scale;
			TransformDirty = true;
		}
	}
	void			setScale(float scale)
	{
		ScaleX = (iwfixed)(scale * IW_GEOM_ONE);
		ScaleY = ScaleX;
		TransformDirty = true;
	}
	void			setScale(iwfixed scale_x, iwfixed scale_y)
	{
		if (scale_x != ScaleX || scale_y != ScaleY)
		{
			ScaleX = scale_x;
			ScaleY = scale_y;
			TransformDirty = true;
		}
	}
	void			setScale(float scale_x, float scale_y)
	{
		ScaleX = (iwfixed)(scale_x * IW_GEOM_ONE);
		ScaleY = (iwfixed)(scale_y * IW_GEOM_ONE);
		TransformDirty = true;
	}
	iwfixed			getScale() const			{ return ScaleX; }
	iwfixed			getScaleX() const			{ return ScaleX; }
	iwfixed			getScaleY() const			{ return ScaleY; }
	virtual void	setColour(CIwColour& colour, bool use_parent = false)
	{
		Colour = colour;
		if (use_parent && LinkedTo != NULL)
			Colour.a = (uint8)(((int)colour.a * LinkedTo->getOpacity()) / 255);
		if (Colour.a == 0 && colour.a != 0)
			TransformDirty = true;
	}
	CIwColour		getColour() const			{ return Colour; }
	int				getOpacity() const			{ return Colour.a; }
	void			setVisible(bool show)		{ Visible = show; }
	bool			isVisible() const			{ return Visible; }
	void			setFilter(bool enable)		{ Filter = enable; }
	bool			isFilter() const			{ return Filter; }
	bool			isVisibleWithParents() const;
	void			forceTransformDirty()		{ TransformDirty = true; }
	void			setPooled(bool pooled)		{ Pooled = pooled; }
	bool			isPooled() const			{ return Pooled; }
	void			setInUse(bool in_use)		{ InUse = in_use; }
	bool			isInUse() const				{ return InUse; }
	void			setLayer(int layer)			{ Layer = layer; }
	int				getLayer() const			{ return Layer; }
	CIwMat2D&		getTransform()				{ return Transform; }
	void			setLinkedTo(CIwGameSprite* sprite);
	CIwGameSprite*	getLinkedTo()				{ return LinkedTo; }
	iwfixed			getAccumDepth() const		{ return AccumDepth; }
	bool			isTransformDirty() const	{ return TransformDirty; }
	IIwGameBrush*	getBrush()					{ return Brush; }
	int				getVertexCount() const		{ return VertexCount; }
	void			setClipRect(CIwRect& rc);
	CIwRect			getClipRect() const			{ return ClipRect; }
	CIwRect			getScreenClipRect() const	{ return ScreenClipRect; }
	void			setBeforeChildren(bool before) { BeforeChildren = before; }
	void			setClipChanged();
	CIwMat2D&		getFinalTransform()			{ return FinalTransform; }
	bool			affectsClip() const			{ return ClipRect.w >= 0; }
	CIwRect			FindFirstClipRect();
	CIwRect			FindFirstScreenClipRect();
	CIwRect			FindFirstScreenClipRect2();
	virtual bool	isClipped();
	CIwVec2*		getTransformedV()			{ return TransformedV; }
	CIwSVec2*		getScreenV()				{ return ScreenV; }
	void			setAlphaMode(CIwMaterial::AlphaMode mode)	{ AlphaMode = mode; }
	CIwMaterial::AlphaMode	getAlphaMode() const				{ return AlphaMode; }

	/// Properties End
protected:
	bool			ChildChangeClip;			// Set to true if child changes clip rect
	CIwMat2D		FinalTransform;				// Final transform
	bool			TransformDirty;				// Dirty when transform changed
	CIwVec2*		TransformedV;				// Transforrmed vertices (used for hit detection and rendering)
	CIwSVec2*		ScreenV;					// Short vesion of transformed vertices
	iwfixed			AccumDepth;					// Accumulated depth
	CIwRect			ScreenClipRect;				// Transformed screen clipping rect for this sprite

	void			TransformClipRect();	// Transforms the local cliping rect to screen coords
	virtual void	RebuildTransform();		// Rebuilds the display transform
	virtual void	BuildFinalTransform();	// Buiolds the final transform
	virtual void	TransformVertices();	// Builds a list of transformed vertices
	void			UpdateClipping();

public:
	CIwGameSprite() : Pooled(false), TransformedV(NULL), ScreenV(NULL)			{ setSpriteType(ST_None); }
	virtual ~CIwGameSprite()
	{
		for (Iterator it = Children.begin(); it != Children.end(); ++it)
			delete *it;
		Children.clear();

		SAFE_DELETE(TransformedV)
		SAFE_DELETE(ScreenV)
	}

	virtual void	Init(int vertex_count = 4);	// Called to initialise the sprite, used after construction or to reset the sprite in a pooled sprite system
	virtual bool	Update();					// Updates the sprite
	virtual void	Draw() = 0;					// Pure virtual, need to implement in derived classes
	virtual void	DrawChildren();				// Draws the sprites children
	virtual bool	HitTest(int x, int y);		// Check to see if point is within area covered by transformed sprite
	virtual bool	HitTestNoClip(int x, int y); // Check to see if point is within area covered by transformed sprite
	virtual bool	isOutsideFocusRange(int x, int y, float scale = 1.0f);
	virtual CIwVec2 TransformPoint(float x, float y);	// Transform point by sprites local angle / scale transform
	virtual CIwVec2 TransformPointToScreen(float x, float y);	// Transform point by sprites final transform

	virtual void	RebuildTransformNow();		// Rebuilds the display transform immediately

	bool			isClippedByManager(uint16 *indices = NULL, int num_verts = 4);
};

//
//
//
// CIwGameDummySprite - A DummySprite is a visual that has no visual component and is usually only used for its attributes
//
//
//
class CIwGameDummySprite : public CIwGameSprite
{
	// Properties
protected:
public:
	// Properties End
public:
	CIwGameDummySprite() : CIwGameSprite()	{ setSpriteType(ST_Dummy);}
	virtual ~CIwGameDummySprite() {}
	
	void	Draw();
};

//
//
//
// CIwGameBitmapSprite - A BitmapSprite is a bitmapped visual representation of an on screen game object
//
//
//
class CIwGameBitmapSprite : public CIwGameSprite
{
	// Properties
protected:
	CIwGameImage*		Image;					// Bitmapped image that represents this sprite
	int					SrcX, SrcY;				// Top left position in source texture
	int					SrcWidth, SrcHeight;	// Width and height of sprite in source texture
	CIw2DImageTransform	ImageTransform;			// Marmalade image transform
	bool				UVsDirty;				// Dirty when source texture has changed
public:
	void		setImage(CIwGameImage* image)
	{
		if (Image != image)
		{
			Image = image;
			UVsDirty = true;
		}
	}
	CIwGameImage*	getImage()										{ return Image; }
	void		setColour(CIwColour& colour, bool use_parent = false)
	{
		Colour = colour;
		if (use_parent && LinkedTo != NULL)
			Colour.a = (uint8)(((int)colour.a * LinkedTo->getOpacity()) / 255);
		uint32 c = Colour.Get();
		for (int t = 0; t < VertexCount; t++)
			Colours[t] = c;
		if (Colour.a == 0 && colour.a != 0)
			TransformDirty = true;
	}
	void		setColour(int index, CIwColour& colour)
	{
		Colours[index] = colour.Get();
	}
	void		setSrcDest(int x, int y, int width, int height)
	{
		setDestSize(width, height);
		if (SrcX != x || SrcY != y || SrcWidth != width || SrcHeight != height)
		{
			SrcX = x;
			SrcY = y; 
			SrcWidth = width;
			SrcHeight = height;
			UVsDirty = true;
		}
	}
	void		setSrcRect(int x, int y, int width, int height)
	{
		if (SrcX != x || SrcY != y || SrcWidth != width || SrcHeight != height)
		{
			SrcX = x;
			SrcY = y; 
			SrcWidth = width;
			SrcHeight = height;
			UVsDirty = true;
		}
	}
	void		setSrcRect(CIwRect* src)
	{
		if (SrcX != src->x || SrcY != src->y || SrcWidth != src->w || SrcHeight != src->h)
		{
			SrcX = src->x;
			SrcY = src->y; 
			SrcWidth = src->w;
			SrcHeight = src->h;
			UVsDirty = true;
		}
	}
	CIwRect		getSrcRect() const									{ return CIwRect(SrcX, SrcY, SrcWidth, SrcHeight); }
	int			getSrcWidth() const									{ return SrcWidth; }
	int			getSrcHeight() const								{ return SrcHeight; }
	void		setFromBrush(CIwGameBrushImage* brush);
	void		setImageTransform(CIw2DImageTransform transform)	{ ImageTransform = transform; }
	CIw2DImageTransform getImageTransform() const					{ return ImageTransform; }
	// Properties End

protected:
	CIwSVec2*			UVList;						// Texture UV coordinate list
	CIwColour*			Colours;					// Vertices colours
	void				RebuildUVList();

public:

	CIwGameBitmapSprite() : CIwGameSprite(), Image(NULL), UVList(NULL), Colours(NULL), SrcX(0), SrcY(0), SrcWidth(0), SrcHeight(0), ImageTransform(IW_2D_IMAGE_TRANSFORM_NONE)	{ setSpriteType(ST_Image); }
	virtual ~CIwGameBitmapSprite()
	{
		SAFE_DELETE(UVList)
		SAFE_DELETE(Colours)
	}

	void	Init(int vertex_count = 4);
	
	void	Draw();
};

//
//
//
// CIwGameBitmapSprite0 - A BitmapSprite9 is a bitmapped visual representation of an on screen game object that uses patch-9 rendering
//
//
//
class CIwGameBitmapSprite9 : public CIwGameBitmapSprite
{
	// Properties
protected:
public:
	// Properties End

protected:
	void				RebuildUVList();
	void				TransformVertices();

	static uint16		Indices[36];
	static uint16		ClipIndices[4];

public:
	CIwGameBitmapSprite9() : CIwGameBitmapSprite()	{ setSpriteType(ST_9Patch); }
	virtual ~CIwGameBitmapSprite9() {}
	
	void	Init(int vertex_count = 16);

	virtual bool	isClipped();
	bool	HitTest(int x, int y);

	bool	isOutsideFocusRange(int x, int y, float scale = 1.0f);
	void	Draw();
};
//
//
//
// CIwGameTextSprite - A CIwGameTextSprite is a text based visual representation of an on screen game object
//
//
//
class CIwGameTextSprite : public CIwGameSprite
{
	// Properties
protected:
	CIwGameString		Text;					// Text to display
	CIwRect				Rect;					// Area where to draw the text
	CIwGameFont*		Font;					// Font
	bool				TextDirty;				// Dirty when text has changed
	IwGxFontFlags		Flags;					// Font flags
	IwGxFontAlignHor	AlignH;					// Horiznotal alignment
	IwGxFontAlignVer	AlignV;					// Vertical alignment
public:
	void		setText(const char* text)
	{
		if (Text.getHash() != IW_GAME_HASH(text))
		{
			Text = text;
			TextDirty = true;
		}
	}
	CIwGameString&	getText()										{ return Text; }
	void			setRect(CIwRect& rect)							{ Rect = rect; TransformDirty = true; TextDirty = true; }
	CIwRect&		getRect()										{ return Rect; }
	void			setFont(CIwGameFont* font)
	{
		Font = font;
		TextDirty = true;
	}
	CIwGameFont*	getFont()										{ return Font; }
	void			setFlags(IwGxFontFlags flags)					{ Flags = flags; }
	void			setAlignH(IwGxFontAlignHor align)				{ AlignH = align; }
	void			setAlignV(IwGxFontAlignVer align)				{ AlignV = align; }
	CIwGxFontPreparedData* getPreparedData()						{ return &PreparedText; }
	// Properties End

protected:
	CIwGxFontPreparedData PreparedText;
	void			RebuildTransform();
	void			UpdateBindings();
	void			TransformVertices();

public:

	CIwGameTextSprite() : CIwGameSprite(), Font(NULL), Flags(IW_GX_FONT_DEFAULT_F), AlignH(IW_GX_FONT_ALIGN_CENTRE), AlignV(IW_GX_FONT_ALIGN_MIDDLE), TextDirty(true), Rect(-128, -128, 256, 256)   { setSpriteType(ST_Text); }
	virtual ~CIwGameTextSprite() {}
	
	void			Draw();

	void			RebuildText();
};

//
//
// CIwGameSpriteManager - A sprite manager 
//
// The sprite manager managers a collection of sprites, including drawing, tracking and clean up
// The sprite manager also carries its own visual transform that will be applied to all of its children, allowing the user to apply rotation, scaling ans translation to all child sprites
//
//
class CIwGameSpriteManager
{
public:
	// Provide public access to iteration of the sprite list
	typedef CIwList<CIwGameSprite*>::iterator	Iterator;
	Iterator		begin() { return Sprites.begin(); }
	Iterator		end()	{ return Sprites.end(); }

	// Properties
protected:
	CIwMat2D					Transform;				// Transform
	CIwList<CIwGameSprite*>		Sprites;				// Our list of sprites
	CIwGameSlotArray<CIwGameSprite*>*	Layers;			// Visible layers used in depth sorting
	bool						Batching;				// Enable sprite batching
	CIwSVec2					COP;					// Centre of projection (3 bit fixed)
	CIwSVec2					ScreenCentre;			// Screen centre (3 bit fixed)
	CIwRect						ScreenClipRect;			// Screen clipping rectangle
	CIwRect						ClipRect;				// Clipping rectangle
public:
	void			addSprite(CIwGameSprite* sprite);
	void			removeSprite(CIwGameSprite* sprite, bool delete_sprites = true);
	void			setTransform(const CIwMat2D& transform)	{ Transform = transform; DirtyChildTransforms(); }
	const CIwMat2D&	getTransform() const					{ return Transform; }
	void			setBatching(bool batching)				{ Batching = batching; }
	bool			getBatching() const						{ return Batching; }
	void			setCOP(int x, int y)					{ COP.x = x; COP.y = y; }
	CIwSVec2		getCOP() const							{ return COP; }
	void			setScreenCentre(int x, int y)			{ ScreenCentre.x = x; ScreenCentre.y = y; }
	CIwSVec2		getScreenCentre() const					{ return ScreenCentre; }
	void			setScreenClipRect(CIwRect& rect)		{ ScreenClipRect = rect; }
	void			setScreenClipRect(int x, int y, int w, int h)	{ ScreenClipRect.x = x; ScreenClipRect.y = y; ScreenClipRect.w = w; ScreenClipRect.h = h; }
	CIwRect			getScreenClipRect() const				{ return ScreenClipRect; }
	void			setClipRect(CIwRect& rect)				{ ClipRect = rect; }
	void			setClipRect(int x, int y, int w, int h)	{ ClipRect.x = x; ClipRect.y = y; ClipRect.w = w; ClipRect.h = h; }
	CIwRect			getClipRect() const						{ return ClipRect; }
	// Properties End

protected:
	void			DirtyChildTransforms();		// Dirties all child transforms to force them to update
	int				MaxLayers;					// Maximum layers
	void			ClearLayers();				// Clears all visible layers ready for next frame

public:
	CIwGameSpriteManager() : MaxLayers(0), Batching(true), COP(0, 0), ScreenCentre(0, 0), ScreenClipRect(0, 0, 0, 0), ClipRect(0, 0, 0, 0)
	{
		// Set up default rotation, scaling and translation
		Transform.SetIdentity();
		Transform.m[0][0] = IW_GEOM_ONE;
		Transform.m[1][1] = IW_GEOM_ONE;
	}
	~CIwGameSpriteManager() { Release(); }

	void			Init(int max_layers = 10);
	void			Draw();
	void			Release(bool delete_sprites = true);
};





#endif // _CIW_GAME_SPRITE_H_
