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

#include "IwGameSprite.h"
#include "IwGameString.h"
#include "IwGameUtil.h"
#include "IwGameRender2d.h"

uint16 CIwGameBitmapSprite9::Indices[] = 
{
	0, 4, 5, 1, 
	1, 5, 6, 2, 
	2, 6, 7, 3, 
	4, 8, 9, 5, 
	5, 9, 10, 6, 
	6, 10, 11, 7, 
	8, 12, 13, 9, 
	9, 13, 14, 10, 
	10, 14, 15, 11, 
};
uint16 CIwGameBitmapSprite9::ClipIndices[] = 
{
	 0, 3, 15, 12 
};

//
//
//
//
// CIwGameSprite implementation
//
//
//
//
void CIwGameSprite::setLinkedTo(CIwGameSprite* sprite)
{
	if (LinkedTo == sprite)
		return;

	// Reove sprite from previous sprites child list or the managers sprite list
	Manager->removeSprite(this, false);
	LinkedTo = sprite;

	if (sprite != NULL)
	{
		// Insert the sprite into the new linked to sprites sprite list
		sprite->addChild(this);
	}
}

void CIwGameSprite::addChild(CIwGameSprite* sprite)
{
	Children.push_back(sprite);
	sprite->setManager(this->Manager);
	sprite->setParent(this);
}

void CIwGameSprite::removeChild(CIwGameSprite* sprite, bool delete_sprites)
{
	for (Iterator it = Children.begin(); it != Children.end(); ++it)
	{
		if (*it == sprite)
		{
			if (delete_sprites && !(*it)->isPooled())
				delete *it;
			Children.erase(it);
			break;
		}
	}
}

void CIwGameSprite::RebuildTransform()
{
	// Build the transform
	AccumDepth = Depth;
	CIwMat2D trans;
	trans.SetIdentity();
	trans.SetTrans(Origin);
	// Set the rotation transform
	Transform.SetRot(Angle);
	// Scale the transform
	Transform.m[0][0] = IW_FIXED_MUL(Transform.m[0][0], ScaleX);
	Transform.m[0][1] = IW_FIXED_MUL(Transform.m[0][1], ScaleX);
	Transform.m[1][1] = IW_FIXED_MUL(Transform.m[1][1], ScaleY);
	Transform.m[1][0] = IW_FIXED_MUL(Transform.m[1][0], ScaleY);
	// Translate the transform
	Transform.SetTrans(Position);
	// Apply origin
	Transform.PreMult(trans);
	// Apply linked sprites transform if linked
	if (LinkedTo != NULL)
	{
		Transform.PostMult(LinkedTo->getTransform());
		AccumDepth = Depth + LinkedTo->getAccumDepth();
	}
/*	else
	{
		// Apply Manager transform if sprite is managed by a Manager sprite manager (doesnt work with none uniform scaling)
		if (Manager != NULL)
			Transform.PostMult(Manager->getTransform());
	}*/
}

bool CIwGameSprite::isClippedByManager(uint16 *indices, int num_verts)
{
	if (Manager == NULL)
		return false;

	CIwRect clip_rect = Manager->getScreenClipRect();
	int left = clip_rect.x;
	int top = clip_rect.y;
	int right = left + clip_rect.w;
	int bottom = top + clip_rect.h;

	int left_off = 0;
	int right_off = 0;
	int top_off = 0;
	int bottom_off = 0;

	for (int t = 0; t < num_verts; t++)
	{
		int x, y;
		if (indices != NULL)
		{
			int i = *(indices + t);
			x = TransformedV[i].x >> 3;
			y = TransformedV[i].y >> 3;
		}
		else
		{
			x = TransformedV[t].x >> 3;
			y = TransformedV[t].y >> 3;
		}

		if (x < left)
			left_off++;
		else
		if (x > right)
			right_off++;

		if (y < top)
			top_off++;
		else
		if (y > bottom)
			bottom_off++;
	}

	if (left_off == num_verts)
		return true;
	if (right_off == num_verts)
		return true;
	if (top_off == num_verts)
		return true;
	if (bottom_off == num_verts)
		return true;

	return false;
}

void CIwGameSprite::RebuildTransformNow()
{
	if (TransformDirty)
	{
		RebuildTransform();
		TransformVertices();
	}
}

