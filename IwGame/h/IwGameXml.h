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

// Warnings
// * Before parsing ensure that the XKL data pools have been set up by calling CIwGameXmlParser::PoolsInit()
// * CIwGameXmlParser::PoolsReset() to reset pool data and prevent running out of pool memory (do not call whilst parsing an XML file)


#if !defined(_IW_GAME_XML_H_)
#define _IW_GAME_XML_H_


#include <IwList.h> 
#include "s3e.h"
#include "IwGx.h"
#include "IwGeom.h"

#include "IwGameUtil.h"
#include "IwGameString.h"
#include "IwGameDataIO.h"

class CIwGameFile;
class CIwGameXmlParser;
struct CIwGameXmlAttribute;
struct CIwGameXmlNode;

//
//
//
// Type definitions provided for ease of code reading
//
//
//
typedef CIwList<CIwGameXmlAttribute*> CIwGameXmlAttributeList;
typedef CIwList<CIwGameXmlNode*> CIwGameXmlNodeList;
typedef CIwList<CIwGameString*> CIwGameXmlStringList;
typedef CIwList<int> CIwGameXmlIntList;
typedef CIwList<float> CIwGameXmlFloatList;
typedef CIwList<bool> CIwGameXmlBoolList;

extern int			g_IntListPool[];
extern int			g_IntListPoolIndex;
extern float		g_FloatListPool[];
extern int			g_FloatListPoolIndex;
extern bool			g_BoolListPool[];
extern int			g_BoolListPoolIndex;


//
//
//
// General tools used by all CIwGameXml classes
//
//
//
class CIwGameXmlTools
{
public:
	static int				GetFirstWhitespaceIndex(char* pMem, int len);
};

//
//
//
// An Xml attribute, e.g. <Something attribute="some value" />
//
//
//
struct CIwGameXmlAttribute
{
public:
	bool					Managed;			// Managec attributes are managed by the pools system and should not be deleted
	CIwGameString			Name;				// Attributes name
	CIwGameString			Value;				// Attributes value

public:
	CIwGameXmlAttribute() : Managed(false) {}
	~CIwGameXmlAttribute() {}

	void			Clear()
	{
		Name = "";
		Value = "";
//		Name.reset();
//		Value.reset();
	}
	
	void	setName(const char* pName)
	{
		Name.Copy(pName, 0, strlen(pName));
	}
	void	setName(char* pName, int len)
	{
		Name.Copy(pName, 0, len);
	}
	CIwGameString& getName() { return Name; }
	void	setValue(const char* pValue)
	{
		Value.Copy(pValue, 0, strlen(pValue));
		Value.ReplaceHTMLCodes();
	}
	void	setValue(char* pValue, int len)
	{
		Value.Copy(pValue, 0, len);
		Value.ReplaceHTMLCodes();
	}
	
	// Single attribute element getters
	CIwGameString&		GetValue() { return Value; }
	int					GetValueAsInt() const;
	float				GetValueAsFloat() const;
	bool				GetValueAsBool() const;
	bool				GetValueAsPoint(CIwFVec2 &point);
	bool				GetValueAsPoint3(CIwFVec3 &point);
	bool				GetValueAsPoint4(CIwFVec4 &point);
	bool				GetValueAsColour(CIwColour &colour);
	bool				GetValueAsRect(CIwRect &rect);
	
	// List attribute element getters (generates a list from comma separated attribute data
	CIwGameXmlStringList*	GetValueAsList();
};

//
//
//
// An Xml node, e.g. <XmlNode></XmlNode>
//
//
//
struct CIwGameXmlNode
{
public:
	typedef CIwList<CIwGameXmlNode*>::iterator		_Iterator;
	typedef CIwList<CIwGameXmlAttribute*>::iterator	_AttribIterator;
	_Iterator				begin()				{ return Children.begin(); }
	_Iterator				end()				{ return Children.end(); }
	_AttribIterator			attribs_begin()		{ return Attributes.begin(); }
	_AttribIterator			attribs_end()		{ return Attributes.end(); }

public:
	bool						Managed;			// Managec nodes are managed by the pools system and should not be deleted
	CIwGameString				Name;				// Node name
	CIwGameString				Value;				// Node value
	bool						HasValue;			// Determines if value is valid
	CIwGameXmlAttributeList		Attributes;			// List of attributes associated with this node
	CIwGameXmlNode*				Parent;				// Parent node
	CIwGameXmlNodeList			Children;			// List of children nodes
	CIwGameXmlParser*			Parser;				// Parser that created this node
	
public:
	CIwGameXmlNode() : Parent(NULL), HasValue(false), Managed(false), Parser(NULL) {}
	~CIwGameXmlNode();
	void Clear()
	{
		Name = "";
		Value = "";
		Attributes.clear();
		Parent = NULL;
		Children.clear();
		HasValue = false;
	}
	
	void AddChild(CIwGameXmlNode* node)
	{
		node->Parent = this;
		Children.push_back(node);
	}
	
