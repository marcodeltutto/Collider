// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. The only restriction to its usage is that this header must remain intact and visible in all IwGame engine files.
// If you use this engine in your product, whilst it is not mandatory, a mention of the IwGame engine would be nice or a quick email to let us know where it is being used.
//
//

#if !defined(_IW_GAME_MODIFIER_H_)
#define _IW_GAME_MODIFIER_H_

#include "IwGameUtil.h"
#include "IwGameString.h"
#include "IwGameXoml.h"

class CIwGameModifierManager;

//
//
//
//
// IIwGameModifier - A class Modifier is attached to a class to modify its behvaiour
//
//
//
//
class IIwGameModifier
{
public:
	// Properties
protected:
	CIwGameModifierManager* Parent;						// Parent manager
	CIwGameString			Params[4];					// Modifier parameters
#if defined (_DEBUG)
	CIwGameString			Name;						// Modifier name
#endif	// _DEBUG
	unsigned int			NameHash;					// Modifier name hash
	bool					Active;						// Modifiers active state
	bool					Initialised;				// Initialised state
public:
	CIwGameModifierManager* getParent()									{ return Parent; }
	void					setParent(CIwGameModifierManager* parent)	{ Parent = parent; }
	void					setName(const char* name)
	{
#if defined (_DEBUG)
		Name = name;
#endif	// _DEBUG
		NameHash = CIwGameString::CalculateHash(name);
	}
#if defined (_DEBUG)
	CIwGameString&			getName()						{ return Name; }
#endif	// _DEBUG
	unsigned int			getNameHash()					{ return NameHash; }
	void					setActive(bool active)			{ Active = active; }
	bool					isActive() const				{ return Active; }
	void					setInitialised(bool init)		{ Initialised = init; }
	bool					isInitialised() const			{ return Initialised; }
	void					setParameter(int index, CIwGameString& data)			{ Params[index] = data; }
	void					setParameter(int index, const char* data)				{ Params[index] = data; }
	// Properties End
public:
	IIwGameModifier() : Active(true), Initialised(false), Parent(NULL) {}
	virtual void			InitModifier(IIwGameXomlResource* target) = 0;				// Initialise the Modifier
	virtual void			ReleaseModifier(IIwGameXomlResource* target) = 0;			// Clean-up the Modifier
	virtual bool			UpdateModifier(IIwGameXomlResource* target, float dt) = 0;	// Update the Modifier
};

// 
// 
//
//
// CIwGameModifierManager - Manages a collection of class modifiers
//
//
//
//
class CIwGameModifierManager : public IIwGameXomlResource
{
public:
	// Public access to iteration
	typedef CIwList<IIwGameModifier*>::iterator _Iterator;
	_Iterator				begin()		{ return Modifiers.begin(); }
	_Iterator				end()		{ return Modifiers.end(); }

protected:
	// Properties
	CIwList<IIwGameModifier*>	Modifiers;			// A collection of class modifiers
public:
	bool					addModifier(IIwGameModifier* modifier);
	bool					removeModifier(IIwGameModifier* modifier);
	IIwGameModifier*		findModifier(unsigned int name_hash);
	IIwGameModifier*		findModifier(const char* name);
	void					clearModifiers();
	// Properties end

public:
	CIwGameModifierManager() : IIwGameXomlResource() { setClassType("modifiers"); }
	~CIwGameModifierManager() { clearModifiers(); }

	void					Execute(IIwGameXomlResource* target, float dt);

	// Implementation of IIwGameXomlResource interface
	bool					LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  CIwGameModifierManagerCreator - Creates an instance of a modifier manager
//
class CIwGameModifierManagerCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameModifierManagerCreator()
	{
		setClassName("modifiers");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameModifierManager(); }
};

// 
// 
//
//
// IIwGameModifierCreator - Base class that is used by classes that create an instance of a class mdofier
//
//
//
//
class IIwGameModifierCreator
{
#if defined(_DEBUG)
	CIwGameString			ClassName;				// Name of class
#endif
	unsigned int			ClassNameHash;			// Hashed name of class
public:
	void					setClassName(const char* name)
	{
#if defined(_DEBUG)
		ClassName = name;
#endif
		ClassNameHash = CIwGameString::CalculateHash(name);
	}
	unsigned int			getClassNameHash() const			{ return ClassNameHash; }
#if defined(_DEBUG)
	CIwGameString&			getClasstName()						{ return ClassName; }
#endif
public:
	virtual IIwGameModifier* CreateInstance() = 0;
};

// 
// 
//
//
// CIwGameMods - CIwGameMods is the main controller responsible for instantiating class modifiers
// 
// 
// 
//
#define IW_GAME_MODS	CIwGameMods::getInstance()
class CIwGameMods
{
public:
	CDEFINE_SINGLETON(CIwGameMods)

	// Public access to class creator iteration
	typedef CIwList<IIwGameModifierCreator*>::iterator _Iterator;
	_Iterator				begin() { return ModifierCreators.begin(); }
	_Iterator				end() { return ModifierCreators.end(); }

protected:
	// Properties
public:
	void						addModifier(IIwGameModifierCreator* creator)
	{
#if defined(_DEBUG)
		if (findCreator(creator->getClassNameHash()))
		{
			CIwGameError::LogError("Warning: Modifier creator already exists in modifier creator list - ", creator->getClasstName().c_str());
			delete creator;
			return;
		}
#endif	//(_DEBUG)
		ModifierCreators.push_back(creator);
	}
	IIwGameModifierCreator*		findCreator(unsigned int name_hash);
	IIwGameModifierCreator*		findCreator(const char* name);
	// Properties end

protected:
	CIwList<IIwGameModifierCreator*> ModifierCreators;

public:
	void			Init();
	void			Release();
};

#endif	// _IW_GAME_MODIFIER_H_