void CIwGameSprite::Init(int vertex_count)
{
	Parent = NULL;
	VertexCount = vertex_count;
	ScreenV = new CIwSVec2[VertexCount];
	TransformedV = new CIwVec2[VertexCount];
	for (int t = 0; t < VertexCount; t++)
	{
		ScreenV[t].x = 0;
		ScreenV[t].y = 0;
		TransformedV[t].x = 0;
		TransformedV[t].y = 0;
	}
	Pooled = false;
	Manager = NULL;
	LinkedTo = NULL;
	Brush = NULL;
	TransformDirty = true;
	Position.x = 0;
	Position.y = 0;
	Origin.x = 0;
	Origin.y = 0;
	Angle = 0;
	ScaleX = IW_GEOM_ONE;
	ScaleY = IW_GEOM_ONE;
	Colour.r = 0xff;
	Colour.g = 0xff;
	Colour.b = 0xff;
	Colour.a = 0xff;
	Width = 0;
	Height = 0;
	Depth = IW_GEOM_ONE;
	AccumDepth = Depth;
	Visible = true;
	Layer = 0;
	FinalTransform.SetIdentity();
	Transform.SetIdentity();
	ClipRect.x = 0;
	ClipRect.y = 0;
	ClipRect.w = -1;	// No clipping
	ClipRect.h = -1;
	ScreenClipRect.x = 0;
	ScreenClipRect.y = 0;
	ScreenClipRect.w = 0;
	ScreenClipRect.h = 0;
	Skew.x = 0;
	Skew.y = 0;
	Skew.w = 0;
	Skew.h = 0;
	BeforeChildren = true;
	ChildChangeClip = false;
	Filter = true;
	AlphaMode = CIwMaterial::ALPHA_BLEND;
}

bool CIwGameSprite::isClipped()
{
	CIwRect rc = FindFirstScreenClipRect();
	int x1 = rc.x;
	int x2 = x1 + rc.w;
	int y1 = rc.y;
	int y2 = y1 + rc.h;

	for (int t = 0; t < VertexCount; t++)
	{
		int x = ScreenV[t].x;
		int y = ScreenV[t].y;
		if (x >= x1 && x < x2 && y >= y1 && y < y2)
			return false;
	}

	return true;
}

bool CIwGameSprite::HitTest(int x, int y)
{
	CIwRect rc = FindFirstScreenClipRect();
	if (x < rc.x || x > rc.x + rc.w || y < rc.y || y > rc.y + rc.h)
		return false;

	return HitTestNoClip(x, y);
}

bool CIwGameSprite::HitTestNoClip(int x, int y)
{
	x *= 8;
	y *= 8;

	int i1 = 0;
	int i2 = 3;
	for (int t = 0; t < VertexCount; t++)
	{
		int x0 = TransformedV[i1].x - TransformedV[i2].x;
		int y0 = TransformedV[i1].y - TransformedV[i2].y;
		int x1 = x - TransformedV[i2].x;
		int y1 = y - TransformedV[i2].y;

		if ((x1 * y0 - x0 * y1) >= 0)
			return false;

		i2 = i1;
		i1++;
	}

	return true;
}

bool CIwGameSprite::isOutsideFocusRange(int x, int y, float scale)
{
	int cx = 0;
	int cy = 0;
	int min_x = 999999;
	int max_x = -999999;
	int min_y = 999999;
	int max_y = -999999;
	for (int t = 0; t < 4; t++)
	{
		int mx = TransformedV[t].x >> 3;
		if (mx < min_x)
			min_x = mx;
		if (mx > max_x)
			max_x = mx;
		int my = TransformedV[t].y >> 3;
		if (my < min_y)
			min_y = my;
		if (mx > max_y)
			max_y = my;
		cx  += mx;
		cy  += my;
	}
	cx >>= 2;
	cy >>= 2;

	int dx = x - cx;
	int dy = y - cy;
	int d = (max_x - min_x) * (max_x - min_x) + (max_y - min_y) * (max_y - min_y);
	int d2 = dx * dx + dy * dy;

	return d2 > (int)(d * scale);
}

CIwVec2 CIwGameSprite::TransformPoint(float x, float y)
{
	CIwVec2 trans = CIwVec2((int)x, (int)y);

	return Transform.TransformVec(trans);
}

CIwVec2 CIwGameSprite::TransformPointToScreen(float x, float y)
{
	CIwVec2 trans = CIwVec2((int)x, (int)y);

	return FinalTransform.TransformVec(trans);
}

void CIwGameSprite::UpdateClipping()
{
	if (ClipRect.w <= 0)
		return;

	if (ClipRect.w != 0)
	{
		CIwGameRender2d::SetCurrentClipRect(ScreenClipRect);
//	else
//		CIwGameRender2d::ResetClipRect();
	}

	// Notify parent that we changed the clip rect
	if (Parent != NULL)
		Parent->ChildChangeClip = true;
}

