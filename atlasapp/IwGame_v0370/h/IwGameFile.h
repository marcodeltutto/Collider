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

#if !defined(_IW_GAME_FILE_H_)
#define _IW_GAME_FILE_H_


// TODO: 
// - Add none blocking file I/O
// - Add timeout check to blocking http download
// - Add methods for reading and writing common types

#include "s3eFile.h"
#include "IwGameString.h"
#include "IwGameUtil.h"
#include "IwGameHttp.h"
#include "IwGameDataIO.h"
#include "IwGameXOML.h"


struct CIwGameFilePathComponents
{
	CIwGameString Folder;
	CIwGameString Filename;
	CIwGameString Extension;
};

//
//
// CIwGameFile - CIwGameFile respresents a Marmalade file
// 
//
class CIwGameFile
{
public:
	enum eiwGameFileError
	{
		ErrorNone, 
		ErrorInvalidPath, 
		ErrorOpenFailed, 
		ErrorAlreadyOpen, 
		ErrorEmptyMode, 
		ErrorHttp, 
	};
protected:
	// Properties
	s3eFile*			File;								// Marmalade file handle
	CIwGameString		Name;								// Name
	CIwGameString		Filename;							// filename
	bool				FileAvailable;						// True when ad is available
	CIwGameCallback		FileAvailableCallback;				// Callback to be called when file is available
	void*				FileAvailableCallbackData;			// Callback data to be passed back with callback
	eiwGameFileError	Error;								// Comntains error code if any if file not received
public:
	s3eFile*			getFileHandle()						{ return File; }
	void				setName(const char* name)			{ Name.setString(name); }
	CIwGameString&		getName()							{ return Name; }
	void				setFilename(const char* filename)	{ Filename.setString(filename); }
	CIwGameString&		getFilename()						{ return Filename; }
	void				setFileAvailable(bool available)	{ FileAvailable = available; }
	bool				isFileAvailable()					{ return FileAvailable; }
	void				setFileAvailableCallback(CIwGameCallback callback, void *data)	{ FileAvailableCallback = callback; FileAvailableCallbackData = data; }
	int					getFileSize();
	eiwGameFileError	getError() const					{ return Error; }
	void*				getContent();
	int					getContentLength() const;
	// Properties end

protected:
	bool				InMemory;
	CIwGameHttpRequest* Request;
	void				NotifyAvailable();
	bool				Download();										// Download file from an external location

public:
	CIwGameFile() :						File(NULL), Error(ErrorNone), Request(NULL), FileAvailableCallback(NULL), FileAvailableCallbackData(NULL) 	{	}
	CIwGameFile(const char* filename) : File(NULL), Error(ErrorNone), Request(NULL), FileAvailableCallback(NULL), FileAvailableCallbackData(NULL)
	{
		Filename.setString(filename);
	}
	virtual		~CIwGameFile()
	{
		SAFE_DELETE(Request)
		Close();
	}
	bool				Open(const char* filename = NULL, const char* mode = NULL, bool blocking = false);	// Open file for read or write
	bool				Open(void* memory_buffer, int memory_buffer_len);		// Open file for read or write from a memory buffer
	bool				Invalid();												// Releases file

	bool				Read(void* buffer, int len);
	bool				Write(void* buffer, int len);
	bool				Seek(int offset, s3eFileSeekOrigin origin);
	void				Close();

	// Utility
	static void			GetComponents(const char* file_path, CIwGameFilePathComponents& components);
	static bool			GetFileType(const char* file_path, CIwGameString& type);
	static bool			isHttp(const char* file_path, int path_len);
	static bool			isFile(const char* file_path, int path_len);
	static bool			FileExists(const char* file_path);

	// Internal
	void				FileReceived(CIwGameHttpRequest* request, int error);	// Called by the http callback internally when the file is received
	
};

//
//
//
//
// CIwGameDataFile - A data file
//
//
//
//
class CIwGameDataFile : public IIwGameXomlResource
{
public:
	enum eCIwGameDataFile_State
	{
		State_Invalid, 
		State_Loaded, 
	};

	enum eCIwGameDataFile_Converter
	{
		Converter_None, 
		Converter_FromHTML, 
		Converter_FromHex, 
		Converter_FromURLEncoded, 
	};

	// Proprties
protected:
	eCIwGameDataFile_State	State;				// State of file
	CIwGameString			Filename;			// Name of file
	CIwGameString			FileType;			// Type of file (has not affect on how file is loaded)
	CIwGameXomlVariable*	TargetVariable;		// The file will be loaded into this variable if it is set
	CIwGameDataInput*		DataInput;			// Input stream
	eCIwGameDataFile_Converter Converter;		// Conversion method used on input data
public:
	eCIwGameDataFile_State	getState() const					{ return State; }
	CIwGameString&			getFilename()						{ return Filename; }
	CIwGameDataInput*		getDataInput()						{ if (Load()) return DataInput; return NULL; }
	void					setFileType(const char* type)		{ FileType = type; }
	CIwGameString&			getFileType()						{ return FileType; }
	void					setTargetVariable(const char* var);
	CIwGameXomlVariable*	getTargetVariable()					{ return TargetVariable; }
	void					setConverter(eCIwGameDataFile_Converter method)		{ Converter = method; }
	eCIwGameDataFile_Converter getConverter() const			{ return Converter; }
	// Properties end

protected:
	CIwGameFile*			File;				// File object
public:

	CIwGameDataFile() : IIwGameXomlResource(), File(NULL), State(State_Invalid), DataInput(NULL), TargetVariable(NULL), Converter(Converter_None) { setClassType("file"); }
	~CIwGameDataFile();

	bool		Init(const char* filename, bool preload, bool blocking);
	bool		Load(bool blocking = true);						// Force load the file

	// Implementation of IIwGameXomlResource interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	// Internal
	void			FinishLoad();									// Called back when aysnc loading is completed
};
//
//  CIwGameDataFileCreator - Creates an instance of a file object
//
class CIwGameDataFileCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameDataFileCreator()
	{
		setClassName("file");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameDataFile(); }
};


#endif	// _IW_GAME_FILE_H_
