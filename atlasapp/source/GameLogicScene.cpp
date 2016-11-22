//
//  GameLogicScene.cpp
//  GameSceneGL
//
//  Created by Tom McLaughlan on 24/05/2013.
//
//

#include "GameLogicScene.h"
//#include "IwGame.h"
#include "CzApp.h"
#include "Game.h"
#include "IwGxScene.h"
//#include "IwGameUIListBox.h"
#include "CzUIListBox.h"
#include "EventManager.h"
#include "TMath.h"
#include "s3eWebView.h"
#include "WebBrowser.h"
#include <sstream>

int GameLogicScene::Init(int max_collidables, int max_layers, bool doSleep) {
  CzScene::Init(max_collidables, max_layers, doSleep);
  gameScore = 0;
  choiceListBox = 0;
  resultLabel = 0;
  currentEvent = -1;
  firstUpdate = false;
  checkResults = false;
  postResult = true;
  eventManager = GAME->getEventManager();
  
  CzScene* menuScene = (CzScene*) GAME->findScene("MenuScene");
  gameId = menuScene->getVariableManager()->findVariable("GameID")->getValue().getHash();
  
  std::cout << menuScene->getVariableManager()->findVariable("GameID")->getValue().c_str() << " " << gameId << std::endl;
  
  // this bit should run on the first time running the game
  // if played before it should load the saved info
  if(!FileUtils::gameSaveExists(gameId))
    newGame();
  else
    loadGame();
}

void GameLogicScene::newGame() {
  gameState.nEvents = eventManager->getNEvents();
  for(int i=0; i < gameState.nEvents; ++i)
    gameState.results.push_back(-1);
  gameState.gameId = gameId;
  gameState.tries = 0;
  gameState.toCheck = checkResults;
  
  GAME->getSaveFile()->insertGameState(gameState);
  FileUtils::writeSaveFile();
}

void GameLogicScene::loadGame() {
  gameState = FileUtils::loadSaveFile()->getGameState(gameId);
  currentEvent=0;
  for(int i=0; i<gameState.nEvents; ++i) {
    if(gameState.results.at(i) == -1) {
      currentEvent = i;
      break;
    }
  }
  checkResults = gameState.toCheck;
}

void GameLogicScene::Update(float dt) {
  CzScene::Update(dt);
  
  // first update the choices in the list
  if(!eventManager->isTutorial()) {
    UpdateChoices();
    if(checkResults)
      CheckSelection();
  }
  else {
    UpdateTutorialChoices();
    
    // for the tutorial, give instant feedback
    CheckTutorialSelection();
    
  }
}

void GameLogicScene::CheckSelection() {
  if (!choiceListBox){
    //marco CIwGameError::LogError("Warning GameLogicScene::CheckSelection() cannot find GameLogicScene ListBox");
    std::cout << "Warning GameLogicScene::CheckSelection() cannot find GameLogicScene ListBox" << std::endl;
    return;
  }
  
  if (choiceListBox->getSelectionIndex() != -1) {
    CzUILabel* selected = (CzUILabel*) choiceListBox->getSelection();
    //if(CIwGameString::CalculateHash(selected->getText().str()) == eventManager->getEventTypeHash()) {
    if(choiceListBox->getSelectionIndex() == eventManager->getTypeInts().at(currentEvent)){
      selected->setColour(80, 255, 80, 255);
    }
    else {
      selected->setColour(255, 80, 80, 255);
    }
  }
}
void GameLogicScene::CheckTutorialSelection() {
  if (!choiceListBox){
    //marco CIwGameError::LogError("Warning GameLogicScene::CheckSelection() cannot find GameLogicScene ListBox");
    std::cout << "Warning GameLogicScene::CheckSelection() cannot find GameLogicScene ListBox" << std::endl;
    return;
  }
  CzUIBase* base = choiceListBox->getSelection();
  if (base == NULL) std::cout << "Warning in GameLogicScene::CheckSelection(): choiceListBox->getSelection() is NULL" << std::endl;
  if (base != NULL) {
    // the list box is unselected at the start of each event. if we select something, check the result
    if (choiceListBox->getSelectionIndex() != -1 && choiceListBox->getSelection()->isSelected()) {
      CzUILabel* selected = (CzUILabel*) choiceListBox->getSelection();
      if(CzString::CalculateHash(selected->getText().str()) == eventManager->getEventTypeHash()) {
        selected->setColour(80, 255, 80, 255);
      }
      else {
        selected->setColour(255, 80, 80, 255);
      }
    }
  }
  //std::cout << "in check not if" << std::endl;
}

