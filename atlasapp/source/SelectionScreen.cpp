//
//  LandingScreen.cpp
//  GameSceneGL
//
//  Created by Tom McLaughlan on 30/05/2013.
//
//

#include "SelectionScreen.h"
#include <sstream>


int LandingScreen::Init(int max_collidables, int max_layers, bool doSleep) {
    CIwGameScene::Init(max_collidables, max_layers, doSleep);
    InitLinks();
    downloaded = false;
    toupdate = true;
}

void LandingScreen::Update(float dt) {
    CIwGameScene::Update(dt);
    CheckForDownloads();
}

void LandingScreen::InitLinks() {
    
    selectionScene = GAME->findScene("SelectionScene");
    
    if(!selectionScene) {
        CIwGameError::LogError("LandingScreen:: Could not find SelectionScene");
        return;
    }
    
}

void LandingScreen::CheckForDownloads() {
    if(!downloaded) {
        CIwGameFile file;
        CIwGameString data;
        
        // download a list of available games from the web.
        
        if(file.Open("http://epweb2.ph.bham.ac.uk/user/mclaughlan/lhsee/gamelist.txt",NULL,true)) {
            int len = file.getFileSize();
            data.allocString(len);
            data.setLength(len);
            file.Read((void*)data.c_str(), len);
            file.Close();
        }
        
        std::vector<std::string> tmp;
        std::istringstream stream(data.c_str());
        std::string line;
        while(stream >> line) {
            if(line.empty())
                continue;
            
            tmp.push_back(line);
        }
        
        av = tmp;
        
        downloaded = true;
        toupdate = true;
    }
}

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