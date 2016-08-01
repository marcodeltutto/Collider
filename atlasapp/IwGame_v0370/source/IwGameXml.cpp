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

#include "IwGameXml.h"
#include "IwGameString.h" 
#include "IwGameDataIO.h"
#include "IwGameFile.h"

#define SHOW_ERRORS

const char* CIwGameXmlParserErrors[] = 
{
	"",
	"No XML tags found", 
	"XML file could not be read", 
	"Missing end tag", 
	"Missing close comment '-->'", 
	"Mismatched end tag", 
	"Missing '=' sign in attribute definition", 
	"Missing value in attribute definition", 
	"Missing closing quote in attribute value definition", 
	"Invalid tag"
};

// Pooled lists to reduce memory fragmentation and loads of allocations / frees
int			g_IntListPool[16];
int			g_IntListPoolIndex = 0;
float		g_FloatListPool[16];
int			g_FloatListPoolIndex = 0;
bool		g_BoolListPool[16];
int			g_BoolListPoolIndex = 0;

CIwGameXmlNode*			CIwGameXmlParser::NodePool = NULL;
int						CIwGameXmlParser::NextFreePoolNodeIndex = 0;
int						CIwGameXmlParser::MaxPoolNodes = 0;
CIwGameXmlTagMarker*	CIwGameXmlParser::TagPool = NULL;
int						CIwGameXmlParser::NextFreePoolTagIndex = 0;
int						CIwGameXmlParser::MaxPoolTags = 0;
CIwGameXmlAttribute*	CIwGameXmlParser::AttributePool = NULL;
int						CIwGameXmlParser::NextFreePoolAttributeIndex = 0;
int						CIwGameXmlParser::MaxPoolAttributes = 0;

//
//
//
//
// CIwGameXmlAttribute implementation
//
//
//
//
int CIwGameXmlAttribute::GetValueAsInt() const
{
	return Value.GetAsInt();
}

float CIwGameXmlAttribute::GetValueAsFloat() const
{
	return Value.GetAsFloat();
}

bool CIwGameXmlAttribute::GetValueAsBool() const
{
	return Value.GetAsBool();
}

bool CIwGameXmlAttribute::GetValueAsPoint(CIwFVec2& point)
{
	if (Value.GetAsListOfFloat(g_FloatListPool) != 2)
	{
		return false;
	}
	point.x = g_FloatListPool[0];
	point.y = g_FloatListPool[1];

	return true;
}

bool CIwGameXmlAttribute::GetValueAsPoint3(CIwFVec3& point)
{
	if (Value.GetAsListOfFloat(g_FloatListPool) != 3)
	{
		return false;
	}
	point.x = g_FloatListPool[0];
	point.y = g_FloatListPool[1];
	point.z = g_FloatListPool[2];

	return true;
}

bool CIwGameXmlAttribute::GetValueAsPoint4(CIwFVec4& point)
{
	if (Value.GetAsListOfFloat(g_FloatListPool) != 4)
	{
		return false;
	}
	point.x = g_FloatListPool[0];
	point.y = g_FloatListPool[1];
	point.z = g_FloatListPool[2];
	point.w = g_FloatListPool[3];

	return true;
}

bool CIwGameXmlAttribute::GetValueAsColour(CIwColour& colour)
{
	if (Value.GetAsListOfInt(g_IntListPool) != 4)
	{
		return false;
	}
	colour.r = g_IntListPool[0];
	colour.g = g_IntListPool[1];
	colour.b = g_IntListPool[2];
	colour.a = g_IntListPool[3];

	return true;
}

bool CIwGameXmlAttribute::GetValueAsRect(CIwRect& rect)
{
	if (Value.GetAsListOfInt(g_IntListPool) != 4)
	{
		return false;
	}
	rect.x = g_IntListPool[0];
	rect.y = g_IntListPool[1];
	rect.w = g_IntListPool[2];
	rect.h = g_IntListPool[3];

	return true;
}

