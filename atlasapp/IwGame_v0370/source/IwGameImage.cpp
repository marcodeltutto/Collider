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

#include "IwGameImage.h"
#include "IwGameFile.h"
#include "IwGameString.h"
#include "IwGameScene.h"
#include "IwGameResources.h"
#include "IwGameActor.h"
#include "png.h"
//#include "pngstruct.h"

//
//
//
//
// CIwGameImage implementation
//
//
//
//

CIwGameImage* CIwGameImage::getCopy()
{
	if (Image2D == NULL)
		return NULL;

	CIwGameImage* image = new CIwGameImage();
	image->NameHash = NameHash;
	image->State = State;
	image->ResourceGroup = ResourceGroup;
	image->Width = Width;
	image->Height = Height;
	image->Image2D = Iw2DCreateImage(Image2D->GetMaterial()->GetTexture()->GetImage());

	return image;
}

int32 CIwGameImage_FileRetrievedAsyncCallback(void* caller, void* data)
{
	CIwGameImage* image = (CIwGameImage*)data;
	image->FinishLoad();

	return 0;
}

bool CIwGameImage::Load(bool blocking)
{
	// If already loaded return true
	if (Image2D != NULL)
		return true;

	// Image is file based
	if (File != NULL)
	{
		File->setFileAvailableCallback(CIwGameImage_FileRetrievedAsyncCallback, this);
		if (File->Open(NULL, "rb", blocking))
			return true;
		else
			return false;
	}

	if (ResourceGroup != NULL)
	{
		// Create image from the resource group
		IwGetResManager()->SetCurrentGroup(ResourceGroup);
		Image2D = Iw2DCreateImageResource(NameHash);
		if (Image2D != NULL)
		{
			Width = Image2D->GetWidth();
			Height = Image2D->GetHeight();
			State = State_Loaded;
			return true;
		}
	}

	return false;
}

void CIwGameImage::FinishLoad()
{
	if (File != NULL)
	{
		if (CIwGameFile::isHttp(File->getFilename().c_str(), File->getFilename().GetLength()))
		{
			if (File->isFileAvailable() && File->getError() == CIwGameFile::ErrorNone)
				Init(File->getContent(), File->getContentLength());
		}
		else
		{
			char* data = new char [File->getFileSize()];
			File->Read(data, File->getFileSize());
			Init(data, File->getFileSize());
			delete data;
		}
		State = State_Loaded;
		SAFE_DELETE(File)
	}
}

bool CIwGameImage::Init(void* memory_file, int memory_file_size)
{
	CIwGameFile file;
	if (file.Open(memory_file, memory_file_size))
	{
		// Load the image
		CIwImage image;
		image.ReadFile(file.getFileHandle());

		// if required format is different then convert it
		if (image.GetFormat() != Format)
		{
			CIwImage* new_image = ConvertToFormat(image, Format);
			Image2D = Iw2DCreateImage(*new_image);
			delete new_image;
		}
		else
			Image2D = Iw2DCreateImage(image);

		if (FilterSet)
			setFilter(Filter);

		if (Image2D == NULL || image.GetFormat() == CIwImage::FORMAT_UNDEFINED)
		{
#if defined(_DEBUG)
			CIwGameError::LogError("Error: CIwGameImage::Init() - Could not create image!");
#endif	// _DEBUG
			return false;
		}
		else
		{
			Width = Image2D->GetWidth();
			Height = Image2D->GetHeight();
#if defined(_DEBUG)
			CIwGameError::LogError("Info: CIwGameImage::Init() - Size = ", CIwGameString(memory_file_size).c_str());
			CIwGameError::LogError("Info: CIwGameImage::Init() - Width = ", CIwGameString(Width).c_str());
			CIwGameError::LogError("Info: CIwGameImage::Init() - Height = ", CIwGameString(Height).c_str());
			CIwGameError::LogError("Info: CIwGameImage::Init() - Bit depth = ", CIwGameString(image.GetBitDepth()).c_str());
			CIwGameError::LogError("Info: CIwGameImage::Init() - Format = ", CIwGameString(image.GetFormat()).c_str());
#endif	// _DEBUG
		}
	}

	// Sanity check
	if (Width <= 0 || Height <= 0 || Width > 16384 || Height > 16384)
		return false;

	State = State_Loaded;

	return true;
}

CIwImage* CIwGameImage::ConvertToFormat(CIwImage& source, CIwImage::Format format)
{
	CIwImage* img = new CIwImage();
	img->SetFormat(format);
	img->SetWidth(source.GetWidth());
	img->SetHeight(source.GetHeight());
	//img->SetPitch(source.GetPitch());
	source.ConvertToImage(img);

	return img;
}