void GameLogicScene::CheckMassSelection() {
  if (!choiceListBox){
    //marco CIwGameError::LogError("Warning GameLogicScene::CheckSelection() cannot find GameLogicScene ListBox");
    return;
  }
  
  // the list box is unselected at the start of each event. if we select something, check the result
  if (choiceListBox->getSelectionIndex() != -1 && choiceListBox->getSelection()->isSelected()) {
    CzUILabel* selected = (CzUILabel*) choiceListBox->getSelection();
    if(CzString::CalculateHash(selected->getText().str()) == eventManager->getEventTypeHash()) {
      selected->setColour(80, 255, 80, 255);
      
      IwGxScene* evDisplay = (IwGxScene*) GAME->findScene("IwGxScene");
      if(!evDisplay)
        return;
      EventFromXML* curEv = evDisplay->currentEvent;
      
      std::string boson;
      int nMu=0;
      int nEl=0;
      float mass = 0.0;
      
      for(int i=0; i<curEv->muonTracks->pt.size(); ++i) {
        if(fabsf(curEv->muonTracks->pt.at(i)) > curEv->muonTracks->pt_cut)
          nMu++;
      }
      for(int i=0; i<curEv->el_pt.size(); ++i) {
        if(fabsf(curEv->el_pt.at(i) > 15.0))
          nEl++;
      }
      
      if(nMu == 1) {
        float mu_pt = fabsf(curEv->muonTracks->pt.at(0));
        float mu_phi = curEv->muonTracks->phi0.at(0);
        float mu_m = 0.10566;
        
        mass = transverseMass(mu_m, mu_pt, mu_phi, curEv->MET_etx, curEv->MET_ety);
        boson = "W";
        
      }
      
      if(nMu == 2) {
        
        
        
        float mu_m = 0.10566;
        
        float mu_pt0 = fabsf(curEv->muonTracks->pt.at(0));
        //                float mu_theta0 = atan(1/curEv->muonTracks->cotTheta.at(0));
        //                if(mu_theta0 < 0)
        //                     mu_theta0+=PI;
        float mu_eta0 = -log(sqrtf(curEv->muonTracks->cotTheta.at(0) * curEv->muonTracks->cotTheta.at(0) + 1.0) - curEv->muonTracks->cotTheta.at(0));//-log(tan(mu_theta0/2.0));
        float mu_phi0 = curEv->muonTracks->phi0.at(0);
        
        float mu_pt1 = fabsf(curEv->muonTracks->pt.at(1));
        //                float mu_theta1 = atan(1/curEv->muonTracks->cotTheta.at(1));
        //                if(mu_theta1 < 0)
        //                    mu_theta1+=PI;
        float mu_eta1 = -log(sqrtf(curEv->muonTracks->cotTheta.at(1) * curEv->muonTracks->cotTheta.at(1) + 1.0) - curEv->muonTracks->cotTheta.at(1));//-log(tan(mu_theta1/2.0));
        float mu_phi1 = curEv->muonTracks->phi0.at(1);
        
        mass = invMass(mu_m, mu_pt0, mu_eta0, mu_phi0, mu_m, mu_pt1, mu_eta1, mu_phi1);
        
        boson = "Z";
        
        
      }
      
      if(nEl == 1) {
        float el_pt = fabsf(curEv->el_pt.at(0));
        float el_phi = curEv->el_phi.at(0);
        float el_m = 0.000511;
        
        mass = transverseMass(el_m, el_pt, el_phi, curEv->MET_etx, curEv->MET_ety);
        
        boson = "W";
        
      }
      
      if(nEl == 2) {
        
        float el_m = 0.000511;
        
        float el_pt0 = fabsf(curEv->el_pt.at(0));
        float el_eta0 = curEv->el_eta.at(0);
        float el_phi0 = curEv->el_phi.at(0);
        
        float el_pt1 = fabsf(curEv->el_pt.at(1));
        float el_eta1 = curEv->el_eta.at(1);
        float el_phi1 = curEv->el_phi.at(1);
        
        mass = invMass(el_m, el_pt0, el_eta0, el_phi0, el_m, el_pt1, el_eta1, el_phi1);
        
        boson = "Z";
      }
      
      if(postResult) {
        postResult=false;
        s3eWebView* webView = s3eWebViewCreate();
        
        int eventNumber = currentEvent + gameId;
        
        std::stringstream ss;
        ss << "http://www.hep.ph.bham.ac.uk/DiscoveringParticles/upload_test.php?Mass=" << mass << "&Event=" << eventNumber << "&Boson=" << boson;
        
        std::cout << ss.str() << std::endl;
        
        s3eWebViewNavigate(webView, ss.str().c_str());
        
        WebBrowser* webScene = (WebBrowser*) GAME->findScene("WebScene");
        if(webScene) {
          webScene->refreshPage();
        }
        
        FileUtils::writeSaveFile();
      }
      
      std::stringstream mss;
      mss << "Calculated Mass = " << mass << " GeV";
      resultLabel->setText(mss.str().c_str());
      resultLabel->setVisible(true);
      
    }
    else {
      selected->setColour(255, 80, 80, 255);
      resultLabel->setText("");
      resultLabel->setVisible(false);
    }
  }
  else {
    resultLabel->setText("");
    resultLabel->setVisible(false);
  }
}

