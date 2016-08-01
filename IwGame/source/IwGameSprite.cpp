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

	if (!Orphan)
	{
		// Reove sprite from previous sprites child list or the managers sprite list
		Manager->removeSprite(this, false);

		if (sprite != NULL)	
		{
			// Insert the sprite into the new linked to sprites sprite list
			sprite->addChild(this);
		}
	}

	LinkedTo = sprite;
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
	CIwFMat2D trans;
	trans.SetIdentity();
	trans.SetTrans(Origin);
	// Set the rotation transform
	Transform.SetRot(Angle);
	// Scale the transform
	Transform.m[0][0] *= ScaleX;
	Transform.m[0][1] *= ScaleX;
	Transform.m[1][1] *= ScaleY;
	Transform.m[1][0] *= ScaleY;
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

bool CIwGameSprite::isClippedByManager(uint16 *indices, int count)
{
	if (Manager == NULL)
		return false;

	CIwFVec4 clip_rect = Manager->getScreenClipRect();
	float left = clip_rect.x;
	float top = clip_rect.y;
	float right = left + clip_rect.z;
	float bottom = top + clip_rect.w;

	int left_off = 0;
	int right_off = 0;
	int top_off = 0;
	int bottom_off = 0;

	for (int t = 0; t < count; t++)
	{
		float x, y;
		if (indices != NULL)
		{
			int i = *(indices + t);
			x = TransformedV[i].x;
			y = TransformedV[i].y;
		}
		else
		{
			x = TransformedV[t].x;
			y = TransformedV[t].y;
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

	if (left_off == count)
		return true;
	if (right_off == count)
		return true;
	if (top_off == count)
		return true;
	if (bottom_off == count)
		return true;

	return false;
}

bool CIwGameSprite::TestOverlap(CIwGameSprite* other, int i1, int i2, int i3)
{
	float x1 = TransformedV[i1].x;
	float y1 = TransformedV[i1].y;
	float x2 = TransformedV[i2].x;
	float y2 = TransformedV[i2].y;
	float x3 = TransformedV[i3].x;
	float y3 = TransformedV[i3].y;

	float v1x = x2 - x1;
	float v1y = y2 - y1;
	float rx = -v1y;
	float ry = v1x;
	bool ref_side = (rx * (x3 - x1) + ry * (y3 - y1)) >= 0;

	CIwFVec2* other_v = other->getTransformedV();
	int vc = other->getVertexCount();
	if (vc == 16)
	{
		int indices[4] = { 0, 3, 15, 12 };
		for (int t = 0; t < 4; t++)
		{
			float x = (other_v + indices[t])->x;
			float y = (other_v + indices[t])->y;

			bool side = (rx * (x - x1) + ry * (y - y1)) >= 0;

			if (side == ref_side)
				return false;
		}
	}
	else
	{
		for (int t = 0; t < vc; t++)
		{
			float x = other_v->x;
			float y = other_v->y;
			bool side = (rx * (x - x1) + ry * (y - y1)) >= 0;

			if (side == ref_side)
				return false;
			other_v++;
		}
	}

	return true;
}

bool CIwGameSprite::SimpleTestOverlap(CIwGameSprite* other)
{
	float x1 = TransformedV[0].x;
	float y1 = TransformedV[0].y;
	float dx, dy;
	CIwFVec2* other_v = other->getTransformedV();
	float x2 = other_v->x;
	float y2 = other_v->y;
	float dx2, dy2;

	if (VertexCount == 16)
	{
		dx = TransformedV[3].x - x1;
		dy = TransformedV[15].y - y1;
	}
	else
	{
		dx = TransformedV[1].x - x1;
		dy = TransformedV[3].y - y1;
	}
	if (other->getVertexCount() == 16)
	{
		dx2 = (other_v + 3)->x - x2;
		dy2 = (other_v + 15)->y - y2;
	}
	else
	{
		dx2 = (other_v + 1)->x - x2;
		dy2 = (other_v + 3)->y - y2;
	}

	if ((x2 > x1 && x2 < (x1 + dx)) || ((x2 + dx2) > x1 && (x2 + dx2) < (x1 + dx)))
	{
		if ((y2 > y1 && y2 < (y1 + dy)) || ((y2 + dy2) > y1 && (y2 + dy2) < (y1 + dy)))
			return true;
	}

	return false;
}

bool CIwGameSprite::TestOverlap(CIwGameSprite* other)
{
	if (Angle == 0 && other->getAngle() == 0)
	{
		return SimpleTestOverlap(other);
	}

	int i1 = 0;
	int i2, i3, i4;

	if (VertexCount == 16)
	{
		i2 = 3;
		i3 = 15;
		i4 = 12;
	}
	else
	{
		i2 = 1;
		i3 = 2;
		i4 = 3;
	}
	if (TestOverlap(other, i1, i2, i3))
		return false;
	if (TestOverlap(other, i1, i4, i3))
		return false;
	if (TestOverlap(other, i4, i3, i1))
		return false;
	if (TestOverlap(other, i3, i2, i1))
		return false;

	if (other->getVertexCount() == 16)
	{
		i2 = 3;
		i3 = 15;
		i4 = 12;
	}
	else
	{
		i2 = 1;
		i3 = 2;
		i4 = 3;
	}

	if (other->TestOverlap(this, i1, i2, i3))
		return false;
	if (other->TestOverlap(this, i1, i4, i3))
		return false;
	if (other->TestOverlap(this, i4, i3, i1))
		return false;
	if (other->TestOverlap(this, i3, i2, i1))
		return false;

	return true;
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
	TransformedV = new CIwFVec2[VertexCount];
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
	ScaleX = 1.0f;
	ScaleY = 1.0f;
	Colour.r = 0xff;
	Colour.g = 0xff;
	Colour.b = 0xff;
	Colour.a = 0xff;
	Width = 0;
	Height = 0;
	Depth = 1.0f;
	AccumDepth = Depth;
	Visible = true;
	Layer = 0;
	FinalTransform.SetIdentity();
	Transform.SetIdentity();
	ClipRect.x = 0;
	ClipRect.y = 0;
	ClipRect.z = -1;	// No clipping
	ClipRect.w = -1;
	ScreenClipRect.x = 0;
	ScreenClipRect.y = 0;
	ScreenClipRect.z = 0;
	ScreenClipRect.w = 0;
	Skew.x = 0;
	Skew.y = 0;
	Skew.z = 0;
	Skew.w = 0;
	BeforeChildren = true;
	ChildChangeClip = false;
	Filter = true;
	Orphan = false;
	IgnoreCamera = false;
	Tiled = false;
	AlphaMode = CIwMaterial::ALPHA_BLEND;
}

bool CIwGameSprite::isClipped()
{
	CIwFVec4 rc = FindFirstScreenClipRect();
	float x1 = rc.x;
	float x2 = x1 + rc.z;
	float y1 = rc.y;
	float y2 = y1 + rc.w;

	for (int t = 0; t < VertexCount; t++)
	{
		float x = ScreenV[t].x;
		float y = ScreenV[t].y;
		if (x >= x1 && x < x2 && y >= y1 && y < y2)
			return false;
	}

	return true;
}

bool CIwGameSprite::HitTest(float x, float y)
{
	CIwFVec4 rc = FindFirstScreenClipRect();
	if (x < rc.x || x > rc.x + rc.z || y < rc.y || y > rc.y + rc.w)
		return false;

	return HitTestNoClip(x, y);
}

bool CIwGameSprite::HitTestNoClip(float x, float y)
{
	int i1 = 0;
	int i2 = 3;
	for (int t = 0; t < VertexCount; t++)
	{
		float x0 = TransformedV[i1].x - TransformedV[i2].x;
		float y0 = TransformedV[i1].y - TransformedV[i2].y;
		float x1 = x - TransformedV[i2].x;
		float y1 = y - TransformedV[i2].y;

		if ((x1 * y0 - x0 * y1) >= 0)
			return false;

		i2 = i1;
		i1++;
	}

	return true;
}

bool CIwGameSprite::isOutsideFocusRange(float x, float y, float scale)
{
	float cx = 0;
	float cy = 0;
	float min_x = 99999999.0f;
	float max_x = -99999999.0f;
	float min_y = 99999999.0f;
	float max_y = -99999999.0f;
	for (int t = 0; t < 4; t++)
	{
		float mx = TransformedV[t].x;
		if (mx < min_x)
			min_x = mx;
		if (mx > max_x)
			max_x = mx;
		float my = TransformedV[t].y;
		if (my < min_y)
			min_y = my;
		if (my > max_y)
			max_y = my;
		cx  += mx;
		cy  += my;
	}
	cx /= 4.0f;
	cy /= 4.0f;

	float dx = x - cx;
	float dy = y - cy;
	float d = (max_x - min_x) * (max_x - min_x) + (max_y - min_y) * (max_y - min_y);
	float d2 = dx * dx + dy * dy;

	return d2 > (d * scale);
}

CIwFVec2 CIwGameSprite::TransformPoint(float x, float y)
{
	CIwFVec2 trans = CIwFVec2(x, y);

	return Transform.TransformVec(trans);
}

CIwFVec2 CIwGameSprite::TransformPointToScreen(float x, float y)
{
	CIwFVec2 trans = CIwFVec2(x, y);

	return FinalTransform.TransformVec(trans);
}

void CIwGameSprite::UpdateClipping()
{
	if (ClipRect.w <= 0)
		return;

	if (ClipRect.w != 0)
	{
		CIwGameRender2d::SetCurrentClipRect((int)ScreenClipRect.x, (int)ScreenClipRect.y, (int)ScreenClipRect.z, (int)ScreenClipRect.w);
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
		if (AccumDepth == 1.0f || AccumDepth == 0)
		{
			if (IgnoreCamera)
				FinalTransform.PostMult(Manager->getTransformNoCamera());
			else
				FinalTransform.PostMult(Manager->getTransform());
		}
		else
		{
			CIwFVec2 cop = Manager->getCOP();
			CIwFVec2 centre = Manager->getScreenCentre();

			CIwFMat2D m;
			if (IgnoreCamera)
				m = Manager->getTransformNoCamera();
			else
				m = Manager->getTransform();
			m.m[0][0] /= AccumDepth;
			m.m[0][1] /= AccumDepth;
			m.m[1][0] /= AccumDepth;
			m.m[1][1] /= AccumDepth;
			m.t.x = (m.t.x - centre.x) / AccumDepth + cop.x;
			m.t.y = (m.t.y - centre.y) / AccumDepth + cop.y;

			FinalTransform.PostMult(m);
		}
	}
}

void CIwGameSprite::TransformClipRect()
{
	// Transform clipping rect
	if (ClipRect.w > 0)
	{
		CIwFVec2 tl = CIwFVec2(ClipRect.x, ClipRect.y);
		CIwFVec2 br = CIwFVec2((ClipRect.x + ClipRect.z), (ClipRect.y + ClipRect.w));
		tl = FinalTransform.TransformVec(tl);
		br = FinalTransform.TransformVec(br);
		float x1 = tl.x;
		float y1 = tl.y;
		float x2 = br.x;
		float y2 = br.y;

		// Ensure clip rect always shrinks
		CIwFVec4 current = FindFirstScreenClipRect();
		float cx1 = current.x;
		float cy1 = current.y;
		float cx2 = current.x + current.z;
		float cy2 = current.y + current.w;
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
		ScreenClipRect.z = (cx2 - cx1);
		ScreenClipRect.w = (cy2 - cy1);
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

CIwFVec4 CIwGameSprite::FindFirstClipRect()
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

	return CIwFVec4(0, 0, 0, 0);
}

CIwFVec4 CIwGameSprite::FindFirstScreenClipRect()
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

	return CIwFVec4(0, 0, 0, 0);
}

CIwFVec4 CIwGameSprite::FindFirstScreenClipRect2()
{
	if (ScreenClipRect.w > 0)
		return ScreenClipRect;

	if (Parent != NULL)
	{
		return Parent->FindFirstScreenClipRect2();
	}
	else
		return Manager->getScreenClipRect();

	return CIwFVec4(0, 0, 0, 0);
}

void CIwGameSprite::setClipRect(CIwFVec4& rc)
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
		float x0 = -Width / 2.0f;
		float y0 = -Height / 2.0f;
		float x1 = Width / 2.0f;
		float y1 = Height / 2.0f;
		float sx1 = Skew.x;
		float sx2 = Skew.y;
		float sy1 = Skew.z;
		float sy2 = Skew.w;
		TransformedV[0].x = x0 + sx1;
		TransformedV[1].x = x1 + sx2;
		TransformedV[2].x = x1 - sx2;
		TransformedV[3].x = x0 - sx1;
		TransformedV[0].y = y0 + sy1;
		TransformedV[1].y = y0 - sy1;
		TransformedV[2].y = y1 - sy2;
		TransformedV[3].y = y1 + sy2;
	}

	float m00 = FinalTransform.m[0][0];
	float m01 = FinalTransform.m[0][1];
	float m10 = FinalTransform.m[1][0];
	float m11 = FinalTransform.m[1][1];
	float tx = FinalTransform.t.x;
	float ty = FinalTransform.t.y;
	for (int t = 0; t < VertexCount; t++)
	{
		float x = TransformedV[t].x;
		float y = TransformedV[t].y;
		float sx = TransformedV[t].x = ((m00 * x) + (m10 * y)) + tx;
		float sy = TransformedV[t].y = ((m01 * x) + (m11 * y)) + ty;
		ScreenV[t].x = (int16)(sx * 8.0f);
		ScreenV[t].y = (int16)(sy * 8.0f);
	}

	
}

