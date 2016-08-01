#include "IwGxScene.h"

//#include "IwGameCamera.h"
#include "CzCamera.h"
//#include "IwGameInput.h"
#include "CzInput.h"
//#include "IwGame.h"
#include "CzApp.h"
#include "My3DUtils.h"
#include "ColourStore.h"
//#include "IwGameUI.h"
#include "CzUIBase.h"
//#include "IwGameUIListBox.h"
#include "CzUIListBox.h"
#include "GAMarmalade.h"
#include "Game.h"

#include "Tracks.h"

#include "IwImage.h"

#include <sstream>
//
//
//
// IwGxScene implementation
//
//
//



int IwGxScene::Init(int max_collidables, int max_layers, bool doSleep)
{
	CzScene::Init(max_collidables, max_layers, doSleep);
    
	//event
	//currentEvent = NULL;
    eventManager = Game::getInstance()->getEventManager();
	//hud scene
	primitives_listBox = NULL;
	endcapButton = NULL;
	resetButton = NULL;
    transverseButton = NULL;
	detectorVisiblityButton = NULL;
	animateEventButton = NULL;
	InitHUDLinks();
	//are we looking in the transverse plane?
	transverseProjectionMode = false;
	showDetectors = false;
	
	//screen touches
	allowUserInput=true;
	//individual shapes for testing
	testInit();
	//camera
	camera = CIwFVec3(0,0,1);
	camera_rot = CIwFVec3(0,0,0);
	up_vec = CIwFVec3(0,1,0);
	linearScaleFactor = CIwFVec3(0.0,0.0,0.0);
	//screen diensions
	OriginalX = IwGLGetInt(IW_GL_WIDTH);
    OriginalY = IwGLGetInt(IW_GL_HEIGHT);
	
    //Rotation variables
    rotX=0;
    rotY=0;
    rotZ=0;
    rotDamp=0.95;
    pinch=0;
    oldpinch=0;
    zoom=&(GAME->zoom);
    minZoom=0.05;
    maxZoom=5.0;
    
    modelMatrix = &(GAME->rotMatrix);
    
	//Animation
	animation.isPlaying= true;
	
	pMat = NULL;
	//projection
	projectionMode=0;
	/*glMatrixMode(GL_MODELVIEW);
	glClearDepthf(1.0);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glClearDepthf(1.0);
    glLoadIdentity();*/
	//glFrustumf(-0.5,0.5,-0.5,0.5,0.1,10.0);
    //glDisable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
	setScreenParams();
	
	classificationGame = NULL;
    analytics = true;
	return 1;
}