void GameLogicScene::UpdateChoices() {
  //std::cout << "GameLogicScene::UpdateChoices() called" << std::endl;
  // set up the choice list box if it's not already
  if (!choiceListBox){
    InitLinks();
    //marco CIwGameError::LogError("Warning GameLogicScene::UpdateChoices() cannot find GameLogicScene ListBox. Initialising.");
    std::cout << "Warning GameLogicScene::UpdateChoices() cannot find GameLogicScene ListBox. Initialising." << std::endl;
    return;
  }
  
  if (!checkButton){
    //marco CIwGameError::LogError("Warning GameLogicScene::UpdateChoices() cannot find GameLogicScene checkResultsButton. Initialising.");
    std::cout << "Warning GameLogicScene::UpdateChoices() cannot find GameLogicScene checkResultsButton. Initialising." << std::endl;
    return;
  }
  
  bool allowCheck = true;
  for(int i=0; i<gameState.nEvents; ++i) {
    if(gameState.results.at(i) == -1) {
      allowCheck = false;
    }
  }
  
  if(!allowCheck) {
    checkButton->setTappable(false);
    checkButton->setColour(50, 50, 50, 80);
  }
  else if(allowCheck) {
    checkResults = true; // marco's addition
    checkButton->setTappable(true);
    checkButton->setColour(255, 255, 255, 255);
  }
  
  
  // update the available choices based on the current event
  // for example, the first event allows one choice, the second gives another
  // this allows for a guided 'tutorial mode'
  if(currentEvent != eventManager->getCurrentEventIndex() || !firstUpdate) {
    currentEvent = eventManager->getCurrentEventIndex();
    CzXomlVariableInt* evCount = (CzXomlVariableInt*) this->getVariableManager()->findVariable("eventCounter");
    std::stringstream evc;
    evc << "Event " << (currentEvent+1) << "/" << eventManager->getNEvents();
    evCount->setValue(evc.str().c_str());
    
    firstUpdate = true;
    postResult = true;
    
    // reset the choice box, unselect everything
    // this removes the selection and stops the game logic from checking new events
    //if(choiceListBox->getSelectionIndex() != -1) {
    //    choiceListBox->getSelection()->setSelected(false);
    //this->getVariableManager()->findVariable("Result")->setValue("");
    //}
    if(gameState.results.at(currentEvent) == -1) {
      choiceListBox->setSelectionIndex(-1);
      //choiceListBox->getSelection()->setSelected(false);
    }
    else {
      choiceListBox->setSelectionIndex(gameState.results.at(currentEvent));
      choiceListBox->getSelection()->setSelected(true);
    }
  }
  
  std::cout << "  checkResults is      " << checkResults << std::endl;
  std::cout << "  gameState.toCheck is " << gameState.toCheck << std::endl;

  if(!checkResults && gameState.results.at(currentEvent) != choiceListBox->getSelectionIndex()) {
    gameState.results.at(currentEvent) = choiceListBox->getSelectionIndex();
  }
  
  if(checkResults) {
    choiceListBox->setSelectionIndex(gameState.results.at(currentEvent));
  }
  if(gameState.toCheck != checkResults) {
    gameState.toCheck = checkResults;
    FileUtils::writeSaveFile();
  }
  
}

