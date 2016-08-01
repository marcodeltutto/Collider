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

#if !defined(_IW_GAME_RENDER_H_)
#define _IW_GAME_RENDER_H_


#include "IwGx.h"
#include "IwGxFont.h"

#include "IwGameUtil.h"
#include "IwGameImage.h"

#define IW_GAME_VERT_ADJUST	0

class CIwGameFont;

struct CIwGameRender2dMaterial
{
	CIwGameImage*	Image;
};

struct CIwGameRender2dPrim
{
	CIwVec2*		Verts;
	CIwSVec2*		UVs;
	int				Material;
	CIwColour*		Colours;
};

//
//
//
// CIwGameRedner2d - Class that performs 2D rendering
//
// 
//
class CIwGameRender2d
{
public:
	CDEFINE_SINGLETON(CIwGameRender2d)

protected:
	// Properties
	CIwGameRender2dPrim*		Primitives;
	int							NextPrimitive;
	int							MaxPrimitives;
	CIwGameRender2dMaterial*	Materials;
	int*						MaterialUsedCounts;
	int							NextMaterial;
	int							MaxMaterials;
	int*						MaterialIndices;
	CIwSVec2**					BatchVerts;
	CIwSVec2**					BatchUVs;
	CIwColour**					BatchColours;
	uint16**					BatchIndices;
public:
	// Properties end

protected:
	static bool				ClipRectModified;		// true if clipping rect has been modified
	static CIwRect			ScreenClipRect;			// Current transformed screen clipping rect
	static uint16			Indices[4];
	int						AddMaterial(CIwGameImage* image);
	CIwTexture*				CurrentTexture;
	CIwMaterial::AlphaMode	CurrentAlphaMode;
	int						RedundantTextureCalls;
	
public:
	void			Init(int max_primitives = 1024, int max_materials = 100);
	void			Release();

	void			DrawImage(CIwSVec2* verts, CIwSVec2* uvs, CIwGameImage* image, CIwColour* colours, bool filter = true, CIwMaterial::AlphaMode alpha_mode = CIwMaterial::ALPHA_BLEND);
	void			DrawImage(CIwVec2* verts, CIwSVec2* uvs, CIwGameImage* image, CIwColour* colours, bool filter = true, CIwMaterial::AlphaMode alpha_mode = CIwMaterial::ALPHA_BLEND);
	void			DrawSharedImages(CIwSVec2* verts, CIwSVec2* uvs, int num_vertices,  int num_polygons, uint16* indices, CIwGameImage* image, CIwColour* colours, bool filter = true, CIwMaterial::AlphaMode alpha_mode = CIwMaterial::ALPHA_BLEND);
	void			DrawSharedImages(CIwVec2* verts, CIwSVec2* uvs, int num_vertices,  int num_polygons, uint16* indices, CIwGameImage* image, CIwColour* colours, bool filter = true, CIwMaterial::AlphaMode alpha_mode = CIwMaterial::ALPHA_BLEND);

	void			DrawText(CIwGxFontPreparedData* prepared_text, CIwGameFont* font, CIwMat2D* transform, CIwColour& colour, CIwRect& skew, bool filter = true, CIwMaterial::AlphaMode alpha_mode = CIwMaterial::ALPHA_BLEND);

	void			AddPrimtive(CIwVec2* verts, CIwSVec2* uvs, CIwGameImage* image, CIwColour* colours);
	void			BatchDrawPrims(bool filter = true);

	void			Begin();
	void			End();

	// Global clipping
	static void		SetCurrentClipRect(CIwRect& rc);
	static void		SetCurrentClipRect(int x, int y, int w, int h);
	static void		UpdateClipRect(CIwRect& rc);
	static void		UpdateClipRect(int x, int y, int w, int h);
	static CIwRect	GetCurrentClipRect()					{ return ScreenClipRect; }
	static void		ResetClipRect();
	static bool		isFullyClipped(CIwRect& rc);
	static void		setClipRectModified(bool modified)		{ ClipRectModified = modified; }
	static bool		wasClipRectModified()					{ return ClipRectModified; }
};

#define IW_GAME_RENDER2D		CIwGameRender2d::getInstance()

#endif	// _IW_GAME_RENDER_H_
