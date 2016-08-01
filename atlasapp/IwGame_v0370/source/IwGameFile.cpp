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

#include "IwGameFile.h"
#include "IwGameUtil.h"
#include "IwGameActor.h"
#include <iostream>

//
//
//
//
// CIwGameFile implementation
//
//
//
//
int	CIwGameFile::getFileSize()
{
	if (File == NULL)
		return -1;

	return s3eFileGetSize(File);
}

bool CIwGameFile::Read(void* buffer, int len)
{
	if (File == NULL)
		return false;

	if (s3eFileRead(buffer, len, 1, File) != 1)
	{
#if defined(_DEBUG)
		s3eFileGetError();
		CIwGameError::LogError("Error: CIwGameFile::Read(): ", s3eFileGetErrorString());
#endif	// _DEBUG
		Close();
		return false;
	}

	return true;
}

bool CIwGameFile::Write(void* buffer, int len)
{
	if (File == NULL)
		return false;

	if (s3eFileWrite(buffer, len, 1, File) != 1)
	{
#if defined(_DEBUG)
		s3eFileGetError();
		CIwGameError::LogError("Error: CIwGameFile::Write(): ", s3eFileGetErrorString());
		Close();
#endif	// _DEBUG
		return false;
	}

	return true;
}

bool CIwGameFile::Open(const char* path, const char* mode, bool blocking)
{
	FileAvailable = false;
	InMemory = false;

	// make sure user cannot open the file twice
	if (File != NULL)
	{
		Error = ErrorAlreadyOpen;
		return false;
	}

	// Get the path
	if (path != NULL)
		Filename.setString(path);
	else
		path = Filename.c_str();
	
	// Check we actually have a path
	if (path == NULL)
	{
		Error = ErrorInvalidPath;
		return false;
	}

	// Check to see if the file is located on the web
	if (isHttp(Filename.c_str(), Filename.GetLength()))
	{
		Download();
		std::cout <<"!!!!!  CIwGameFile::Open(), FileName = "<<Filename.c_str()<<" finished Download(), FileAvailable = "<<FileAvailable<<"blocking = "<<blocking<<"\n";

		if (blocking)
		{
			while (!FileAvailable)
			{
				IW_GAME_HTTP_MANAGER->Update();
				s3eDeviceYield(0);
			}
			if (Error != ErrorNone)
				return false;
		}
		std::cout <<"!!!!!  CIwGameFile::Open(), about to return true !!!!!\n";
		return true;
	}
	else
	{
		if (mode == NULL)
		{
			Error = ErrorEmptyMode;
			return false;
		}
	}

	// Open the file
	File = s3eFileOpen(Filename.c_str(), mode);
	if (File == NULL)
	{
#if defined(_DEBUG)
		s3eFileGetError();
		CIwGameError::LogError("Error: CIwGameFile::Open(): ", s3eFileGetErrorString());
#endif	// _DEBUG
		Error = ErrorOpenFailed;
		return false;
	}
	NotifyAvailable();

	FileAvailable = true;

	return true;
}

bool CIwGameFile::Open(void* memory_buffer, int memory_buffer_len)
{
	FileAvailable = false;
	InMemory = true;

	File = s3eFileOpenFromMemory(memory_buffer, memory_buffer_len);
	if (File == NULL)
	{
#if defined(_DEBUG)
		s3eFileGetError();
		CIwGameError::LogError("Error: CIwGameFile::Open(memory): ", s3eFileGetErrorString());
#endif	// _DEBUG
		Error = ErrorOpenFailed;
		return false;
	}

	FileAvailable = true;

	return true;
}

void CIwGameFile::Close()
{
	if (File != NULL)
	{
		s3eFileClose(File);
		File = NULL;
	}
}

bool CIwGameFile::Seek(int offset, s3eFileSeekOrigin origin)
{
	if (File == NULL)
		return false;

	if (s3eFileSeek(File, offset, origin) != S3E_RESULT_SUCCESS)
		return false;

	return true;
}

void* CIwGameFile::getContent()
{
	if (!InMemory || Request == NULL)
		return NULL;
	if (FileAvailable && Request->getError() == 0)
		return (void*)Request->getContent().c_str();

	return NULL;
}

int CIwGameFile::getContentLength() const
{
	if (!InMemory || Request == NULL)
		return -1;
	if (FileAvailable && Request->getError() == 0)
		return Request->getContentLength();

	return -1;
}

int32 FileRetrievedCallback(void* caller, void* data)
{
	CIwGameHttpRequest* request = (CIwGameHttpRequest*)caller;

	if (request->getProcessed())									// Check to see if our request was processed by the http manager
	{
		((CIwGameFile*)data)->FileReceived(request, request->getError());
		IW_GAME_HTTP_MANAGER->RemoveRequest(request);				// Remove request from http manager queue
	}

	return 0;
}

