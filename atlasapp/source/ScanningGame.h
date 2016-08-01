//
//  ScanningGame.h
//  GameSceneGL
//
//  Created by Tom McLaughlan on 14/05/2013.
//
//

#ifndef __GameSceneGL__ScanningGame__
#define __GameSceneGL__ScanningGame__

#include "IwGxScene.h"
#include "IwGameXml.h"

#ifndef _CLASSIFICATIONGAME_H_
#define _CLASSIFICATIONGAME_H_


typedef CIwList<CIwGameString> CIwGameStringList;

class ClassificationGame : public IIwGameXomlResource{
    
public:
    
	ClassificationGame() : IIwGameXomlResource() {
		setClassType("ClassificationGame");
		links = NULL;
		eventsList = NULL;
		scene = NULL;
        
		introScene = NULL;
		eventsScene = NULL;
	}
	virtual ~ClassificationGame(){
		if (links!= NULL) delete links;
		//if (eventsList!= NULL) delete eventsList; //tidied up elsewhere
	};
    
    
	virtual bool LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
	IwGxScene * eventDisplay;
	static unsigned int name_hash, eventsFileName_hash, introText_hash,completeText_hash, links_hash;
	CIwGameString introText, completeText;
	CIwGameStringList * links;
    
	EventsFromXML * eventsList;
	IwGxScene * scene;
    
	CIwGameScene * introScene;
	CIwGameScene * eventsScene;
    
	void createIntroScene();
	void createEventsScene();
    
};



class ClassificationGameCreator : public IIwGameXomlClassCreator
{
public:
	ClassificationGameCreator()
	{
		setClassName("ClassificationGame");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new ClassificationGame(); }
};
#endif