CIwGameXmlStringList* CIwGameXmlAttribute::GetValueAsList()
{
	CIwGameXmlStringList* pList = new CIwGameXmlStringList;
	
	// Separate Value by commas
	int			len = Value.GetLength();
	const char* text = Value.c_str();
	char		c;
	
	while ((c = *text++) != 0)
	{
		// Find a none white space
		if (c != ' ' && c != '\t' && c != '\n' && c != ',')
		{
			int count = 0;
			const char* found = text;
			
			// Find end of string
			while (count++ < 63)
			{
				c = *text;
				if (c == '\n' || c == ',' || c == 0)
					break;
				text++;
			}
			int len = text - found;
			if (len > 0)
			{
				CIwGameString *pString = new CIwGameString();
				pString->Copy((char*)found, 0, len);
			
				pList->push_back(pString);
			}
		}
	}

	return pList;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
// CIwGameXmlNode implementation
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CIwGameXmlNode::~CIwGameXmlNode()
{
	if (!Managed)
	{
		for (CIwGameXmlAttributeList::iterator it = Attributes.begin(); it != Attributes.end(); ++it)
		{
			if (!(*it)->Managed)
				delete *it;
		}
		for (CIwGameXmlNodeList::iterator it = Children.begin(); it != Children.end(); ++it)
		{
			if (!(*it)->Managed)
				delete *it;
		}
	}
	Attributes.clear();
	Children.clear();
}

void CIwGameXmlNode::AddAttribute(CIwGameXmlAttribute *attribute)
{
	CIwGameXmlAttribute* old_attribute = GetAttribute(attribute->getName().c_str());

	if (old_attribute == NULL)
	{
		// Attribute was not present so add
		Attributes.push_back(attribute);
	}
/*	else
	{
		// Attribute was present so update it
		old_attribute->setName((char*)attribute->getName().c_str(), attribute->getName().GetLength());
		old_attribute->setValue((char*)attribute->GetValue().c_str(), attribute->GetValue().GetLength());
	}*/
}

void CIwGameXmlNode::AddAttribute(CIwGameString& name, CIwGameString& value)
{
	CIwGameXmlAttribute* old_attribute = GetAttribute(name.c_str());

	if (old_attribute == NULL)
	{
		// Attribute was not present so add
		CIwGameXmlAttribute* attribute = CIwGameXmlParser::AllocAttribute();
		attribute->setName((char*)name.c_str(), name.GetLength());
		attribute->setValue((char*)value.c_str(), value.GetLength());
		Attributes.push_back(attribute);
	}
/*	else
	{
		// Attribute was present so update it
		old_attribute->setName((char*)name.c_str(), name.GetLength());
		old_attribute->setValue((char*)value.c_str(), value.GetLength());
	}*/
}

void CIwGameXmlNode::AddAttribute(CIwGameString& name, const char* value)
{
	CIwGameXmlAttribute* old_attribute = GetAttribute(name.c_str());

	if (old_attribute == NULL)
	{
		// Attribute was not present so add
		CIwGameXmlAttribute* attribute = CIwGameXmlParser::AllocAttribute();
		attribute->setName((char*)name.c_str(), name.GetLength());
		attribute->setValue((char*)value, strlen(value));
		Attributes.push_back(attribute);
	}
/*	else
	{
		// Attribute was present so update it
		old_attribute->setName((char*)name.c_str(), name.GetLength());
		old_attribute->setValue((char*)value, strlen(value));
	}*/
}

void CIwGameXmlNode::UpdateAttribute(CIwGameString& name, const char* value)
{
	CIwGameXmlAttribute* old_attribute = GetAttribute(name.c_str());

	if (old_attribute == NULL)
	{
		// Attribute was not present so add
		CIwGameXmlAttribute* attribute = CIwGameXmlParser::AllocAttribute();
		attribute->setName((char*)name.c_str(), name.GetLength());
		attribute->setValue((char*)value, strlen(value));
		Attributes.push_back(attribute);
	}
	else
	{
		// Attribute was present so update it
		old_attribute->setName((char*)name.c_str(), name.GetLength());
		old_attribute->setValue((char*)value, strlen(value));
	}
}

int CIwGameXmlNode::GetValueAsInt() const
{
	return Value.GetAsInt();
}

float CIwGameXmlNode::GetValueAsFloat() const
{
	return Value.GetAsFloat();
}

bool CIwGameXmlNode::GetValueAsBool() const
{
	return Value.GetAsBool();
}

void CIwGameXmlNode::PrintAttributes()
{
	for (CIwGameXmlAttributeList::iterator i = Attributes.begin(); i != Attributes.end(); ++i)
	{
		s3eDebugOutputString(" ");
		s3eDebugOutputString((*i)->Name.c_str());
		s3eDebugOutputString("=\"");
		s3eDebugOutputString((*i)->Value.c_str());
		s3eDebugOutputString("\"");
	}

}

void CIwGameXmlNode::PrintTree(int level)
{
	for (int t = 0; t < level; t++)
	{
		s3eDebugOutputString("\t");
	}
	s3eDebugOutputString("<");
	s3eDebugOutputString(Name.c_str());
	PrintAttributes();
	if (HasValue)
	{
		if (Value.GetLength() == 0)
			s3eDebugOutputString(" />\n");
		else
		{
			s3eDebugOutputString(">");
			s3eDebugOutputString(Value.c_str());
		}
	}
	else
	{
		s3eDebugOutputString(">\n");
		for (CIwGameXmlNodeList::iterator i = Children.begin(); i != Children.end(); ++i)
			(*i)->PrintTree(level + 1);
		for (int t = 0; t < level; t++)
			s3eDebugOutputString("\t");
	}
//	if (Value.GetLength() > 0 || Value.c_str() == NULL)
	if (Value.GetLength() > 0 || !HasValue)
	{
		s3eDebugOutputString("</");
		s3eDebugOutputString(Name.c_str());
		s3eDebugOutputString(">\n");
	}
}

void CIwGameXmlNode::ToLower(bool recursive)
{
	Name.ToLower();
	for (CIwGameXmlAttributeList::iterator i = Attributes.begin(); i != Attributes.end(); ++i)
	{
		(*i)->getName().ToLower();
	}
	if (recursive)
	{
		for (CIwGameXmlNodeList::iterator i2 = Children.begin(); i2 != Children.end(); ++i2)
		{
			(*i2)->ToLower(recursive);
		}
	}
}

void CIwGameXmlNode::ToUpper(bool recursive)
{
	Name.ToUpper();
	for (CIwGameXmlAttributeList::iterator i = Attributes.begin(); i != Attributes.end(); ++i)
	{
		(*i)->getName().ToUpper();
	}
	if (recursive)
	{
		for (CIwGameXmlNodeList::iterator i2 = Children.begin(); i2 != Children.end(); ++i2)
		{
			(*i2)->ToUpper(recursive);
		}
	}
}

int CIwGameXmlNode::SaveAttributes(CIwGameFile* file)
{
	for (CIwGameXmlAttributeList::iterator i = Attributes.begin(); i != Attributes.end(); ++i)
	{
		int err = s3eFilePrintf(file->getFileHandle(), " %s=\"%s\"", (*i)->Name.c_str(), (*i)->Value.c_str());
		if (err < 0)
			return err;
	}

	return 0;
}

int CIwGameXmlNode::SaveTree(CIwGameFile* file, int level)
{
	int err = 0;

	for (int t = 0; t < level; t++)
	{
		err = s3eFilePrintf(file->getFileHandle(), "\t");
		if (err < 0)
			return err;
	}
	err = s3eFilePrintf(file->getFileHandle(), "<%s", Name.c_str());
	if (err < 0)
		return err;
	err = SaveAttributes(file);
	if (err < 0)
		return err;
	if (HasValue)
	{
		if (Value.GetLength() == 0)
		{
			err = s3eFilePrintf(file->getFileHandle(), " />\n");
			if (err < 0)
				return err;
		}
		else
		{
			err = s3eFilePrintf(file->getFileHandle(), ">");
			if (err < 0)
				return err;
			err = s3eFilePrintf(file->getFileHandle(), "%s", Value.c_str());
			if (err < 0)
				return err;
		}
		err = s3eFilePrintf(file->getFileHandle(), "</%s>\n", Name.c_str());
		if (err < 0)
			return err;
	}
	else
	{
		if (Children.size() == 0)
		{
			err = s3eFilePrintf(file->getFileHandle(), " />\n");
			if (err < 0)
				return err;
		}
		else
		{
			err = s3eFilePrintf(file->getFileHandle(), ">\n");
			if (err < 0)
				return err;
			for (CIwGameXmlNodeList::iterator i = Children.begin(); i != Children.end(); ++i)
			{
				err = (*i)->SaveTree(file, level + 1);
				if (err < 0)
					return err;
			}
			for (int t = 0; t < level; t++)
			{
				err = s3eFilePrintf(file->getFileHandle(), "\t");
				if (err < 0)
					return err;
			}
			err = s3eFilePrintf(file->getFileHandle(), "</%s>\n", Name.c_str());
			if (err < 0)
				return err;
		}
	}

	return 0;
}

void CIwGameXmlNode::Copy(CIwGameXmlNode* copy)
{
	Managed = copy->Managed;
	Name = copy->Name;
	Value = copy->Value;
	HasValue = copy->HasValue;
	Parent = copy->Parent;

	// Remove old attributes list
	if (!Managed)
	{
		for (CIwGameXmlAttributeList::iterator it = Attributes.begin(); it != Attributes.end(); ++it)
		{
			if (!(*it)->Managed)
				delete *it;
		}
	}
	Attributes.clear();

	// Add in new attributes
	for (CIwGameXmlAttributeList::iterator it = copy->attribs_begin(); it != copy->attribs_end(); ++it)
	{
		CIwGameXmlAttribute* attrib;
		
		if (Managed)
			attrib = CIwGameXmlParser::AllocAttribute();
		else
			attrib = new CIwGameXmlAttribute();
		attrib->Managed = Managed;
		attrib->setName((*it)->getName().c_str());
		attrib->setValue((*it)->GetValue().c_str());
		AddAttribute(attrib);
	}
}

CIwGameXmlNode* CIwGameXmlNode::Clone(CIwGameXmlNode* parent)
{
	// Clone node
	CIwGameXmlNode* node = new CIwGameXmlNode();
	node->Managed = false;
	node->Name = Name;
	node->Value = Value;
	node->HasValue = HasValue;
	node->Parent = parent;

	// Clone nodes attributes
	for (CIwGameXmlAttributeList::iterator it = Attributes.begin(); it != Attributes.end(); ++it)
	{
		CIwGameXmlAttribute* attrib = new CIwGameXmlAttribute();
		attrib->Managed = false;
		attrib->setName((*it)->getName().c_str());
		attrib->setValue((*it)->GetValue().c_str());
		node->AddAttribute(attrib);
	}

	return node;
}

CIwGameXmlNode* CIwGameXmlNode::CloneTree(CIwGameXmlNode* parent)
{
	// Clone node
	CIwGameXmlNode* node = Clone(parent);

	// Clone children
	for (CIwGameXmlNodeList::iterator it = Children.begin(); it != Children.end(); ++it)
	{
		node->AddChild((*it)->CloneTree(this));
	}

	return node;
}

int CIwGameXmlNode::Save(const char* filename)
{
	// Dont save root node
	if (Name == ".")
	{
		CIwGameXmlNode* node = GetFirstNode();
		if (node == NULL)
			return -1;
		return node->Save(filename);
	}

	CIwGameFile file;
	if (file.Open(filename, "wb", true))
	{
		s3eFilePrintf(file.getFileHandle(), "<?xml version=\"1.0\"?>\n");
		int err = SaveTree(&file);
		if (err < 0)
			return err;
	}
	else
		return -1;

	return 0;
}

CIwGameXmlNode* CIwGameXmlNode::GetFirstNode()
{
	if (Children.size() == 0)
		return NULL;
	
	return Children.element_at(0);
}

CIwGameXmlNode* CIwGameXmlNode::GetFirstNamedNode(unsigned int name_hash)
{
	// Check this mode
	if (Name.getHash() == name_hash)
		return this;
	
	// Check child nodes
	for (CIwGameXmlNodeList::iterator i = Children.begin(); i != Children.end(); ++i)
	{
		CIwGameXmlNode* pNode = *i;
		CIwGameXmlNode* pFound =  pNode->GetFirstNamedNode(name_hash);
		if (pFound != NULL)
			return pFound;
	}
	
	return NULL;
}

void CIwGameXmlNode::GetNamedNodes(unsigned int name_hash, CIwGameXmlNodeList *nodes)
{
	// Check this mode
	if (Name.getHash() == name_hash)
		nodes->push_back(this);
		
	// Check child nodes
	for (CIwGameXmlNodeList::iterator i = Children.begin(); i != Children.end(); ++i)
		(*i)->GetNamedNodes(name_hash, nodes);
}

CIwGameXmlAttribute*	CIwGameXmlNode::GetAttribute(const char* name)
{
	unsigned int hash = CIwGameString::CalculateHash(name);
	
	for (CIwGameXmlAttributeList::iterator i = Attributes.begin(); i != Attributes.end(); ++i)
	{
		if ((*i)->Name.getHash() == hash)
			return *i;
	}
	
	return NULL;
}

CIwGameXmlAttribute*	CIwGameXmlNode::GetAttribute(unsigned int hash)
{
	for (CIwGameXmlAttributeList::iterator i = Attributes.begin(); i != Attributes.end(); ++i)
	{
		if ((*i)->Name.getHash() == hash)
			return *i;
	}
	
	return NULL;
}

CIwGameXmlAttribute*	CIwGameXmlNode::GetAttributeAtInex(int index)
{
	if (index < 0 || index >= Attributes.size())
		return NULL;
	
	CIwGameXmlAttributeList::iterator i = Attributes.begin();
	while (index-- > 0)
		++i;
		
	return *i;
}

//
//
//
//
// CXMLParser implementation
//
//
//
//
const char* CIwGameXmlParser::GetErrorString(eXMLParserError error) const
{
	return CIwGameXmlParserErrors[error];
}

const char* CIwGameXmlParser::GetCloseTagName(CIwGameXmlNode *node) const
{
	int len = node->Name.GetLength();
	char* pCloseTag = new char [len + 3];
	pCloseTag[0] = '<';
	pCloseTag[1] = '/';
	pCloseTag[len + 3] = '>';
	
	for (int t = 0; t < len; t++)
		pCloseTag[t + 2] = node->Name[t];
		
	return pCloseTag;
}

void CIwGameXmlParser::ShowError(eXMLParserError error, int pos) const
{
#ifdef SHOW_ERRORS
	s3eDebugOutputString(GetErrorString(error));
	s3eDebugOutputString(" at line ");
	CIwGameString num = CIwGameString(m_pDataInput->GetLineNumber(pos));
	s3eDebugOutputString(num.c_str());
#endif
}

eXMLParserError	CIwGameXmlParser::ParseAttributes(CIwGameXmlNode* node)
{
	while (1)
	{
		char* pBuffer = (char *)m_pDataInput->getData();
		
		// Skip tag name
		if (m_pDataInput->SkipToWhitespaceInTag() < 0)
			return eXMLParserError_None;
			
		// Skip to first attribute name
		int name_pos = m_pDataInput->SkipToNoneWhitespaceInTag();
		if (name_pos < 0)
			return eXMLParserError_None;
		
		// Find length of name
		int name_end_pos = m_pDataInput->SkipToWhitespaceInTag();
		if (name_end_pos < 0)
			return eXMLParserError_None;
		
		// Set attribute name
		CIwGameXmlAttribute* attrib = AllocAttribute();
		node->AddAttribute(attrib);
		attrib->setName(pBuffer + name_pos, name_end_pos - name_pos);
		
		// Find equal sign
		int equal_pos = m_pDataInput->SkipToCharInTag('=');
		if (equal_pos < 0)
		{
			ShowError(eXMLParserError_MissingEquals, m_pDataInput->GetPos());
			return eXMLParserError_MissingEquals;
		}
			
		// Find open quote position
		int quote_start = m_pDataInput->SkipToCharInTag('"');
		if (quote_start < 0)
		{
			ShowError(eXMLParserError_MissingValue, m_pDataInput->GetPos());
			return eXMLParserError_MissingValue;
		}
		m_pDataInput->Skip(1);
		
		// Get closing quote position
		int quote_end = m_pDataInput->SkipToCharInTag('"');
		if (quote_end < 0)
		{
			ShowError(eXMLParserError_MissingClosingQuote, m_pDataInput->GetPos());
			return eXMLParserError_MissingClosingQuote;
		}
			
		attrib->setValue(pBuffer + quote_start + 1, quote_end - quote_start - 1);
	}
	
	return eXMLParserError_None;
}

eXMLParserError CIwGameXmlParser::Parse(CIwGameFile *file)
{
	if (file->Open(NULL, "rb", true))
	{
		int size = file->getFileSize();
		m_pDataInput = new CIwGameDataInput();
		m_pDataInput->Init(size);
		if (!file->Read(m_pDataInput->getData(), size))
			return eXMLParserError_FileError;
	}
	else
		return eXMLParserError_FileError;

	m_pDataInput->StripXMLComments();
/*file = s3eFileOpen("./Test9.xml", "wb");
if (file != NULL)
{
	s3eFileWrite(m_pDataInput->getData(), 1, m_pDataInput->GetLength(), file);
	s3eFileClose(file);
}
else
{
	s3eFileGetError();
	s3eDebugOutputString(s3eFileGetErrorString());
}*/

	return Parse(m_pDataInput);
}

eXMLParserError CIwGameXmlParser::Parse(const char* filename)
{
	CIwGameFile file(filename);

	eXMLParserError err = Parse(&file);

/*	CIwGameString save_file;
	save_file.setString(pFilename);
	save_file += ".out.txt";
	m_pRoot->Save(save_file);*/

	return err;
}

int CIwGameXmlParser::GetNextWord(const char* data, int &offset)
{
	char c;
	int index = 0;
	offset = -1;

	while ((c = *data++) != 0)
	{
		if (offset < 0)
		{
			// Find none whitespace
			if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
				offset = index;
		}
		else
		{
			// Find whitespace or xml tag brace
			if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '>' || c == '=')
				return index - offset;
		}

		index++;
	}

	return -1;
}

