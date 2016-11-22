//
//  EventManager.cpp
//  GameSceneGL
//
//  Created by Tom McLaughlan on 08/05/2013.
//
//

#include "EventManager.h"
#include "Game.h"
#include "IwGxScene.h"
#include <algorithm>
#include "CzXml.h"

EventManager::EventManager() {
    Reset();
}

void EventManager::Reset() {
    urls.clear();
    types.clear();
    descriptions.clear();
    correct_results.clear();
    incorrect_results.clear();
    guesses.clear();
    prefix = "";
    currentEvent = 0;
    tut = false;
    level = 3;
}

bool EventManager::DownloadLiveEvents() {
    // this method updates the live event list
  
  std::cout << "MARCO 1." << std::endl;

    Reset();
  
  std::cout << "MARCO 2." << std::endl;

  
    //CzXmlParser p;
  CzXmlParser*    p = new CzXmlParser();
  
  std::cout << "MARCO 3." << std::endl;

  
    //marco p.PoolsReset(); //marco: was it removed in this new version? I think so
    //p.Parse("http://atlas-live.cern.ch/lhsee/files.xml");
  
  //eCzXmlParseError err = p->Parse("http://atlas-live.cern.ch/lhsee/files.xml");
  CzFile * mytest = new CzFile("myfiles.xml"); // files.xml
  std::cout << mytest->getFileSize() << std::endl;
  std::cout << "File exists? " << CzFile::FileExists("myfiles.xml") <<std::endl;
  std::cout << "Now parsing file: " << "allfiles.xml" << std::endl;

  CzFile * theFile = new CzFile("http://atlas-live.cern.ch/lhsee/files.xml");
  eCzXmlParseError err = p->Parse("http://atlas-live.cern.ch/lhsee/files.xml");
  if ( err == XmlErrorNone) {std::cout << "xml is ok" <<std::endl;}
  else std::cout << "xml is NOT ok. err is " << err << std::endl;
  /*
  std::cout << "************TRIAL***************" << std::endl;
  
  CzFile * myfile = new CzFile("http://atlas-live.cern.ch/lhsee/files.xml");
  
  std::cout << "Before opening, the size is " << myfile->getFileSize() << std::endl;
  
  myfile->Open("http://atlas-live.cern.ch/lhsee/files.xml", "", true);
  
  std::cout << "After opening, the size is  " << myfile->getFileSize() << std::endl;


  std::cout << "********************************" << std::endl;
   */
  
  
  std::cout << "MARCO 4." << std::endl;
  
    //marco CzXmlNode* n = p.NodePool;
    CzXmlNode* n = p->getRoot();
  //std::cout << "MARCO: n.Name is " << (n->Name)  << std::endl;

  std::cout << "MARCO 5." << std::endl;
  
  
    CzXmlNodeList* nodes = p->getNamedNodes(n, "Event");
    
    std::cout << "MARCO: nodes->size() is " << nodes->size() << std::endl;
    
    for(CzXmlNodeList::iterator it = nodes->begin(); it != nodes->end(); ++it) {
        std::cout << (*it)->getValue().c_str() << std::endl;
        addEvent((*it)->getValue().c_str(), "");
    }
     std::cout << "MARCO: done this " << nodes->size() << std::endl;
    return true;
}

void EventManager::setEventURL(const std::string& url) {
    prefix = url;
}

void EventManager::addEvent(const std::string& event, const std::string& type) {
    urls.push_back(event);
    types.push_back(type);
    guesses.push_back(-1);
}

std::vector<std::string> EventManager::getTypes() {
    std::vector<std::string> res;
    
    for(std::vector<std::string>::const_iterator it = types.begin(); it != types.end(); ++it) {
        if(res.size() == 0 || std::find(res.begin(),res.end(),*it) == res.end())
            res.push_back(*it);
    }
    
    return res;
}

std::vector<int> EventManager::getTypeInts() {
    std::vector<std::string> uniqueTypes = getTypes();
    std::vector<int> res;
    for(int i=0; i<types.size(); ++i) {
        
        int answer = -1;
        for(int j=0; j<uniqueTypes.size(); ++j) {
            if(types.at(i) == uniqueTypes.at(j))
                res.push_back(j);
        }
    }
    if(res.size() != types.size())
        std::cout << "ERROR" << std::endl;
    
    return res;
}

const std::string& EventManager::getResult(int event, bool correct) {
    if (correct)
        return correct_results.at(event);
    else
        return incorrect_results.at(event);
}

void EventManager::setTutorial(bool b) {
    tut = b;
}

bool EventManager::isTutorial() {
    return tut;
}

void EventManager::setLevel(int l) {
    level = l;
}

int EventManager::getLevel() {
    return level;
}

int EventManager::getNEvents() {
    return urls.size();
}

std::string EventManager::getCurrentEvent() {
    if(urls.size() == 0)
        return "empty.xml";
    
    std::cout << "url size = " << urls.size() << std::endl;
    std::cout << "current event = " << currentEvent << std::endl;
    
    std::cout << (prefix + urls.at(currentEvent)) << std::endl;
    
    return  prefix + urls.at(currentEvent);
}

int EventManager::getCurrentEventIndex() {
    return currentEvent;
}

int EventManager::getEventTypeHash() {
    if(urls.size() == 0)
        return 0;
    
    return CzString::CalculateHash(types.at(currentEvent).c_str());
}

void EventManager::nextEvent() {
    if(currentEvent == urls.size() - 1)
        currentEvent = 0;
    else
        ++currentEvent;
}

void EventManager::prevEvent() {
    if(currentEvent == 0)
        currentEvent = urls.size() - 1;
    else
        --currentEvent;
}

void EventManager::setEvent(int i) {
    if(i > 0 && i < urls.size())
        currentEvent = i;
    else
        currentEvent = 0;
}