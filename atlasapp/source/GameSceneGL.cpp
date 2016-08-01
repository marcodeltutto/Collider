#include "IwGxScene.h"

#include "IwGameCamera.h"
#include "IwGameInput.h"
#include "IwGame.h"
#include "My3DUtils.h"
#include "ColourStore.h"
#include "IwGameUI.h"
#include "IwGameUIListBox.h"

#include "IwImage.h"
//
//
//
// IwGxScene implementation
//
//
//

int IwGxScene::Init(int max_collidables, int max_layers, bool doSleep)
{
	
	CIwGameScene::Init(max_collidables, max_layers, doSleep);

	//event
	currentEvent = NULL;
	//hud scene
	primitives_listBox = NULL;
	projectionButton = NULL;
	detectorVisiblityButton = NULL;
	animateEventButton = NULL;
	InitHUDLinks();
	//are we looking in the transverse plane?
	transverseProjectionMode = false;
	showDetectors = true;
	
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
	
	//Animation

	
	//projection
	projectionMode=0;
	glMatrixMode(GL_MODELVIEW);
	glClearDepthf(1.0);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glClearDepthf(1.0);
    glLoadIdentity();
	//glFrustumf(-0.5,0.5,-0.5,0.5,0.1,10.0);
    glDisable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
	setScreenParams();
	

	return 1;
}