bool CIwGameFile::Download()
{
	FileAvailable = false;

	// Build download request
	if (Request == NULL)
		Request = new CIwGameHttpRequest();
	Request->setGET();
	Request->setURI(Filename.c_str());
	Request->setContentAvailableCallback(&FileRetrievedCallback, (void*)this);
	IW_GAME_HTTP_MANAGER->AddRequest(Request);

#if defined(_DEBUG)
	CIwGameError::LogError("CIwGameFile::Download with URL:", Filename.c_str());
#endif	// _DEBUG

	return false;
}

void CIwGameFile::FileReceived(CIwGameHttpRequest* request, int error)
{
	// If there wwas an error then set the error
	if (error != 0)
	{
		Error = ErrorHttp;
#if defined(_DEBUG)
		CIwGameError::LogError("Error: CIwGameFile::FileReceived(): ", CIwGameString(error).c_str());
#endif	// _DEBUG
	}

	// Open the file
	Open((void*)request->getContent().c_str(), request->getContentLength());

	// Notify caller that the file is available
	NotifyAvailable();
}

void CIwGameFile::NotifyAvailable()
{
	// Call any user supplied callback
	if (FileAvailableCallback != NULL)
		FileAvailableCallback((void*)this, FileAvailableCallbackData);

	FileAvailable = true;
}

//
//
//
// Utility methods
//
//
//

void CIwGameFile::GetComponents(const char* file_path, CIwGameFilePathComponents& components)
{
	int							len = strlen(file_path) - 1;
	const char*					name_ptr = file_path + len;

	// Scan backwards looking for dot
	int index = 0;
	while (len >= 0)
	{
		if (*name_ptr == '.')
		{
			components.Extension.setString(name_ptr + 1, index);
			name_ptr--;
			len--;
			break;
		}
		else
		if (len == 0)
		{
			components.Extension.setString(name_ptr, index + 1);
			name_ptr--;
			len--;
			break;
		}
		name_ptr--;
		index++;
		len--;
	}
	if (len < 0)
		return;

	// Continue scanning for filename
	index = 0;
	while (len >= 0)
	{
		if (*name_ptr == '/' || *name_ptr == '\\')
		{
			components.Filename.setString(name_ptr + 1, index);
			name_ptr--;
			len--;
			break;
		}
		else
		if (len == 0)
		{
			components.Filename.setString(name_ptr, index + 1);
			name_ptr--;
			len--;
			break;
		}

		name_ptr--;
		index++;
		len--;
	}
	if (len < 0)
		return;

	len = (name_ptr - file_path) + 1;
	if (len > 0)
		components.Folder.setString(file_path, len);
}

bool CIwGameFile::GetFileType(const char* file_path, CIwGameString& type)
{
	int						len = strlen(file_path) - 1;
	const char*				name_ptr = file_path + len;

	// Scan backwards looking for dot
	int index = 0;
	while (len >= 0)
	{
		if (*name_ptr == '.')
		{
			type.setString(name_ptr + 1, index);
			type.ToLower();
			break;
		}
		else
		if (len == 0)
		{
			type.setString(name_ptr, index + 1);
			type.ToLower();
			break;
		}
		name_ptr--;
		index++;
		len--;
	}
	
	return true;
}

bool CIwGameFile::isHttp(const char* file_path, int path_len)
{
	// Ignore very short path names because they cannot be a URI
	if (path_len < 10)
		return false;

	int check = *((int*)file_path);
	if (check == (((int)'h') | ((int)'t' << 8) | ((int)'t' << 16) | ((int)'p' << 24)) || check == (((int)'H') | ((int)'T' << 8) | ((int)'T' << 16) | ((int)'P' << 24)))
	{
		if (*(file_path + 4) == ':' || *(file_path + 5) == ':')
			return true;
	}

	return false;
}

bool CIwGameFile::isFile(const char* file_path, int path_len)
{
	// Ignore very short path names because they cannot be a URI
	if (path_len < 3)
		return false;

	for (int t = 0; t < path_len; t++)
	{
		if (*file_path++ == '.')
			return true;
	}

	return false;
}

bool CIwGameFile::FileExists(const char* file_path)
{
	return s3eFileCheckExists(file_path) == S3E_TRUE;
}


//
//
//
//
// CIwGameDataFile implementation
//
//
//
//
CIwGameDataFile::~CIwGameDataFile()
{
	SAFE_DELETE(File)
	SAFE_DELETE(DataInput)
}