bool CIwGameSprite::Update()
{
	// Do not render if not visible
//	if (!Visible || Colour.a == 0)
	if (!Visible)
		return false;

	bool dirty = TransformDirty;

	if (Orphan)
	{
		if (LinkedTo != NULL)
		{
			if (!LinkedTo->isVisible())
				return false;
			if (LinkedTo->isTransformDirty())
				TransformDirty = true;
		}
	}

	if (TransformDirty)
	{
		RebuildTransform();
		TransformVertices();
	}

	// Update children
	for (Iterator it = Children.begin(); it != Children.end(); ++it)
	{
		if (dirty)
			(*it)->forceTransformDirty();
		(*it)->Update();
	}

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
	Tiled = brush->isTiled();
	CIwSVec2* uvs = brush->getUVList();
	if (uvs != NULL)
	{
		setUVList(uvs, 0, VertexCount);
		UVsDirty = false;
	}
	else
	{
		CIwRect rect = brush->getSrcRect();
		SrcX = rect.x;
		SrcY = rect.y;
		SrcWidth = rect.w;
		SrcHeight = rect.h;
		UVsDirty = true;
	}
}

void CIwGameBitmapSprite::setUVList(CIwSVec2* uvs, int num_uvs)
{
	SAFE_DELETE(UVList);

	UVList = uvs;
	UVsDirty = false;
}

