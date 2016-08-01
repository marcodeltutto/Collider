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

#include "IwGameRender2d.h"
#include "IwGameFont.h"

uint16 CIwGameRender2d::Indices[] = { 0, 3, 1, 2 };
CIwRect	CIwGameRender2d::ScreenClipRect = CIwRect(0, 0, 0, 0);
bool CIwGameRender2d::ClipRectModified = false;


//
//
//
// CIwGameRedner2d implementation
//
// 
//
CDECLARE_SINGLETON(CIwGameRender2d)

void CIwGameRender2d::Begin()
{
	CurrentTexture = NULL;
	CurrentAlphaMode = CIwMaterial::ALPHA_NONE;
	RedundantTextureCalls = 0;
}

void CIwGameRender2d::End()
{
//	CIwGameError::LogError("RedundantTextureCalls - ", CIwGameString(RedundantTextureCalls).c_str());
}

void CIwGameRender2d::Init(int max_primitives, int max_materials)
{
	Primitives = new CIwGameRender2dPrim[max_primitives]();
	NextPrimitive = 0;
	MaxPrimitives = max_primitives;
	Materials = new CIwGameRender2dMaterial[max_materials]();
	NextMaterial = 0;
	MaxMaterials = max_materials;
	MaterialUsedCounts = new int [max_materials];
	for (int t = 0; t < max_materials; t++)
		MaterialUsedCounts[t] = 0;
	MaterialIndices = new int [max_materials];	
	for (int t = 0; t < max_materials; t++)
		MaterialIndices[t] = 0;
	BatchVerts = new CIwSVec2*[max_materials];
	BatchUVs = new CIwSVec2*[max_materials];
	BatchColours = new CIwColour*[max_materials];
	BatchIndices = new uint16*[max_materials];
}

void CIwGameRender2d::Release()
{
	SAFE_DELETE_ARRAY(Primitives)
	SAFE_DELETE_ARRAY(Materials)
	SAFE_DELETE(MaterialUsedCounts)
	SAFE_DELETE(MaterialIndices)
	SAFE_DELETE(BatchVerts)
	SAFE_DELETE(BatchUVs)
	SAFE_DELETE(BatchColours)
	SAFE_DELETE(BatchIndices)
}

void CIwGameRender2d::DrawImage(CIwSVec2* verts, CIwSVec2* uvs, CIwGameImage* image, CIwColour* colours, bool filter, CIwMaterial::AlphaMode alpha_mode)
{
	IwGxSetUVStream(uvs, 0, 4);
	IwGxSetVertStreamScreenSpaceSubPixel(verts, 4);
	IwGxSetColStream(colours);
	IwGxSetNormStream(NULL);

	CIwTexture* texture = image->getImage2D()->GetMaterial()->GetTexture();
	if (CurrentTexture == NULL || texture != CurrentTexture || CurrentAlphaMode != alpha_mode)
	{
		CIwMaterial* mat = IW_GX_ALLOC_MATERIAL();
		mat->SetTexture(image->getImage2D()->GetMaterial()->GetTexture());
		mat->SetDepthWriteMode(CIwMaterial::DEPTH_WRITE_DISABLED);
//		mat->SetClamping(true);
		if (image->isFilterSet())
			mat->SetFiltering(texture->GetFiltering());
		else
			mat->SetFiltering(filter);
		mat->SetAlphaMode(alpha_mode);
		IwGxSetMaterial(mat);
		CurrentTexture = texture;
		CurrentAlphaMode = alpha_mode;
	}
	else
	{
		RedundantTextureCalls++;
	}
	IwGxDrawPrims(IW_GX_QUAD_STRIP, CIwGameRender2d::Indices, 4);
}

void CIwGameRender2d::DrawImage(CIwVec2* verts, CIwSVec2* uvs, CIwGameImage* image, CIwColour* colours, bool filter, CIwMaterial::AlphaMode alpha_mode)
{
	CIwSVec2* v = IW_GX_ALLOC(CIwSVec2, 4);
	for (int t = 0; t < 4; t++)
	{
		v[t].x = verts[t].x;
		v[t].y = verts[t].y;
	}
	DrawImage(v, uvs, image, colours, filter, alpha_mode);
}

