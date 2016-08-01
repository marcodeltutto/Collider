
#include "ABoxFromXML.h"

#include "ACylinderFromXML.h"

unsigned int ABoxFromXML::zi_hash =  CzString::CalculateHash("zi");
unsigned int ABoxFromXML::zo_hash =  CzString::CalculateHash("zo");
unsigned int ABoxFromXML::ri_hash =  CzString::CalculateHash("ri");
unsigned int ABoxFromXML::ro_hash =  CzString::CalculateHash("ro");
unsigned int ABoxFromXML::w_hash =  CzString::CalculateHash("w");
unsigned int ABoxFromXML::eta_hash =  CzString::CalculateHash("eta");
unsigned int ABoxFromXML::phi_hash =  CzString::CalculateHash("phi");
unsigned int ABoxFromXML::dphi_hash =  CzString::CalculateHash("dphi");



int ABoxFromXML::LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node){
		//return true;
    
        dphi = 0;
    
		//go through attributes, assign it
		for (CzXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
		{
			unsigned int name_hash = (*it)->getName().getHash();
			if (name_hash == ABoxFromXML::zi_hash) set_zi((*it)->getValueAsFloat());
			if (name_hash == ABoxFromXML::zo_hash) set_zo((*it)->getValueAsFloat());
			if (name_hash == ABoxFromXML::ri_hash ) set_ri((*it)->getValueAsFloat());
			if (name_hash == ABoxFromXML::ro_hash ) set_ro((*it)->getValueAsFloat());
			if (name_hash == ABoxFromXML::w_hash  ) set_w((*it)->getValueAsFloat());
			if (name_hash == ABoxFromXML::eta_hash  ) set_eta((*it)->getValueAsFloat());
			if (name_hash == ABoxFromXML::dphi_hash  ) set_dphi((*it)->getValueAsFloat());
			if (name_hash == ABoxFromXML::phi_hash  ) (*it)->Value.getAsListOfInt(phi);

		}
		//parent is IwGxScene
		IwGxScene* glScene = NULL;

		glScene = ((IwGxScene*)parent);
		MeshGroup * scene_meshes = &glScene->meshes;
		
		//cleanup
		glScene->toBeCleaned.push_back(this);

		
		//work out the coords. of the detector boxes
		//and add them to the scene meshgroup
		float phi_default = 22.5;
		float x[] = {0,0,0,0};
		float y[] = {0,0,0,0};
		float radius =  ri + ro / 2;
		float width = ro - ri;

		float fPI = (float) PI;
		for (int i =0;i<8;i++){
			if (phi[i] <0) break;
            //if(phi[i] > 5) break; // cutaway
			int stationPhi = phi[i];
			float phiTemp = (stationPhi-1) * fPI / 4;
			if (phiTemp < 0) phiTemp += fPI;
			else phiTemp -= fPI;
			phiTemp *=  180 / fPI;
            phiTemp += dphi;
			float angle1 = phiTemp - phi_default /2;
			//float angle2 = phi_default;
			float phi1 = fPI / 180 *angle1;
			float phi2 = fPI / 180 *(angle1 + phi_default);
			float cosphi1 = (float)cos(phi1);
			float cosphi2 = (float)cos(phi2);
			float sinphi1 = (float)sin(phi1);
			float sinphi2 = (float)sin(phi2);

			x[0]= (radius - width/2) * cosphi1;
			y[0]= (radius - width/2) * sinphi1;
            
			x[1]= (radius - width/2) * cosphi2;
			y[1]= (radius - width/2) * sinphi2;
			
			x[2]= (radius + width/2) * cosphi2;
			y[2]= (radius + width/2) * sinphi2;
			
			x[3]= (radius + width/2) * cosphi1;
			y[3]= (radius + width/2) * sinphi1;
			
            
			GLCube * glCube_temp = new GLCube();
			glCube_temp->Init(x[0],y[0],x[1],y[1],x[2],y[2],x[3],y[3],zi,zo,glScene->meshes.worldScale);
			glCube_temp->nameHash = ACylinderFromXML::muonDet_hash;
			//set the colour of the cube
			glCube_temp->setRGBA(glScene->name_color_map[ACylinderFromXML::muonDet_hash]);
			scene_meshes->children->push_back(glCube_temp);
            
            //mirror the muon system
			GLCube * glCube_temp_2 = new GLCube();
            glCube_temp_2->Init(x[0],y[0],x[1],y[1],x[2],y[2],x[3],y[3],-1*zi,-1*zo,glScene->meshes.worldScale);
			glCube_temp_2->nameHash = ACylinderFromXML::muonDet_hash;
			//set the colour of the cube
			glCube_temp_2->setRGBA(glScene->name_color_map[ACylinderFromXML::muonDet_hash]);
			scene_meshes->children->push_back(glCube_temp_2);
			
			
		}
    
//        GLCube * glCube_temp_3 = new GLCube();
//        glCube_temp_3->Init(-2640,-2640,2640,-2640,2640,2640,-2640,2640,-2640,2640,glScene->meshes.worldScale);
//        glCube_temp_3->nameHash = ACylinderFromXML::muonDet_hash;
//        //set the colour of the cube
//        glCube_temp_3->setRGBA(150,150,0,10);
//        scene_meshes->children->push_back(glCube_temp_3);

		return true;
	}