void CIwGameSprite::BuildFinalTransform()
{
	// Apply Manager transform if sprite is managed by a Manager sprite manager (doesnt work with none uniform scaling)
	FinalTransform = Transform;
	if (Manager != NULL)
	{
		if (AccumDepth == IW_GEOM_ONE)
		{
			FinalTransform.PostMult(Manager->getTransform());
		}
		else
		{
			CIwSVec2 cop = Manager->getCOP();
			CIwSVec2 centre = Manager->getScreenCentre();

			CIwMat2D m = Manager->getTransform();
			m.m[0][0] = (m.m[0][0] << IW_GEOM_POINT) / AccumDepth;
			m.m[0][1] = (m.m[0][1] << IW_GEOM_POINT) / AccumDepth;
			m.m[1][0] = (m.m[1][0] << IW_GEOM_POINT) / AccumDepth;
			m.m[1][1] = (m.m[1][1] << IW_GEOM_POINT) / AccumDepth;
			m.t.x = ((m.t.x - centre.x) << IW_GEOM_POINT) / AccumDepth + cop.x;
			m.t.y = ((m.t.y - centre.y) << IW_GEOM_POINT) / AccumDepth + cop.y;

			FinalTransform.PostMult(m);
		}
	}

	// Remove sub-pixel precision (causes nasty artifacts)
//	FinalTransform.t.x &= 0xfffffff8;
//	FinalTransform.t.y &= 0xfffffff8;
//	FinalTransform.t.x = (FinalTransform.t.x << IW_GEOM_POINT) / AccumDepth;
//	FinalTransform.t.y = (FinalTransform.t.y << IW_GEOM_POINT) / AccumDepth;
}

void CIwGameSprite::TransformClipRect()
{
	// Transform clipping rect
	if (ClipRect.w > 0)
	{
		CIwVec2 tl = CIwVec2(ClipRect.x << 3, ClipRect.y << 3);
		CIwVec2 br = CIwVec2((ClipRect.x + ClipRect.w) << 3, (ClipRect.y + ClipRect.h) << 3);
		tl = FinalTransform.TransformVec(tl);
		br = FinalTransform.TransformVec(br);
		int x1 = tl.x >> 3;
		int y1 = tl.y >> 3;
		int x2 = br.x >> 3;
		int y2 = br.y >> 3;

		// Ensure clip rect always shrinks
		CIwRect current = FindFirstScreenClipRect();
		int cx1 = current.x;
		int cy1 = current.y;
		int cx2 = current.x + current.w;
		int cy2 = current.y + current.h;
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
	}
}

bool CIwGameSprite::isVisibleWithParents() const
{
	if (Parent != NULL)
	{
		if (!Parent->isVisibleWithParents())
			return false;
	}

	return Visible;
}

CIwRect CIwGameSprite::FindFirstClipRect()
{
	if (Parent != NULL)
	{
		if (Parent->getClipRect().w > 0)
			return Parent->getClipRect();

		return Parent->FindFirstClipRect();
	}
	else
	if (Manager != NULL)
		return Manager->getClipRect();

	return CIwRect(0, 0, 0, 0);
}

CIwRect CIwGameSprite::FindFirstScreenClipRect()
{
	if (Parent != NULL)
	{
		if (Parent->getClipRect().w > 0)
			return Parent->getScreenClipRect();

		return Parent->FindFirstScreenClipRect();
	}
	else
	if (Manager != NULL)
		return Manager->getScreenClipRect();

	return CIwRect(0, 0, 0, 0);
}

CIwRect CIwGameSprite::FindFirstScreenClipRect2()
{
	if (ScreenClipRect.w > 0)
		return ScreenClipRect;

	if (Parent != NULL)
	{
		return Parent->FindFirstScreenClipRect2();
	}
	else
		return Manager->getScreenClipRect();

	return CIwRect(0, 0, 0, 0);
}

void CIwGameSprite::setClipRect(CIwRect& rc)
{
	ClipRect = rc;
/*	// Ensure clip rect doesnt grow compared to parent
	int x1 = rc.x;
	int y1 = rc.y;
	int x2 = x1 + rc.w;
	int y2 = y1 + rc.h;

	CIwRect current = FindFirstClipRect();
	int cx1, cx2, cy1, cy2;
	cx1 = current.x;
	cy1 = current.y;
	cx2 = current.x + current.w;
	cy2 = current.y + current.h;
	if (x1 > cx1)
		cx1 = x1;
	if (x2 < cx2)
		cx2 = x2;
	if (y1 > cy1)
		cy1 = y1;
	if (y2 < cy2)
		cy2 = y2;

	ClipRect.x = cx1;
	ClipRect.y = cy1;
	ClipRect.w = (cx2 - cx1);
	ClipRect.h = (cy2 - cy1);*/

}

void CIwGameSprite::setClipChanged()
{
	// Notify all parents of clip change
	if (Parent != NULL)
	{
		Parent->ChildChangeClip = true;
		Parent->setClipChanged();
	}
}

