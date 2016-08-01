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

#if !defined(_CIW_GAME_UI_IMAGE_VIEW_H_)
#define _CIW_GAME_UI_IMAGE_VIEW_H_

#include "IwGameUI.h"
#include "IwGameScene.h"
#include "IwGameBrush.h"
#include "IwGameInput.h"

//
//
//
//
// CIwGameUIImageView - UI class that allows the user to navugate a large image
//
//
//
//
class CIwGameUIImageView : public CIwGameUIBase
{
public:
	// Properties
protected:
	CIwGameUIIcon*		Icon;
	CIwVec2				Area;
	float				Zoom;
	float				MinZoom, MaxZoom;
public:
	CIwGameUIIcon*		getIcon()						{ return Icon; }
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
	CIwGameUIImageView();

	virtual bool		InitView(IIwGameBrush* brush, int width, int height, bool native_res, float min_scale, float max_scale);

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
//  CIwGameUIImageViewCreator - Creates an instance of a ImageView actor object
//
class CIwGameUIImageViewCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameUIImageViewCreator()
	{
		setClassName("imageview");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameUIImageView(); }
};

#endif	// _CIW_GAME_UI_IMAGE_VIEW_H_
