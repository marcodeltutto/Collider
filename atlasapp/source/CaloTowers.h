//
//  EventFromXML.h
//  IwGxScene
//
//  Created by boddyadmin on 23/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef CaloTowers_h
#define CaloTowers_h


#include "GLCube.h"
#include <vector>
#include <math.h>
#include <iostream>

/** @brief This class is used by the EventFromXML class; input is a 2-D eta-phi calorimeter histogram. store the data and then dynamically create some GLCube instances using the eta, phi and ET of each histogram bin.
 * eta * phi are always the same (a 16 * 16 bin histgram), ET is the sum ET in the bin, over all cells in the ecal or hcal + fcal.
 *
 * The height of the cube is defined by ET. the absolute height from the center of the coordinate system is define in the source using the average rho and z of the calorimeters.

*/
class CaloTowers{
public:
	//PI as a float
	float fPI;

	//data-members
	std::vector<GLCube*> *caloTowerCubes;	
	std::vector<float> * etHistogram;
	std::vector<int> * psiHistogram, * phiHistogram;
	std::vector<float> * currentOuterRadius;

	static unsigned int hCalhist_et_hash, hCalhist_phi_hash, hCalhist_psi_hash;
	static unsigned int eCalhist_et_hash, eCalhist_phi_hash, eCalhist_psi_hash;
	static unsigned int ecalTowers_hash, hcalTowers_hash;
	char * name;
	int nCaloTowers, nPoly;
	int ncalls;
	float barrel_rho, endcap_z, endcap_region;
	float worldScale;
	
	//acceptance & quality cuts

	//c-tor
	CaloTowers(){
			etHistogram = new std::vector<float>;
			psiHistogram = new std::vector<int>;
			phiHistogram = new std::vector<int>;
			
			caloTowerCubes = new std::vector<GLCube*>();
			fPI= static_cast<float>(PI);
			currentOuterRadius = new std::vector<float>;
	};
	//d-tor
	virtual ~CaloTowers(){
		etHistogram->clear(); 
		psiHistogram->clear();
		phiHistogram->clear();
		caloTowerCubes->clear();
		currentOuterRadius->clear();

		delete etHistogram;
		delete psiHistogram;
		delete phiHistogram;
		delete caloTowerCubes;
		delete currentOuterRadius;
	};


	//save all mem. management for here
	void Init(
		std::vector<float> * _etHistogram, 
		std::vector<int> * _psiHistogram, 
		std::vector<int> * _phiHistogram,
		float _worldScale = 50.0){
		
		//can re-use via calling Init
		Clear();
		caloTowerCubes->reserve(nCaloTowers);
		worldScale = static_cast<float>(1.0);
	}

	void Release(){
		for (int iTower=(int)caloTowerCubes->size()-1;iTower>=0;iTower--)
			delete caloTowerCubes->at(iTower);

		Clear();
	}
	
	void setName(char* _name){name = _name;}
	void setNCaloTowers(int _nCaloTowers){nCaloTowers = _nCaloTowers;}

	
	static unsigned int tileEt_hash, tilePhi_hash, tilePsi_hash;

	void Clear(){
		etHistogram->clear();
		psiHistogram->clear();
		phiHistogram->clear();
		caloTowerCubes->clear();
	}

	void defineGLCubes(){
		//for each calo tower, define a cube from the psi, phi and ET. 
		//add it to the vector of GLCubes
		for (unsigned int iCube = 0;iCube <etHistogram->size();iCube++){
			caloTowerCubes->push_back(new GLCube());
			defineGLCube(caloTowerCubes->at(iCube),etHistogram->at(iCube),psiHistogram->at(iCube),phiHistogram->at(iCube));
		}

	}
	