int CIwGameXmlParser::GetNextQuotedWord(const char* data, int &offset)
{
	char c;
	char prev_c = 0;
	int index = 0;
	offset = -1;

	while ((c = *data++) != 0)
	{
		if (offset < 0)
		{
			// Find none whitespace
			if (c == '"')
				offset = index + 1;
		}
		else
		{
			// Find whitespace
			if (c == '"' && prev_c != '\\')
				return index - offset;
		}

		index++;
		prev_c = c;
	}

	return -1;
}

eXMLParserError	CIwGameXmlParser::ParseAttributes(const char* data, int count, CIwGameXmlNode* node)
{
	while (count > 0)
	{
		// Get attribute name
		int offset;
		int len = GetNextWord(data, offset);
		char c = *(data + offset);
		if (len <= 0 || c == '/' || c == '>')
			return eXMLParserError_None;

		CIwGameXmlAttribute* attrib = AllocAttribute();
//memcpy(str, data + offset, len);
//str[len] = 0;
//sprintf(str2, "Attrib name %s, ", str);
//s3eDebugOutputString(str2);
		attrib->setName((char*)data + offset, len);
		node->AddAttribute(attrib);
		data += len + 1 + offset;
		count -= len + 1 + offset;
		if (count <= 0)
			return eXMLParserError_InvalidTag;

		// Get attribute value
		len = GetNextQuotedWord(data, offset);
		if (len < 0)
			return eXMLParserError_InvalidTag;
//memcpy(str, data + offset, len);
//str[len] = 0;
//sprintf(str2, "Attrib value %s\n", str);
//s3eDebugOutputString(str2);
		attrib->setValue((char*)data + offset, len);
		data += len + 2 + offset;
		count -= len + 2 + offset;
		if (count <= 0)
			return eXMLParserError_InvalidTag;
	}

	return eXMLParserError_None;
}