void IwGxScene::InitHUDLinks(){
    
	hudScene = (IwGxScene*)Parent->findScene("MenuScene");
	if (!hudScene){
		//marco CIwGameError::LogError("Warning IwGxScene::InitHUDLinks() cannot find MenuScene");
		return;
	}
	gameScene = (IwGxScene*)Parent->findScene("GameScene");
	if (!gameScene){
		//marco CIwGameError::LogError("Warning IwGxScene::InitHUDLinks() cannot find GameScene");
	}
	liveScene = (IwGxScene*)Parent->findScene("LiveScene");
	if (!liveScene){
		//marco CIwGameError::LogError("Warning IwGxScene::InitHUDLinks() cannot find LiveScene");
	}
    colourScene = (IwGxScene*)Parent->findScene("ColourScene");
	if (!colourScene){
		//marco CIwGameError::LogError("Warning IwGxScene::InitHUDLinks() cannot find ColourScene");
		return;
	}
    
    if(!eventManager->isTutorial()) {
        hudScene->findActor("MenuHelp")->setVisible(false);
        if(gameScene)
            gameScene->findActor("IntroText")->setVisible(false);
        CzScene* menuButtonScene = Parent->findScene("menuButtonScene");
        if (!menuButtonScene){
            //marco CIwGameError::LogError("Warning IwGxScene::InitHUDLinks() cannot find menuButtonScene");
        }
        CzScene* gameMenu = Parent->findScene("GameMenu");
        if (!gameMenu){
            //marco CIwGameError::LogError("Warning IwGxScene::InitHUDLinks() cannot find GameMenu");
        }
        if(gameMenu) {
            gameMenu->findActor("InfoText")->setVisible(false);
            gameMenu->findActor("ArrowRight")->setVisible(false);
        }
        menuButtonScene->findActor("InfoText")->setVisible(false);
        menuButtonScene->findActor("ArrowLeft")->setVisible(false);
    }
    else if(eventManager->isTutorial()) {
        hudScene->findActor("MenuHelp")->setVisible(true);
        if(gameScene)
            gameScene->findActor("IntroText")->setVisible(true);
        CzScene* menuButtonScene = Parent->findScene("menuButtonScene");
        if (!menuButtonScene){
            //marco CIwGameError::LogError("Warning IwGxScene::InitHUDLinks() cannot find menuButtonScene");
        }
        CzScene* gameMenu = Parent->findScene("GameMenu");
        if (!gameMenu){
            //marco CIwGameError::LogError("Warning IwGxScene::InitHUDLinks() cannot find GameMenu");
        }
        if(gameMenu) {
            gameMenu->findActor("InfoText")->setVisible(true);
            gameMenu->findActor("ArrowRight")->setVisible(true);
        }
        menuButtonScene->findActor("InfoText")->setVisible(true);
        menuButtonScene->findActor("ArrowLeft")->setVisible(true);
    }
    
    gameID = hudScene->getVariableManager()->findVariable("GameID")->getValue().c_str();
    
	//sliders variable bindings
	red_value = (CzXomlVariableFloat*) colourScene->getVariableManager()->findVariable("slider_red_value");
	green_value = (CzXomlVariableFloat*) colourScene->getVariableManager()->findVariable("slider_green_value");
	blue_value = (CzXomlVariableFloat*) colourScene->getVariableManager()->findVariable("slider_blue_value");
	alpha_value = (CzXomlVariableFloat*) colourScene->getVariableManager()->findVariable("slider_alpha_value");
	//sliders
	red_slider = (CzUISlider*) colourScene->findActor("slider_red");
	green_slider = (CzUISlider*) colourScene->findActor("slider_green");
	blue_slider = (CzUISlider*) colourScene->findActor("slider_blue");
	alpha_slider = (CzUISlider*) colourScene->findActor("slider_alpha");
	//listBox
	primitives_listBox = (CzUIListBox*) colourScene->findActor("PrimitivesListBox");
	//button for orthogonal projection
	endcapButton = (CzUILabel*) hudScene->findActor("endcapbutton");
	detectorVisiblityButton = (CzUILabel*) hudScene->findActor("detectorVisiblityButton");
	animateEventButton = (CzUILabel*) hudScene->findActor("animateEventButton");
	resetButton = (CzUILabel*) hudScene->findActor("resetButton");
	transverseButton = (CzUILabel*) hudScene->findActor("transverseButton");
	nextEventButton = (CzUILabel*) hudScene->findActor("nextEventButton");
	prevEventButton = (CzUILabel*) hudScene->findActor("prevEventButton");
	protonAnimationEventButton = (CzUILabel*) hudScene->findActor("protonAnimateEventButton");
	//facebook post-to-wall
	facebookButton = (CzUILabel*) hudScene->findActor("facebookButton");
    //menu active?
    menu_active = (CzXomlVariableBool*) hudScene->getVariableManager()->findVariable("MenuActive");
    next_ev = (CzXomlVariableBool*) hudScene->getVariableManager()->findVariable("NextEvent");
    prev_ev = (CzXomlVariableBool*) hudScene->getVariableManager()->findVariable("PrevEvent");
    set_ev = (CzXomlVariableInt*) hudScene->getVariableManager()->findVariable("SetEvent");
    
    if(gameScene)
        game_active = (CzXomlVariableBool*) gameScene->getVariableManager()->findVariable("GameActive");
    else {
        game_active = new CzXomlVariableBool();
        game_active->setValue("false");
    }
    if(liveScene)
        live_active = (CzXomlVariableBool*) liveScene->getVariableManager()->findVariable("LiveActive");
    else {
        live_active = new CzXomlVariableBool;
        live_active->setValue("false");
    }
}
void IwGxScene::UpdateColorHUD(){
	if (!primitives_listBox){
		//marco CIwGameError::LogError("Warning IwGxScene::UpdateColorHUD() cannot find HUDScene ListBox");
		return;
	}
	
	//pointer to the R in an RGBA array
	//which item is selected in the listbox?
	float * red_in_map = UpdateColorHUDPrimitive();
	
#ifdef _DEBUG
	//std::cout<<"red_in_map = "<< red_in_map <<std::endl;
#endif
	if (red_in_map==NULL) return;
	
	//update colours
	*red_in_map = red_slider->getValue();
	*(red_in_map+1) = green_slider->getValue();
	*(red_in_map+2) = blue_slider->getValue();
	*(red_in_map+3) = alpha_slider->getValue();

#ifdef _DEBUG
	//std::cout<<"r,g,b,a from HUD = "<< red_value->getValue().GetAsFloat()<<","<< green_value->getValue().GetAsFloat()<<","<< blue_value->getValue().GetAsFloat()<<","<< alpha_value->getValue().GetAsFloat()<<" and from map = "<<*name_color_map[Tracks::idTracks_hash] <<","<<*(name_color_map[Tracks::idTracks_hash]+1)<<std::endl;
#endif

}

float * IwGxScene::GetRGBAFromListIndex(int index){
	float * red_in_map =NULL;
	if (index==0) red_in_map = name_color_map[Tracks::idTracks_hash];
	else if (index==1) red_in_map = name_color_map[Tracks::muonTracks_hash];
	else if (index==2) red_in_map = name_color_map[ACylinderFromXML::pixName_hash];
//	else if (index==3) red_in_map = name_color_map[ACylinderFromXML::trtName_hash];
	else if (index==3) red_in_map = &colours.rgba_ECAL[0];
	else if (index==4) red_in_map = &colours.rgba_HCAL[0];
	else if (index==5) red_in_map = name_color_map[ACylinderFromXML::muonDet_hash];

	return red_in_map;
}
float * IwGxScene::UpdateColorHUDPrimitive(){
	float * red_in_map = NULL;
	static int index = -1;
	//find out which index of the listBox in HUDSceen is selected
	//has the selected box changed?
	if (index != primitives_listBox->getSelectionIndex()){
		//is anything selected?
		if (primitives_listBox->getSelectionIndex() == -1) return NULL;
		index = primitives_listBox->getSelectionIndex();
		//get the address from the listbox index
		red_in_map = GetRGBAFromListIndex(index);
		if (!red_in_map) return NULL;
#ifdef _DEBUG
		//std::cout <<" slider colours previously = "<<red_value->getValue().c_str()<<","<<green_value->getValue().c_str()<<","<<blue_value->getValue().c_str()<<","<<alpha_value->getValue().c_str()<<std::endl;
#endif
		//then update the slider positions based on this objects colours in the map
		//if (!red_in_map) return NULL;
		red_value->setValue(CzString(*(red_in_map)).c_str());
		green_value->setValue(CzString(*(red_in_map+1)).c_str());
		blue_value->setValue(CzString(*(red_in_map+2)).c_str());
		alpha_value->setValue(CzString(*(red_in_map+3)).c_str());
		//update the sliders too, bug in Engine I think!
		red_slider->setValue(*(red_in_map), true);     // marco: adding to true,this will give the same implementation as before
		green_slider->setValue(*(red_in_map+1), true);
		blue_slider->setValue(*(red_in_map+2), true);
		alpha_slider->setValue(*(red_in_map+3), true);
		
/*
#ifdef _DEBUG
		std::cout <<" updating slider colours, index = "<<index<<std::endl;
		std::cout <<" updating slider colours, r,g,b,a= "<<*(red_in_map)<<","<<*(red_in_map+1)<<","<<*(red_in_map+2)<<","<<*(red_in_map+3)<<std::endl;
		std::cout <<" slider colours NOW = "<<red_value->getValue().c_str()<<","<<green_value->getValue().c_str()<<","<<blue_value->getValue().c_str()<<","<<alpha_value->getValue().c_str()<<std::endl;
#endif
*/
		return NULL;
	}
	
	//index not changed, so we pass the address of the correct RGBA array
	index = primitives_listBox->getSelectionIndex();
	//get the address from the listbox index
	red_in_map = GetRGBAFromListIndex(index);
	return red_in_map;
//#ifdef _DEBUG
//	std::cout<<"primitives index  = "<< index<<std::endl;
//#endif

}

