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

#include "IwGameUITextView.h"

//
//
//
//
//
// CIwGameUITextView implementation
//
//
//
//
//

CIwGameUITextView::CIwGameUITextView() : CIwGameUILabel(), Zoom(1.0f), MinZoom(1.f), MaxZoom(1.0f), PrevZoom(0)
{
	setActualClassType("textview");
	PreviousDist = 1.0f;
	ZoomValid = false;
	CompoundScale = 1.0f;
	CompoundPosition.x = 0;
	CompoundPosition.y = 0;
	PanVelocity.x = 0;
	PanVelocity.y = 0;
	NumTouches = 0;
	TouchIndices[0] = -1;
	TouchIndices[1] = -1;
}

void CIwGameUITextView::setArea(CIwVec2& size)
{
	Area = size;
}

void CIwGameUITextView::setZoom(float scale)
{
	if (scale < MinZoom)
		scale = MinZoom;
	if (scale > MaxZoom)
		scale = MaxZoom;

	float scale2 = scale / CompoundScale;
	float tx = Midpoint.x - Midpoint.x * scale2;
	float ty = Midpoint.y - Midpoint.y * scale2;
	CompoundPosition.x = (scale2 * CompoundPosition.x) + tx;
	CompoundPosition.y = (scale2 * CompoundPosition.y) + ty;


	Zoom = scale;
	CompoundScale = scale;
	TextActor->setScale(Zoom);
}

bool CIwGameUITextView::InitView(bool native_res, float min_scale, float max_scale)
{
	TextActor->setTappable(false);
#if defined(_DEBUG)
	CIwGameString name = Name;
	name += "text";
	TextActor->setName(name.c_str());
#endif	// _DEBUG

	Area = TextActor->getSize();

	MinZoom = min_scale;
	MaxZoom = max_scale;

	return true;
}