void CIwGameSprite::TransformVertices()
{
	BuildFinalTransform();

	TransformClipRect();

	// Generate transformed vertices
	{
		int x0 = -(Width * 4) - IW_GAME_VERT_ADJUST;
		int y0 = -(Height * 4) - IW_GAME_VERT_ADJUST;
		int x1 = (Width * 4) + IW_GAME_VERT_ADJUST;
		int y1 = (Height * 4) + IW_GAME_VERT_ADJUST;
		int sx1 = Skew.x;
		int sx2 = Skew.y;
		int sy1 = Skew.w;
		int sy2 = Skew.h;
		TransformedV[0].x = x0 + sx1;
		TransformedV[1].x = x1 + sx2;
		TransformedV[2].x = x1 - sx2;
		TransformedV[3].x = x0 - sx1;
		TransformedV[0].y = y0 + sy1;
		TransformedV[1].y = y0 - sy1;
		TransformedV[2].y = y1 - sy2;
		TransformedV[3].y = y1 + sy2;
	}

	int m00 = FinalTransform.m[0][0];
	int m01 = FinalTransform.m[0][1];
	int m10 = FinalTransform.m[1][0];
	int m11 = FinalTransform.m[1][1];
	int tx = FinalTransform.t.x;
	int ty = FinalTransform.t.y;
	for (int t = 0; t < VertexCount; t++)
	{
		int x = TransformedV[t].x;
		int y = TransformedV[t].y;
		ScreenV[t].x = TransformedV[t].x = IW_FIXED_MUL2(m00, m10, x, y) + tx, 
		ScreenV[t].y = TransformedV[t].y = IW_FIXED_MUL2(m01, m11, x, y) + ty;
	}

	
}

bool CIwGameSprite::Update()
{
	// Do not render if not visible
	if (!Visible || Colour.a == 0)
		return false;

	if (LinkedTo != NULL)
	{
		if (!LinkedTo->isVisible())
			return false;
		if (LinkedTo->isTransformDirty())
			TransformDirty = true;
	}

	if (TransformDirty)
	{
		RebuildTransform();
		TransformVertices();
	}

	// Update children
	for (Iterator it = Children.begin(); it != Children.end(); ++it)
		(*it)->Update();

	return true;
}

void CIwGameSprite::DrawChildren()
{
	if (Children.size() == 0)
		return;

	CIwRect current_clip = CIwGameRender2d::GetCurrentClipRect();

	// Set current clip rect
	if (ClipRect.w > 0)
		UpdateClipping();

	ChildChangeClip = false;

	CIwRect restored_clip = CIwGameRender2d::GetCurrentClipRect();

	// Draw children
	for (Iterator it = Children.begin(); it != Children.end(); ++it)
	{
		// Draw child
		(*it)->Draw();

		// if child or any of its children modified clipping then restore it
		if (ChildChangeClip)
		{
			if (ClipRect.w > 0)
				CIwGameRender2d::SetCurrentClipRect(restored_clip);
			else
				CIwGameRender2d::SetCurrentClipRect(current_clip);		// We have no clip rect defined at this poibnt so we simply restore the clip rect that was there before we starting drawing the children
			ChildChangeClip = false;
		}
	}

	// Put the original clipping back
	if (ClipRect.w >= 0)
		CIwGameRender2d::SetCurrentClipRect(current_clip);
}


//
//
//
//
// CIwGameDummySprite implementation
//
//
//
//

void CIwGameDummySprite::Draw()
{
	if (!Visible)
		return;

	TransformDirty = false;

	// Draw children
	DrawChildren();
}


//
//
//
//
// CIwGameBitmapSprite implementation
//
//
//
//

void CIwGameBitmapSprite::Init(int vertex_count)
{
	CIwGameSprite::Init(vertex_count);
	UVList = new CIwSVec2[vertex_count];
	Colours = new CIwColour[vertex_count];
}

void CIwGameBitmapSprite::setFromBrush(CIwGameBrushImage* brush)
{
	Brush = brush;
	Image = brush->getImage();
	CIwRect rect = brush->getSrcRect();
	SrcX = rect.x;
	SrcY = rect.y;
	SrcWidth = rect.w;
	SrcHeight = rect.h;
	UVsDirty = true;
}