void IwGxScene::InitColourMap(){
	//map entries for detector types to RGBA arrays
	name_color_map[ACylinderFromXML::pixName_hash] = &colours.rgba_Pix[0];
	//name_color_map[ACylinderFromXML::sctName_hash] = &colours.rgba_SCT[0];
	name_color_map[ACylinderFromXML::sctName_hash] = &colours.rgba_Pix[0];
	name_color_map[ACylinderFromXML::trtName_hash] = &colours.rgba_Pix[0];
	//ecal colours
	name_color_map[ACylinderFromXML::ecalName_hash] = &colours.rgba_ECAL[0];
	name_color_map[ACylinderFromXML::LArEcap_hash] = &colours.rgba_ECAL[0];
	//hcal colours
	name_color_map[ACylinderFromXML::tileEcap_hash] = &colours.rgba_HCAL[0];
	//muon detector colours
	name_color_map[ACylinderFromXML::muonDet_hash] = &colours.rgba_muonDets[0];
	//event colours
	name_color_map[Tracks::idTracks_hash] = &colours.rgba_idTracks[0];
	name_color_map[Tracks::muonTracks_hash] = &colours.rgba_muonTracks[0];
	name_color_map[CaloTowers::ecalTowers_hash] = &colours.rgba_ecalTowers[0];
	name_color_map[CaloTowers::hcalTowers_hash] = &colours.rgba_hcalTowers[0];
	name_color_map[EventFromXML::MET_hash] = &colours.rgba_MET[0];

	//endcap colours	
	name_color_map[EndcapFromXML::pix_hash] = &colours.rgba_Pix[0];
	name_color_map[EndcapFromXML::sct_hash] = &colours.rgba_Pix[0];
	name_color_map[EndcapFromXML::trt_hash] = &colours.rgba_Pix[0];
	name_color_map[EndcapFromXML::ecal_hash] = &colours.rgba_ECAL[0];
	name_color_map[EndcapFromXML::hcal_hash] = &colours.rgba_HCAL[0];
	name_color_map[EndcapFromXML::muon_hash] = &colours.rgba_muonDets[0];
	
	


//#ifdef _DEBUG
//	std::cout<<"find real "<< name_color_map.find(ACylinderFromXML::sctName_hash)<<" not real "<<name_color_map.find(0) <<std::endl;
//#endif
}
//need to delete some objects created in XOML on the heap
void IwGxScene::XOMLcleanup(){
#ifdef _DEBUG
	CIwGameError::LogError("starting GameScene::XOMLcleanup()");
#endif
	if (! toBeCleaned.size()) return;

	for (int iObj = (int)toBeCleaned.size()-1; iObj>=0;iObj--){
		delete toBeCleaned.at(iObj);
	}
	toBeCleaned.clear();

}
void IwGxScene::testInit(){
#ifdef _DEBUG
	CIwGameError::LogError("starting GameScene::testInit()");
#endif

	/*glCube.Init(0.5,0.0,0.0,
				0.5,0.5,0.0,
				0.75,0.5,0.0,
				0.75,0.75,0.0,
				
				0.5,0.0,0.5,
				0.5,0.5,0.5,
				0.75,0.5,0.5,
				0.75,0.75,0.5,	

				0.5);*/

	/*
	GLCylinder * glCylinder = new GLCylinder();
	GLDonut * glDonut = new GLDonut();
	GLEndcap * glEndcap = new GLEndcap();

	glCylinder.Init(0.5,-0.5,0.5,12,1.0);
	glDonut.Init(5.5,-5.5,0.7,0.4,10,1.0);
	glEndcap.Init(5.5,-5.5,0.7,0.4,10,1.0);
	*/
	/*
	GLDonut * glDonut = new GLDonut();
	glDonut->Init(5.5,-5.5,0.7,0.4,10,1.0);
	glDonut->setRGBA(colours.rgba_SCT);
	*/
	
//	growthFactorFromInput = 1.0;
//	zoomMin = 0.15;
//	zoomMax = 10.0;
	
	meshes.Init();
	eventMeshes.Init();

	//meshes.pos = CIwFVec3(0.0,0.0,0.0); 
	//meshes.worldScale = 500.0f;
	meshes.pos = CIwFVec3(0.0,0.0,10); 
	meshes.worldScale = 2.f;

	eventMeshes.pos = CIwFVec3(0.0,0.0,0.1); 
	eventMeshes.worldScale = 2.f;

	

	//GLDonut * glDonut = new GLDonut();
	
	//glDonut->Init(0.5,-0.5,0.7,0.4,10,meshes.worldScale);
	//glDonut->setRGBA(colours.rgba_SCT);
	
	InitColourMap();
	/*
	meshes.children->push_back(glCube);
	meshes.children->push_back(glCylinder);
	meshes.children->push_back(glDonut);
	*/
	

	//load detectors
	CZ_XOML->Process(this,"test_detectors.xml",false);


	XOMLcleanup();
	//load test event
	/*
	LoadThisFile("test_event_2tracks.xml");
	LoadThisFile("test_event_1muonTrack.xml");
	
	//load event list from CERN
	LoadThisFile("http://www-pnp.physics.ox.ac.uk/~boddy/files/files.xml");
	//init the animation
	*/

	/*classification game*/
	
	std::cout<<"!!!!!!!!!!!!!!!!!!!\n";
	std::cout<<" About to load classificationGame.\n";
	std::cout<<"!!!!!!!!!!!!!!!!!!!\n";
	//IW_GAME_XOML->Process(this,"HiggsGame.xml",false);
	
    //LoadThisFile("higgs_hunt_events.xml");
	//LoadThisFile("test_event.xml");
    
    /*CIwGameXomlVariable* ev = (CIwGameXomlVariable*) hudScene->getVariableManager()->findVariable("Event");
    
    CIwGameString event = ev->getValue();
    
    IW_GAME_XOML->Process(this, event.c_str(), false);*/
    StreamFromHTTP("");
    
//    LoadThisFile("minerva_higgs_event.xml");
	//IW_GAME_XOML->Process(this,"minerva_higgs_event.xml",false);
	//LoadThisFile("test_event_2tracks.xml");
	animation.Init(eventMeshes.worldScale,this);
#ifdef _DEBUG
	std::cout <<" !!!!! starting animation in init with toBeCleaned.size() = "<<toBeCleaned.size()<<"!!!!!\n";
#endif

}

