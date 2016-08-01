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

#include "IwGameUI.h"
#include "IwGameUtil.h"
#include "IwGameString.h"
#include "IwGame.h"

CIwList<CIwGameAnim*>	CIwGameUIBase::PresetAnimations;


//
//
//
//
// CIwGameUIBase implementation
//
//
//
//
CIwGameUIBase::CIwGameUIBase() : CIwGameActorImage(), BackgroundBrush(NULL), SelectedBackgroundBrush(NULL), DisabledBackgroundBrush(NULL), Enabled(true), Selected(false), SelectType(SelectType_Normal), 
	LayoutDirty(true), EnableSpring(true), ToggledOn(false), Tapped(false), ShowTimeline(NULL), HideTimeline(NULL), SizeToContent(Axis_None), NeedsResize(false)
{
	setActualClassType("uibase");
	Category = AC_UI;
	BackgroundColour.r = 0xff;
	BackgroundColour.g = 0xff;
	BackgroundColour.b = 0xff;
	BackgroundColour.a = 0xff;
	SelectedBackgroundColour.r = 0xff;
	SelectedBackgroundColour.g = 0xff;
	SelectedBackgroundColour.b = 0xff;
	SelectedBackgroundColour.a = 0xff;
	DisabledBackgroundColour.r = 0x7f;
	DisabledBackgroundColour.g = 0x7f;
	DisabledBackgroundColour.b = 0x7f;
	DisabledBackgroundColour.a = 0x7f;
	ClippingMargin.x = 0;
	ClippingMargin.y = 0;
	ClippingMargin.w = 0;
	ClippingMargin.h = 0;
	OriginalClippingMargin.x = 0;
	OriginalClippingMargin.y = 0;
	OriginalClippingMargin.w = 0;
	OriginalClippingMargin.h = 0;
	PreviousScrollPos.x = 0;
	PreviousScrollPos.y = 0;
	ScrollPos.x = 0;
	ScrollPos.y = 0;
	ScrollVelocity.x = 0;
	ScrollVelocity.y = 0;
	ScrollRange.x = 0;
	ScrollRange.y = 0;
	ScrollRange.w = 0;
	ScrollRange.h = 0;
	InitialScrollPos.x = -999999;
	InitialScrollPos.y = -999999;
	ColourOffset.x = 0;
	ColourOffset.y = 0;
	ColourOffset.w = 0;
	ColourOffset.h = 0;
}

CIwGameUIBase::~CIwGameUIBase()
{
	if (ShowTimeline != NULL)
	{
		if (!ShowTimeline->isManaged())
			delete ShowTimeline;
	}
	if (HideTimeline != NULL)
	{
		if (!HideTimeline->isManaged())
			delete HideTimeline;
	}
}

void CIwGameUIBase::ResizeToContent()
{
	if (SizeToContent)
	{
		// Set docking
		if (Docking != DK_None)
			setDocking(Docking);
		LayoutDirty = true;
	}
}

void CIwGameUIBase::setVisible(bool visible, bool force_set)
{
	if (!force_set)
	{
		if (!IsVisible && visible)
		{
			if (ShowTimeline != NULL)
			{
				if (HideTimeline != NULL)
					HideTimeline->stop();
				ShowTimeline->restart();
				ShowTimeline->Update(0.1f);
			}
		}
		if (IsVisible && !visible)
		{
			if (HideTimeline != NULL)
			{
				if (ShowTimeline != NULL)
					ShowTimeline->stop();
				HideTimeline->restart();
				HideTimeline->Update(0.1f);
				return;
			}
		}
	}

	CIwGameActor::setVisible(visible);
}

void CIwGameUIBase::setShowTimeline(CIwGameAnimTimeline* timeline)
{
	if (ShowTimeline != NULL)
	{
		if (!ShowTimeline->isManaged())
			delete ShowTimeline;
	}
	ShowTimeline = timeline;
}

void CIwGameUIBase::setHideTimeline(CIwGameAnimTimeline* timeline)
{
	if (HideTimeline != NULL)
	{
		if (!HideTimeline->isManaged())
			delete HideTimeline;
	}
	HideTimeline = timeline;
}

void CIwGameUIBase::setBackgroundBrush(IIwGameBrush* brush)
{
	if (BackgroundBrush == brush)
		return;
	if (brush == NULL)
	{
		BackgroundBrush = NULL;
		if (!Selected && Enabled)
			SetImage(NULL);
		return;
	}

	BackgroundBrush = brush;
	if (!Selected && Enabled)
		SetFromBrush(brush);
}

void CIwGameUIBase::setBackgroundColour(CIwColour& colour)
{
	BackgroundColour = colour;
	if (!Selected && Enabled)
		setColour(colour.r, colour.g, colour.b, colour.a); 
}

void CIwGameUIBase::setSelectedBackgroundBrush(IIwGameBrush* brush)
{
	if (SelectedBackgroundBrush == brush)
		return;
	if (brush == NULL)
	{
		SelectedBackgroundBrush = NULL;
		if (Selected && Enabled)
			SetImage(NULL);
		return;
	}

	SelectedBackgroundBrush = brush;
	if (Selected && Enabled)
		SetFromBrush(brush);
}

void CIwGameUIBase::setSelectedBackgroundColour(CIwColour& colour)
{
	SelectedBackgroundColour = colour;
	if (Selected && Enabled)
		setColour(colour.r, colour.g, colour.b, colour.a); 
}

void CIwGameUIBase::setDisabledBackgroundBrush(IIwGameBrush* brush)
{
	if (DisabledBackgroundBrush == brush)
		return;
	if (brush == NULL)
	{
		DisabledBackgroundBrush = NULL;
		if (!Enabled)
			SetImage(NULL);
		return;
	}

	DisabledBackgroundBrush = brush;
	if (!Enabled)
		SetFromBrush(brush);
}

void CIwGameUIBase::setDisabledBackgroundColour(CIwColour& colour)
{
	DisabledBackgroundColour = colour;
	if (!Enabled)
		setColour(colour.r, colour.g, colour.b, colour.a); 
}

void CIwGameUIBase::setEnabled(bool enabled)
{
	if (!enabled)
	{
		if (DisabledBackgroundBrush != NULL)
			SetFromBrush(DisabledBackgroundBrush);
		if (ColourOffset.x != 0 || ColourOffset.y != 0 || ColourOffset.w != 0 || ColourOffset.h != 0)
		{
			CIwRect c;
			c.x = DisabledBackgroundColour.r + ColourOffset.x;
			c.y = DisabledBackgroundColour.g + ColourOffset.y;
			c.w = DisabledBackgroundColour.b + ColourOffset.w;
			c.h = DisabledBackgroundColour.a + ColourOffset.h;
			if (c.x < 0) c.x = 0; else if (c.x > 255) c.x = 255;
			if (c.y < 0) c.y = 0; else if (c.y > 255) c.y = 255;
			if (c.w < 0) c.w = 0; else if (c.w > 255) c.w = 255;
			if (c.h < 0) c.h = 0; else if (c.h > 255) c.h = 255;
			setColour((uint8)c.x, (uint8)c.y, (uint8)c.w, (uint8)c.h);
		}
		else
			setColour(DisabledBackgroundColour);
	}
	else
	{
		if (Selected)
		{
			if (SelectedBackgroundBrush != NULL)
				SetFromBrush(SelectedBackgroundBrush);
			if (ColourOffset.x != 0 || ColourOffset.y != 0 || ColourOffset.w != 0 || ColourOffset.h != 0)
			{
				CIwRect c;
				c.x = SelectedBackgroundColour.r + ColourOffset.x;
				c.y = SelectedBackgroundColour.g + ColourOffset.y;
				c.w = SelectedBackgroundColour.b + ColourOffset.w;
				c.h = SelectedBackgroundColour.a + ColourOffset.h;
				if (c.x < 0) c.x = 0; else if (c.x > 255) c.x = 255;
				if (c.y < 0) c.y = 0; else if (c.y > 255) c.y = 255;
				if (c.w < 0) c.w = 0; else if (c.w > 255) c.w = 255;
				if (c.h < 0) c.h = 0; else if (c.h > 255) c.h = 255;
				setColour((uint8)c.x, (uint8)c.y, (uint8)c.w, (uint8)c.h);
			}
			else
				setColour(SelectedBackgroundColour);
		}
		else
		{
			if (BackgroundBrush != NULL)
				SetFromBrush(BackgroundBrush);
			if (ColourOffset.x != 0 || ColourOffset.y != 0 || ColourOffset.w != 0 || ColourOffset.h != 0)
			{
				CIwRect c;
				c.x = BackgroundColour.r + ColourOffset.x;
				c.y = BackgroundColour.g + ColourOffset.y;
				c.w = BackgroundColour.b + ColourOffset.w;
				c.h = BackgroundColour.a + ColourOffset.h;
				if (c.x < 0) c.x = 0; else if (c.x > 255) c.x = 255;
				if (c.y < 0) c.y = 0; else if (c.y > 255) c.y = 255;
				if (c.w < 0) c.w = 0; else if (c.w > 255) c.w = 255;
				if (c.h < 0) c.h = 0; else if (c.h > 255) c.h = 255;
				setColour((uint8)c.x, (uint8)c.y, (uint8)c.w, (uint8)c.h);
			}
			else
				setColour(BackgroundColour);
		}
	}

	Enabled = enabled;
}

void CIwGameUIBase::setScrollPos(float x, float y)
{
	ScrollPos.x = x; ScrollPos.y = y;
	ScrollRangeCheck();
}

bool CIwGameUIBase::ScrollRangeCheck()
{
	bool changed = false;

	if (ScrollRange.w != 0)
	{
		if (ScrollPos.x < (float)ScrollRange.x)
		{
			if (EnableSpring)
				ScrollPos.x += ((float)ScrollRange.x - ScrollPos.x) * 0.5f;
			else
			{
				ScrollPos.x = (float)ScrollRange.x;
				ScrollVelocity.x = 0;
			}
			changed = true;
		}
		else
		if (ScrollPos.x > (float)ScrollRange.x + ScrollRange.w)
		{
			if (EnableSpring)
				ScrollPos.x += ((float)(ScrollRange.x + ScrollRange.w) - ScrollPos.x) * 0.5f;
			else
			{
				ScrollPos.x = (float)ScrollRange.x + ScrollRange.w;
				ScrollVelocity.x = 0;
			}
			changed = true;
		}
	}
	else
		ScrollPos.x = ScrollRange.x;

	if (ScrollRange.h != 0)
	{
		if (ScrollPos.y < (float)ScrollRange.y)
		{
			if (EnableSpring)
				ScrollPos.y += ((float)ScrollRange.y - ScrollPos.y) * 0.5f;
			else
			{
				ScrollPos.y = (float)ScrollRange.y;
				ScrollVelocity.y = 0;
			}
			changed = true;
		}
		else
		if (ScrollPos.y > (float)ScrollRange.y + ScrollRange.h)
		{
			if (EnableSpring)
				ScrollPos.y += ((float)(ScrollRange.y + ScrollRange.h) - ScrollPos.y) * 0.5f;
			else
			{
				ScrollPos.y = (float)ScrollRange.y + ScrollRange.h;
				ScrollVelocity.y = 0;
			}
			changed = true;
		}
	}
	else
		ScrollPos.y = ScrollRange.y;

	return changed;
}

