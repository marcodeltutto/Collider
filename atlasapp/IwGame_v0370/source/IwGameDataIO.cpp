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

#include "IwGameDataIO.h"

//
// CIwGameDataInput implementation
//
bool CIwGameDataInput::Init(int nLen)
{
	Data = new char[nLen];
	if (Data == NULL)
	{
		return false;
	}
	Position = 0;
	Length = nLen;
	EndOfFile = false;

	return true;
}

void CIwGameDataInput::Release()
{
	if (Data != NULL && !Managed)
	{
		delete Data;
		Data = NULL;
	}
	EndOfFile = true;
	Length = 0;
	Position = 0;
}

int CIwGameDataInput::Skip(int nNumBytes)
{
	if (EndOfFile)
		return 0;
		
	Position += nNumBytes;
	if (Position < 0 || Position >= Length)
		EndOfFile = true;
		
	return Position;
}

int CIwGameDataInput::SetPos(int nPos)
{
	EndOfFile = false;
	Position = nPos;
	
	if (Position < 0 || Position >= Length)
		EndOfFile = true;

	return Position;	
}

char CIwGameDataInput::GetByte()
{
	if (EndOfFile)
		return 0;

	char data = (char)Data[Position];

	Position++;
	if (Position >= Length)
		EndOfFile = true;
		
	return data;
}

float CIwGameDataInput::GetByteAsFloat()
{
	if (EndOfFile)
		return 0;

	float data = (float)Data[Position];

	Position++;
	if (Position >= Length)
		EndOfFile = true;
		
	return data;
}

char CIwGameDataInput::GetUByte()
{
	if (EndOfFile)
		return 0;

	char data = Data[Position];

	Position++;
	if (Position >= Length)
		EndOfFile = true;
		
	return data;
}

int CIwGameDataInput::GetUByteAsInt()
{
	if (EndOfFile)
		return 0;

	int data = Data[Position];

	Position++;
	if (Position >= Length)
		EndOfFile = true;
		
	return data;
}

float CIwGameDataInput::GetUByteAsFloat()
{
	if (EndOfFile)
		return 0;

	int data = (int)Data[Position];

	Position++;
	if (Position >= Length)
		EndOfFile = true;
		
	return (float)data;
}

int CIwGameDataInput::GetBytes(char *pData, int nCount)
{
	if (EndOfFile)
		return 0;

	int rem = Remaining();

	if (nCount > rem)
	{
		nCount = rem;
		EndOfFile = true;
	}

	memcpy(pData, (char*)&Data[Position], nCount);

	Position += nCount;
	if (Position >= Length)
		EndOfFile = true;

	return nCount;
}

int CIwGameDataInput::GetBytes(short* pData, int nCount)
{
	if (EndOfFile)
		return 0;

	while (nCount != 0)
	{
		*pData++ = Data[Position];

		Position++;
		if (Position >= Length)
		{
			EndOfFile = true;
			break;
		}
		
		nCount--;
	}
		
	return nCount;
}

int CIwGameDataInput::GetBytes(int* pData, int nCount)
{
	if (EndOfFile)
		return 0;

	while (nCount != 0)
	{
		*pData++ = Data[Position];

		Position++;
		if (Position >= Length)
		{
			EndOfFile = true;
			break;
		}
		
		nCount--;
	}
		
	return nCount;
}

int CIwGameDataInput::GetBytes(float* pData, int nCount)
{
	if (EndOfFile)
		return 0;

	while (nCount != 0)
	{
		*pData++ = (float)Data[Position];

		Position++;
		if (Position >= Length)
		{
			EndOfFile = true;
			break;
		}
		
		nCount--;
	}
		
	return nCount;
}

int CIwGameDataInput::GetUBytes(char *pData, int nCount)
{
	if (EndOfFile)
		return 0;

	int rem = Remaining();

	if (nCount > rem)
	{
		nCount = rem;
		EndOfFile = true;
	}

	memcpy(pData, &Data[Position], nCount);

	Position += nCount;
	if (Position >= Length)
		EndOfFile = true;

	return nCount;
}

int CIwGameDataInput::GetUBytes(int* pData, int nCount)
{
	if (EndOfFile)
		return 0;

	while (nCount != 0)
	{
		*pData++ = (int)Data[Position];

		Position++;
		if (Position >= Length)
		{
			EndOfFile = true;
			break;
		}
		
		nCount--;
	}
		
	return nCount;
}

