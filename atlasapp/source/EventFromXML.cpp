//
//  EventFromXML.cpp
//  IwGxScene
//
//  Created by boddyadmin on 23/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//


#include "EventFromXML.h"

CzString EventFromXML::trks = CzString("track");
CzString EventFromXML::idtrks_key = CzString("tracks");
CzString EventFromXML::idtrks_key_2 = CzString("CombinedInDetTracks");
CzString EventFromXML::muontrks_key = CzString("ConvertedMuIdCBTracks");
CzString EventFromXML::muontrks_key_2 = CzString("ConvertedMBoyTracks");
CzString EventFromXML::count_key = CzString("count");

CzString EventFromXML::electron = CzString("electron");
CzString EventFromXML::photon = CzString("photon");
CzString EventFromXML::jet = CzString("jet");

unsigned int EventFromXML::pt_hash = CzString::CalculateHash("pt");
unsigned int EventFromXML::eta_hash = CzString::CalculateHash("eta");
unsigned int EventFromXML::phi_hash = CzString::CalculateHash("phi");
unsigned int EventFromXML::z0_hash = CzString::CalculateHash("z0");

unsigned int EventFromXML::MET_hash = CzString::CalculateHash("ETMis");
unsigned int EventFromXML::METetx_hash = CzString::CalculateHash("etx");
unsigned int EventFromXML::METety_hash = CzString::CalculateHash("ety");
unsigned int EventFromXML::type_hash = CzString::CalculateHash("type");

unsigned int EventFromXML::dataTime_hash = CzString::CalculateHash("dataTime");

//unsigned int EventsFromXML::parentIsCG_hash = CIwGameString::CalculateHash("parentIsCG");