	void AddAttribute(CIwGameXmlAttribute *attribute);
	void AddAttribute(CIwGameString& name, CIwGameString& value, CIwGameXmlParser* parser);
	void AddAttribute(CIwGameString& name, const char* value, CIwGameXmlParser* parser);
	void UpdateAttribute(CIwGameString& name, const char* value, CIwGameXmlParser* parser);

	void SetName(const char* name)
	{
		Name.Copy((char*)name, 0, strlen(name));
	}
	void SetName(const char* name, int len)
	{
		Name.Copy((char*)name, 0, len);
	}
	void SetValue(const char* value)
	{
		Value.Copy((char*)value, 0, strlen(value));
		Value.ReplaceHTMLCodes();
		HasValue = true;
	}
	void SetValue(const char* value, int len)
	{
		Value.Copy((char*)value, 0, len);
		Value.ReplaceHTMLCodes();
		HasValue = true;
	}
	const CIwGameString&	GetName() const { return Name; }
	
	// Single value getters (data between tags)
	const CIwGameString&	GetValue() const { return Value; }
	int						GetValueAsInt() const;
	float					GetValueAsFloat() const;
	bool					GetValueAsBool() const;

	CIwGameXmlParser*		getParser()		{ return Parser; }
	void					setParser(CIwGameXmlParser* parser)	{ Parser = parser; }
	
	// Print attributes / tree (DEBUG)
	void					PrintAttributes(CIwGameString& out);
	void					PrintTree(CIwGameString& out, int level = 0);

	// Get first child node
	CIwGameXmlNode*			GetFirstNode();
	// Get first occurrence of a node in this node and all children
	CIwGameXmlNode*			GetFirstNamedNode(unsigned int name_hash);
	// Get all occurrences of a node in this node and all children
	void					GetNamedNodes(unsigned int name_hash, CIwGameXmlNodeList* nodes);

	// Gets attribute count
	int						GetAttributeCount() const { return Attributes.size(); }
	// Gets the named attribute
	CIwGameXmlAttribute*	GetAttribute(const char* name);
	CIwGameXmlAttribute*	GetAttribute(unsigned int hash);
	// Gets the Nth attribute
	CIwGameXmlAttribute*	GetAttributeAtInex(int index);

	// Save XML file
	int						SaveAttributes(CIwGameFile* file);
	int						SaveTree(CIwGameFile* file, int level = 0);
	int						Save(const char* filename);

	// Tag / attribute case chnage (does not change the case of attribute values)
	void					ToLower(bool recursive);
	void					ToUpper(bool recursive);

	// Tree cloning
	CIwGameXmlNode*			Clone(CIwGameXmlNode* parent);
	CIwGameXmlNode*			CloneTree(CIwGameXmlNode* parent);

};

enum eXMLParserError
{
	eXMLParserError_None,
	eXMLParserError_NoTagFound,
	eXMLParserError_FileError,
	eXMLParserError_MissingEndTag,
	eXMLParserError_MissingEndComment,
	eXMLParserError_MismatchedEndTag, 
	eXMLParserError_MissingEquals, 
	eXMLParserError_MissingValue, 
	eXMLParserError_MissingClosingQuote, 
	eXMLParserError_InvalidTag, 
	eXMLParserError_InvalidComment, 
	eXMLParserError_PreParseError, 
	eXMLParserError_InvalidPools, 
};

struct CIwGameXmlTagMarker
{
	int		Start;
	int		Length;
	int		Line;
	
	void	Clear()
	{
		Start = 0;
		Length = 0;
		Line = 0;
	}
};

//
//
//
// The Xml parser
//
//
//
class CIwGameXmlParser
{
public:

private:
	CIwGameString				m_Encoding;					// Encoding type
	CIwGameString				m_Version;					// XML version
	CIwGameDataInput*			m_pDataInput;				// Input stream
	CIwGameXmlNode*				m_pRoot;					// Root node
	CIwGameXmlTagMarker*		Marker;
	int							MarkerCount;

	CIwList<CIwGameXmlNode*>::iterator		RemoveNodeFromList(CIwList<CIwGameXmlNode*>& list, const CIwList<CIwGameXmlNode*>::iterator& it);
public:
	CIwGameXmlParser() : m_pDataInput(NULL), m_pRoot(NULL)
	{
		TagPool = NULL;
		NextFreePoolTagIndex = 0;
		MaxPoolTags = 0;
		NodePool = NULL;
		NextFreePoolNodeIndex = 0;
		MaxPoolNodes = 0;
		AttributePool = NULL;
		NextFreePoolAttributeIndex = 0;
		MaxPoolAttributes = 0;
//		m_Encoding.setString("utf-8");
//		m_Version.setString("1.0");
	}
	virtual ~CIwGameXmlParser()
	{
//		if (m_pRoot != NULL)
//			delete m_pRoot;
		SAFE_DELETE_ARRAY(TagPool);
		SAFE_DELETE_ARRAY(NodePool);
		SAFE_DELETE_ARRAY(AttributePool);
		SAFE_DELETE(m_pDataInput);
	}

