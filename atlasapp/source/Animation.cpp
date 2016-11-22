#include "Animation.h"


#ifdef _DEBUG
	#include<iostream>
#endif


#ifndef MIN
    #define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
    #define MAX(a,b) (((a)>(b))?(a):(b))
#endif


void Animation::SetupPaths(){
	//steps in path
	//circular path
	addStep(0, 0, 0, 0.5, 4.0, &circular_path);
	addStep(0, 180/57.3, 120/57.3, 0.5 ,200,&circular_path,true);

	//proton path
	addStep(0,3.14,0,0.1,20,&proton_path);
	addStep(0, 3.14,0,1.0,30,&proton_path);
	addStep(0,3.14 ,0,5.0,30,&proton_path);
	addStep(3.14 ,0,0,3.0,30,&proton_path,true);
	addStep(3.14 ,0,0,0.5,30,&proton_path);
}




//worldScale converts the real-world coordinates to OpenGL coordinates
void Animation::Init(float worldScale, IwGxScene * _scene){
	//radius at which the scene will render to in cm
	drawRadius = 10.;
	deltaRadius = 5;

	drawRadius /=worldScale;
	deltaRadius /=worldScale;
	scene = _scene;
	currentEvent = _scene->currentEvent;
	animation_mode=1;
	SetupPaths();

	if (!scene || !currentEvent){
		//marco CIwGameError::LogError("Warning:: cannot init animation since event of scene == NULL");
		return;
	}

	idTracks = currentEvent->idTracks;
	muonTracks = currentEvent->muonTracks;

	ecalHisto = &currentEvent->eCalTowers;
	hcalHisto = &currentEvent->hCalTowers;

	if (idTracks) InitTrackAnimation(idTracks);
	if (muonTracks) InitTrackAnimation(muonTracks);

	if (ecalHisto) InitCaloTowersAnimation(ecalHisto);
	if (hcalHisto) InitCaloTowersAnimation(hcalHisto);

	isPlaying = true;
	eventIsPlaying = false;
}


/* restart the animation, reset the path parameters.*/
void Animation::ResetAnimationPath(int mode){
	
	if (mode<0) mode = this->animation_mode;

	std::vector<animationStep> * path = NULL;
	animationStep * step = NULL;
	if (mode==1) path= &circular_path;
	else if (mode==2) path= & proton_path;
	else return;
	for (int iStep=0;iStep<path->size();iStep++) path->at(iStep).currentStep = 0;
	
}


/*re-start an event animation */
void Animation::Restart(float worldScale){
	currentEvent = scene->currentEvent;

	if (!scene ||! currentEvent){
		//marco CIwGameError::LogError("Warning:: cannot init animation since event of scene == NULL");
		return;
	}

	idTracks = currentEvent->idTracks;
	muonTracks = currentEvent->muonTracks;
	
	//replace temporary vertices
	drawRadius=1e6;
	UpdateTracks(idTracks);
	UpdateTracks(muonTracks);
	
	
	drawRadius = 10./ worldScale;
	//tracks
	InitTrackAnimation(idTracks);
	InitTrackAnimation(muonTracks);

	//calo towers
	if (ecalHisto) InitCaloTowersAnimation(ecalHisto);
	if (hcalHisto) InitCaloTowersAnimation(hcalHisto);

	isPlaying = true;
	eventIsPlaying = true;

	//reset met arrow
	//if (currentEvent->metArrow)
	//	currentEvent->metArrow->scaleBy(1e-5);

}

//setup the track instance for animation
void Animation::InitTrackAnimation(Tracks * tracks){
	if (!tracks) {
		//marco CIwGameError::LogError("Warning:: cannot init tracks for animation");
		return;
	}
	
	
	tracks->lastDrawnTrackIndex.clear();
	tracks->totalDistanceFromPerigee.clear();
	tracks->totalScalarFromPerigee.clear();
	tracks->numPolyAnimate.clear();
	tracks->replaced_verts.clear();

	for (unsigned int iTrack=0;iTrack < tracks->numPoly.size();iTrack++){
	
		tracks->lastDrawnTrackIndex.push_back(0);
		tracks->totalDistanceFromPerigee.push_back(CIwFVec3(0,0,0));
		tracks->totalScalarFromPerigee.push_back(0.0f);
		tracks->numPolyAnimate.push_back(0);

		tracks->replaced_verts.push_back(std::pair<int,CIwFVec3> (-1,CIwFVec3(0,0,0)));
	}
	//init the insert/replace vertices
	tracks->NextUndrawnVertex = CIwFVec3(0,0,0); 
	tracks->LastReplacedVertex= CIwFVec3(0,0,0); 
	

}