void IwGxScene::Update(float dt)
{
    if(analytics) {
        analytics = false;
        std::stringstream start;
        start << "start_" << gameID;
        std::cout << start.str() << std::endl;
        GAMarmalade::getobj()->pageview(start.str().c_str());
        GAMarmalade::getobj()->update();
    }
	//std::cout <<"Entering Update\n";
    
	//check for touches
    HandleInputs();
	CzScene::Update(dt);
	//get slider values for colours
	UpdateColorHUD();
	//show endcaps?
	UpdateEndcapButton();
    //move to transverse projection?
	UpdateTransverseButton();
	//show / hide detector button pressed?
	UpdateShowDetectorsButton();
	//restart animation button pressed?
	UpdateAnimateEventButton();
    //reset the rotation matrix?
    UpdateResetButton();
	//restart proton animation button pressed?
	//UpdateProtonAnimateEventButton();
	//posting to Facebook
	//UpdateFacebookButton();
	//update scene animation
	if (animation.isPlaying) animation.UpdateAnimation(&meshes.rot, growthFactorFromInput);
	
    // This is here because we want this to take effect in the menu too.
    if(fabs(rotX)>0)
        rotX*=rotDamp;
    if(fabs(rotY)>0)
        rotY*=rotDamp;
    if(fabs(rotZ)>0)
        rotZ*=rotDamp;
	
}


void IwGxScene::TakeScreenshot(){
	int size = IwGxGetScreenWidth() * IwGxGetScreenHeight();
	std::vector<unsigned int> byte_buffer;
	byte_buffer.reserve(4*size);
	for (unsigned int iByte=0;iByte < size*4;iByte++) byte_buffer.push_back(0);
	glReadPixels(0,0,IwGxGetScreenWidth(),IwGxGetScreenHeight(),GL_RGBA,GL_UNSIGNED_BYTE,&byte_buffer[0]);


}

void IwGxScene::SavePixelsOnScreen(){


}

void IwGxScene::UpdateResetButton(){
	if (!resetButton){
		//marco CIwGameError::LogError("Warning IwGxScene::UpdateResetButton(), button not present!");
		return;
	}
	static bool isPressed = false;
	//is unchanged then do nothing
	if (resetButton->isSelected() == isPressed) return;
	//update the bool
	else isPressed = resetButton->isSelected();
    modelMatrix->SetIdentity();
    (*zoom) = 0.7;
}

void IwGxScene::UpdateAnimateEventButton(){
	if (!animateEventButton){
		//marco CIwGameError::LogError("Warning IwGxScene::UpdateAnimateEventButton(), button not present!");
		return;
	}
	static bool isPressed = false;
	//is unchanged then do nothing
	if (animateEventButton->isSelected()== isPressed) return;
	//update the bool
	else isPressed = animateEventButton->isSelected();
	
	//start an animation
#ifdef _DEBUG
	std::cout <<" !!!!! re-starting animation with toBeCleaned.size() = "<<toBeCleaned.size()<<"!!!!!\n";
#endif
    
	//set mode of animation to rotation
	animation.animation_mode = 1;
	//restart the animation
	animation.ResetAnimationPath();
	animation.Restart(eventMeshes.worldScale);
	

}

void IwGxScene::UpdateProtonAnimateEventButton(){
	if (!protonAnimationEventButton){
		//marco CIwGameError::LogError("Warning IwGxScene::UpdateProtonAnimateEventButton(), button not present!");
		return;
	}
	static bool isPressed = false;
	//is unchanged then do nothing
	if (protonAnimationEventButton->isSelected()== isPressed) return;
	//update the bool
	else isPressed = protonAnimationEventButton->isSelected();
	//start an animation
#ifdef _DEBUG
	std::cout <<" !!!!! re-starting animation with toBeCleaned.size() = "<<toBeCleaned.size()<<"!!!!!\n";
#endif
	//set mode of animation to follow the proton
	animation.animation_mode = 2;
	//restart the animation
	animation.ResetAnimationPath();
	animation.Restart(eventMeshes.worldScale);
	

}



