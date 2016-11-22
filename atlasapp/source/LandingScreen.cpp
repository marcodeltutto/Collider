//
//  LandingScreen.cpp
//  GameSceneGL
//
//  Created by Tom McLaughlan on 30/05/2013.
//
//

#include "LandingScreen.h"
//#include "IwGameString.h"
#include "CzString.h"
#include "CzUILabel.h"
#include "s3eFacebook.h"
#include <sstream>


int LandingScreen::Init(int max_collidables, int max_layers, bool doSleep) {
    CzScene::Init(max_collidables, max_layers, doSleep);
    downloaded = false;
    toupdate = true;
    currentGameId = 0;
    eventManager = GAME->getEventManager();
    fbSession = 0;
    gameId_var = 0;
    InitLinks();
}

void LandingScreen::Update(float dt) {
    CzScene::Update(dt);
    //CheckForDownloads();
    UpdateSelectionScreen();
}

void LandingScreen::InitLinks() {
    
    
    congratsScene = GAME->findScene("CongratulationScene");
    
    if(!congratsScene) {
        //marco CIwGameError::LogError("LandingScreen:: Could not find CongratulationScene");
        return;
    }
    
    selectionScene = GAME->findScene("SelectionScene");
    
    if(!selectionScene) {
        //marco CIwGameError::LogError("LandingScreen:: Could not find SelectionScene");
        return;
    }
    
    menuScene = GAME->findScene("MenuScene");
    
    if(!menuScene) {
        //marco CIwGameError::LogError("LandingScreen:: Could not find MenuScene");
        return;
    }
    
    varMan = menuScene->getVariableManager();
    gameId_var = varMan->findVariable("GameID");
    
    resetGame = (CzXomlVariableBool*) varMan->findVariable("ResetGame");

}

