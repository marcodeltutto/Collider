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
	float					Width, Height;			// Destination width and height (used to represent the visible extents of the sprite on screen)
	float					Depth;					// Depth
	CIwFMat2D				Transform;				// Transform
	CIwFVec2				Position;				// Position of the sprite
	CIwFVec2				Origin;					// Origin of sprite (0, 0 is sprites centre)
	CIwFVec4				Skew;					// 4 vertex skew offsets (3 bit fixed)
	float					Angle;					// Rotation of sprite (degrees)
	float					ScaleX;					// X axis scale of sprite
	float					ScaleY;					// X axis scale of sprite
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
	CIwFVec4				ClipRect;				// Local clipping rect used to clip this sprites children
	CIwMaterial::AlphaMode	AlphaMode;				// Alpha combine mode
	bool					Orphan;					// When set to true sprites are orphaned outside the usual parent / child hierarchy and layered as independent objects
	bool					Tiled;					// When set to true this sprite wil tile Uv coordinates that outside the normal range
	bool					IgnoreCamera;			// When set to true sprite ignores camera transform
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
		Width = (float)width;
		Height = (float)height;
		TransformDirty = true;
	}
	void			setDepth(float depth)		{ if (depth != Depth) TransformDirty = true; Depth = depth; }
	float			getDestWidth() const		{ return Width; }
	float			getDestHeight() const		{ return Height; }
	float			getDepth() const			{ return Depth; }
	void			setPosAngScale(float x, float y, float angle, float scale)
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
	void			setPosAngScale(float x, float y, float angle, float scale_x, float scale_y)
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
	void			setPosition(float x, float y)
	{
		if (x != Position.x || y != Position.y)
		{
			Position.x = x;
			Position.y = y;
			TransformDirty = true;
		}
	}
	CIwFVec2		getPosition() const			{ return Position; }
	void			setOrigin(float x, float y)
	{
		if (x != Origin.x || y != Origin.y)
		{
			Origin.x = x;
			Origin.y = y;
			TransformDirty = true;
		}
	}
	CIwFVec2		getOrigin() const			{ return Origin; }
	void			setSkew(float x1, float x2, float y1, float y2)
	{
		if (Skew.x != x1 || Skew.y != x2 || Skew.z != y1 || Skew.w != y2)
		{
			Skew.x = x1;
			Skew.y = x2;
			Skew.z = y1;
			Skew.w = y2;
			TransformDirty = true;
		}
	}
	CIwFVec4			getSkew() const			{ return Skew; }
	void			setAngle(float angle)
	{
		if (angle != Angle)
		{
			Angle = angle;
			TransformDirty = true;
		}
	}
	float			getAngle() const			{ return Angle; }		
	void			setScale(float scale)
	{
		if (scale != ScaleX || scale != ScaleY)
		{
			ScaleX = scale;
			ScaleY = scale;
			TransformDirty = true;
		}
	}
	void			setScale(float scale_x, float scale_y)
	{
		if (scale_x != ScaleX || scale_y != ScaleY)
		{
			ScaleX = scale_x;
			ScaleY = scale_y;
			TransformDirty = true;
		}
	}
	float			getScale() const			{ return ScaleX; }
	float			getScaleX() const			{ return ScaleX; }
	float			getScaleY() const			{ return ScaleY; }
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
	CIwFMat2D&		getTransform()				{ if (TransformDirty) RebuildTransform(); return Transform; }
	void			setLinkedTo(CIwGameSprite* sprite);
	CIwGameSprite*	getLinkedTo()				{ return LinkedTo; }
	float			getAccumDepth() const		{ return AccumDepth; }
	bool			isTransformDirty() const	{ return TransformDirty; }
	IIwGameBrush*	getBrush()					{ return Brush; }
	int				getVertexCount() const		{ return VertexCount; }
	void			setClipRect(CIwFVec4& rc);
	CIwFVec4		getClipRect() const			{ return ClipRect; }
	CIwFVec4		getScreenClipRect() const	{ return ScreenClipRect; }
	void			setBeforeChildren(bool before) { BeforeChildren = before; }
	void			setClipChanged();
	CIwFMat2D&		getFinalTransform()			{ return FinalTransform; }
	bool			affectsClip() const			{ return ClipRect.w >= 0; }
	CIwFVec4		FindFirstClipRect();
	CIwFVec4		FindFirstScreenClipRect();
	CIwFVec4		FindFirstScreenClipRect2();
	virtual bool	isClipped();
	CIwFVec2*		getTransformedV()			{ return TransformedV; }
	CIwSVec2*		getScreenV()				{ return ScreenV; }
	void			setAlphaMode(CIwMaterial::AlphaMode mode)	{ AlphaMode = mode; }
	CIwMaterial::AlphaMode	getAlphaMode() const				{ return AlphaMode; }
	void			setOrphan(bool enable)						{ Orphan = enable; }
	bool			getOrphan() const							{ return Orphan; }
	void			setIgnoreCamera(bool enable)				{ IgnoreCamera = enable; }
	bool			getIgnoreCamera() const						{ return IgnoreCamera; }
	void			setTiled(bool tiled)						{ Tiled = tiled; }
	bool			isTiled() const								{ return Tiled; }

	/// Properties End
