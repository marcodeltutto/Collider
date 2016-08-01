//
//  EventFromXML.h
//  IwGxScene
//
//  Created by boddyadmin on 23/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

//#include "IwGameXoml.h"
#include "CzXoml.h"
#include "IwGxScene.h"
#include "Tracks.h"
#include "CaloTowers.h"

#include <vector>
#include <math.h>

#ifndef EventFromXML_h
#define EventFromXML_h


/**
@brief This class is used to read the data in an XML event (with Tracks, calorimeter histograms, ptmiss etc.), created whenever an XML element with the "Event" tag is read by the XML parser. Used for local and distributed files (i.e. at CERN somewhere).

 Tracks objects are created if a Tracks-tag  is read by XML-parser. Parser looks for attribute-name "Tracks" (ID tracks) and "Combined_tracks" (combined muon tracks". If found, created dynamically and initialised using data from XML parser, then added to event-display (IwGxScene) renderer's MeshGroup.

Similarly calorimeter bins (Cuboids, GLCube) and ptmiss (GLArrow) are created dynamically and added to event-display's MeshGroup for rendering based on XML content of <event>...</event>. 
*/
class EventFromXML: public IzXomlResource{
public: 
    EventFromXML(): IzXomlResource(){
        setClassType("Event");
		eCalTowers.barrel_rho = 148.17f;
		eCalTowers.endcap_z = 380.073f;
		eCalTowers.endcap_region = 0.427f;

		hCalTowers.barrel_rho = 386.0f;
		hCalTowers.endcap_z = 615.0f;
		hCalTowers.endcap_region = 0.560f;

		idTracks = new Tracks();
		muonTracks = new Tracks();

		MET_phi = MET_scalarSum = MET_etx = MET_ety = -999.f;
		eventHasMET = false;
		metArrow = NULL;

		
    };
	~EventFromXML(){
		//delete idTracks;
		//delete muonTracks;
	};
    
    //std::vector<float> EMcal_phi, EMcal_eta, EMcal_et, Hcal_eta,Hcal_phi, Hcal_et;
	Tracks * idTracks;
	Tracks * muonTracks;

	CaloTowers eCalTowers;
	CaloTowers hCalTowers;

	GLArrow * metArrow;
    
    virtual int LoadFromXoml(IzXomlResource* parent, bool load_children = true, CzXmlNode* node = 0);
    virtual void Release();
	//keys for XML reader
    static CzString trks, idtrks_key, muontrks_key, idtrks_key_2, muontrks_key_2, count_key, electron, photon, jet;
	static unsigned int MET_hash, METetx_hash, METety_hash, type_hash, pt_hash, eta_hash, phi_hash, z0_hash, dataTime_hash;
	float MET_phi, MET_scalarSum, MET_etx, MET_ety;
	bool eventHasMET;
    std::string dataTime;
    
    std::vector<float> el_pt;
    std::vector<float> el_eta;
    std::vector<float> el_phi;
    std::vector<float> el_z0;
	
    std::vector<float> jet_pt;
    std::vector<float> jet_eta;
    std::vector<float> jet_phi;
    std::vector<float> jet_z0;
	
    std::vector<float> ph_pt;
    std::vector<float> ph_eta;
    std::vector<float> ph_phi;
    std::vector<float> ph_z0;
	
};


class EventFromXMLCreator : public IzXomlClassCreator
{
public:
	EventFromXMLCreator()
	{
		setClassName("Event");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new EventFromXML(); }
};


/* This is for streaming a simple list of URIs, ostensibly from the ATLAS-Live server at CERN or some other file.*/

/** @brief
 Simple class to read an XML file and store URLs (or local locations) of XML event files formatted to be read by EventForXML.

 For the classification games, the type of the event could be added as an attribute. See data/higgs_hunt_events.xml for example.
 */
class EventsFromXML: public IzXomlResource{
public: 
	std::vector<CzString> URIs;
	std::vector<CzString> eventTypes;
	bool parentIsCG;
	static unsigned int parentIsCG_hash;

    EventsFromXML(): IzXomlResource(){
        setClassType("Events");
		parentIsCG = false;
	}
	~EventsFromXML(){
		//for (int iObj = (int)URIs.size()-1; iObj>=0;iObj--)
		//	delete URIs.at(iObj);
		
		URIs.clear();
		eventTypes.clear();
	
	};
	virtual int LoadFromXoml(IzXomlResource* parent, bool load_children = true, CzXmlNode* node=0);
};


class EventsFromXMLCreator : public IzXomlClassCreator
{
public:
	EventsFromXMLCreator()
	{
		setClassName("Events");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new EventsFromXML(); }
};



#endif