void IwGxScene::InitHUDLinks(){
	hudScene = Parent->findScene("HUDScene");
	
	if (!hudScene){
		CIwGameError::LogError("Warning IwGxScene::InitHUDLinks() cannot find HUDScene");
		return;
	}
	//sliders variable bindings
	red_value = (CIwGameXomlVariableFloat*) hudScene->getVariableManager()->findVariable("slider_red_value");
	green_value = (CIwGameXomlVariableFloat*) hudScene->getVariableManager()->findVariable("slider_green_value");
	blue_value = (CIwGameXomlVariableFloat*) hudScene->getVariableManager()->findVariable("slider_blue_value");
	alpha_value = (CIwGameXomlVariableFloat*) hudScene->getVariableManager()->findVariable("slider_alpha_value");
	//sliders
	red_slider = (CIwGameUISlider*) hudScene->findActor("slider_red");
	green_slider = (CIwGameUISlider*) hudScene->findActor("slider_green");
	blue_slider = (CIwGameUISlider*) hudScene->findActor("slider_blue");
	alpha_slider = (CIwGameUISlider*) hudScene->findActor("slider_alpha");
	//listBox
	primitives_listBox = (CIwGameUIListBox*) hudScene->findActor("PrimitivesListBox");
	//button for orthogonal projection
	projectionButton = (CIwGameUILabel*) hudScene->findActor("projectionbutton");
	detectorVisiblityButton = (CIwGameUILabel*) hudScene->findActor("detectorVisiblityButton");
	animateEventButton = (CIwGameUILabel*) hudScene->findActor("animateEventButton");
	protonAnimationEventButton = (CIwGameUILabel*) hudScene->findActor("protonAnimateEventButton");

}
void IwGxScene::UpdateColorHUD(){
	if (!primitives_listBox){
		CIwGameError::LogError("Warning IwGxScene::UpdateColorHUD() cannot find HUDScene ListBox");
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
	else if (index==3) red_in_map = name_color_map[ACylinderFromXML::trtName_hash];
	else if (index==4) red_in_map = &colours.rgba_ECAL[0];
	else if (index==5) red_in_map = &colours.rgba_HCAL[0];
	else if (index==6) red_in_map = name_color_map[ACylinderFromXML::muonDet_hash];

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
		red_value->setValue(CIwGameString(*(red_in_map)).c_str());
		green_value->setValue(CIwGameString(*(red_in_map+1)).c_str());
		blue_value->setValue(CIwGameString(*(red_in_map+2)).c_str());
		alpha_value->setValue(CIwGameString(*(red_in_map+3)).c_str());
		//update the sliders too, bug in Engine I think!
		red_slider->setValue(*(red_in_map));
		green_slider->setValue(*(red_in_map+1));
		blue_slider->setValue(*(red_in_map+2));
		alpha_slider->setValue(*(red_in_map+3));
		
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
	name_color_map[ACylinderFromXML::trtName_hash] = &colours.rgba_TRT[0];
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
	
	//endcap colours	
	name_color_map[EndcapFromXML::pix_hash] = &colours.rgba_Pix[0];
	name_color_map[EndcapFromXML::sct_hash] = &colours.rgba_Pix[0];
	name_color_map[EndcapFromXML::trt_hash] = &colours.rgba_TRT[0];
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
	
	growthFactorFromInput = 1.0;
	zoomMin = 0.15;
	zoomMax = 10.0;
	
	meshes.Init();
	eventMeshes.Init();

	//meshes.pos = CIwFVec3(0.0,0.0,0.0); 
	//meshes.worldScale = 500.0f;
	meshes.pos = CIwFVec3(0.0,0.0,0.5); 
	meshes.worldScale = 500.0f;

	eventMeshes.pos = CIwFVec3(0.0,0.0,0.5); 
	eventMeshes.worldScale = 500.0f;

	

	//GLDonut * glDonut = new GLDonut();
	
	//glDonut->Init(0.5,-0.5,0.7,0.4,10,meshes.worldScale);
	//glDonut->setRGBA(colours.rgba_SCT);
	
	InitColourMap();
	/*
	meshes.children->push_back(glCube);
	meshes.children->push_back(glCylinder);
	meshes.children->push_back(glDonut);
	*/
	//LoadThisFile("test_event_2tracks.xml");

	//load detectors
	IW_GAME_XOML->Process(this,"test_detectors.xml",false);
	XOMLcleanup();
	//load test event
	//LoadThisFile("test_event_2tracks.xml");
	//LoadThisFile("test_event_1muonTrack.xml");
	LoadThisFile("test_event.xml");
	//load event list from CERN
	//LoadThisFile("http://www-pnp.physics.ox.ac.uk/~boddy/files/files.xml");
	//init the animation
	animation.Init(eventMeshes.worldScale,this);
#ifdef _DEBUG
	std::cout <<" !!!!! starting animation in init with toBeCleaned.size() = "<<toBeCleaned.size()<<"!!!!!\n";
#endif

}

void IwGxScene::Update(float dt)
{

	//check for touches
	HandleInputs();
	CIwGameScene::Update(dt);
	//get slider values for colours
	UpdateColorHUD();
	//are we moving to transverse projection mode?
	UpdateProjectionButton();
	//show / hide detector button pressed?
	UpdateShowDetectorsButton();
	//restart animation button pressed?
	UpdateAnimateEventButton();
	//restart proton animation button pressed?
	UpdateProtonAnimateEventButton();
	//update scene animation
	if (animation.isPlaying) animation.UpdateAnimation(&meshes.rot, growthFactorFromInput);

	
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

void IwGxScene::UpdateAnimateEventButton(){
	if (!animateEventButton){
		CIwGameError::LogError("Warning IwGxScene::UpdateAnimateEventButton(), button not present!");
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
		CIwGameError::LogError("Warning IwGxScene::UpdateProtonAnimateEventButton(), button not present!");
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
		CIwGameError::LogError("Warning IwGxScene::UpdateShowDetectorButton(), button not present!");
		return;
	}
	if (detectorVisiblityButton->isSelected()) showDetectors = false;
	else showDetectors = true;

}
void IwGxScene::UpdateProjectionButton(){
	if (!projectionButton){
		CIwGameError::LogError("Warning IwGxScene::UpdateProjectionButton(), button not present!");
		return;
	}
	static bool prevSelected = false;
	static CIwFVec3 moveAngleDelta = CIwFVec3(0.5,0.5,0.5);
	//update bool
	transverseProjectionMode = projectionButton->isSelected();
	//button state has not changed, button remains switched off
	if (!prevSelected &&  !projectionButton->isSelected()) return;

	//previously off, not switched on
	if (!prevSelected && projectionButton->isSelected()){
		
		prevSelected = projectionButton->isSelected();
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
	if (prevSelected && !projectionButton->isSelected()){
		prevSelected = projectionButton->isSelected();
		
		//turn ON drawing of the endcap detectors
		meshes.UpdateMeshesToDraw(EndcapFromXML::pix_hash,true);
		meshes.UpdateMeshesToDraw(EndcapFromXML::sct_hash,true);
		meshes.UpdateMeshesToDraw(EndcapFromXML::trt_hash,true);
		meshes.UpdateMeshesToDraw(EndcapFromXML::ecal_hash,true);
		meshes.UpdateMeshesToDraw(EndcapFromXML::hcal_hash,true);
		meshes.UpdateMeshesToDraw(EndcapFromXML::muon_hash,true);
		return;
	}

	//button remains on
	if (prevSelected && projectionButton->isSelected()){
		//update x angle
		if (fabsf(meshes.rot.x) < 1.5) meshes.rot.x=0;
		else if (meshes.rot.x > 0) meshes.rot.x -= moveAngleDelta.x;
		else meshes.rot.x +=moveAngleDelta.x;
		//update z angle
		if (fabsf(meshes.rot.z) < 1.5) meshes.rot.z=0;
		else if (meshes.rot.z > 0) meshes.rot.z -= moveAngleDelta.z;
		else meshes.rot.z +=moveAngleDelta.z;
		//update y angle
		
		if (fabsf(meshes.rot.y) >88 && fabsf(meshes.rot.y) < 92) meshes.rot.y=90;
		else if (meshes.rot.y > 90) meshes.rot.y -= moveAngleDelta.y;
		else meshes.rot.y +=moveAngleDelta.y;
		
	}

	
	return;
}

void IwGxScene::Draw()
{
#ifdef _DEBUG
	CIwGameError::LogError("starting GamesceneGL::Draw()");
#endif
		
	IwGxFlush();
	//These are necessary, something FUBAR happens in IwGxDraw but this solves it
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, 0);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	

    //set the viewport
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
	
	CIwGameScene::Draw();
	
}

/* Draw mode for orthogonal viewing */
void IwGxScene::orthogonalDraw(){
	glOrthof(-1,1,-1,1,-10,10);
	
	glScalef(growthFactorFromInput,growthFactorFromInput,growthFactorFromInput);
	
	glTranslatef(linearScaleFactor.x,linearScaleFactor.y,linearScaleFactor.z);
	
	
	glRotatef(meshes.rot.x, 1, 0, 0);
    glRotatef(meshes.rot.y, 0, 1, 0);
    glRotatef(meshes.rot.z, 0, 0, 1);

	if (showDetectors) meshes.Draw();
	eventMeshes.Draw();
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
	static float prevdiff2 = 1.0;
	static CIwFVec2 prev_touch_prev, touch_prev;

	if (IW_GAME_INPUT->isPointerAvailable()){
		// Get the total number of current touches
		int num_touches = IW_GAME_INPUT->getTouchCount();
		if (num_touches != 0){
			// Check list of touches to see which are active
			for (int t = 0; t < IW_GAME_MAX_TOUCHES; t++){
				// Get the touch data
				CIwGameTouch* touch = IW_GAME_INPUT->getTouch(t);
				if (touch->active){
					//Print locations
					//CIwGameError::LogError("touch #: ",CIwGameString(t).c_str());
					if (t==0){
						//cannot pan around in transverse projection mode
						if (transverseProjectionMode) continue;
						
							//orthogonal projection
						if (projectionMode==0){
							meshes.rot.y -=(float)touch->dx/5.f;
							meshes.rot.x -= (float)touch->dy/5.f;
						}
						//perspective projection
						else{
						camera.x -= (float)touch->dx/(float)OriginalX ;
						camera.y -= (float)touch->dy/(float)OriginalY ;
						}
						//meshes.rot.x -= (float)touch->dx;
						//meshes.rot.y -= (float)touch->dy;
					}
					//multitouch zoom
					if (t==1){

						
						//previous touch
						CIwGameTouch* previous_touch = IW_GAME_INPUT->getTouch(0);
						//nerf previous rotation, zoom OR rotate but both simultaneously is odd
						if (projectionMode==0 && !transverseProjectionMode){
							meshes.rot.y +=(float)previous_touch->dx/5.f;
							meshes.rot.x += (float)previous_touch->dy/5.f;
						}
						

						float touch_width = sqrtf( (touch->x - previous_touch->x)*(touch->x - previous_touch->x) + (touch->y - previous_touch->y)*(touch->y - previous_touch->y));

						prev_touch_prev.x = previous_touch->dx - previous_touch->x;
						prev_touch_prev.y = previous_touch->dy - previous_touch->y;

						touch_prev.x = touch->dx - touch->x;
						touch_prev.y = touch->dy - touch->y;
						
						float prev_touch_width = sqrtf((touch_prev.x - prev_touch_prev.x)*(touch_prev.x - prev_touch_prev.x) + (touch_prev.y - prev_touch_prev.y)*(touch_prev.y - prev_touch_prev.y));
						float growth = touch_width/prev_touch_width;
#ifdef _DEBUG	
						CIwGameError::LogError("HandleInputs, growth = ",CIwGameString(growth).c_str());
						CIwGameError::LogError("HandleInputs, meshes.pos.z = ",CIwGameString(meshes.pos.z).c_str());
#endif
						if (growth>1.f) {
							if (growthFactorFromInput < zoomMax) growthFactorFromInput += growth/20.f;
							
						}
						else {
							if (growthFactorFromInput - 1.f/growth/20.f > zoomMin) growthFactorFromInput -= 1.f/growth/20.f;
						}
						
						
						
					}
				}
			}
		}
	}

}



bool IwGxScene::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Load the actor from XOML
	if (!CIwGameScene::LoadFromXoml(parent, load_children, node))
		return false;

	// Process InertActor specific attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

	}

	return true;
}

void IwGxScene::StreamEventFromCERN(){




}

//iOS runs the GL viewport opposite to Android :-#
//so here we say if running iOS do the opposite to everything else with the glViewport dimensions
//welcome to cross-platform native app programming.
void IwGxScene::setScreenParams(){

		
		//check OS type
		int os = s3eDeviceGetInt(S3E_DEVICE_OS);

		if (os == S3E_OS_ID_IPHONE){
			glScreenWidth = IwGLGetInt(IW_GL_HEIGHT);
			glScreenHeight = IwGLGetInt(IW_GL_WIDTH);
		}
		else{
			glScreenWidth = IwGLGetInt(IW_GL_WIDTH);
			glScreenHeight = IwGLGetInt(IW_GL_HEIGHT);

		}

}