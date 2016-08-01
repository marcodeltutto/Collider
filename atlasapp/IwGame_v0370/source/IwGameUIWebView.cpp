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

#include "IwGameUIWebView.h"

//
//
//
//
// CIwGameActor implementation
//
//
//
//

bool CIwGameUIWebView::InitView(bool modal, bool transparent, const char* java_script)
{
	PrevVisible = false;
	Modal = modal;
	if (s3eWebViewAvailable())
	{
		if (Modal)
			s3eWebViewCreateModal(URI.c_str());
		else
		{
			WebView = s3eWebViewCreate(transparent);
			s3eWebViewNavigate(WebView, URI.c_str());
			if (java_script != NULL)
				s3eWebViewSendJavaScript(WebView, java_script);
		}
	}
	else
		return false;

	OldPosition.x = 0;
	OldPosition.y = 0;
	PrevVisible = !IsVisible;

	return true;
}

bool CIwGameUIWebView::UpdateVisual()
{
	if (WebView == NULL)
		return true;

	if (!CIwGameUIBase::UpdateVisual())
		return false;

	CIwMat2D transform = Visual->getFinalTransform();
	int sw = (transform.m[0][0] * Size.x) / IW_GEOM_ONE;
	int sh = (transform.m[1][1] * Size.y) / IW_GEOM_ONE;
	int x = (transform.t.x >> 3) - (sw >> 1);
	int y = (transform.t.y >> 3) - (sh >> 1);

	if (IsVisible && !PrevVisible)
		s3eWebViewShow(WebView, x, y, sw, sh);
	else
	if (!IsVisible && PrevVisible)
		s3eWebViewHide(WebView);

	if (IsVisible)
	{
		if (OldPosition.x != x || OldPosition.y != y)
			s3eWebViewShow(WebView, x, y, sw, sh);
	}

	PrevVisible = IsVisible;
	OldPosition.x = x;
	OldPosition.y = y;

	return true;
}

bool CIwGameUIWebView::setProperty(unsigned int element_name, CIwGameString& data, bool delta)
{
	if (WebView == NULL)
		return false;

	if (CIwGameUIBase::setProperty(element_name, data, delta))
		return true;

	if (element_name == CIwGameXomlNames::URI_Hash)
		s3eWebViewNavigate(WebView, data.c_str());
	else
	if (element_name == CIwGameXomlNames::JavaScript_Hash)
		s3eWebViewSendJavaScript(WebView, data.c_str());
	else
		return false;


	return true;
}


bool CIwGameUIWebView::UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var)
{
	if (WebView == NULL)
		return false;

	if (!var->Modified || var->getValue().IsEmpty())
		return false;
	if (CIwGameActor::UpdateBinding(element_name, var))
		return true;

	if (element_name == CIwGameXomlNames::URI_Hash)
	{
		if (var->Type == VT_String)
		{
			if (!var->getValue().IsEmpty())
				s3eWebViewNavigate(WebView, var->getValue().c_str());
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected string for URI");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::JavaScript_Hash)
	{
		if (var->Type == VT_String)
		{
			if (!var->getValue().IsEmpty())
				s3eWebViewSendJavaScript(WebView, var->getValue().c_str());
		}
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Incorrect binding variable type, expected string for JavaScript");
#endif // _DEBUG
	}

	return true;
}

bool CIwGameUIWebView::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (!s3eWebViewAvailable())
	{
		CIwGameError::LogError("Error: XOML - WebView not supported on this platform");
		return false;
	}

	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - WebView needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameUIBase::LoadFromXoml(parent, false, node))
		return false;

	// Process WebView specific attributes
	bool modal = false;
	bool transparent = false;
	const char* java_script = NULL;

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();
		if (name_hash == CIwGameXomlNames::URI_Hash)
			URI = (*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Modal_Hash)
			modal = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::JavaScript_Hash)
			java_script = (*it)->GetValue().c_str();
		else
		if (name_hash == CIwGameXomlNames::Transparent_Hash)
			transparent = (*it)->GetValueAsBool();
	}

	InitView(modal, transparent, java_script);

	// Process inner tags
	if (load_children)
	{
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}

	return true;
}