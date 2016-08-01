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

// TODO:
// * Add support for gradient brush

#include "IwGameBrush.h"
#include "IwGameString.h"
#include "IwGameScene.h"
#include "IwGameResources.h"
#include "IwGameActor.h"

bool IIwGameBrush::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Get brush data
	eIwGameBrushType	type = BT_Solid;
	CIwFVec4			colour;
	CIwGameString*		name = NULL;
	CIwGameString*		image_name = NULL;
	CIwRect				rect = CIwRect(0, 0, 0, 0);
	CIwRect				scale_area = CIwRect(0, 0, 0, 0);
	CIwGameString*		condition = NULL;

	CIwGameScene* scene = NULL;
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		scene = ((CIwGameActor*)parent)->getScene();
	else
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); ++it)
	{
		unsigned int attrib_hash = (*it)->getName().getHash();

		if (attrib_hash == CIwGameXomlNames::Name_Hash)
		{
			name = &(*it)->GetValue();
		}
		else
		if (attrib_hash == CIwGameXomlNames::Type_Hash)
		{
			unsigned int type_hash = (*it)->GetValue().getHash();
			if (type_hash == CIwGameXomlNames::Solid_Hash)
				type = BT_Solid;
			else
			if (type_hash == CIwGameXomlNames::Gradient_Hash)
				type = BT_Gradient;
			else
			if (type_hash == CIwGameXomlNames::Image_Hash)
				type = BT_Image;
			else
			if (type_hash == CIwGameXomlNames::Patch9_Hash)
				type = BT_9Patch;
			else
			{
				CIwGameError::LogError("Error: XOML - Invalid brush type - ", (*it)->GetValue().c_str());
				return false;
			}
		}
		else
		if (attrib_hash == CIwGameXomlNames::Colour_Hash)
		{
			if (!(*it)->GetValueAsPoint4(colour))
				CIwGameError::LogError("Warning: XOML - Brush Colour should be a vec4");
		}
		else
		if (attrib_hash == CIwGameXomlNames::Image_Hash)
		{
			image_name = &(*it)->GetValue();
			type = BT_Image;
		}
		else
		if (attrib_hash == CIwGameXomlNames::SrcRect_Hash)
		{
			if (!(*it)->GetValueAsRect(rect))
				CIwGameError::LogError("Warning: XOML - Brush SrcRect should be a rect");
		}
		else
		if (attrib_hash == CIwGameXomlNames::Tag_Hash)
		{
			setTag((*it)->GetValue().c_str());
		}
		else
		if (attrib_hash == CIwGameXomlNames::ScaleArea_Hash)
		{
			if (!(*it)->GetValueAsRect(scale_area))
				CIwGameError::LogError("Warning: XOML - Brush ScaleArea should be a rect");
		}
		else
		if (attrib_hash == CIwGameXomlNames::Condition_Hash)
		{
			condition = &(*it)->GetValue();
		}
	}

	if (condition != NULL)
	{
		// Find the condition variable
		bool condition_not = false;
		CIwGameXomlVariable* var = NULL;
		if (*(condition->c_str()) == '!')
		{
			condition_not = true;
			CIwGameString cond = condition->c_str() + 1;
			var = CIwGameXomlVariable::GetVariable(cond, scene);
		}
		else
			var = CIwGameXomlVariable::GetVariable(*condition, scene);
		if (var != NULL)
		{
			bool res = var->isTrue();
			if (condition_not)
				res = !res;
			if (!res)
			{
				IW_GAME_XOML->setExitOnError(false);
				return false;
			}
		}
#if defined (_DEBUG)
		else
			CIwGameError::LogError("Warning: condition variable not found - ", condition->c_str());
#endif // _DEBUG
	}

	IIwGameBrush* brush = NULL;
	switch (type)
	{
	case BT_Solid:
		{
			CIwGameBrushSolid* b = new CIwGameBrushSolid();
			b->setColour((uint8)colour.x, (uint8)colour.y, (uint8)colour.z, (uint8)colour.w);
			brush = b;
		}
		break;
	case BT_Gradient:
		{
		}
		break;
	case BT_Image:
	case BT_9Patch:
		{
			CIwGameImage* image = NULL;

			if (image_name == NULL)
			{
				CIwGameError::LogError("Error: An image / patch9 brush requires an image to be specified");
				return false;
			}
	
			if (scene != NULL)
			{
				image = (CIwGameImage*)scene->getResourceManager()->findResource(image_name->getHash(), CIwGameXomlNames::Image_Hash);
				if (image == NULL)
					CIwGameError::LogError("Warning: XOML - Brush - Could not find brush image - ", image_name->c_str());
			}
			else
			{
				image = (CIwGameImage*)IW_GAME_GLOBAL_RESOURCES->getResourceManager()->findResource(image_name->getHash(), CIwGameXomlNames::Image_Hash);
				if (image == NULL)
					CIwGameError::LogError("Warning: XOML - Brush - Could not find brush image - ", image_name->c_str());
			}

			if (image != NULL)
			{
				if (type == BT_Image)
				{
					CIwGameBrushImage* b = new CIwGameBrushImage();
					b->setImage(image);
					b->setSrcRect(rect);
					brush = b;
				}
				else
				if (type == BT_9Patch)
				{
					CIwGameBrushImage9* b = new CIwGameBrushImage9();
					b->setImage(image);
					b->setSrcRect(rect);
					b->setScalableArea(scale_area);
					brush = b;
				}
			}
		}
		break;
	}

	if (brush != NULL)
	{
		brush->setName(name->c_str());
		brush->setBrushType(type);

		// If we are declared inside a scene then material is local to the scene
		if (scene != NULL)
			scene->getResourceManager()->addResource(brush);
		else
			IW_GAME_GLOBAL_RESOURCES->getResourceManager()->addResource(brush);
	}

	IW_GAME_XOML->setExitOnError(false);
	return false;
}






