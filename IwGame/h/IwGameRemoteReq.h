// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. You must ensure that this whole notice is present in all files and derivatives, so the reader knows its origin.
// If you use this engine in your product then please ensure that you credit IwGame's usage appropriately. Please see http://www.drmop.com/index.php/iwgame-engine/ for licensing details and support
//
//

#if !defined(_IW_GAME_REMOTEREQ_H_)
#define _IW_GAME_REMOTEREQ_H_


#include "IwGameUtil.h"
#include "IwGameXoml.h"
#include "IwGameHttp.h"
#include "IwGameActions.h"

// 
// 
//
//
// CIwGameRemmoteReq - A remote request 
// 
// 
// 
//

class CIwGameRemoteReq : public IIwGameXomlResource
{
public:
	// Properties
protected:
	CIwGameEventManager*		EventsManager;			// List of events
	CIwGameXomlVariable*		Variable;				// Variable that is will receieve the resopnse data
	bool						Post;					// Uses post if true otherwise uses get
public:
	// Properties end
	void						setUrl(const char* url)			{ Request.setURI(url); }
	CIwGameEventManager*		getEventsManager()				{ return EventsManager; }
	void						setPost(bool post)				{ Post = post; }
	bool						isPost() const					{ return Post; }
	void						setVariable(CIwGameXomlVariable* var)	{ Variable = var; }
	CIwGameXomlVariable*		getVariable()							{ return Variable; }
	void						setData(const char* data)		{ Request.setBody(data); }
protected:
	CIwGameHttpRequest			Request;
	void						ProcessEventActions(unsigned int event_name);


public:
	CIwGameRemoteReq() : IIwGameXomlResource(), EventsManager(NULL), Variable(NULL), Post(false)
	{
		setClassType("remotereq");
		EventsManager = new CIwGameEventManager();
	}
	virtual ~CIwGameRemoteReq();

	// Implementation of IIwGameXomlClass interface
	bool						LoadFromXoml(IIwGameXomlResource* parebt, bool load_children, CIwGameXmlNode* node);

	void					MakeRequest();
	void					DataReceived();
};

//
//
//
//
//  CIwGameRemoteReqCreator - Creates an instance of a RemoteReq
//
//
//
//
class CIwGameRemoteReqCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameRemoteReqCreator()
	{
		setClassName("remotereq");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameRemoteReq(); }
};




#endif	// _IW_GAME_REMOTEREQ_H_