bool CIwGameUITextView::Update(float dt)
{
	if (!IsVisible)
		return CIwGameUILabel::Update(dt);

	// Set clip rect for the list box
	CIwFVec4 rc;
	rc.x = (float)((-Size.x >> 1) + ClippingMargin.x);
	rc.y = (float)((-Size.y >> 1) + ClippingMargin.w);
	rc.z = (float)(Size.x - (ClippingMargin.x + ClippingMargin.y));
	rc.w = (float)(Size.y - (ClippingMargin.w + ClippingMargin.h));
	Visual->setClipRect(rc);

	// Get touches
	CIwGameTouch* touch1 = IW_GAME_INPUT->getTouch(TouchIndices[0]);
	CIwGameTouch* touch2 = IW_GAME_INPUT->getTouch(TouchIndices[1]);

	// Prevent dragging of control whilst manipulating the contained image
	if (touch1 != NULL || touch2 != NULL)
	{
		bool draggable = IsDraggable;
		IsDraggable = false;
		if (!CIwGameUILabel::Update(dt))
			return false;
		IsDraggable = draggable;
	}
	else
	{
		if (!CIwGameUILabel::Update(dt))
			return false;
	}

	PrevZoom = Zoom;

	// Convert touches to camera coords
	CIwFVec2 p1 = touch1 == NULL ? CIwFVec2(0, 0) : Scene->ScreenToCamera((float)touch1->x, (float)touch1->y);
	CIwFVec2 p2 = touch2 == NULL ? CIwFVec2(0, 0) : Scene->ScreenToCamera((float)touch2->x, (float)touch2->y);
	p1.x -= (Position.x);
	p1.y -= (Position.y);
	p2.x -= (Position.x);
	p2.y -= (Position.y);

	CIwFMat2D& transform = Scene->getVirtualTransform();
	if ((touch1 != NULL && touch1->active) && (touch2 == NULL || !touch2->active))
	{
		// Handle panning
		if (InitialScrollPos.x < -10000)
			InitialScrollPos = ScrollPos;
		CIwVec2 drag_delta = IW_GAME_INPUT->getDragDelta(0);
		if (drag_delta.x != 0 || drag_delta.y != 0)
		{
			CIwFMat2D& transform2 = Visual->getFinalTransform();
			CIwFVec2 drag = CIwFVec2(((float)drag_delta.x) / transform2.m[0][0], ((float)drag_delta.y) / transform2.m[1][1]);
			if (drag.x >= -1 && drag.x <= 1)
				drag.x = 0;
			if (drag.y >= -1 && drag.y <= 1)
				drag.y = 0;
			PanVelocity.x = drag.x;
			PanVelocity.y = drag.y;
			if (drag.x != 0)
			{
				CompoundPosition.x += (float)drag.x;
				PanVelocity.x = (float)-drag.x / dt;
			}
			if (drag.y != 0)
			{
				CompoundPosition.y += (float)drag.y;
				PanVelocity.y = (float)-drag.y / dt;
			}
		}
	}
	else
	{
		InitialScrollPos.x = -999999;
		if (PanVelocity.x != 0 || PanVelocity.y != 0)
			CompoundPosition -= PanVelocity * dt;

		PanVelocity *= 0.8f;
		if (PanVelocity.x > -0.5f && PanVelocity.x < 0.5f)
			PanVelocity.x = 0;
		if (PanVelocity.y > -0.5f && PanVelocity.y < 0.5f)
			PanVelocity.y = 0;
	}
	if ((touch1 != NULL && touch1->active) && (touch2 != NULL && touch2->active))
	{
		// Handle pinch zoom
		float x1 = p1.x;
		float y1 = p1.y;
		float x2 = p2.x;
		float y2 = p2.y;
		float d = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
		float dist = 1.0f;
		if (d != 0)
			dist = (float)sqrt(d);

		if (dist > 0)
		{
			if (ZoomValid)
			{
				// Update scale and position
				CompoundScale = SavedCompoundScale;
				CompoundPosition = SavedCompoundPosition;

				float scale = dist / PreviousDist;
				CompoundScale = CompoundScale * scale;
				if (CompoundScale > MaxZoom)
				{
					scale = MaxZoom / SavedCompoundScale;
					CompoundScale = MaxZoom;
				}
				else
				if (CompoundScale < MinZoom)
				{
					scale = MinZoom / SavedCompoundScale;
					CompoundScale = MinZoom;
				}
				float tx = Midpoint.x - Midpoint.x * scale;
				float ty = Midpoint.y - Midpoint.y * scale;
				CompoundPosition.x = (scale * CompoundPosition.x) + tx;
				CompoundPosition.y = (scale * CompoundPosition.y) + ty;
			}
			else
			{
				// Latch start scale and position
				SavedCompoundScale = CompoundScale;
				SavedCompoundPosition = CompoundPosition;
				PreviousDist = dist;
				ZoomValid = true;
				Midpoint.x = (((x1 + x2) / 2)) / Visual->getTransform().m[0][0];
				Midpoint.y = (((y1 + y2) / 2)) / Visual->getTransform().m[1][1];
			}
		}
	}
	else
	{
		ZoomValid = false;
	}

	// Set image scale
	Zoom = CompoundScale;
	if (Zoom < MinZoom)
		Zoom = MinZoom;
	if (Zoom > MaxZoom)
		Zoom = MaxZoom;
	TextActor->setScale(Zoom);

//	setZoom(Zoom);

	// Move image to bounds
	int cx = ClippingMargin.x + ClippingMargin.y;
	int cy = ClippingMargin.w + ClippingMargin.h;
	int dx = (int)(Area.x * Zoom - Size.x + cx);
	int dy = (int)(Area.y * Zoom - Size.y + cy);
	if (dx < 0)
		dx = 0;
	if (dy < 0)
		dy = 0;
	int x1 = -dx / 2;
	int y1 = -dy / 2;
	int x2 = dx / 2;
	int y2 = dy / 2;

	if (EnableSpring)
	{
		if ((touch1 == NULL || !touch1->active) && (touch2 == NULL || !touch2->active))
		{
			if (CompoundPosition.x < x1)
				CompoundPosition.x += (x1 - CompoundPosition.x) * 0.5f;
			else
			if (CompoundPosition.x > x2)
				CompoundPosition.x += (x2 - CompoundPosition.x) * 0.5f;
			if (CompoundPosition.y < y1)
				CompoundPosition.y += (y1 - CompoundPosition.y) * 0.5f;
			else
			if (CompoundPosition.y > y2)
				CompoundPosition.y += (y2 - CompoundPosition.y) * 0.5f;
		}
	}
	else
	{
		if (CompoundPosition.x < x1)
			CompoundPosition.x = (float)x1;
		else
		if (CompoundPosition.x > x2)
			CompoundPosition.x = (float)x2;
		if (CompoundPosition.y < y1)
			CompoundPosition.y = (float)y1;
		else
		if (CompoundPosition.y > y2)
			CompoundPosition.y = (float)y2;
	}

	// Set image position
	TextActor->setPosition(CompoundPosition.x, CompoundPosition.y);

	return true;
}

bool CIwGameUITextView::UpdateVisual()
{
	if (!CIwGameActorImage::UpdateVisual())
		return false;

	return true;
}

void CIwGameUITextView::OnBeginTouch(int index, int x, int y, bool allow_bubble)
{
	if (!Enabled)
		return;

	if (!TextActor->getVisual()->HitTest((float)x, (float)y))
	{
		CIwGameActor::OnBeginTouch(index, x, y, allow_bubble);
		return;
	}

	if (NumTouches > 2)
		return;

	if (TouchIndices[0] < 0)
	{
		TouchIndices[0] = index;
		NumTouches++;
	}
	else
	if (TouchIndices[1] < 0)
	{
		TouchIndices[1] = index;
		NumTouches++;
	}

	CIwGameActor::OnBeginTouch(index, x, y, allow_bubble);

	if (SelectType == SelectType_Normal)
	{
		setSelected(true);
	}
}

