#include "IwGxScene.h"
//#include "IwGameXml.h"
#include "CzXml.h"
#include "EventManager.h"
#include "Game.h"

#ifndef _CLASSIFICATIONGAME_H_
#define _CLASSIFICATIONGAME_H_


typedef CIwList<CzString> CIwGameStringList;

class ClassificationGame : public IzXomlResource{

public:

	ClassificationGame() : IzXomlResource() { 
		setClassType("ClassificationGame"); 
        eventManager = GAME->getEventManager();
	}
	virtual ~ClassificationGame(){
        //marco CIwGameError::LogError("ClassificationGame Destructor");
	};

	virtual int LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node);
    
	static unsigned int name_hash, eventList_hash, eventName_hash, description_hash, author_hash, url_hash, link_hash, linktitle_hash, tutorial_hash, level_hash;
	CzString description;
    CzString author;
    CzString name;
    CzString link;
    CzString linktitle;
    bool isTutorial;
    bool isMassGame;
    int level;
    
    EventManager* eventManager;
};



class ClassificationGameCreator : public IzXomlClassCreator
{
public:
	ClassificationGameCreator()
	{
		setClassName("ClassificationGame");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new ClassificationGame(); }
};
#endif