void EventFromXML::Release(){
	eCalTowers.Release();
	hCalTowers.Release();
}
int EventFromXML::LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node){
    
    std::cout << "Loading Event" << std::endl;
    
    //marco CIwGameError::LogError(CzString(parent->getNameHash()).c_str());
    std::cout << CzString(parent->getNameHash()).c_str() << std::endl;   
 
	IwGxScene* glScene = NULL;
	glScene = ((IwGxScene*)parent);
    
	//loop over attributes
	for (CzXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
		{
			CzString  _val= (*it)->getValue();
            if (CzString::CalculateHash((*it)->Name.getString())==EventFromXML::dataTime_hash) {
                dataTime = _val.c_str();
                CzScene* liveScene = GAME->findScene("LiveScene");
                if(liveScene) {
                    CzXomlVariable* dateVar = (CzXomlVariable*) liveScene->getVariableManager()->findVariable("EventDescription");
                    //dateVar->setValue(_val.c_str());
                }
            }
		}
	//int num_children = node
	int num_children = node->Children.size();

	//loop over children
	//CIwGameError::LogError("count_key_hash = ",CIwGameString(count_key.getHash()).c_str());
        std::cout << "count_key_hash = " << CzString(count_key.getHash()).c_str() << std::endl;

	for (CzXmlNode::_Iterator it = node->begin(); it != node->end(); it++)
		{
            

			char * _val =(*it)->Value.getString();
			int str_length = strlen(_val);
            
			//eCal histogram
			if (CzString::CalculateHash((*it)->Name.getString())==CaloTowers::eCalhist_et_hash)
				(*it)->Value.getAsVectorOfFloat(eCalTowers.etHistogram);
			if (CzString::CalculateHash((*it)->Name.getString())==CaloTowers::eCalhist_psi_hash)
				(*it)->Value.getAsVectorOfInt(eCalTowers.psiHistogram);
			if (CzString::CalculateHash((*it)->Name.getString())==CaloTowers::eCalhist_phi_hash)
				(*it)->Value.getAsVectorOfInt(eCalTowers.phiHistogram);

			//hCal histogram
			if (CzString::CalculateHash((*it)->Name.getString())==CaloTowers::hCalhist_et_hash)
				(*it)->Value.getAsVectorOfFloat(hCalTowers.etHistogram);
			if (CzString::CalculateHash((*it)->Name.getString())==CaloTowers::hCalhist_psi_hash)
				(*it)->Value.getAsVectorOfInt(hCalTowers.psiHistogram);
			if (CzString::CalculateHash((*it)->Name.getString())==CaloTowers::hCalhist_phi_hash)
				(*it)->Value.getAsVectorOfInt(hCalTowers.phiHistogram);

			
			//Is this node a track node?
			bool isIdTrks = false;
			bool isMuonTrks = false;
			if (CzString::CalculateHash((*it)->Name.getString())== trks.getHash()){
				int nTrk = 0;
				for (CzXmlNode::_AttribIterator it_at = (*it)->attribs_begin(); it_at != (*it)->attribs_end(); it_at++){
					//CIwGameError::LogError("!!! track_at ",(*it_at)->Name.c_str());
                    std::cout << "!!! track_at " << (*it_at)->Name.c_str() << std::endl;
	
					if ((*it_at)->Name.getHash()==count_key.getHash()){ 
						nTrk = (*it_at)->getValueAsInt();
						//CIwGameError::LogError("!!! track_at ",(*it_at)->Name.c_str());
                                                std::cout << "!!! track_at " << (*it_at)->Name.c_str() << std::endl;
					}
					if ((*it_at)->Value.getHash()==idtrks_key.getHash() || (*it_at)->Value.getHash()==idtrks_key_2.getHash()){
						isIdTrks = true;
						//CIwGameError::LogError("!!! track_at ",(*it_at)->Name.c_str());
                                                std::cout << "!!! track_at " << (*it_at)->Name.c_str() << std::endl;
					}
					else if ((*it_at)->Value.getHash()==muontrks_key.getHash() || (*it_at)->Value.getHash()==muontrks_key_2.getHash())
                        isMuonTrks = true;
				}
				//CIwGameError::LogError("!!! made it ",(*it)->Name.c_str());
                                std::cout << "!!! made it " << (*it)->Name.c_str() << std::endl;
				//if (!(isIdTrks||isMuonTrks)) continue;

				Tracks * temp_tracks = 0;
				char * _name = 0;
				if (isIdTrks){ 
					temp_tracks = idTracks;
					_name = idtrks_key.getString();
				}
				else if (isMuonTrks){ 
					temp_tracks = muonTracks;
					_name = muontrks_key.getString();
				}
				else continue;
				
                float ptcut = 5.0;
                float muptcut = 10.0;
                if(GAME->getEventManager()->getLevel() == 1) {
                    ptcut = 5.0;
                    muptcut = 15.0;
                }
                else if(GAME->getEventManager()->getLevel() == 2) {
                    ptcut = 1.0;
                    muptcut = 10.0;
                }
                else if(GAME->getEventManager()->getLevel() == 3) {
                    ptcut = 0.2;
                    muptcut = 7.5;
                }
                
                if(isMuonTrks)
                    temp_tracks->Init(nTrk, _name, 270.0, 107.23, 40, muptcut);
                else
                    temp_tracks->Init(nTrk, _name, 270.0, 107.23, 40, ptcut);
				//CIwGameError::LogError("!!! made it here too, ntrk =",CIwGameString(nTrk).c_str());
				//CIwGameError::LogError("!!! made it here too, trkack name =",CIwGameString(_name).c_str());
                                std::cout << "!!! made it here too, ntrk =" << CzString(nTrk).c_str() << std::endl;
                                std::cout << "!!! made it here too, trkack name =" << CzString(_name).c_str() << std::endl;

				for (CzXmlNode::_Iterator it2 = (*it)->begin(); it2 != (*it)->end(); it2++)
				{
					//CIwGameError::LogError((*it2)->Name.c_str());
					//char * _val2 =(*it2)->Value.getString();
					//CIwGameError::LogError(_val2);
					//char * _val2 =(*it2)->Value.getString(;)
					
					//tracks
					//if ((*it2)->Name.c_str()))=='Track'){
					if (isIdTrks||isMuonTrks){
						//float vectors
						if ((*it2)->Name.getHash()==temp_tracks->pt_hash)
							(*it2)->Value.getAsVectorOfFloat(&temp_tracks->pt);
						else if ((*it2)->Name.getHash()==temp_tracks->phi0_hash)
							(*it2)->Value.getAsVectorOfFloat(&temp_tracks->phi0);
						else if ((*it2)->Name.getHash()==temp_tracks->cotTheta_hash)
							(*it2)->Value.getAsVectorOfFloat(&temp_tracks->cotTheta);
						else if ((*it2)->Name.getHash()==temp_tracks->d0_hash)
							(*it2)->Value.getAsVectorOfFloat(&temp_tracks->d0);
						else if ((*it2)->Name.getHash()==temp_tracks->z0_hash)
							(*it2)->Value.getAsVectorOfFloat(&temp_tracks->z0);
						//int vectors
						else if ((*it2)->Name.getHash()==temp_tracks->npix_hash)
							(*it2)->Value.getAsVectorOfInt(&temp_tracks->nPix);
						else if ((*it2)->Name.getHash()==temp_tracks->nsct_hash)
							(*it2)->Value.getAsVectorOfInt(&temp_tracks->nSCT);
						else if ((*it2)->Name.getHash()==temp_tracks->ntrt_hash)
							(*it2)->Value.getAsVectorOfInt(&temp_tracks->nTRT);
					}

					if (isMuonTrks){
						//float vectors
						if ((*it2)->Name.getHash()==temp_tracks->numpolyline_hash)
							(*it2)->Value.getAsVectorOfInt(&temp_tracks->numPoly);
						else if ((*it2)->Name.getHash()==temp_tracks->polylineX_hash)
							(*it2)->Value.getAsVectorOfFloat(&temp_tracks->polyX);
						else if ((*it2)->Name.getHash()==temp_tracks->polylineY_hash)
							(*it2)->Value.getAsVectorOfFloat(&temp_tracks->polyY);
						else if ((*it2)->Name.getHash()==temp_tracks->polylineZ_hash)
							(*it2)->Value.getAsVectorOfFloat(&temp_tracks->polyZ);

					}
					

				}
			}
			//is this node an ETmiss node?
			if (CzString::CalculateHash((*it)->Name.getString())== EventFromXML::MET_hash){
				//iterate over child elements
				for (CzXmlNode::_Iterator it2 = (*it)->begin(); it2 != (*it)->end(); it2++){
					if (CzString::CalculateHash((*it2)->Name.getString()) == EventFromXML::METetx_hash)
						MET_etx = (*it2)->Value.getAsFloat();
					if (CzString::CalculateHash((*it2)->Name.getString()) == EventFromXML::METety_hash)
						MET_ety = (*it2)->Value.getAsFloat();
				}
				eventHasMET = true;

			}
            //is this node an electron?
            if (CzString::CalculateHash((*it)->Name.getString()) == EventFromXML::electron.getHash()) {
                for (CzXmlNode::_Iterator it2 = (*it)->begin(); it2 != (*it)->end(); it2++){
					if (CzString::CalculateHash((*it2)->Name.getString()) == EventFromXML::pt_hash)
						(*it2)->Value.getAsVectorOfFloat(&el_pt);
					if (CzString::CalculateHash((*it2)->Name.getString()) == EventFromXML::eta_hash)
						(*it2)->Value.getAsVectorOfFloat(&el_eta);
					if (CzString::CalculateHash((*it2)->Name.getString()) == EventFromXML::phi_hash)
						(*it2)->Value.getAsVectorOfFloat(&el_phi);
                }
                
//                std::vector<float> trk_pt;
//                std::vector<float> trk_eta;
//                std::vector<float> trk_phi;
//                std::vector<float> trk_z0;
//                for (CIwGameXmlNode::_Iterator it = node->begin(); it != node->end(); it++)
//                {
//                    bool isIdTrks = false;
//                    if (CIwGameString::CalculateHash((*it)->Name.getString())== trks.getHash()){
//                        for (CIwGameXmlNode::_AttribIterator it_at = (*it)->attribs_begin(); it_at != (*it)->attribs_end(); it_at++){
//                            if ((*it_at)->Value.getHash()==idtrks_key.getHash()){
//                                isIdTrks = true;
//                            }
//                        }
//
//                        for (CIwGameXmlNode::_Iterator it2 = (*it)->begin(); it2 != (*it)->end(); it2++)
//                        {
//                            if (isIdTrks){
//                                //float vectors
//                                if ((*it2)->Name.getHash()==Tracks::z0_hash)
//                                    (*it2)->Value.GetAsVectorOfFloat(&trk_z0);
//                                if ((*it2)->Name.getHash()==Tracks::pt_hash)
//                                    (*it2)->Value.GetAsVectorOfFloat(&trk_pt);
//                                if ((*it2)->Name.getHash()==Tracks::cotTheta_hash)
//                                    (*it2)->Value.GetAsVectorOfFloat(&trk_eta);
//                                if ((*it2)->Name.getHash()==Tracks::phi0_hash)
//                                    (*it2)->Value.GetAsVectorOfFloat(&trk_phi);
//                            }
//                        }
//
//                    }
//
//                }
//                
//                for(int i = 0; i<el_pt.size(); ++i) {
//                    int z0index = getTrackIndex(trk_pt, trk_eta, trk_phi, el_pt[i], el_eta[i], el_phi[i]);
//                    std::cout << z0index << std::endl;
//                    el_z0.push_back(trk_z0[z0index]);
//                }
//                
//                
//                    if (CIwGameString::CalculateHash((*it2)->Name.getString()) == CIwGameString::CalculateHash("trackIndex")){
//                        std::vector<float> trkz0;
//                        for (CIwGameXmlNode::_Iterator it = node->begin(); it != node->end(); it++)
//                        {
//                            bool isIdTrks = false;
//                            if (CIwGameString::CalculateHash((*it)->Name.getString())== trks.getHash()){
//                                for (CIwGameXmlNode::_AttribIterator it_at = (*it)->attribs_begin(); it_at != (*it)->attribs_end(); it_at++){
//                                    if ((*it_at)->Value.getHash()==idtrks_key.getHash()){
//                                        isIdTrks = true;
//                                    }
//                                }
//                                
//                                for (CIwGameXmlNode::_Iterator it2 = (*it)->begin(); it2 != (*it)->end(); it2++)
//                                {
//                                    if (isIdTrks){
//                                        //float vectors
//                                        if ((*it2)->Name.getHash()==Tracks::z0_hash)
//                                            (*it2)->Value.GetAsVectorOfFloat(&trkz0);
//                                    }
//                                }
//
//                            }
//
//                        }
//                        if(trkz0.size() > 0) {
//                            std::vector<int> elTrackIndex;
//                            (*it2)->Value.GetAsVectorOfInt(&elTrackIndex);
//                            for(unsigned int i=0; i<el_pt.size(); ++i){
//                                if (elTrackIndex.at(i) >= 0)
//                                    el_z0.push_back(trkz0.at(elTrackIndex.at(i)));
//                                else
//                                    el_z0.push_back(0);
//                            }
//                        }
//                    }
//				}
            }
            //is this node a photon?
            if (CzString::CalculateHash((*it)->Name.getString()) == EventFromXML::photon.getHash()) {
                for (CzXmlNode::_Iterator it2 = (*it)->begin(); it2 != (*it)->end(); it2++){
					if (CzString::CalculateHash((*it2)->Name.getString()) == EventFromXML::pt_hash)
						(*it2)->Value.getAsVectorOfFloat(&ph_pt);
					if (CzString::CalculateHash((*it2)->Name.getString()) == EventFromXML::eta_hash)
						(*it2)->Value.getAsVectorOfFloat(&ph_eta);
					if (CzString::CalculateHash((*it2)->Name.getString()) == EventFromXML::phi_hash)
						(*it2)->Value.getAsVectorOfFloat(&ph_phi);
				}
            }
            //is this node a jet?
            if (CzString::CalculateHash((*it)->Name.getString()) == EventFromXML::jet.getHash()) {
                for (CzXmlNode::_Iterator it2 = (*it)->begin(); it2 != (*it)->end(); it2++){
					if (CzString::CalculateHash((*it2)->Name.getString()) == EventFromXML::pt_hash)
						(*it2)->Value.getAsVectorOfFloat(&jet_pt);
					if (CzString::CalculateHash((*it2)->Name.getString()) == EventFromXML::eta_hash)
						(*it2)->Value.getAsVectorOfFloat(&jet_eta);
					if (CzString::CalculateHash((*it2)->Name.getString()) == EventFromXML::phi_hash)
						(*it2)->Value.getAsVectorOfFloat(&jet_phi);
				}
            }
		
		}
	


	//glScene->currentEvent = this;

	//detector mesh group
	MeshGroup * scene_meshes = &glScene->meshes;
	idTracks->worldScale = scene_meshes->worldScale;
	muonTracks->worldScale = scene_meshes->worldScale;
	eCalTowers.worldScale = scene_meshes->worldScale;
	hCalTowers.worldScale = scene_meshes->worldScale;
	//event mesh group
	MeshGroup * event_meshes = &glScene->eventMeshes;
	

	//have MET
	if (eventHasMET){
		GLArrow * mArrow  = new GLArrow();
		mArrow->Init(0.8,0.2,scene_meshes->worldScale);
		mArrow->set(MET_etx,MET_ety);
		mArrow->setRGBA(glScene->name_color_map[EventFromXML::MET_hash]);
		event_meshes->children->push_back(mArrow);
		metArrow = mArrow;
	}
	//std::cout <<" metArrow point 2 = "<< metArrow->verts[2].x<<","<<metArrow->verts[2].y<<","<<metArrow->verts[2].z<<std::endl;
    
	//have id tracks?
	if (idTracks->pt.size()>0){
		idTracks->genHelixInfo();
		idTracks->genPolyLinePoints(idTracks->nPoly);
		idTracks->setVertices();
	}
    
	//have muon tracks?
	if (muonTracks->pt.size()>0){
#ifdef _DEBUG
		std::cout <<"!!!!! have "<<muonTracks->pt.size()<<" muon tracks, npoly X,Y,Z= "<<muonTracks->polyX.size()<<","<<muonTracks->polyY.size() <<","<<muonTracks->polyZ.size() <<" numPoly 0 = "<<muonTracks->numPoly.at(0) <<" numPoly 1 = "<<muonTracks->numPoly.at(1) <<std::endl;
#endif
		muonTracks->z_max = 1000*1000;
		muonTracks->rho_max2 = 140*140;
		muonTracks->setVertices();
	}
    
    for (unsigned int i=0; i<el_pt.size();++i){
        if(el_pt[i] < 15.0)
            continue;
        GLCone* coneTest = new GLCone();
        
        coneTest->Init(el_pt[i], el_eta[i], el_phi[i], 0, 1, 16, scene_meshes->worldScale);
        coneTest->setRGBA(50, 250, 100, 80);
        event_meshes->children->push_back(coneTest);
        

    }

    /*for (unsigned int i=0; i<ph_pt.size();++i){
        if(ph_pt[i] < 15.0)
            continue;
        GLCone* coneTest = new GLCone();
        
        coneTest->Init(ph_pt[i], ph_eta[i], ph_phi[i], 0, 2, 16, scene_meshes->worldScale);
        coneTest->setRGBA(250, 250, 30, 80);
        event_meshes->children->push_back(coneTest);
    }
    
    
    for (unsigned int i=0; i<jet_pt.size();++i){
        if(jet_pt[i] < 15.0)
            continue;
        GLCone* coneTest = new GLCone();
        
        coneTest->Init(jet_pt[i], jet_eta[i], jet_phi[i], 0, 3, 16, scene_meshes->worldScale);
        coneTest->setRGBA(120, 120, 120, 60);
        event_meshes->children->push_back(coneTest);
    }*/
	
	//has ecal histogram
	if (eCalTowers.hasData()) eCalTowers.defineGLCubes();
	
	//has hcal histogram
	if (hCalTowers.hasData()) hCalTowers.defineGLCubes();
	
	muonTracks->setRGBA(glScene->name_color_map[Tracks::muonTracks_hash]);
	muonTracks->nameHash= Tracks::muonTracks_hash;

	idTracks->setRGBA(glScene->name_color_map[Tracks::idTracks_hash]);
	idTracks->nameHash= Tracks::idTracks_hash;

	//scene_meshes->children->push_back(muonTracks);
	//scene_meshes->children->push_back(idTracks);
	event_meshes->children->push_back(muonTracks);
	event_meshes->children->push_back(idTracks);

	//add any calo towers
	for (unsigned int iTower=0;iTower < eCalTowers.caloTowerCubes->size();iTower++){
		//set colour / hash
		eCalTowers.caloTowerCubes->at(iTower)->setRGBA(glScene->name_color_map[CaloTowers::ecalTowers_hash]);
		eCalTowers.caloTowerCubes->at(iTower)->nameHash = CaloTowers::ecalTowers_hash;
		//scene_meshes->children->push_back(eCalTowers.caloTowerCubes->at(iTower));
		event_meshes->children->push_back(eCalTowers.caloTowerCubes->at(iTower));
	}
	
	
	for (unsigned int iTower=0;iTower < hCalTowers.caloTowerCubes->size();iTower++){
		//set colour / hash
		hCalTowers.caloTowerCubes->at(iTower)->setRGBA(glScene->name_color_map[CaloTowers::hcalTowers_hash]);
		hCalTowers.caloTowerCubes->at(iTower)->nameHash = CaloTowers::hcalTowers_hash;
		//scene_meshes->children->push_back(hCalTowers.caloTowerCubes->at(iTower));
		event_meshes->children->push_back(hCalTowers.caloTowerCubes->at(iTower));
	}
    
	
	//set this event to parent scene's currentEvent
	glScene->currentEvent = (EventFromXML*) this;
	//add to list of objects to be cleaned
	glScene->toBeCleaned.push_back(this);

    std::cout << "LOADED EVENT" << std::endl;
    
	return true;
}


/* For loading a list of events from XML */

int EventsFromXML::LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node){
//
//		URIs.clear();
//		eventTypes.clear();
//		//set parent type
//		IwGxScene* glScene = NULL;
//		parentIsCG = false;
//
//		
//		//event hash
//		static unsigned int event_hash = CIwGameString::CalculateHash("Event");
//
//		//iterate over nodes, each with value "Event"
//		for (CIwGameXmlNode::_Iterator it = node->begin(); it != node->end(); it++){	
////#ifdef _DEBUG
//			//std::cout <<"EventsFromXML::LoadFromXoml:: node = "<<(*it)->GetValue().c_str()<<std::endl;
////#endif		
//			if (CIwGameString::CalculateHash((*it)->Name.getString()) == event_hash){
//
//				//store event URL
//				URIs.push_back((*it)->GetValue());
//				//store its type (attribute)
//				for (CIwGameXmlNode::_AttribIterator atIt = (*it)->attribs_begin(); atIt != (*it)->attribs_end(); atIt++){
//					if (CIwGameString::CalculateHash((*atIt)->Name.getString()) == EventFromXML::type_hash)
//						eventTypes.push_back((*atIt)->GetValue());
//				}
//			}
//			
//		}
//
//		//iterate over Events attributes
//		for (CIwGameXmlNode::_AttribIterator atIt = node->attribs_begin(); atIt != node->attribs_end(); atIt++){
//			unsigned int hash = CIwGameString::CalculateHash((*atIt)->Name.getString());
//			if (hash == parentIsCG_hash) parentIsCG = true;
//			
//		}
//
//#ifdef _DEBUG
//		std::cout <<"Events and types in the EventList: parentIsCG = "<<parentIsCG<<"\n";
//		for (unsigned int iEvent=0;iEvent<URIs.size();iEvent++)
//			std::cout <<URIs.at(iEvent).c_str()<<" , "<<eventTypes.at(iEvent).c_str()<<std::endl;
//#endif
//
//		if (!parentIsCG) glScene = static_cast<IwGxScene*>(parent);
//		else {
//			ClassificationGame * cGame = static_cast<ClassificationGame*>(parent);
//			cGame->eventsList = this;
//			glScene = cGame->scene;
//		}
//
//
//		glScene->toBeCleaned.push_back(this);
		return true;
	}