void CIwGameBitmapSprite::setUVList(CIwSVec2* uvs, int offset, int num_uvs)
{
	if (offset + num_uvs > VertexCount)
		return;

	for (int t = offset; t < offset + num_uvs; t++)
	{
		UVList[t].x = uvs->x;
		UVList[t].y = uvs->y;
		uvs++;
	}
}

void CIwGameBitmapSprite::setUV(CIwSVec2* uv, int offset)
{
	if (offset >= VertexCount)
		return;

	UVList[offset].x = uv->x;
	UVList[offset].y = uv->y;
}

void CIwGameBitmapSprite::RebuildUVList()
{
	if (Image == NULL)
		return;

	int		w = Image->getWidth();
	int		h = Image->getHeight();
//	int		px = ((IW_GEOM_ONE / w) >> 1);
//	int		py = ((IW_GEOM_ONE / h) >> 1);
	int		px = 0;
	int		py = 0;
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
	if (Image != NULL && Image->getTexture() != NULL)
	{
		if (!isClippedByManager(NULL, VertexCount))
		{
			if (UVsDirty)
				RebuildUVList();

			if (Manager != NULL && Manager->getBatching())
				IW_GAME_RENDER2D->AddPrimtive(ScreenV, UVList, Image, Colours);
			else
				IW_GAME_RENDER2D->DrawImage(ScreenV, UVList, Image, Colours, Filter, Tiled, AlphaMode);
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
	float x0 = -Width / 2.0f;
	float y0 = -Height / 2.0f;
	float x1 = Width / 2.0f;
	float y1 = Height / 2.0f;
	float sw = (float)SrcWidth;
	float sh = (float)SrcHeight;
	CIwRect area = ((CIwGameBrushImage9*)Brush)->getScalableArea();

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
	x1 -= sw - (float)(area.x + area.w);
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
	y1 -= sh - (float)(area.y + area.h);
	TransformedV[8].y = y1;
	TransformedV[9].y = y1;
	TransformedV[10].y = y1;
	TransformedV[11].y = y1;

	float m00 = FinalTransform.m[0][0];
	float m01 = FinalTransform.m[0][1];
	float m10 = FinalTransform.m[1][0];
	float m11 = FinalTransform.m[1][1];
	float tx = FinalTransform.t.x;
	float ty = FinalTransform.t.y;
	for (int t = 0; t < VertexCount; t++)
	{
		float x = TransformedV[t].x;
		float y = TransformedV[t].y;
		float sx = TransformedV[t].x = ((m00 * x) + (m10 * y)) + tx;
		float sy = TransformedV[t].y = ((m01 * x) + (m11 * y)) + ty;
		ScreenV[t].x = (int16)(sx * 8.0f);
		ScreenV[t].y = (int16)(sy * 8.0f);
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
	if (Image != NULL && Image->getTexture() != NULL)
	{
		if (!isClippedByManager(&ClipIndices[0], 4))
		{
			if (UVsDirty)
				RebuildUVList();

//			if (Manager != NULL &&  Manager->getBatching())
//				IW_GAME_RENDER2D->AddPrimtive(ScreenV, UVList, Image, Colours);
//			else
				IW_GAME_RENDER2D->DrawSharedImages(ScreenV, UVList, 16, 9, Indices, Image, Colours, Filter, Tiled, AlphaMode);
		}
	}

	if (BeforeChildren)
		DrawChildren();
}

bool CIwGameBitmapSprite9::isClipped()
{
	CIwFVec4 rc = FindFirstScreenClipRect();
	float x1 = rc.x;
	float x2 = x1 + rc.z;
	float y1 = rc.y;
	float y2 = y1 + rc.w;

	int i1 = 0;
	int i2 = 3;
	for (int t = 0; t < VertexCount; t++)
	{
		int ii1 = ClipIndices[i1];
		int ii2 = ClipIndices[i2];
		float x = (float)ScreenV[ii1].x;
		float y = (float)ScreenV[ii2].y;
		if (x >= x1 && x < x2 && y >= y1 && y < y2)
			return false;
	}

	return true;
}

bool CIwGameBitmapSprite9::HitTest(float x, float y)
{
	CIwFVec4 rc = FindFirstScreenClipRect2();
	if (x < rc.x || x > rc.x + rc.z || y < rc.y || y > rc.y + rc.w)
		return false;

	int i1 = 0;
	int i2 = 3;
	for (int t = 0; t < 4; t++)
	{
		int ii1 = ClipIndices[i1];
		int ii2 = ClipIndices[i2];
		float x0 = TransformedV[ii1].x - TransformedV[ii2].x;
		float y0 = TransformedV[ii1].y - TransformedV[ii2].y;
		float x1 = x - TransformedV[ii2].x;
		float y1 = y - TransformedV[ii2].y;

		if ((x1 * y0 - x0 * y1) >= 0)
			return false;

		i2 = i1;
		i1++;
	}

	return true;
}

bool CIwGameBitmapSprite9::isOutsideFocusRange(float x, float y, float scale)
{
	float cx = 0;
	float cy = 0;
	float min_x = 99999999.0f;
	float max_x = -99999999.0f;
	float min_y = 99999999.0f;
	float max_y = -99999999.0f;
	int idx[4] = { 0, 3, 15, 12 };

	for (int t = 0; t < 4; t++)
	{
		int i = idx[t];
		float mx = TransformedV[i].x;
		if (mx < min_x)
			min_x = mx;
		if (mx > max_x)
			max_x = mx;
		float my = TransformedV[i].y;
		if (my < min_y)
			min_y = my;
		if (my > max_y)
			max_y = my;
		cx  += mx;
		cy  += my;
	}
	cx /= 4;
	cy /= 4;

	float dx = x - cx;
	float dy = y - cy;
	float d = (max_x - min_x) * (max_x - min_x) + (max_y - min_y) * (max_y - min_y);
	float d2 = dx * dx + dy * dy;

	return d2 > d * scale;
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
	CIwFMat2D trans;
	trans.SetIdentity();
	trans.SetTrans(Origin);
	trans.t.x -= (float)(Rect.w >> 1);
	trans.t.y -= (float)(Rect.h >> 1);
	// Set the rotation transform
	Transform.SetRot(Angle);
	// Scale the transform
	Transform.m[0][0] *= ScaleX;
	Transform.m[0][1] *= ScaleX;
	Transform.m[1][1] *= ScaleY;
	Transform.m[1][0] *= ScaleY;
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
		if (AccumDepth == 1.0f || AccumDepth == 0)
		{
			if (IgnoreCamera)
				FinalTransform.PostMult(Manager->getTransformNoCamera());
			else
				FinalTransform.PostMult(Manager->getTransform());
		}
		else
		{
			CIwFVec2 cop = Manager->getCOP();
			CIwFVec2 centre = Manager->getScreenCentre();

			CIwFMat2D m;
			if (IgnoreCamera)
				m = Manager->getTransformNoCamera();
			else
				m = Manager->getTransform();
			m.m[0][0] /= AccumDepth;
			m.m[0][1] /= AccumDepth;
			m.m[1][0] /= AccumDepth;
			m.m[1][1] /= AccumDepth;
			m.t.x = (m.t.x - centre.x) / AccumDepth + cop.x;
			m.t.y = (m.t.y - centre.y) / AccumDepth + cop.y;

			FinalTransform.PostMult(m);
		}
	}

	TransformClipRect();

	// Generate transformed vertices
	float x0 = 0;
	float y0 = 0;
	float x1 = (float)Rect.w;
	float y1 = (float)Rect.h;
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
	CIwGameRender2d::SetCurrentClipRect((int)ScreenClipRect.x, (int)ScreenClipRect.y, (int)ScreenClipRect.z, (int)ScreenClipRect.w);
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



