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

#include "IwGameUITabBar.h"


//
//
//
//
// CIwGameUITab implementation
//
//
//
//
void CIwGameUITab::InitTab()
{
}

void CIwGameUITab::PostInitTab(CIwGameUITabBar* tabbar)
{
	TabBar = tabbar;

	if (Selected)
		setToggledOn(true);
}

void CIwGameUITab::OnToggledOn()
{
	CIwGameUIBase::OnToggledOn();

	if (TabBar != NULL)
		TabBar->ShowView(View, true);
}

void CIwGameUITab::OnToggledOff()
{
	CIwGameUIBase::OnToggledOff();

	if (TabBar != NULL)
		TabBar->ShowView(View, false);
}

void CIwGameUITab::OnOrientationChange(eIwGameScene_Orientation old_orientation, eIwGameScene_Orientation new_orientation)
{
	CIwGameUILabel::OnOrientationChange(old_orientation, new_orientation);
}

bool CIwGameUITab::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - Tab needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameUILabel::LoadFromXoml(parent, false, node))
		return false;

	InitTab();

	if (load_children)
	{
		// Process inner tags
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}

	return true;
}



//
//
//
//
// CIwGameUITabs implementation
//
//
//
//
void CIwGameUITabs::InitTabs()
{
}

bool CIwGameUITabs::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - Tabs needs to be declared inside a scene or an actor");
		return false;
	}

	Orientation = Orientation_Horizontal;

	// Load main actor attributes
	if (!CIwGameUIListBox::LoadFromXoml(parent, false, node))
		return false;

	InitTabs();

	if (load_children)
	{
		// Process inner tags
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}

	return true;
}

//
//
//
//
// CIwGameUITabBar implementation
//
//
//
//
void CIwGameUITabBar::LinkChanged(CIwGameActor* child, bool remove)
{
	if (child->getCategory() != AC_UI)
	{
#if defined(_DEBUG)
		CIwGameError::LogError("Error: Only actors that are derived from UI actor classes can be added to tabbar - ", child->getName().c_str());
#endif	// _DEBUG
		return;
	}

	if (remove)
	{
		for (_Iterator it = begin(); it != end(); ++it)
		{
			delete *it;
			Children.erase(it);
			break;
		}
	}
	else
	{
		Children.push_back((CIwGameUIBase*)child);
	}
}

void CIwGameUITabBar::ShowView(CIwGameUIBase* view, bool show)
{
	if (AutoHide || show)
		view->setVisible(show);

	if (show)
	{
		OnViewChanged(PrevView, view);
		PrevView = view;
	}
}

bool CIwGameUITabBar::Update(float dt)
{
	if (!IsVisible)
		return CIwGameUICanvas::Update(dt);

	return CIwGameUICanvas::Update(dt);
}

bool CIwGameUITabBar::UpdatePanels()
{
	// Position tabs and views
	bool top = true;
	_Iterator it = begin();
	if ((*it)->getActualClassTypeHash() != CIwGameXomlNames::Tabs_Hash)
		top = false;

	if (Orientation == Orientation_Horizontal)
	{
		float dy = (float)-Size.y / 2;

		for (; it != end(); ++it)
		{
			CIwFVec2 pos = (*it)->getPosition();
			CIwVec2 size = (*it)->getSize();
			CIwRect margin = (*it)->getMargin();

			float y;
			if ((*it)->getActualClassTypeHash() == CIwGameXomlNames::Tabs_Hash)
			{
				if (top)
					y = dy + size.y / 2 + margin.w;
				else
					y = (float)Size.y / 2 - size.y / 2 - margin.h;

				dy += size.y + margin.w + margin.h;
			}
			else
			{
				if (top)
					y = dy + size.y / 2 + margin.w;
				else
					y = dy + size.y / 2 + margin.w;

			}
			(*it)->setPosition(pos.x, y);
			(*it)->setOriginalPosition(0, y);
		}
	}
	else
	{
		float dx = (float)-Size.x / 2;

		for (; it != end(); ++it)
		{
			CIwFVec2 pos = (*it)->getPosition();
			CIwVec2 size = (*it)->getSize();
			CIwRect margin = (*it)->getMargin();

			float x;
			if ((*it)->getActualClassTypeHash() == CIwGameXomlNames::Tabs_Hash)
			{
				if (top)
					x = dx + size.x / 2 + margin.x;
				else
					x = (float)Size.x / 2 - size.x / 2 - margin.y;

				dx += size.x + margin.x + margin.y;
			}
			else
			{
				if (top)
					x = dx + size.x / 2 + margin.x;
				else
					x = dx + size.x / 2 + margin.x;

			}
			(*it)->setPosition(x, pos.y);
			(*it)->setOriginalPosition(x, 0);
		}
	}

	return true;
}

bool CIwGameUITabBar::UpdateLayout()
{
	UpdatePanels();

	return CIwGameUIBase::UpdateLayout();
}

void CIwGameUITabBar::InitTabBar()
{
	// Find the tabs actor
	CIwGameUITabs* tabs = NULL;
	for (_Iterator it = begin(); it != end(); ++it)
	{
		if ((*it)->getActualClassTypeHash() == CIwGameXomlNames::Tabs_Hash)
		{
			tabs = (CIwGameUITabs*)*it;
			break;
		}
	}

	if (tabs != NULL)
	{
		// Get orientation
		Orientation = tabs->getOrientation();

		// Hook views up to tabs
		CIwGameUIListBox::_Iterator it2 = tabs->begin();
		for (_Iterator it = begin(); it != end(); ++it)
		{
			if ((*it)->getActualClassTypeHash() != CIwGameXomlNames::Tabs_Hash)	// Ignore the tabs container
			{
				(*it)->setVisible(false, true);
				CIwGameUITab* tab = (CIwGameUITab*)(*it2)->Actor;
				tab->setView(*it);
				tab->PostInitTab(this);

				if (!tab->isSelected())
				{
					CIwGameAnimTimeline* timeline = (*it)->getShowTimeline();
					if (timeline != NULL)
						timeline->ForceUpdateTargetOnly();
				}

				it2++;
				if (it2 == tabs->end())	// Ran out of tabs
					break;
			}
		}
	}

	UpdatePanels();
}

bool CIwGameUITabBar::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - TabBar needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameUICanvas::LoadFromXoml(parent, false, node))
		return false;

	// Process attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();
		if (name_hash == CIwGameXomlNames::OnViewChanged_Hash)
			EventsManager->addEvent("OnViewChanged", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::AutoHide_Hash)
			AutoHide = (*it)->GetValueAsBool();
	}

	if (load_children)
	{
		// Process inner tags
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}

	InitTabBar();

	return true;
}

bool CIwGameUITabBar::setProperty(unsigned int element_name, CIwGameString& data, bool delta)
{
	if (CIwGameUICanvas::setProperty(element_name, data, delta))
		return true;

/*	if (element_name == CIwGameXomlNames::View_Hash)
	{
	}
	else
		return false;

	return true;*/

	return false;
}

bool CIwGameUITabBar::UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var)
{
	if (!var->Modified || var->getValue().IsEmpty())
		return false;
	if (CIwGameUICanvas::UpdateBinding(element_name, var))
		return true;

//	return true;
	return false;
}


void CIwGameUITabBar::OnViewChanged(CIwGameUIBase* prev_view, CIwGameUIBase* new_view)
{
	ProcessEventActions(CIwGameXomlNames::OnViewChanged_Hash);
}


