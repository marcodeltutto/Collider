#if !defined(_IWGX_SCENE_GL_H_)
#define _IWGX_SCENE_GL_H_

//#include "IwGameScene.h"
#include "CzScene.h"
#include "GLMesh.h"
#include "GLCube.h"
#include "GLEndcap.h"
#include "GLCylinder.h"
#include "GLDonut.h"
#include "GLArrow.h"
#include "GLCone.h"
#include "MeshGroup.h"
#include "ABoxFromXML.h"
#include "ACylinderFromXML.h"
#include "EndcapFromXML.h"
#include "EventFromXML.h"
#include "EventManager.h"
#include "ColourStore.h"
#include "Animation.h"
//#include "IwGameUIListBox.h"
#include "CzUIListBox.h"
#include "GAMarmalade.h"
//#include "IwGameUISlider.h"
#include "CzUISlider.h"
#include "CzUILabel.h" // added by marco
#include "IwGL.h"
#include <vector>
#include <map>
//#include "IwGameFacebook.h"
#include "CzPlatformFacebook.h"
#include "ClassificationGame.h"
#include <sstream>
//#ifdef _DEBUG
	#include<iostream>
//#endif
//
//
// IwGxScene - Open GL compatible game scene
//
//
class IwGxScene : public CzScene
{
public:
	int				Init(int max_collidables = 128, int max_layers = 10, bool doSleep = true);
	void			Update(float dt);
	void			Draw();

	~IwGxScene(){
        if(!analytics){
            std::stringstream end;
            end << "end_" << gameID;
            std::cout << end.str() << std::endl;
            GAMarmalade::getobj()->pageview(end.str().c_str());
            GAMarmalade::getobj()->update();
            analytics = true;
        }
        
		XOMLcleanup();
		if (!(pMat==NULL)) delete pMat;
		if (classificationGame!=NULL) delete classificationGame;
	}
	MeshGroup       meshes, eventMeshes;
	// Implementation of IIwGameXomlClass interface
	int		    	LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node);
	void			testInit();
	void			XOMLcleanup();
	void			InitColourMap();
	Animation		animation;
	void			Release();

	CIwMaterial* pMat;
	CIwFMat* modelMatrix;

	virtual void NotifySuspending(IwGxScene * scene){
		IwGLSuspend();
	}
	virtual void NotifyResuming(IwGxScene * scene){
		IwGLResume();
	}
	//GL camera position
	CIwFVec3 camera, camera_rot, up_vec;
	
	//for cleaning
	std::vector <IzXomlResource*> toBeCleaned;
	//colours
	ColourStore colours;
	std::map<unsigned int, float*> name_color_map;
	bool allowUserInput;
	float zoomMin, zoomMax;
	void HandleInputs();
	//screen dimensions
	int OriginalX,OriginalY;
    
    std::string gameID;
    
    //for rotating
    float rotX, rotY, rotZ, rotDamp;
    float oldrotX, oldrotY, oldrotZ;
    //pinching
    float pinch, oldpinch;
    float minZoom, maxZoom;
    float* zoom;
	
	//event
    EventManager * eventManager;
	EventFromXML * currentEvent;
	EventsFromXML  eventList;
	//links to HUD stuff;
	IwGxScene* hudScene;
	IwGxScene* colourScene;
	IwGxScene* gameScene;
	IwGxScene* liveScene;
    
    bool analytics;
	
    CzXomlVariableBool* menu_active;
    CzXomlVariableBool* game_active;
    CzXomlVariableBool* live_active;
    CzXomlVariableBool* next_ev;
    CzXomlVariableBool* prev_ev;
    CzXomlVariableInt* set_ev;
	CzXomlVariableFloat * red_value, * green_value, * blue_value, * alpha_value;
	CzUISlider * red_slider, * green_slider, * blue_slider, * alpha_slider;
	CzUIListBox * primitives_listBox;
	CzUILabel * endcapButton, * transverseButton, * detectorVisiblityButton, * animateEventButton, * protonAnimationEventButton, * facebookButton, * nextEventButton, * prevEventButton, * resetButton;
	//Game stuff
	ClassificationGame * classificationGame;
	void InitHUDLinks();
	void UpdateColorHUD();
	float * UpdateColorHUDPrimitive();
	float * GetRGBAFromListIndex(int index);
	//projection stuff
	int projectionMode;
	void orthogonalDraw();
	void perspectiveDraw();
	float growthFactorFromInput;
	CIwFVec3 linearScaleFactor;
	//non-colour HUD buttons
	void UpdateEndcapButton();
	void UpdateTransverseButton();
	void UpdateShowDetectorsButton();
	void UpdateAnimateEventButton();
	void UpdateResetButton();
	void UpdateNextEventButton();
	void UpdatePrevEventButton();
	void UpdateProtonAnimateEventButton();
	void UpdateFacebookButton();
	void TakeScreenshot();
	void SavePixelsOnScreen();
	int glScreenWidth, glScreenHeight;
	
	void setScreenParams();
	void StreamEventFromCERN();
    void StreamFromHTTP(const std::string& url);

	bool transverseProjectionMode, showDetectors;

	//load an event XML file with an event xml 
	void LoadThisFile(const char * fileName){
		bool hasWorked = CZ_XOML->Process(this,fileName,false);
	}
	
};

//
//  IwGxSceneCreator - Creates an instance of a game scene GL object
//
class IwGxSceneCreator : public IzXomlClassCreator
{
public:
	IwGxSceneCreator()
	{
		setClassName("iwgxscene");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new IwGxScene(); }
};

#endif // _IWGX_SCENE_H_
