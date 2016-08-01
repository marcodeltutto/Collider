//
//  HistogramScene.cpp
//  GameSceneGL
//
//  Created by Tom McLaughlan on 28/06/2013.
//
//

#include "HistogramScene.h"




int HistogramScene::Init(int max_collidables, int max_layers, bool doSleep) {
    CIwGameScene::Init(max_collidables, max_layers, doSleep);
}

void HistogramScene::Update(float dt) {
    CIwGameScene::Update(dt);
}

void HistogramScene::Draw() {
    CIwGameScene::Draw();
}

Histogram::Histogram() {
    nbins = 0;
    startbin = 0;
    endbin = 0;
    histId = 0;
}

Histogram::Histogram(Histogram* hist) {
    nbins = hist->getNBins();
    startbin = hist->getStartBin();
    endbin = hist->getEndBin();
    histId = hist->getHistID();
    
    for(int i=0; i<hist->getNBins(); ++i) {
        data.push_back(hist->getBinContent(i));
    }
}

Histogram::Histogram(int histId_, int nbins_, float startbin_, float endbin_) {
    nbins = nbins_;
    startbin = startbin_;
    endbin = endbin_;
    histId = histId_;
    
    data.reserve(nbins);
}

void Histogram::Fill(float d) {
    float inc = (endbin - startbin) / nbins;
    int binIndex = (d - startbin) / (inc);
    
    data[binIndex] += 1;
}

void Histogram::setBinContent(int bin, float content) {
    data[bin] = content;
}

float Histogram::getBinContent(int bin) {
    return data[bin];
}