void CIwGameUITextView::OnEndTouch(int index, int x, int y, bool allow_bubble)
{
	if (!Enabled)
		return;

	if (TouchIndices[0] == index)
	{
		TouchIndices[0] = -1;
		NumTouches--;
	}
	else
	if (TouchIndices[1] == index)
	{
		TouchIndices[1] = -1;
		NumTouches--;
	}

	CIwGameActor::OnEndTouch(index, x, y, allow_bubble);

	if (SelectType == SelectType_Normal)
	{
		setSelected(false);
	}
}

bool CIwGameUITextView::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - TextView needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameUILabel::LoadFromXoml(parent, false, node))
		return false;

	// Process Imageview specific attributes
	CIwGameString	brush_name;
	CIwFVec2		area = CIwFVec2(-100, -100);
	float			scale = 1.0f;

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Zoom_Hash)
			scale = (*it)->GetValueAsFloat();
		else
		if (name_hash == CIwGameXomlNames::MinZoom_Hash)
			MinZoom = (*it)->GetValueAsFloat();
		else
		if (name_hash == CIwGameXomlNames::MaxZoom_Hash)
			MaxZoom = (*it)->GetValueAsFloat();
	}

	InitView(true, MinZoom, MaxZoom);
	setZoom(scale);

	if (load_children)
	{
		// Process inner tags
		if (!IW_GAME_XOML->Process(TextActor, node))
			return false;
	}
	return true;
}

bool CIwGameUITextView::getProperty(unsigned int element_name, CIwGameXomlProperty& prop)
{
	if (CIwGameUIBase::getProperty(element_name, prop))
		return true;

	if (element_name == CIwGameXomlNames::MinZoom_Hash)
	{
		prop.Type = PT_Float;
		prop.p_float = MinZoom;
	}
	else
	if (element_name == CIwGameXomlNames::MaxZoom_Hash)
	{
		prop.Type = PT_Float;
		prop.p_float = MaxZoom;
	}
	else
	if (element_name == CIwGameXomlNames::Zoom_Hash)
	{
		prop.Type = PT_Float;
		prop.p_float = Zoom;
	}
	else
		return false;

	return true;
}

bool CIwGameUITextView::setProperty(unsigned int element_name, CIwGameString& data, bool delta)
{
	if (CIwGameUIBase::setProperty(element_name, data, delta))
		return true;

	if (element_name == CIwGameXomlNames::MinZoom_Hash)
	{
		if (delta)
			setMinZoom(MinZoom + data.GetAsFloat());
		else
			setMinZoom(data.GetAsFloat());
	}
	else
	if (element_name == CIwGameXomlNames::MaxZoom_Hash)
	{
		if (delta)
			setMaxZoom(MaxZoom + data.GetAsFloat());
		else
			setMaxZoom(data.GetAsFloat());
	}
	else
	if (element_name == CIwGameXomlNames::Zoom_Hash)
	{
		if (delta)
			setZoom(Zoom + data.GetAsFloat());
		else
			setZoom(data.GetAsFloat());
	}
	else
		return false;

	return true;
}

bool CIwGameUITextView::UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var)
{
	// Two way binding for Zoom (passes the value back to the variable)
	if (PrevZoom != Zoom)
	{
		if (element_name == CIwGameXomlNames::Zoom_Hash)
		{
			if (var->Type == VT_Float)
				var->setValue(CIwGameString(Zoom).c_str());	// Write Value back to bound variable
			else
			if (var->Type == VT_Int)
				var->setValue(CIwGameString((int)Zoom).c_str());	// Write Value back to bound variable
			return true;
		}
	}

	if (!var->Modified || var->getValue().IsEmpty())
		return false;
	if (CIwGameUILabel::UpdateBinding(element_name, var))
		return true;

	if (element_name == CIwGameXomlNames::Zoom_Hash)
	{
		if (var->Type == VT_Float)
			setZoom(((CIwGameXomlVariableFloat*)var)->NativeValue);
		else
		if (var->Type == VT_Int)
			setZoom((float)((CIwGameXomlVariableInt*)var)->NativeValue);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected float or int for Zoom");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::MinZoom_Hash)
	{
		if (var->Type == VT_Float)
			setMinZoom(((CIwGameXomlVariableFloat*)var)->NativeValue);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected float for MinZoom");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::MaxZoom_Hash)
	{
		if (var->Type == VT_Float)
			setMaxZoom(((CIwGameXomlVariableFloat*)var)->NativeValue);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected float for MaxZoom");
#endif // _DEBUG
	}
	else
		return false;

	return true;
}

