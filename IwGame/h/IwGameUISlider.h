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

#if !defined(_CIW_GAME_UI_SLIDER_H_)
#define _CIW_GAME_UI_SLIDER_H_

#include "IwGameUI.h"
#include "IwGameScene.h"
#include "IwGameBrush.h"
#include "IwGameInput.h"

//
//
//
//
// CIwGameUISlider - UI class that allows the user to select a value using a sliding scale (horizontal, vertical and round)
//
//
//
//


enum eIwGameUISlider_Type
{
	SliderType_Horizontal, 
	SliderType_Vertical, 
};

class CIwGameUISlider : public CIwGameUIBase
{
public:
	// Properties
protected:
	eIwGameUISlider_Type	SliderType;				// Type of slider
	float					Value;					// Position of thumb within the range
	CIwFVec2				ValueRange;				// The tange of the thumb
	int						OriginalSliderSize;		// Original size of slider
	int						SliderSize;				// Size of slider
public:
	void					setValue(float pos);
	float					getValue() const				{ return Value; }
	void					setValueRange(CIwFVec2& range);
	CIwFVec2				getValueRange() const			{ return ValueRange; }
	void					setSliderType(eIwGameUISlider_Type type) { SliderType = type; }
	eIwGameUISlider_Type	getSliderType() const			{ return SliderType; }
	void					setSliderSize(int size)			{ SliderSize = size; }
	int						getSliderSize() const			{ return SliderSize; }

	bool					setProperty(unsigned int element_name, CIwGameString& data, bool delta);
	bool					getProperty(unsigned int element_name, CIwGameXomlProperty& prop);
	// Properties end
protected:
	bool					ThumbChanged;			// True when slidee value changed
	bool					UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var);
	void					UpdateSliderSize();
		
public:

	CIwGameUISlider() : CIwGameUIBase(), Value(0), ValueRange(0, 1.0f), SliderType(SliderType_Horizontal), SliderSize(0), ThumbChanged(false) { IsDraggable = true; HoldFocus = true; }

	bool				Update(float dt);

	// Event handlers
	void				OnOrientationChange(eIwGameScene_Orientation old_orientation, eIwGameScene_Orientation new_orientation);
	void				OnBeginTouch(int index, int x, int y, bool allow_bubble = true);
	void				OnEndTouch(int index, int x, int y, bool allow_bubble = true);
	virtual void		OnValueChanged(float old_value, float new_value);

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  CIwGameUISliderCreator - Creates an instance of a Slider UI actor object
//
class CIwGameUISliderCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameUISliderCreator()
	{
		setClassName("slider");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameUISlider(); }
};

#endif	// #define _CIW_GAME_UI_SLIDER_H_

