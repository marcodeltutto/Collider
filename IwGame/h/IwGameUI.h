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

#if !defined(_CIW_GAME_UI_H_)
#define _CIW_GAME_UI_H_

#include "IwGameActor.h"
#include "IwGameScene.h"
#include "IwGameBrush.h"
#include "IwGameInput.h"
#include "IwGameAnim.h"
#include "IwGameVideo.h"

enum eIwGameUI_Orientation
{
	Orientation_Horizontal, 
	Orientation_Vertical, 
};

enum eIwGameUI_AlignH
{
	AlignH_Left, 
	AlignH_Centre, 
	AlignH_Right, 
};

enum eIwGameUI_AlignV
{
	AlignV_Top, 
	AlignV_Middle, 
	AlignV_Bottom, 
};

//
//
//
//
// CIwGameUIBase - Base class of all UI components
//
//
//
//
class CIwGameUIBase : public CIwGameActorImage
{
public:
	enum eIwGameUI_SelectType
	{
		SelectType_Normal, 
		SelectType_Toggle, 
		SelectType_Sticky, 
	};
	enum eIwGameUI_Axis
	{
		Axis_None, 
		Axis_X, 
		Axis_Y, 
		Axis_XY, 
	};
	// Properties
protected:
	bool				PrevSelected;						// Previous Selected status
	bool				ToggledOn;							// ToggleOn state
	bool				Selected;							// Selected status
	IIwGameBrush*		BackgroundBrush;					// Default none selected brush
	CIwColour			BackgroundColour;					// Default none selected colour
	IIwGameBrush*		SelectedBackgroundBrush;			// Selected brush
	CIwColour			SelectedBackgroundColour;			// Selected colour
	IIwGameBrush*		DisabledBackgroundBrush;			// Disabled brush
	CIwColour			DisabledBackgroundColour;			// Disabled colour
	bool				Enabled;							// Enabled state
	bool				LayoutDirty;						// When true the layout of this elemeny and any children is re-calculated
	eIwGameUI_SelectType SelectType;						// Selection method used for this UI element
	CIwRect				OriginalClippingMargin;				// Original margin that is used to adjust the clipping rectangle
	CIwRect				ClippingMargin;						// Margin that is used to adjust the clipping rectangle
	CIwFVec2			PreviousScrollPos;					// Previous scroll position
	CIwFVec2			ScrollPos;							// Current scroll position
	CIwFVec2			ScrollVelocity;						// Current scroll position
	CIwRect				ScrollRange;						// X and Y Ranges of scroll
	bool				EnableSpring;						// Enables drag release spring
	eIwGameUI_Axis		SizeToContent;						// Determines if and how an elemen should size to its content
	CIwGameAnimTimeline* ShowTimeline;						// Timeline to play when this UI element is made visible
	CIwGameAnimTimeline* HideTimeline;						// Timeline to play when this UI element is made invisible
	bool				NeedsResize;						// Set to true when child actors sizes have changed
	CIwRect				ColourOffset;						// Amount to offset the current colou of the cnotrol
public:
	void				setVisible(bool visible, bool force_set = false);
	void				setBackgroundBrush(IIwGameBrush* brush);
	IIwGameBrush*		getBackgroundBrush()							{ return BackgroundBrush; }
	void				setBackgroundColour(CIwColour& colour);
	CIwColour			getBackgroundColour() const						{ return BackgroundColour; }
	void				setSelectedBackgroundBrush(IIwGameBrush* brush);
	IIwGameBrush*		getSelectedBackgroundBrush()					{ return SelectedBackgroundBrush; }
	void				setSelectedBackgroundColour(CIwColour& colour);
	CIwColour			getSelectedBackgroundColour() const				{ return SelectedBackgroundColour; }
	void				setDisabledBackgroundBrush(IIwGameBrush* brush);
	IIwGameBrush*		getDisabledBackgroundBrush()					{ return DisabledBackgroundBrush; }
	void				setDisabledBackgroundColour(CIwColour& colour);
	CIwColour			getDisabledBackgroundColour() const				{ return DisabledBackgroundColour; }
	void				setSelectType(eIwGameUI_SelectType type)		{ SelectType = type; }
	eIwGameUI_SelectType getSelectType() const							{ return SelectType; }
	void				setSelected(bool selected, bool force_select = false);
	bool				isSelected() const								{ return Selected; }
	virtual void 		setEnabled(bool enabled);
	bool				isEnabled() const								{ return Enabled; }
	bool				setProperty(unsigned int element_name, CIwGameString& data, bool delta);
	bool				getProperty(unsigned int element_name, CIwGameXomlProperty& prop);
	void				setLayoutDirty(bool dirty)						{ LayoutDirty = dirty; }
	bool				isLayoutDirty() const							{ return LayoutDirty; }
	void				setClippingMargin(int x, int y, int w, int h)	{ ClippingMargin.x = x; ClippingMargin.y = y; ClippingMargin.w = w; ClippingMargin.h = h; }
	CIwRect				getClippingMargin()								{ return ClippingMargin; }
	void				setScrollPos(float x, float y);
	CIwFVec2			getScrollPos() const							{ return ScrollPos; }
	CIwFVec2			getPreviousScrollPos() const					{ return PreviousScrollPos; }
	void				setScrollVelocity(float x, float y)				{ ScrollVelocity.x = x; ScrollVelocity.y = y; }
	CIwFVec2			getScrollVelocity() const						{ return ScrollVelocity; }
	void				setScrollRange(int x, int y, int w, int h)		{ ScrollRange.x = x; ScrollRange.y = y; ScrollRange.w = w; ScrollRange.h = h; } 
	CIwRect				getScrollRange() const							{ return ScrollRange; }
	void				setToggledOn(bool toggled_on);
	bool				isToggledOn() const								{ return ToggledOn; }
	void				setShowTimeline(CIwGameAnimTimeline* timeline);
	CIwGameAnimTimeline* getShowTimeline()								{ return ShowTimeline; }
	void				setHideTimeline(CIwGameAnimTimeline* timeline);
	CIwGameAnimTimeline* getHideTimeline()								{ return HideTimeline; }
	void				setNeedsResize(bool resize)						{ NeedsResize = resize; }
	bool				getNeedsResize() const							{ return NeedsResize; }
	eIwGameUI_Axis		getSizeToContent() const						{ return SizeToContent; }
	void				NotifyParentsResize();
	void				setColourOffset(CIwRect& offset)				{ ColourOffset = offset; setEnabled(Enabled); }
	void				resetColourOffset()								{ ColourOffset.x = 0; ColourOffset.y = 0; ColourOffset.w = 0; ColourOffset.h = 0; setEnabled(Enabled); }
	CIwRect				getColourOffset() const							{ return ColourOffset; }
	void				setShowTimeline(const char* timeline_name);
	void				setHideTimeline(const char* timeline_name);
	// Properties end
protected:
	bool				Tapped;
	CIwFVec2			InitialScrollPos;
	bool				UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var);
	virtual bool		UpdateLayout();									// Updates the layout of the child items
	bool				ScrollRangeCheck();

