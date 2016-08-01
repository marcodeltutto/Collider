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

#include "IwGameUIPanels.h"
#include "IwGameUtil.h"
#include "IwGameString.h"
#include "IwGameTemplates.h"

//
//
//
//
//
// CIwGameUICanvas implementation
//
//
//
//
//

CIwGameUICanvas::~CIwGameUICanvas()
{
	Children.clear();
}

void CIwGameUICanvas::LinkChanged(CIwGameActor* child, bool remove)
{
	if (remove)
		RemoveActor(child);
	else
	{
		Children.push_back(child);
		child->setScreenDocking(false);
	}
}

void CIwGameUICanvas::RemoveActor(CIwGameActor* actor)
{
	// Renove named actor from the panel
	for (_Iterator it = Children.begin(); it != Children.end(); ++it)
	{
		if (*it == actor)
		{
			Children.erase(it);
			break;
		}
	}
}

bool CIwGameUICanvas::UpdateLayout()
{
	float width = (float)Size.x / 2;
	float height = (float)Size.y / 2;

	float dx = PreviousScrollPos.x - ScrollPos.x;
	float dy = PreviousScrollPos.y - ScrollPos.y;

	// Dock any actors that are set as dockable and scroll rest
	for (_Iterator it = begin(); it != end(); ++it)
	{
		CIwGameActor* actor = *it;
		eIwGameScene_Docking docking = actor->getDocking();
		if (docking != DK_None)
		{
			CIwFVec2 pos = CIwFVec2(0, 0);
			CIwRect margin = actor->getMargin();

			switch (docking)
			{
			case DK_Top:
				pos.x = Position.x;
				pos.y = Position.y - height + (actor->getSize().y + 1) / 2 + margin.w;
				break;
			case DK_Bottom:
				pos.x = Position.x;
				pos.y = Position.y + height - ((actor->getSize().y + 1) / 2 + margin.h);
				break;
			case DK_Left:
				pos.x = Position.x - width + (actor->getSize().x + 1) / 2 + margin.x;
				pos.y = Position.y;
				break;
			case DK_Right:
				pos.x = Position.x + width - ((actor->getSize().x + 1) / 2 + margin.y);
				pos.y = Position.y;
				break;
			case DK_TopLeft:
				pos.x = Position.x - width + (actor->getSize().x + 1) / 2 + margin.x;
				pos.y = Position.y - height + (actor->getSize().y + 1) / 2 + margin.w;
				break;
			case DK_TopRight:
				pos.x = Position.x + width - ((actor->getSize().x + 1) / 2 + margin.y);
				pos.y = Position.y - height + (actor->getSize().y + 1) / 2 + margin.w;
				break;
			case DK_BottomLeft:
				pos.x = Position.x - width + (actor->getSize().x + 1) / 2 + margin.x;
				pos.y = Position.y + height - ((actor->getSize().y + 1) / 2 + margin.h);
				break;
			case DK_BottomRight:
				pos.x = Position.x + width - ((actor->getSize().x + 1) / 2 + margin.y);
				pos.y = Position.y + height - ((actor->getSize().y + 1) / 2 + margin.h);
				break;
			}

			actor->setPosition(pos);
		}
		else
		{
			CIwFVec2 pos = actor->getPosition();
			actor->setPosition(pos.x + dx, pos.y + dy);
		}
	}

	return CIwGameUIBase::UpdateLayout();
}

bool CIwGameUICanvas::Update(float dt)
{
	if (!CIwGameUIBase::Update(dt))
		return false;

	// Set clip rect for the list box
	CIwRect rc;
	rc.x = (-Size.x >> 1) + ClippingMargin.x;
	rc.y = (-Size.y >> 1) + ClippingMargin.w;
	rc.w = Size.x - (ClippingMargin.x +  + ClippingMargin.y);
	rc.h = Size.y - (ClippingMargin.w +  + ClippingMargin.h);
	Visual->setClipRect(rc);

	return true;
}

bool CIwGameUICanvas::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - Canvas needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameUIBase::LoadFromXoml(parent, false, node))
		return false;

	// Process StackPanel specific attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

	}

	// Process inner tags
	if (load_children)
	{
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}

	ResizeToContent();

	return true;
}

//
//
//
//
//
// CIwGameUIStackPanel implementation
//
//
//
//
//

