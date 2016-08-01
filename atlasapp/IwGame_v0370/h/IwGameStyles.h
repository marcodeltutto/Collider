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