void LandingScreen::UpdateSelectionScreen() {
  
  //std::cout << "%%%% Calling the function UpdateSelectionScreen()" << std::endl;
  
    // 5381 is "" ? hmmm
    if(CzString::CalculateHash(gameId_var->getValue().c_str()) == 5381)
        return;

    if(currentGameId != CzString::CalculateHash(gameId_var->getValue().c_str())) {
        currentGameId = CzString::CalculateHash(gameId_var->getValue().c_str());
        std::cout << gameId_var->getValue().c_str() << std::endl;
    }
  
    //std::cout << "Got to here at list 1" << std::endl;
    GameState gameState = GAME->getSaveFile()->getGameState(currentGameId);
    //std::cout << "Got to here at list 2" << std::endl;

    CzXomlVariableInt* triesCounter = (CzXomlVariableInt*) selectionScene->getVariableManager()->findVariable("Attempts");
    std::stringstream ssatt;
    ssatt << gameState.tries;
    triesCounter->setValue(ssatt.str().c_str());
    
    bool checkTut = false;
    //std::cout << "Got to here at list 3" << std::endl;


    if(eventManager->isTutorial()) {
        checkTut = true;
        for(int i=0; i<gameState.nEvents;++i) {
            if(gameState.results.at(i) == -1)
                checkTut = false;
        }
    }
    
//    if(eventManager->isTutorial()) {
//        CIwGameUILabel* tries = (CIwGameUILabel*) selectionScene->findActor("attempts");
//        CIwGameUILabel* triesCount = (CIwGameUILabel*) selectionScene->findActor("attemptsNumber");
//        
//        tries->setColour(0,0,0,0);
//        triesCount->setColour(0,0,0,0);
//    }
//    else {
//        CIwGameUILabel* tries = (CIwGameUILabel*) selectionScene->findActor("attempts");
//        CIwGameUILabel* triesCount = (CIwGameUILabel*) selectionScene->findActor("attemptsNumber");
//        
//        tries->setColour(255,255,255,255);
//        triesCount->setColour(255,255,255,255);
//    }
    
    
    

  //std::cout << "Got to here at list 4" << std::endl;

    CzUILabel* facebookButton = (CzUILabel*) selectionScene->findActor("facebookButton");
    CzUILabel* facebookShareButton = (CzUILabel*) congratsScene->findActor("facebookShareButton");
    CzUILabel* resetButton = (CzUILabel*) selectionScene->findActor("resetButton");
    
    facebookShareButton->setTappable(true);
    facebookShareButton->setColour(255, 255, 255, 255);
    
    if(!gameState.toCheck && !checkTut) {
        resetButton->setTappable(false);
        resetButton->setColour(50, 50, 50, 50);
        facebookButton->setTappable(false);
        facebookButton->setColour(50,50,50,50);
    }
    else if(gameState.toCheck || checkTut) {
        resetButton->setTappable(true);
        resetButton->setColour(255, 255, 255, 255);
        facebookButton->setTappable(true);
        facebookButton->setColour(255, 255, 255, 255);
    }

  //std::cout << "Got to here at list 5" << std::endl;

    int correct = 0;
    bool higgs = false;
    for(int i=0; i<gameState.nEvents; ++i) {
        if(gameState.toCheck || checkTut) {
            if(gameState.results.at(i) == eventManager->getTypeInts().at(i)) {
                correct++;
                if(eventManager->getTypes().at(eventManager->getTypeInts().at(i)) == "Higgs to 4 leptons") {
                    higgs=true;
                    //std::cout << "HIGGS" << std::endl;
                }
            }
        }
    }

    if(gameState.toCheck || checkTut) {
        CzUILabel* scoreLabel = (CzUILabel*) congratsScene->findActor("score");
        std::stringstream score;
        score << "You scored " << correct << "/" << gameState.nEvents << ". ";

        if (gameState.nEvents != 0) {
          if(correct/gameState.nEvents > 0.5)
            score << "Congratulations!";
          else
            score << "Well done!";
        } else
          score << "Error: gameState.nEvents is zero";

        if(higgs) {
            score.str("");
            score << "You scored " << correct << "/" << gameState.nEvents << " and found the Higgs! Congratulations!";
        }
        
        scoreLabel->setText(score.str().c_str());
    }

  //std::cout << "Just before Facebook session" << std::endl;

    if(facebookButton->isSelected() || facebookShareButton->isSelected()) {
        facebookButton->setSelected(false);
        facebookShareButton->setSelected(false);
      
      //std::cout << "Facebook session 1" << std::endl;
      
        if(!fbSession) fbSession = s3eFBInit("141331202732781");
      //std::cout << "Facebook session 2" << std::endl;

      
        if(!s3eFBSession_LoggedIn(fbSession))
            s3eFBSession_Login(fbSession, NULL);
        
//        if(!IW_GAME_FACEBOOK->isLoggedIn())
//            IW_GAME_FACEBOOK->Login();
//        
//        if(IW_GAME_FACEBOOK->isLoggedIn()) {
//            IW_GAME_FACEBOOK->PostWall("Something Something Physics", "http://epweb2.ph.bham.ac.uk/user/mclaughlan/lhsee/web/lhsee/", "", "LHSee", "If you are reading this, then my test has worked!");
//        }
        

        
        //CIwGameScene* selScene = GAME->findScene("SelectionScene");
        std::string gameName = selectionScene->getVariableManager()->findVariable("Title")->getValue().str();
        
        std::stringstream caption;
        caption << "I just completed \"" << gameName << "\" in Collider and got " << correct << "/" << gameState.nEvents << "!";
        
        if(higgs) {
            caption.str("");
            caption << "I just completed \"" << gameName << "\" in Collider and found the Higgs boson! [score " << correct << "/" << gameState.nEvents << "]";
        }
        

        std::cout << caption.str() << std::endl;
        
        if(s3eFBSession_LoggedIn(fbSession)) {
//
            s3eFBDialog* dialog = s3eFBDialog_WithAction(fbSession, "feed");
//            s3eFBDialog_AddParamString(dialog,"attachment","{\"name\":\"LHsee\",\"caption\":\"Something Something Physics\",\"description\":\"If you are reading this, that means my test was successful. World domination will follow shortly.\"}");
            s3eFBDialog_AddParamString(dialog,"name","Collider");
            s3eFBDialog_AddParamString(dialog,"caption",caption.str().c_str());
            s3eFBDialog_AddParamString(dialog,"description","Download Collider now on iOS and Android and see if you can find the Higgs boson!");
            s3eFBDialog_AddParamString(dialog,"link","http://collider.physics.ox.ac.uk");
            s3eFBDialog_AddParamString(dialog,"picture","http://collider.physics.ox.ac.uk/FBBadge.png");
            s3eFBDialog_Show(dialog,&facebookCallback);
        }
     
    }

    if((gameState.toCheck || checkTut) && resetGame->NativeValue) {
        resetGame->NativeValue = false;
        std::cout << "gameid = " << gameState.gameId << std::endl;
        for(int i=0; i<gameState.nEvents; ++i) {
            gameState.results.at(i) = -1;
        }
        if(!eventManager->isTutorial())
            gameState.tries = gameState.tries+1;
        std::cout << "tries = " << gameState.tries << std::endl;
        gameState.toCheck = false;
        GAME->getSaveFile()->insertGameState(gameState);
        FileUtils::writeSaveFile();
        return;
    }
    
    CzUIListBox* eventSelect = (CzUIListBox*) selectionScene->findActor("EventsListBox");
    for(int i=0; i<eventSelect->getItemCount(); ++i) {
        std::stringstream ss;
        ss << "event" << i;
        CzUILabel* label = (CzUILabel*) selectionScene->findActor(ss.str().c_str());
        
        if(gameState.nEvents==0 || gameState.results.at(i) == -1) {
            label->setColour(255, 255, 255, 255);
            continue;
        }

        if(gameState.toCheck || checkTut) {
            if(gameState.results.at(i) == eventManager->getTypeInts().at(i)) {
                label->setColour(80, 255, 80, 255);
                if(eventManager->getTypes().at(eventManager->getTypeInts().at(i)) == "Higgs to 4 leptons")
                    label->setText("H");
            }
            else {
                 label->setColour(255, 80, 80, 255);
            }
        }
        else {
            label->setColour(80, 80, 255, 255);
        }
    }
    
}

