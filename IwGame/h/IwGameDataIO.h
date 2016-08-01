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

#if !defined(_IW_GAME_DATAIO_H_)
#define _IW_GAME_DATAIO_H_

#include "IwGameUtil.h" 
#include "IwGameString.h" 

//
//
// CIwGameDataInput - Provides stream like read access to a memory buffer
//
//
class CIwGameDataInput
{
	// Properties
protected:
	char* 			Data;					// Streams data
	int				Position;				// Current position in stream
	int				Length;					// Length of stream (actual allocated memory sie is aligned to 32 bytes)
	bool			EndOfFile;				// true if end of stream reached
	bool			Managed;				// If true data buffer will not be released when object deleted
public:
	// Properties End
	
private:
public:
	CIwGameDataInput(bool managed) : Data(NULL), Position(0), Length(0), EndOfFile(false), Managed(managed) {}
	CIwGameDataInput() : Data(NULL), Position(0), Length(0), EndOfFile(false), Managed(false) {}
	virtual ~CIwGameDataInput() { Release(); }

	bool	Init(int nLen);
	virtual void	Release();

	char*	getData() { return Data; }
	void	setData(char* data)
	{
		Release();
		Data = data;
		Position = 0;
		EndOfFile = false;
	}
	void	setData(char* data, int length)
	{
		Release();
		Data = data;
		Position = 0;
		Length = length;
		EndOfFile = false;
	}
	int		Skip(int nNumBytes);
	int		SetPos(int nPos);
	int		GetPos() const			{ return Position; }
	int		GetLength() const		{ return Length; }
	bool	IsEOF() const			{ return EndOfFile; }
	int		Remaining() const		{ return Length - Position;	}
	char	GetByte();
	float	GetByteAsFloat();
	char	GetUByte();
	int		GetUByteAsInt();
	float	GetUByteAsFloat();
	int		GetBytes(char* pData, int nCount);
	int		GetBytes(short* pData, int nCount);
	int		GetBytes(int* pData, int nCount);
	int		GetBytes(float* pData, int nCount);
	int		GetUBytes(char* pData, int nCount);
	int		GetUBytes(int* pData, int nCount);
	int		GetUBytes(float* pData, int nCount);
	int		GetChars(char* pData, int nCount);
	short	GetShort();
	short	GetShortSwab();
	float	GetShortAsFloat();
	int		GetUShort();
	float	GetUShortAsFloat();
	int		GetShorts(short* pData, int nCount);
	int		GetShorts(int* pData, int nCount);
	int		GetShorts(float* pData, int nCount);
	int		GetUShorts(short* pData, int nCount);
	int		GetUShorts(int* pData, int nCount);
	int		GetUShorts(float* pData, int nCount);
	int		GetInt();
	int		GetIntSwab();
	int		GetInts(int* pData, int nCount);
	int		GetInts(float* pData, int nCount);
	float	GetFloat();
	int		GetNextString(CIwGameString *pString);
	int		GetNextString(char *pString, int max_len);
	int		GetNextQuotedStringAsint(int *pNum);
	bool	GetNextQuotedString(CIwGameString *pString);
	bool	GetNextQuotedString(char *pString, int max_len);
	bool	GetNextOccuranceOf(char find);
	int		GetNextTag(char tag_start_char, char tag_end_char, int range, int &start_pos);
	int		GetNextTagOrValue(char tag_start_char, char tag_end_char, int range, int &start_pos);
	int		FindString(const char* pString, int str_len) const;

	int		SkipToWhitespaceInTag();
	int		SkipToNoneWhitespaceInTag();
	int		SkipToCharInTag(char chr);
	
	int		GetLineNumber(int pos) const;
	
	int		CalculateCheckSum();
	int		StripXMLComments();
	int		CountXmlEntities(int& tags, int& attributes);
};

//
//
// CIwGameDataOutput - Provides stream write like access to a memory buffer
//
//
class CIwGameDataOutput
{
protected:
	// Properties
	char* 			Data;					// Streams data
	int				Position;				// Current position in stream
	int				Length;					// Length of stream (actual allocated memory sie is aligned to 32 bytes)
	bool			EndOfFile;				// true if end of stream reached
	bool			Managed;				// If true data buffer will not be released when object deleted
public:
	// Properties End
	
private:
public:
	CIwGameDataOutput(bool managed) : Data(NULL), Position(0), Length(0), EndOfFile(false), Managed(managed) {}
	CIwGameDataOutput() :	Data(NULL), Position(0), Length(0), EndOfFile(false), Managed(false) {}
	virtual ~CIwGameDataOutput() { Release(); }

	bool	Init(int nLen);
	virtual void	Release();

	char*	getData() { return Data; }
	void	setData(char* data)
	{
		Release();
		Data = data;
		Position = 0;
		EndOfFile = false;
	}
	void	setData(char* data, int length)
	{
		Release();
		Data = data;
		Position = 0;
		Length = length;
		EndOfFile = false;
	}
	int		Skip(int nNumBytes);
	int		SetPos(int nPos);
	int		GetPos() const { return Position; }
	int		GetLength() const { return Length; }
	bool	IsEOF() const { return EndOfFile; }
	int		Remaining() const { return Length - Position;	}
	void	Set(char data);
	void	Set(short data);
	void	Set(int data);
	void	Set(float data);
	void	Set(char* pData, int nCount);
	void	Set(short* pData, int nCount);
	void	Set(int* pData, int nCount);
	void	Set(float* pData, int nCount);
	void	Set(CIwGameString* pString);
	
	int		CalculateCheckSum();
};


#endif	//_IW_GAME_DATAIO_H_