/*
// Compression requires version 8 of libjpg
void CIwGameImage_init_buffer(jpeg_compress_struct* cinfo) {}
boolean CIwGameImage_empty_buffer(jpeg_compress_struct* cinfo) { return TRUE; }
void CIwGameImage_term_buffer(jpeg_compress_struct* cinfo) {}

char* CIwGameImage::CreateJPEG()
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr       jerr;
	struct jpeg_destination_mgr dmgr;
 
	CIwTexture* texture = Image2D->GetMaterial()->GetTexture();
	int bpp = (texture->GetDepth() + 7) >> 3;
	CIwImage::Format format = texture->GetFormat();
	uint8* texels = texture->GetTexels();

	// Create our memory buffer to hold compressed JPEG
	JOCTET * out_buffer   = new JOCTET[Width * Height * 3];
 
	// Set up manager
	dmgr.init_destination    = CIwGameImage_init_buffer;
	dmgr.empty_output_buffer = CIwGameImage_empty_buffer;
	dmgr.term_destination    = CIwGameImage_term_buffer;
	dmgr.next_output_byte    = out_buffer;
	dmgr.free_in_buffer      = Width * Height * bpp;
 
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
 
	cinfo.dest = &dmgr;
	cinfo.image_width      = Width;
	cinfo.image_height     = Height;
	cinfo.input_components = 3;
	cinfo.in_color_space   = JCS_RGB;
 
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality (&cinfo, 90, true);
	jpeg_start_compress(&cinfo, true);

	uint8* converted = new uint8 [Width * 3];
	while (cinfo.next_scanline < cinfo.image_height)
	{
		JSAMPROW row_pointer;
		if (format == CIwImage::Format::RGB_888)
			row_pointer = (JSAMPROW)&texels[cinfo.next_scanline * Width * bpp];
		else
		{
			row_pointer = (JSAMPROW)converted;
			if (format == CIwImage::Format::RGB_565)
			{
				uint8* in_data = &texels[cinfo.next_scanline * Width * bpp];
				uint8* out_data = &converted[cinfo.next_scanline * Width * bpp];
				for (int t = 0; t < Width; t++)
				{
					int rgb = *in_data++;
					*out_data++ = (rgb & 0xf800) >> 8;
					*out_data++ = (rgb & 0x07e0) >> 3;
					*out_data++ = (rgb & 0x001f) << 3;
				}
			}
		}
		jpeg_write_scanlines(&cinfo, &row_pointer, 1);
	}
	jpeg_finish_compress(&cinfo);

	delete converted;

	return (char*)out_buffer;
}*/

struct CIwGameImage_png_encode
{
  char*		buffer;
  size_t	size;
};

void CIwGameImage_png_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	CIwGameImage_png_encode* pd = (CIwGameImage_png_encode*)png_ptr->io_ptr;
	memcpy(pd->buffer + pd->size, data, length);

	pd->size += length;
}

void CIwGameImage_png_flush(png_structp png_ptr)
{
}

uint8* CIwGameImage::CreatePNG(int& size)
{
	int			width = Width;
	int			height = Height;
	uint8*		buffer = (uint8*)Image2D->GetMaterial()->GetTexture()->GetTexels();
	int			code = 0;
	FILE*		fp = NULL;
	png_structp png_ptr = NULL;
	png_infop	info_ptr = NULL;
	png_bytep	row = NULL;
	CIwGameImage_png_encode	png_state;
	bool		error = false;
	CIwImage::Format format = Image2D->GetMaterial()->GetTexture()->GetFormat();

	if (format != CIwImage::RGB_565 && format != CIwImage::RGB_888 && format != CIwImage::RGBA_8888)
		return NULL;

	int bbp = 2;
	if (format == CIwImage::RGB_888)
		bbp = 3;
	else
	if (format == CIwImage::RGBA_8888)
		bbp = 4;

	png_state.buffer = new char [width * height * bbp];
	png_state.size = 0;
	if (png_state.buffer == NULL)
	{
		goto cleanup;
		error = true;
	}

	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		goto cleanup;
		error = true;
	}

	// Initialize info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		goto cleanup;
		error = true;
	}

	// Setup Exception handling
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		goto cleanup;
		error = true;
	}

	// Set write function and data buffer
	png_set_write_fn(png_ptr, &png_state, CIwGameImage_png_write_data, CIwGameImage_png_flush);
	png_init_io(png_ptr, (png_FILE_p)&png_state);

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);

	// Allocate memory for one row
	row = (png_bytep)new char[3 * width * sizeof(png_byte)];

	// Write image data
	int x, y;
	for (y = 0; y < height; y++)
	{
		png_byte* out_data = row;
		if (format == CIwImage::RGB_565)
		{
			uint16* in_data = (uint16*)&buffer[y * width * 2];
			for (x = 0; x < width; x++)
			{
				uint16 rgb = *in_data++;
				*out_data++ = (rgb & 0xf800) >> 8;
				*out_data++ = (rgb & 0x07e0) >> 3;
				*out_data++ = (rgb & 0x001f) << 3;
			}
		}
		else
		if (format == CIwImage::RGB_888)
		{
			uint8* in_data = (uint8*)&buffer[y * width * 3];
			for (x = 0; x < width; x++)
			{
				*out_data++ = *in_data++;
				*out_data++ = *in_data++;
				*out_data++ = *in_data++;
			}
		}
		else
		if (format == CIwImage::RGBA_8888)
		{
			uint8* in_data = (uint8*)&buffer[y * width * 4];
			for (x = 0; x < width; x++)
			{
				*out_data++ = *in_data++;
				*out_data++ = *in_data++;
				*out_data++ = *in_data++;
				in_data++;
			}
		}
		png_write_row(png_ptr, row);
	}

	// End write
	png_write_end(png_ptr, NULL);

	// Cleanup