int CIwGameDataInput::GetUBytes(float* pData, int nCount)
{
	if (EndOfFile)
		return 0;

	while (nCount != 0)
	{
		*pData++ = (float)Data[Position];

		Position++;
		if (Position >= Length)
		{
			EndOfFile = true;
			break;
		}
		
		nCount--;
	}
		
	return nCount;
}

int CIwGameDataInput::GetChars(char* pData, int nCount)
{
	if (EndOfFile)
		return 0;

	int i = 0;
	while (nCount != 0)
	{
		*pData++ = (char)Data[Position];

		Position++;
		if (Position >= Length)
		{
			EndOfFile = true;
			break;
		}
		
		nCount--;
	}
		
	return nCount;
}
	
short CIwGameDataInput::GetShort()
{
	if (EndOfFile)
		return 0;
		
	short *pData = (short *)&Data[Position];
	
	Position += 2;
	if (Position >= Length)
		EndOfFile = true;

	return *pData;
}

short CIwGameDataInput::GetShortSwab()
{
	if (EndOfFile)
		return 0;
		
	int nData = (int)Data[Position + 1] << 8;
	nData |= (int)Data[Position + 0];

	Position += 2;
	if (Position >= Length)
		EndOfFile = true;

	return (short)nData;
}

float CIwGameDataInput::GetShortAsFloat()
{
	if (EndOfFile)
		return 0;
		
	short *pData = (short *)&Data[Position];
	
	Position += 2;
	if (Position >= Length)
		EndOfFile = true;

	return (float)*pData;
}

int CIwGameDataInput::GetUShort()
{
	if (EndOfFile)
		return 0;
		
	short *pData = (short *)&Data[Position];
	
	Position += 2;
	if (Position >= Length)
		EndOfFile = true;

	return (int)*pData;
}

float CIwGameDataInput::GetUShortAsFloat()
{
	if (EndOfFile)
		return 0;
		
	short *pData = (short *)&Data[Position];
	
	Position += 2;
	if (Position >= Length)
		EndOfFile = true;

	return (float)*pData;
}

int CIwGameDataInput::GetShorts(short* pData, int nCount)
{
	if (EndOfFile)
		return 0;

	short *pSData = (short *)&Data[Position];
		
	while (nCount != 0)
	{
		*pData++ = *pSData++;
		
		Position += 2;
		if (Position >= Length)
		{
			EndOfFile = true;
			break;
		}
		
		nCount--;
	}

	return nCount;
}

int CIwGameDataInput::GetShorts(int* pData, int nCount)
{
	if (EndOfFile)
		return 0;

	int *pIData = (int *)&Data[Position];
		
	while (nCount != 0)
	{
		*pData++ = *pIData++;
		
		Position += 4;
		if (Position >= Length)
		{
			EndOfFile = true;
			break;
		}
		
		nCount--;
	}

	return nCount;
}

int CIwGameDataInput::GetShorts(float* pData, int nCount)
{
	if (EndOfFile)
		return 0;

	float *pFData = (float *)&Data[Position];
		
	while (nCount != 0)
	{
		*pData++ = *pFData++;
		
		Position += 4;
		if (Position >= Length)
		{
			EndOfFile = true;
			break;
		}
		
		nCount--;
	}

	return nCount;
}

int CIwGameDataInput::GetUShorts(short* pData, int nCount)
{
	if (EndOfFile)
		return 0;

	short *pSData = (short *)&Data[Position];
		
	while (nCount != 0)
	{
		*pData++ = *pSData++;
		
		Position += 2;
		if (Position >= Length)
		{
			EndOfFile = true;
			break;
		}
		
		nCount--;
	}

	return nCount;
}

int CIwGameDataInput::GetUShorts(int* pData, int nCount)
{
	if (EndOfFile)
		return 0;

	int *pIData = (int *)&Data[Position];
		
	while (nCount != 0)
	{
		*pData++ = *pIData++;
		
		Position += 4;
		if (Position >= Length)
		{
			EndOfFile = true;
			break;
		}
		
		nCount--;
	}

	return nCount;
}

int CIwGameDataInput::GetUShorts(float* pData, int nCount)
{
	if (EndOfFile)
		return 0;

	float *pFData = (float *)&Data[Position];
		
	while (nCount != 0)
	{
		*pData++ = *pFData++;
		
		Position += 4;
		if (Position >= Length)
		{
			EndOfFile = true;
			break;
		}
		
		nCount--;
	}

	return nCount;
}

int CIwGameDataInput::GetInt()
{
	if (EndOfFile)
		return 0;
		
	int *pData = (int *)&Data[Position];

	Position += 4;
	if (Position >= Length)
		EndOfFile = true;

	return *pData;
}

