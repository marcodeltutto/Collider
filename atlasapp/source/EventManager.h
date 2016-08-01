//
//  EventManager.h
//  GameSceneGL
//
//  Created by Tom McLaughlan on 08/05/2013.
//
//

#ifndef _EventManager_
#define _EventManager_

#include <iostream>
#include <vector>

//#include "IwGame.h"
#include "CzApp.h"

class EventManager {

public:
    EventManager();
    void setEventURL(const std::string& url);
    void addEvent(const std::string& event, const std::string& type);
    
    void nextEvent();
    void prevEvent();
    void setEvent(int i);
    
    void setTutorial(bool b);
    bool isTutorial();
    void setLevel(int l);
    int getLevel();
    
    const std::string& getResult(int event, bool correct);
    
    std::vector<std::string> getTypes();
    std::vector<int> getTypeInts();
    
    void Reset();
    
    bool DownloadLiveEvents();
    
    int getNEvents();
    std::string getCurrentEvent();
    int getCurrentEventIndex();
    int getEventTypeHash();
    
private:
    std::vector <std::string> urls;
    std::vector <int> guesses;
    std::vector <std::string> types;
    std::vector <std::string> descriptions;
    std::vector <std::string> correct_results;
    std::vector <std::string> incorrect_results;
    int currentEvent;
    bool tut;
    int level;
    std::string prefix;
    int32 GetCompletdCallback(void* caller, void *data);
    
};


#endif /* defined(__GameSceneGL__EventManager__) */