cleanup:
	if (info_ptr != NULL)
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL)
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	if (row != NULL)
		delete row;
	if (error && png_state.buffer != NULL)
	{
		delete png_state.buffer;
		png_state.buffer = NULL;
		png_state.size = 0;
	}

	size = png_state.size;
	return (uint8*)png_state.buffer;
}

bool CIwGameImage::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Process image attributes
	CIwGameString* name = NULL;
	CIwGameString* location = NULL;
	bool preload = true;
	bool blocking = false;
	CIwGameString* condition = NULL;

	CIwGameScene* scene = NULL;
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		scene = ((CIwGameActor*)parent)->getScene();
	else
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
		{
			name = &(*it)->GetValue();
			setName(name->c_str());
		}
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Location_Hash)
			location = &(*it)->GetValue();
		else
 		if (name_hash == CIwGameXomlNames::Preload_Hash)
			preload = (*it)->GetValueAsBool();
		else
 		if (name_hash == CIwGameXomlNames::Blocking_Hash)
			blocking = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::Condition_Hash)
			condition = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Format_Hash)
		{
			FormatSet = true;
			unsigned int format_hash = (*it)->GetValue().getHash();
			if (format_hash == IW_GAME_HASH("RGB_565"))
				Format = CIwImage::RGB_565;
			else
			if (format_hash == IW_GAME_HASH("RGBA_4444"))
				Format = CIwImage::RGBA_4444;
			else
			if (format_hash == IW_GAME_HASH("RGBA_5551"))
				Format = CIwImage::RGBA_5551;
			else
			if (format_hash == IW_GAME_HASH("RGB_888"))
				Format = CIwImage::RGB_888;
			else
			if (format_hash == IW_GAME_HASH("RGBA_6666"))
				Format = CIwImage::RGBA_6666;
			else
			if (format_hash == IW_GAME_HASH("RGB_332"))
				Format = CIwImage::RGB_332;
			else
			if (format_hash == IW_GAME_HASH("RGBA_8888"))
				Format = CIwImage::RGBA_8888;
			else
			{
				FormatSet = false;
				CIwGameError::LogError("Warning: Invalid texture format set - ", (*it)->GetValue().c_str());
			}
		}
		else
 		if (name_hash == CIwGameXomlNames::Filter_Hash)
			setFilter((*it)->GetValueAsBool());
	}

	if (location == NULL || name == NULL)
	{
		CIwGameError::LogError("Error: An Image requires a location and a name to be specified");
		return false;
	}

	if (condition != NULL)
	{
		// Find the condition variable
		bool condition_not = false;
		CIwGameXomlVariable* var = NULL;
		if (*(condition->c_str()) == '!')
		{
			condition_not = true;
			CIwGameString cond = condition->c_str() + 1;
			var = CIwGameXomlVariable::GetVariable(cond, scene);
		}
		else
			var = CIwGameXomlVariable::GetVariable(*condition, scene);
		if (var != NULL)
		{
			bool res = var->isTrue();
			if (condition_not)
				res = !res;
			if (!res)
			{
				IW_GAME_XOML->setExitOnError(false);
				return false;
			}
		}
#if defined (_DEBUG)
		else
			CIwGameError::LogError("Warning: condition variable not found - ", condition->c_str());
#endif // _DEBUG
	}

	// Check to see if image is located externally
	if (CIwGameFile::isHttp(location->c_str(), location->GetLength()))
	{
		Init(location->c_str());
	}
	else
	if (CIwGameFile::isFile(location->c_str(), location->GetLength()))
	{
		Init(location->c_str());
	}
	else
	{
		// Find resuorce group
		CIwGameResourceGroup* group = NULL;
		if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
			group = (CIwGameResourceGroup*)scene->getResourceManager()->findResource(location->c_str(), CIwGameXomlNames::ResourceGroup_Hash);
		else
			group = (CIwGameResourceGroup*)IW_GAME_GLOBAL_RESOURCES->getResourceManager()->findResource(location->c_str(), CIwGameXomlNames::ResourceGroup_Hash);

		if (group != NULL)
			Init(name->c_str(), group->getResourceGroup());
		else
		{
			CIwGameError::LogError("Error: XOML - Invalid image resource group name - ", location->c_str());
			return false;
		}
	}
	if (preload)
	{
		Load(blocking);
	}

	// If we are declared inside a scene then image is local to the scene
	if (scene != NULL)
		scene->getResourceManager()->addResource(this);
	else
		IW_GAME_GLOBAL_RESOURCES->getResourceManager()->addResource(this);

	return true;
}