void CIwGameBitmapSprite::RebuildUVList()
{
	if (Image == NULL)
		return;

	int		w = Image->getWidth();
	int		h = Image->getHeight();
	int		px = ((IW_GEOM_ONE / w) >> 1);
	int		py = ((IW_GEOM_ONE / h) >> 1);
//	int		px = 1;
//	int		py = 1;
	int		x0 = (SrcX * IW_GEOM_ONE) / w, x1 = ((SrcX + SrcWidth) * IW_GEOM_ONE) / w - px;
	int		y0 = (SrcY * IW_GEOM_ONE) / h, y1 = ((SrcY + SrcHeight) * IW_GEOM_ONE) / h - py;
//	int		x0 = (SrcX * IW_GEOM_ONE) / w, x1 = ((SrcX + SrcWidth) * IW_GEOM_ONE) / w;
//	int		y0 = (SrcY * IW_GEOM_ONE) / h, y1 = ((SrcY + SrcHeight) * IW_GEOM_ONE) / h;

	if (ImageTransform & IW2D_FLIP_X_F)
	{
		int t = x0;
		x0 = x1;
		x1 = t;
	}
	if (ImageTransform & IW2D_FLIP_Y_F)
	{
		int t = y0;
		y0 = y1;
		y1 = t;
	}

	UVList[0].x = x0;
	UVList[0].y = y0;
	UVList[1].x = x1;
	UVList[1].y = y0;
	UVList[2].x = x1;
	UVList[2].y = y1;
	UVList[3].x = x0;
	UVList[3].y = y1;
	UVsDirty = false;
}

void CIwGameBitmapSprite::Draw()
{
	if (!Visible)
		return;

	TransformDirty = false;

	if (!BeforeChildren)
		DrawChildren();

	// Do not render if not visible
	if (Image != NULL && Image->getImage2D() != NULL)
	{
		if (!isClippedByManager(NULL, VertexCount))
		{
			if (UVsDirty)
				RebuildUVList();

			if (Manager != NULL &&  Manager->getBatching())
				IW_GAME_RENDER2D->AddPrimtive(TransformedV, UVList, Image, Colours);
			else
				IW_GAME_RENDER2D->DrawImage(ScreenV, UVList, Image, Colours, Filter, AlphaMode);
		}
	}
#if defined (_DEBUG_)
	else
		CIwGameError::LogError("Image is not available!");
#endif // _DEBUG_

	if (BeforeChildren)
		DrawChildren();
}

//
//
//
//
// CIwGameBitmapSprite9 implementation
//
//
//
//

void CIwGameBitmapSprite9::Init(int vertex_count)
{
	CIwGameBitmapSprite::Init(vertex_count);
}

void CIwGameBitmapSprite9::RebuildUVList()
{
	if (Image == NULL)
		return;

	int		w = Image->getWidth();
	int		h = Image->getHeight();
	CIwRect area = ((CIwGameBrushImage9*)Brush)->getScalableArea();
	int		px = ((IW_GEOM_ONE / w) >> 1);
	int		py = ((IW_GEOM_ONE / h) >> 1);
	int		x0 = (SrcX * IW_GEOM_ONE) / w;
	int		x1 = ((SrcX + area.x) * IW_GEOM_ONE) / w;
	int		x2 = ((SrcX + area.x + area.w) * IW_GEOM_ONE) / w - px;
	int		x3 = ((SrcX + SrcWidth) * IW_GEOM_ONE) / w - px;
	int		y0 = (SrcY * IW_GEOM_ONE) / h;
	int		y1 = ((SrcY + area.y) * IW_GEOM_ONE) / h;
	int		y2 = ((SrcY + area.y + area.h) * IW_GEOM_ONE) / h;
	int		y3 = ((SrcY + SrcHeight) * IW_GEOM_ONE) / h - py;

	if (ImageTransform & IW2D_FLIP_X_F)
	{
		int t = x0;
		x0 = x3;
		x3 = t;
		t = x1;
		x1 = x2;
		x2 = t;
	}
	if (ImageTransform & IW2D_FLIP_Y_F)
	{
		int t = y0;
		y0 = y3;
		y3 = t;
		t = y1;
		y1 = y2;
		y2 = t;
	}

	UVList[0].x = x0;
	UVList[4].x = x0;
	UVList[8].x = x0;
	UVList[12].x = x0;
	UVList[1].x = x1;
	UVList[5].x = x1;
	UVList[9].x = x1;
	UVList[13].x = x1;
	UVList[2].x = x2;
	UVList[6].x = x2;
	UVList[10].x = x2;
	UVList[14].x = x2;
	UVList[3].x = x3;
	UVList[7].x = x3;
	UVList[11].x = x3;
	UVList[15].x = x3;

	UVList[0].y = y0;
	UVList[1].y = y0;
	UVList[2].y = y0;
	UVList[3].y = y0;
	UVList[4].y = y1;
	UVList[5].y = y1;
	UVList[6].y = y1;
	UVList[7].y = y1;
	UVList[8].y = y2;
	UVList[9].y = y2;
	UVList[10].y = y2;
	UVList[11].y = y2;
	UVList[12].y = y3;
	UVList[13].y = y3;
	UVList[14].y = y3;
	UVList[15].y = y3;

	UVsDirty = false;

	// We also need to rebuol the transform as the cetrices are based on the textures size
	RebuildTransform();
}

