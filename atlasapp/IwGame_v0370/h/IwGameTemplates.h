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

#if !defined(_IW_GAME_TEMPLATES_H_)
#define _IW_GAME_TEMPLATES_H_

#include "s3e.h"

#include "IwGameUtil.h"
#include "IwGameXoml.h"

//
//
//
//
//  CIwGameTemplate - A XOML template which can be used to instantiate a custom version of a class
//
//
//
//
class CIwGameTemplate : public IIwGameXomlResource
{
public:
public:
	// Properties
protected:
	unsigned int			TargetType;				// Type of class that this template can be applied to such as actor, scene etc..
	CIwGameXmlNode*			Root;					// Root node
public:
	unsigned int			getTargetType() const					{ return TargetType; }
	CIwGameXmlNode*			getRoot()								{ return Root; }
	// Properties end
protected:
	void		ReplaceAttributes(CIwGameXmlAttribute* attrib, CIwGameXmlNode* replacements);
	void		ReplaceTreeAttributes(CIwGameXmlNode* node, CIwGameXmlNode* replacements);
public:
	CIwGameTemplate() : IIwGameXomlResource(), Root(NULL) { setClassType("template"); }
	virtual ~CIwGameTemplate()
	{
		SAFE_DELETE(Root)
	}

	bool				Instantiate(IIwGameXomlResource* parent, CIwGameXmlNode* replacements);

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//
//
//
//  CIwGameTemplateCreator - Creates an instance of a template
//
//
//
//
class CIwGameTemplateCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameTemplateCreator()
	{
		setClassName("template");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameTemplate(); }
};

//
//
//
//
// CIwGameTemplateFrom utility class to allow loading of XOMl files from XOML
// 
//
// 
//
class CIwGameTemplateFrom : public IIwGameXomlResource
{
public:
protected:
	//// Properties
public:
	//// Properties end
protected:

public:
	CIwGameTemplateFrom() : IIwGameXomlResource() { setClassType("fromtemplate"); }
	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parebt, bool load_children, CIwGameXmlNode* node);
};

class CIwGameTemplateFromCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameTemplateFromCreator()
	{
		setClassName("fromtemplate");
	}
	IIwGameXomlResource* CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameTemplateFrom(); }
};




#endif // _IW_GAME_TEMPLATES_H_