void IwGxScene::UpdateShowDetectorsButton(){
	if (!detectorVisiblityButton){
		//marco CIwGameError::LogError("Warning IwGxScene::UpdateShowDetectorButton(), button not present!");
		return;
	}
	if (detectorVisiblityButton->isSelected()) showDetectors = false;
	else showDetectors = true;
}


void IwGxScene::UpdateFacebookButton(){
	//static bool isPosting = false;

	if (!facebookButton){
		//marco CIwGameError::LogError("Warning IwGxScene::UpdateFacebookButton(), button not present!");
		return;
	}
	if (facebookButton->isSelected() /*&& !isPosting*/){
		//user wants to post to facebook
		char * msg = "Something about Physics.";
		char * linkUri = "http://www2.physics.ox.ac.uk/about-us/outreach/public/lhsee";
		char * imgUri = "http://www-pnp.physics.ox.ac.uk/~boddy/files/appicon96.png";
		char * msgTitle="TestTitle";
		char * desc = "TestDescription";
		char * name = "TestName";
		std::cout <<"!!!!!!!!!!!!! About to make FB post !!!!!!!!!!!!\n";
		//CIwGameFacebook::getInstance()->PostWall(msg,linkUri,imgUri,name,desc);
		//CIwGameFacebook::getInstance()->PostWall(msg,linkUri,imgUri,name,desc);

      std::cout << "Marco: PLATFORM_FACEBOOK not implemented" << std::endl;
      /*PLATFORM_FACEBOOK->PostWall(
			"Something about Physics.",
			"http://www2.physics.ox.ac.uk/about-us/outreach/public/lhsee",
			"http://www-pnp.physics.ox.ac.uk/~boddy/files/appicon96.png",
			"TestName",
			"TestDescription");*/
		//isPosting = true;
	}
	
	return;

}

void IwGxScene::UpdateEndcapButton(){
	if (!endcapButton){
		//marco CIwGameError::LogError("Warning IwGxScene::UpdateProjectionButton(), button not present!");
		return;
	}
	static bool prevSelected = false;
	//update bool
	transverseProjectionMode = endcapButton->isSelected();
	//button state has not changed, button remains switched off
	if (!prevSelected &&  !endcapButton->isSelected()) return;

	//previously off, not switched on
	if (!prevSelected && endcapButton->isSelected()){
		
		prevSelected = endcapButton->isSelected();
		//turn off drawing of the endcap detectors
		meshes.UpdateMeshesToDraw(EndcapFromXML::pix_hash,false);
		meshes.UpdateMeshesToDraw(EndcapFromXML::sct_hash,false);
		meshes.UpdateMeshesToDraw(EndcapFromXML::trt_hash,false);
		meshes.UpdateMeshesToDraw(EndcapFromXML::ecal_hash,false);
		meshes.UpdateMeshesToDraw(EndcapFromXML::hcal_hash,false);
		meshes.UpdateMeshesToDraw(EndcapFromXML::muon_hash,false);
		
		return;
	}
	//previously on, now switched off
	if (prevSelected && !endcapButton->isSelected()){
		prevSelected = endcapButton->isSelected();
		
		//turn ON drawing of the endcap detectors
		meshes.UpdateMeshesToDraw(EndcapFromXML::pix_hash,true);
		meshes.UpdateMeshesToDraw(EndcapFromXML::sct_hash,true);
		meshes.UpdateMeshesToDraw(EndcapFromXML::trt_hash,true);
		meshes.UpdateMeshesToDraw(EndcapFromXML::ecal_hash,true);
		meshes.UpdateMeshesToDraw(EndcapFromXML::hcal_hash,true);
		meshes.UpdateMeshesToDraw(EndcapFromXML::muon_hash,true);
		return;
	}
	return;
}

void IwGxScene::UpdateTransverseButton() {
    
    if (!transverseButton){
		//marco CIwGameError::LogError("Warning IwGxScene::UpdateResetButton(), button not present!");
		return;
	}
	static bool isPressed = false;
	//is unchanged then do nothing
	if (transverseButton->isSelected() == isPressed) return;
	//update the bool
	else isPressed = transverseButton->isSelected();
    modelMatrix->SetIdentity();
    modelMatrix->PreRotateY((float)PI / 2.0);
    
    return;
}