bool CIwGameUIBase::Update(float dt)
{
	if (!IsVisible)
		return CIwGameActor::Update(dt);

	if (Visual != NULL)
	{
		if (HideTimeline != NULL)
		{
			bool is_playing = HideTimeline->isPlaying();
			if (is_playing)
				HideTimeline->Update(dt);
			if (is_playing && !HideTimeline->isPlaying())
				CIwGameActor::setVisible(false);
		}
		if (ShowTimeline != NULL)
			ShowTimeline->Update(dt);
	}

	// Handle touch scrolling
	PreviousScrollPos = ScrollPos;
	if (ScrollRange.w != 0 || ScrollRange.h != 0)
	{
		if (!IsDraggable)
		{
			if (IsDragging)
			{
				if (InitialScrollPos.x < -10000)
					InitialScrollPos = ScrollPos;
				CIwVec2 drag_delta  = IW_GAME_INPUT->getDragDelta(TouchIndex);
				if (drag_delta.x != 0 || drag_delta.y != 0)
				{
					CIwFMat2D& transform = Visual->getFinalTransform();
					CIwFVec2 drag = CIwFVec2(((float)drag_delta.x) / transform.m[0][0], ((float)drag_delta.y) / transform.m[1][1]);
					if (ScrollRange.w != 0)
					{
						if (drag.x >= -1 && drag.x <= 1)
							drag.x = 0;
						ScrollVelocity.x = drag.x;
						if (drag.x != 0)
						{
							ScrollPos.x -= (float)drag.x;
							ScrollVelocity.x = (float)drag.x / dt;
							LayoutDirty = true;
						}
					}
					if (ScrollRange.h != 0)
					{
						if (drag.y >= -1 && drag.y <= 1)
							drag.y = 0;
						ScrollVelocity.y = drag.y;
						if (drag.y != 0)
						{
							ScrollPos.y -= (float)drag.y;
							ScrollVelocity.y = (float)drag.y / dt;
							LayoutDirty = true;
						}
					}

					// If we move scroll too much then release cous
					CIwFVec2 pos_diff = InitialScrollPos - ScrollPos;
					if (pos_diff.GetLengthSquared() > 32)
					{
						CIwGameActor* focus = Scene->getParent()->getTouchFocus(TouchIndex);
						if (focus != this && focus != NULL && !focus->getHoldFocus())
						{
							Scene->getParent()->releaseTouchFocus(TouchIndex, 999999, 999999, false);	// Use stupidly large values so they are invalid
							Scene->getParent()->setTouchFocus(TouchIndex, this);
						}
					}
				}
			}
			else
				InitialScrollPos.x = -999999;
		}

		if (!IsDragging && (ScrollVelocity.x != 0 || ScrollVelocity.y != 0))
		{
			ScrollPos -= ScrollVelocity * dt;
			LayoutDirty = true;
		}

		if (EnableSpring)
		{
			if (!IsDragging)
			{
				if (ScrollRangeCheck())
					LayoutDirty = true;
			}
		}
		else
		{
			if (ScrollRangeCheck())
				LayoutDirty = true;
		}

		ScrollVelocity *= 0.9f;
		if (ScrollVelocity.x > -0.5f && ScrollVelocity.x < 0.5f)
			ScrollVelocity.x = 0;
		if (ScrollVelocity.y > -0.5f && ScrollVelocity.y < 0.5f)
			ScrollVelocity.y = 0;
	}

	if (LayoutDirty)
		UpdateLayout();

	if (!CIwGameActor::Update(dt))
		return false;

	return true;
}

bool CIwGameUIBase::UpdateLayout()
{
	LayoutDirty = false;

	if (SizeToContent != Axis_None && LinkedTo != NULL)
	{
		((CIwGameUIBase*)LinkedTo)->setLayoutDirty(true);
	}

//	ResizeToContent();

	return true;
}

void CIwGameUIBase::setShowTimeline(const char* timeline_name)
{
	CIwGameAnimTimeline* timeline = NULL;
	unsigned int timeline_hash = IW_GAME_HASH(timeline_name);

	// Check preeet animations first
	CIwGameAnim* anim = FindPresetAnimation(timeline_hash);
	if (anim != NULL)
	{
		if (anim->getCategory() == 0)
			timeline = CIwGameAnimUtil::CreateTimeline(timeline_name, anim, this, "PositionX", 1, true);
		else
		if (anim->getCategory() == 1)
			timeline = CIwGameAnimUtil::CreateTimeline(timeline_name, anim, this, "PositionY", 1, true);
		else
		if (anim->getCategory() == 2)
			timeline = CIwGameAnimUtil::CreateTimeline(timeline_name, anim, this, "Opacity", 1, false);
	}

	// Now check scene and global reources
	if (timeline == NULL)
		timeline = CIwGameAnimTimelinesManager::FindTimeline(timeline_hash, this);

	if (timeline == NULL)
		CIwGameError::LogError("Warning: XOML - Could not find named show timeline in the presets, scene or in the global resources - ", timeline_name);
	else
	{
		setShowTimeline(timeline);
		timeline->setTargetElement(this);
	}
}

void CIwGameUIBase::setHideTimeline(const char* timeline_name)
{
	CIwGameAnimTimeline* timeline = NULL;
	unsigned int timeline_hash = IW_GAME_HASH(timeline_name);

	// Check preeet animations first
	CIwGameAnim* anim = FindPresetAnimation(timeline_hash);
	if (anim != NULL)
	{
		if (anim->getCategory() == 0)
			timeline = CIwGameAnimUtil::CreateTimeline(timeline_name, anim, this, "PositionX", 1, true);
		else
		if (anim->getCategory() == 1)
			timeline = CIwGameAnimUtil::CreateTimeline(timeline_name, anim, this, "PositionY", 1, true);
		else
		if (anim->getCategory() == 2)
			timeline = CIwGameAnimUtil::CreateTimeline(timeline_name, anim, this, "Opacity", 1, false);
	}

	// Now check scene and global reources
	if (timeline == NULL)
		timeline = CIwGameAnimTimelinesManager::FindTimeline(timeline_hash, this);

	if (timeline == NULL)
		CIwGameError::LogError("Warning: XOML - Could not find named hide timeline in the presets, scene or in the global resources - ", timeline_name);
	else
	{
		setHideTimeline(timeline);
		timeline->setTargetElement(this);
	}
}

bool CIwGameUIBase::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - uibase needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameActorImage::LoadFromXoml(parent, false, node))
		return false;

	// Process attributes
	CIwGameString*	brush_name = NULL;
	CIwGameString*	selected_brush_name = NULL;
	CIwGameString*	disabled_brush_name = NULL;
	CIwGameString*	show_timeline = NULL;
	CIwGameString*	hide_timeline = NULL;
	bool			enabled = true;
	bool			selected = false;
	bool			key_focus = false;

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Brush_Hash || name_hash == CIwGameXomlNames::Background_Hash)
			brush_name = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::SelectedBackground_Hash)
			selected_brush_name = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::DisabledBackground_Hash)
			disabled_brush_name = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::BackgroundColour_Hash)
		{
			if (!(*it)->GetValueAsColour(BackgroundColour))
				CIwGameError::LogError("Warning: XOML - Invalid value for UIBase::BackgroundColour expected colour");
		}
		else
		if (name_hash == CIwGameXomlNames::SelectedColour_Hash)
		{
			if (!(*it)->GetValueAsColour(SelectedBackgroundColour))
				CIwGameError::LogError("Warning: XOML - Invalid value for UIBase::SelectedColour expected colour");
		}
		else
		if (name_hash == CIwGameXomlNames::DisabledColour_Hash)
		{
			if (!(*it)->GetValueAsColour(DisabledBackgroundColour))
				CIwGameError::LogError("Warning: XOML - Invalid value for UIBase::DisabledColour expected colour");
		}
		else
		if (name_hash == CIwGameXomlNames::Enabled_Hash)
			enabled = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::SelectType_Hash)
		{
			unsigned int type_hash = (*it)->GetValue().getHash();

			if (type_hash == CIwGameXomlNames::Toggle_Hash)
				SelectType = SelectType_Toggle;
			else
			if (type_hash == CIwGameXomlNames::Sticky_Hash)
				SelectType = SelectType_Sticky;
			else
				SelectType = SelectType_Normal;
		}
		else
		if (name_hash == CIwGameXomlNames::Selected_Hash)
			selected = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::Spring_Hash)
			EnableSpring = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::OnToggledOn_Hash)
			EventsManager->addEvent("OnToggledOn", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnToggledOff_Hash)
			EventsManager->addEvent("OnToggledOff", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnBackKey_Hash)
			EventsManager->addEvent("OnBackKey", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnMenuKey_Hash)
			EventsManager->addEvent("OnMenuKey", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::ClipMargin_Hash)
		{
			if (!(*it)->GetValueAsRect(ClippingMargin))
				CIwGameError::LogError("Warning: XOML - Invalid value for UIBase::ClipMargin expected rect");
		}
		else
		if (name_hash == CIwGameXomlNames::ScrollRange_Hash)
		{
			if (!(*it)->GetValueAsRect(ScrollRange))
				CIwGameError::LogError("Warning: XOML - Invalid value for UIBase::ScrollRange expected rect");
		}
		else
		if (name_hash == CIwGameXomlNames::ScrollPos_Hash)
		{
			if (!(*it)->GetValueAsPoint(ScrollPos))
				CIwGameError::LogError("Warning: XOML - Invalid value for UIBase::ScrollPos expected vec2");
		}
		else
		if (name_hash == CIwGameXomlNames::ShowTimeline_Hash)
			show_timeline = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::HideTimeline_Hash)
			hide_timeline = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::SizeToContent_Hash)
		{
			unsigned int size_type = (*it)->GetValue().getHash();
			if (size_type == IW_GAME_HASH("x"))
				SizeToContent = Axis_X;
			else
			if (size_type == IW_GAME_HASH("y"))
				SizeToContent = Axis_Y;
			else
			if (size_type == IW_GAME_HASH("XY"))
				SizeToContent = Axis_XY;
		}
		else
		if (name_hash == CIwGameXomlNames::KeyFocus_Hash)
			key_focus = (*it)->GetValueAsBool();
	}

	CIwGameBrushImage* brush = NULL;
	if (brush_name != NULL)
	{
		brush = (CIwGameBrushImage*)Scene->getResourceManager()->findResource(brush_name->getHash(), CIwGameXomlNames::Brush_Hash);
		if (brush != NULL)
			BackgroundBrush = brush;
		else
			CIwGameError::LogError("Warning: UIBase - Could not find brush in the scene or in the global resources - ", brush_name->c_str());
	}
	if (selected_brush_name != NULL)
	{
		brush = (CIwGameBrushImage*)Scene->getResourceManager()->findResource(selected_brush_name->getHash(), CIwGameXomlNames::Brush_Hash);
		if (brush != NULL)
			SelectedBackgroundBrush = brush;
		else
			CIwGameError::LogError("Warning: UIBase - Could not find selected brush in the scene or in the global resources - ", selected_brush_name->c_str());
	}
	if (disabled_brush_name != NULL)
	{
		brush = (CIwGameBrushImage*)Scene->getResourceManager()->findResource(disabled_brush_name->getHash(), CIwGameXomlNames::Brush_Hash);
		if (brush != NULL)
			DisabledBackgroundBrush = brush;
		else
			CIwGameError::LogError("Warning: UIBase - Could not find disabled brush in the scene or in the global resources - ", disabled_brush_name->c_str());
	}

	// Search the scenes resources for the specified timelines
	if (show_timeline != NULL)
		setShowTimeline(show_timeline->c_str());
	if (hide_timeline != NULL)
		setHideTimeline(hide_timeline->c_str());

	// Calculate clipping 
	OriginalClippingMargin = ClippingMargin;
	if (ClippingMargin.x < 0)
		ClippingMargin.x = (Size.x * -ClippingMargin.x) / 100;
	if (ClippingMargin.y < 0)
		ClippingMargin.y = (Size.x * -ClippingMargin.y) / 100;
	if (ClippingMargin.w < 0)
		ClippingMargin.w = (Size.y * -ClippingMargin.w) / 100;
	if (ClippingMargin.h < 0)
		ClippingMargin.h = (Size.y * -ClippingMargin.h) / 100;

	setBackgroundColour(BackgroundColour);

	if (SelectType != SelectType_Normal)
		Selected = selected;

	setEnabled(enabled);

	// If set as key focus then notify the game
	if (key_focus)
		Scene->getParent()->setKeyFocus(this);

	if (load_children)
	{
		// Process inner tags
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}

	return true;
}

