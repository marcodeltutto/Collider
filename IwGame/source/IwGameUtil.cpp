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

#include "IwGameUtil.h"
#include "IwGameString.h"
#include "IwGameFile.h"

//#define _DEBUG_FILENAME_	"raw:///sdcard/debug.txt"
//#define _DEBUG_FILENAME_	"rst:///IwGame/shared/documents/debug.txt"
#if defined(_MSC_VER)
#define _DEBUG_FILENAME_	"raw://debug.txt"
#endif

//
//
// CIwGameUtils implementation
//
//
struct IwGameGraphicsMode
{
	const char* name;
	int			width;
	int			height;
};
static IwGameGraphicsMode g_CIwGameUtils_GraphicsModeNames[] = 
{
	{ "QQVGA", 160, 120 },		// 0
	{ "HQVGA", 240, 160 },		// 1
	{ "QVGA", 320, 240 },		// 2
	{ "WQVGA", 400, 240 },		// 3
	{ "HVGA", 480, 320 },		// 4
	{ "VGA", 640, 480 },		// 5
	{ "WVGA", 800, 480 }, 		// 6
	{ "FWVGA", 854, 480 }, 		// 7
	{ "SVGA", 800, 600 }, 		// 8
	{ "DVGA", 960, 640 }, 		// 9
	{ "WSVGA", 1024, 576 }, 	// 10
	{ "WSVGA", 1024, 600 }, 	// 11
	{ "XVGA", 1024, 768 }, 		// 12
	{ "WXVGA", 1280, 768 }, 	// 13
	{ "XVGA+", 1152, 864 }, 	// 14
	{ "WXVGA+", 1140, 900 }, 	// 15
	{ "SXVGA", 1280, 1024 }, 	// 16
	{ "SXVGA+", 1400, 1050 }, 	// 17
	{ "WSXVGA+", 1680, 1050 }, 	// 18
	{ "UXVGA", 1600, 1200 }, 	// 19
	{ "WUXVGA", 1920 , 1200 }, 	// 20
	{ "QWXGA", 2048, 1152 }, 	// 21
	{ "QXGA", 2048, 1536 }, 	// 22
	{ "WQXGA", 2560, 1600 }, 	// 23
	{ "QSXGA", 2560, 2048 }, 	// 24
	{ "WQSXGA", 3200, 2048 }, 	// 25
	{ "QUXGA", 3200, 2400 }, 	// 26
	{ "WQUXGA", 3840, 2400 }, 	// 27
	{ "HXGA", 4096, 3072 }, 	// 28
	{ "WHXGA", 5120, 3200 }, 	// 29
	{ "HSXGA", 5120, 4096 }, 	// 30
	{ "WHSXGA", 6400, 4096 }, 	// 31
	{ "HUXGA", 6400, 4800 }, 	// 32
	{ "WHUXGA", 7680, 4800 }, 	// 33
	{ "nHD", 640, 360 }, 		// 34
	{ "qHD", 960, 540 }, 		// 35
	{ "WQHD", 2560, 1440 }, 	// 36
	{ "QFHD", 3840, 2160 }, 	// 37
};

int g_ShowTracing = 2;

const char* CIwGameUtils::GetGraphicModeName(int width, int height)
{
	for (int t = 0; t < 37; t++)
	{
		int w = g_CIwGameUtils_GraphicsModeNames[t].width;
		int h = g_CIwGameUtils_GraphicsModeNames[t].height;
		if ((width == w && height == h) || (width == h && height == w))
			return g_CIwGameUtils_GraphicsModeNames[t].name;
	}

	return NULL;
}

int CIwGameUtils::GetGraphicModeIndex(int width, int height)
{
	for (int t = 0; t < 37; t++)
	{
		int w = g_CIwGameUtils_GraphicsModeNames[t].width;
		int h = g_CIwGameUtils_GraphicsModeNames[t].height;
		if ((width == w && height == h) || (width == h && height == w))
			return t;
	}

	return -1;
}

