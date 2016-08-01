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
	CIwTexture*				Texture;			// Texture
	int						Width;				// Pixel width of image
	int						Height;				// Pixl height of image
	bool					Filter;				// Should this image be filtered
	bool					FilterSet;			// True if the filter was set
	CIwImage::Format		Format;				// Format to convert texture to
	bool					FormatSet;			// If set then format conversion will take place
public:
	eCIwGameImage_State		getState() const					{ return State; }
	CIw2DImage*				getImage2D()						{ if (Load()) return Image2D; return NULL; }
	CIwTexture*				getTexture();
	int						getWidth() const					{ return Width; }
	int						getHeight() const					{ return Height; }
	CIwGameImage*			getCopy();
	void					setFilter(bool enable);
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
						Filter(true), FilterSet(false), Texture(NULL) { setClassType("image"); }
	virtual ~CIwGameImage()
	{
		if (Image2D == NULL)
		{
			if (Texture != NULL)
				delete Texture;
		}
		else
			delete Image2D;
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
	bool			Init(void* pixels, int width, int height, int pitch, CIwImage::Format format, bool modifiable);	// Init an image from raw data
	void			Release();

	bool			Load(bool blocking = true);						// Force load the image

	// Utility
//	char*			CreateJPEG();
//	uint8*			CreatePNG(int& size);
	CIwImage*		ConvertToFormat(CIwImage& source, CIwImage::Format formmat);
    void			SaveBmp(const char* filename);
    void			SaveTga(const char* filename);
    void			SavePng(const char* filename);
    void			SaveJpg(const char* filename, int quality = 100);

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