int CIwGameDataInput::GetIntSwab()
{
	if (EndOfFile)
		return 0;
		
	int nData = (int)Data[Position + 3] << 24;
	nData |= (int)Data[Position + 2] << 16;
	nData |= (int)Data[Position + 1] << 8;
	nData |= (int)Data[Position + 0];

	Position += 4;
	if (Position >= Length)
		EndOfFile = true;

	return nData;
}

float CIwGameDataInput::GetFloat()
{
	float data;

	GetUBytes((char*)&data, sizeof(data));

	return data;
}

int CIwGameDataInput::GetInts(int* pData, int nCount)
{
	if (EndOfFile)
		return 0;

	int *pIData = (int *)&Data[Position];
		
	while (nCount != 0)
	{
		*pData++ = *pIData++;
		
		Position += 4;
		if (Position >= Length)
		{
			EndOfFile = true;
			break;
		}
		
		nCount--;
	}

	return nCount;
}

int CIwGameDataInput::GetInts(float* pData, int nCount)
{
	if (EndOfFile)
		return 0;

	int *pFData = (int *)&Data[Position];
		
	while (nCount != 0)
	{
		*pData++ = (float)*pFData++;
		
		Position += 4;
		if (Position >= Length)
		{
			EndOfFile = true;
			break;
		}
		
		nCount--;
	}

	return nCount;
}

int CIwGameDataInput::GetNextQuotedStringAsint(int *pNum)
{
	CIwGameString num;

	if (GetNextQuotedString(&num))
	{
		*pNum = num.GetAsInt();
		return 1;
	}

	return 0;
}

bool CIwGameDataInput::GetNextQuotedString(CIwGameString *pString)
{
	char buffer[2048];
	if (GetNextQuotedString(buffer, 2048))
	{
		pString->Copy(buffer);
		return true;
	}

	return false;
}

bool CIwGameDataInput::GetNextQuotedString(char *pString, int max_len)
{
	if (EndOfFile)
		return false;

	// Search for quote
	char*	pData = (char *)&Data[Position];
	int		quote_count = 0;
	int		index = 0;

	do
	{
		char c = *pData++;

//		if (c == 0 || c == '\n')
		if (c == 0)
			return false;	// Error if we encounter null or newline

		if (c == '"')
		{
			quote_count++;
		}
		else
		{
			if (quote_count == 1)
				pString[index++] = c;

		}
		Position++;
		if (Position >= Length)
		{
			EndOfFile = true;
			return false;
		}
	}
	while (quote_count != 2 && index < max_len);

	if (quote_count != 2)
		return false;
	pString[index] = 0;

	return true;
}

int CIwGameDataInput::GetNextString(CIwGameString *pString)
{
	char buffer[2048];
	if (GetNextString(buffer, 2048))
	{
		pString->Copy(buffer);
		return 0;
	}
	
	return 0;
}

int CIwGameDataInput::GetNextString(char *pString, int max_len)
{
	if (EndOfFile)
		return false;

	char*	pData = (char *)&Data[Position];
	int		index = 0;

	do
	{
		char c = *pData++;

		Position++;
		if (Position >= Length)
			EndOfFile = true;
		
		if (c == 0 || c == '\n' || c == ' ' || c == '\t')
			break;
			
		pString[index++] = c;

		if (EndOfFile)
			return 0;
	}
	while (index < max_len);
	pString[index] = 0;

	return index;
}

bool CIwGameDataInput::GetNextOccuranceOf(char find)
{
	if (EndOfFile)
		return false;

	char*	pData = (char *)&Data[Position];

	do
	{
		char c = *pData++;
		
		if (c == find)
			return true;

		if (c == 0)
			return false;

		Position++;
		if (Position >= Length)
		{
			EndOfFile = true;
			return false;
		}
	}
	while (1);

	return false;
}

int CIwGameDataInput::FindString(char* pString, int str_len) const
{
	char*	pData = (char *)&Data[Position];
	int		char_index = 0;
	int		pos = Position;

	char c;
	do
	{
		c = *pData++;

		if (c == *(pString + char_index))
		{
			char_index++;
			if (char_index >= str_len)
				return pos - str_len + 1;
		}
		else
			char_index = 0;

		pos++;
	}
	while (pos < Length && c != 0);

	return -1;
}

