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

#include "IwGameTemplates.h"
#include "IwGameScene.h"
#include "IwGameResources.h"
#include "IwGameActor.h"

//
//
//
//
// CIwGameTemplate implementation
//
//
//
//
bool CIwGameTemplate::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Parse template attributes
	CIwGameString name;
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
		{
			setName((*it)->GetValue().c_str());
			name = (*it)->GetValue();
		}
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
	}

	// Only clone none empty template
	if (node->begin() != node->end())
	{
		Root = node->CloneTree(NULL);
	}

	CIwGameScene* scene = NULL;
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		scene = ((CIwGameActor*)parent)->getScene();
	else
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	if (scene != NULL)
		return scene->getResourceManager()->addResource(this);
	else
		return IW_GAME_GLOBAL_RESOURCES->getResourceManager()->addResource(this);

	return true;
}

bool CIwGameTemplate::Instantiate(IIwGameXomlResource* parent, CIwGameXmlNode* replacements)
{
	// We need to work on a copy of the tree so we do not overwtite the template
	CIwGameXmlNode* object_node = getRoot();
	CIwGameXmlNode* tree_copy = object_node->CloneTree(NULL);

	// Search copy of template replacing any attribute values with the provided values
	if (replacements != NULL)
		ReplaceTreeAttributes(tree_copy, replacements);

	// Process the XOML nodes
	if (!IW_GAME_XOML->Process(parent, tree_copy))
	{
		delete tree_copy;
		return false;
	}

	// Remove temp tree
	delete tree_copy;

	return true;
}

void CIwGameTemplate::ReplaceAttributes(CIwGameXmlAttribute* attrib, CIwGameXmlNode* replacements)
{
	// Only attempt to replace an attribute if it contains a dollar sign. Names enclosed in dollar signs are to be replaced by attributes from the template
	CIwGameString& value = attrib->GetValue();
//CIwGameError::LogError("Checking - ", value.c_str());
	if (value.Contains('$') < 0)
		return;

	// Check attribute list 
	CIwGameString var;
	for (CIwGameXmlNode::_AttribIterator it = replacements->attribs_begin(); it != replacements->attribs_end(); it++)
	{
		CIwGameString& prop_name = (*it)->getName();
		CIwGameString& prop_value = (*it)->GetValue();
		unsigned int prop_name_hash = prop_name.getHash();
		if (prop_name_hash != CIwGameXomlNames::Template_Hash)
		{
			var = "$";
			var += prop_name;
			var += "$";
			if (value.Replace(var.c_str(), prop_value.c_str()) >= 0)
			{
//CIwGameError::LogError("  string - ", var.c_str());
//CIwGameError::LogError("  with - ", value.c_str());
			}
		}
	}
}

void CIwGameTemplate::ReplaceTreeAttributes(CIwGameXmlNode* node, CIwGameXmlNode* replacements)
{
	// Replace any marked attribute values with replacements provided by the user
	for (CIwGameXmlNode::_AttribIterator it2 = node->attribs_begin(); it2 != node->attribs_end(); it2++)
	{
		unsigned int name_hash = (*it2)->getName().getHash();

		ReplaceAttributes(*it2, replacements);
	}

	// Process child nodes
	for (CIwGameXmlNode::_Iterator it = node->begin(); it != node->end(); it++)
	{
		ReplaceTreeAttributes(*it, replacements);
	}
}



//
// 
// 
//
// CIwGameTemplateFrom Implementation
// 
// 
// 
//
bool CIwGameTemplateFrom::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	CIwGameString temp;

	// Process LoadXoml specific attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Template_Hash)
		{
			temp = (*it)->GetValue();
		}
	}

	if (!temp.IsEmpty())
	{
		CIwGameTemplate* templ = NULL;
		CIwGameScene* scene = NULL;

		if (parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
			scene = ((CIwGameActor*)parent)->getScene();

		// Find the template
		if (scene != NULL)
		{
			templ = (CIwGameTemplate*)scene->getResourceManager()->findResource(temp.c_str(), CIwGameXomlNames::Template_Hash);
		}
		else
		if (parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		{
			scene = (CIwGameScene*)parent;
			templ = (CIwGameTemplate*)scene->getResourceManager()->findResource(temp.c_str(), CIwGameXomlNames::Template_Hash);
		}

		if (templ == NULL)
			templ = (CIwGameTemplate*)IW_GAME_GLOBAL_RESOURCES->getResourceManager()->findResource(temp.c_str(), CIwGameXomlNames::Template_Hash);

		// Template cant be found so return error
		if (templ == NULL)
		{
			CIwGameError::LogError("Warning: XOML - Template does not exist in scene or globally - ",  temp.c_str());
			return false;
		}

		if (!templ->Instantiate(parent, node))
			return false;
	}

	IW_GAME_XOML->setExitOnError(false);
	return false;
}





