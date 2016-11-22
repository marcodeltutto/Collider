//
//  EventFromXML.h
//  IwGxScene
//
//  Created by boddyadmin on 23/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Tracks_h
#define Tracks_h

#include "GLMesh.h"
#include "ColourStore.h"
#include <vector>
#include <math.h>
#include <iostream>

/** @brief
 This class is used to render the ID and muon tracks in the event display.  It is the only class that inherits from GLMesh with a more complicated Draw function. 
 For each reconstructed track, a vector of vertices is created, the indices used are a static array that simply joins adjacent vertices (0,1,1,2,2,3 etc.).

 The input from the EventFromXML for a Tracks instance are vectors of the helix parameters of each track in the event (i.e. 5 vectors (q/pt, cot(theta),phi0,d0 and z0). For each set of helix parameters the position of set of vertices are calculated for the track from the center of the coordinate system to the edge of the fiducial volume if the Inner Detector (for details of the algorithm see Chris Boddy's thesis http://www-pnp.physics.ox.ac.uk/~boddy/files/thesis.pdf, Section 4.5.1).

 A separate vector is used for each track to make the animation of tracks easier to implement. 
 */
class Tracks: public GLMesh{
public:
	//c-tor
	Tracks(){};
	//d-tor
	virtual ~Tracks(){};
	//save all mem. management for here
	void Init(int _nTracks, char * _name, float _z_max = 270.0, float _rho_max = 107.23, int _nPoly = 40, float _pt_cut = 2.5, float _z0_cut = 15.0, float _d0_cut = 50.0, float _worldScale = 50.0, int _nPixcut=1, int _nSCTcut=1, int _nTRTcut = 0){
		
		//can re-use via calling Init
		Clear();

		GLMesh::Init(_worldScale);
		//quality cuts
		nPixcut = _nPixcut;
		nSCTcut = _nSCTcut;
		nTRTcut = _nTRTcut;
		//define fiducial region in rho-z plane
		z_max = _z_max;
		z_max *= z_max;

		rho_max = _rho_max;
		rho_max2 = rho_max * rho_max;
		//how smooth a track (higher is smoother)
		nPoly = _nPoly;
		//name and ncands
		name = _name; 
		nTracks = _nTracks;
		//acc. cuts
		pt_cut = _pt_cut;
		z0_cut = _z0_cut;
		d0_cut = _d0_cut;

		//reserve some memory
		pt.reserve(nTracks);
		cotTheta.reserve(nTracks);
		phi0.reserve(nTracks);
		d0.reserve(nTracks);
		z0.reserve(nTracks);

		polyX.reserve(nTracks * _nPoly);
		polyY.reserve(nTracks * _nPoly);
		polyZ.reserve(nTracks * _nPoly);
		
		nPix.reserve(nTracks);
		nSCT.reserve(nTracks);
		nTRT.reserve(nTracks);
		numPoly.reserve(nTracks);
		
		numPolyAnimate.reserve(nTracks);
		lastDrawnTrackIndex.reserve(nTracks);
		totalDistanceFromPerigee.reserve(nTracks);

		r0.reserve(nTracks);
		dA_max.reserve(nTracks);
		center_x.reserve(nTracks);
		center_y.reserve(nTracks);
		//for drawing
		indices.reserve(2*nTracks*_nPoly);
		vertices.reserve(3*nTracks*_nPoly);

		//new indices declaration for strip fan drawing
		if (Tracks::indices_2[255]==0)
			for (int i=0;i<255;i++) Tracks::indices_2[i] = i;

	}

