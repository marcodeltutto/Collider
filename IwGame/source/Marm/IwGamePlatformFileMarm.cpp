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

#include "Marm/IwGamePlatformFileMarm.h"
#include "s3eFile.h"


//
// Marmalade File system
//
int		CIwGamePlatformFileMarm::Init()
{
	return 1;
}

void	CIwGamePlatformFileMarm::Release()
{
}

CxFile	CIwGamePlatformFileMarm::Open(const char* filename, const char* mode)
{
	return (CxFile*)s3eFileOpen(filename, mode);
}

CxFile	CIwGamePlatformFileMarm::OpenFromMemory(void* memory, uint num_bytes)
{
	return (CxFile*)s3eFileOpenFromMemory(memory, num_bytes);
}

void	CIwGamePlatformFileMarm::Close(CxFile file)
{
	s3eFileClose((s3eFile*)file);
}

bool	CIwGamePlatformFileMarm::Seek(CxFile file, int offset, CxFileSeekOrigin origin)
{
	s3eFileSeekOrigin o = S3E_FILESEEK_CUR;
	if (origin == FileSeek_Set)
		o = S3E_FILESEEK_SET;
	else
	if (origin == FileSeek_End)
		o = S3E_FILESEEK_END;

	return s3eFileSeek((s3eFile*)file, offset, o) == S3E_RESULT_SUCCESS;
}

uint	CIwGamePlatformFileMarm::Read(CxFile file, void* buffer, uint num_bytes)
{
	return s3eFileRead(buffer, num_bytes, 1, (s3eFile*)file);
}

uint	CIwGamePlatformFileMarm::Write(CxFile file, void* buffer, uint num_bytes)
{
	return s3eFileWrite(buffer, num_bytes, 1, (s3eFile*)file);
}

bool	CIwGamePlatformFileMarm::Exists(const char* filename)
{
	return s3eFileCheckExists(filename) == S3E_TRUE;
}

bool	CIwGamePlatformFileMarm::Delete(const char* filename)
{
	return s3eFileDelete(filename) == S3E_RESULT_SUCCESS;
}

int		CIwGamePlatformFileMarm::getSize(CxFile file)
{
	return s3eFileGetSize((s3eFile*)file);
}

const char* CIwGamePlatformFileMarm::getErrorString()
{
	s3eFileGetError();
	return s3eFileGetErrorString();
}