bool CIwGameXmlParser::PreParse()
{
//CIwGameError::LogError("CIwGameXmlParser::PreParse ****");
	Marker = TagPool + NextFreePoolTagIndex;
	while (1)
	{
		if (m_pDataInput->IsEOF())
			break;

		int tag_pos = 0;
		int tag_len = m_pDataInput->GetNextTagOrValue('<', '>', m_pDataInput->Remaining(), tag_pos);
		if (tag_len > 0)
		{
//CIwGameError::LogError("CIwGameXmlParser::PreParse - Marker - ", CIwGameString(m_pDataInput->getData(), tag_pos, tag_len).c_str());
			CIwGameXmlTagMarker* tag_marker = AllocTag();
			if (tag_marker == NULL)
			{
				return false;
			}
			tag_marker->Start = tag_pos;
			tag_marker->Length = tag_len;
			MarkerCount++;
			
//			CIwGameString* tag = new CIwGameString(m_pDataInput->getData(), tag_pos, tag_len);
//			TempElements.push_back(tag);

//s3eDebugOutputString(tag->c_str());
		}
	}

	return true;
}

eXMLParserError CIwGameXmlParser::Parse(CIwGameXmlNode* parent)
{
	CIwList<CIwGameXmlNode*>* parent_stack = new CIwList<CIwGameXmlNode*>();
	
	parent_stack->push_back(parent);
	int		count = NextFreePoolTagIndex;
	int		tag_index = 0;
	char*	c_data = m_pDataInput->getData();

	parent = parent_stack->back();

	while (tag_index < MarkerCount)
	{
		if (parent == NULL)
			break;
		CIwGameXmlTagMarker* marker = Marker + tag_index;
		const char* data = c_data + marker->Start;
		int len = marker->Length;

		if (*data == '<' && *(data + len - 1) == '>')
		{
			if (*(data + 1) == '/')
			{
				// Found an end tag, check that name matches parents
				int et_offset = 0;
				int et_len = GetNextWord(data + 2, et_offset);
				if (et_len <= 0)
				{
					CIwGameError::LogError("CIwGameXmlParser::Parse - Invalid close tag");
					return eXMLParserError_InvalidTag;
				}
				if (!parent->Name.Compare(data + et_offset + 2, et_len))
				{
					CIwGameError::LogError("CIwGameXmlParser::Parse - Mismatched end tag - ", parent->Name.c_str());
					CIwGameError::LogError("CIwGameXmlParser::Parse - Instead found - ", CIwGameString(data, et_offset + 2, et_len).c_str());
					return eXMLParserError_MismatchedEndTag;
				}
				for (CIwList<CIwGameXmlNode*>::iterator it = parent_stack->begin(); it != parent_stack->end(); it++)
				{
					CIwGameXmlNode* node = *it;
					if (node == parent)
					{
						parent_stack->erase(it);
						parent = parent_stack->back();
						break;
					}
				}
				tag_index++;
			}
			else
			{
				// Create a node
				CIwGameXmlNode* node = AllocNode();
				int offset;
				int nlen = GetNextWord(data + 1, offset);
				if (nlen <= 0)
					return eXMLParserError_InvalidTag;
				node->Name.setString(data + offset + 1, nlen);
				if ((len - 2) > nlen)
					ParseAttributes(data + offset + nlen + 1, len - nlen - offset - 1, node);
				parent->AddChild(node);

				if (*(data + len - 2) != '/')		// Self closing tags dont get added as parents
				{
					parent_stack->push_back(node);	// Found an inner tag so push parent
					parent = parent_stack->back();
				}

				// Check next line to see if it is a tag or a value
				tag_index++;
				if (tag_index != (NextFreePoolTagIndex - 1))
				{
					CIwGameXmlTagMarker* marker2 = Marker + tag_index;
					const char* data2 = c_data + marker2->Start;
					int len2 = marker2->Length;

					if (*data2 != '<')
					{
						// Found a value
						node->Value.setString(data2, len2);
						node->HasValue = true;
					}
				}
			}
		}
		else
			tag_index++;
	}

	parent_stack->clear();
	delete parent_stack;

	return eXMLParserError_None;
}

