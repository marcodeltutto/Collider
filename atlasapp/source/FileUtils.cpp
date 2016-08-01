//
//  FileUtils.cpp
//  GameSceneGL
//
//  Created by Tom McLaughlan on 29/05/2013.
//
//

#include "FileUtils.h"
//#include "IwGameFile.h"
#include "CzFile.h"
#include "Game.h"
#include <sstream>
#include <iostream>

void FileUtils::writeSaveFile() {
    
    std::string dataToWrite = generateSaveString(GAME->getSaveFile());
    
    CzFile file;
    CzString data(dataToWrite.c_str());
    if(file.Open("savedata.dat","wb")) {
        file.Write((void*)data.c_str(), data.getLength());
        file.Close();
    }
}

std::string FileUtils::generateSaveString(SaveFile* saveFile) {

    std::stringstream ss;
    for(int i=0; i<saveFile->getSavedGames().size(); ++i) {
        ss << generateGameStateString(saveFile->getSavedGames().at(i)) << std::endl;
    }
    
    return ss.str();
}

std::string FileUtils::generateGameStateString(GameState gameState) {
    
    std::stringstream ss;
    
    ss << gameState.gameId << " ";
    ss << gameState.nEvents;
    for(int i = 0; i < gameState.nEvents; ++i) {
        //std::cout << i << " " << gameState.results.at(i) << std::endl;
        ss << " " << gameState.results.at(i);
    }
    ss << " " << gameState.toCheck << " " << gameState.tries << std::endl;
    
    return ss.str();
    
}

bool FileUtils::gameSaveExists(int gameId) {
    
    SaveFile* savedGames = GAME->getSaveFile();
    if(savedGames->getGameState(gameId).nEvents > 0)
        return true;
    
    return false;
}

SaveFile * FileUtils::loadSaveFile() {
    
    CzFile file;
    if(file.FileExists("savedata.dat")) {
        std::cout << "file exists!" << std::endl;
        CzString data;
        if(file.Open("savedata.dat","rb")) {
            int len = file.getFileSize();
            data.allocString(len);
            data.setLength(len);
            file.Read((void*)data.c_str(), len);
            file.Close();
        }
        SaveFile * readFromFile = new SaveFile();
        //now we have to add the games to it...
        //readFromFile->insertGameState(gameState);
        
        std::istringstream stream(data.c_str());
        std::string line;
        while(std::getline(stream, line)) {
            if(line.empty() || line.size() < 8)
                continue;
            
            std::cout << line << std::endl;
            std::cout << line.size() << std::endl;
            
            GameState gs;
            
            std::istringstream ssline(line);
            ssline >> gs.gameId;
            ssline >> gs.nEvents;
            std::vector<int> res;
            for(int i=0; i < gs.nEvents; ++i) {
                int tmp;
                ssline >> tmp;
                res.push_back(tmp);
            }
            gs.results = res;
            ssline >> gs.toCheck;
            ssline >> gs.tries;
            
            readFromFile->insertGameState(gs);
        }
        
        return readFromFile;
    }
    else {
        SaveFile * empty = new SaveFile();
        return empty;
    }
    
}

//std::string FileUtils::generateHistoSaveString(Histogram *histo) {
//    
//}
//
//void HistoFile::insertHisto(Histogram *histo) {
//    for(int i=0; i<savedHistos.size(); ++i) {
//        if(savedHistos.at(i)->getHistID() == histo->getHistID()) {
//            savedHistos.at(i) = new Histogram(histo);
//            return;
//        }
//    }
//    
//    savedHistos.push_back(new Histogram(histo));
//}
//
//Histogram* HistoFile::getHisto(int histoId) {
//    for(std::vector<Histogram*>::const_iterator it = savedHistos.begin(); it != savedHistos.end(); ++it) {
//        if( (*it)->getHistID() == histoId)
//            return (*it);
//    }
//    std::cout << "Invalid HistoID" << std::endl;
//    
//    return NULL;
//}

GameState SaveFile::getGameState(int gameId) {
    
    for(std::vector<GameState>::const_iterator it = savedGames.begin(); it != savedGames.end(); ++it) {
        if( (*it).gameId == gameId)
            return (*it);
    }
    
    GameState unsaved;
    unsaved.gameId = 0;
    std::vector<int> res;
    unsaved.nEvents = 0;
    unsaved.results = res;
    unsaved.tries = 0;
    unsaved.toCheck = false;
    return unsaved;
}

void SaveFile::insertGameState(GameState gameState) {
    
    for(int i=0; i<savedGames.size(); ++i) {
        if(savedGames.at(i).gameId == gameState.gameId) {
            savedGames.at(i) = gameState;
            return;
        }
    }
    
    savedGames.push_back(gameState);
}