CIwGameUIStackPanel::~CIwGameUIStackPanel()
{
	Children.clear();
}

void CIwGameUIStackPanel::LinkChanged(CIwGameActor* child, bool remove)
{
	if (remove)
		RemoveActor(child);
	else
		Children.push_back(child);
}

void CIwGameUIStackPanel::RemoveActor(CIwGameActor* actor)
{
	// Renove named actor from the panel
	for (_Iterator it = Children.begin(); it != Children.end(); ++it)
	{
		if (*it == actor)
		{
			Children.erase(it);
			break;
		}
	}
}

bool CIwGameUIStackPanel::UpdateLayout()
{
	// Stack all actors
	_Iterator _begin = begin();
	_Iterator _end = end();
	if (Children.size() != 0)
	{
		_Iterator it = _begin;
		CIwRect margin = (*it)->getMargin();
		CIwVec2 pos;
		
		// Calculate initial start position of content
		if (Orientation == Orientation_Horizontal)
		{
			pos = CIwVec2(-Size.x / 2, margin.w);
			if (AlignH == AlignH_Left)
				pos.x += (*it)->getSize().x / 2;
		}
		else
		{
			pos = CIwVec2(margin.x, -Size.y / 2);
			if (AlignV == AlignV_Top)
				pos.y += (*it)->getSize().y / 2;
		}

		// Arrange the content
		CIwVec2 opos = CIwVec2(pos.x, pos.y);
		_Iterator it2 = _begin;
		if (it2 != _end)
			it2++;
		for (it = _begin; it != _end; ++it)
		{
			if (Orientation == Orientation_Horizontal)
			{
				CIwRect margin = (*it)->getMargin();
				if (AlignV == AlignV_Top)
					pos.y = -Size.y / 2 + (*it)->getSize().y / 2 + margin.w;
				else
				if (AlignV == AlignV_Bottom)
					pos.y = Size.y / 2 - (*it)->getSize().y / 2 - margin.h;
				else
					pos.y = 0;
				pos.x += margin.x;
				(*it)->setPosition((float)pos.x, (float)pos.y);
				int d = (*it)->getSize().x / 2;
				if (it2 != _end)
				{
					d += (*it2)->getSize().x / 2;
					it2++;
				}
				pos.x += d + margin.y;
			}
			else
			{
				CIwRect margin = (*it)->getMargin();
				if (AlignH == AlignH_Left)
					pos.x = -Size.x / 2 + (*it)->getSize().x / 2 + margin.x;
				else
				if (AlignH == AlignH_Right)
					pos.x = Size.x / 2 - (*it)->getSize().x / 2 - margin.y;
				else
					pos.x = 0;
				pos.y += margin.w;
				(*it)->setPosition((float)pos.x, (float)pos.y);
				int d = (*it)->getSize().y / 2;
				if (it2 != _end)
				{
					d += (*it2)->getSize().y / 2;
					it2++;
				}
				pos.y += d + margin.h;
			}
		}

		// If content is aligned to right or bottom of stack panel then adjust positions to align them with edge
		if (Orientation == Orientation_Horizontal)
		{
			if (AlignH == AlignH_Right)
			{
				float dx = (float)(Size.x - (pos.x - opos.x));	// Calculate how much to adjust positions to align with right edge of stack panel
				for (it = _begin; it != _end; ++it)
				{
					CIwFVec2 pos2 = (*it)->getPosition();
					pos2.x += dx;
					(*it)->setPosition(pos2);
				}
			}
			else
			if (AlignH == AlignH_Centre)
			{
				float dx = (float)(((Size.x + (*_begin)->getSize().x / 2 - (pos.x - opos.x)) / 2));	// Calculate how much to adjust positions to align with centre of stack panel
				for (it = _begin; it != _end; ++it)
				{
					CIwFVec2 pos2 = (*it)->getPosition();
					pos2.x += dx;
					(*it)->setPosition(pos2);
				}
			}
		}
		else
		if (Orientation == Orientation_Vertical)
		{
			if (AlignV == AlignV_Bottom)
			{
				float dy = (float)(Size.y - (pos.y - opos.y));	// Calculate how much to adjust positions to align with bottom edge of stack panel
				for (it = _begin; it != _end; ++it)
				{
					CIwFVec2 pos2 = (*it)->getPosition();
					pos2.y += dy;
					(*it)->setPosition(pos2);
				}
			}
			else
			if (AlignV == AlignV_Middle)
			{
				float dy = (float)(((Size.y + (*_begin)->getSize().y / 2 - (pos.y - opos.y)) / 2));	// Calculate how much to adjust positions to align with middle of stack panel
				for (it = _begin; it != _end; ++it)
				{
					CIwFVec2 pos2 = (*it)->getPosition();
					pos2.y += dy;
					(*it)->setPosition(pos2);
				}
			}
		}
	}

	return CIwGameUIBase::UpdateLayout();
}

