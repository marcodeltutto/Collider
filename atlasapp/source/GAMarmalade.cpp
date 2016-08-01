//project
#include "GAMarmalade.h"

//marmalade
#include "IwUriEscape.h"

//std
#include <sstream>

GAMarmalade* GAMarmalade::instance = NULL;

int32 GotHeaders(void* sys, void* user)
{
	if( GAMarmalade::instance->http.GetStatus() == S3E_RESULT_SUCCESS )
	{
		GAMarmalade::instance->pendingRequests[GAMarmalade::instance->currpage]--;
        
		if( GAMarmalade::instance->pendingRequests[GAMarmalade::instance->currpage] == 0 )
		{
			GAMarmalade::instance->pendingRequests.erase(GAMarmalade::instance->currpage);
		}
	}
	else
	{
		GAMarmalade::instance->offline = true;
		GAMarmalade::instance->tryagaininit = time(0);
	}
    
	GAMarmalade::instance->save();
	GAMarmalade::instance->posting = false;
    
    return 0;
}

GAMarmalade::GAMarmalade():postcount(0),posting(false),offline(false)
{
	std::stringstream visitor;
	visitor << rand() << rand();
	visitor >> visitor_id;
}

void GAMarmalade::update()
{
	IwAssertMsg(MYAPP,GAMarmalade::instance != NULL, ("Null GAMarmalade Singleton.Please call init."));
    
	if( GAMarmalade::instance->offline )
	{
		if( (time(0) - GAMarmalade::instance->tryagaininit) >= 20 )
		{
			GAMarmalade::instance->offline = false;
		}
	}
	else
	{
		if( !GAMarmalade::instance->posting )
		{
			if( instance->pendingRequests.size() > 0 )
			{
				GAMarmalade::instance->post( (*GAMarmalade::instance->pendingRequests.begin()).first );
				GAMarmalade::instance->posting = true;
			}
		}
	}
}

void GAMarmalade::init(std::string GA_ID, std::string DomainHash, std::string Site)
{
	IwAssertMsg(MYAPP,GAMarmalade::instance == NULL, ("GAMarmalade Singleton not null.Init called twice without end called."));
    
	instance = new GAMarmalade;
    
	instance->mGA_ID = GA_ID;
	instance->mDomainHash = DomainHash;
	instance->mSite = CIwUriEscape::Escape(Site);
    
	if( s3eFileCheckExists("gooanlycsreq.txt") )
	{
		instance->poppulateMap();
	}
}

GAMarmalade* GAMarmalade::getobj()
{
	IwAssertMsg(MYAPP,GAMarmalade::instance != NULL, ("Null GAMarmalade Singleton.Please call init."));
	return GAMarmalade::instance;
}

void GAMarmalade::end()
{
	IwAssertMsg(MYAPP,GAMarmalade::instance != NULL, ("Null GAMarmalade Singleton.Please call init."));
	GAMarmalade::instance->http.Cancel();
	delete GAMarmalade::instance;
	GAMarmalade::instance = NULL;
}

void  GAMarmalade::poppulateMap()
{
	fhandle = s3eFileOpen("gooanlycsreq.txt", "r");
    
	char buff[1024];
	
	while( s3eFileReadString(buff,1022,fhandle) )
	{
		std::string s(buff);
        
		std::stringstream tmpbuff;
        
		unsigned int ntimes;
		unsigned int semicolon = s.find(';')+1;
        
		tmpbuff << s.substr(semicolon, s.length()-semicolon );
		tmpbuff >> ntimes;
        
		instance->pendingRequests.insert( std::pair< std::string, unsigned int >( s.substr(0, semicolon-1 ), ntimes ) );
	}
    
	s3eFileClose(fhandle);
    
	GAMarmalade::instance->posting = false;
}

void GAMarmalade::pageview(std::string page)
{
	IwAssertMsg(MYAPP,GAMarmalade::instance != NULL, ("Null GAMarmalade Singleton.Please call init."));
    
	page = CIwUriEscape::Escape(page);
    
	if( GAMarmalade::instance->pendingRequests.find( page ) != GAMarmalade::instance->pendingRequests.end() )
	{
		GAMarmalade::instance->pendingRequests[page]++;
	}
	else
	{
		GAMarmalade::instance->pendingRequests.insert( std::pair< std::string, unsigned int >( page , 1 ) );
	}
    
	GAMarmalade::instance->save();
}

void GAMarmalade::post(std::string page)
{
	currpage = page;
    
	std::stringstream finalstream;
    
	postcount++;
	
	time_t previous_visit;
    
	if( instance->postcount == 1)
	{
		timevisit = previous_visit = time(0);
        
		std::stringstream buff;
		buff << timevisit;
		buff >> initial_visit;
	}
	else
	{
		previous_visit = timevisit;
		timevisit = time(0);
	}
    
	finalstream << "http://www.google-analytics.com/__utm.gif?utmwv=5.3.3&utms=3&utmn=" <<
    rand() << rand() << "&utmhn=" << instance->mSite <<
	"&utmcs=UTF-8&utmsc=32-bit&utmul=en-us&utmje=1&utmfl=11.3%20r300&utmdt=null&utmr=-&utmp=" <<
	page << "&utmac=" << instance->mGA_ID << "&utmcc=__utma%3D" << instance->mDomainHash <<
	"." << visitor_id << "." << initial_visit + "." << previous_visit << "." << timevisit <<
	"." << postcount << "%3B%2B" << "__utmz%3D" << instance->mDomainHash << "." << timevisit <<
	"." << postcount << ".1.utmcsr%3Dgoogle%7Cutmccn%3D(organic)%7Cutmcmd%3Dorganic%7Cutmctr%3D(not%2520provided)%3B&utmu=q~";
	
	instance->http.Post( finalstream.str().c_str(), NULL, 0, GotHeaders, 0 );
}

void GAMarmalade::save()
{
	GAMarmalade::instance->fhandle = s3eFileOpen("gooanlycsreq.txt", "w");
    
	for( std::map< std::string, unsigned int >::iterator it = GAMarmalade::instance->pendingRequests.begin();
		it != GAMarmalade::instance->pendingRequests.end(); it++ )
	{
		std::stringstream tmpbuff;
		tmpbuff << it->first << ";"<< it->second << std::endl;
        
		s3eFileWrite( tmpbuff.str().c_str(), strlen( tmpbuff.str().c_str() ), 1, GAMarmalade::instance->fhandle );
	}
    
	s3eFileClose(GAMarmalade::instance->fhandle);
}