	//GLMesh drawing etc.
	//set the vertices to be drawn as tracks
	void setVertices(){
		
		int totalVertices=0;

		for (unsigned int iPoint=0;iPoint < polyX.size();iPoint++){
				vertices.push_back(polyZ[iPoint]);
				vertices.push_back(-polyX[iPoint]);
				vertices.push_back(polyY[iPoint]);

				verts.push_back(CIwFVec3(polyZ[iPoint],-polyX[iPoint],polyY[iPoint]));
		}


		
		for (unsigned int iTrack=0;iTrack < pt.size();iTrack++){		
			//fill the index buffer for GL_LINES
			for (int iPoint = totalVertices; iPoint < (totalVertices+(int)numPoly[iTrack]-1);iPoint++){
				indices.push_back((unsigned short) (iPoint));
				indices.push_back((unsigned short) (iPoint+ 1));
			}
			totalVertices += numPoly[iTrack];
			
		}
		
		//scale to worldsize
		for (unsigned int iVert=0;iVert< vertices.size();iVert++) vertices[iVert] /= worldScale;

		for (unsigned int iVert=0;iVert< verts.size();iVert++) {
			verts[iVert].x /= worldScale;
			verts[iVert].y /= worldScale;
			verts[iVert].z /= worldScale;
		}
		
		for (unsigned int iPoint=0;iPoint< polyX.size();iPoint++) {
			polyX[iPoint] /= worldScale;
			polyY[iPoint] /= worldScale;
			polyZ[iPoint] /= worldScale;
		}
		
		for (unsigned int iNpoly=0;iNpoly < numPoly.size();iNpoly++)
			numPolyAnimate.push_back(numPoly.at(iNpoly));
		
		/*added for new drawing method */
		verts_2.clear();
		
		
		//loop over tracks
		int globalTotal=0;
		for (unsigned int iTrack=0;iTrack < pt.size();++iTrack){
			//another track of vertices
			verts_2.push_back(std::vector<CIwFVec3>());
			//loop over vertices
			for (unsigned int iVert=0;iVert < numPoly.at(iTrack);iVert++){
				verts_2.at(iTrack).push_back(CIwFVec3(polyZ[iVert+globalTotal],-polyX[iVert+globalTotal],polyY[iVert+globalTotal]));
			}
			globalTotal += numPoly.at(iTrack);
		}

		


		//set first vertex, index
		//FirstVertex = &vertices[0];
		//FirstVertex = &verts[0];
		FirstVertex  = NULL;
		//FirstVertex = &verts_2.at(0)[0];
		//FirstIndex = &indices[0];
		FirstIndex = &Tracks::indices_2[0];

		num_indices = indices.size();
	}

	std::vector<float> pt,phi0,cotTheta,z0,d0;
	std::vector<int> nSCT,nPix,nTRT, numPoly, numPolyAnimate;
	std::vector<float> polyX, polyY, polyZ, vertices;
	std::vector<CIwFVec3> verts;
	
	std::vector<float> r0, dA_max, center_x, center_y;
	std::vector<unsigned short> indices;
	std::vector<int> lastDrawnTrackIndex;
	std::vector<CIwFVec3> totalDistanceFromPerigee;
	std::vector<float> totalScalarFromPerigee;
	char * name;
	int nTracks, nPoly;
	int ncalls;
    
	std::vector<std::vector<CIwFVec3> > verts_2;
	std::vector<std::pair<int,CIwFVec3> > replaced_verts;
	//std::vector<std::vector<unsigned short> > indices_2;
	static unsigned short indices_2[256];

	CIwFVec3 NextUndrawnVertex, LastReplacedVertex;

	float z_max, rho_max, rho_max2;
	//acceptance & quality cuts
	float pt_cut, z0_cut, d0_cut;
	int nPixcut, nSCTcut, nTRTcut;

