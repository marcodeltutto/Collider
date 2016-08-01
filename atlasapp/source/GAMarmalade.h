/*
 Google analytics for Marmalade API.
 Programmer: Danilo Ganzella
 Marmalade Forum: mirrorlink
 */

#ifndef GAMARMALADE_H
#define GAMARMALADE_H

//marmalade
#include "IwHTTP.h"
#include "s3eFile.h"

//std
#include <string>
#include <map>

class GAMarmalade
{
	friend int32 GotHeaders(void* sys, void* user);
    
public:
    
    static void init(std::string GA_ID, std::string DomainHash, std::string Site);
    static GAMarmalade* getobj();
    
    void pageview(std::string page);
    void update();
    static void end();
    
private:
    
    time_t timevisit, tryagaininit;
    std::string initial_visit, visitor_id, currpage;
    
    unsigned int postcount;
    bool posting, offline;
    CIwHTTP http;
    
    std::string mGA_ID, mDomainHash, mSite;
    std::map< std::string, unsigned int > pendingRequests;
    
    GAMarmalade();
    
    static GAMarmalade* instance;
    
    void poppulateMap();
    
    s3eFile * fhandle;
    
    void post(std::string page);
    void save();
};

#endif