bool CIwGameDataFile::Init(const char* filename, bool preload, bool blocking)
{
	SAFE_DELETE(DataInput)
	SAFE_DELETE(File)
	State = State_Invalid;

	Filename = filename;
	File = new CIwGameFile(filename);

	if (preload)
		return Load(blocking);

	return true;
}

int32 CIwGameDataFile_FileRetrievedAsyncCallback(void* caller, void* data)
{
	CIwGameDataFile* file = (CIwGameDataFile*)data;
	file->FinishLoad();

	return 0;
}

bool CIwGameDataFile::Load(bool blocking)
{
	if (DataInput != NULL)
		return true;

	if (File != NULL)
	{
		File->setFileAvailableCallback(CIwGameDataFile_FileRetrievedAsyncCallback, this);
		if (File->Open(NULL, "rb", blocking))
			return true;
		else
			return false;
	}

	return false;
}

void CIwGameDataFile::FinishLoad()
{
	if (File != NULL)
	{
		if (CIwGameFile::isHttp(File->getFilename().c_str(), File->getFilename().GetLength()))
		{
			// Store the retrieved data
			if (File->isFileAvailable() && File->getError() == CIwGameFile::ErrorNone)
			{
				if (Converter == Converter_None)
				{
					DataInput = new CIwGameDataInput(true);
					DataInput->setData((char*)File->getContent(), File->getContentLength());
				}
				else
				{
					// Convert the retrieved data
					CIwGameString data;
					data.setString((char*)File->getContent(), File->getContentLength());
					switch (Converter)
					{
					case Converter_FromHTML:
						data.ReplaceHTMLCodes();
						break;
					case Converter_FromHex:
						data.URLDecode();
						break;
					case Converter_FromURLEncoded:
						data.HexDecode();
						break;
					}
					DataInput = new CIwGameDataInput();
					DataInput->Init(File->getContentLength() + 1);
					memcpy(DataInput->getData(), data.c_str(), data.GetLength() + 1);
					SAFE_DELETE(File)
				}
			}
			else
			{
				State = State_Invalid;
				return;
			}
		}
		else
		{
			// Load data
			DataInput = new CIwGameDataInput();
			DataInput->setData(new char [File->getFileSize()], File->getFileSize());
			File->Read(DataInput->getData(), DataInput->GetLength());

			if (Converter != Converter_None)
			{
				// Convert the retrieved data
				CIwGameString data;
				data.setString(DataInput->getData(), DataInput->GetLength());
				switch (Converter)
				{
				case Converter_FromHTML:
					data.ReplaceHTMLCodes();
					break;
				case Converter_FromHex:
					data.URLDecode();
					break;
				case Converter_FromURLEncoded:
					data.HexDecode();
					break;
				}
				DataInput->Release();
				DataInput->Init(data.GetLength() + 1);
				memcpy(DataInput->getData(), data.c_str(), data.GetLength() + 1);
			}

			SAFE_DELETE(File)
		}

		// if target variable is set then write the contents of the file to the variable
		if (TargetVariable != NULL)
		{
			CIwGameString data;
			data.setString(DataInput->getData(), DataInput->GetLength());
			TargetVariable->setValue(data.c_str());
		}

		State = State_Loaded;
	}
}

bool CIwGameDataFile::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Process image attributes
	CIwGameString* name = NULL;
	CIwGameString* location = NULL;
	bool preload = false;
	bool blocking = false;
	CIwGameString* condition = NULL;
	CIwGameString* variable = NULL;

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
		if (name_hash == CIwGameXomlNames::FileType_Hash)
			FileType = (*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Variable_Hash)
			variable = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Converter_Hash)
		{
			unsigned int converter_hash = (*it)->GetValue().getHash();
			if (converter_hash == IW_GAME_HASH("html"))
				Converter = Converter_FromHTML;
			else
			if (converter_hash == IW_GAME_HASH("hex"))
				Converter = Converter_FromHex;
			else
			if (converter_hash == IW_GAME_HASH("urlencoded"))
				Converter = Converter_FromURLEncoded;
		}
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

	// if target variable is set then write the contents of the file to the variable
	if (variable != NULL)
	{
		CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(*variable, scene);
		if (var == NULL)
			CIwGameError::LogError("Warning: File target variable does not exist - ", variable->c_str());
		else
			TargetVariable = var;
	}


	// If we are declared inside a scene then image is local to the scene
	if (scene != NULL)
		scene->getResourceManager()->addResource(this);
	else
		IW_GAME_GLOBAL_RESOURCES->getResourceManager()->addResource(this);

	// Initialise the video
	Init(location->c_str(), preload, blocking);

	return true;
}