void IwGxScene::Draw()
{
#ifdef _DEBUG
	CIwGameError::LogError("starting GamesceneGL::Draw()");
#endif
		
	//IwGxFlush();

	
	//These are necessary, something FUBAR happens in IwGxDraw but this solves it
	//static CIwFMat viewMatrix;
	
	//viewMatrix.SetIdentity();
	

	//IwGxLightingOff();
	/*glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, 0);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisable(GL_ALPHA_TEST);:
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);*/
	
	//IwGxSetColStream(0,0);
	
	if (pMat==NULL) pMat = new CIwMaterial;
	//static CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
	
	
//
//	static int16 s = 40;
	/*
	static CIwFVec3 s_Vecs[4] =
	{
	CIwFVec3(-s, -s, 0),
	CIwFVec3(-s, s, 0),
	CIwFVec3( s, -s, 0),
	CIwFVec3( s, s, 0),
	};*/

	/*
	static CIwSVec2 s_Vecs[4] =
	{
	CIwSVec2((int16)IwGxGetScreenWidth()/2,(int16)IwGxGetScreenHeight()/2),
	CIwSVec2((int16)IwGxGetScreenWidth()/2,(int16)IwGxGetScreenHeight()),
	CIwSVec2((int16)IwGxGetScreenWidth(),(int16)IwGxGetScreenHeight()),
	CIwSVec2((int16)IwGxGetScreenWidth(),(int16)IwGxGetScreenHeight()/2),

	};
	*/
//	
//	static CIwFVec3 s_Vecs[4] =
//	{
//	
//		CIwFVec3(0,0,4),
//		CIwFVec3(0,1.0,4),
//		CIwFVec3(1.0,1.0,4),
//		CIwFVec3(1.0,0,4),
//	
//	};
//	static CIwFVec3 s_Vecs2[4] =
//	{
//	
//		CIwFVec3(0.5,0.5,4),
//		CIwFVec3(0.5,1.0,4),
//		CIwFVec3(1.0,1.0,4),
//		CIwFVec3(1.0,0.5,4),
//	
//	};
//
//
//	static uint16 s_Inds[4] = {0, 1, 2, 3};
//	
//
//	static CIwColour s_Cols[8] =
//{
//    /*{0xff, 0x00, 0x00, 0x00},
//    {0xff, 0xff, 0x00, 0x00},
//    {0x00, 0xff, 0xff, 0x00},
//    {0x00, 0x00, 0xff, 0x00},
//    {0xff, 0x00, 0xff, 0x00},
//    {0xff, 0x00, 0x00, 0x00},
//    {0xff, 0xff, 0x00, 0x00},
//    {0x00, 0xff, 0xff, 0x00},*/
//	{0xff, 0x00, 0x00, 0xff},
//    {0x00, 0xff, 0x00, 0xff},
//    {0x00, 0x00, 0x00, 0xff},
//
//};

	//IwGxSetPerspMul(0xa0);
    //IwGxSetFarZNearZ(0x1000, 0x10);

	//modelMatrix.SetIdentity();
	//viewMatrix.SetIdentity();

	//IwGxSetPerspMul(0);
	//IwGxSetFarZNearZ(10,1);

	//viewMatrix.t.x = -0xc0;
    //viewMatrix.t.y = -0xc0;

	//IwGxSetViewMatrix	(& viewMatrix);
	
	
	//IwGxSetScreenSpaceO0rg(&CIwSVec2::g_Zero);
    //IwGxSetScreenSpaceSlot(-1);
	//IwGxFlush();
	//IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

	IwGxSetMaterial(pMat);
	
	//IwGxSetVertStreamScreenSpace(s_Vecs, 3);
    
    IwGxSetOrtho(-(*zoom)*glScreenWidth,(*zoom)*glScreenWidth,-(*zoom)*glScreenHeight,(*zoom)*glScreenHeight);
	//IwGxSetOrtho(-1,1,-1,1,0.001,50);
	
    //IwGxSetOrtho(-zoom*1000,zoom*1000,-zoom*1000,zoom*1000);
    
	//IwGxSetColStream(s_Cols, 3);
	//static CIwColour red, green;
	//pMat->SetAlphaMode(CIwMaterial::BLEND);
	pMat->SetAlphaMode(CIwMaterial::ALPHA_ADD);
	pMat->SetCullMode(CIwMaterial::CULL_BACK);

//    pMat->SetCullMode(CIwMaterial::CULL_BACK);
	//red.Set(255,0,0,100);
	//green.Set(0,255,0,255);
	/*
	IwGxSetColAmbient(green);
	IwGxSetVertStream(s_Vecs2, 3);
	IwGxDrawPrims(IW_GX_TRI_LIST, NULL, 3);
	
	
	IwGxSetColAmbient(red);
	IwGxSetVertStream(s_Vecs, 3);
	IwGxDrawPrims(IW_GX_TRI_LIST, NULL, 3);
	*/
	

	//IwGxFlush();
	//IwGxSwapBuffers();
	//IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);
	
    //set the viewport
	/*
	glViewport(0, 0, glScreenWidth, glScreenHeight);
	
	
	glMatrixMode(GL_MODELVIEW);
	glClearDepthf(1.0);
    glLoadIdentity();
	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glClearDepthf(1.0);
    glLoadIdentity();


	

	
	//orthogonal projection
	if (projectionMode==0) orthogonalDraw();
	//perspective 
	else perspectiveDraw();
	*/
	if (projectionMode==0) orthogonalDraw();
	
	CzScene::Draw();
	return;

	
}

/* Draw mode for orthogonal viewing */
/*void IwGxScene::orthogonalDraw(){
	glOrthof(-1,1,-1,1,-10,10);
	
	glScalef(growthFactorFromInput,growthFactorFromInput,growthFactorFromInput);
	
	glTranslatef(linearScaleFactor.x,linearScaleFactor.y,linearScaleFactor.z);
	
	
	glRotatef(meshes.rot.x, 1, 0, 0);
    glRotatef(meshes.rot.y, 0, 1, 0);
    glRotatef(meshes.rot.z, 0, 0, 1);
	

	if (showDetectors) meshes.Draw();
	eventMeshes.Draw();
}*/