void Animation::UpdateMET(){
	if (!scene ||! currentEvent){
		//marco CIwGameError::LogError("Warning:: cannot update MET in  since event of scene are NULL");
		return;
	}
	if (currentEvent->metArrow==NULL) return;

	//scale vertices by drawRadius, until this exceeds the maximumR
	GLArrow * met = currentEvent->metArrow;
	static float screenMaxR = met->maximumR / met->worldScale;
	if (drawRadius < screenMaxR)
		met->scaleBy(drawRadius);
	else return;
	

}
void Animation::UpdateEvent(){
	if (!scene ||! currentEvent){
		//marco CIwGameError::LogError("Warning:: cannot update animation since event of scene are NULL");
		return;
	}
    
    //if we have drawn far enough, stop the animation!
    if(drawRadius > 1250.0)
        isPlaying = false;
    
	//update the draw radius
	drawRadius += deltaRadius;

	//update the ID tracks
  if (!idTracks) std::cout << "Warning:: cannot update ID tracks animation since event has no ID tracks" << std::endl;
	else UpdateTracks(idTracks);

	//std::cout <<"about to update muon tracks\n";
	if (!muonTracks) std::cout << "Warning:: cannot update muon tracks animation since event has no muon tracks" << std::endl;
	else UpdateTracks(muonTracks);

	if (ecalHisto) UpdateCaloTowers(ecalHisto);
	if (hcalHisto) UpdateCaloTowers(hcalHisto);
}

void Animation::UpdateTracks(Tracks * tracks){
	//std::cout <<"Entering Animation::UpdateTracks(..)\n";
	//input: vertices:: vector of floats (x1,y1,z1,x2,y2,z2,etc.)
	//all tracks in same vector
	//input: numPoly:: vector of ints: number of poly points in each track
	std::vector<int> * numPoly = &tracks->numPoly;
    //std::cout << "KKKKKK" << numPoly->size() << std::endl;
	//running total of indices used for each track
	int globalTotal =0;
	//loop over tracks
	//number of poly points previous to this track
	int nPreviousPoints = 0;
	
	for (unsigned int iTrack =0;iTrack < numPoly->size();iTrack++){
		//numPoly.at(iTrack) is the total number of all indices in the track
		//regardless of whether theyre inside the draw volume
		
		
		//UpdateTrackIndex(tracks,globalTotal,iTrack,nPreviousPoints);
		UpdateTrackIndex_2(tracks,globalTotal,iTrack,nPreviousPoints);
		//find last point inside drawing volume
		globalTotal += (numPoly->at(iTrack)-1)*2;
		//update number of points in previous tracks
		nPreviousPoints += numPoly->at(iTrack);
	}
	
}


