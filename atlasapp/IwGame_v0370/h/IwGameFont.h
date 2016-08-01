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

#if !defined(_CIW_GAME_FONT_H_)
#define _CIW_GAME_FONT_H_

#include "IwGxFont.h"
#include "IwGameUtil.h"
#include "IwGameXoml.h"

class CIwGameXomlResourceManager;

//
//
//
//
// CIwGameFont - Represents a text rendering font
//
//
//
//
class CIwGameFont : public IIwGameXomlResource
{
public:
	enum eCIwGameFont_State
	{
		State_Invalid, 
		State_Loaded, 
	};

	// Properties
protected:
	eCIwGameFont_State		State;				// State of image
	CIwResGroup*			ResourceGroup;		// Resource group that our font lives in
	CIwGxFont*				Font;				// IwGxFont
	int						PointSize;			// Point size of font
	CIwGameString			Filename;			// Name of font file
	CIwGameFont*			UsingFont;			// Points to a font that this font is re-using
public:
	eCIwGameFont_State		getState() const						{ return State; }
	CIwGxFont*				getFont()								{ if (Load()) return Font; return NULL; }
	void					setPointSize(int size)					{ PointSize = size; }
	int						getPointSize() const					{ return PointSize; }
	CIwGameFont*			getUsingFont()							{ return UsingFont; }
	CIwGameString&			getFilename()							{ return Filename; }
	bool					isFontLoaded() const					{ return Font != NULL; }
	// Properties End
protected:
	CIwGameFile*			File;				// File object (if font is file based)
	bool					Delete;

public:
	CIwGameFont() : IIwGameXomlResource(), Font(NULL), UsingFont(NULL), ResourceGroup(NULL), File(NULL), State(State_Invalid), PointSize(8), Delete(false)		{ setClassType("font"); }
	virtual ~CIwGameFont();

	void			Init(const char* ResourceName, CIwResGroup* resource_group)		// Init a font from a font resource located within a resource group
	{
		ResourceGroup = resource_group;
		Font = (CIwGxFont*)resource_group->GetResHashed(IW_GAME_HASH(ResourceName), "CIwGxFont");
		State = State_Loaded;
	}

	bool			Load(bool blocking = true);						// Force load the image

	// Implementation of IIwGameXomlClass interface
	bool			LoadFromXoml(IIwGameXomlResource* parebt, bool load_children, CIwGameXmlNode* node);

	// Internal
	void			FinishLoad();									// Called back when aysnc loading is completed
};

//
//  CIwGameFontCreator - Creates an instance of a font object
//
class CIwGameFontCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameFontCreator()
	{
		setClassName("font");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameFont(); }
};

#endif	// _CIW_GAME_FONT_H_
