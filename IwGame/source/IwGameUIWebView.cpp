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
CIwGameUIWebView::~CIwGameUIWebView()
{
	// Delete temp html file
	if (!TempFilename.IsEmpty())
	{
		CIwGameFile::DeleteFile(TempFilename.c_str());
	}

	if (WebView != NULL)
		s3eWebViewDestroy(WebView);
}

void CIwGameUIWebView::setHtml(const char* html)
{
	CIwGameString h = html;
	h.URLDecode();

	TempFilename.allocString(1024);
	TempFilename = "ram://";
	TempFilename += Name;
	TempFilename += CIwGameString((int)(s3eTimerGetMs() & 0xffffffff));	// Ensure filename is unique
	TempFilename += ".html";
	CIwGameFile file;
	if (file.Open(TempFilename.c_str(), "wb"))
	{
		file.Write((void*)h.c_str(), h.GetLength());
	}
}

bool CIwGameUIWebView::InitView(bool modal, bool transparent, const char* java_script)
{
	PrevVisible = false;
	Modal = modal;
	if (s3eWebViewAvailable())
	{
		// If local html then displaythe local html instead
		if (!TempFilename.IsEmpty())
			URI = TempFilename;
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

void CIwGameUIWebView::UpdatePosSize()
{
	float sw = (float)Scene->getScreenSize().x;
	float sh = (float)Scene->getScreenSize().y;
	float x;
	float y;
	if (PercentagePosition)
	{
		x = ((OriginalPosition.x * sw) / 100.0f);
		y = ((OriginalPosition.y * sh) / 100.0f);
	}
	else
	{
		x = OriginalPosition.x;
		y = OriginalPosition.y;
	}
	if (OriginalSize.x < 0)
		Size.x = (int)((-OriginalSize.x * sw) / 100.0f);
	if (OriginalSize.y < 0)
		Size.y = (int)((-OriginalSize.y * sh) / 100.0f);
	x += (sw - Size.x) / 2;
	y += (sh - Size.y) / 2;

	switch (Docking)
	{
	case DK_Top:
		Position.x = x;
		Position.y = Margin.w;
		break;
	case DK_Bottom:
		Position.x = x;
		Position.y = sh - Size.y - Margin.h;
		break;
	case DK_Left:
		Position.x = Margin.x;
		Position.y = y;
		break;
	case DK_Right:
		Position.x = sw - Size.x - Margin.y;
		Position.y = y;
		break;
	case DK_TopLeft:
		Position.x = Margin.x;
		Position.y = Margin.w;
		break;
	case DK_TopRight:
		Position.x = sw - Size.x - Margin.y;
		Position.y = Margin.w;
		break;
	case DK_BottomLeft:
		Position.x = Margin.x;
		Position.y = sh - Size.y - Margin.h;
		break;
	case DK_BottomRight:
		Position.x = sw - Size.x - Margin.y;
		Position.y = sh - Size.y - Margin.h;
		break;
	default:
		Position.x = x;
		Position.y = y;
	}
}

bool CIwGameUIWebView::UpdateVisual()
{
	if (WebView == NULL)
		return true;

	UpdatePosSize();
	
	if (IsVisible && !PrevVisible)
		s3eWebViewShow(WebView, (int)Position.x, (int)Position.y, (int)Size.x, (int)Size.y);
	else
	if (!IsVisible && PrevVisible)
		s3eWebViewHide(WebView);

	if (IsVisible)
	{
		if (OldPosition.x != Position.x || OldPosition.y != Position.y)
			s3eWebViewShow(WebView, (int)Position.x, (int)Position.y, (int)Size.x, (int)Size.y);
	}

	PrevVisible = IsVisible;
	OldPosition.x = Position.x;
	OldPosition.y = Position.y;

	return true;
}

bool CIwGameUIWebView::getProperty(unsigned int element_name, CIwGameXomlProperty& prop)
{
	if (WebView == NULL)
		return false;

	if (CIwGameUIBase::getProperty(element_name, prop))
		return true;

	return true;
}

bool CIwGameUIWebView::setProperty(unsigned int element_name, CIwGameString& data, bool delta)
{
	if (CIwGameUIBase::setProperty(element_name, data, delta))
		return true;

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
	if (element_name == CIwGameXomlNames::Html_Hash)
	{
		setHtml(data.c_str());
		if (!TempFilename.IsEmpty())
			s3eWebViewNavigate(WebView, TempFilename.c_str());
	}
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
	if (CIwGameUIBase::UpdateBinding(element_name, var))
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
		else
		if (name_hash == CIwGameXomlNames::Html_Hash)
			setHtml((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnError_Hash)
			EventsManager->addEvent("OnError", (*it)->GetValue().c_str(), true);
	}

	if (!s3eWebViewAvailable())
	{
		ProcessEventActions(IW_GAME_HASH("OnError"));
		CIwGameError::LogError("Error: XOML - WebView not supported on this platform");
		IW_GAME_XOML->setExitOnError(false);
		return false;
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

void CIwGameUIWebView::OnOrientationChange(eIwGameScene_Orientation old_orientation, eIwGameScene_Orientation new_orientation)
{
	if (WebView != NULL)
		s3eWebViewHide(WebView);
	PrevVisible = false;
	IsVisible = true;

	CIwGameUIBase::OnOrientationChange(old_orientation, new_orientation);
}