	void setName(char* _name){name = _name;}
	void setNtracks(int _nTracks){nTracks = _nTracks;}
	static unsigned int pt_hash, phi0_hash, cotTheta_hash, z0_hash, d0_hash, npix_hash, nsct_hash, ntrt_hash;
	static unsigned int numpolyline_hash, polylineX_hash, polylineY_hash, polylineZ_hash;
	static unsigned int idTracks_hash, muonTracks_hash;
	void Clear(){
		//hits
		nPix.clear();nSCT.clear();nTRT.clear();
		//poly-points
		polyX.clear();polyY.clear();polyZ.clear();numPoly.clear();
		//perigee params
		pt.clear();
		cotTheta.clear();
		phi0.clear();
		d0.clear();
		z0.clear();
		//helix info.
		r0.clear();
		dA_max.clear();
		center_x.clear();
		center_y.clear();
		//for drawing
		vertices.clear();
		verts.clear();
		
		for (int iTrack=0;iTrack < verts_2.size();iTrack++) verts_2.at(iTrack).clear();
		verts_2.clear();

		indices.clear();
		//for animation
		numPolyAnimate.clear();
		lastDrawnTrackIndex.clear();
		totalDistanceFromPerigee.clear();
		totalScalarFromPerigee.clear();

		FirstVertex = NULL; 
		FirstIndex = NULL;
	}

	
	void alternateDraw(){
		
		FirstIndex = &Tracks::indices_2[0];
		int globalTotal =0;
		for (unsigned int iTrack=0;iTrack < numPoly.size(); iTrack++){
			
			FirstVertex = &(verts_2.at(iTrack).at(0));
			int nIndices = verts_2.at(iTrack).size();
			
			nIndices = numPolyAnimate.at(iTrack);
			if (nIndices < 1) continue;
			//num_indices = (numPolyAnimate.at(iTrack)-1)*2;
			//if (num_indices <0) num_indices =0;

			//std::cout <<"!!!Calling Tracks::alternateDraw() nIndices = "<<nIndices <<" FirstVertex = "<<FirstVertex<<" !!!\n";

            float oldalph = ColourStore::rgba_idTracks[3];
            
            float alphascale = (fabsf(pt[iTrack]) / 10.0);
            if(alphascale >= 1.0)
                alphascale = 1.0;
            if(alphascale <= 0.3)
                alphascale = 0.3;
            
            ColourStore::rgba_idTracks[3] = oldalph * alphascale;
            glLineWidth(2.0);
            if(fabsf(pt[iTrack]) > pt_cut && fabsf(z0[iTrack]) < z0_cut)
                GLMesh::Draw(true,nIndices);
            
            glLineWidth(1.0);
            ColourStore::rgba_idTracks[3] = oldalph;
            
			globalTotal += nIndices;
		}
	}


	bool isValid(){
		if (pt.size()!=cotTheta.size()) return false;
		if (pt.size()!=phi0.size()) return false;
		if (pt.size()!=d0.size()) return false;
		if (pt.size()!=z0.size()) return false;
		
		if (polyX.size()!=polyY.size()) return false;
		if (polyX.size()!=polyZ.size()) return false;
		return true;
	}