	void defineGLCube(GLCube * glCube, float etValue, int psiBin, int phiBin){
		//define coordinates of the cube
		//call with Init(..)
		//coordinates of cuboid
		static float x_coords[8], y_coords[8], z_coords[8];
		static float psi, phi, et;
		//segmentation of phi, psi planes into nbins
		static float nBins = 32;
		static float binWidth = fPI / nBins;
		//total radius lengths for tower
		static float total_vec_inner;
		static float total_vec_outer;
		//some detector specific geometry stuff
		static float first_bin_psi = static_cast<float>(0.09817);
		static float penultimate_mid_psi = static_cast<float>(1.4726);
		static float small_psi_diff =	static_cast<float>(0.05);
		static float psi_axis_offset = static_cast<float>(0.05);
		static float psi_corrected;
		static float rho_x, rho_y, z_inner, z_outer;
		static float twof = static_cast<float>(1.0);
		static float fourf = static_cast<float>(1.0);
		for (unsigned int i=0;i<8;i++) x_coords[i] = y_coords[i] = z_coords[i] = static_cast<float>(0.0);

		psi = (static_cast<float>(psiBin) + 0.5) * (fPI / nBins);
		phi = (static_cast<float>(phiBin) + 0.5) * (2*fPI / nBins);
		et = etValue * static_cast<float>(8.0);
        
//
//        endcap_region = 0;
//        endcap_z = 0;
//        barrel_rho = 0;
        
		//define the radius length
		if (fabs((fPI - endcap_region) < psi && psi < (fPI + endcap_region)) || psi < endcap_region || (2.0*fPI - endcap_region) < psi) {
                //these 4 (r,z) points define a path that will be filled i.e. the bin corners
                rho_x = endcap_z * tanf(psi - binWidth / static_cast<float>(4.0));
                rho_y = endcap_z * tanf(psi + binWidth / static_cast<float>(4.0));

                total_vec_inner = sqrtf(endcap_z * endcap_z + rho_x * rho_x);
                total_vec_outer = sqrtf(endcap_z * endcap_z + rho_x * rho_x) + et;

        }//end of endcap bins
        //in the barrel regions
        else {

                z_inner = barrel_rho / tanf(psi - binWidth / static_cast<float>(4.0));
                z_outer = barrel_rho / tanf(psi + binWidth / static_cast<float>(4.0));

                total_vec_inner = sqrtf(barrel_rho * barrel_rho + z_outer * z_outer);
                total_vec_outer = sqrtf(barrel_rho * barrel_rho + z_outer * z_outer) + et;
        }//end of barrel regions

		//here we define all the coordinates :-o
		if (fabsf(psi - first_bin_psi) < small_psi_diff) {
                psi_corrected = psi;
                //zeroth bin
                if (fabsf(fabsf(psi) - first_bin_psi) < small_psi_diff) psi = first_bin_psi + psi_axis_offset;
                //half way

                x_coords[0] = total_vec_inner * cosf( (phi - binWidth / twof)) * sinf( (psi - binWidth / fourf));
                y_coords[0] = total_vec_inner * sinf( (phi - binWidth / twof)) * sinf( (psi - binWidth / fourf));
                z_coords[0] = total_vec_inner * cosf( (psi - binWidth / fourf));

                x_coords[1] = total_vec_inner * cosf( (phi + binWidth / twof)) * sinf( (psi - binWidth / fourf));
                y_coords[1] = total_vec_inner * sinf( (phi + binWidth / twof)) * sinf( (psi - binWidth / fourf));
                z_coords[1] = total_vec_inner * cosf( (psi - binWidth / fourf));

                x_coords[4] = total_vec_outer * cosf( (phi - binWidth / twof)) * sinf( (psi - binWidth / fourf));
                y_coords[4] = total_vec_outer * sinf( (phi - binWidth / twof)) * sinf( (psi - binWidth / fourf));
                z_coords[4] = total_vec_outer * cosf( (psi - binWidth / fourf));

                x_coords[5] = total_vec_outer * cosf( (phi + binWidth / twof)) * sinf( (psi - binWidth / fourf));
                y_coords[5] = total_vec_outer * sinf( (phi + binWidth / twof)) * sinf( (psi - binWidth / fourf));
                z_coords[5] = total_vec_outer * cosf( (psi - binWidth / fourf));

                psi = psi_corrected;

                x_coords[2] = total_vec_inner * cosf( (phi + binWidth / twof)) * sinf( (psi + binWidth / fourf));
                y_coords[2] = total_vec_inner * sinf( (phi + binWidth / twof)) * sinf( (psi + binWidth / fourf));
                z_coords[2] = total_vec_inner * cosf( (psi + binWidth / fourf));

                x_coords[3] = total_vec_inner * cosf( (phi - binWidth / twof)) * sinf( (psi + binWidth / fourf));
                y_coords[3] = total_vec_inner * sinf( (phi - binWidth / twof)) * sinf( (psi + binWidth / fourf));
                z_coords[3] = total_vec_inner * cosf( (psi + binWidth / fourf));

                x_coords[6] = total_vec_outer * cosf( (phi + binWidth / twof)) * sinf( (psi + binWidth / fourf));
                y_coords[6] = total_vec_outer * sinf( (phi + binWidth / twof)) * sinf( (psi + binWidth / fourf));
                z_coords[6] = total_vec_outer * cosf( (psi + binWidth / fourf));

                x_coords[7] = total_vec_outer * cosf( (phi - binWidth / twof)) * sinf( (psi + binWidth / fourf));
                y_coords[7] = total_vec_outer * sinf( (phi - binWidth / twof)) * sinf( (psi + binWidth / fourf));
                z_coords[7] = total_vec_outer * cosf( (psi + binWidth / fourf));

          }

            
          else if (fabsf(psi - 3.0433f) < small_psi_diff) {
                psi = psi - small_psi_diff;
                psi_corrected = psi;

                x_coords[2] = total_vec_inner * cosf( (phi + binWidth / twof)) * sinf( (psi + binWidth / fourf));
                y_coords[2] = total_vec_inner * sinf( (phi + binWidth / twof)) * sinf( (psi + binWidth / fourf));
                z_coords[2] = total_vec_inner * cosf( (psi + binWidth / fourf));

                x_coords[3] = total_vec_inner * cosf( (phi - binWidth / twof)) * sinf( (psi + binWidth / fourf));
                y_coords[3] = total_vec_inner * sinf( (phi - binWidth / twof)) * sinf( (psi + binWidth / fourf));
                z_coords[3] = total_vec_inner * cosf( (psi + binWidth / fourf));

                x_coords[6] = total_vec_outer * cosf( (phi + binWidth / twof)) * sinf( (psi + binWidth / fourf));
                y_coords[6] = total_vec_outer * sinf( (phi + binWidth / twof)) * sinf( (psi + binWidth / fourf));
                z_coords[6] = total_vec_outer * cosf( (psi + binWidth / fourf));

                x_coords[7] = total_vec_outer * cosf( (phi - binWidth / twof)) * sinf( (psi + binWidth / fourf));
                y_coords[7] = total_vec_outer * sinf( (phi - binWidth / twof)) * sinf( (psi + binWidth / fourf));
                z_coords[7] = total_vec_outer * cosf( (psi + binWidth / fourf));

                psi = psi_corrected;

                x_coords[0] = total_vec_inner * cosf( (phi - binWidth / twof)) * sinf( (psi - binWidth / fourf));
                y_coords[0] = total_vec_inner * sinf( (phi - binWidth / twof)) * sinf( (psi - binWidth / fourf));
                z_coords[0] = total_vec_inner * cosf( (psi - binWidth / fourf));

                x_coords[1] = total_vec_inner * cosf( (phi + binWidth / twof)) * sinf( (psi - binWidth / fourf));
                y_coords[1] = total_vec_inner * sinf( (phi + binWidth / twof)) * sinf( (psi - binWidth / fourf));
                z_coords[1] = total_vec_inner * cosf( (psi - binWidth / fourf));

                x_coords[4] = total_vec_outer * cosf( (phi - binWidth / twof)) * sinf( (psi - binWidth / fourf));
                y_coords[4] = total_vec_outer * sinf( (phi - binWidth / twof)) * sinf( (psi - binWidth / fourf));
                z_coords[4] = total_vec_outer * cosf( (psi - binWidth / fourf));

                x_coords[5] = total_vec_outer * cosf( (phi + binWidth / twof)) * sinf( (psi - binWidth / fourf));
                y_coords[5] = total_vec_outer * sinf( (phi + binWidth / twof)) * sinf( (psi - binWidth / fourf));
                z_coords[5] = total_vec_outer * cosf( (psi - binWidth / fourf));


          }
          //draw nomrally
          else {
                x_coords[0] = total_vec_inner * cosf( (phi - binWidth / twof)) * sinf( (psi - binWidth / fourf));
                y_coords[0] = total_vec_inner * sinf( (phi - binWidth / twof)) * sinf( (psi - binWidth / fourf));
                z_coords[0] = total_vec_inner * cosf( (psi - binWidth / fourf));

                x_coords[1] = total_vec_inner * cosf( (phi + binWidth / twof)) * sinf( (psi - binWidth / fourf));
                y_coords[1] = total_vec_inner * sinf( (phi + binWidth / twof)) * sinf( (psi - binWidth / fourf));
                z_coords[1] = total_vec_inner * cosf( (psi - binWidth / fourf));

                x_coords[4] = total_vec_outer * cosf( (phi - binWidth / twof)) * sinf( (psi - binWidth / fourf));
                y_coords[4] = total_vec_outer * sinf( (phi - binWidth / twof)) * sinf( (psi - binWidth / fourf));
                z_coords[4] = total_vec_outer * cosf( (psi - binWidth / fourf));

                x_coords[5] = total_vec_outer * cosf( (phi + binWidth / twof)) * sinf( (psi - binWidth / fourf));
                y_coords[5] = total_vec_outer * sinf( (phi + binWidth / twof)) * sinf( (psi - binWidth / fourf));
                z_coords[5] = total_vec_outer * cosf( (psi - binWidth / fourf));


                x_coords[2] = total_vec_inner * cosf( (phi + binWidth / twof)) * sinf( (psi + binWidth / fourf));
                y_coords[2] = total_vec_inner * sinf( (phi + binWidth / twof)) * sinf( (psi + binWidth / fourf));
                z_coords[2] = total_vec_inner * cosf( (psi + binWidth / fourf));

                x_coords[3] = total_vec_inner * cosf( (phi - binWidth / twof)) * sinf( (psi + binWidth / fourf));
                y_coords[3] = total_vec_inner * sinf( (phi - binWidth / twof)) * sinf( (psi + binWidth / fourf));
                z_coords[3] = total_vec_inner * cosf( (psi + binWidth / fourf));

                x_coords[6] = total_vec_outer * cosf( (phi + binWidth / twof)) * sinf( (psi + binWidth / fourf));
                y_coords[6] = total_vec_outer * sinf( (phi + binWidth / twof)) * sinf( (psi + binWidth / fourf));
                z_coords[6] = total_vec_outer * cosf( (psi + binWidth / fourf));

                x_coords[7] = total_vec_outer * cosf( (phi - binWidth / twof)) * sinf( (psi + binWidth / fourf));
                y_coords[7] = total_vec_outer * sinf( (phi - binWidth / twof)) * sinf( (psi + binWidth / fourf));
                z_coords[7] = total_vec_outer * cosf( (psi + binWidth / fourf));
          }

        glCube->Init(
                     x_coords[0], y_coords[0], z_coords[0],
                     x_coords[1], y_coords[1], z_coords[1],
                     x_coords[2], y_coords[2], z_coords[2],
                     x_coords[3], y_coords[3], z_coords[3],
                     x_coords[4], y_coords[4], z_coords[4],
                     x_coords[5], y_coords[5], z_coords[5],
                     x_coords[6], y_coords[6], z_coords[6],
                     x_coords[7], y_coords[7], z_coords[7],
                     worldScale
        );
		//for the animation system
		glCube->inner_radius = total_vec_inner/worldScale;
		glCube->outer_radius = total_vec_outer/worldScale;
		currentOuterRadius->push_back(0.0f);
	}

	
	bool hasData(){
		if (!etHistogram || !psiHistogram || !phiHistogram) return false;
		if (etHistogram->size()==0) return false;
		return true;
	}
};
#endif
