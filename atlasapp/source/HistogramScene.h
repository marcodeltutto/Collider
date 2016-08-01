//
//  HistogramScene.h
//  GameSceneGL
//
//  Created by Tom McLaughlan on 28/06/2013.
//
//

#ifndef __HistogramScene__
#define __HistogramScene__

#include <iostream>
#include <vector>
//#include "IwGameScene.h"
#include "CzScene.h"

class Histogram {
   
private:
    int nbins;
    float startbin;
    float endbin;
    std::vector<float> data;
    int histId;
    
public:
    Histogram();
    Histogram(Histogram* hist_);
    Histogram(int histId_, int nbins_, float startbin_, float endbin_);
    virtual ~Histogram() {};
    
    int getNBins() { return nbins; }
    float getStartBin() { return startbin; }
    float getEndBin() { return endbin; }
    int getHistID() { return histId; }
    void setBinContent(int bin, float content);
    float getBinContent(int bin);
    void Fill(float d);
};

class HistogramScene : public CzScene {
    
private:
    Histogram* histo;
    
public:
    virtual ~HistogramScene() {}
    int		Init(int max_collidables = 128, int max_layers = 10, bool doSleep = true);
    void	Update(float dt);
    void	Draw();

    Histogram* getHisto() { return histo; }
    
    // Implementation of IIwGameXomlResource interface
	int	LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node)
	{
		if (!CzScene::LoadFromXoml(parent, load_children, node))
			return false;
        
		// Add our own custom attribute parsing
		for (CzXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
		{
			unsigned int name_hash = (*it)->getName().getHash();
		}
        
		return true;
	}

};

//
//  HistogramSceneCreator - Creates an instance the 'LandingScreen'
//
class HistogramSceneCreator : public IzXomlClassCreator
{
public:
	HistogramSceneCreator()
	{
		setClassName("HistogramScene");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new HistogramScene(); }
};


#endif /* defined(__HistogramScene__) */