protected:
	bool			ChildChangeClip;			// Set to true if child changes clip rect
	CIwFMat2D		FinalTransform;				// Final transform
	bool			TransformDirty;				// Dirty when transform changed
	CIwFVec2*		TransformedV;				// Transforrmed vertices (used for hit detection and rendering)
	CIwSVec2*		ScreenV;					// Short vesion of transformed vertices
	float			AccumDepth;					// Accumulated depth
	CIwFVec4		ScreenClipRect;				// Transformed screen clipping rect for this sprite

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
	virtual bool	HitTest(float x, float y);		// Check to see if point is within area covered by transformed sprite
	virtual bool	HitTestNoClip(float x, float y); // Check to see if point is within area covered by transformed sprite
	virtual bool	isOutsideFocusRange(float x, float y, float scale = 1.0f);
	virtual CIwFVec2 TransformPoint(float x, float y);	// Transform point by sprites local angle / scale transform
	virtual CIwFVec2 TransformPointToScreen(float x, float y);	// Transform point by sprites final transform

	virtual void	RebuildTransformNow();		// Rebuilds the display transform immediately

	bool			isClippedByManager(uint16 *indices = NULL, int count = 4);
	bool			SimpleTestOverlap(CIwGameSprite* other);
	bool			TestOverlap(CIwGameSprite* other, int i1, int i2, int i3);
	bool			TestOverlap(CIwGameSprite* other);
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
	CIwSVec2*	getUVList()											{ return UVList; }
	void		setUVList(CIwSVec2* uvs, int num_uvs);					// Replaces the entire UV list (Note that the sprite will take over management of the UV List)
	void		setUVList(CIwSVec2* uvs, int offset, int num_uvs);		// Modifies UV's in an existing UV list
	void		setUV(CIwSVec2* uv, int offset);						// Modifies single UV's in an existing UV list

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
	bool	HitTest(float x, float y);

	bool	isOutsideFocusRange(float x, float y, float scale = 1.0f);
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
	CIwFMat2D					Transform;				// Transform
	CIwFMat2D					TransformNoCamera;		// Transform without camera
	CIwList<CIwGameSprite*>		Sprites;				// Our list of sprites
	CIwGameSlotArray<CIwGameSprite*>*	Layers;			// Visible layers used in depth sorting
	bool						Batching;				// Enable sprite batching
	CIwFVec2					COP;					// Centre of projection
	CIwFVec2					ScreenCentre;			// Screen centre
	CIwFVec4					ScreenClipRect;			// Screen clipping rectangle
	CIwFVec4					ClipRect;				// Clipping rectangle
public:
	void			addSprite(CIwGameSprite* sprite);
	void			removeSprite(CIwGameSprite* sprite, bool delete_sprites = true);
	void			setTransform(const CIwFMat2D& transform)	{ Transform = transform; DirtyChildTransforms(); }
	const CIwFMat2D&	getTransform() const					{ return Transform; }
	void			setTransformNoCamera(const CIwFMat2D& transform)	{ TransformNoCamera = transform; DirtyChildTransforms(); }
	const CIwFMat2D&	getTransformNoCamera() const					{ return TransformNoCamera; }
	void			setBatching(bool batching)				{ Batching = batching; }
	bool			getBatching() const						{ return Batching; }
	void			setCOP(float x, float y)				{ COP.x = x; COP.y = y; }
	CIwFVec2		getCOP() const							{ return COP; }
	void			setScreenCentre(float x, float y)		{ ScreenCentre.x = x; ScreenCentre.y = y; }
	CIwFVec2		getScreenCentre() const					{ return ScreenCentre; }
	void			setScreenClipRect(CIwFVec4& rect)		{ ScreenClipRect = rect; }
	void			setScreenClipRect(float x, float y, float w, float h)	{ ScreenClipRect.x = x; ScreenClipRect.y = y; ScreenClipRect.z = w; ScreenClipRect.w = h; }
	CIwFVec4		getScreenClipRect() const				{ return ScreenClipRect; }
	void			setClipRect(CIwFVec4& rect)				{ ClipRect = rect; }
	void			setClipRect(float x, float y, float w, float h)	{ ClipRect.x = x; ClipRect.y = y; ClipRect.z = w; ClipRect.w = h; }
	CIwFVec4		getClipRect() const						{ return ClipRect; }
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
		Transform.m[0][0] = 1.0f;
		Transform.m[1][1] = 1.0f;
		TransformNoCamera.SetIdentity();
		TransformNoCamera.m[0][0] = 1.0f;
		TransformNoCamera.m[1][1] = 1.0f;
	}
	~CIwGameSpriteManager() { Release(); }

	void			Init(int max_layers = 10);
	void			Draw();
	void			Release(bool delete_sprites = true);
};





#endif // _CIW_GAME_SPRITE_H_