eXMLParserError CIwGameXmlParser::Parse(CIwGameDataInput* data)
{
	if (NodePool == NULL)
	{
		CIwGameError::LogError("Error:: Xml node and tag pools not setup, call CIwGameXmlParser::Init()");
		return eXMLParserError_InvalidPools;
	}

	if (m_pRoot != NULL)
	{
		if (NodePool == NULL)
			delete m_pRoot;
		m_pRoot = NULL;
	}
	if (m_pDataInput != NULL && data != m_pDataInput)
	{
		delete m_pDataInput;
		m_pDataInput = NULL;
	}

	m_pDataInput = data;

	// Parse header
	ParseXMLHeader();
	
	// Create root
	m_pRoot = AllocNode();
	m_pRoot->SetName(".", 1);

	MarkerCount = 0;
	if (!PreParse())
		eXMLParserError_PreParseError;

	eXMLParserError error = Parse(m_pRoot);

	//	eXMLParserError error = Parse(m_pRoot, m_pDataInput->Remaining(), num_tags_found);
	return error;
}

eXMLParserError CIwGameXmlParser::ParseXMLHeader()
{
	int pos = m_pDataInput->FindString("<?xml", 5);
	if (pos >= 0)
	{
		pos = m_pDataInput->FindString("?>", 2);
		if (pos >= 0)
		{
			// For now we ignore the xml header info
			// TODO: Pull version and encoding
			m_pDataInput->SetPos(pos + 2);
		}
	}

	return eXMLParserError_None;
}

