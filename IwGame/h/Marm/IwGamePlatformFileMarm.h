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

#if !defined(_CIW_GAME_PLATFORM_FILE_MARM_H_)
#define _CIW_GAME_PLATFORM_FILE_MARM_H_

#include "IwGamePlatformMarm.h"

class CIwGamePlatformFileMarm : public IIwGamePlatformFile
{
	// Properties
protected:
public:
	// Properties end
public:
	int		Init();
	void	Release();

	CxFile	Open(const char* filename, const char* mode);
	CxFile	OpenFromMemory(void* memory, uint num_bytes);
	void	Close(CxFile file);
	bool	Seek(CxFile file, int offset, CxFileSeekOrigin origin);
	uint	Read(CxFile file, void* buffer, uint num_bytes);
	uint	Write(CxFile file, void* buffer, uint num_bytes);
	bool	Exists(const char* filename);
	bool	Delete(const char* filename);
	int		getSize(CxFile file);
	const char*	getErrorString();
};


#endif	// _CIW_GAME_PLATFORM_FILE_MARM_H_