void Animation::UpdateTrackIndex(Tracks * tracks, int globalTotal, int iTrack, int nPreviousPoints){
	//is finished?
	

	//if (tracks->lastDrawnTrackIndex.at(iTrack) == globalTotal + (tracks->numPoly.at(iTrack)-1)*2) return;
	//last drawn index in the previous frame
	
	int lastDrawnTrackIndex = globalTotal + tracks->lastDrawnTrackIndex.at(iTrack);
	//last possible track index
	int lastTrackIndex = globalTotal+ (tracks->numPoly.at(iTrack)-1)*2;
	//distance from center to vertex
	static CIwFVec3 distanceFromPerigee;
	
//#ifdef _DEBUG
//	std::cout<<"update track "<<iTrack <<" glob. total = "<<globalTotal<<" lastTrackIndex "<<lastTrackIndex <<" lastDrawnTrackIndex = "<< lastDrawnTrackIndex<<" numPoly = "<<tracks->numPoly.at(iTrack)<<std::endl;
//#endif
	bool justAdded = false;
	//loop over remianing un-drawn indices to see if another(s) can be drawn to
	for (int iIndex = lastDrawnTrackIndex+2;iIndex <= lastTrackIndex;iIndex=iIndex + 2){
		//get vertex from index
		int iVert = (int) iIndex/2 +iTrack;
		
		distanceFromPerigee.x = tracks->polyX.at(iVert);
		distanceFromPerigee.y = tracks->polyY.at(iVert);
		distanceFromPerigee.z = tracks->polyZ.at(iVert);
		//number of previous poly points
		
		if (iVert - nPreviousPoints>0){
			distanceFromPerigee.x -= tracks->polyX.at(iVert-1);
			distanceFromPerigee.y -= tracks->polyY.at(iVert-1);
			distanceFromPerigee.z -= tracks->polyZ.at(iVert-1);
		}
		float distance = distanceFromPerigee.GetLength();
//#ifdef _DEBUG
		//std::cout <<"iTrack = "<< iTrack <<" poly point: "<<tracks->polyX.at(iVert)<<","<<tracks->polyY.at(iVert) <<","<<tracks->polyZ.at(iVert) <<" vertices *3 "<<tracks->vertices.at(iVert*3+1) <<","<<tracks->vertices.at(iVert*3+2) <<","<<tracks->vertices.at(iVert*3) <<" drawRadius = "<<drawRadius<<" total distance so far = "<<tracks->totalScalarFromPerigee.at(iTrack)+ distance<< std::endl;
//#endif	
		
		
		//if still inside volume then add 
		if (tracks->totalScalarFromPerigee.at(iTrack) + distance < drawRadius/* && ! (tracks->numPolyAnimate.at(iTrack) == (iIndex - globalTotal)/2 + 2)*/ ){
//#ifdef _DEBUG
	//std::cout<<"updating tracks to next REAL vertex "<<iTrack <<" distance = "<<distance<<" numPoly = "<<tracks->numPoly.at(iTrack) <<" numPolyAnimate = "<<tracks->numPolyAnimate.at(iTrack)<<std::endl;
//#endif		ReplaceLastTrackVertex(tracks,iTrack,iVert);
			//update distance track has travelled
			tracks->totalScalarFromPerigee.at(iTrack) += distance;

			if (iIndex - globalTotal>0) tracks->numPolyAnimate.at(iTrack) = (iIndex - globalTotal)/2 + 1;
			tracks->lastDrawnTrackIndex.at(iTrack) = iIndex - globalTotal;
			
			justAdded = true;
		}
		//if not, append replace first un-drawn vertex with point in-between last drawn vertex
		//and first un-drawn vertex
		else{
//#ifdef _DEBUG
	//		std::cout <<"outside drawing volume @ vertex "<<iVert<<std::endl;
//#endif 
			
			//exit condition, end of vertices for this track
			if (iIndex==lastTrackIndex){
				tracks->numPolyAnimate.at(iTrack) = (iIndex - globalTotal)/2 + 1;
				return;
			}
			/*
			if (!justAdded) {
				iVert-=1;
				iIndex-=2;
			}
			*/
			//if (!justAdded) ReplaceLastTrackVertex(tracks,iTrack,iVert-1);
			ReplaceLastTrackVertex(tracks,iTrack,iVert);

			if ((iIndex - globalTotal)/2 + 1 > tracks->numPoly.at(iTrack)) return;

			//remaining draw distance
			float remainingDistance = drawRadius - tracks->totalScalarFromPerigee.at(iTrack);


			InsertLastTrackVertex(tracks, iTrack, iVert, remainingDistance);
			//update number of points to animate (if necessary)
			tracks->numPolyAnimate.at(iTrack) = (iIndex - globalTotal)/2 + 2;
			break;
		}

	}

}

