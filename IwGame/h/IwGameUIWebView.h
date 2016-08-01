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

#if !defined(_CIW_GAME_UI_WEB_VIEW_H_)
#define _CIW_GAME_UI_WEB_VIEW_H_

#include "IwGameActor.h"
#include "IwGameScene.h"
#include "IwGameBrush.h"
#include "IwGameInput.h"
#include "IwGameUI.h"

#include "s3eWebView.h"


//
//
//
//
// CIwGameUIWebView - A wrapper for the Marmalade WebView class providing access to web view display
//
//
//
//
class CIwGameUIWebView : public CIwGameUIBase
{
public:
	// Properties
protected:
	CIwGameString		URI;				// Html to display
	s3eWebView*			WebView;			// The created web view object
	bool				Modal;				// If true then takes over full screen and does not go back to app until dismissed

public:
	void				setURI(const char* uri)		{ URI = uri; }
	bool				setProperty(unsigned int element_name, CIwGameString& data, bool delta);
	bool				getProperty(unsigned int element_name, CIwGameXomlProperty& prop);
	void				setHtml(const char* html);
	// Properties end
protected:
	CIwGameString		TempFilename;
	bool				PrevVisible;
	CIwFVec2			OldPosition;
	CIwFVec2			OldSize;
	bool				UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var);

public:
	CIwGameUIWebView() : CIwGameUIBase(), WebView(NULL), Modal(false) {}
	~CIwGameUIWebView();

	bool				InitView(bool modal, bool transparent, const char* java_script);
	bool				UpdateVisual();
	void				UpdatePosSize();

	// Event handlers

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	void				OnOrientationChange(eIwGameScene_Orientation old_orientation, eIwGameScene_Orientation new_orientation);

};

//
//  CIwGameUIWebViewCreator - Creates an instance of a web view actor object
//
class CIwGameUIWebViewCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameUIWebViewCreator()
	{
		setClassName("webview");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameUIWebView(); }
};


#endif	// #define _CIW_GAME_UI_WEB_VIEW_H_

