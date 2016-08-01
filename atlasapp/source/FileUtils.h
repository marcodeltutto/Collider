//
//  FileUtils.h
//  GameSceneGL
//
//  Created by Tom McLaughlan on 29/05/2013.
//
//

#ifndef _FILEUTILS_H_
#define _FILEUTILS_H_

#include <iostream>
#include <vector>

struct GameState {
    int gameId;
    int nEvents;
    int tries;
    bool toCheck;
    std::vector<int> results;
};

class SaveFile {
public:
    void insertGameState(GameState gameState);
    GameState getGameState(int gameId);
    std::vector<GameState> getSavedGames() {return savedGames;}
private:
    std::vector<GameState> savedGames;
};

//class HistoFile {
//public:
//    void insertHisto(Histogram* histo);
//    Histogram* getHisto(int histoId);
//private:
//    std::vector<Histogram*> savedHistos;
//};

namespace FileUtils {
    std::string generateSaveString(SaveFile* saveFile);
    std::string generateGameStateString(GameState gameState);
    SaveFile * loadSaveFile();
    void writeSaveFile();
    bool gameSaveExists(int gameId);
    
//    std::string generateHistoSaveString(Histogram* histo);
//    void writeHistoFile();
//    HistoFile * loadHistoFile();
//    void cleanupHistos();
};

#endif /* defined(_FILEUTILS_H_) */
