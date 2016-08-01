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

#if !defined(IW_GAME_ACTIONS_H_)
#define IW_GAME_ACTIONS_H_

#include "s3e.h"

#include "IwGameUtil.h"
#include "IwGameXoml.h"


//
//
//
//
//  CIwGameEvent - Repersents some named event with an actions list
//
//
//
//
struct CIwGameEvent
{
	unsigned int		EventName;		// Event method type (e.g. OnResume, OnTapped)
	unsigned int		ActionsName;	// Name of Actions collection to perform when event occurs
#if defined(_DEBUG)
	CIwGameString		_EventName;
	CIwGameString		_ActionsName;
#endif	//_DEBUG
};

//
//
//
//
//  CIwGameEventManager - Manages a collection of events
//
//
//
//
class CIwGameEventManager
{
public:
	// Public access to event iteration
	typedef CIwList<CIwGameEvent*>::iterator _Iterator;
	_Iterator					begin() { return Events.begin(); }
	_Iterator					end() { return Events.end(); }
protected:
	CIwList<CIwGameEvent*> Events;
public:
	CIwGameEventManager() {}
	virtual ~CIwGameEventManager()
	{
		for (_Iterator it = begin(); it != end(); ++it)
		{
			delete *it;
		}
		Events.clear();
	}

	void	addEvent(const char* event_name, const char* actions_name, bool replace = false)
	{
		CIwGameEvent* evt = NULL;
		if (replace)
			evt = findEvent(IW_GAME_HASH(event_name));
		if (evt == NULL)
			evt = new CIwGameEvent();
		evt->EventName = IW_GAME_HASH(event_name);
		evt->ActionsName = IW_GAME_HASH(actions_name);
#if defined(_DEBUG)
		evt->_EventName = event_name;
		evt->_ActionsName = actions_name;
#endif	//_DEBUG
		Events.push_back(evt);
	}

	void	addEvent(CIwGameEvent* evt)
	{
		Events.push_back(evt);
	}

	CIwGameEvent* findEvent(unsigned int event_name_hash)
	{
		for (_Iterator it = begin(); it != end(); ++it)
		{
			if ((*it)->EventName == event_name_hash)
				return *it;
		}

		return NULL;
	}
};

//
//
//
//
//  CIwGameAction - An action is a command that can be excuted from XOML
//
//
//
//
struct CIwGameAction
{
	unsigned int			MethodName;		// Action method type
	CIwGameString			Params[5];		// Action parameters
	CIwGameXomlVariable*	Variables[5];	// Cached variable parameters (prevents continually searching for variables)
	unsigned int			Conditions;		// Name of conditions variable that defines a set of conditions that must be met for this action to be executed
	bool					Not;			// if true then result of condition will be inversed
#if defined(_DEBUG)
	CIwGameString			_MethodName;
#endif	//_DEBUG
	CIwGameString			_Conditions;
	CIwGameAction() : MethodName(0), Conditions(0), Not(false) {}
	void					setMethodName(const char* name)
	{
#if defined(_DEBUG)
		_MethodName = name;
#endif	//_DEBUG
		MethodName = IW_GAME_HASH(name);
	}
	unsigned int			getMethodName()	const					{ return MethodName; }
	void					setParameter1(CIwGameString& p)			{ Params[0] = p; }
	CIwGameString&			getParameter1(IIwGameXomlResource* parent = NULL);
	void					setParameter2(CIwGameString& p)			{ Params[1] = p; }
	CIwGameString&			getParameter2(IIwGameXomlResource* parent = NULL);
	void					setParameter3(CIwGameString& p)			{ Params[2] = p; }
	CIwGameString&			getParameter3(IIwGameXomlResource* parent = NULL);
	void					setParameter4(CIwGameString& p)			{ Params[3] = p; }
	CIwGameString&			getParameter4(IIwGameXomlResource* parent = NULL);
	void					setParameter5(CIwGameString& p)			{ Params[4] = p; }
	CIwGameString&			getParameter5(IIwGameXomlResource* parent = NULL);
	void					setConditions(const char* cond)
	{
		_Conditions = cond;
		Conditions = IW_GAME_HASH(cond);
	}
	unsigned int			getConditions()	const	{ return Conditions; }


};

//
//
//
//
//  CIwGameActions - A collection of actions
//
//
//
//
class CIwGameActions : public IIwGameXomlResource
{
public:
	// Public access to actions iteration
	typedef CIwList<CIwGameAction*>::iterator _Iterator;
	_Iterator					begin() { return Actions.begin(); }
	_Iterator					end() { return Actions.end(); }
	// Properties
protected:
	CIwList<CIwGameAction*>		Actions;			// List of actions
	unsigned int				Conditions;			// Name of conditions variable that defines a set of conditions that must be met for this set of actions to be executed
	CIwGameString				_Conditions;
	bool						Not;				// if true then result of condition will be inversed
public:
	void						setConditions(const CIwGameString& conditions)
	{
		Conditions = conditions.getHash();
		_Conditions = conditions;
	}
	unsigned int				getConditions() const							{ return Conditions; }
	void						setNot(bool _not)								{ Not = _not; }
	bool						isNot() const									{ return Not; }
	// Properties end

	CIwGameActions() : IIwGameXomlResource(), Conditions(0), Not(false) { setClassType("actions");	}
	virtual ~CIwGameActions()
	{
		for (_Iterator it = begin(); it != end(); ++it)
			delete *it;
		Actions.clear();
	}

	void			Execute(IIwGameXomlResource* source);

	// Implementation of IIwGameXomlResource interface
	bool			LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//
//
//
//  CIwGameActionsreator - Creates an instance of an actions ovject
//
//
//
//
class CIwGameActionsCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameActionsCreator()
	{
		setClassName("actions");
	}
	IIwGameXomlResource* CreateInstance(IIwGameXomlResource* parent) { return new CIwGameActions(); }
};

// 
// 
//
//
// CIwGameActionsManager - Manages a collection of actions
//
//
//
//
class CIwGameActionsManager
{
public:
	// Public access to iteration
	typedef CIwList<CIwGameActions*>::iterator _Iterator;
	_Iterator				begin()		{ return Actions.begin(); }
	_Iterator				end()		{ return Actions.end(); }

protected:
	// Properties
	CIwList<CIwGameActions*>		Actions;			// A collection of actionss
	IIwGameXomlResource*			Parent;				// Parent container
public:
	void					addActions(CIwGameActions* actions);
	void					removeActions(CIwGameActions* Actions);
	void					removeActions(unsigned int name_hash);
	CIwGameActions*			findActions(unsigned int name_hash);
	CIwGameActions*			findActions(const char* name);
	void					clearActionss();
	void					setParent(IIwGameXomlResource* scene)	{ Parent = scene; }
	IIwGameXomlResource*	getParent()								{ return Parent; }
	// Properties end

public:
	CIwGameActionsManager() : Parent(NULL) {}
	~CIwGameActionsManager() { clearActionss(); }

	// Utility
	static CIwGameActions*	FindActions(const char* name, IIwGameXomlResource* container);
	static CIwGameActions*	FindActions(unsigned int name_hash, IIwGameXomlResource* container);
	static CIwGameActions*	FindActions(unsigned int name_hash, CIwGameScene* container, unsigned int actor_hash);
	static CIwGameActions*	FindActions(const char* name, CIwGameScene* container, const char* actor_name);
};


#endif // IW_GAME_ACTIONS_H_