eXMLParserError CIwGameXmlParser::Parse(CIwGameXmlNode* pParent, int range, int &num_tags_found)
{
	char* pBuffer = (char *)m_pDataInput->getData();
	
	while (1)
	{
		if (m_pDataInput->IsEOF())
			break;
	
		// Search for a tag
		int old_pos = m_pDataInput->GetPos();
		int tag_pos;
		int tag_len = m_pDataInput->GetNextTag('<', '>', range, tag_pos);
		if (tag_len >= 0)
		{
			if (*(pBuffer + tag_pos) == '/')
			{
				// Close tag was found so return to parent
				m_pDataInput->SetPos(old_pos);
				return eXMLParserError_None;
			}
			else
			{
				// Set node name
				int ws = CIwGameXmlTools::GetFirstWhitespaceIndex(pBuffer + tag_pos, tag_len);

				// Create a new node
				CIwGameXmlNode* new_node = AllocNode();
				new_node->SetName(pBuffer + tag_pos, ws <= 0 ? tag_len : ws);

				// Create new node and add it to parent
				if (pParent != NULL)
				{
					pParent->AddChild(new_node);
				}
						
				num_tags_found++;

				// Get attributes
				int op = m_pDataInput->GetPos();
				m_pDataInput->SetPos(tag_pos);
				eXMLParserError pe = ParseAttributes(new_node);
				if (pe != eXMLParserError_None)
					return pe;
				m_pDataInput->SetPos(op);

				// If not short tag then check for inner tags
				char ec = *(pBuffer + tag_pos + tag_len - 1);
				if (ec != '/')
				{
					// Parse inner tags
					int num_tags = 0;
					eXMLParserError error = Parse(new_node, range, num_tags);
					if (error != eXMLParserError_None)
						return error;
						
					// Check closing tag matches
					int close_tag_pos;
					int close_tag_len = m_pDataInput->GetNextTag('<', '>', range, close_tag_pos);
					if (close_tag_len >= 0)
					{
						// If close tag was only tag found then get value from between tags
						if (num_tags == 0)
						{
							int value_len = close_tag_pos - (tag_pos + tag_len) - 2;
							new_node->SetValue(pBuffer + tag_pos + tag_len + 1, value_len);
						}

						// Check to ensure that close tag matches open tag
						if (new_node->Name.GetLength() != (close_tag_len - 1) || !new_node->Name.Compare(pBuffer + close_tag_pos + 1, close_tag_len - 1))
						{
//								ShowError(eXMLParserError_MismatchedEndTag, close_tag_pos);
//								return eXMLParserError_MismatchedEndTag;
						}
					}
					else
					{
						ShowError(eXMLParserError_MissingEndTag, tag_pos);
						return eXMLParserError_MissingEndTag;
					}
				}
				else
				{
					new_node->SetValue("", 0);
				}
			}
		}
		else
		{
			ShowError(eXMLParserError_InvalidTag, tag_pos);
			return eXMLParserError_InvalidTag;
		}
	}
	
	return eXMLParserError_NoTagFound;
}

//
// CIwGameXmlTools implementation
//
int	CIwGameXmlTools::GetFirstWhitespaceIndex(char* mem, int len)
{
	for (int t = 0; t < len; t++)
	{
		char c = *mem++;
		if (c == ' ' || c == '\t' || c == '\n')
			return t;
	}

	return -1;
}