//if a vertex in the buffer has been over-written with a temporary one, overwrite with original point
bool Animation::ReplaceLastTrackVertex(Tracks * tracks, int iTrack, int iVert){
	//current last vertex in the vertex buffer
	static CIwFVec3 currentVertex,deltaVertex, nonAnimatedVertex;
	
	
	currentVertex.x = tracks->vertices.at(iVert*3+1);
	currentVertex.y = tracks->vertices.at(iVert*3+2);
	currentVertex.z = tracks->vertices.at(iVert*3);
	
	
	nonAnimatedVertex.x = tracks->polyX.at(iVert);
	nonAnimatedVertex.y = tracks->polyY.at(iVert);
	nonAnimatedVertex.z = tracks->polyZ.at(iVert);
	//*/

	deltaVertex = currentVertex - nonAnimatedVertex;
	
	/*
	deltaVertex.x = tracks->vertices.at(iVert*3+2) - tracks->polyX.at(iVert);
	deltaVertex.y = tracks->vertices.at(iVert*3) - tracks->polyY.at(iVert);
	deltaVertex.z = tracks->vertices.at(iVert*3+1) - tracks->polyZ.at(iVert);
	*/
	deltaVertex.x = tracks->vertices.at(iVert*3+1) - tracks->polyX.at(iVert);
	deltaVertex.y = tracks->vertices.at(iVert*3+2) - tracks->polyY.at(iVert);
	deltaVertex.z = tracks->vertices.at(iVert*3) - tracks->polyZ.at(iVert);
	
	if (deltaVertex.GetLength() < 1e-8) {
		//this vertex has not been replaced;
		return false;
		
	}

//#ifdef _DEBUG
	//std::cout <<"replacing vertex of track "<<iTrack<<" from "<<currentVertex.x <<","<<currentVertex.y <<","<<currentVertex.z <<" to "<<tracks->polyX.at(iVert) <<","<<tracks->polyY.at(iVert) <<","<<tracks->polyZ.at(iVert) <<std::endl;
//#endif
	//this vertex has been replaced, needs to replace the vertex buffer coordinates
	tracks->vertices.at(iVert*3 + 1) =  tracks->polyX.at(iVert);
	tracks->vertices.at(iVert*3 + 2) =  tracks->polyY.at(iVert);
	tracks->vertices.at(iVert*3) =  tracks->polyZ.at(iVert);
	
	tracks->verts.at(iVert).x =  tracks->polyZ.at(iVert);
	tracks->verts.at(iVert).y =  tracks->polyX.at(iVert);
	tracks->verts.at(iVert).z =  tracks->polyY.at(iVert);
	return true;
}
//interpolate a point between the last drawn vertex and the first undrawn vertex
//replace the undrawn vertex with the interpolated point and update numPolyAnimate +=1
void Animation::InsertLastTrackVertex(Tracks * tracks, int iTrack, int iVert, float remainingDrawDistance){

	static CIwFVec3 deltaVertex;
	//get normalised vector difference between last drawn point and next undrawn point
	deltaVertex.x = tracks->polyX.at(iVert+1) - tracks->polyX.at(iVert);
	deltaVertex.y = tracks->polyY.at(iVert+1) - tracks->polyY.at(iVert);
	deltaVertex.z = tracks->polyZ.at(iVert+1) - tracks->polyZ.at(iVert);
	deltaVertex.Normalise();
#ifdef _DEBUG
	if (deltaVertex.GetLengthSquared()!=deltaVertex.GetLengthSquared()) 
		std::cout <<"!!!!!!!!!!! deltaVertex.GetLength() = "<<deltaVertex.GetLengthSquared()<<std::endl;
#endif 
	

	
	//normalise the length of this vector to the remaining draw distance
	//deltaVertex *= remainingDrawDistance;
	deltaVertex.x *= remainingDrawDistance;
	deltaVertex.y *= remainingDrawDistance;
	deltaVertex.z *= remainingDrawDistance;
	//now replace the next point in the buffer with these modified values
	tracks->vertices.at((iVert+1)*3) = tracks->polyZ.at(iVert) + deltaVertex.z;
	tracks->vertices.at((iVert+1)*3 + 1) = tracks->polyX.at(iVert) + deltaVertex.x;
	tracks->vertices.at((iVert+1)*3 + 2) = tracks->polyY.at(iVert) +  deltaVertex.y;

	tracks->verts.at((iVert+1)).x = tracks->polyZ.at(iVert) + deltaVertex.z;
	tracks->verts.at((iVert+1)).y = tracks->polyX.at(iVert) + deltaVertex.x;
	tracks->verts.at((iVert+1)).z = tracks->polyY.at(iVert) +  deltaVertex.y;

//#ifdef _DEBUG
	//std::cout <<"inserting vertex "<<iVert+1 <<" of track "<<iTrack<<" with x,y,z  = "<< tracks->vertices.at((iVert+1)*3 + 1)<<","<<tracks->vertices.at((iVert+1)*3 + 2)<<","<<tracks->vertices.at((iVert+1)*3 )<<" instead of " <<tracks->polyX.at(iVert+1)<<","<< tracks->polyY.at(iVert+1)<<","<<tracks->polyZ.at(iVert+1)<<std::endl;
//#endif
}


void Animation::InitCaloTowersAnimation(CaloTowers * towers){
	if (!towers){
      std::cout << "Warning, cannot init calo Towers, towers == NULL!" << std::endl;
		return;
	}
#ifdef _DEBUG
	//std::cout <<"Now Animation::InitCaloTowersAnimation(..) for "<<towers->caloTowerCubes->size()<<" towers\n";
#endif
	
	//loop over bins (GLCubes)
	for (unsigned int iTower=0;iTower<towers->caloTowerCubes->size();iTower++){
		//update radius
		towers->currentOuterRadius->at(iTower) = -1.0f;
		//initially set all towers to not be drawn
		towers->caloTowerCubes->at(iTower)->isDrawn = false;
	}

}

