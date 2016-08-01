#include "ClassificationGame.h"
//#include "IwGameTemplates.h"
#include "CzTemplates.h"
#include "GameLogicScene.h"
#include "HistogramScene.h"
#include <iostream>
#include <sstream>


//unsigned int ClassificationGame:: = CIwGameString::CalculateHash("");
unsigned int ClassificationGame::name_hash = CzString::CalculateHash("name");
unsigned int ClassificationGame::eventName_hash = CzString::CalculateHash("event");
unsigned int ClassificationGame::description_hash = CzString::CalculateHash("description");
unsigned int ClassificationGame::author_hash = CzString::CalculateHash("author");
unsigned int ClassificationGame::url_hash = CzString::CalculateHash("url");
unsigned int ClassificationGame::link_hash = CzString::CalculateHash("link");
unsigned int ClassificationGame::linktitle_hash = CzString::CalculateHash("title");
unsigned int ClassificationGame::tutorial_hash = CzString::CalculateHash("tutorial");
unsigned int ClassificationGame::level_hash = CzString::CalculateHash("level");



int ClassificationGame::LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node){
    
    eventManager->Reset();
	
    level = 3;
    isTutorial = false;
  
	//loop over attributes
	for (CzXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); ++it){
			unsigned int name_hash = (*it)->getName().getHash();
			
			if (name_hash == ClassificationGame::name_hash) name = (*it)->getValue();
            else if (name_hash == ClassificationGame::tutorial_hash) {
                if (CzString::CalculateHash((*it)->getValue().c_str()) == CzString::CalculateHash("true"))
                    isTutorial = true;
                else
                    isTutorial = false;
            }
            else if (name_hash == ClassificationGame::level_hash) level = (*it)->getValueAsInt();
            else if (name_hash == ClassificationGame::author_hash) author= (*it)->getValue();
			
		}
	
	//loop over child elements
	for (CzXmlNode::_Iterator it = node->begin(); it != node->end(); ++it){

			unsigned int name_hash = (*it)->GetName().getHash();

        if (name_hash == ClassificationGame::eventName_hash) {
            
            eventManager->addEvent((*it)->getAttribute("file")->getValue().c_str(),(*it)->getAttribute("type")->getValue().c_str());
        }
        else if (name_hash == ClassificationGame::link_hash) {
            for (CzXmlNode::_AttribIterator it2 = (*it)->attribs_begin(); it2 != (*it)->attribs_end(); ++it2){
                unsigned int name_hash2 = (*it2)->getName().getHash();
                if (name_hash2 == ClassificationGame::url_hash) link = (*it2)->getValue();
                else if (name_hash2 == ClassificationGame::linktitle_hash) linktitle = (*it2)->getValue().c_str();
            }
        }
//        else if (name_hash == ClassificationGame::histo_hash) {
//            for (CIwGameXmlNode::_AttribIterator it2 = (*it)->attribs_begin(); it2 != (*it)->attribs_end(); ++it2){
//                unsigned int name_hash2 = (*it2)->getName().getHash();
//                int h_nbins = 0;
//                float h_startbin = 0.0;
//                float h_endbin = 0.0;
//                unsigned int h_id = 0;
//                
//                if(name_hash2 == CIwGameString::CalculateHash("name"))
//                    h_id = name.getHash() + (*it2)->GetValue().getHash();
//                else if(name_hash2 == CIwGameString::CalculateHash("bins"))
//                    h_nbins = (*it2)->GetValueAsInt();
//                else if(name_hash2 == CIwGameString::CalculateHash("start"))
//                    h_startbin = (*it2)->GetValueAsFloat();
//                else if(name_hash2 == CIwGameString::CalculateHash("end"))
//                    h_endbin = (*it2)->GetValueAsFloat();
//                
//                Histogram* tmpHist = new Histogram(h_id, h_nbins, h_startbin, h_endbin);
//                
//                
//            }
//        }
			
	}
    CzScene* selScene = GAME->findScene("SelectionScene");
    if(selScene) {
        CzXomlVariableManager* varMan = selScene->getVariableManager();
        
        varMan->setVariable("Title", name.c_str());
        std::stringstream ss;
        ss << "by " << author.str();
        varMan->setVariable("Author",ss.str().c_str());
        varMan->setVariable("Description", description.c_str());
        
        int nEvents = eventManager->getNEvents();
        eventManager->setTutorial(isTutorial);
        eventManager->setLevel(level);
        
        CzXomlVariableArray* el = (CzXomlVariableArray*) varMan->findVariable("EventList");
        el->Init(nEvents);
        for(int i=0; i<nEvents; ++i) {
            std::stringstream evnum;
            evnum << (i+1);
            el->setValue(i, evnum.str().c_str());
        }
    }
    
    CzScene* webScene = GAME->findScene("WebScene");
    if(webScene) {
        webScene->getVariableManager()->findVariable("WebURL")->setValue(link.getString());
    }

    GameLogicScene* gameScene = (GameLogicScene*) GAME->findScene("GameScene");
    
    if(gameScene) {
        CzXomlVariableManager* varMan = gameScene->getVariableManager();
        CzXomlVariableArray* cs = (CzXomlVariableArray*) varMan->findVariable("Choices");
      
        eventManager->setEvent(gameScene->currentEvent);
        
        varMan->setVariable("ChoicesVisible", "false");
        cs->Init(1);
        cs->setValue(0,"");
        std::vector<std::string> choices = eventManager->getTypes();

        eventManager->setTutorial(isTutorial);

        if(choices.size() > 1) {
            varMan->setVariable("ChoicesVisible", "true");
            cs->Init(choices.size());
            for(int i=0; i<choices.size(); ++i) {
                cs->setValue(i, choices[i].c_str());
            }
        }
        
        if(!name.isEmpty()) varMan->setVariable("GameName", name.c_str());
        if(!description.isEmpty()) varMan->setVariable("Description", description.c_str());
        CzUILabel* webButton = (CzUILabel*) gameScene->findActor("webButton");
        webButton->setText(linktitle.c_str());
        //std::cout << linktitle.c_str() << std::endl;
        //varMan->setVariable("Link", link.c_str());
    }
    
	return true;
}