int CIwGameDataInput::GetNextTag(char tag_start_char, char tag_end_char, int range, int &start_pos)
{
	if (EndOfFile)
		return -1;

	char*	pData = (char *)&Data[Position];
	int		index = 0;
	bool	found_first = false;
	int		num_open = 0;

	while (range-- > 0)
	{
		char c = *pData++;

		Position++;
		if (Position >= Length)
			EndOfFile = true;
			
		if (c == 0)
			return -1;

		if (!found_first)
		{
			if (c == tag_start_char)
			{
				found_first = true;
				start_pos = Position;
			}
		}
		else
		{
			if (c == tag_end_char)
				return index;
			index++;
		}
		if (c == tag_start_char)
		{
			num_open++;
			if (num_open > 1)
				return -1;
		}

		if (EndOfFile)
			return -1;
	}

	return -1;
}

int CIwGameDataInput::GetNextTagOrValue(char tag_start_char, char tag_end_char, int range, int &start_pos)
{
	if (Position >= Length)
	{
		EndOfFile = true;
		return -1;
	}
	if (EndOfFile)
		return -1;

	char*	pData = (char *)&Data[Position];
	int		index = 0;
	bool	found_first = false;
	int		num_open = 0;
	bool	tag_found = false;

	while (range-- > 0)
	{
		char c = *pData++;

		if (c == 0)
			return -1;

		if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
		{
			if (!found_first)
			{
				found_first = true;
				start_pos = Position;
				if (c == tag_start_char)
					tag_found = true;
			}
			else
			{
				if (c == tag_end_char)
				{
					if (tag_found)
					{
						Position++;
						return index + 1;
					}
				}
				else
				if (c == tag_start_char)
				{
					return index;
				}
			}
			if (c == tag_start_char)
			{
				num_open++;
				if (num_open > 1)
					return -1;
			}
		}
		if (found_first)
			index++;


		Position++;
		if (Position >= Length)
		{
			EndOfFile = true;
			return -1;
		}
	}

	return -1;
}

int CIwGameDataInput::SkipToWhitespaceInTag()
{
	if (EndOfFile)
		return -1;

	char*	pData = (char *)&Data[Position];

	do
	{
		char c = *pData++;

		if (c == ' ' || c == '\t' || c == '=' || c == '\n')
			return Position;
//		if (c == 0 || c == '/' || c == '>')
		if (c == 0 || c == '>')
			return -1;

		Position++;
		if (Position >= Length)
		{
			EndOfFile = true;
			return -1;
		}

	}
	while (1);

	return -1;
}

int CIwGameDataInput::SkipToNoneWhitespaceInTag()
{
	if (EndOfFile)
		return -1;

	char*	pData = (char *)&Data[Position];

	do
	{
		char c = *pData++;

		if (c == 0)
			return -1;

		if (c != ' ' && c != '\t' && c != '\n')
			return Position;

		Position++;
		if (Position >= Length)
		{
			EndOfFile = true;
			return -1;
		}

	}
	while (1);

	return -1;
}

int CIwGameDataInput::SkipToCharInTag(char chr)
{
	if (EndOfFile)
		return -1;

	char*	pData = (char *)&Data[Position];

	do
	{
		char c = *pData++;

//		if (c == 0 || c == '/' || c == '>')
		if (c == 0 || c == '>')
			return -1;

		if (c == chr)
			return Position;

		Position++;
		if (Position >= Length)
		{
			EndOfFile = true;
			return -1;
		}

	}
	while (1);

	return -1;
}

int	CIwGameDataInput::CalculateCheckSum()
{
	int checksum = 0;
	for (int t = 0; t < Position; t++)
	{
		checksum += Data[t];
	}
	
	return checksum;
}

int	CIwGameDataInput::GetLineNumber(int pos) const
{
	char* pData = Data;
	int line = 0;
	
	while (pos-- != 0)
	{
		if (*pData++ == '\n')
			line++;
	}
	
	return line + 1;
}

//
// CIwGameDataOutput implementation
//
bool CIwGameDataOutput::Init(int nLen)
{
	Data = new char[nLen];
	if (Data == NULL)
	{
		return false;
	}
	memset(Data, 0, nLen);
	Position = 0;
	Length = nLen;
	EndOfFile = false;

	return true;
}

void CIwGameDataOutput::Release()
{
	if (Data != NULL && !Managed)
	{
		delete Data;
		Data = NULL;
	}
	EndOfFile = true;
	Length = 0;
	Position = 0;
}

int CIwGameDataOutput::Skip(int nNumBytes)
{
	if (EndOfFile)
		return 0;
		
	Position += nNumBytes;
	if (Position < 0 || Position >= Length)
		EndOfFile = true;
		
	return Position;
}

int CIwGameDataOutput::SetPos(int nPos)
{
	EndOfFile = false;
	Position = nPos;
	
	if (Position < 0 || Position >= Length)
		EndOfFile = true;

	return Position;	
}

