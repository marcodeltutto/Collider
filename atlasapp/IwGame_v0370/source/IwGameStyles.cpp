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

#include "IwGameStyles.h"
#include "IwGameScene.h"
#include "IwGameActor.h"
#include "IwGameResources.h"

//
//
//
//
// CIwGameStyle implementation
//
//
//
//
void CIwGameStyle::addProperty(CIwGameStyleProperty* prop)
{
	Properties.push_back(prop);
}

void CIwGameStyle::addProperty(const char* property_name, const char* property_value)
{
	CIwGameStyleProperty* prop = new CIwGameStyleProperty();
	prop->Name = property_name;
	prop->Value = property_value;
	Properties.push_back(prop);
}

void CIwGameStyle::clearProperties()
{
	for (_Iterator it = begin(); it != end(); ++it)
	{
		delete *it;
	}
	Properties.clear();
}

bool CIwGameStyle::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Parse Style attributes
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

	// Parse properties
	CIwGameString property_name;
	CIwGameString property_value;
	for (CIwGameXmlNode::_Iterator it2 = node->begin(); it2 != node->end(); ++it2)
	{
		unsigned int name_hash = (*it2)->GetName().getHash();
		if (name_hash == CIwGameXomlNames::Set_Hash)
		{
			// Get setter data
			for (CIwGameXmlNode::_AttribIterator it3 = (*it2)->attribs_begin(); it3 != (*it2)->attribs_end(); ++it3)
			{
				unsigned int attrib_hash = (*it3)->getName().getHash();

				if (attrib_hash == CIwGameXomlNames::Property_Hash)
					property_name = (*it3)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Value_Hash)
					property_value = (*it3)->GetValue();
			}
		}
		if (property_name.IsEmpty() || property_value.IsEmpty())
		{
			CIwGameError::LogError("Error: XOML - Invalid property name or value for Style - ", name.c_str());
			return false;
		}
		else
		{
			// Create and add the style property
			property_name.ToLower();
			addProperty(property_name.c_str(), property_value.c_str());
		}
	}

	// If we are declared inside a scene then shape is local to the scene
	CIwGameScene* scene = NULL;
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		scene = ((CIwGameActor*)parent)->getScene();
	else
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	if (scene != NULL)
		scene->getResourceManager()->addResource(this);
	else
		IW_GAME_GLOBAL_RESOURCES->getResourceManager()->addResource(this);

	return true;
}