public:
	CIwGameUIBase();
	~CIwGameUIBase();

	bool				Update(float dt);
	virtual void		ResizeToContent();

	// Event handlers
	void				OnTapped(int index, int x, int y, bool allow_bubble);
	void				OnBeginTouch(int index, int x, int y, bool allow_bubble);
	void				OnEndTouch(int index, int x, int y, bool allow_bubble);
	virtual void		OnToggledOn();
	virtual void		OnToggledOff();
	virtual void		OnBackKey();
	virtual void		OnMenuKey();
	void				OnOrientationChange(eIwGameScene_Orientation old_orientation, eIwGameScene_Orientation new_orientation);

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	// Implementation of IIwGameAnimTarget interface
	bool				UpdateFromAnimation(CIwGameAnimInstance *animation);

	// Preset UI animations
public:
	static CIwList<CIwGameAnim*>	PresetAnimations;
	static void						InitPresetAnimations();
	static void						ReleasePresetAnimations();
	static void						UpdatePresetAnimations();
	static CIwGameAnim*				FindPresetAnimation(unsigned int name_hash);

};


//
//
//
//
// CIwGameUILabel - A text based actor that acts as a label
//
//
//
//
class CIwGameUILabel : public CIwGameUIBase
{
	// Properties
protected:
	CIwGameActorText*	TextActor;
	CIwColour			TextColour;
	CIwGameString		Text;
	CIwColour			SelectedTextColour;
	CIwGameString		SelectedText;
	CIwColour			DisabledTextColour;
	CIwGameString		DisabledText;
public:
	CIwGameActorText*	getTextActor()									{ return TextActor; }
	void				setTextColour(CIwColour& colour);
	CIwColour			getTextColour()	const							{ return TextColour; }
	void				setText(const char* text);
	CIwGameString&		getText()										{ return Text; }
	void				setSelectedTextColour(CIwColour& colour);
	CIwColour			getSelectedTextColour() const					{ return SelectedTextColour; }
	void				setSelectedText(const char* text);
	CIwGameString&		getSelectedText()								{ return SelectedText; }
	void				setDisabledTextColour(CIwColour& colour);
	CIwColour			getDisabledTextColour() const					{ return DisabledTextColour; }
	void				setDisabledText(const char* text);
	CIwGameString&		getDisabledText()								{ return DisabledText; }
	void				setEnabled(bool enabled);
	bool				setProperty(unsigned int element_name, CIwGameString& data, bool delta);
	bool				getProperty(unsigned int element_name, CIwGameXomlProperty& prop);
	// Properties end
protected:
	void				UpdateSizeFromText();
	bool				UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var);
