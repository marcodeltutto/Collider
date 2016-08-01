#if !defined(_GAME_SCENE_GL_H_)
#define _GAME_SCENE_GL_H_

#include "IwGameScene.h"
#include "GLMesh.h"
#include "GLCube.h"
#include "GLEndcap.h"
#include "GLCylinder.h"
#include "GLDonut.h"
#include "MeshGroup.h"
#include "ABoxFromXML.h"
#include "ACylinderFromXML.h"
#include "EndcapFromXML.h"
#include "EventFromXML.h"
#include "ColourStore.h"
#include "Animation.h"
#include "IwGameUIListBox.h"
#include "IwGameUISlider.h"
#include "IwGL.h"
#include <vector>
#include <map>


//#ifdef _DEBUG
	#include<iostream>
//#endif
//
//
// IwGxScene - Open GL compatible game scene
//
//
class IwGxScene : public CIwGameScene
{


	


public:
	int				Init(int max_collidables = 128, int max_layers = 10, bool doSleep = true);
	void			Update(float dt);
	void			Draw();

	~IwGxScene(){
		XOMLcleanup();
	}
	MeshGroup       meshes, eventMeshes;
	// Implementation of IIwGameXomlClass interface
	bool			LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
	void			testInit();
	void			XOMLcleanup();
	void			InitColourMap();
	Animation		animation;
	void			Release();
	virtual void NotifySuspending(CIwGameScene * scene){
		IwGLSuspend();
	}
	virtual void NotifyResuming(CIwGameScene * scene){
		IwGLResume();
	}
	//GL camera position
	CIwFVec3 camera, camera_rot, up_vec;
	//for cleaning
	std::vector <IIwGameXomlResource*> toBeCleaned;
	//colours
	ColourStore colours;
	std::map<unsigned int, float*> name_color_map;
	bool allowUserInput;
	float zoomMin, zoomMax;
	void HandleInputs();
	//screen dimensions
	int OriginalX,OriginalY;
	//event
	EventFromXML * currentEvent;
	EventsFromXML  eventList;
	//links to HUD stuff;
	CIwGameScene* hudScene;
	CIwGameXomlVariableFloat * red_value, * green_value, * blue_value, * alpha_value;
	CIwGameUISlider * red_slider, * green_slider, * blue_slider, * alpha_slider;
	CIwGameUIListBox * primitives_listBox;
	CIwGameUILabel * projectionButton, * detectorVisiblityButton, * animateEventButton, * protonAnimationEventButton;
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
	void UpdateProjectionButton();
	void UpdateShowDetectorsButton();
	void UpdateAnimateEventButton();
	void UpdateProtonAnimateEventButton();
	void TakeScreenshot();
	void SavePixelsOnScreen();
	int glScreenWidth, glScreenHeight;
	
	void setScreenParams();
	void StreamEventFromCERN();

	bool transverseProjectionMode, showDetectors;

	//load an event XML file with an event xml 
	void LoadThisFile(char * fileName){
		bool hasWorked = IW_GAME_XOML->Process(this,fileName,false);
	}
	
};

//
//  IwGxSceneCreator - Creates an instance of a game scene GL object
//
class IwGxSceneCreator : public IIwGameXomlClassCreator
{
public:
	IwGxSceneCreator()
	{
		setClassName("gamescenegl");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new IwGxScene(); }
};

#endif // _GAME_SCENE_H_