void Animation::UpdateCaloTowers(CaloTowers * towers){
	
	if (!towers){
      std::cout << "Warning, cannot init calo Towers, towers == NULL!" << std::endl;
		return;
	}
#ifdef _DEBUG
	//std::cout <<"Now updating caloTowers\n";
#endif
	//loop over bins (GLCubes)
	GLCube * tower = NULL;
	for (unsigned int iTower=0;iTower<towers->caloTowerCubes->size();iTower++){
		
		tower = towers->caloTowerCubes->at(iTower);
#ifdef _DEBUG
		//std::cout <<"Now updating calo tower "<<iTower<<" with inner rad, outer rad, current outer rad = "<<tower->inner_radius<<","<<tower->outer_radius<<","<<towers->currentOuterRadius->at(iTower)<<std::endl;
#endif
		//check if in draw volume
		if (tower->inner_radius > drawRadius) continue;
		//make sure is drawn
		if (!tower->isDrawn) tower->isDrawn = true;
		//update the vertex positions of the outer radius of this tower
		UpdateTowerVertices(tower,towers->currentOuterRadius->at(iTower));
	}


}

void Animation::UpdateTowerVertices(GLCube * tower, float & currentOuterRadius){
	static float scaleFactor=0.0f;
	//now visible for first time
	if (currentOuterRadius <0.0f) {
		currentOuterRadius = tower->inner_radius;
	}
	//finished?
	if (drawRadius > tower->outer_radius){ 

		if (fabsf(tower->outer_radius - currentOuterRadius) < 1e-4) return;

		scaleFactor = tower->outer_radius/currentOuterRadius;
		for (int iVert=12; iVert < 24;iVert++) tower->vertices[iVert] *= scaleFactor;
		currentOuterRadius = tower->outer_radius;
		return;
	}
	// inner_radius < drawRadius < outer_radius
	//draw up to the draw radius
	//scale vertices [12-23] to draw radius
	scaleFactor = drawRadius / currentOuterRadius;
	for (int iVert=12; iVert < 24;iVert++) tower->vertices[iVert] *= scaleFactor;
	//update outer radius for next iteration
	currentOuterRadius = drawRadius;

}

void Animation::addStep(float rx, float ry, float rz, float z, int ns,std::vector<animationStep> * steps, bool triggerEvent){

	steps->push_back(animationStep());
	animationStep *step = &steps->back();

	step->rotation.x = rx; step->rotation.y = ry; step->rotation.z = rz;
	step->deltaRotation.x = step->deltaRotation.y = step->deltaRotation.z = 0;
	step->zoom = z;
	step->nsteps = ns;
	step->currentStep = 0;	
	step->triggerEvent = triggerEvent;
}


void Animation::UpdateAnimation(CIwFVec3 * rotation, float & zoom){

	UpdateOrthoPosition(rotation, zoom, animation_mode);
	if (eventIsPlaying) UpdateEvent();

}

void Animation::UpdateOrthoPosition(CIwFVec3 * rotation, float & zoom, int mode){
	//mode determines how to move the "camera" 
	//0 - do nothing
	//1 - circle around the detector, moving out
	//2 - straight down the pipe from near the detector
	//3 - longer and faster around the LHC (not now)

	//current step in tye animationPath
	static int pathStep =0;
	static int prevMode = -1;
	
	if (mode==0) return;

	std::vector<animationStep> * path = NULL;
	animationStep * step = NULL;
	if (mode==1) path= &circular_path;
	else if (mode==2) path= & proton_path;
	
	/*if (prevMode!=mode){
		//changed mode of animation
		prevMode = mode;
		//reset path step
		pathStep = 0;
		//reset the current step in each animation step
		for (int iStep=0;iStep<path->size();iStep++) path->at(iStep).currentStep = 0;
		
	}*/

	
	//path = &circular_path;
	//path = &proton_path;
	//hack to rest pathStep if necessary; if an animation has been restarted mid-way through
	if (path->at(0).currentStep==0) pathStep=0;
	//move towards this step
	step = &path->at(pathStep);
	
	//check if this step is finished
#ifdef _DEBUG
	std::cout<<"!! Animation::UpdateOrthoPosition(..), zoom, dZoom, nstep, currentstep, pathStep, mode = "<<zoom<<","<<step->deltaZoom <<","<<step->nsteps <<","<<step->currentStep<<","<<pathStep<<","<<mode<<std::endl;
#endif
	if (step->currentStep==step->nsteps){
		//is the entire chain finished
		if (pathStep == path->size()-1) return;
		else {
			pathStep++;
			step = &path->at(pathStep);
		}
	}

	//define delta angle and delta zoom from here to this step
	if (step->currentStep==0){
		step->deltaRotation.x = (step->rotation.x - rotation->x) / step->nsteps;
		step->deltaRotation.y = (step->rotation.y - rotation->y) / step->nsteps;
		step->deltaRotation.z = (step->rotation.z - rotation->z) / step->nsteps;
		step->deltaZoom = (step->zoom - zoom)/ step->nsteps;
		//start playing the event
		if (step->triggerEvent) eventIsPlaying = true;
	}
	//update the rotation and zoom
	/*rotation->x += step->deltaRotation.x;
	rotation->y += step->deltaRotation.y;
	rotation->z += step->deltaRotation.z;*/
	*rotation += step->deltaRotation;
	zoom += step->deltaZoom;
	//update step
	step->currentStep++;

}


