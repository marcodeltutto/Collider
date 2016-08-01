
#include "ACylinderFromXML.h"
//names for donut shapes
unsigned int ACylinderFromXML::ecalName_hash = CzString::CalculateHash("LAr");
unsigned int ACylinderFromXML::hcalName_hash = CzString::CalculateHash("HCAL");
unsigned int ACylinderFromXML::trtName_hash = CzString::CalculateHash("TRT");
//atrributes
unsigned int ACylinderFromXML::zmin_hash = CzString::CalculateHash("zMin");
unsigned int ACylinderFromXML::zmax_hash = CzString::CalculateHash("zMax");
unsigned int ACylinderFromXML::r0_hash	 = CzString::CalculateHash("r0");
unsigned int ACylinderFromXML::n_hash	 = CzString::CalculateHash("n");
unsigned int ACylinderFromXML::thickness_hash	 = CzString::CalculateHash("thickness");

unsigned int ACylinderFromXML::tileEcap_hash	 = CzString::CalculateHash("ExtendedTILE");
unsigned int ACylinderFromXML::LArEcap_hash	 = CzString::CalculateHash("LArOuterEndcap");
unsigned int ACylinderFromXML::pixName_hash	 = CzString::CalculateHash("pixel");
unsigned int ACylinderFromXML::sctName_hash	 = CzString::CalculateHash("silicon");

unsigned int ACylinderFromXML::muonDet_hash	 = CzString::CalculateHash("muonDet");


int          ACylinderFromXML::LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node){
		
		//go through attributes, assign it
		for (CzXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
		{
			unsigned int name_hash = (*it)->getName().getHash();
			if (name_hash == zmin_hash) set_zmin((*it)->getValueAsFloat());
			if (name_hash == zmax_hash) set_zmax((*it)->getValueAsFloat());
			if (name_hash == r0_hash) set_r0((*it)->getValueAsFloat());
			if (name_hash == thickness_hash) set_thickness((*it)->getValueAsFloat());
			if (name_hash == n_hash) set_nameHash(CzString::CalculateHash((*it)->Value.c_str()));

		}
		//parent is IwGxScene
		IwGxScene* glScene = NULL;

		glScene = ((IwGxScene*)parent);
		MeshGroup * scene_meshes = &glScene->meshes;
		
		//cleanup
		glScene->toBeCleaned.push_back(this);

		//is this a donut shape?
		//unsigned int nameHash = name.getHash();


		if (nameHash == ecalName_hash || nameHash == hcalName_hash || nameHash == trtName_hash || nameHash==muonDet_hash){
#ifdef _DEBUG
			CIwGameError::LogError("now making a donut");
#endif
			float rmin = r0 - thickness / 2;
			float rmax = r0 + thickness / 2;

			GLDonut * glDonut_temp = new GLDonut();
			glDonut_temp->Init(zmin,zmax,rmin,rmax,12,glScene->meshes.worldScale);
			//set colour if name is in the map
			if (glScene->name_color_map.find(nameHash) != glScene->name_color_map.end()) glDonut_temp->setRGBA(glScene->name_color_map.find(nameHash)->second);
			//marco else CIwGameError::LogError("Warning missing colour schene in map for object");
			
			scene_meshes->children->push_back(glDonut_temp);
		}
		//then it's a cylinder shape
		else{
			GLCylinder * glCylinder_temp = new GLCylinder();
			glCylinder_temp->Init(zmin,zmax,r0,12,glScene->meshes.worldScale);
			
			//set colour if name is in the map
			if (glScene->name_color_map.find(nameHash) != glScene->name_color_map.end()) glCylinder_temp->setRGBA(glScene->name_color_map.find(nameHash)->second);
			//marco else CIwGameError::LogError("Warning missing colour schene in map for object");
			//add to scene
			scene_meshes->children->push_back(glCylinder_temp);
		}
		
		return true;
	}