void CIwGameBitmapSprite9::TransformVertices()
{
	BuildFinalTransform();

	TransformClipRect();

	// Calcuklate 9-patch vertices
	int x0 = -(Width * 4) - IW_GAME_VERT_ADJUST;
	int y0 = -(Height * 4) - IW_GAME_VERT_ADJUST;
	int x1 = (Width * 4) + IW_GAME_VERT_ADJUST;
	int y1 = (Height * 4) + IW_GAME_VERT_ADJUST;
	int sw = SrcWidth << 3;
	int sh = SrcHeight << 3;
	CIwRect area = ((CIwGameBrushImage9*)Brush)->getScalableArea();
	area.x <<= 3;
	area.y <<= 3;
	area.w <<= 3;
	area.h <<= 3;

	// Build horizontal vertices
	TransformedV[0].x = x0;
	TransformedV[4].x = x0;
	TransformedV[8].x = x0;
	TransformedV[12].x = x0;
	x0 += area.x;
	TransformedV[1].x = x0;
	TransformedV[5].x = x0;
	TransformedV[9].x = x0;
	TransformedV[13].x = x0;
	TransformedV[3].x = x1;
	TransformedV[7].x = x1;
	TransformedV[11].x = x1;
	TransformedV[15].x = x1;
	x1 -= sw - (area.x + area.w);
	TransformedV[2].x = x1;
	TransformedV[6].x = x1;
	TransformedV[10].x = x1;
	TransformedV[14].x = x1;

	// Build vertical vertices
	TransformedV[0].y = y0;
	TransformedV[1].y = y0;
	TransformedV[2].y = y0;
	TransformedV[3].y = y0;
	y0 += area.y;
	TransformedV[4].y = y0;
	TransformedV[5].y = y0;
	TransformedV[6].y = y0;
	TransformedV[7].y = y0;
	TransformedV[12].y = y1;
	TransformedV[13].y = y1;
	TransformedV[14].y = y1;
	TransformedV[15].y = y1;
	y1 -= sh - (area.y + area.h);
	TransformedV[8].y = y1;
	TransformedV[9].y = y1;
	TransformedV[10].y = y1;
	TransformedV[11].y = y1;

	int m00 = FinalTransform.m[0][0];
	int m01 = FinalTransform.m[0][1];
	int m10 = FinalTransform.m[1][0];
	int m11 = FinalTransform.m[1][1];
	int tx = FinalTransform.t.x;
	int ty = FinalTransform.t.y;
	for (int t = 0; t < VertexCount; t++)
	{
		int x = TransformedV[t].x;
		int y = TransformedV[t].y;
		ScreenV[t].x = TransformedV[t].x = IW_FIXED_MUL2(m00, m10, x, y) + tx, 
		ScreenV[t].y = TransformedV[t].y = IW_FIXED_MUL2(m01, m11, x, y) + ty;
	}
}

void CIwGameBitmapSprite9::Draw()
{
	if (!Visible)
		return;

	TransformDirty = false;

	if (!BeforeChildren)
		DrawChildren();

	// Do not render if not visible
	if (Image != NULL && Image->getImage2D() != NULL)
	{
		if (!isClippedByManager(&ClipIndices[0], 4))
		{
			if (UVsDirty)
				RebuildUVList();

//			if (Manager != NULL &&  Manager->getBatching())
//				IW_GAME_RENDER2D->AddPrimtive(TransformedV, UVList, Image, Colours);
//			else
				IW_GAME_RENDER2D->DrawSharedImages(ScreenV, UVList, 16, 9, Indices, Image, Colours, Filter, AlphaMode);
		}
	}

	if (BeforeChildren)
		DrawChildren();
}

bool CIwGameBitmapSprite9::isClipped()
{
	CIwRect rc = FindFirstScreenClipRect();
	int x1 = rc.x;
	int x2 = x1 + rc.w;
	int y1 = rc.y;
	int y2 = y1 + rc.h;

	int i1 = 0;
	int i2 = 3;
	for (int t = 0; t < VertexCount; t++)
	{
		int ii1 = ClipIndices[i1];
		int ii2 = ClipIndices[i2];
		int x = ScreenV[ii1].x;
		int y = ScreenV[ii2].y;
		if (x >= x1 && x < x2 && y >= y1 && y < y2)
			return false;
	}

	return true;
}

