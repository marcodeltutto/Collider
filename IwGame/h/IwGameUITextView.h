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

#if !defined(_CIW_GAME_UI_TEXT_VIEW_H_)
#define _CIW_GAME_UI_TEXT_VIEW_H_

#include "IwGameUI.h"
#include "IwGameScene.h"
#include "IwGameBrush.h"
#include "IwGameInput.h"

//
//
//
//
// CIwGameUITextView - UI class that allows the user to view and edit multi-line scrolling text
//
//
//
//
class CIwGameUITextView : public CIwGameUILabel
{
public:
	// Properties
protected:
	CIwVec2				Area;
	float				Zoom;
	float				MinZoom, MaxZoom;
public:
	void				setArea(CIwVec2& size);
	CIwVec2				getArea() const					{ return Area; }
	void				setZoom(float scale);
	float				getZoom() const					{ return Zoom; }
	void				setMinZoom(float scale)			{ MinZoom = scale; }
	float				getMinZoom() const				{ return MinZoom; }
	void				setMaxZoom(float scale)			{ MaxZoom = scale; }
	float				getMaxZoom() const				{ return MaxZoom; }

	// Properties end
protected:
	float				PrevZoom;
	CIwFVec2			Midpoint;
	bool				ZoomValid;
	float				PreviousDist;
	float				CompoundScale;
	CIwFVec2			CompoundPosition;
	float				SavedCompoundScale;
	CIwFVec2			SavedCompoundPosition;
	CIwFVec2			PanVelocity;
	int					TouchIndices[2];
	int					NumTouches;
	bool				setProperty(unsigned int element_name, CIwGameString& data, bool delta);
	bool				getProperty(unsigned int element_name, CIwGameXomlProperty& prop);
	bool				UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var);
public:
	CIwGameUITextView();

	virtual bool		InitView(bool native_res, float min_scale, float max_scale);

	bool				Update(float dt);
	bool				UpdateVisual();

	// Event handlers
	void				OnBeginTouch(int index, int x, int y, bool allow_bubble);
	void				OnEndTouch(int index, int x, int y, bool allow_bubble);

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	// Implementation of IIwGameAnimTarget interface
//	bool				UpdateFromAnimation(CIwGameAnimInstance *animation);
};

//
//  CIwGameUITextViewCreator - Creates an instance of a TextView actor object
//
class CIwGameUITextViewCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameUITextViewCreator()
	{
		setClassName("textview");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameUITextView(); }
};


#endif	// #define _CIW_GAME_UI_TEXT_VIEW_H_

