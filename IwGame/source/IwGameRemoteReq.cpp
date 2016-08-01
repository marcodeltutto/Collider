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

#include "IwGameUtil.h"
#include "IwGameXoml.h"
#include "IwGameRemoteReq.h"
#include "IwGameScene.h"
#include "IwGameString.h"


//
//
//
//
// CIwGameRemoteReq implementation
//
//
//
//

int32 RemoteReqDataRetrievedCallback(void* caller, void *data)
{
	CIwGameHttpRequest* request = (CIwGameHttpRequest*)caller;
	CIwGameRemoteReq* req = (CIwGameRemoteReq*)data;

	if (request->getProcessed())									// Check to see if our request was processed by the http manager
	{
		req->DataReceived();
		IW_GAME_HTTP_MANAGER->RemoveRequest(request);				// Remove request from http manager queue
	}

	return 0;
}

void CIwGameRemoteReq::DataReceived()
{
	if (Request.getError() != 0)
		ProcessEventActions(CIwGameXomlNames::OnError_Hash);
	else
	{
		if (Variable != NULL)
			Variable->setValue(Request.getContent().c_str());

		ProcessEventActions(CIwGameXomlNames::OnResponse_Hash);
	}
}

CIwGameRemoteReq::~CIwGameRemoteReq()
{
	IW_GAME_HTTP_MANAGER->RemoveRequest(&Request);
	SAFE_DELETE(EventsManager)
}

void CIwGameRemoteReq::ProcessEventActions(unsigned int event_name)
{
	// Find the event
	CIwGameEvent* evt = EventsManager->findEvent(event_name);
	if (evt != NULL)
	{
		// Find actions list
		CIwGameActions* actions = CIwGameActionsManager::FindActions(evt->ActionsName, Parent->getParent());
		if (actions != NULL)
			actions->Execute(this);
	}
}

bool CIwGameRemoteReq::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	CIwGameString* var_name = NULL;
	CIwGameString* data = NULL;
	CIwGameScene* scene = NULL;
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
			setName((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::URL_Hash)
			setUrl((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Data_Hash)
			Request.setBody((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnResponse_Hash)
			EventsManager->addEvent("OnTapped", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::OnError_Hash)
			EventsManager->addEvent("OnError", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Post_Hash)
		{
			if ((*it)->GetValueAsBool())
				Request.setPOST();
			else
				Request.setGET();
		}
		else
		if (name_hash == CIwGameXomlNames::Variable_Hash)
			var_name = &(*it)->GetValue();
	}

	Request.setContentAvailableCallback(&RemoteReqDataRetrievedCallback, this);

	// Find and attach the variable
	if (var_name != NULL)
	{
		CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(*var_name, scene);
		if (var == NULL)
			CIwGameError::LogError("Warning: RemoteReq target variable not found - ", var_name->c_str());
		Variable = var;
	}


	if (scene != NULL)
		return scene->getResourceManager()->addResource(this);
	else
		return IW_GAME_GLOBAL_RESOURCES->getResourceManager()->addResource(this);

	return true;
}

void CIwGameRemoteReq::MakeRequest()
{
	IW_GAME_HTTP_MANAGER->AddRequest(&Request);
}