bool CIwGameUIStackPanel::Update(float dt)
{
	if (!CIwGameUIBase::Update(dt))
		return false;

	return true;
}

void CIwGameUIStackPanel::ResizeToContent()
{
	if (SizeToContent)
	{
		int w = 0, h = 0;
		for (_Iterator it = begin(); it != end(); ++it)
		{
			CIwRect margin = (*it)->getMargin();
			CIwVec2 size = (*it)->getSize();
			int x = size.x + margin.x + margin.y;
			int y = size.y + margin.w + margin.h;
			if (Orientation == Orientation_Horizontal)
			{
				w += x;
				if (y > h)
					h = y;
			}
			else
			{
				h += y;
				if (x > w)
					w = x;
			}
		}
		switch (SizeToContent)
		{
		case Axis_X:
			Size.x = w;
			if (Visual != NULL)
				h = Visual->getDestHeight();
			break;
		case Axis_Y:
			Size.y = h;
			if (Visual != NULL)
				w = Visual->getDestWidth();
			break;
		case Axis_XY:
			Size.x = w;
			Size.y = h;
			break;
		}
		SetDestSize(w, h);
		LayoutDirty = true;
	}

	CIwGameUIBase::ResizeToContent();
}

bool CIwGameUIStackPanel::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - StackPanel needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameUIBase::LoadFromXoml(parent, false, node))
		return false;

	// Process StackPanel specific attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Orientation_Hash)
		{
			unsigned int hash = (*it)->GetValue().getHash();
			if (hash == CIwGameString::CalculateHash("vertical"))
				Orientation = Orientation_Vertical;
		}
		else
		if (name_hash == CIwGameXomlNames::AlignH_Hash)
		{
			unsigned int align_hash = (*it)->GetValue().getHash();
			if (align_hash == CIwGameXomlNames::left_Hash)
				AlignH = AlignH_Left;
			else
			if (align_hash == CIwGameXomlNames::right_Hash)
				AlignH = AlignH_Right;
			else
				AlignH = AlignH_Centre;
		}
		else
		if (name_hash == CIwGameXomlNames::AlignV_Hash)
		{
			unsigned int align_hash = (*it)->GetValue().getHash();
			if (align_hash == CIwGameXomlNames::top_Hash)
				AlignV = AlignV_Top;
			else
			if (align_hash == CIwGameXomlNames::bottom_Hash)
				AlignV = AlignV_Bottom;
			else
				AlignV = AlignV_Middle;
		}
	}

	// Process inner tags
	if (load_children)
	{
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}

	ResizeToContent();

	return true;
}

//
//
//
//
//
// CIwGameUIWrapPanel implementation
//
//
//
//
//

CIwGameUIWrapPanel::~CIwGameUIWrapPanel()
{
	Children.clear();
}

void CIwGameUIWrapPanel::LinkChanged(CIwGameActor* child, bool remove)
{
	if (remove)
		RemoveActor(child);
	else
		Children.push_back(child);
}

void CIwGameUIWrapPanel::RemoveActor(CIwGameActor* actor)
{
	// Renove named actor from the panel
	for (_Iterator it = Children.begin(); it != Children.end(); ++it)
	{
		if (*it == actor)
		{
			Children.erase(it);
			break;
		}
	}
}

