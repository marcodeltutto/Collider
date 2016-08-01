//
//  WebBrowser.cpp
//  GameSceneGL
//
//  Created by Tom McLaughlan on 12/06/2013.
//
//

#include "WebBrowser.h"
#include "s3eWebView.h"
#include "s3eSurface.h" // marco's  addition

#include <sstream>


int WebBrowser::Init(int max_collidables, int max_layers, bool doSleep) {
    CzScene::Init(max_collidables, max_layers, doSleep);
    webView = s3eWebViewCreate();
    s3eWebViewClearCache(webView);
    //s3eWebViewShow(webView, 64, 10, s3eSurfaceGetInt(S3E_SURFACE_WIDTH)-10-64, s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)-20);
    url="";
    
    wasVisible = false;
    
    //check OS type
//    int os = s3eDeviceGetInt(S3E_DEVICE_OS);
//    
//    if (os == S3E_OS_ID_IPHONE){
//        width = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT) * 0.9;
//        height = s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
//        startx = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT) * 0.1;
//    }
//    else{
        width = s3eSurfaceGetInt(S3E_SURFACE_WIDTH) * 0.9;
        height = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);
        startx = s3eSurfaceGetInt(S3E_SURFACE_WIDTH) * 0.1;
//    }
    
    starty = 0;
//    width = s3eSurfaceGetInt(S3E_SURFACE_WIDTH) * 0.9;
//    height = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);
    homeButton = NULL;
    refreshButton = NULL;
    InitLinks();
    
}

void WebBrowser::InitLinks() {
    
    //marco CIwGameError::LogError("Initialising links");
    
    if(!homeButton) {
        homeButton = (CzUILabel*) this->findActor("homeButton");
    }
    if(!refreshButton) {
        refreshButton = (CzUILabel*) this->findActor("refreshButton");
    }
}

void WebBrowser::Release() {
    s3eWebViewDestroy(webView);
}

void WebBrowser::refreshPage() {
    s3eWebViewSendJavaScript(webView, "location.reload()");
    std::cout << "reload" << std::endl;
}

void WebBrowser::Update(float dt) {
    CzScene::Update(dt);

    if(!homeButton || !refreshButton)
        InitLinks();
    
    if(homeButton->isSelected()) {
        s3eWebViewNavigate(webView, url.c_str());
        homeButton->setSelected(false);
    }
    if(refreshButton->isSelected()) {
        refreshPage();
        refreshButton->setSelected(false);
    }
    
    CzXomlVariable* weburl = (CzXomlVariable*) this->getVariableManager()->findVariable("WebURL");
    
    if(!this->isVisible() && wasVisible) {
        //this->setClippingArea(0, 0, 0, 0);
        this->setLayer(-1);
        s3eWebViewHide(webView);
        wasVisible = false;
    }
    if(this->isVisible() && !wasVisible) {
        this->setLayer(3);
        //this->setClippingArea(0, 0, s3eSurfaceGetInt(S3E_SURFACE_WIDTH), s3eSurfaceGetInt(S3E_SURFACE_HEIGHT));
        s3eWebViewShow(webView, startx, starty, width, height);
        wasVisible = true;
    }
    
    
    if(url != weburl->getValue().c_str()) {
        url = weburl->getValue().c_str();
        std::cout << url.c_str() << std::endl;
        s3eWebViewNavigate(webView, url.c_str());
    }
    
}

void WebBrowser::Draw()
{
    CzScene::Draw();

}