//
//  LiveScene.cpp
//  GameSceneGL
//
//  Created by Tom McLaughlan on 10/07/2013.
//
//

#include "LiveScene.h"
#include "IwGxScene.h"

// We initialise the scene and reset the event manager. this will display an empty event.
// The scene starts with the menu open, instructing the user to refresh in order to load live events.
int LiveScene::Init(int max_collidables, int max_layers, bool doSleep) {
    CzScene::Init(max_collidables, max_layers, doSleep);
    eventManager = GAME->getEventManager();
    eventManager->Reset();
    refreshButton = 0;
    nextButton = 0;
    prevButton = 0;
    InitLinks();
  
  return 0;
}

void LiveScene::InitLinks() {
    refreshButton = (CzUILabel*) this->findActor("refreshButton");
    nextButton = (CzUILabel*) this->findActor("nextEventButton");
    prevButton = (CzUILabel*) this->findActor("prevEventButton");
}

void LiveScene::Update(float dt) {
    CzScene::Update(dt);
    if(!refreshButton) {
        std::cout << "no refreshButton found" << std::endl;
        InitLinks();
        
        // pre-load some new events on the first Update call.
        eventManager->DownloadLiveEvents();
        
        return;
    }
    
    
    // download new events and set the refresh button to the 'off' state
    if(refreshButton->isSelected()) {
        refreshButton->setSelected(false);
        eventManager->DownloadLiveEvents();
        eventManager->setEvent(0);
    }
    if(nextButton->isSelected()) {
        nextButton->setSelected(false);
        int curEv = eventManager->getCurrentEventIndex();
        eventManager->DownloadLiveEvents();
        eventManager->setEvent(curEv);
    }
    if(prevButton->isSelected()) {
        prevButton->setSelected(false);
        int curEv = eventManager->getCurrentEventIndex();
        eventManager->DownloadLiveEvents();
        eventManager->setEvent(curEv);
    }
    
    CzXomlVariable* dateVar = (CzXomlVariable*) getVariableManager()->findVariable("EventDescription");
    
    // if the date string is out of date, update it.
    if(GAME->findScene("IwGxScene")) {
        if(dateVar->getValue().getHash() != CzString::CalculateHash(((IwGxScene*) GAME->findScene("IwGxScene"))->currentEvent->dataTime.c_str()))
            dateVar->setValue(((IwGxScene*) GAME->findScene("IwGxScene"))->currentEvent->dataTime.c_str());
    }
}