bool CIwGameBitmapSprite9::HitTest(int x, int y)
{
	CIwRect rc = FindFirstScreenClipRect2();
	if (x < rc.x || x > rc.x + rc.w || y < rc.y || y > rc.y + rc.h)
		return false;

	x *= 8;
	y *= 8;

	int i1 = 0;
	int i2 = 3;
	for (int t = 0; t < 4; t++)
	{
		int ii1 = ClipIndices[i1];
		int ii2 = ClipIndices[i2];
		int x0 = TransformedV[ii1].x - TransformedV[ii2].x;
		int y0 = TransformedV[ii1].y - TransformedV[ii2].y;
		int x1 = x - TransformedV[ii2].x;
		int y1 = y - TransformedV[ii2].y;

		if ((x1 * y0 - x0 * y1) >= 0)
			return false;

		i2 = i1;
		i1++;
	}

	return true;
}

bool CIwGameBitmapSprite9::isOutsideFocusRange(int x, int y, float scale)
{
	int cx = 0;
	int cy = 0;
	int min_x = 999999;
	int max_x = -999999;
	int min_y = 999999;
	int max_y = -999999;
	int idx[4] = { 0, 3, 15, 12 };

	for (int t = 0; t < 4; t++)
	{
		int i = idx[t];
		int mx = TransformedV[i].x >> 3;
		if (mx < min_x)
			min_x = mx;
		if (mx > max_x)
			max_x = mx;
		int my = TransformedV[i].y >> 3;
		if (my < min_y)
			min_y = my;
		if (mx > max_y)
			max_y = my;
		cx  += mx;
		cy  += my;
	}
	cx >>= 2;
	cy >>= 2;

	int dx = x - cx;
	int dy = y - cy;
	int d = (max_x - min_x) * (max_x - min_x) + (max_y - min_y) * (max_y - min_y);
	int d2 = dx * dx + dy * dy;

	return d2 > (int)(d * scale);
}

//
//
//
//
// CIwGameTextSprite implementation
//
//
//
//
void CIwGameTextSprite::RebuildTransform()
{
	// Build the transform
	AccumDepth = Depth;
	CIwMat2D trans;
	trans.SetIdentity();
	trans.SetTrans(Origin);
	trans.t.x -= Rect.w << 2;
	trans.t.y -= Rect.h << 2;
	// Set the rotation transform
	Transform.SetRot(Angle);
	// Scale the transform
	Transform.m[0][0] = IW_FIXED_MUL(Transform.m[0][0], ScaleX);
	Transform.m[0][1] = IW_FIXED_MUL(Transform.m[0][1], ScaleX);
	Transform.m[1][1] = IW_FIXED_MUL(Transform.m[1][1], ScaleY);
	Transform.m[1][0] = IW_FIXED_MUL(Transform.m[1][0], ScaleY);
	// Translate the transform
	Transform.SetTrans(Position);
	// Apply origin
	Transform.PreMult(trans);
	// Apply linked sprites transform if linked
	if (LinkedTo != NULL)
	{
		Transform.PostMult(LinkedTo->getTransform());
		AccumDepth = Depth + LinkedTo->getAccumDepth();
	}
/*	else
	{
		// Apply Manager transform if sprite is managed by a Manager sprite manager (doesnt work with none uniform scaling)
		if (Manager != NULL)
			Transform.PostMult(Manager->getTransform());
	}*/
}

void CIwGameTextSprite::TransformVertices()
{
	// Apply Manager transform if sprite is managed by a Manager sprite manager (doesnt work with none uniform scaling)
	FinalTransform = Transform;
	if (Manager != NULL)
	{
		if (AccumDepth == IW_GEOM_ONE)
		{
			FinalTransform.PostMult(Manager->getTransform());
		}
		else
		{
			CIwSVec2 cop = Manager->getCOP();
			CIwSVec2 centre = Manager->getScreenCentre();

			CIwMat2D m = Manager->getTransform();
			m.m[0][0] = (m.m[0][0] << IW_GEOM_POINT) / AccumDepth;
			m.m[0][1] = (m.m[0][1] << IW_GEOM_POINT) / AccumDepth;
			m.m[1][0] = (m.m[1][0] << IW_GEOM_POINT) / AccumDepth;
			m.m[1][1] = (m.m[1][1] << IW_GEOM_POINT) / AccumDepth;
			m.t.x = ((m.t.x - centre.x) << IW_GEOM_POINT) / AccumDepth + cop.x;
			m.t.y = ((m.t.y - centre.y) << IW_GEOM_POINT) / AccumDepth + cop.y;

			FinalTransform.PostMult(m);
		}
	}

	TransformClipRect();

	// Generate transformed vertices
	int x0 = -4;
	int y0 = -4;
	int x1 = (Rect.w * 8) + 4;
	int y1 = (Rect.h * 8) + 4;
	TransformedV[0].x = x0;
	TransformedV[1].x = x1;
	TransformedV[2].x = x1;
	TransformedV[3].x = x0;
	TransformedV[0].y = y0;
	TransformedV[1].y = y0;
	TransformedV[2].y = y1;
	TransformedV[3].y = y1;
	for (int t = 0; t < 4; t++)
		TransformedV[t] = FinalTransform.TransformVec(TransformedV[t]);
}

