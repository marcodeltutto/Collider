
#include "EndcapFromXML.h"
#include "ACylinderFromXML.h"

//names for donut shapes
unsigned int EndcapFromXML::pix_hash = CzString::CalculateHash("Pixelecap");
unsigned int EndcapFromXML::sct_hash = CzString::CalculateHash("Siliconecap");
unsigned int EndcapFromXML::trt_hash = CzString::CalculateHash("TRTecap");
unsigned int EndcapFromXML::ecal_hash = CzString::CalculateHash("LArOuterEndcap");
unsigned int EndcapFromXML::hcal_hash = CzString::CalculateHash("ExtendedTILE");
unsigned int EndcapFromXML::muon_hash = CzString::CalculateHash("muonDetEcap");
//atrributes
unsigned int EndcapFromXML::zmin_hash = CzString::CalculateHash("zMin");
unsigned int EndcapFromXML::zmax_hash = CzString::CalculateHash("zMax");
unsigned int EndcapFromXML::rmin_hash	 = CzString::CalculateHash("rmin");
unsigned int EndcapFromXML::rmax_hash	 = CzString::CalculateHash("rmax");
unsigned int EndcapFromXML::c_hash	 = CzString::CalculateHash("c");
unsigned int EndcapFromXML::n_hash	 = CzString::CalculateHash("n");


int EndcapFromXML::LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node){
		
		//go through attributes, assign it
		for (CzXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
		{
			unsigned int name_hash = (*it)->getName().getHash();
			if (name_hash == zmin_hash) set_zmin((*it)->getValueAsFloat());
			if (name_hash == zmax_hash) set_zmax((*it)->getValueAsFloat());
			if (name_hash == rmin_hash) set_rmin((*it)->getValueAsFloat());
			if (name_hash == rmax_hash) set_rmax((*it)->getValueAsFloat());
			if (name_hash == c_hash) set_name((*it)->Value);
			if (name_hash == n_hash) set_nameHash(CzString::CalculateHash((*it)->Value.c_str()));

		}
		//parent is IwGxScene
		IwGxScene* glScene = NULL;
		//if (parent != NULL && parent->getClassTypeHash() ==CIwGameString::CalculateHash("gamescenegl")){

		glScene = ((IwGxScene*)parent);
		MeshGroup * scene_meshes = &glScene->meshes;
		
		//cleanup
		glScene->toBeCleaned.push_back(this);

		//is this a donut shape?
		//unsigned int nameHash = name.getHash();
		
		//ID endcaps
    if (nameHash != ecal_hash && nameHash !=hcal_hash && nameHash!=muon_hash){
//		GLEndcap * glEndcap_pos = new GLEndcap();
//		GLEndcap * glEndcap_neg = new GLEndcap();
		GLDonut * glEndcap_pos = new GLDonut();
		GLDonut * glEndcap_neg = new GLDonut();
		
        float zlow = ((zmin+zmax)/2.0) - (fabs(zmin-zmax) / 4.0);
        float zhigh = ((zmin+zmax)/2.0) + (fabs(zmin-zmax) / 4.0);
        
		glEndcap_pos->Init(zlow,zhigh,rmin,rmax,12,glScene->meshes.worldScale,nameHash);
		glEndcap_neg->Init(-zlow,-zhigh,rmin,rmax,12,glScene->meshes.worldScale,nameHash);
		
		if (glScene->name_color_map.find(nameHash) != glScene->name_color_map.end()){ 
			glEndcap_pos->setRGBA(glScene->name_color_map.find(nameHash)->second);
			glEndcap_neg->setRGBA(glScene->name_color_map.find(nameHash)->second);
		}
		//marco else CIwGameError::LogError("Warning missing colour schene in map for object");

		scene_meshes->children->push_back(glEndcap_pos);
		scene_meshes->children->push_back(glEndcap_neg);
		}
		//Calo endcaps
		else{
			GLDonut * glEndcap_pos = new GLDonut();
			GLDonut * glEndcap_neg = new GLDonut();

			glEndcap_pos->Init(zmin,zmax,rmin,rmax,12,glScene->meshes.worldScale,nameHash);
			glEndcap_neg->Init(-zmin,-zmax,rmin,rmax,12,glScene->meshes.worldScale,nameHash);
		
			if (glScene->name_color_map.find(nameHash) != glScene->name_color_map.end()){ 
				glEndcap_pos->setRGBA(glScene->name_color_map.find(nameHash)->second);
				glEndcap_neg->setRGBA(glScene->name_color_map.find(nameHash)->second);
			}
			// marco else CIwGameError::LogError("Warning missing colour schene in map for object");


			scene_meshes->children->push_back(glEndcap_pos);
			scene_meshes->children->push_back(glEndcap_neg);
		}

		return true;
	}

