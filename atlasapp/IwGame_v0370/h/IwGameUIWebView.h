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
	CIwGameString		URI;				// Default URI
	s3eWebView*			WebView;			// The created web view object
	bool				Modal;				// If true then takes over full screen and does not go back to app until dismissed

public:
	void				setURI(const char* uri);
	bool				setProperty(unsigned int element_name, CIwGameString& data, bool delta);
	// Properties end
protected:
	bool				PrevVisible;
	CIwVec2				OldPosition;
	bool				UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var);

public:
	CIwGameUIWebView() : CIwGameUIBase(), WebView(NULL) {}
	~CIwGameUIWebView()  { if (WebView != NULL) s3eWebViewDestroy(WebView); }

	bool				InitView(bool modal, bool transparent, const char* java_script);
	bool				UpdateVisual();

	// Event handlers

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
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