static void facebookCallback(struct s3eFBDialog* dialog, s3eResult* streamResult, void* userData)
{

    if (*streamResult == S3E_RESULT_SUCCESS)
    {
        std::cout << "`x66ee66Stream dialog succeeded" << std::endl;
    }
    else if (s3eFBDialog_Error(dialog))
    {
        std::cout << "`xee6666Stream dialog error %s" << std::endl;
    }
    else
    {
        const char* url = s3eFBDialog_DidNotCompleteWithUrl(dialog);
        
        if (url)
        {
            std::cout << "`xee6666Stream dialog did not complete with url %s" << std::endl;
        }
        else
        {
            std::cout << "`xee6666Stream dialog did not complete" << std::endl;
        }
    }
}

//void LandingScreen::CheckForDownloads() {
//    if(!downloaded) {
//        CIwGameFile file;
//        CIwGameString data;
//        
//        // download a list of available games from the web.
//        
//        if(file.Open("http://epweb2.ph.bham.ac.uk/user/mclaughlan/lhsee/gamelist.txt",NULL,true)) {
//            int len = file.getFileSize();
//            data.allocString(len);
//            data.setLength(len);
//            file.Read((void*)data.c_str(), len);
//            file.Close();
//        }
//        
//        std::vector<std::string> tmp;
//        std::istringstream stream(data.c_str());
//        std::string line;
//        while(stream >> line) {
//            if(line.empty())
//                continue;
//            
//            tmp.push_back(line);
//        }
//        
//        av = tmp;
//        
//        downloaded = true;
//        toupdate = true;
//    }
//}

void LandingScreen::UpdateDownloadButtons() {
    if (toupdate) {
        toupdate = false;
        
        // now we should update the two different menus. one with downloaded games, one with undownloaded games
        // plan:
        // - check against a list of 'downloaded' games - this needs to be created and updated whenever a new game is downloaded/deleted
        // - create an xml list of downloaded games, called 'LocalList.xml' with the format to put buttons on the screen
        // - same for available for download games, called 'DownloadList.xml'
        // - have all the games in a list, with one more button for 'download more...' which loads the DownloadList.xml
        //
        // this method should create the xml files...
        //
        
        
    }
}