void CIwGameDataOutput::Set(char data)
{
	Position++;
	if (Position >= Length)
		EndOfFile = true;
		
	if (EndOfFile)
	{
		Position--;
		return;
	}

	Data[Position] = data;
}

void CIwGameDataOutput::Set(short data)
{
	Position += 2;
	if (Position >= Length)
		EndOfFile = true;
		
	if (EndOfFile)
	{
		Position -= 2;
		return;
	}

	short *pData = (short *)&Data[Position - 2];
	*pData = data;
}

void CIwGameDataOutput::Set(int data)
{
	Position += 4;
	if (Position >= Length)
		EndOfFile = true;
		
	if (EndOfFile)
	{
		Position -= 4;
		return;
	}

	int *pData = (int *)&Data[Position - 4];
	*pData = data;
}

void CIwGameDataOutput::Set(float data)
{
	Position += 4;
	if (Position >= Length)
		EndOfFile = true;
		
	if (EndOfFile)
	{
		Position -= 4;
		return;
	}

	float *pData = (float *)&Data[Position - 4];
	*pData = data;
}

void CIwGameDataOutput::Set(CIwGameString *pData)
{
	int len = pData->GetLength();

	Position += len;
	if (Position >= Length)
		EndOfFile = true;
		
	if (EndOfFile)
	{
		Position -= len;
		return;
	}

	memcpy((char *)&Data[Position - len], (char *)pData->c_str(), len);
}

void CIwGameDataOutput::Set(char* pData, int nCount)
{
	Position += nCount;
	if (Position >= Length)
		EndOfFile = true;
		
	if (EndOfFile)
	{
		Position -= nCount;
		return;
	}

	char *pDData = (char *)&Data[Position - nCount];
	for (int t = 0; t < nCount; t++)
	{
		*pDData++ = *pData++;
	}
}

void CIwGameDataOutput::Set(short* pData, int nCount)
{
	Position += nCount << 1;
	if (Position >= Length)
		EndOfFile = true;
		
	if (EndOfFile)
	{
		Position -= nCount << 1;
		return;
	}

	short *pDData = (short *)&Data[Position - (nCount << 1)];
	for (int t = 0; t < nCount; t++)
	{
		*pDData++ = *pData++;
	}
}

void CIwGameDataOutput::Set(int* pData, int nCount)
{
	Position += nCount << 2;
	if (Position >= Length)
		EndOfFile = true;
		
	if (EndOfFile)
	{
		Position -= nCount << 2;
		return;
	}

	int *pDData = (int *)&Data[Position - (nCount << 2)];
	for (int t = 0; t < nCount; t++)
	{
		*pDData++ = *pData++;
	}
}

void CIwGameDataOutput::Set(float* pData, int nCount)
{
	Position += nCount << 2;
	if (Position >= Length)
		EndOfFile = true;
		
	if (EndOfFile)
	{
		Position -= nCount << 2;
		return;
	}

	float *pDData = (float *)&Data[Position - (nCount << 2)];
	for (int t = 0; t < nCount; t++)
	{
		*pDData++ = *pData++;
	}
}

int	CIwGameDataOutput::CalculateCheckSum()
{
	int checksum = 0;
	for (int t = 0; t < Position; t++)
	{
		checksum += Data[t];
	}
	
	return checksum;
}

int CIwGameDataInput::StripXMLComments()
{
	char* new_data = new char[Length + 1];
	char* data2 = new_data;

	// Copy all text except commeents to new buffer
	char* data = Data;
	int len = Length;

	while (len-- > 0)
	{
		char c = *data++;
		bool end_commend = false;

		if (len > 6 && c == '<' && *data == '!' && *(data + 1) == '-' && *(data + 2) == '-')
		{
			data += 3;
			len -= 3; if (len < 0) return -1;
			// Found a comment, now search ahead to the end of the comment
			while (len-- > 0)
			{
				c = *data++;
				if (c == '-')
				{
					len--; if (len < 0) return -1;
					c = *data++;
					if (c == '-')
					{
						len--; if (len < 0) return -1;
						c = *data++;
						if (c == '>')
						{
							end_commend = true;
							break;
						}
					}
				}
			}
			if (len <= 0)
				break;
		}
		if (!end_commend)
		{
			// Replace special chars
			if (c == '\\')
			{
				if (*data == 'n')
				{
					*data2++ = '\n';
					data++;
					len--;
				}
				else
					*data2++ = c;
			}
			else
				*data2++ = c;
		}
	}
	*data2 = 0;

	Length = strlen(new_data);
	delete Data;
	Data = new_data;

	return 1;
}