void IwGxScene::orthogonalDraw(){
	/*glOrthof(-1,1,-1,1,-10,10);
	
	glScalef(growthFactorFromInput,growthFactorFromInput,growthFactorFromInput);
	
	glTranslatef(linearScaleFactor.x,linearScaleFactor.y,linearScaleFactor.z);
	
	
	glRotatef(meshes.rot.x, 1, 0, 0);
    glRotatef(meshes.rot.y, 0, 1, 0);
    glRotatef(meshes.rot.z, 0, 0, 1);
	*/
	static CIwFMat scaleMatrix;
	scaleMatrix.SetIdentity();
	
	//scaleMatrix.SetTrans();
	
	//modelMatrix.SetIdentity();
    static CIwFMat rotMatrix;
    rotMatrix.SetIdentity();
    rotMatrix.PreRotateX(-1*rotY);
    rotMatrix.PreRotateY(rotX);
    rotMatrix.PreRotateZ(rotZ);
    //rotMatrix.PreRotateZ(meshes.rot.z);
    
    modelMatrix->PostMult(rotMatrix);
    
//	meshes.rotX.SetRotX(meshes.rot.x);
//	meshes.rotY.SetRotY(meshes.rot.y);
//	meshes.rotZ.SetRotZ(meshes.rot.z);
//	
//	modelMatrix = meshes.rotX * meshes.rotY * meshes.rotZ;
	
	

//	modelMatrix.t.x = meshes.pos.x;
//	modelMatrix.t.y = meshes.pos.y;
//	modelMatrix.t.z = meshes.pos.z;

	//modelMatrix.Scale(growthFactorFromInput);
	//modelMatrix.ScaleTrans(growthFactorFromInput);
	//scale by growthFactorFromInput
	//modelMatrix.
    //float scale = 4;
//    int16 screenw = (int16)IwGxGetScreenWidth();
//    int16 screenh = (int16)IwGxGetScreenHeight();
//    CIwGameError::LogError("w = ",CIwGameString(screenw).c_str());
//    CIwGameError::LogError("h = ",CIwGameString(screenh).c_str());
    
    IwGxSetFarZNearZ(3000, 0.1);
    modelMatrix->SetTrans(CIwFVec3(0,0,1500));
    
	IwGxSetModelMatrix	(modelMatrix);
	//IwGxMultMatrix();


	if (showDetectors) meshes.Draw();
	eventMeshes.Draw();
    

//    IwGxPrintString(5, 8, CIwGameString(zoom).c_str());
//    IwGxPrintString(5, 16, CIwGameString(rotX).c_str());
//    IwGxPrintString(5, 24, CIwGameString(rotY).c_str());
}


/* Draw mode for pespective viewing */
void IwGxScene::perspectiveDraw(){
	
	ChrisGluLookAt_v3(&camera,&meshes.pos,&up_vec);
	
	glRotatef(meshes.rot.x, 1, 0, 0);
    glRotatef(meshes.rot.y, 0, 1, 0);
    glRotatef(meshes.rot.z, 0, 0, 1);
	
	glTranslatef(meshes.pos.x, meshes.pos.y, meshes.pos.z);

	if (showDetectors) meshes.Draw();
	eventMeshes.Draw();
}


