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

// TODO:
// * Replace parameters in DrawImage and DrawSharedImages with a rendering struct
// * Add DrawSharedImages to batch rendering system
// * Add support for rendering none textured polygons
// * Add support for rendering polygons with more than 4 sides
// * Filtering and Tiling need to be taken into account when swapping current material
// * Filtering and Tiling need to integrated into the batching system
// * Replace all Marmalade IwGx functions with Open GL native functions

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

void CIwGameRender2d::DrawImage(CIwSVec2* verts, CIwSVec2* uvs, CIwGameImage* image, CIwColour* colours, bool filter, bool tiled, CIwMaterial::AlphaMode alpha_mode)
{
	IwGxSetUVStream(uvs, 0, 4);
	IwGxSetVertStreamScreenSpaceSubPixel(verts, 4);
	IwGxSetColStream(colours);
	IwGxSetNormStream(NULL);

	CIwTexture* texture = image->getTexture();
	if (CurrentTexture == NULL || texture != CurrentTexture || CurrentAlphaMode != alpha_mode)
	{
		CIwMaterial* mat = IW_GX_ALLOC_MATERIAL();
		mat->SetTexture(image->getTexture());
		mat->SetDepthWriteMode(CIwMaterial::DEPTH_WRITE_DISABLED);
		mat->SetClamping(!tiled);
		if (image->isFilterSet())
			mat->SetFiltering(texture->GetFiltering());
		else
			mat->SetFiltering(filter);
		mat->SetAlphaMode(alpha_mode);
		mat->SetCullMode(CIwMaterial::CULL_NONE);
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

void CIwGameRender2d::DrawImage(CIwFVec2* verts, CIwSVec2* uvs, CIwGameImage* image, CIwColour* colours, bool filter, bool tiled, CIwMaterial::AlphaMode alpha_mode)
{
	CIwSVec2* v = IW_GX_ALLOC(CIwSVec2, 4);
	for (int t = 0; t < 4; t++)
	{
		v[t].x = (int)verts[t].x;
		v[t].y = (int)verts[t].y;
	}
	DrawImage(v, uvs, image, colours, filter, tiled, alpha_mode);
}

void CIwGameRender2d::DrawSharedImages(CIwSVec2* verts, CIwSVec2* uvs, int num_vertices, int num_polygons, uint16* indices, CIwGameImage* image, CIwColour* colours, bool filter, bool tiled, CIwMaterial::AlphaMode alpha_mode)
{
	IwGxSetUVStream(uvs, 0, num_vertices);
	IwGxSetVertStreamScreenSpaceSubPixel(verts, num_vertices);
	IwGxSetColStream(colours);
	IwGxSetNormStream(NULL);

	CIwTexture* texture = image->getTexture();
	if (CurrentTexture == NULL || texture != CurrentTexture || CurrentAlphaMode != alpha_mode)
	{
		CIwMaterial* mat = IW_GX_ALLOC_MATERIAL();
		mat->SetTexture(image->getTexture());
		mat->SetDepthWriteMode(CIwMaterial::DEPTH_WRITE_DISABLED);
		mat->SetClamping(!tiled);
		if (image->isFilterSet())
			mat->SetFiltering(texture->GetFiltering());
		else
			mat->SetFiltering(filter);
		mat->SetAlphaMode(alpha_mode);
		mat->SetCullMode(CIwMaterial::CULL_NONE);
		IwGxSetMaterial(mat);
		CurrentTexture = texture;
	}
	else
	{
		RedundantTextureCalls++;
	}
	IwGxDrawPrims(IW_GX_QUAD_LIST, indices, num_polygons << 2);
}

void CIwGameRender2d::DrawSharedImages(CIwFVec2* verts, CIwSVec2* uvs, int num_vertices, int num_polygons, uint16* indices, CIwGameImage* image, CIwColour* colours, bool filter, bool tiled, CIwMaterial::AlphaMode alpha_mode)
{
	CIwSVec2* v = IW_GX_ALLOC(CIwSVec2, num_vertices);
	for (int t = 0; t < num_vertices; t++)
	{
		v[t].x = (int)verts[t].x;
		v[t].y = (int)verts[t].y;
	}
	DrawSharedImages(v, uvs, num_vertices, num_polygons, indices, image, colours, filter, tiled, alpha_mode);
}

void CIwGameRender2d::DrawText(CIwGxFontPreparedData* prepared_text, CIwGameFont* font, CIwFMat2D* transform, CIwColour& colour, CIwFVec4& skew, bool filter, CIwMaterial::AlphaMode alpha_mode)
{
	// Cache the transform
	float m00 = transform->m[0][0];
	float m01 = transform->m[0][1];
	float m10 = transform->m[1][0];
	float m11 = transform->m[1][1];
	float tx = transform->t.x * 8.0f;
	float ty = transform->t.y * 8.0f;
	float sx1 = (float)skew.x;
	float sx2 = (float)skew.y;
	float sy1 = (float)skew.z;
	float sy2 = (float)skew.w;
/*	CIwRect clip = ScreenClipRect;
	clip.x <<= 3;
	clip.y <<= 3;
	clip.w = clip.x + (clip.w << 3);
	clip.h = clip.y + (clip.h << 3);*/

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
				float x1 = (float)(rect.x << 3);
				float y1 = (float)(rect.y << 3);
				float x2 = x1 + (float)(rect.w << 3);
				float y2 = y1 + (float)(rect.h << 3);
				float ax = (m00 * x1) + tx;
				float ay = (m11 * y1) + ty;
				float bx = (m00 * x2) + tx;
				float by = (m11 * y2) + ty;

//				if ((ax < clip.w && bx >= clip.x) && (ay < clip.h && by >= clip.y))
//				{
					pVert->x = (int)(ax + sx1);
					pVert->y = (int)(ay + sy1);
					pVert++;
					pVert->x = (int)(ax - sx1);
					pVert->y = (int)(by + sy2);
					pVert++;
					pVert->x = (int)(bx - sx2);
					pVert->y = (int)(by - sy2);
					pVert++;
					pVert->x = (int)(bx + sx2);
					pVert->y = (int)(ay - sy1);
					pVert++;
//				}
			}
		}
		else
		{
			for (int t2 = 0; t2 < num_chars; t2++)
			{
				CIwRect rect = prepared_text->GetCharacterArea(char_ids[t2]);
				float x1 = (float)(rect.x << 3);
				float y1 = (float)(rect.y << 3);
				float x2 = x1 + (float)(rect.w << 3);
				float y2 = y1 + (float)(rect.h << 3);
				pVert->x = (int)(m00 * x1 + m10 * y1 + tx + sx1);
				pVert->y = (int)(m01 * x1 + m11 * y1 + ty + sy1);
				pVert++;
				pVert->x = (int)(m00 * x1 + m10 * y2 + tx - sx1);
				pVert->y = (int)(m01 * x1 + m11 * y2 + ty + sy2);
				pVert++;
				pVert->x = (int)(m00 * x2 + m10 * y2 + tx - sx2);
				pVert->y = (int)(m01 * x2 + m11 * y2 + ty - sy2);
				pVert++;
				pVert->x = (int)(m00 * x2 + m10 * y1 + tx + sx2);
				pVert->y = (int)(m01 * x2 + m11 * y1 + ty - sy1);
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
			mat->SetClamping(true);
			mat->SetFiltering(filter);
			mat->SetAlphaMode(alpha_mode);
			mat->SetCullMode(CIwMaterial::CULL_NONE);
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

void CIwGameRender2d::AddPrimtive(CIwSVec2* verts, CIwSVec2* uvs, CIwGameImage* image, CIwColour* colours)
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
		mat->SetTexture(Materials[t].Image->getTexture());
		mat->SetDepthWriteMode(CIwMaterial::DEPTH_WRITE_DISABLED);
		mat->SetClamping(true);
		mat->SetFiltering(filter);
		mat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
		mat->SetCullMode(CIwMaterial::CULL_NONE);
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













