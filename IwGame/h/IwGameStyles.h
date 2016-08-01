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

#if !defined(IW_GAME_STYLES_H_)
#define IW_GAME_STYLES_H_

#include "s3e.h"

#include "IwGameUtil.h"
#include "IwGameXoml.h"

//
//
//
//
//  CIwGameStyleProperty - A style property
//
//
//
//
struct CIwGameStyleProperty
{
public:
	CIwGameString	Name;					// Name of property
	CIwGameString	Value;					// Value of the property

	CIwGameStyleProperty() {}
	virtual ~CIwGameStyleProperty() {}
};

//
//
//
//
//  CIwGameStyle - A class that is used to style other classes
//
//
//
//
class CIwGameStyle : public IIwGameXomlResource
{
public:
	// Public access to actor iteration
	typedef CIwList<CIwGameStyleProperty*>::iterator _Iterator;
	_Iterator					begin() { return Properties.begin(); }
	_Iterator					end() { return Properties.end(); }
public:
	// Properties
protected:
	CIwList<CIwGameStyleProperty*>	Properties;
public:
	void						addProperty(CIwGameStyleProperty* prop);
	void						addProperty(const char* property_name, const char* property_value);
	void						clearProperties();
	// Properties end

public:
	CIwGameStyle() : IIwGameXomlResource() { setClassType("style"); }
	virtual ~CIwGameStyle()
	{
		clearProperties();
	}

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//
//
//
//  CIwGameStyleCreator - Creates an instance of a style
//
//
//
//
class CIwGameStyleCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameStyleCreator()
	{
		setClassName("style");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameStyle(); }
};



#endif // IW_GAME_STYLES_H_