	//generate the helix info. from the perigee params
	void genHelixInfo(){
		float tmp;
		for (unsigned int iTrk =0;iTrk< pt.size();iTrk++){

//			//quality cuts
//			if (fabsf(pt[iTrk]) < pt_cut  || nPix[iTrk] < nPixcut|| nSCT[iTrk] < nSCTcut || nTRT[iTrk] < nTRTcut){ 
//				r0.push_back(0.0);
//				center_x.push_back(0.0);
//				center_y.push_back(0.0);
//				dA_max.push_back(0.0);
//				continue;
//			}
			//radius
			tmp = static_cast<float>(100/0.6) * pt[iTrk];
			r0.push_back(tmp);
			//center of the  circle
			tmp = static_cast<float>(-1.0) * (r0[iTrk]  + d0[iTrk]) * sinf(phi0[iTrk]);
			center_x.push_back(tmp);
			tmp = (r0[iTrk]  + d0[iTrk]) *  cosf(phi0[iTrk]);
			center_y.push_back(tmp);
			//dAmax
			float fPI = static_cast<float>(PI/2.0);
			float m_fPI = static_cast<float>(-1.0*PI/2.0);
			
			ncalls = 0;
			if (r0[iTrk] > 0.0) tmp = find_dAmax(iTrk,0.0,fPI,false);
			else tmp = find_dAmax(iTrk,0.0,m_fPI,false);
			dA_max.push_back(tmp);
			//CIwGameError::LogError("!!! HIHI 2");
			
		}
	}
	//using generated helix param. info, generate polyline points for tracks
	void genPolyLinePoints(int nPoints = 20){
		polyX.clear();
		polyY.clear();
		polyZ.clear();
		numPoly.clear();
		//temp vars for each point
		static float da_temp, xi, yi, zi;

		for (unsigned int iTrack = 0;iTrack < pt.size();++iTrack){
			//generate the points for this track, add them to the vector for GL call
//			if (fabsf(pt[iTrack]) < pt_cut) {
			/*if (fabsf(dA_max[iTrack]) <1e-5) {
				numPoly.push_back(0);
				continue;
			}*/
			numPoly.push_back(nPoints);
			for (int iPoint =0;iPoint < nPoints;iPoint++){
				da_temp = static_cast<float>(iPoint) * dA_max[iTrack] / static_cast<float>(nPoints);
                
				xi = center_x[iTrack] + r0[iTrack] * sinf(phi0[iTrack] + da_temp);
				yi = center_y[iTrack] - r0[iTrack] * cosf(phi0[iTrack] + da_temp);
				zi = z0[iTrack] + cotTheta[iTrack]  * r0[iTrack]  * da_temp;
				polyX.push_back(xi);
				polyY.push_back(yi);
				polyZ.push_back(zi);
			}
		}


	}
	//calculate the max. angle from of the helix that is inside the fiducial detector volume
	//see C. Boddy's thesis for details
	float find_dAmax(int iTrk, float last_dAin, float prev_dA, bool prevInside=false){
		static float new_da = 0.0;
		if (ncalls>75) return 0.0;
		ncalls++;
		//CIwGameError::LogError("ncalls find_da_max = ",CIwGameString(ncalls).c_str());

		if (!prevInside && r0[iTrk] > 0.0) new_da = static_cast<float>(last_dAin + (prev_dA - last_dAin)/2.0);
		else if (!prevInside && r0[iTrk] < 0.0) new_da = static_cast<float>(last_dAin - (-prev_dA + last_dAin)/2.0);
		else new_da = static_cast<float>(prev_dA * 1.5);

		//inside fiducial volume @ new_da
		if (isPointInDet(iTrk,new_da)){
			last_dAin = new_da;
			return find_dAmax(iTrk,last_dAin,new_da,true);
		}
		
		//outside fiducial volume @ new_da
		//done
        if (fabsf(last_dAin) > 1e-8 && fabsf(prev_dA - new_da) <1e-3)
			return last_dAin;
		//keep going
		return  find_dAmax(iTrk,last_dAin,new_da,false);
		

	}

	//is this value of dA still inside the fiducial detector volume?
	bool isPointInDet(int iTrk, float new_da){
		static float xi,yi,zi,rho2,z2;
		//CIwGameError::LogError("!!! ENTERING ISPOINTINDET");
		//inside the z region?
		zi = z0[iTrk] + cotTheta[iTrk] * r0[iTrk] * new_da;
		z2 = zi * zi;
		if (z2 > z_max) {
		//CIwGameError::LogError("!!! LEAVING ISPOINTINDET");
		return false;
		}

		//inside the rho region?
		xi = center_x[iTrk] + r0[iTrk] * sinf(phi0[iTrk]+new_da);
		yi = center_y[iTrk] - r0[iTrk] * cosf(phi0[iTrk]+new_da);
		rho2 = xi*xi + yi*yi;
		//CIwGameError::LogError("!!! LEAVING ISPOINTINDET");
		if (rho2 > rho_max2) return false;
		return true;
		
	}
	//GLMesh stuff
	virtual void Release(){
		Clear();
	}
	
	void Draw(bool drawStrip = false, int nIndicesToDraw=0){
		alternateDraw();
		//GLMesh::Draw();
	}
	
	
	//alternate draw fn
	/*
	void alternateDraw(){
		//running total of indices used
		int globalTotal =0;
		//number of indices of a track
		int nIndices=0;
		//loop over tracks
		
		for (unsigned int iTrack=0;iTrack < numPoly.size(); iTrack++){
			//reset num_indices and FirstIndex for this track
			FirstIndex = &indices[globalTotal];
			//if (numPoly.at(iTrack)<2) continue;
			nIndices = (numPoly.at(iTrack)-1)*2;
			num_indices = (numPolyAnimate.at(iTrack)-1)*2;
			if (num_indices <0) num_indices =0;
			//std::cout <<"calling tracks draw with num_indices = "<<num_indices<<" nIndices = "<<nIndices<<std::endl;
			GLMesh::Draw();
			//update total
			globalTotal += nIndices;
			

		}
	}
	*/
	
	
};
#endif