void IwGxScene::HandleInputs(){
	//is user allowed to modify scene with touch input?
	if (!allowUserInput) return;
    
    if(menu_active->NativeValue || game_active->NativeValue || live_active->NativeValue) {
    
        this->findActor("pauseLabel")->setVisible(true);
        return;
    }
    this->findActor("pauseLabel")->setVisible(false);
    
    // Check to make sure that the pointer is available
    if (CZ_INPUT->isPointerAvailable())
    {
        // Get the total number of current touches
        int num_touches = CZ_INPUT->getTouchCount();
        if (num_touches != 0)
        {
            if(num_touches == 1){
                CzTouch* touch = CZ_INPUT->getTouch(0);
                if (touch->active) {
                    // Get the touch data
                    
                    // define the rotation axis
                    //CIwFVec3 rotationAxis(touch->dx,touch->dy,0);
                    // ensure the axis is normal
                    //rotationAxis.Normalise();
                    //viewMatrix.SetIdentity();
                    //viewMatrix.RotateVec(CIwFVec3(touch->dy, touch->dx, 0));
                    if(!CZ_INPUT->isMultiTouch() && touch->y > IwGxGetScreenHeight()*0.95)
                        (*zoom) = 0.05 + (4*((IwGxGetScreenWidth() - touch->x) / (float)IwGxGetScreenWidth()));
                    else{
                        rotY=touch->dy/100.f;
                        rotX=touch->dx/100.f;
                    }
                    //viewMatrix.SetTrans(CIwFVec3(0,0,128));
                    //IwGxSetModelMatrix( &viewMatrix );
                    
                }
                
            }
            if (num_touches == 2) {
                CzTouch* touch_0 = CZ_INPUT->getTouch(0);
                CzTouch* touch = CZ_INPUT->getTouch(1);
                if(touch->active) {
                    
                    if(oldrotZ==0){
                        oldrotZ = atan2(touch_0->y - touch->y, touch_0->x - touch->x);
                    }
                    else{
                        
                        float angle = atan2(touch_0->y - touch->y, touch_0->x - touch->x);
                        rotZ = oldrotZ - angle;
                        oldrotZ = angle;
                    }
                    
                    if(oldpinch==0){
                        oldpinch = sqrtf((touch_0->x - touch->x)*(touch_0->x - touch->x) + (touch_0->y - touch->y)*(touch_0->y - touch->y));
                    }
                    else{
                        pinch = sqrtf((touch_0->x - touch->x)*(touch_0->x - touch->x) + (touch_0->y - touch->y)*(touch_0->y - touch->y));
                        
                        if((*zoom) <= maxZoom && (*zoom) >= minZoom)
                            (*zoom) /= pinch/oldpinch;
                        else if((*zoom) > maxZoom)
                            (*zoom) = maxZoom;
                        else if((*zoom) < minZoom)
                            (*zoom) = minZoom;
                        
                        oldpinch = pinch;
                        
                        //CIwFMat scaleMatrix;
                        //scaleMatrix.SetIdentity();
                        //scaleMatrix.Scale(zoom);
                        
                        //viewMatrix.PostMult(scaleMatrix);
                        //                        float trans = viewMatrix.GetTrans().z;
                        //                        if(trans_z/zoom < 256 && trans_z/zoom > 80) {
                        //                            trans_z /= zoom;
                        //                        }
                        
                        //                        if(zoom < maxZoom && zoom > minZoom)
                        //                            viewMatrix.Scale(zoom);
                        
                        //viewMatrix.ScaleTrans(zoom);
                        //viewMatrix.ScaleTrans(distance);
                    }
                }
            }
            
        }
        else{
            oldrotZ = 0;
            oldpinch = 0;
        }
        
    }
    
    
//	static float prevdiff2 = 1.0;
//	static CIwFVec2 prev_touch_prev, touch_prev;
//
//	if (IW_GAME_INPUT->isPointerAvailable()){
//		// Get the total number of current touches
//		int num_touches = IW_GAME_INPUT->getTouchCount();
//		if (num_touches != 0){
//			// Check list of touches to see which are active
//			for (int t = 0; t < IW_GAME_MAX_TOUCHES; t++){
//				// Get the touch data
//				CIwGameTouch* touch = IW_GAME_INPUT->getTouch(t);
//				if (touch->active){
//					//Print locations
//					//CIwGameError::LogError("touch #: ",CIwGameString(t).c_str());
//					if (t==0){
//						//cannot pan around in transverse projection mode
//						if (transverseProjectionMode) continue;
//						
//							//orthogonal projection
//						if (projectionMode==0){
//							meshes.rot.y -=(float)touch->dx/50.f;
//							meshes.rot.x -= (float)touch->dy/50.f;
//                            rotX = (float)touch->dx/50.f;
//                            rotY = (float)touch->dy/50.f;
//						}
//						//perspective projection
//						else{
//						camera.x -= (float)touch->dx/(float)OriginalX ;
//						camera.y -= (float)touch->dy/(float)OriginalY ;
//						}
//						//meshes.rot.x -= (float)touch->dx;
//						//meshes.rot.y -= (float)touch->dy;
//					}
//					//multitouch zoom
//					if (t==1){
//
//						
////						//previous touch
////						CIwGameTouch* previous_touch = IW_GAME_INPUT->getTouch(0);
////						//nerf previous rotation, zoom OR rotate but both simultaneously is odd
////						if (projectionMode==0 && !transverseProjectionMode){
////							meshes.rot.y +=(float)previous_touch->dx/50.f;
////							meshes.rot.x += (float)previous_touch->dy/50.f;
////						}
////						
////
////						float touch_width = sqrtf( (touch->x - previous_touch->x)*(touch->x - previous_touch->x) + (touch->y - previous_touch->y)*(touch->y - previous_touch->y));
////
////						prev_touch_prev.x = previous_touch->dx - previous_touch->x;
////						prev_touch_prev.y = previous_touch->dy - previous_touch->y;
////
////						touch_prev.x = touch->dx - touch->x;
////						touch_prev.y = touch->dy - touch->y;
////						
////						float prev_touch_width = sqrtf((touch_prev.x - prev_touch_prev.x)*(touch_prev.x - prev_touch_prev.x) + (touch_prev.y - prev_touch_prev.y)*(touch_prev.y - prev_touch_prev.y));
////						float growth = touch_width/prev_touch_width;
////#ifdef _DEBUG	
////						CIwGameError::LogError("HandleInputs, growth = ",CIwGameString(growth).c_str());
////						CIwGameError::LogError("HandleInputs, meshes.pos.z = ",CIwGameString(meshes.pos.z).c_str());
////#endif
////						if (growth>1.f) {
////							if (growthFactorFromInput < zoomMax) growthFactorFromInput += growth/20.f;
////							
////						}
////						else {
////							if (growthFactorFromInput - 1.f/growth/20.f > zoomMin) growthFactorFromInput -= 1.f/growth/20.f;
////						}
//                        
//                        
//                        
//						
//						
//					}
//				}
//			}
//		}
//	}

}



int IwGxScene::LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node)
{
	// Load the actor from XOML
	if (!CzScene::LoadFromXoml(parent, load_children, node))
		return false;

	// Process InertActor specific attributes
	for (CzXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

	}

	return true;
}

void IwGxScene::StreamEventFromCERN(){



}

void IwGxScene::StreamFromHTTP(const std::string& url) {
    
    if(next_ev->NativeValue)
        eventManager->nextEvent();
    if(prev_ev->NativeValue)
        eventManager->prevEvent();
    
    next_ev->setValue("false");
    prev_ev->setValue("false");
    
    std::cout << "set ev " << set_ev->NativeValue << std::endl;
    
    if(set_ev->NativeValue != -1){
        std::cout << set_ev->NativeValue << std::endl;
        eventManager->setEvent(set_ev->NativeValue);
        set_ev->NativeValue = -1;
        std::cout << set_ev->NativeValue << std::endl;
    }
    
    std::string ev = eventManager->getCurrentEvent();
    std::cout << ev << std::endl;
    
    LoadThisFile(ev.c_str());
}

//iOS runs the GL viewport opposite to Android :-#
//so here we say if running iOS do the opposite to everything else with the glViewport dimensions
//welcome to cross-platform native app programming.§
void IwGxScene::setScreenParams(){

    
        glScreenWidth = (int16)IwGxGetScreenWidth();
        glScreenHeight = (int16)IwGxGetScreenHeight();
		
//		//check OS type
//		int os = s3eDeviceGetInt(S3E_DEVICE_OS);
//
//		if (os == S3E_OS_ID_IPHONE){
//			glScreenWidth = IwGLGetInt(IW_GL_HEIGHT);
//			glScreenHeight = IwGLGetInt(IW_GL_WIDTH);
//		}
//		else{
//			glScreenWidth = IwGLGetInt(IW_GL_WIDTH);
//			glScreenHeight = IwGLGetInt(IW_GL_HEIGHT);
//
//		}

}