public:
	CIwGameUILabel();

	virtual bool		InitTextActor(CIwGameFont* font, CIwRect& rect, CIwGameString& text, int flags, IwGxFontAlignHor align_h, IwGxFontAlignVer align_v, CIwFVec4& skew, CIwRect& text_margin, bool auto_height, bool text_use_parent_opacity = false);

	bool				Update(float dt);

	// Event handlers

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	// Implementation of IIwGameAnimTarget interface
	bool				UpdateFromAnimation(CIwGameAnimInstance *animation);
};

//
//  CIwGameUILabelCreator - Creates an instance of a label actor object
//
class CIwGameUILabelCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameUILabelCreator()
	{
		setClassName("label");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameUILabel(); }
};

//
//
//
//
// CIwGameUIIcon- An image based actor that acts as an icon
//
//
//
//
class CIwGameUIIcon : public CIwGameUIBase
{
	// Properties
protected:
public:
	// Properties end
protected:
public:
	CIwGameUIIcon() : CIwGameUIBase()	{ setActualClassType("icon"); }

//	bool				Update(float dt);
	// Implementation of IIwGameXomlClass interface
//	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  CIwGameUIIconCreator - Creates an instance of an icon actor object
//
class CIwGameUIIconCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameUIIconCreator()
	{
		setClassName("icon");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameUIIcon(); }
};

//
//
//
//
// CIwGameUIVideoOverlay- A video overlay based UI actor
//
//
//
//
class CIwGameUIVideoOverlay : public CIwGameUIBase
{
	// Properties
protected:
	bool				AutoPlay;
	CIwGameVideo*		Video;
	bool				Playing;
	bool				Started;
public:
	void				setAutoPlay(bool auto_play)		{ AutoPlay = auto_play; }
	bool				isAutoPlay() const				{ return AutoPlay; }
	void				setVideo(CIwGameVideo* video)	{ Video = video; }
	CIwGameVideo*		getVideo()						{ return Video; }
	bool				isPlaying() const;
	bool				setProperty(unsigned int element_name, CIwGameString& data, bool delta);
	bool				getProperty(unsigned int element_name, CIwGameXomlProperty& prop);
	// Properties end
protected:
public:
	CIwGameUIVideoOverlay() : CIwGameUIBase(), AutoPlay(false), Video(NULL), Playing(false), Started(false)	{ setActualClassType("videooverlay"); }

	bool				Update(float dt);

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	bool				Play();
	void				Stop();
	void				Pause();
	void				Resume();
};

//
//  CIwGameUIVideoOverlayCreator - Creates an instance of an video overlay actor object
//
class CIwGameUIVideoOverlayCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameUIVideoOverlayCreator()
	{
		setClassName("videooverlay");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameUIVideoOverlay(); }
};

//
//
//
//
// CIwGameUITextBox - An image based actor that acts as a text box
//
//
//
//
class CIwGameUITextBox : public CIwGameUILabel
{
public:

	// Properties
protected:
	CIwGameString		Prompt;								// Prompt to show to the user when asked for text input
	CIwGameString		TargetVariable;						// Name of thevarianle that will be updated with the value entered
	eIwGameInputTypeHint TypeHint;							// Hint to which type of data user should enter
public:
	void				setPrompt(const char* prompt)					{ Prompt = prompt; }
	CIwGameString&		getPrompt()										{ return Prompt; }
	void				setTargetVariable(const char* name)				{ TargetVariable = name; }
	CIwGameString&		getTargetVariable()								{ return TargetVariable; }
	void				setTypeHint(eIwGameInputTypeHint hint)			{ TypeHint = hint; }
	eIwGameInputTypeHint getTypeHint() const							{ return TypeHint; }
	bool				setProperty(unsigned int element_name, CIwGameString& data, bool delta);
	// Properties end
protected:
	bool				UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var);
public:
	CIwGameUITextBox();

	// Event handlers
	virtual void		OnTapped(int index, int x, int y, bool allow_bubble);
	virtual void		OnTextChanged();

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  CIwGameUITextBoxCreator - Creates an instance of text box actor object
//
class CIwGameUITextBoxCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameUITextBoxCreator()
	{
		setClassName("textbox");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameUITextBox(); }
};







#endif	// _CIW_GAME_UI_H_
