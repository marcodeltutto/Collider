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

#include "IwGameFont.h"
#include "IwGameResources.h"
#include "IwGameScene.h"
#include "IwGameActor.h"

//
//
//
//
// CIwGameFont implementation
//
//
//
//

CIwGameFont::~CIwGameFont()
{
	if (Delete)
		SAFE_DELETE(Font)
	SAFE_DELETE(File)
}

bool CIwGameFont::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Process image attributes
	CIwGameString* name = NULL;
	CIwGameString* resource_name = NULL;
	CIwGameString* location = NULL;
	CIwGameString* condition = NULL;
	bool preload = true;
	bool blocking = false;
	float auto_point_size = 0;	// Approximate number of lines of font allows on screen down longest axis

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
		if (name_hash == CIwGameXomlNames::ResourceName_Hash)
			resource_name = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Location_Hash)
			location = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Condition_Hash)
			condition = &(*it)->GetValue();
		else
 		if (name_hash == CIwGameXomlNames::Preload_Hash)
			preload = (*it)->GetValueAsBool();
		else
 		if (name_hash == CIwGameXomlNames::Blocking_Hash)
			blocking = (*it)->GetValueAsBool();
		else
 		if (name_hash == CIwGameXomlNames::PointSize_Hash)
			PointSize = (*it)->GetValueAsInt();
		else
 		if (name_hash == CIwGameXomlNames::AutoPointSize_Hash)
			auto_point_size = (*it)->GetValueAsFloat();
	}

	if (location == NULL || name == NULL)
	{
		CIwGameError::LogError("Error: A Font requires a location and a name to be specified");
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

	// If no resource_name specified then assign it the name
	if (resource_name == NULL)
		resource_name = name;

	// If auto point size then choose point size from screen resolution
	if (auto_point_size > 0)
	{
		int d = IwGxGetScreenWidth() > IwGxGetScreenHeight() ? IwGxGetScreenWidth() : IwGxGetScreenHeight();
		float dy = (float)d / auto_point_size;		// Calculate approximate pixel size
		int point_size = (int)(dy / 1.34f);	// Calculate approximate point size from pixel height of glyph
		PointSize += point_size;
	}

	// Check to see if image is located externally
	if (CIwGameFile::isHttp(location->c_str(), location->GetLength()) || CIwGameFile::isFile(location->c_str(), location->GetLength()))
	{
		if (CIwGameFile::isFile(location->c_str(), location->GetLength()))
		{
			// Check for font already existing
			if (scene != NULL)
				UsingFont = scene->getResourceManager()->findFontWithFilename(location->c_str());
			else
				UsingFont = IW_GAME_GLOBAL_RESOURCES->getResourceManager()->findFontWithFilename(location->c_str()), false;
		}

		Filename = *location;
		if (preload)
			Load(blocking);
	}
	else
	{
		// Find resource group
		CIwGameResourceGroup* group = NULL;
		if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
			group = (CIwGameResourceGroup*)scene->getResourceManager()->findResource(location->c_str(), CIwGameXomlNames::ResourceGroup_Hash);
		else
			group = (CIwGameResourceGroup*)IW_GAME_GLOBAL_RESOURCES->getResourceManager()->findResource(location->c_str(), CIwGameXomlNames::ResourceGroup_Hash);

		if (group != NULL)
		{
			Init(resource_name->c_str(), group->getResourceGroup());
#if defined (_DEBUG)
			CIwGameError::LogError("Info: Font created from resource - ", resource_name->c_str());
#endif // _DEBUG

		}
		else
		{
			CIwGameError::LogError("Error: XOML - Invalid font resource group name - ", location->c_str());
			return false;
		}
	}

	// If we are declared inside a scene then font is local to the scene
	if (scene != NULL)
		scene->getResourceManager()->addResource(this);
	else
		IW_GAME_GLOBAL_RESOURCES->getResourceManager()->addResource(this);

	return true;
}

int32 CIwGameFont_FileRetrievedAsyncCallback(void* caller, void* data)
{
	CIwGameFont* font = (CIwGameFont*)data;
	font->FinishLoad();

	return 0;
}

bool CIwGameFont::Load(bool blocking)
{
	// If already loaded return true
	if (Font != NULL)
		return true;

	if (CIwGameFile::isHttp(Filename.c_str(), Filename.GetLength()))
	{
		if (UsingFont != NULL)
		{
			if (UsingFont->getState() == State_Loaded)
			{
				Font = IwGxFontCreateTTFontFromBuffer((uint8*)UsingFont->File->getContent(), UsingFont->File->getContentLength(), PointSize);
				State = State_Loaded;
				Delete = true;
				return true;
			}
		}
		else
		{
			File = new CIwGameFile();
			File->setFilename(Filename.c_str());
			File->setFileAvailableCallback(CIwGameFont_FileRetrievedAsyncCallback, this);
			if (File->Open(NULL, "rb", blocking))
				return true;
			else
				return false;
		}
	}
	else
	if (CIwGameFile::isFile(Filename.c_str(), Filename.GetLength()))
	{
		// Create font from local file
		Font = IwGxFontCreateTTFont(Filename.c_str(), PointSize);
		if (Font == NULL)
			return false;
		else
		{
			State = State_Loaded;
			Delete = true;
		}
	}

	return false;
}

void CIwGameFont::FinishLoad()
{
	if (File != NULL)
	{
		if (CIwGameFile::isHttp(File->getFilename().c_str(), File->getFilename().GetLength()))
		{
			// Font info has to remain in memory so we do not delete the file once it is loaded
			if (File->isFileAvailable() && File->getError() == CIwGameFile::ErrorNone)
				Font = IwGxFontCreateTTFontFromBuffer((uint8*)File->getContent(), File->getContentLength(), PointSize);
		}
		else
		{
			uint8* data = new uint8 [File->getFileSize()];
			File->Read(data, File->getFileSize());
			Font = IwGxFontCreateTTFontFromBuffer(data, File->getFileSize(), PointSize);
			delete data;
		}
		Delete = true;
		State = State_Loaded;
	}
}