bool CIwGameUIBase::getProperty(unsigned int element_name, CIwGameXomlProperty& prop)
{
	if (CIwGameActorImage::getProperty(element_name, prop))
		return true;

	if (element_name == CIwGameXomlNames::Background_Hash)
	{
		prop.Type = PT_Resource;
		prop.p_data = BackgroundBrush;
	}
	else
	if (element_name == CIwGameXomlNames::SelectedBackground_Hash)
	{
		prop.Type = PT_Resource;
		prop.p_data = SelectedBackgroundBrush;
	}
	else
	if (element_name == CIwGameXomlNames::DisabledBackground_Hash)
	{
		prop.Type = PT_Resource;
		prop.p_data = DisabledBackgroundBrush;
	}
	else
	if (element_name == CIwGameXomlNames::BackgroundColour_Hash)
	{
		prop.Type = PT_Vec4;
		prop.p_vec[0] = (float)BackgroundColour.r;
		prop.p_vec[1] = (float)BackgroundColour.g;
		prop.p_vec[2] = (float)BackgroundColour.b;
		prop.p_vec[3] = (float)BackgroundColour.a;
	}
	else
	if (element_name == CIwGameXomlNames::SelectedColour_Hash)
	{
		prop.Type = PT_Vec4;
		prop.p_vec[0] = (float)SelectedBackgroundColour.r;
		prop.p_vec[1] = (float)SelectedBackgroundColour.g;
		prop.p_vec[2] = (float)SelectedBackgroundColour.b;
		prop.p_vec[3] = (float)SelectedBackgroundColour.a;
	}
	else
	if (element_name == CIwGameXomlNames::DisabledColour_Hash)
	{
		prop.Type = PT_Vec4;
		prop.p_vec[0] = (float)DisabledBackgroundColour.r;
		prop.p_vec[1] = (float)DisabledBackgroundColour.g;
		prop.p_vec[2] = (float)DisabledBackgroundColour.b;
		prop.p_vec[3] = (float)DisabledBackgroundColour.a;
	}
	else
	if (element_name == CIwGameXomlNames::Enabled_Hash)
	{
		prop.Type = PT_Bool;
		prop.p_bool = Enabled;
	}
	else
	if (element_name == CIwGameXomlNames::Selected_Hash)
	{
		prop.Type = PT_Bool;
		prop.p_bool = Selected;
	}
	else
	if (element_name == CIwGameXomlNames::ScrollPos_Hash)
	{
		prop.Type = PT_Vec2;
		prop.p_vec[0] = ScrollPos.x;
		prop.p_vec[1] = ScrollPos.y;
	}
	else
	if (element_name == CIwGameXomlNames::SelectType_Hash)
	{
		prop.Type = PT_Int;
		prop.p_int = SelectType;
	}
	else
	if (element_name == CIwGameXomlNames::Spring_Hash)
	{
		prop.Type = PT_Bool;
		prop.p_bool = EnableSpring;
	}
	else
	if (element_name == CIwGameXomlNames::ClipMargin_Hash)
	{
		prop.Type = PT_Vec4;
		prop.p_vec[0] = (float)ClippingMargin.x;
		prop.p_vec[1] = (float)ClippingMargin.y;
		prop.p_vec[2] = (float)ClippingMargin.w;
		prop.p_vec[3] = (float)ClippingMargin.h;
	}
	else
	if (element_name == CIwGameXomlNames::ScrollRange_Hash)
	{
		prop.Type = PT_Vec4;
		prop.p_vec[0] = (float)ScrollRange.x;
		prop.p_vec[1] = (float)ScrollRange.y;
		prop.p_vec[2] = (float)ScrollRange.w;
		prop.p_vec[3] = (float)ScrollRange.h;
	}
	else
	if (element_name == CIwGameXomlNames::SizeToContent_Hash)
	{
		prop.Type = PT_Int;
		prop.p_int = SizeToContent;
	}
	else
	if (element_name == CIwGameXomlNames::KeyFocus_Hash)
	{
		prop.Type = PT_Bool;
		prop.p_bool = Scene->getParent()->getKeyFocus() == this;
	}
	else
		return false;

	return true;
}

bool CIwGameUIBase::setProperty(unsigned int element_name, CIwGameString& data, bool delta)
{
	if (element_name == CIwGameXomlNames::Size_Hash)
		LayoutDirty = true;

	if (CIwGameActorImage::setProperty(element_name, data, delta))
		return true;

	float float_pool[8];

	if (element_name == CIwGameXomlNames::Background_Hash)
	{
		IIwGameBrush* brush = (IIwGameBrush*)Scene->getResourceManager()->findResource(data.c_str(), CIwGameXomlNames::Brush_Hash);
		setBackgroundBrush(brush);
		if (brush == NULL && !data.IsEmpty())
		{
#if defined(_DEBUG)
			CIwGameError::LogError("Warning: UIBase - Background could not be found in the scene or global resources - ", data.c_str());
#endif // _DEBUG
		}
	}
	else
	if (element_name == CIwGameXomlNames::SelectedBackground_Hash)
	{
		IIwGameBrush* brush = (IIwGameBrush*)Scene->getResourceManager()->findResource(data.c_str(), CIwGameXomlNames::Brush_Hash);
		setSelectedBackgroundBrush(brush);
		if (brush == NULL && !data.IsEmpty())
		{
#if defined(_DEBUG)
			CIwGameError::LogError("Warning: UIBase - SelectedBackground could not be found in the scene or global resources - ", data.c_str());
#endif // _DEBUG
		}
	}
	else
	if (element_name == CIwGameXomlNames::DisabledBackground_Hash)
	{
		IIwGameBrush* brush = (IIwGameBrush*)Scene->getResourceManager()->findResource(data.c_str(), CIwGameXomlNames::Brush_Hash);
		setDisabledBackgroundBrush(brush);
		if (brush == NULL && !data.IsEmpty())
		{
#if defined(_DEBUG)
			CIwGameError::LogError("Warning: UIBase - DisabledBackground could not be found in the scene or global resources - ", data.c_str());
#endif // _DEBUG
		}
	}
	else
	if (element_name == CIwGameXomlNames::BackgroundColour_Hash)
	{
		CIwColour c;
		data.GetAsListOfFloat(float_pool);
		if (delta)
		{
			c = BackgroundColour;
			c.r += (uint8)float_pool[0];
			c.g += (uint8)float_pool[1];
			c.b += (uint8)float_pool[2];
			c.a += (uint8)float_pool[3];
		}
		else
		{
			c.r = (uint8)float_pool[0];
			c.g = (uint8)float_pool[1];
			c.b = (uint8)float_pool[2];
			c.a = (uint8)float_pool[3];
		}
		setBackgroundColour(c);
	}
	else
	if (element_name == CIwGameXomlNames::SelectedColour_Hash)
	{
		CIwColour c;
		data.GetAsListOfFloat(float_pool);
		if (delta)
		{
			c = SelectedBackgroundColour;
			c.r += (uint8)float_pool[0];
			c.g += (uint8)float_pool[1];
			c.b += (uint8)float_pool[2];
			c.a += (uint8)float_pool[3];
		}
		else
		{
			c.r = (uint8)float_pool[0];
			c.g = (uint8)float_pool[1];
			c.b = (uint8)float_pool[2];
			c.a = (uint8)float_pool[3];
		}
		setSelectedBackgroundColour(c);
	}
	else
	if (element_name == CIwGameXomlNames::DisabledColour_Hash)
	{
		CIwColour c;
		data.GetAsListOfFloat(float_pool);
		if (delta)
		{
			c = DisabledBackgroundColour;
			c.r += (uint8)float_pool[0];
			c.g += (uint8)float_pool[1];
			c.b += (uint8)float_pool[2];
			c.a += (uint8)float_pool[3];
		}
		else
		{
			c.r = (uint8)float_pool[0];
			c.g = (uint8)float_pool[1];
			c.b = (uint8)float_pool[2];
			c.a = (uint8)float_pool[3];
		}
		setDisabledBackgroundColour(c);
	}
	else
	if (element_name == CIwGameXomlNames::Enabled_Hash)
	{
		if (delta)
			setEnabled(!Enabled);
		else
			setEnabled(data.GetAsBool());
	}
	else
	if (element_name == CIwGameXomlNames::Selected_Hash)
	{
		if (delta)
			setSelected(!Selected);
		else
			setSelected(data.GetAsBool());
	}
	else
	if (element_name == CIwGameXomlNames::ScrollPos_Hash)
	{
		data.GetAsListOfFloat(float_pool);
		if (delta)
			setScrollPos(ScrollPos.x + float_pool[0], ScrollPos.y + float_pool[1]);
		else
			setScrollPos(float_pool[0], float_pool[1]);
	}
	else
	if (element_name == CIwGameXomlNames::SelectType_Hash)
	{
		unsigned int type_hash = data.getHash();

		if (type_hash == CIwGameXomlNames::Toggle_Hash)
			SelectType = SelectType_Toggle;
		else
		if (type_hash == CIwGameXomlNames::Sticky_Hash)
			SelectType = SelectType_Sticky;
		else
			SelectType = SelectType_Normal;
	}
	else
	if (element_name == CIwGameXomlNames::Spring_Hash)
	{
		if (delta)
			EnableSpring = !EnableSpring;
		else
			EnableSpring = data.GetAsBool();
	}
	else
	if (element_name == CIwGameXomlNames::OnToggledOn_Hash)
		EventsManager->addEvent("OnToggledOn", data.c_str(), true);
	else
	if (element_name == CIwGameXomlNames::OnToggledOff_Hash)
		EventsManager->addEvent("OnToggledOff", data.c_str(), true);
	else
	if (element_name == CIwGameXomlNames::OnBackKey_Hash)
		EventsManager->addEvent("OnBackKey", data.c_str(), true);
	else
	if (element_name == CIwGameXomlNames::OnMenuKey_Hash)
		EventsManager->addEvent("OnMenuKey", data.c_str(), true);
	else
	if (element_name == CIwGameXomlNames::ClipMargin_Hash)
	{
		CIwRect rc;
		data.GetAsListOfFloat(float_pool);
		if (delta)
		{
			ClippingMargin.x += (int)float_pool[0];
			ClippingMargin.y += (int)float_pool[1];
			ClippingMargin.w += (int)float_pool[2];
			ClippingMargin.h += (int)float_pool[3];
		}
		else
		{
			ClippingMargin.x = (int)float_pool[0];
			ClippingMargin.y = (int)float_pool[1];
			ClippingMargin.w = (int)float_pool[2];
			ClippingMargin.h = (int)float_pool[3];
		}
	}
	else
	if (element_name == CIwGameXomlNames::ScrollRange_Hash)
	{
		CIwRect rc;
		data.GetAsListOfFloat(float_pool);
		if (delta)
		{
			ScrollRange.x += (int)float_pool[0];
			ScrollRange.y += (int)float_pool[1];
			ScrollRange.w += (int)float_pool[2];
			ScrollRange.h += (int)float_pool[3];
		}
		else
		{
			ScrollRange.x = (int)float_pool[0];
			ScrollRange.y = (int)float_pool[1];
			ScrollRange.w = (int)float_pool[2];
			ScrollRange.h = (int)float_pool[3];
		}
	}
	else
	if (element_name == CIwGameXomlNames::ShowTimeline_Hash)
		setShowTimeline(data.c_str());
	else
	if (element_name == CIwGameXomlNames::HideTimeline_Hash)
		setHideTimeline(data.c_str());
	else
	if (element_name == CIwGameXomlNames::SizeToContent_Hash)
	{
		unsigned int size_type = data.getHash();
		if (size_type == IW_GAME_HASH("x"))
			SizeToContent = Axis_X;
		else
		if (size_type == IW_GAME_HASH("y"))
			SizeToContent = Axis_Y;
		else
		if (size_type == IW_GAME_HASH("XY"))
			SizeToContent = Axis_XY;
	}
	else
	if (element_name == CIwGameXomlNames::KeyFocus_Hash)
		Scene->getParent()->setKeyFocus(this);
	else
		return false;

	return true;
}