void Animation::UpdateTrackIndex_2(Tracks * tracks, int globalTotal, int iTrack, int nPreviousPoints){

	int lastDrawnTrackVertex =  tracks->lastDrawnTrackIndex.at(iTrack);
	
	//last possible track index
	int lastTrackVertex = tracks->numPoly.at(iTrack)-1;
	//distance from center to vertex
	static CIwFVec3 distanceFromPerigee;
	

	//float remainingDrawRadius = drawRadius - tracks->totalScalarFromPerigee.at(iTrack);
	float remainingDrawRadius = deltaRadius;
	
	//input: drawRadius, vertices for track,
	//last vertex may have been replaced
	//lastDrawnTrackIndex: last fixed vertex inside volume (+ unfixed one)
	//totalScalarDistanceFromPerigee: distance to last fixed vertex inside volume

	//first find all new fixed vertices inside draw volume
	//then adjust the next one for remaining draw distance


	int replacedVertex = tracks->replaced_verts.at(iTrack).first;
	//last vertex is un-fixed, temporary between two fixed vertices
	if (replacedVertex !=-1){
		lastDrawnTrackVertex = replacedVertex;
		//work out remaining draw distance to next vertex
		//distanceFromPerigee = tracks->replaced_verts.at(iTrack).second - tracks->verts_2.at(iTrack).at(replacedVertex+1);
		distanceFromPerigee = tracks->replaced_verts.at(iTrack).second - tracks->verts_2.at(iTrack).at(replacedVertex);
		float distance = distanceFromPerigee.GetLength();
		//std::cout <<"Animation::UpdateTrackIndex_2(..) drawRadius = "<<drawRadius<<", remainingDrawRadius = "<<remainingDrawRadius<<" deltaRadius = "<< deltaRadius<< "totalScalarFromPerigee" <<tracks->totalScalarFromPerigee.at(iTrack) << " has replaced vertex with distance to = "<<distance<<" replacedVertex = "<<replacedVertex <<" numPolyAnimate = "<<tracks->numPolyAnimate.at(iTrack)<<std::endl;
		//next fixed vertex is inside draw volume
		if (/*tracks->totalScalarFromPerigee.at(iTrack) + */ distance < remainingDrawRadius){
			//replace original fixed vertex
			tracks->replaced_verts.at(iTrack).first = -1;
			tracks->verts_2.at(iTrack).at(replacedVertex).x = tracks->replaced_verts.at(iTrack).second.x;
			tracks->verts_2.at(iTrack).at(replacedVertex).y = tracks->replaced_verts.at(iTrack).second.y;
			tracks->verts_2.at(iTrack).at(replacedVertex).z = tracks->replaced_verts.at(iTrack).second.z;
			//update remaining draw distance
			remainingDrawRadius -= distance;
			tracks->totalScalarFromPerigee.at(iTrack) += distance;
		}
		//next fixed vertex is *not* inside draw volume
		//so update this unfixed vertex with extra draw distance and we're done
		else{
			distanceFromPerigee.Normalise();
			distanceFromPerigee *= remainingDrawRadius;
			//tracks->replaced_verts.at(iTrack).second += distanceFromPerigee;
			//tracks->verts_2.at(iTrack).at(replacedVertex) += distanceFromPerigee;
			tracks->verts_2.at(iTrack).at(replacedVertex).x += distanceFromPerigee.x;
			tracks->verts_2.at(iTrack).at(replacedVertex).y += distanceFromPerigee.y;
			tracks->verts_2.at(iTrack).at(replacedVertex).z += distanceFromPerigee.z;
			//remainingDrawRadius -= distance;
			//tracks->totalScalarFromPerigee.at(iTrack) += distance;
			tracks->totalScalarFromPerigee.at(iTrack) += remainingDrawRadius;
			return;
		}
	}
	
	//all vertices are fixed vertices
	//now iterate to find how many other fixed vertices are still inside draw volume
	//drawRadius is the remaining distance track will go in this frame
	static CIwFVec3 deltaVertex;
	for (int iVert = lastDrawnTrackVertex+1; iVert < lastTrackVertex;iVert++){
		deltaVertex  = tracks->verts_2.at(iTrack).at(iVert) - tracks->verts_2.at(iTrack).at(iVert-1);
		float toNextVertex = deltaVertex.GetLength();
		//this one is inside
		if (remainingDrawRadius > toNextVertex){
			remainingDrawRadius -= toNextVertex;
			tracks->totalScalarFromPerigee.at(iTrack) += toNextVertex;
			continue;
		}
		else{
			//this vertex is not inside draw volume
			//replace it, update lastDrawnTrackVertex
			tracks->replaced_verts.at(iTrack).first = iVert;
			tracks->replaced_verts.at(iTrack).second.x = tracks->verts_2.at(iTrack).at(iVert).x;
			tracks->replaced_verts.at(iTrack).second.y = tracks->verts_2.at(iTrack).at(iVert).y;
			tracks->replaced_verts.at(iTrack).second.z = tracks->verts_2.at(iTrack).at(iVert).z;
			deltaVertex.Normalise();
			deltaVertex *= remainingDrawRadius;
			tracks->verts_2.at(iTrack).at(iVert) = tracks->verts_2.at(iTrack).at(iVert-1) + deltaVertex;
			tracks->totalScalarFromPerigee.at(iTrack) += deltaVertex.GetLength();
			//update last drawn track vertex
			tracks->lastDrawnTrackIndex.at(iTrack) = iVert;
			tracks->numPolyAnimate.at(iTrack) = iVert+1;
			return;


		}
	}

}




