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

#if !defined(_CIW_GAME_UI_PANELS_H_)
#define _CIW_GAME_UI_PANELS_H_

#include "IwGameActor.h"
#include "IwGameScene.h"
#include "IwGameUI.h"

//
//
//
//
// CIwGameUICanvas - A panel that allows placement of any UI element in any place, no specific layout rules are enforced
//
//
//
//
class CIwGameUICanvas : public CIwGameUIBase
{
	// Public access to actor iteration
	typedef CIwList<CIwGameActor*>::iterator _Iterator;
	_Iterator					begin() { return Children.begin(); }
	_Iterator					end() { return Children.end(); }

	// Properties
protected:
public:
	// Properties end
protected:
	CIwList<CIwGameActor*>		Children;				// Collection of actors that are linked to this panel
	void						LinkChanged(CIwGameActor* child, bool remove);
	void						RemoveActor(CIwGameActor* actor);
	bool						UpdateLayout();			// Updates the layout of the child items
public:
	CIwGameUICanvas() : CIwGameUIBase() { setActualClassType("canvas"); }
	virtual ~CIwGameUICanvas();

	bool				Update(float dt);

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  CIwGameUICanvasCreator - Creates an instance of a canvas actor object
//
class CIwGameUICanvasCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameUICanvasCreator()
	{
		setClassName("canvas");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameUICanvas(); }
};

//
//
//
//
// CIwGameUIStackPanel - An image based game actor that acts as a stack pabnel, allowing other actors to be stacked inside it
//
//
//
//
class CIwGameUIStackPanel : public CIwGameUIBase
{
	// Public access to actor iteration
	typedef CIwList<CIwGameActor*>::iterator _Iterator;
	_Iterator					begin() { return Children.begin(); }
	_Iterator					end() { return Children.end(); }

	// Properties
protected:
	eIwGameUI_Orientation		Orientation;			// Stacking orientation
	eIwGameUI_AlignH			AlignH;					// Horizontal alignment of contained actors
	eIwGameUI_AlignV			AlignV;					// Vertical alignment of contained actors
public:
	void						setOrientation(eIwGameUI_Orientation o)	{ Orientation = o; setLayoutDirty(true); }
	eIwGameUI_Orientation		getOrientation() const					{ return Orientation; }
	void						setAlignH(eIwGameUI_AlignH align)		{ AlignH = align; setLayoutDirty(true); }
	eIwGameUI_AlignH			getAlignH() const						{ return AlignH; }
	void						setAlignV(eIwGameUI_AlignV align)		{ AlignV = align; setLayoutDirty(true); }
	eIwGameUI_AlignV			getAlignV() const						{ return AlignV; }
	// Properties end
protected:
	CIwList<CIwGameActor*>		Children;				// Collection of actors that are linked to this panel
	void						LinkChanged(CIwGameActor* child, bool remove);
	void						RemoveActor(CIwGameActor* actor);
	bool						UpdateLayout();			// Updates the layout of the child items
public:
	CIwGameUIStackPanel() : CIwGameUIBase(), Orientation(Orientation_Horizontal), AlignH(AlignH_Centre), AlignV(AlignV_Middle)	{ setActualClassType("stackpanel"); }
	virtual ~CIwGameUIStackPanel();

	bool				Update(float dt);

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	void				ResizeToContent();
};

//
//  CIwGameUIStackPanelCreator - Creates an instance of a stack panel actor object
//
class CIwGameUIStackPanelCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameUIStackPanelCreator()
	{
		setClassName("stackpanel");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameUIStackPanel(); }
};

//
//
//
//
// CIwGameUIWrapPanel - A panel that stacks content horizontally until no space left then moves down to the next line
//
//
//
//
class CIwGameUIWrapPanel : public CIwGameUIBase
{
	// Public access to actor iteration
	typedef CIwList<CIwGameActor*>::iterator _Iterator;
	_Iterator					begin() { return Children.begin(); }
	_Iterator					end() { return Children.end(); }

	// Properties
protected:
	eIwGameUI_Orientation		Orientation;			// Stacking orientation
public:
	void						setOrientation(eIwGameUI_Orientation o)	{ Orientation = o; setLayoutDirty(true); }
	eIwGameUI_Orientation		getOrientation() const					{ return Orientation; }
	// Properties end
protected:
	CIwList<CIwGameActor*>		Children;				// Collection of actors that are linked to this panel
	void						LinkChanged(CIwGameActor* child, bool remove);
	void						RemoveActor(CIwGameActor* actor);
	bool						UpdateLayout();			// Updates the layout of the child items
public:
	CIwGameUIWrapPanel() : CIwGameUIBase(), Orientation(Orientation_Horizontal) 	{ setActualClassType("wrappanel"); }
	virtual ~CIwGameUIWrapPanel();

	bool				Update(float dt);

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  CIwGameUIWrapPanelCreator - Creates an instance of a wrap panel actor object
//
class CIwGameUIWrapPanelCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameUIWrapPanelCreator()
	{
		setClassName("wrappanel");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameUIWrapPanel(); }
};



#endif	// _CIW_GAME_UI_PANELS_H_