// This updates the available choices for the tutorial.
// Each event should introduce a new idea, adding one option to the answers
// The game then starts with one option, introducing more concepts, eventually
// leading to a handful of options with a game.
void GameLogicScene::UpdateTutorialChoices() {
  if (!choiceListBox){
    InitLinks();
    //marco CIwGameError::LogError("Warning GameLogicScene::UpdateTutorialChoices() cannot find GameLogicScene ListBox. Initialising.");
    return;
  }
  
  // update the available choices based on the current event
  // for example, the first event allows one choice, the second gives another
  // this allows for a guided 'tutorial mode'
  //
  //    checkButton->setTappable(false);
  //    checkButton->setColour(50, 50, 50, 80);
  
  bool allowCheck = true;
  for(int i=0; i<gameState.nEvents; ++i) {
    if(gameState.results.at(i) == -1) {
      allowCheck = false;
    }
  }
  
  if(!allowCheck) {
    checkButton->setTappable(false);
    checkButton->setColour(50, 50, 50, 80);
  }
  else if(allowCheck) {
    checkButton->setTappable(true);
    checkButton->setColour(255, 255, 255, 255);
  }
  
  // update the available choices based on the current event
  // for example, the first event allows one choice, the second gives another
  // this allows for a guided 'tutorial mode'
  if(currentEvent != eventManager->getCurrentEventIndex() || !firstUpdate) {
    currentEvent = eventManager->getCurrentEventIndex();
    CzXomlVariableInt* evCount = (CzXomlVariableInt*) this->getVariableManager()->findVariable("eventCounter");
    std::stringstream evc;
    evc << "Event " << (currentEvent+1) << "/" << eventManager->getNEvents();
    evCount->setValue(evc.str().c_str());
    
    firstUpdate = true;
    postResult = true;
    
    CzXomlVariableArray* cs = (CzXomlVariableArray*) this->getVariableManager()->findVariable("Choices");
    
    int nchoices = (eventManager->getCurrentEventIndex()+1) > eventManager->getTypes().size() ? eventManager->getTypes().size() : (eventManager->getCurrentEventIndex()+1);
    
    
    for(int i=0; i<choiceListBox->getItemCount(); ++i){
      CzUIListBoxItem* lbi = (CzUIListBoxItem*) choiceListBox->getItem(i);
      CzUILabel* lab = (CzUILabel*) lbi->Actor;
      if(i>=nchoices) {
        lab->setTappable(false);
        CzColour col;
        col.set(50, 50, 50, 150);
        lab->setBackgroundColour(col);
      }
      else {
        lab->setTappable(true);
        CzColour col;
        col.set(255, 255, 255, 255);
        lab->setBackgroundColour(col);
      }
    }
    
    
    
    if(gameState.results.at(currentEvent) == -1) {
      choiceListBox->setSelectionIndex(-1);
    }
    else {
      choiceListBox->setSelectionIndex(gameState.results.at(currentEvent));
      choiceListBox->getSelection()->setSelected(true);
    }
    
  }
  
  if(gameState.results.at(currentEvent) != choiceListBox->getSelectionIndex()) {
    gameState.results.at(currentEvent) = choiceListBox->getSelectionIndex();
  }
  
}

// This updates the available choices for the tutorial.
// Each event should introduce a new idea, adding one option to the answers
// The game then starts with one option, introducing more concepts, eventually
// leading to a handful of options with a game.
void GameLogicScene::UpdateMassChoices() {
  // set up the choice list box if it's not already
  if (!choiceListBox){
    InitLinks();
    //marco CIwGameError::LogError("Warning GameLogicScene::UpdateChoices() cannot find GameLogicScene ListBox. Initialising.");
    return;
  }
  
  if (!checkButton){
    //marco CIwGameError::LogError("Warning GameLogicScene::UpdateChoices() cannot find GameLogicScene checkResultsButton. Initialising.");
    return;
  }
  
  checkButton->setTappable(false);
  checkButton->setColour(50, 50, 50, 80);
  
  
  // update the available choices based on the current event
  // for example, the first event allows one choice, the second gives another
  // this allows for a guided 'tutorial mode'
  if(currentEvent != eventManager->getCurrentEventIndex() || !firstUpdate) {
    currentEvent = eventManager->getCurrentEventIndex();
    CzXomlVariableInt* evCount = (CzXomlVariableInt*) this->getVariableManager()->findVariable("eventCounter");
    std::stringstream evc;
    evc << currentEvent;
    evCount->setValue(evc.str().c_str());
    
    firstUpdate = true;
    postResult = true;
    
    // reset the choice box, unselect everything
    // this removes the selection and stops the game logic from checking new events
    //if(choiceListBox->getSelectionIndex() != -1) {
    //    choiceListBox->getSelection()->setSelected(false);
    //this->getVariableManager()->findVariable("Result")->setValue("");
    //}
    
    
    if(gameState.results.at(currentEvent) == -1) {
      choiceListBox->setSelectionIndex(-1);
      //choiceListBox->getSelection()->setSelected(false);
    }
    else {
      choiceListBox->setSelectionIndex(gameState.results.at(currentEvent));
      choiceListBox->getSelection()->setSelected(true);
    }
    
  }
  
  if(gameState.results.at(currentEvent) != choiceListBox->getSelectionIndex()) {
    gameState.results.at(currentEvent) = choiceListBox->getSelectionIndex();
  }
}

void GameLogicScene::InitLinks() {
  //choices
  choiceListBox = (CzUIListBox*) this->findActor("ChoiceListBox");
  checkButton = (CzUILabel*) this->findActor("checkResultsButton");
  resultLabel = (CzUILabel*) this->findActor("resulttext");
  resultLabel->setVisible(false);
}