	void					SetEncoding(const char* enocding) { m_Encoding.setString(enocding); }
	const CIwGameString&	GetEncoding() const { return m_Encoding; }
	void					SetVersion(const char* version) { m_Version.setString(version); }
	const CIwGameString&	GetVersion() const { return m_Version; }

	CIwGameXmlNode*		AllocNode()
	{
		if (NodePool == NULL)
			return new CIwGameXmlNode();
		if (NextFreePoolNodeIndex >= MaxPoolNodes)
		{
			return new CIwGameXmlNode();
//			CIwGameError::LogError("Error: Ran out of nodes, allocate more");
//			return NULL;
		}
		CIwGameXmlNode* node = NodePool + NextFreePoolNodeIndex++;
		node->Clear();
		node->setParser(this);
		node->Managed = true;

		return node;
	}
	CIwGameXmlTagMarker*	AllocTag()
	{
		if (TagPool == NULL)
			return new CIwGameXmlTagMarker();
		if (NextFreePoolTagIndex >= MaxPoolTags)
		{
			return new CIwGameXmlTagMarker();
//			CIwGameError::LogError("Error: Ran out of tag markers, allocate more");
//			return NULL;
		}
		CIwGameXmlTagMarker* tag = TagPool + NextFreePoolTagIndex++;
		tag->Clear();

		return tag;
	}
	
	CIwGameXmlAttribute*	AllocAttribute()
	{
		if (AttributePool == NULL)
			return new CIwGameXmlAttribute();
		if (NextFreePoolAttributeIndex >= MaxPoolAttributes)
		{
			return new CIwGameXmlAttribute();
//			CIwGameError::LogError("Error: Ran out of attributes, allocate more");
//			return NULL;
		}
		CIwGameXmlAttribute* attribute = AttributePool + NextFreePoolAttributeIndex++;
		attribute->Clear();
		attribute->Managed = true;

		return attribute;
	}

	// Parse the supplied file
	eXMLParserError				Parse(const char* pFilename);
	eXMLParserError				Parse(CIwGameFile *file);
	
	// Parse the supplied memory buffer
	eXMLParserError				Parse(CIwGameDataInput* pData);		// This class becomes owner of CDataInput object

	// Return XML error string
	const char*					GetErrorString(eXMLParserError error) const;
	
	// Get root node (note that root node is '.' and not the first XML tag)
	CIwGameXmlNode*				GetRoot()
	{
		return m_pRoot;
	}
	
	// Get first occurrence of a node in this node and all children
	CIwGameXmlNode*				GetFirstNamedNode(CIwGameXmlNode *parent, const char* node_name)
	{
		if (parent == NULL)
			parent = GetRoot();

		int hash = CIwGameString::CalculateHash(node_name);
		if (parent != NULL)
			return parent->GetFirstNamedNode(hash);
		if (m_pRoot != NULL)
			return m_pRoot->GetFirstNamedNode(hash);
			
		return NULL;
	}
	// Get all occurrences of a node in this node and all children (caller is responsible for cleaning up list)
	// Pass NULL as pNodeName to return all nodes
	CIwGameXmlNodeList*			GetNamedNodes(CIwGameXmlNode *parent, const char* node_name)
	{
		if (parent == NULL)
			parent = GetRoot();

		int				hash = CIwGameString::CalculateHash(node_name);
		CIwGameXmlNodeList*	pNodes = new CIwGameXmlNodeList;
		if (parent != NULL)
			parent->GetNamedNodes(hash, pNodes);
		else
		if (m_pRoot != NULL)
			m_pRoot->GetNamedNodes(hash, pNodes);
			
		return pNodes;
	}

private:
	eXMLParserError		ParseXMLHeader();
	eXMLParserError		ParseAttributes(const char* data, int count, CIwGameXmlNode* node);
	bool				PreParse();
	eXMLParserError		Parse(CIwGameXmlNode* parent);

	eXMLParserError		ParseAttributes(CIwGameXmlNode* node);
	eXMLParserError		Parse(CIwGameXmlNode* pParent, int range, int &num_tags_found);
	const char*			GetCloseTagName(CIwGameXmlNode *node) const;
	void				ShowError(eXMLParserError error, int pos) const;

	int					GetNextWord(const char* data, int &offset);
	int					GetNextQuotedWord(const char* data, int &offset);

//	friend CIwGameXmlNode;
//	friend CIwGameXmlAttribute;

	// Tag and node pooling
	CIwGameXmlTagMarker*	TagPool;					// Used to separate tags from the raw XML file
	int						NextFreePoolTagIndex;
	int						MaxPoolTags;
	CIwGameXmlNode*			NodePool;					// Xml nodes pool
	int						NextFreePoolNodeIndex;		// Bext free node index in nodes pool
	int						MaxPoolNodes;				// Maximum available nodes in the nodes pool
	CIwGameXmlAttribute*	AttributePool;				// Xml attribute pool
	int						NextFreePoolAttributeIndex;	// Bext free attribute index in nodes pool
	int						MaxPoolAttributes;			// Maximum available attributes in the attributes pool
public:

};




#endif	// _IW_GAME_XML_H_