void CIwGameTextSprite::RebuildText()
{
	if (Font == NULL || Text.c_str() == NULL || Font->getFont() == NULL)
		return;

	IwGxFontSetFont(Font->getFont());
	IwGxFontSetFlags(Flags);
	IwGxFontSetRect(Rect);
	IwGxFontSetAlignmentHor(AlignH);
	IwGxFontSetAlignmentVer(AlignV);

	IwGxFontSetScale(IW_GEOM_ONE);
    IwGxFontPrepareText(PreparedText, Text.c_str());

	TextDirty = false;
}

void CIwGameTextSprite::Draw()
{
	if (!Visible)
		return;

	TransformDirty = false;

	if (!BeforeChildren)
		DrawChildren();

	if (!isClippedByManager())
	{
		// Do not render if not visible
		if (Font != NULL && Text.c_str() != NULL && Font->getFont() != NULL)
		{
			IwGxFontSetFont(Font->getFont());
			IwGxFontSetFlags(Flags);
			IwGxFontSetRect(Rect);
			IwGxFontSetAlignmentHor(AlignH);
			IwGxFontSetAlignmentVer(AlignV);

			if (TextDirty)
				RebuildText();

			// Cannot batch as we cannot get access to glyph UV coordinates
			IW_GAME_RENDER2D->DrawText(&PreparedText, Font, &FinalTransform, Colour, Skew, Filter, AlphaMode);
		}
	}

	if (BeforeChildren)
		DrawChildren();
}

//
//
//
// CIwGameSpriteManager implementation
//
//
//
void CIwGameSpriteManager::addSprite(CIwGameSprite* sprite)
{
	Sprites.push_back(sprite);
	sprite->setManager(this);
}

void CIwGameSpriteManager::removeSprite(CIwGameSprite* sprite, bool delete_sprites)
{
	CIwGameSprite* parent = sprite->getParent();
	if (parent != NULL)
		parent->removeChild(sprite, delete_sprites);
	else
	{
		for (Iterator it = Sprites.begin(); it != Sprites.end(); ++it)
		{
			if (*it == sprite)
			{
				if (delete_sprites && !(*it)->isPooled())
					delete *it;
				Sprites.erase(it);
				break;
			}
		}
	}
}

void CIwGameSpriteManager::Init(int max_layers)
{
	// Allocate layers
	Layers = new CIwGameSlotArray<CIwGameSprite*>[max_layers];
	MaxLayers = max_layers;
}

void CIwGameSpriteManager::Draw()
{
	CIwGameRender2d::SetCurrentClipRect(ScreenClipRect.x, ScreenClipRect.y, ScreenClipRect.w, ScreenClipRect.h);
//	bool sub_pixel = Iw2DGetSubPixelCoords();

	// Enable sub pixel coordinates
//	Iw2DSetSubPixelCoords(true);

	// Clear layers
	ClearLayers();

	// Update and organise sprites into layers
	for (Iterator it = Sprites.begin(); it != Sprites.end(); ++it)
	{
		if ((*it)->Update())
		{
			int layer = (*it)->getLayer();
			if (layer < 0) layer = 0;
			if (layer >= MaxLayers) layer = MaxLayers - 1;
			(Layers + layer)->add(*it);
		}
	}

	// Draw sprite layers
	CIwGameSlotArray<CIwGameSprite*>* layers = Layers;
	for (int t = 0; t < MaxLayers; t++)
	{
		int last_used = layers->getLastUsedSlot();
		for (int t2 = 0; t2 <= last_used; t2++)
		{
			CIwGameSprite* sprite = layers->element_at(t2);
			if (sprite != NULL)
				sprite->Draw();
		}

		if (Batching)
			IW_GAME_RENDER2D->BatchDrawPrims();
		layers++;
	}

//	Iw2DSetSubPixelCoords(sub_pixel);
}

void CIwGameSpriteManager::Release(bool delete_sprites)
{
	if (delete_sprites)
	{
		// Delete all sprites in the sprite manager
		for (Iterator it = Sprites.begin(); it != Sprites.end(); ++it)
		{
			if (!(*it)->isPooled())
				delete *it;
		}
	}
	Sprites.clear();

	// Clean up layers
	SAFE_DELETE_ARRAY(Layers)
}

void CIwGameSpriteManager::DirtyChildTransforms()
{
	// Force all childreen to rebuild their transforms
	for (Iterator it = Sprites.begin(); it != Sprites.end(); ++it)
	{
		(*it)->forceTransformDirty();
	}
}

void CIwGameSpriteManager::ClearLayers()
{
	for (int t = 0; t < MaxLayers; t++)
	{
		(Layers + t)->clear();
	}
}



