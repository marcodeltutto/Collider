//
//  GameLogicScene.h
//  GameSceneGL
//
//  Created by Tom McLaughlan on 24/05/2013.
//
//

#ifndef _WEB_BROWSER_H_
#define _WEB_BROWSER_H_

#include "Game.h"
//#include "IwGameScene.h"
#include "CzScene.h"
//#include "IwGameUI.h"
#include "CzUIBase.h"
#include "EventManager.h"
//#include "IwGameUIListBox.h"
#include "CzUIListBox.h"
#include "CzUILabel.h"
#include "FileUtils.h"
#include <iostream>
#include <vector>
#include <string>

/*
 * This scene controls the main Web Browser
 * The main functionality of the class is to create a WebView for the online help
 **/

class s3eWebView;

class WebBrowser : public CzScene
{
public:
    virtual ~WebBrowser() { }
    
    int		Init(int max_collidables = 128, int max_layers = 10, bool doSleep = true);
	void	Update(float dt);
	void	Draw();
    void    Release();
    
    void InitLinks();
    void refreshPage();
    
    bool wasVisible;
    
    // Implementation of IIwGameXomlResource interface
	int	LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node)
	{
		if (!CzScene::LoadFromXoml(parent, load_children, node))
			return false;
        
		// Add our own custom attribute parsing
		for (CzXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
		{
			unsigned int name_hash = (*it)->getName().getHash();
		}
        
		return true;
	}
    
private:
    std::string url;
    s3eWebView* webView;
    int startx;
    int starty;
    int width;
    int height;
    
    CzUILabel* homeButton;
    CzUILabel* refreshButton;

};

//
//  WebBrowserCreator - Creates an instance the 'WebBrowser'
//
class WebBrowserCreator : public IzXomlClassCreator
{
public:
	WebBrowserCreator()
	{
		setClassName("WebBrowser");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new WebBrowser(); }
};

#endif /* defined(_WEB_BROWSER_H_) */