void CIwGameRender2d::DrawSharedImages(CIwSVec2* verts, CIwSVec2* uvs, int num_vertices, int num_polygons, uint16* indices, CIwGameImage* image, CIwColour* colours, bool filter, CIwMaterial::AlphaMode alpha_mode)
{
	IwGxSetUVStream(uvs, 0, num_vertices);
	IwGxSetVertStreamScreenSpaceSubPixel(verts, num_vertices);
	IwGxSetColStream(colours);
	IwGxSetNormStream(NULL);

	CIwTexture* texture = image->getImage2D()->GetMaterial()->GetTexture();
	if (CurrentTexture == NULL || texture != CurrentTexture || CurrentAlphaMode != alpha_mode)
	{
		CIwMaterial* mat = IW_GX_ALLOC_MATERIAL();
		mat->SetTexture(image->getImage2D()->GetMaterial()->GetTexture());
		mat->SetDepthWriteMode(CIwMaterial::DEPTH_WRITE_DISABLED);
//		mat->SetClamping(true);
		if (image->isFilterSet())
			mat->SetFiltering(texture->GetFiltering());
		else
			mat->SetFiltering(filter);
		mat->SetAlphaMode(alpha_mode);
		IwGxSetMaterial(mat);
		CurrentTexture = texture;
	}
	else
	{
		RedundantTextureCalls++;
	}
	IwGxDrawPrims(IW_GX_QUAD_LIST, indices, num_polygons << 2);
}

void CIwGameRender2d::DrawSharedImages(CIwVec2* verts, CIwSVec2* uvs, int num_vertices, int num_polygons, uint16* indices, CIwGameImage* image, CIwColour* colours, bool filter, CIwMaterial::AlphaMode alpha_mode)
{
	CIwSVec2* v = IW_GX_ALLOC(CIwSVec2, num_vertices);
	for (int t = 0; t < num_vertices; t++)
	{
		v[t].x = verts[t].x;
		v[t].y = verts[t].y;
	}
	DrawSharedImages(v, uvs, num_vertices, num_polygons, indices, image, colours, filter, alpha_mode);
}

void CIwGameRender2d::DrawText(CIwGxFontPreparedData* prepared_text, CIwGameFont* font, CIwMat2D* transform, CIwColour& colour, CIwRect& skew, bool filter, CIwMaterial::AlphaMode alpha_mode)
{
	// Cache the transform
	int m00 = transform->m[0][0];
	int m01 = transform->m[0][1];
	int m10 = transform->m[1][0];
	int m11 = transform->m[1][1];
	int tx = transform->t.x;
	int ty = transform->t.y;
	int sx1 = skew.x;
	int sx2 = skew.y;
	int sy1 = skew.w;
	int sy2 = skew.h;
	CIwRect clip = ScreenClipRect;
	clip.x <<= 3;
	clip.y <<= 3;
	clip.w = clip.x + (clip.w << 3);
	clip.h = clip.y + (clip.h << 3);

	IwGxLightingOn();
	IwGxSetColStream(NULL);
	IwGxSetNormStream(NULL);

	// A font can consist of multiple materials so we need to process all of them
	for (int t = 0; t < font->getFont()->GetNumberMaterials(); t++)
    {
		// Set UV stream
		uint32* char_ids;
		int num_chars = IwGxFontSetUVs(*prepared_text, -1, t, &char_ids);

		// Generate transformed vertices from glyphs
		int nv = num_chars << 2;
        CIwSVec2* pVerts = IW_GX_ALLOC(CIwSVec2, nv);
        CIwSVec2* pVert = pVerts;
		if (m01 == 0 && m10 == 0)
		{
			// No rotation optimisation
			for (int t2 = 0; t2 < num_chars; t2++)
			{
				CIwRect rect = prepared_text->GetCharacterArea(char_ids[t2]);
				int x1 = (rect.x << 3) - IW_GAME_VERT_ADJUST;
				int y1 = (rect.y << 3) - IW_GAME_VERT_ADJUST;
				int x2 = x1 + (rect.w << 3) + IW_GAME_VERT_ADJUST;
				int y2 = y1 + (rect.h << 3) + IW_GAME_VERT_ADJUST;
				int ax = ((m00 * x1) >> IW_GEOM_POINT) + tx;
				int ay = ((m11 * y1) >> IW_GEOM_POINT) + ty;
				int bx = ((m00 * x2) >> IW_GEOM_POINT) + tx;
				int by = ((m11 * y2) >> IW_GEOM_POINT) + ty;

//				if ((ax < clip.w && bx >= clip.x) && (ay < clip.h && by >= clip.y))
//				{
					pVert->x = ax + sx1;
					pVert->y = ay + sy1;
					pVert++;
					pVert->x = ax - sx1;
					pVert->y = by + sy2;
					pVert++;
					pVert->x = bx - sx2;
					pVert->y = by - sy2;
					pVert++;
					pVert->x = bx + sx2;
					pVert->y = ay - sy1;
					pVert++;
//				}
			}
		}
		else
		{
			for (int t2 = 0; t2 < num_chars; t2++)
			{
				CIwRect rect = prepared_text->GetCharacterArea(char_ids[t2]);
				int x1 = rect.x << 3;
				int y1 = rect.y << 3;
				int x2 = x1 + (rect.w << 3);
				int y2 = y1 + (rect.h << 3);
				pVert->x = IW_FIXED_MUL2(m00, m10, x1, y1) + tx + sx1;
				pVert->y = IW_FIXED_MUL2(m01, m11, x1, y1) + ty + sy1;
				pVert++;
				pVert->x = IW_FIXED_MUL2(m00, m10, x1, y2) + tx - sx1;
				pVert->y = IW_FIXED_MUL2(m01, m11, x1, y2) + ty + sy2;
				pVert++;
				pVert->x = IW_FIXED_MUL2(m00, m10, x2, y2) + tx - sx2;
				pVert->y = IW_FIXED_MUL2(m01, m11, x2, y2) + ty - sy2;
				pVert++;
				pVert->x = IW_FIXED_MUL2(m00, m10, x2, y1) + tx + sx2;
				pVert->y = IW_FIXED_MUL2(m01, m11, x2, y1) + ty - sy1;
				pVert++;
			}
		}

		if (nv > 0)
		{
			// Set vertex stream
			IwGxSetVertStreamScreenSpaceSubPixel(pVerts, nv);

			// Create a material
			CIwMaterial* mat = IW_GX_ALLOC_MATERIAL();
			mat->Copy(*IwGxFontGetFont()->GetMaterial(t));
			mat->SetDepthWriteMode(CIwMaterial::DEPTH_WRITE_DISABLED);
			mat->SetColEmissive(colour);
//			mat->SetClamping(true);
			mat->SetFiltering(filter);
			mat->SetAlphaMode(alpha_mode);
			IwGxSetMaterial(mat);

			// Finally draw the glyphs
			IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, nv);
		}
    }
	IwGxLightingOff();
	CurrentTexture = NULL;
}