bool CIwGameUIWrapPanel::UpdateLayout()
{
	int left = -Size.x / 2 + ClippingMargin.x;
	int right = Size.x / 2 - ClippingMargin.y;
	int top = -Size.y / 2 + ClippingMargin.w;
	int bottom = Size.y / 2 - ClippingMargin.h;

	ScrollRange.w = 0;
	ScrollRange.h = 0;

	// Stack all actors
	_Iterator _begin = begin();
	_Iterator _end = end();
	if (Children.size() != 0)
	{
		_Iterator it = _begin;
		CIwRect margin = (*it)->getMargin();
		CIwVec2 pos;
		
		// Calculate initial start position of content
		pos = CIwVec2(-Size.x / 2 - (int)ScrollPos.x, -Size.y / 2 - (int)ScrollPos.y);
		pos.x += (*it)->getSize().x / 2;
		pos.y += (*it)->getSize().y / 2;

		// Arrange the content
		CIwVec2 opos = pos;
		_Iterator it2 = _begin;
		if (it2 != _end)
			it2++;

		int max_height = 0, max_width = 0;
		int max_x = left, max_y = top;

		for (it = _begin; it != _end; ++it)
		{
			CIwGameActor* actor = *it;
			CIwVec2 size = actor->getSize();
			CIwRect margin = actor->getMargin();
			if (Orientation == Orientation_Horizontal)
			{
				pos.x += margin.x;
				if ((pos.x + (size.x / 2)) > right)
				{
					pos.x = opos.x + margin.x;
					pos.y += (size.y / 2) + max_height;
					max_height = 0;
				}
				int height = size.y / 2 + margin.w + margin.h;
				if (height > max_height)
					max_height = height;
				actor->setPosition((float)pos.x, (float)(pos.y + margin.w));
				int dy = (size.y / 2) + margin.w;
				if ((pos.y + dy) < top || (pos.y - (size.y / 2) + margin.w) > bottom)
					actor->setVisible(false);
				else
					actor->setVisible(true);
				int y = pos.y + dy + margin.h;
				if (y > max_y)
					max_y = y;
				int d = size.x / 2;
				if (it2 != _end)
				{
					d += (*it2)->getSize().x / 2;
					it2++;
				}
				pos.x += d + margin.y;
			}
			else
			{
				pos.y += margin.w;
				if ((pos.y + (size.y / 2)) > bottom)
				{
					pos.y = opos.y + margin.w;
					pos.x += (size.x / 2) + max_width;
					max_width = 0;
				}
				int width = size.x / 2 + margin.x + margin.y;
				if (width > max_width)
					max_width = width;
				actor->setPosition((float)(pos.x + margin.x), (float)pos.y);
				int dx = (size.x / 2) + margin.x;
				if ((pos.x + dx) < left || (pos.x - (size.x / 2) + margin.x) > right)
					actor->setVisible(false);
				else
					actor->setVisible(true);
				int x = pos.x + dx + margin.y;
				if (x > max_x)
					max_x = x;
				int d = size.y / 2;
				if (it2 != _end)
				{
					d += (*it2)->getSize().y / 2;
					it2++;
				}
				pos.y += d + margin.h;
			}
		}
		if (Orientation == Orientation_Horizontal)
		{
			// Calculate vertical scroll range 
			ScrollRange.h = (max_y + (int)ScrollPos.y) - top - Size.y;
			if (ScrollRange.h < 0)
				ScrollRange.h = 0;
		}
		else
		{
			// Calculate horizontal scroll range 
			ScrollRange.w = (max_x + (int)ScrollPos.x) - left - Size.x;
			if (ScrollRange.w < 0)
				ScrollRange.w = 0;
		}
	}

	return CIwGameUIBase::UpdateLayout();
}

bool CIwGameUIWrapPanel::Update(float dt)
{
	if (!CIwGameUIBase::Update(dt))
		return false;

	// Set clip rect for the list box
	CIwRect rc;
	rc.x = (-Size.x >> 1) + ClippingMargin.x;
	rc.y = (-Size.y >> 1) + ClippingMargin.w;
	rc.w = Size.x - (ClippingMargin.x + ClippingMargin.y);
	rc.h = Size.y - (ClippingMargin.w + ClippingMargin.h);
	Visual->setClipRect(rc);

	return true;
}

bool CIwGameUIWrapPanel::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - WrapPanel needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameUIBase::LoadFromXoml(parent, false, node))
		return false;

	// Process StackPanel specific attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Orientation_Hash)
		{
			unsigned int hash = (*it)->GetValue().getHash();
			if (hash == CIwGameString::CalculateHash("vertical"))
				Orientation = Orientation_Vertical;
		}
	}

	// Process inner tags
	if (load_children)
	{
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}

	ResizeToContent();

	return true;
}
