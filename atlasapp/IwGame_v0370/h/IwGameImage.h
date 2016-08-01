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

#if !defined(_CIW_GAME_IMAGE_H_)
#define _CIW_GAME_IMAGE_H_

#include "IwGx.h"
#include "Iw2D.h"
#include "IwList.h"
#include "IwGameUtil.h"
#include "IwGameXoml.h"
#include "IwGameFile.h"

class CIwGameXomlResourceManager;

//
//
//
//
// CIwGameImage - Represents a bitmapped image
//
// Note that images can be either loaded on demand (loaded on first call to getImage2D() or pre loaded by calling Load()
//
//
//
//
class CIwGameImage : public IIwGameXomlResource
{
public:
	enum eCIwGameImage_State
	{
		State_Invalid, 
		State_Loaded, 
	};

	// Properties
protected:
	eCIwGameImage_State		State;				// State of image
	CIwResGroup*			ResourceGroup;		// Resource group that our image lives in
	CIw2DImage*				Image2D;			// Iw2D Image
	int						Width;				// Pixel width of image
	int						Height;				// Pixl height of image
	bool					Filter;				// Should this image be filtered
	bool					FilterSet;			// True if the filter was set
	CIwImage::Format		Format;				// Format to convert texture to
	bool					FormatSet;			// If set then format conversion will take place
public:
	eCIwGameImage_State		getState() const					{ return State; }
	CIw2DImage*				getImage2D()						{ if (Load()) return Image2D; return NULL; }
	int						getWidth() const					{ return Width; }
	int						getHeight() const					{ return Height; }
	CIwGameImage*			getCopy();
	void					setFilter(bool enable)				{ Filter = enable; if (Image2D != NULL) Image2D->GetMaterial()->GetTexture()->SetFiltering(Filter); FilterSet = true; }
	bool					isFilter() const					{ return Filter; }
	bool					isFilterSet() const					{ return FilterSet; }
	void					setFormat(CIwImage::Format format)	{ Format = format; FormatSet = true; }
	CIwImage::Format		getFormat() const					{ return Format; }
	// Properties End
protected:
	CIwGameFile*			File;				// File object (if image is file based)
	bool					DecompressJPEG(char* jpeg_data, int jpeg_data_size);

public:
	CIwGameImage() :	IIwGameXomlResource(), ResourceGroup(NULL), Image2D(NULL), File(NULL), State(State_Invalid), Width(0), Height(0), Format(CIwImage::RGBA_5551), FormatSet(false),
						Filter(true), FilterSet(false){ setClassType("image"); }
	virtual ~CIwGameImage()
	{
		SAFE_DELETE(Image2D)
		SAFE_DELETE(File)
	}

	void			Init(const char* ResourceName, CIwResGroup* resource_group)		// Init an image from an image located within a resource group (image is not loaded)
	{
		setName(ResourceName);
		ResourceGroup = resource_group;
	}

	bool			Init(void* memory_file, int memory_file_size);	// Init an image from a memory based file (image is loaded)
	void			Init(const char* filename)						// Init an image from a file (image is not loaded)
	{
		File = new CIwGameFile();
		File->setFilename(filename);
	}

	bool			Load(bool blocking = true);						// Force load the image

	// Utility
//	char*			CreateJPEG();
	uint8*			CreatePNG(int& size);
	CIwImage*		ConvertToFormat(CIwImage& source, CIwImage::Format formmat);

	// Implementation of IIwGameXomlClass interface
	bool			LoadFromXoml(IIwGameXomlResource* parebt, bool load_children, CIwGameXmlNode* node);

	// Internal
	void			FinishLoad();									// Called back when aysnc loading is completed
};

//
//  CIwGameImageCreator - Creates an instance of an image object
//
class CIwGameImageCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameImageCreator()
	{
		setClassName("image");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameImage(); }
};

#endif	// _CIW_GAME_IMAGE_H_