int CIwGameRender2d::AddMaterial(CIwGameImage* image)
{
	// If material already exists then return it
	for (int t = 0; t < NextMaterial; t++)
	{
		if ((Materials + t)->Image == image)
		{
			MaterialUsedCounts[t]++;
			return t;
		}
	}

	// Material not found so add it
#if defined(_DEBUG)
	if (NextMaterial >= MaxMaterials - 1)
	{
		CIwGameError::LogError("CIwGameRender2d - Ran out of materials, please increase maximum in CIwGameRender2d::Init()");
		return - 1;
	}
#endif

	MaterialUsedCounts[NextMaterial] = 1;
	CIwGameRender2dMaterial* mat = Materials + NextMaterial++;
	mat->Image = image;

	return NextMaterial - 1;
}

void CIwGameRender2d::AddPrimtive(CIwVec2* verts, CIwSVec2* uvs, CIwGameImage* image, CIwColour* colours)
{
#if defined(_DEBUG)
	if (NextPrimitive >= MaxPrimitives - 1)
	{
		CIwGameError::LogError("CIwGameRender2d - Ran out of primitives, please increase maximum in CIwGameRender2d::Init()");
		return;
	}
#endif
	CIwGameRender2dPrim* prim = Primitives + NextPrimitive++;
	prim->Verts = verts;
	prim->UVs = uvs;
	prim->Colours = colours;
	prim->Material = AddMaterial(image);
}