int CIwGameUtils::CountDigits(int number)
{
	int count = 0;
	if (number > 0)
	{
		if (number >= 1000000000)
			return 10;
		else
		if (number >= 100000000)
			return 9;
		else
		if (number >= 10000000)
			return 8;
		else
		if (number >= 1000000)
			return 7;
		else
		if (number >= 100000)
			return 6;
		else
		if (number >= 10000)
			return 5;
		else
		if (number >= 1000)
			return 4;
		else
		if (number >= 100)
			return 3;
		else
		if (number >= 10)
			return 2;
		else
			return 1;
	}
	else
	{
		if (number <= -1000000000)
			return 10;
		else
		if (number <= -100000000)
			return 9;
		else
		if (number <= -10000000)
			return 8;
		else
		if (number <= -1000000)
			return 7;
		else
		if (number <= -100000)
			return 6;
		else
		if (number <= -10000)
			return 5;
		else
		if (number <= -1000)
			return 4;
		else
		if (number <= -100)
			return 3;
		else
		if (number <= -10)
			return 2;
		else
			return 1;
	}
}

CIwGameUtils::eDeviceType CIwGameUtils::GetDeviceType()
{
	// Get the devices operating system
	int width = PLATFORM_DISPLAY->getCurrentWidth();
	int height = PLATFORM_DISPLAY->getCurrentHeight();
	int os = s3eDeviceGetInt(S3E_DEVICE_OS);

	// Calculate portal type from OS
	switch (os)
	{
	case S3E_OS_ID_SYMBIAN:
		return DeviceType_Symbian;
	case S3E_OS_ID_WINMOBILE:
	case S3E_OS_ID_WINCE:
		return DeviceType_WinMobile;
	case S3E_OS_ID_QNX:
		return DeviceType_QNX;
	case S3E_OS_ID_BADA:
		return DeviceType_Bada;
	case S3E_OS_ID_ANDROID:
		return DeviceType_Android;
	case S3E_OS_ID_IPHONE:
		if (width > 960 && height > 640 || height > 960 && width > 640)
			return DeviceType_iPad;
		else
			return DeviceType_iPhone;
	case S3E_OS_ID_WEBOS:
		return DeviceType_WebOS;
	case S3E_OS_ID_WINDOWS:
		return DeviceType_Windows;
	case S3E_OS_ID_OSX:
		return DeviceType_OSX;
	}

	return DeviceType_Unsupported;
}


//
//
// CError implementation
//
//
void CIwGameError::Init()
{
	if (g_ShowTracing == 0)
		return;
#if defined(_DEBUG_FILENAME_)
	CIwGameString s = "AppEasy Debug Output";
	CxFile file = PLATFORM_FILE->Open(_DEBUG_FILENAME_, "wb");
	if (file != NULL)
	{
		PLATFORM_FILE->Write(file, (void*)s.c_str(), s.GetLength());
		PLATFORM_FILE->Close(file);
	}
#endif	// _DEBUG_FILENAME_
}
void CIwGameError::LogError(const char* message)
{
	if (g_ShowTracing == 0)
		return;

	if (message == NULL)
		return;

	s3eDebugOutputString(message);

#if defined(_DEBUG_FILENAME_)
	CxFile file = PLATFORM_FILE->Open(_DEBUG_FILENAME_, "ab");
	if (file != NULL)
	{
		PLATFORM_FILE->Write(file, (void*)message, strlen(message));
		PLATFORM_FILE->Write(file, (void*)"\r\n", 2);
		PLATFORM_FILE->Close(file);
	}
#endif	// _DEBUG_FILENAME_
}
void CIwGameError::LogError(const char* message, const char* data)
{
	if (g_ShowTracing == 0)
		return;

	if (message == NULL || data == NULL)
		return;

	CIwGameString str;

	str.setString(message);
	str += (char*)data;

	s3eDebugOutputString(str.c_str());

#if defined(_DEBUG_FILENAME_)
	CxFile file = PLATFORM_FILE->Open(_DEBUG_FILENAME_, "ab");
	if (file != NULL)
	{
		PLATFORM_FILE->Write(file, (void*)str.c_str(), str.GetLength());
		PLATFORM_FILE->Write(file, (void*)"\r\n", 2);
		PLATFORM_FILE->Close(file);
	}
#endif	// _DEBUG_FILENAME_
}