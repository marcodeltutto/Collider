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

#if !defined(_CIW_GAME_UI_TABBAR_H_)
#define _CIW_GAME_UI_TABBAR_H_

#include "IwGameUI.h"
#include "IwGameScene.h"
#include "IwGameBrush.h"
#include "IwGameInput.h"
#include "IwGameUIListBox.h"
#include "IwGameUIPanels.h"

class CIwGameUITabBar;

//
//
//
//
// CIwGameUITab - A UI element that represents a tab
//
//
//
//
class CIwGameUITab : public CIwGameUILabel
{
	// Properties
protected:
	CIwGameUIBase*		View;
	CIwGameUITabBar*	TabBar;
public:
	void				setView(CIwGameUIBase* view)	{ View = view; }
	CIwGameUIBase*		getView()						{ return View; }
	// Properties end
protected:
public:
	CIwGameUITab() : CIwGameUILabel(), View(NULL), TabBar(NULL)
	{
		setActualClassType("tab");
	}

	void				InitTab();
	void				PostInitTab(CIwGameUITabBar* tabbar);			// Called after views have been created

	// Event handlers
	void				OnToggledOn();
	void				OnToggledOff();
	void				OnOrientationChange(eIwGameScene_Orientation old_orientation, eIwGameScene_Orientation new_orientation);

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  CIwGameUITabCreator - Creates an instance of a tab actor object
//
class CIwGameUITabCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameUITabCreator()
	{
		setClassName("tab");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameUITab(); }
};

//
//
//
//
// CIwGameUITabs - A UI element that represents a collection of tabs
//
//
//
//
class CIwGameUITabs : public CIwGameUIListBox
{
	// Properties
protected:
public:
	// Properties end
protected:
public:
	CIwGameUITabs() : CIwGameUIListBox()
	{
		setActualClassType("tabs");
	}

	void				InitTabs();
	void				PostInitTabs();		// Called after views have been created

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  CIwGameUITabsCreator - Creates an instance of a tabs actor object
//
class CIwGameUITabsCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameUITabsCreator()
	{
		setClassName("tabs");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameUITabs(); }
};



//
//
//
//
// CIwGameUITabBar - UI class that allows the user to select between different groups of controls using tabs
//
//
//
//
class CIwGameUITabBar : public CIwGameUICanvas
{
public:
	// Public access to items iteration
	typedef CIwList<CIwGameUIBase*>::iterator _Iterator;
	_Iterator					begin() { return Children.begin(); }
	_Iterator					end() { return Children.end(); }

	// Properties
protected:
	eIwGameUI_Orientation		Orientation;			// Stacking orientation (set from the tabs list box)
	bool						AutoHide;
public:
	void						setAutoHide(bool enable)		{ AutoHide = enable; }
	bool						getAutoHide() const				{ return AutoHide; }
	eIwGameUI_Orientation		getOrientation() const			{ return Orientation; }
	bool						setProperty(unsigned int element_name, CIwGameString& data, bool delta);
	bool						UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var);
	// Properties end
protected:
	bool						UpdateLayout();
	bool						UpdatePanels();
	CIwGameUIBase*				PrevView;
	CIwList<CIwGameUIBase*>		Children;			// Collection of views
	void						LinkChanged(CIwGameActor* child, bool remove);

public:
	CIwGameUITabBar() : CIwGameUICanvas(), PrevView(NULL), AutoHide(true), Orientation(Orientation_Vertical)	{ setActualClassType("tabbar"); }

	void				InitTabBar();
	bool				Update(float dt);

	// Event handlers
	void				OnViewChanged(CIwGameUIBase* prev_view, CIwGameUIBase* new_view);

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	void				ShowView(CIwGameUIBase* view, bool show);
};

//
//  CIwGameUITabBarCreator - Creates an instance of a tabbar actor object
//
class CIwGameUITabBarCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameUITabBarCreator()
	{
		setClassName("tabbar");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameUITabBar(); }
};


#endif	// #define _CIW_GAME_UI_TABBAR_H_