bool CIwGameUIBase::UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var)
{
	if (!var->Modified || var->getValue().IsEmpty())
		return false;

	if (element_name == CIwGameXomlNames::Size_Hash)
		LayoutDirty = true;

	if (CIwGameActorImage::UpdateBinding(element_name, var))
		return true;

	if (element_name == CIwGameXomlNames::Background_Hash)
	{
		if (var->Type == VT_String)
		{
			IIwGameBrush* brush = (IIwGameBrush*)Scene->getResourceManager()->findResource(var->getValue().c_str(), CIwGameXomlNames::Brush_Hash);
			setBackgroundBrush(brush);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: UIBase - Incorrect binding variable type, expected string for Background");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::SelectedBackground_Hash)
	{
		if (var->Type == VT_String)
		{
			IIwGameBrush* brush = (IIwGameBrush*)Scene->getResourceManager()->findResource(var->getValue().c_str(), CIwGameXomlNames::Brush_Hash);
			setSelectedBackgroundBrush(brush);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: UIBase - Incorrect binding variable type, expected string for SelectedBackground");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::DisabledBackground_Hash)
	{
		if (var->Type == VT_String)
		{
			IIwGameBrush* brush = (IIwGameBrush*)Scene->getResourceManager()->findResource(var->getValue().c_str(), CIwGameXomlNames::Brush_Hash);
			setDisabledBackgroundBrush(brush);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: UIBase - Incorrect binding variable type, expected string for DisabledBackground");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::BackgroundColour_Hash)
	{
		if (var->Type == VT_Vec4)
		{
			CIwFVec4* data = &((CIwGameXomlVariableVec4*)var)->NativeValue;
			CIwColour c;
			c.r = (uint8)data->x;
			c.g = (uint8)data->y;
			c.b = (uint8)data->z;
			c.a = (uint8)data->w;
			setBackgroundColour(c);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: UIBase - Incorrect binding variable type, expected vec4 for BackgroundColour");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::SelectedColour_Hash)
	{
		if (var->Type == VT_Vec4)
		{
			CIwFVec4* data = &((CIwGameXomlVariableVec4*)var)->NativeValue;
			CIwColour c;
			c.r = (uint8)data->x;
			c.g = (uint8)data->y;
			c.b = (uint8)data->z;
			c.a = (uint8)data->w;
			setSelectedBackgroundColour(c);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: UIBase - Incorrect binding variable type, expected vec4 for SelectedColour");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::DisabledColour_Hash)
	{
		if (var->Type == VT_Vec4)
		{
			CIwFVec4* data = &((CIwGameXomlVariableVec4*)var)->NativeValue;
			CIwColour c;
			c.r = (uint8)data->x;
			c.g = (uint8)data->y;
			c.b = (uint8)data->z;
			c.a = (uint8)data->w;
			setDisabledBackgroundColour(c);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: UIBase - Incorrect binding variable type, expected vec4 for DisabledColour");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::Enabled_Hash)
	{
		if (var->Type == VT_Bool)
			setEnabled(((CIwGameXomlVariableBool*)var)->NativeValue);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: UIBase - Incorrect binding variable type, expected bool for Enabled");
#endif // _DEBUG
	}
	if (element_name == CIwGameXomlNames::ScrollPos_Hash)
	{
		if (var->Type == VT_Vec2)
		{
			CIwFVec2* data = &((CIwGameXomlVariableVec2*)var)->NativeValue;
			setScrollPos(data->x, data->y);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: UIBase - Incorrect binding variable type, expected vec2 for ScrollPos");
#endif // _DEBUG
	}
	else
		return false;

	return true;
}

bool CIwGameUIBase::UpdateFromAnimation(CIwGameAnimInstance *animation)
{
	// We need to chheck for size change here and recalculatue layout if it occurs
	unsigned int element_name = animation->getTargetPropertyHash();
	if (element_name == CIwGameXomlNames::Size_Hash)
		LayoutDirty = true;

	if (CIwGameActorImage::UpdateFromAnimation(animation))
		return true;

	bool delta = animation->isDelta();

	if (element_name == CIwGameXomlNames::BackgroundColour_Hash)
	{
		CIwGameAnimFrameVec4* frame = (CIwGameAnimFrameVec4*)animation->getCurrentData();
		if (delta)
		{
			CIwColour c = BackgroundColour;
			c.r += (uint8)frame->data.x;
			c.g += (uint8)frame->data.y;
			c.b += (uint8)frame->data.z;
			c.a += (uint8)frame->data.w;
			setBackgroundColour(c);
		}
		else
		{
			CIwColour c = BackgroundColour;
			c.r = (uint8)frame->data.x;
			c.g = (uint8)frame->data.y;
			c.b = (uint8)frame->data.z;
			c.a = (uint8)frame->data.w;
			setBackgroundColour(c);
		}
	}
	else
	if (element_name == CIwGameXomlNames::SelectedColour_Hash)
	{
		CIwGameAnimFrameVec4* frame = (CIwGameAnimFrameVec4*)animation->getCurrentData();
		if (delta)
		{
			CIwColour c = SelectedBackgroundColour;
			c.r += (uint8)frame->data.x;
			c.g += (uint8)frame->data.y;
			c.b += (uint8)frame->data.z;
			c.a += (uint8)frame->data.w;
			setSelectedBackgroundColour(c);
		}
		else
		{
			CIwColour c = SelectedBackgroundColour;
			c.r = (uint8)frame->data.x;
			c.g = (uint8)frame->data.y;
			c.b = (uint8)frame->data.z;
			c.a = (uint8)frame->data.w;
			setSelectedBackgroundColour(c);
		}
	}
	else
	if (element_name == CIwGameXomlNames::DisabledColour_Hash)
	{
		CIwGameAnimFrameVec4* frame = (CIwGameAnimFrameVec4*)animation->getCurrentData();
		if (delta)
		{
			CIwColour c = DisabledBackgroundColour;
			c.r += (uint8)frame->data.x;
			c.g += (uint8)frame->data.y;
			c.b += (uint8)frame->data.z;
			c.a += (uint8)frame->data.w;
			setDisabledBackgroundColour(c);
		}
		else
		{
			CIwColour c = DisabledBackgroundColour;
			c.r = (uint8)frame->data.x;
			c.g = (uint8)frame->data.y;
			c.b = (uint8)frame->data.z;
			c.a = (uint8)frame->data.w;
			setDisabledBackgroundColour(c);
		}
	}
	else
	if (element_name == CIwGameXomlNames::ScrollPos_Hash)
	{
		CIwGameAnimFrameVec2* frame = (CIwGameAnimFrameVec2*)animation->getCurrentData();
		if (delta)
			setScrollPos(ScrollPos.x + frame->data.x, ScrollPos.y + frame->data.y);
		else
			setScrollPos(frame->data.x, frame->data.y);
	}
	else
		return false;

	return true;
}

void CIwGameUIBase::OnBeginTouch(int index, int x, int y, bool allow_bubble)
{
	if (TouchIndex >= 0)
		return;

	if (!Enabled)
		return;

	CIwGameActor::OnBeginTouch(index, x, y, allow_bubble);

	if (SelectType != SelectType_Toggle)
	{
		setSelected(true);
	}
}

void CIwGameUIBase::OnEndTouch(int index, int x, int y, bool allow_bubble)
{
	if (!Enabled)
		return;

	CIwGameActor::OnEndTouch(index, x, y, allow_bubble);

	if (SelectType == SelectType_Normal)
		setSelected(false);
	else
	if (!Tapped && !ToggledOn)
		setSelected(false);

	Tapped = false;
}

void CIwGameUIBase::OnTapped(int index, int x, int y, bool allow_bubble)
{
	if (!Enabled)
		return;

	CIwGameActor::OnTapped(index, x, y, allow_bubble);

	if (SelectType == SelectType_Sticky)
	{
		setSelected(Selected);
		if (!ToggledOn)
			setToggledOn(true);
	}
	else
	if (SelectType == SelectType_Toggle)
	{
		setSelected(!Selected);
		setToggledOn(!ToggledOn);
	}

	Tapped = true;
}

void CIwGameUIBase::setSelected(bool selected, bool force_select)
{
	PrevSelected = Selected;

	Selected = selected;
	setEnabled(Enabled);
}

void CIwGameUIBase::setToggledOn(bool toggled_on)
{
	if (SelectType == SelectType_Normal)
		return;

	if (!ToggledOn && toggled_on)
		OnToggledOn();
	else
	if (ToggledOn && !toggled_on)
		OnToggledOff();

	ToggledOn = toggled_on;
}

void CIwGameUIBase::OnOrientationChange(eIwGameScene_Orientation old_orientation, eIwGameScene_Orientation new_orientation)
{
	CIwGameActorImage::OnOrientationChange(old_orientation, new_orientation);

	// Calculate clipping 
	if (ClippingMargin.x < 0)
		ClippingMargin.x = (Size.x * -OriginalClippingMargin.x) / 100;
	if (ClippingMargin.y < 0)
		ClippingMargin.y = (Size.x * -OriginalClippingMargin.y) / 100;
	if (ClippingMargin.w < 0)
		ClippingMargin.w = (Size.y * -OriginalClippingMargin.w) / 100;
	if (ClippingMargin.h < 0)
		ClippingMargin.h = (Size.y * -OriginalClippingMargin.h) / 100;

	LayoutDirty = true;
}

void CIwGameUIBase::OnToggledOn()
{
	ProcessEventActions(CIwGameXomlNames::OnToggledOn_Hash);
}

void CIwGameUIBase::OnToggledOff()
{
	ProcessEventActions(CIwGameXomlNames::OnToggledOff_Hash);
}

void CIwGameUIBase::OnBackKey()
{
	ProcessEventActions(CIwGameXomlNames::OnBackKey_Hash);
}

void CIwGameUIBase::OnMenuKey()
{
	ProcessEventActions(CIwGameXomlNames::OnMenuKey_Hash);
}

void CIwGameUIBase::InitPresetAnimations()
{
	float width = (float)IwGxGetScreenWidth();
	float height = (float)IwGxGetScreenHeight();

	//
	// Create generic common animations
	// 
	float v1 = -width;
	float v2 = 0;
	PresetAnimations.push_back(CIwGameAnimUtil::CreateFloatAnim("PA_ScrollOnFromLeft", v1, v2, 0.5f, CIwGameAnimFrame::AT_QuadIn, 0));
	v1 = 0;
	v2 = -width;
	PresetAnimations.push_back(CIwGameAnimUtil::CreateFloatAnim("PA_ScrollOffToLeft", v1, v2, 0.5f, CIwGameAnimFrame::AT_QuadIn, 0));
	v1 = width;
	v2 = 0;
	PresetAnimations.push_back(CIwGameAnimUtil::CreateFloatAnim("PA_ScrollOnFromRight", v1, v2, 0.5f, CIwGameAnimFrame::AT_QuadIn, 0));
	v1 = 0;
	v2 = width;
	PresetAnimations.push_back(CIwGameAnimUtil::CreateFloatAnim("PA_ScrollOffToRight", v1, v2, 0.5f, CIwGameAnimFrame::AT_QuadIn, 0));
	v1 = -height;
	v2 = 0;
	PresetAnimations.push_back(CIwGameAnimUtil::CreateFloatAnim("PA_ScrollOnFromTop", v1, v2, 0.5f, CIwGameAnimFrame::AT_QuadIn, 1));
	v1 = 0;
	v2 = -height;
	PresetAnimations.push_back(CIwGameAnimUtil::CreateFloatAnim("PA_ScrollOffToTop", v1, v2, 0.5f, CIwGameAnimFrame::AT_QuadIn, 1));
	v1 = height;
	v2 = 0;
	PresetAnimations.push_back(CIwGameAnimUtil::CreateFloatAnim("PA_ScrollOnFromBottom", v1, v2, 0.5f, CIwGameAnimFrame::AT_QuadIn, 1));
	v1 = 0;
	v2 = height;
	PresetAnimations.push_back(CIwGameAnimUtil::CreateFloatAnim("PA_ScrollOffToBottom", v1, v2, 0.5f, CIwGameAnimFrame::AT_QuadIn, 1));
	PresetAnimations.push_back(CIwGameAnimUtil::CreateFloatAnim("PA_FadeOn", 1, 255.0f, 0.5f, CIwGameAnimFrame::AT_QuadIn, 2));
	PresetAnimations.push_back(CIwGameAnimUtil::CreateFloatAnim("PA_FadeOff", 255.0f, 1, 0.5f, CIwGameAnimFrame::AT_QuadIn, 2));
}

void CIwGameUIBase::ReleasePresetAnimations()
{
	for (CIwList<CIwGameAnim*>::iterator it = PresetAnimations.begin(); it != PresetAnimations.end(); ++it)
		delete *it;
	PresetAnimations.clear();
}

void CIwGameUIBase::UpdatePresetAnimations()
{
	float width = (float)IwGxGetScreenWidth();
	float height = (float)IwGxGetScreenHeight();

	CIwGameAnim* anim = FindPresetAnimation(IW_GAME_HASH("PA_ScrollOnFromLeft"));
	((CIwGameAnimFrameVec2*)anim->getFrame(0))->data.x = -width;
	anim = FindPresetAnimation(IW_GAME_HASH("PA_ScrollOffToLeft"));
	((CIwGameAnimFrameVec2*)anim->getFrame(1))->data.x = -width;
	anim = FindPresetAnimation(IW_GAME_HASH("PA_ScrollOnFromRight"));
	((CIwGameAnimFrameVec2*)anim->getFrame(0))->data.x = width;
	anim = FindPresetAnimation(IW_GAME_HASH("PA_ScrollOffToRight"));
	((CIwGameAnimFrameVec2*)anim->getFrame(1))->data.x = width;
	anim = FindPresetAnimation(IW_GAME_HASH("PA_ScrollOnFromTop"));
	((CIwGameAnimFrameVec2*)anim->getFrame(0))->data.y = -height;
	anim = FindPresetAnimation(IW_GAME_HASH("PA_ScrollOffToTop"));
	((CIwGameAnimFrameVec2*)anim->getFrame(1))->data.y = -height;
	anim = FindPresetAnimation(IW_GAME_HASH("PA_ScrollOnFromBottom"));
	((CIwGameAnimFrameVec2*)anim->getFrame(0))->data.y = height;
	anim = FindPresetAnimation(IW_GAME_HASH("PA_ScrollOffToBottom"));
	((CIwGameAnimFrameVec2*)anim->getFrame(1))->data.y = height;
}


CIwGameAnim* CIwGameUIBase::FindPresetAnimation(unsigned int name_hash)
{
	for (CIwList<CIwGameAnim*>::iterator it = PresetAnimations.begin(); it != PresetAnimations.end(); ++it)
	{
		if ((*it)->getNameHash() == name_hash)
			return *it;
	}

	return NULL;
}

void CIwGameUIBase::NotifyParentsResize()
{
	// Notify parent that it needs to resize
	if (LinkedTo != NULL && LinkedTo->getCategory() == CIwGameActor::AC_UI)
	{
		CIwGameUIBase* ui = (CIwGameUIBase*)LinkedTo;
		if (ui->getSizeToContent())
		{
			ui->setNeedsResize(true);
			ui->NotifyParentsResize();
		}
	}
}


//
//
//
//
//
// CIwGameUIVideoOverlay implementation
//
//
//
//
//


bool CIwGameUIVideoOverlay::isPlaying() const
{
	return Playing;
}

bool CIwGameUIVideoOverlay::Play()
{
	if (Video != NULL)
	{
		// Only allow video to begin play after UI element has been rendered at least once to allow us to pick up screen coordinates
		CIwFVec2* v = Visual->getTransformedV();
		float dx = (v + 1)->x - v->x;
		if (dx != 0)
		{
			if (BackgroundBrush != NULL && BackgroundBrush->getBrushType() == IIwGameBrush::BT_9Patch)
				Video->setRect((int)v->x, (int)v->y, (int)((v + 3)->x - v->x), (int)((v + 12)->y - v->y));
			else
				Video->setRect((int)v->x, (int)v->y, (int)((v + 1)->x - v->x), (int)((v + 3)->y - v->y));
			Started = true;
			if (!Video->Play())
				return false;
			Playing = true;
			ProcessEventActions(CIwGameXomlNames::OnStart_Hash);

			return true;
		}
	}

	return false;
}

void CIwGameUIVideoOverlay::Stop()
{
	IW_GAME_VIDEO->Stop();

	if (Playing)
		ProcessEventActions(CIwGameXomlNames::OnEnd_Hash);

	Playing = false;
}

void CIwGameUIVideoOverlay::Pause()
{
	IW_GAME_VIDEO->Pause();
	if (Playing)
		ProcessEventActions(CIwGameXomlNames::OnPause_Hash);
}

void CIwGameUIVideoOverlay::Resume()
{
	IW_GAME_VIDEO->Resume();
	if (Playing)
		ProcessEventActions(CIwGameXomlNames::OnResume_Hash);
}

bool CIwGameUIVideoOverlay::Update(float dt)
{
	if (!CIwGameUIBase::Update(dt))
		return false;

	if (!Started && AutoPlay)
	{
		Play();
	}

	if (Playing)
	{
		if (!IW_GAME_VIDEO->isPlaying())
		{
			Playing = false;
			ProcessEventActions(CIwGameXomlNames::OnEnd_Hash);
		}
	}

	return true;
}

bool CIwGameUIVideoOverlay::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - VideoOverlay needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameUIBase::LoadFromXoml(parent, false, node))
		return false;

	// Process VideoOverlay specific attributes
	CIwGameString*	video_name = NULL;
	float			volume = 1.0f;
	int				repeat = 1;

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Video_Hash)
			video_name = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::AutoPlay_Hash)
			AutoPlay = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::Repeat_Hash)
			repeat = (*it)->GetValueAsInt();
		else
		if (name_hash == CIwGameXomlNames::Volume_Hash)
			volume = (*it)->GetValueAsFloat();
		else
		if (name_hash == CIwGameXomlNames::OnStart_Hash)
			EventsManager->addEvent("OnStart", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnEnd_Hash)
			EventsManager->addEvent("OnEnd", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnPause_Hash)
			EventsManager->addEvent("OnPause", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnResume_Hash)
			EventsManager->addEvent("OnResume", (*it)->GetValue().c_str());
	}

	if (LinkedTo != NULL)
	{
		if (Size.x == 0)
			Size.x = LinkedTo->getSize().x;
		if (Size.y == 0)
			Size.y = LinkedTo->getSize().y;
	}

	// Search the scenes resources for the specified video
	if (video_name != NULL)
	{
		Video = (CIwGameVideo*)Scene->getResourceManager()->findResource(video_name->c_str(), CIwGameXomlNames::Video_Hash);
		if (Video == NULL)
			CIwGameError::LogError("Warning: XOML - Could not find Video in the scene or in the global resources - ", video_name->c_str());
		else
		{
			Video->setVolume(volume);
			Video->setRepeatCount(repeat);
		}
	}

	if (load_children)
	{
		// Process inner tags
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}
	return true;
}

bool CIwGameUIVideoOverlay::getProperty(unsigned int element_name, CIwGameXomlProperty& prop)
{
	if (CIwGameUIBase::getProperty(element_name, prop))
		return true;

	if (element_name == CIwGameXomlNames::IsPlaying_Hash)
	{
		prop.Type = PT_Bool;
		prop.p_bool = Playing;
	}
	else
	if (element_name == CIwGameXomlNames::Started_Hash)
	{
		prop.Type = PT_Bool;
		prop.p_bool = Started;
	}
	else
	if (element_name == CIwGameXomlNames::Video_Hash)
	{
		prop.Type = PT_Resource;
		prop.p_data = Video;
	}
	else
		return false;

	return true;
}

bool CIwGameUIVideoOverlay::setProperty(unsigned int element_name, CIwGameString& data, bool delta)
{
	if (CIwGameUIBase::setProperty(element_name, data, delta))
		return true;

	if (element_name == CIwGameXomlNames::Command_Hash)
	{
		unsigned int command_hash = data.getHash();
		if (command_hash == IW_GAME_HASH("stop"))
			Stop();
		else
		if (command_hash == IW_GAME_HASH("play"))
			Play();
		else
		if (command_hash == IW_GAME_HASH("pause"))
			Pause();
		else
		if (command_hash == IW_GAME_HASH("resume"))
			Resume();
	}
	else
	if (element_name == CIwGameXomlNames::Video_Hash)
	{
		Video = (CIwGameVideo*)Scene->getResourceManager()->findResource(data.c_str(), CIwGameXomlNames::Video_Hash);
		if (Video == NULL)
			CIwGameError::LogError("Warning: XOML - Could not find Video in the scene or in the global resources - ", data.c_str());
	}
	else
	if (element_name == CIwGameXomlNames::Volume_Hash)
	{
		if (Video != NULL)
			Video->setVolume(data.GetAsFloat());
	}
	else
	if (element_name == CIwGameXomlNames::Repeat_Hash)
	{
		if (Video != NULL)
			Video->setRepeatCount(data.GetAsInt());
	}
	else
		return false;

	return true;
}

//
//
//
//
//
// CIwGameUILabel implementation
//
//
//
//
//
CIwGameUILabel::CIwGameUILabel() : CIwGameUIBase(), TextActor(NULL)
{
	setActualClassType("label");
	TextColour.r = 0xff;
	TextColour.g = 0xff;
	TextColour.b = 0xff;
	TextColour.a = 0xff;
	SelectedTextColour.r = 0xff;
	SelectedTextColour.g = 0xff;
	SelectedTextColour.b = 0xff;
	SelectedTextColour.a = 0xff;
	DisabledTextColour.r = 0xff;
	DisabledTextColour.g = 0xff;
	DisabledTextColour.b = 0xff;
	DisabledTextColour.a = 0xff;
}

void CIwGameUILabel::UpdateSizeFromText()
{
	if (!TextActor->isAutoHeight())
		return;

	SetDestSize(TextActor->getSize().x, TextActor->getSize().y);

	// Notify parent that it needs to resize
	NotifyParentsResize();
}

void CIwGameUILabel::setTextColour(CIwColour& colour)
{
	TextColour = colour;
	if (!Selected && Enabled)
	{
		if (TextActor != NULL)
			TextActor->setColour(colour);
	}
}

void CIwGameUILabel::setText(const char* text)
{
	if (text == NULL || CIwGameString::CalculateHash(text) == Text.getHash())
		return;

	Text = text;
	if (!Selected && Enabled)
	{
		if (TextActor != NULL)
		{
			TextActor->setText(text);
			// Text was changed so update container size if need be
			UpdateSizeFromText();
		}
	}
}

void CIwGameUILabel::setSelectedTextColour(CIwColour& colour)
{
	SelectedTextColour = colour;
	if (Selected && Enabled)
	{
		if (TextActor != NULL)
			TextActor->setColour(colour);
	}
}

void CIwGameUILabel::setSelectedText(const char* text)
{
	if (text == NULL || CIwGameString::CalculateHash(text) == SelectedText.getHash())
		return;

	SelectedText = text;
	if (Selected && Enabled)
	{
		if (TextActor != NULL)
			TextActor->setText(text);
	}
}

void CIwGameUILabel::setDisabledTextColour(CIwColour& colour)
{
	DisabledTextColour = colour;
	if (!Enabled)
	{
		if (TextActor != NULL)
			TextActor->setColour(colour);
	}
}

void CIwGameUILabel::setDisabledText(const char* text)
{
	if (text == NULL || CIwGameString::CalculateHash(text) == DisabledText.getHash())
		return;

	DisabledText = text;
	if (!Enabled)
	{
		if (TextActor != NULL)
			TextActor->setText(text);
	}
}

void CIwGameUILabel::setEnabled(bool enabled)
{
	CIwGameUIBase::setEnabled(enabled);

	if (TextActor == NULL)
		return;

	if (!enabled)
	{
		if (!DisabledText.IsEmpty())
			TextActor->setText(DisabledText.c_str());
		TextActor->setColour(DisabledTextColour);
	}
	else
	{
		if (Selected)
		{
			if (!SelectedText.IsEmpty())
				TextActor->setText(SelectedText.c_str());
			TextActor->setColour(SelectedTextColour);
		}
		else
		{
			if (!Text.IsEmpty())
				TextActor->setText(Text.c_str());
			TextActor->setColour(TextColour);
		}
	}
}

bool CIwGameUILabel::InitTextActor(CIwGameFont* font, CIwRect& rect, CIwGameString& text, int flags, IwGxFontAlignHor align_h, IwGxFontAlignVer align_v, CIwFVec4& skew, CIwRect& text_margin, bool auto_height, bool text_use_parent_opacity)
{
	TextActor = new CIwGameActorText();
	Scene->addActor(TextActor);
	TextActor->setOrphan(Orphan);
	TextActor->setLinkedTo(this);

	if (text_margin.x != 0 && text_margin.y != 0 && text_margin.w != 0  && text_margin.h != 0 )
		TextActor->setMarginIsPadding(true);
	TextActor->setAutoHeight(auto_height);
	TextActor->setMargin(text_margin.x, text_margin.y, text_margin.w, text_margin.h);
	TextActor->Init(font, rect, text, flags);
	TextActor->setAlignH(align_h);
	TextActor->setAlignV(align_v);
	TextActor->setColour(TextColour);
//	TextActor->setRect(rect);
	TextActor->setDepth(0);
	TextActor->setLayer(Layer);
	TextActor->setIgnoreCamera(IgnoreCamera);
	TextActor->setUseParentOpacity(text_use_parent_opacity);
	TextActor->getVisual()->setSkew(skew.x, skew.y, skew.z, skew.w);

	UpdateSizeFromText();

	return true;
}

bool CIwGameUILabel::Update(float dt)
{
	bool font_loaded = TextActor->isFontLoaded();

	if (!CIwGameUIBase::Update(dt))
		return false;

	if (!font_loaded && TextActor->isFontLoaded())
		NotifyParentsResize();
	else
	{
		if (TextActor != NULL)
		{
			if (TextActor->getSize().y != Size.y)
				UpdateSizeFromText();
		}
	}

	return true;
}

bool CIwGameUILabel::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - Label needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameUIBase::LoadFromXoml(parent, false, node))
		return false;

	// Process label specific attributes
	CIwGameString	font_name;
	IwGxFontAlignHor alignh = IW_GX_FONT_ALIGN_CENTRE;
	IwGxFontAlignVer alignv = IW_GX_FONT_ALIGN_MIDDLE;
	int				flags = 0;
	CIwRect			rect = CIwRect(0, 0, -100, -100);
	CIwRect			text_margin = CIwRect(0, 0, 0, 0);
	CIwFVec4		skew = CIwFVec4(0, 0, 0, 0);
	bool			text_colour_set = false;
	bool			sel_text_colour_set = false;
	bool			text_auto_height = false;
	bool			text_use_parent_opacity = false;

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Font_Hash)
		{
			font_name = (*it)->GetValue();
		}
		else
		if (name_hash == CIwGameXomlNames::Text_Hash)
		{
			Text = (*it)->GetValue();
		}
		else
		if (name_hash == CIwGameXomlNames::SelectedText_Hash)
		{
			SelectedText = (*it)->GetValue();
		}
		else
		if (name_hash == CIwGameXomlNames::DisabledText_Hash)
		{
			DisabledText = (*it)->GetValue();
		}
		else
		if (name_hash == CIwGameXomlNames::TextColour_Hash)
		{
			if (!(*it)->GetValueAsColour(TextColour))
				CIwGameError::LogError("Warning: XOML - Invalid value for Label::TextColour expected colour");
			else
				text_colour_set = true;
		}
		else
		if (name_hash == CIwGameXomlNames::SelectedTextColour_Hash)
		{
			if (!(*it)->GetValueAsColour(SelectedTextColour))
				CIwGameError::LogError("Warning: XOML - Invalid value for Label::SelectedTextColour expected colour");
			else
				sel_text_colour_set = true;
		}
		else
		if (name_hash == CIwGameXomlNames::DisabledTextColour_Hash)
		{
			if (!(*it)->GetValueAsColour(DisabledTextColour))
				CIwGameError::LogError("Warning: XOML - Invalid value for Label::DisabledTextColour expected colour");
		}
		else
		if (name_hash == CIwGameXomlNames::AlignH_Hash)
		{
			unsigned int align_hash = (*it)->GetValue().getHash();
			if (align_hash == CIwGameXomlNames::left_Hash)
				alignh = IW_GX_FONT_ALIGN_LEFT;
			else
			if (align_hash == CIwGameXomlNames::centre_Hash)
				alignh = IW_GX_FONT_ALIGN_CENTRE;
			else
			if (align_hash == CIwGameXomlNames::right_Hash)
				alignh = IW_GX_FONT_ALIGN_RIGHT;
		}
		else
		if (name_hash == CIwGameXomlNames::AlignV_Hash)
		{
			unsigned int align_hash = (*it)->GetValue().getHash();
			if (align_hash == CIwGameXomlNames::top_Hash)
				alignv = IW_GX_FONT_ALIGN_TOP;
			else
			if (align_hash == CIwGameXomlNames::middle_Hash)
				alignv = IW_GX_FONT_ALIGN_MIDDLE;
			else
			if (align_hash == CIwGameXomlNames::bottom_Hash)
				alignv = IW_GX_FONT_ALIGN_BOTTOM;
		}
		else
		if (name_hash == CIwGameXomlNames::Wrap_Hash)
		{
			if ((*it)->GetValueAsBool())
				flags |= IW_GX_FONT_DEFAULT_F;
			else
				flags |= IW_GX_FONT_NOWRAP_F;
		}
		else
		if (name_hash == CIwGameXomlNames::rect_Hash)
		{
			if (!(*it)->GetValueAsRect(rect))
			{
				CIwGameError::LogError("Warning: XOML - Invalid value for Label::Rect expected rect");
			}
		}
		else
		if (name_hash == CIwGameXomlNames::TextSkew_Hash)
		{
			if (!(*it)->GetValueAsPoint4(skew))
				CIwGameError::LogError("Warning: XOML - Invalid value for Label::TextSkew expected vec4");
		}
		else
		if (name_hash == CIwGameXomlNames::TextMargin_Hash)
		{
			if (!(*it)->GetValueAsRect(text_margin))
			{
				CIwGameError::LogError("Warning: XOML - Invalid value for Label::TextMargin expected rect");
			}
		}
		else
		if (name_hash == CIwGameXomlNames::AutoHeight_Hash)
			text_auto_height = (*it)->GetValueAsBool();
		else
			if (name_hash == CIwGameXomlNames::TextUseParentOpacity_Hash)
			text_use_parent_opacity = (*it)->GetValueAsBool();
	}

	if (text_colour_set && !sel_text_colour_set)
		SelectedTextColour = TextColour;

	// if base bacjground nto supplied then set size to parents size
	if (LinkedTo != NULL)
	{
		if (Size.x == 0)
			Size.x = LinkedTo->getSize().x;
		if (Size.y == 0)
			Size.y = LinkedTo->getSize().y;
	}

	// Search the scenes resources for the specified font
	CIwGameFont* font = (CIwGameFont*)Scene->getResourceManager()->findResource(font_name.c_str(), CIwGameXomlNames::Font_Hash);
	if (font == NULL)
		CIwGameError::LogError("Warning: XOML - Could not find label font in the scene or in the global resources - ", font_name.c_str());

	// Initialise the text component actor
	if (rect.w == 0)
	{
		CIwRect rc = CIwRect(-Size.x / 2, -Size.y / 2, Size.x, Size.y);
		InitTextActor(font, rc, Text, flags, alignh, alignv, skew, text_margin, text_auto_height, text_use_parent_opacity);
	}
	else
		InitTextActor(font, rect, Text, flags, alignh, alignv, skew, text_margin, text_auto_height, text_use_parent_opacity);
	setTextColour(TextColour);

	if (load_children)
	{
		// Process inner tags
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}
	return true;
}

bool CIwGameUILabel::getProperty(unsigned int element_name, CIwGameXomlProperty& prop)
{
	if (CIwGameUIBase::getProperty(element_name, prop))
		return true;

	if (element_name == CIwGameXomlNames::SelectedText_Hash)
	{
		prop.Type = PT_String;
		prop.p_data = (void*)SelectedText.c_str();
	}
	else
	if (element_name == CIwGameXomlNames::DisabledText_Hash)
	{
		prop.Type = PT_String;
		prop.p_data = (void*)DisabledText.c_str();
	}
	else
	if (element_name == CIwGameXomlNames::TextColour_Hash)
	{
		prop.Type = PT_Vec4;
		prop.p_vec[0] = (float)TextColour.r;
		prop.p_vec[1] = (float)TextColour.g;
		prop.p_vec[2] = (float)TextColour.b;
		prop.p_vec[3] = (float)TextColour.a;
	}
	else
	if (element_name == CIwGameXomlNames::SelectedTextColour_Hash)
	{
		prop.Type = PT_Vec4;
		prop.p_vec[0] = (float)SelectedTextColour.r;
		prop.p_vec[1] = (float)SelectedTextColour.g;
		prop.p_vec[2] = (float)SelectedTextColour.b;
		prop.p_vec[3] = (float)SelectedTextColour.a;
	}
	else
	if (element_name == CIwGameXomlNames::DisabledTextColour_Hash)
	{
		prop.Type = PT_Vec4;
		prop.p_vec[0] = (float)DisabledTextColour.r;
		prop.p_vec[1] = (float)DisabledTextColour.g;
		prop.p_vec[2] = (float)DisabledTextColour.b;
		prop.p_vec[3] = (float)DisabledTextColour.a;
	}
	else
		return false;

	return true;
}

bool CIwGameUILabel::setProperty(unsigned int element_name, CIwGameString& data, bool delta)
{
	if (CIwGameUIBase::setProperty(element_name, data, delta))
		return true;

	float float_pool[8];

	if (element_name == CIwGameXomlNames::Text_Hash)
	{
		if (delta)
		{
			Text += data;
			setText(Text.c_str());
		}
		else
			setText(data.c_str());
	}
	else
	if (element_name == CIwGameXomlNames::SelectedText_Hash)
	{
		if (delta)
			SelectedText += data;
		else
			setSelectedText(data.c_str());
	}
	else
	if (element_name == CIwGameXomlNames::DisabledText_Hash)
	{
		if (delta)
			DisabledText += data;
		else
			setDisabledText(data.c_str());
	}
	else
	if (element_name == CIwGameXomlNames::TextColour_Hash)
	{
		CIwColour c;
		data.GetAsListOfFloat(float_pool);
		if (delta)
		{
			c = TextColour;
			c.r += (uint8)float_pool[0];
			c.g += (uint8)float_pool[1];
			c.b += (uint8)float_pool[2];
			c.a += (uint8)float_pool[3];
		}
		else
		{
			c.r = (uint8)float_pool[0];
			c.g = (uint8)float_pool[1];
			c.b = (uint8)float_pool[2];
			c.a = (uint8)float_pool[3];
		}
		setTextColour(c);
	}
	else
	if (element_name == CIwGameXomlNames::SelectedTextColour_Hash)
	{
		CIwColour c;
		data.GetAsListOfFloat(float_pool);
		if (delta)
		{
			c = SelectedTextColour;
			c.r += (uint8)float_pool[0];
			c.g += (uint8)float_pool[1];
			c.b += (uint8)float_pool[2];
			c.a += (uint8)float_pool[3];
		}
		else
		{
			c.r = (uint8)float_pool[0];
			c.g = (uint8)float_pool[1];
			c.b = (uint8)float_pool[2];
			c.a = (uint8)float_pool[3];
		}
		setSelectedTextColour(c);
	}
	else
	if (element_name == CIwGameXomlNames::DisabledTextColour_Hash)
	{
		CIwColour c;
		data.GetAsListOfFloat(float_pool);
		if (delta)
		{
			c = DisabledTextColour;
			c.r += (uint8)float_pool[0];
			c.g += (uint8)float_pool[1];
			c.b += (uint8)float_pool[2];
			c.a += (uint8)float_pool[3];
		}
		else
		{
			c.r = (uint8)float_pool[0];
			c.g = (uint8)float_pool[1];
			c.b = (uint8)float_pool[2];
			c.a = (uint8)float_pool[3];
		}
		setDisabledTextColour(c);
	}
	else
	if (element_name == CIwGameXomlNames::AlignH_Hash)
	{
		if (TextActor != NULL)
		{
			IwGxFontAlignHor alignh = IW_GX_FONT_ALIGN_CENTRE;

			unsigned int align_hash = data.getHash();
			if (align_hash == CIwGameXomlNames::left_Hash)
				alignh = IW_GX_FONT_ALIGN_LEFT;
			else
			if (align_hash == CIwGameXomlNames::right_Hash)
				alignh = IW_GX_FONT_ALIGN_RIGHT;
			TextActor->setAlignH(alignh);
		}
		else
			return false;
	}
	else
	if (element_name == CIwGameXomlNames::AlignV_Hash)
	{
		if (TextActor != NULL)
		{
			IwGxFontAlignVer alignv = IW_GX_FONT_ALIGN_MIDDLE;

			unsigned int align_hash = data.getHash();
			if (align_hash == CIwGameXomlNames::top_Hash)
				alignv = IW_GX_FONT_ALIGN_TOP;
			else
			if (align_hash == CIwGameXomlNames::bottom_Hash)
				alignv = IW_GX_FONT_ALIGN_BOTTOM;
			TextActor->setAlignV(alignv);
		}
		else
			return false;
	}
	else
	if (element_name == CIwGameXomlNames::Wrap_Hash)
	{
		if (TextActor != NULL)
		{
			if (data.GetAsBool())
				TextActor->setFlags(IW_GX_FONT_DEFAULT_F);
			else
				TextActor->setFlags(IW_GX_FONT_NOWRAP_F);
		}
		else
			return false;
	}
	else
	if (element_name == CIwGameXomlNames::TextSkew_Hash)
	{
		if (TextActor != NULL || TextActor->getVisual() == NULL)
		{
			CIwFVec4 skew = TextActor->getVisual()->getSkew();
			data.GetAsListOfFloat(float_pool);
			if (delta)
			{
				skew.x += float_pool[0];
				skew.y += float_pool[1];
				skew.z += float_pool[2];
				skew.w += float_pool[3];
			}
			else
			{
				skew.x = float_pool[0];
				skew.y = float_pool[1];
				skew.z = float_pool[2];
				skew.w = float_pool[3];
			}
			TextActor->getVisual()->setSkew(skew.x, skew.y, skew.z, skew.w);
		}
		else
			return false;
	}
	else
	if (element_name == CIwGameXomlNames::AutoHeight_Hash)
	{
		if (TextActor != NULL)
		{
			TextActor->setAutoHeight(data.GetAsBool());
			UpdateSizeFromText();
		}
		else
			return false;
	}
	else
		return false;

	return true;
}

bool CIwGameUILabel::UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var)
{
	if (!var->Modified || var->getValue().IsEmpty())
		return false;
	if (CIwGameUIBase::UpdateBinding(element_name, var))
		return true;

	if (element_name == CIwGameXomlNames::Text_Hash)
		setText(var->getValue().c_str());
	else
	if (element_name == CIwGameXomlNames::SelectedText_Hash)
	{
		if (var->Type == VT_String)
			setSelectedText(var->getValue().c_str());
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Label Incorrect binding variable type, expected string for SelectedText");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::DisabledText_Hash)
	{
		if (var->Type == VT_String)
			setDisabledText(var->getValue().c_str());
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Label Incorrect binding variable type, expected string for DisabledText");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::TextColour_Hash)
	{
		if (var->Type == VT_Vec4)
		{
			CIwFVec4* data = &((CIwGameXomlVariableVec4*)var)->NativeValue;
			CIwColour c;
			c.r = (uint8)data->x;
			c.g = (uint8)data->y;
			c.b = (uint8)data->z;
			c.a = (uint8)data->w;
			setTextColour(c);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Label Incorrect binding variable type, expected vec4 for TextColour");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::SelectedTextColour_Hash)
	{
		if (var->Type == VT_Vec4)
		{
			CIwFVec4* data = &((CIwGameXomlVariableVec4*)var)->NativeValue;
			CIwColour c;
			c.r = (uint8)data->x;
			c.g = (uint8)data->y;
			c.b = (uint8)data->z;
			c.a = (uint8)data->w;
			setSelectedTextColour(c);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Label Incorrect binding variable type, expected vec4 for SelectedTextColour");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::DisabledTextColour_Hash)
	{
		if (var->Type == VT_Vec4)
		{
			CIwFVec4* data = &((CIwGameXomlVariableVec4*)var)->NativeValue;
			CIwColour c;
			c.r = (uint8)data->x;
			c.g = (uint8)data->y;
			c.b = (uint8)data->z;
			c.a = (uint8)data->w;
			setDisabledTextColour(c);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Label Incorrect binding variable type, expected vec4 for DisabledTextColour");
#endif // _DEBUG
	}
	else
		return false;

	return true;
}

bool CIwGameUILabel::UpdateFromAnimation(CIwGameAnimInstance *animation)
{
	if (CIwGameUIBase::UpdateFromAnimation(animation))
		return true;

	unsigned int element_name = animation->getTargetPropertyHash();
	bool delta = animation->isDelta();

	if (element_name == CIwGameXomlNames::Text_Hash)
	{
		CIwGameAnimFrameString* frame = (CIwGameAnimFrameString*)animation->getCurrentData();
		if (delta)
		{
			CIwGameString text = Text;
			text += frame->data;
			setText(text.c_str());
		}
		else
		{
			setText(frame->data.c_str());
		}
	}
	else
	if (element_name == CIwGameXomlNames::TextColour_Hash)
	{
		CIwGameAnimFrameVec4* frame = (CIwGameAnimFrameVec4*)animation->getCurrentData();
		if (delta)
		{
			CIwColour c = TextColour;
			c.r += (uint8)frame->data.x;
			c.g += (uint8)frame->data.y;
			c.b += (uint8)frame->data.z;
			c.a += (uint8)frame->data.w;
			setTextColour(c);
		}
		else
		{
			CIwColour c = TextColour;
			c.r = (uint8)frame->data.x;
			c.g = (uint8)frame->data.y;
			c.b = (uint8)frame->data.z;
			c.a = (uint8)frame->data.w;
			setTextColour(c);
		}
	}
	else
	if (element_name == CIwGameXomlNames::SelectedTextColour_Hash)
	{
		CIwGameAnimFrameVec4* frame = (CIwGameAnimFrameVec4*)animation->getCurrentData();
		if (delta)
		{
			CIwColour c = SelectedTextColour;
			c.r += (uint8)frame->data.x;
			c.g += (uint8)frame->data.y;
			c.b += (uint8)frame->data.z;
			c.a += (uint8)frame->data.w;
			setSelectedTextColour(c);
		}
		else
		{
			CIwColour c = SelectedTextColour;
			c.r = (uint8)frame->data.x;
			c.g = (uint8)frame->data.y;
			c.b = (uint8)frame->data.z;
			c.a = (uint8)frame->data.w;
			setSelectedTextColour(c);
		}
	}
	else
	if (element_name == CIwGameXomlNames::DisabledTextColour_Hash)
	{
		CIwGameAnimFrameVec4* frame = (CIwGameAnimFrameVec4*)animation->getCurrentData();
		if (delta)
		{
			CIwColour c = DisabledTextColour;
			c.r += (uint8)frame->data.x;
			c.g += (uint8)frame->data.y;
			c.b += (uint8)frame->data.z;
			c.a += (uint8)frame->data.w;
			setDisabledTextColour(c);
		}
		else
		{
			CIwColour c = DisabledTextColour;
			c.r = (uint8)frame->data.x;
			c.g = (uint8)frame->data.y;
			c.b = (uint8)frame->data.z;
			c.a = (uint8)frame->data.w;
			setDisabledTextColour(c);
		}
	}
	else
		return false;

	return true;
}


//
//
//
//
//
// CIwGameUITextBox implementation
//
//
//
//
//
CIwGameUITextBox::CIwGameUITextBox() : CIwGameUILabel()
{
	setActualClassType("textbox");
	TypeHint = ITH_Text;
}

void CIwGameUITextBox::OnTapped(int index, int x, int y, bool allow_bubble)
{
	if (!Enabled)
		return;

	CIwGameUILabel::OnTapped(index, x, y, allow_bubble);

	if (Prompt.IsEmpty())
		Prompt = "Enter Text";

	const char* str = IW_GAME_INPUT->showOnScreenKeyboard(Prompt.c_str(), TypeHint, Text.c_str());
	if (str != NULL)
	{
		setText(str);

		// Find and update target variable
		if (!TargetVariable.IsEmpty())
		{
			CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(TargetVariable, Scene);
			if (var != NULL)
				var->setValue(Text.c_str());
		}
		OnTextChanged();
	}

}

void CIwGameUITextBox::OnTextChanged()
{
	ProcessEventActions(CIwGameXomlNames::OnTextChanged_Hash);
}

bool CIwGameUITextBox::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - TextBox needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameUILabel::LoadFromXoml(parent, false, node))
		return false;

	// Process text box specific attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Variable_Hash)
		{
			TargetVariable = (*it)->GetValue();
		}
		else
		if (name_hash == CIwGameXomlNames::Prompt_Hash)
		{
			Prompt = (*it)->GetValue();
		}
		else
		if (name_hash == CIwGameXomlNames::TypeHint_Hash)
		{
			unsigned int type_hash = (*it)->GetValue().getHash();
			if (type_hash == CIwGameString::CalculateHash("number"))
				TypeHint = ITH_Number;
			else
			if (type_hash == CIwGameString::CalculateHash("password"))
				TypeHint = ITH_Password;
			else
			if (type_hash == CIwGameString::CalculateHash("email"))
				TypeHint = ITH_Email;
			else
			if (type_hash == CIwGameString::CalculateHash("url"))
				TypeHint = ITH_URL;
			else
				TypeHint = ITH_Text;
		}
		else
		if (name_hash == CIwGameXomlNames::OnTextChanged_Hash)
			EventsManager->addEvent("OnTextChanged", (*it)->GetValue().c_str());
	}

	if (load_children)
	{
		// Process inner tags
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}

	return true;
}

bool CIwGameUITextBox::setProperty(unsigned int element_name, CIwGameString& data, bool delta)
{
	if (CIwGameUILabel::setProperty(element_name, data, delta))
		return true;

	if (element_name == CIwGameXomlNames::Prompt_Hash)
		setPrompt(data.c_str());
	else
	if (element_name == CIwGameXomlNames::Variable_Hash)
		setTargetVariable(data.c_str());
	else
	if (element_name == CIwGameXomlNames::TypeHint_Hash)
	{
		unsigned int type_hash = data.getHash();
		if (type_hash == CIwGameString::CalculateHash("number"))
			setTypeHint(ITH_Number);
		else
		if (type_hash == CIwGameString::CalculateHash("password"))
			setTypeHint(ITH_Password);
		else
		if (type_hash == CIwGameString::CalculateHash("email"))
			setTypeHint(ITH_Email);
		else
		if (type_hash == CIwGameString::CalculateHash("url"))
			setTypeHint(ITH_URL);
		else
			setTypeHint(ITH_Text);
	}
	else
	if (element_name == CIwGameXomlNames::OnTextChanged_Hash)
		EventsManager->addEvent("OnTextChanged", data.c_str(), true);
	else
		return false;

	return true;
}

bool CIwGameUITextBox::UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var)
{
	if (!var->Modified || var->getValue().IsEmpty())
		return false;
	if (CIwGameUILabel::UpdateBinding(element_name, var))
		return true;

	if (element_name == CIwGameXomlNames::Prompt_Hash)
	{
		if (var->Type == VT_String)
			setPrompt(var->getValue().c_str());
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: TextBox Incorrect binding variable type, expected string for Prompt");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::Variable_Hash)
	{
		if (var->Type == VT_String)
			setTargetVariable(var->getValue().c_str());
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: TextBox Incorrect binding variable type, expected string for Variable");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::TypeHint_Hash)
	{
		if (var->Type == VT_String)
		{
			unsigned int type_hash = var->getValue().getHash();
			if (type_hash == CIwGameString::CalculateHash("number"))
				setTypeHint(ITH_Number);
			else
			if (type_hash == CIwGameString::CalculateHash("password"))
				setTypeHint(ITH_Password);
			else
			if (type_hash == CIwGameString::CalculateHash("email"))
				setTypeHint(ITH_Email);
			else
			if (type_hash == CIwGameString::CalculateHash("url"))
				setTypeHint(ITH_URL);
			else
				setTypeHint(ITH_Text);
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: TextBox Incorrect binding variable type, expected string for TypeHint");
#endif // _DEBUG
	}
	else
		return false;

	return true;
}