void CIwGameRender2d::BatchDrawPrims(bool filter)
{
	if (NextMaterial == 0)
		return;

	// Allocate memory from data cache for verts, UV's and colours
	for (int t = 0; t < NextMaterial; t++)
	{
		int prim_count = MaterialUsedCounts[t] * 4;
		BatchVerts[t] = IW_GX_ALLOC(CIwSVec2, prim_count);
		BatchUVs[t] = IW_GX_ALLOC(CIwSVec2, prim_count);
		BatchColours[t] = IW_GX_ALLOC(CIwColour, prim_count);
		BatchIndices[t] = IW_GX_ALLOC(uint16, prim_count);
	}

	// Populate the data cache
	CIwGameRender2dPrim* prims = Primitives;
	for (int t = 0; t < NextPrimitive; t++)
	{
		int mat_id = prims->Material;
		int idx = MaterialIndices[mat_id];
		CIwSVec2* v = BatchVerts[mat_id] + idx;
		CIwSVec2* uv = BatchUVs[mat_id] + idx;
		CIwColour* c = BatchColours[mat_id] + idx;
		uint16* i = BatchIndices[mat_id] + idx;
		for (int t2 = 0; t2 < 4; t2++)
		{
			v->x = (prims->Verts + t2)->x;
			v->y = (prims->Verts + t2)->y;
			uv->x = (prims->UVs + t2)->x;
			uv->y = (prims->UVs + t2)->y;
			c->Set(*(prims->Colours + t2));
			v++; uv++; c++;
		}
		*i++ = idx;
		*i++ = idx + 3;
		*i++ = idx + 2;
		*i++ = idx + 1;
		MaterialIndices[mat_id] += 4;
		prims++;
	}

	// Render batched streams
	for (int t = 0; t < NextMaterial; t++)
	{
		int count = MaterialUsedCounts[t] * 4;
		IwGxSetUVStream(BatchUVs[t], 0, count);
		IwGxSetVertStreamScreenSpaceSubPixel(BatchVerts[t], count);
		IwGxSetColStream(BatchColours[t], count);
		IwGxSetNormStream(NULL);
		CIwMaterial* mat = IW_GX_ALLOC_MATERIAL();
		mat->SetTexture(Materials[t].Image->getImage2D()->GetMaterial()->GetTexture());
		mat->SetDepthWriteMode(CIwMaterial::DEPTH_WRITE_DISABLED);
		mat->SetClamping(true);
		mat->SetFiltering(filter);
		mat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
		IwGxSetMaterial(mat);
		IwGxDrawPrims(IW_GX_QUAD_LIST, BatchIndices[t], count);
	}

	// Reset batch pointers
	for (int t = 0; t < NextMaterial; t++)
		MaterialUsedCounts[t] = 0;
	for (int t = 0; t < NextMaterial; t++)
		MaterialIndices[t] = 0;
	NextPrimitive = 0;
	NextMaterial = 0;
}


void CIwGameRender2d::ResetClipRect()
{
	if (ScreenClipRect.w != 0)
	{
		IwGxClearScissorScreenSpace();
		ScreenClipRect.w = 0;
		ClipRectModified = true;
	}
}
void CIwGameRender2d::SetCurrentClipRect(CIwRect& rc)
{
	SetCurrentClipRect(rc.x, rc.y, rc.w, rc.h);
}

void CIwGameRender2d::SetCurrentClipRect(int x, int y, int w, int h)
{
	if (x != ScreenClipRect.x || y != ScreenClipRect.y || w != ScreenClipRect.w || h != ScreenClipRect.h)
	{
//		if (w == 0)
//			ResetClipRect();
//		else
		{
			ScreenClipRect.x = x;
			ScreenClipRect.y = y;
			ScreenClipRect.w = w;
			ScreenClipRect.h = h;
//			IwGxClearScissorScreenSpace();
			IwGxSetScissorScreenSpace(x, y, w, h);
			ClipRectModified = true;
		}
	}
}

void CIwGameRender2d::UpdateClipRect(CIwRect& rc)
{
	UpdateClipRect(rc.x, rc.y, rc.w, rc.h);
}
void CIwGameRender2d::UpdateClipRect(int x, int y, int w, int h)
{
	int x1 = x;
	int y1 = y;
	int x2 = x + w;
	int y2 = y + h;
	int cx1 = ScreenClipRect.x;
	int cy1 = ScreenClipRect.y;
	int cx2 = ScreenClipRect.x + ScreenClipRect.w;
	int cy2 = ScreenClipRect.y + ScreenClipRect.h;

	if (x1 > cx1)
		cx1 = x1;
	if (x2 < cx2)
		cx2 = x2;
	if (y1 > cy1)
		cy1 = y1;
	if (y2 < cy2)
		cy2 = y2;

	ScreenClipRect.x = cx1;
	ScreenClipRect.y = cy1;
	ScreenClipRect.w = (cx2 - cx1);
	ScreenClipRect.h = (cy2 - cy1);
	IwGxSetScissorScreenSpace(ScreenClipRect.x, ScreenClipRect.y, ScreenClipRect.w, ScreenClipRect.h);
	ClipRectModified = true;
}

bool CIwGameRender2d::isFullyClipped(CIwRect &rc)
{
	int left = ScreenClipRect.x;
	int top = ScreenClipRect.y;
	int right = left + ScreenClipRect.w;
	int bottom = top + ScreenClipRect.h;
	int x1 = rc.x;
	int x2 = x1 + rc.w;
	int y1 = rc.y;
	int y2 = y1 + rc.h;

	if (x2 < left || x1 > right || y2 < top || y2 > bottom)
		return true;

	return false;
}