//if a vertex in the buffer has been over-written with a temporary one, overwrite with original point
bool Animation::ReplaceLastTrackVertex_2(Tracks * tracks, int iTrack, int iVert){
	//current last vertex in the vertex buffer
	static CIwFVec3 * currentVertex, deltaVertex, nonAnimatedVertex;
	
	currentVertex = &tracks->verts_2.at(iTrack).at(iVert);

	//has the vertex been replaced?
	if (tracks->replaced_verts.at(iTrack).first==-1) return false;
	if (tracks->replaced_verts.at(iTrack).first==iVert){
		tracks->verts_2.at(iTrack).at(iVert).x = tracks->replaced_verts.at(iTrack).second.x;
		tracks->verts_2.at(iTrack).at(iVert).y = tracks->replaced_verts.at(iTrack).second.y;
		tracks->verts_2.at(iTrack).at(iVert).z = tracks->replaced_verts.at(iTrack).second.z;
	
	}

	//reset this
	tracks->replaced_verts.at(iTrack).first = -1;
	return false;


	/*currentVertex.x = tracks->vertices.at(iVert*3+1);
	currentVertex.y = tracks->vertices.at(iVert*3+2);
	currentVertex.z = tracks->vertices.at(iVert*3);*/
	
	

	nonAnimatedVertex.x = tracks->polyX.at(iVert);
	nonAnimatedVertex.y = tracks->polyY.at(iVert);
	nonAnimatedVertex.z = tracks->polyZ.at(iVert);
	//*/

	deltaVertex = *currentVertex - nonAnimatedVertex;
	
	/*
	deltaVertex.x = tracks->vertices.at(iVert*3+2) - tracks->polyX.at(iVert);
	deltaVertex.y = tracks->vertices.at(iVert*3) - tracks->polyY.at(iVert);
	deltaVertex.z = tracks->vertices.at(iVert*3+1) - tracks->polyZ.at(iVert);
	*/
	deltaVertex.x = tracks->vertices.at(iVert*3+1) - tracks->polyX.at(iVert);
	deltaVertex.y = tracks->vertices.at(iVert*3+2) - tracks->polyY.at(iVert);
	deltaVertex.z = tracks->vertices.at(iVert*3) - tracks->polyZ.at(iVert);
	
	if (deltaVertex.GetLength() < 1e-8) {
		//this vertex has not been replaced;
		return false;
		
	}

//#ifdef _DEBUG
	//std::cout <<"replacing vertex of track "<<iTrack<<" from "<<currentVertex.x <<","<<currentVertex.y <<","<<currentVertex.z <<" to "<<tracks->polyX.at(iVert) <<","<<tracks->polyY.at(iVert) <<","<<tracks->polyZ.at(iVert) <<std::endl;
//#endif
	//this vertex has been replaced, needs to replace the vertex buffer coordinates
	tracks->vertices.at(iVert*3 + 1) =  tracks->polyX.at(iVert);
	tracks->vertices.at(iVert*3 + 2) =  tracks->polyY.at(iVert);
	tracks->vertices.at(iVert*3) =  tracks->polyZ.at(iVert);
	
	tracks->verts.at(iVert).x =  tracks->polyZ.at(iVert);
	tracks->verts.at(iVert).y =  tracks->polyX.at(iVert);
	tracks->verts.at(iVert).z =  tracks->polyY.at(iVert);
	return true;
}
//interpolate a point between the last drawn vertex and the first undrawn vertex
//replace the undrawn vertex with the interpolated point and update numPolyAnimate +=1
void Animation::InsertLastTrackVertex_2(Tracks * tracks, int iTrack, int iVert, float remainingDrawDistance){


	static CIwFVec3 deltaVertex;

	deltaVertex = tracks->verts_2.at(iTrack).at(iVert+1) - tracks->verts_2.at(iTrack).at(iVert);
	deltaVertex.Normalise();
	deltaVertex.x *= remainingDrawDistance;

	//set replaced vertex
	tracks->replaced_verts.at(iTrack).first = iVert+1;
	tracks->replaced_verts.at(iTrack).second.x  = tracks->verts_2.at(iTrack).at(iVert+1).x;
	tracks->replaced_verts.at(iTrack).second.y  = tracks->verts_2.at(iTrack).at(iVert+1).y;
	tracks->replaced_verts.at(iTrack).second.z  = tracks->verts_2.at(iTrack).at(iVert+1).z;

	tracks->verts_2.at(iTrack).at(iVert+1) = tracks->verts_2.at(iTrack).at(iVert) + deltaVertex;
	return;


	//get normalised vector difference between last drawn point and next undrawn point
	deltaVertex.x = tracks->polyX.at(iVert+1) - tracks->polyX.at(iVert);
	deltaVertex.y = tracks->polyY.at(iVert+1) - tracks->polyY.at(iVert);
	deltaVertex.z = tracks->polyZ.at(iVert+1) - tracks->polyZ.at(iVert);
	deltaVertex.Normalise();
#ifdef _DEBUG
	if (deltaVertex.GetLengthSquared()!=deltaVertex.GetLengthSquared()) 
		std::cout <<"!!!!!!!!!!! deltaVertex.GetLength() = "<<deltaVertex.GetLengthSquared()<<std::endl;
#endif 
	

	
	//normalise the length of this vector to the remaining draw distance
	//deltaVertex *= remainingDrawDistance;
	deltaVertex.x *= remainingDrawDistance;
	deltaVertex.y *= remainingDrawDistance;
	deltaVertex.z *= remainingDrawDistance;
	//now replace the next point in the buffer with these modified values
	tracks->vertices.at((iVert+1)*3) = tracks->polyZ.at(iVert) + deltaVertex.z;
	tracks->vertices.at((iVert+1)*3 + 1) = tracks->polyX.at(iVert) + deltaVertex.x;
	tracks->vertices.at((iVert+1)*3 + 2) = tracks->polyY.at(iVert) +  deltaVertex.y;

	tracks->verts.at((iVert+1)).x = tracks->polyZ.at(iVert) + deltaVertex.z;
	tracks->verts.at((iVert+1)).y = tracks->polyX.at(iVert) + deltaVertex.x;
	tracks->verts.at((iVert+1)).z = tracks->polyY.at(iVert) +  deltaVertex.y;

//#ifdef _DEBUG
	//std::cout <<"inserting vertex "<<iVert+1 <<" of track "<<iTrack<<" with x,y,z  = "<< tracks->vertices.at((iVert+1)*3 + 1)<<","<<tracks->vertices.at((iVert+1)*3 + 2)<<","<<tracks->vertices.at((iVert+1)*3 )<<" instead of " <<tracks->polyX.at(iVert+1)<<","<< tracks->polyY.at(iVert+1)<<","<<tracks->polyZ.at(iVert+1)<<std::endl;
//#endif
}
