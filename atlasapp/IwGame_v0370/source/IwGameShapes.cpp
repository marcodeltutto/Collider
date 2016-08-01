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

#include "IwGameShapes.h"
#include "IwGameUtil.h"
#include "IwGameXoml.h"
#include "IwGameScene.h"
#include "IwGameActor.h"


//
//
//
//
// CIwGameShape implementation
//
//
//
//

bool CIwGameShape::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Process Actor attributes
	float	width = 0, height = 0, radius = 0;
	CIwGameGeomShape::eIwGameGeomShapeType type = CIwGameGeomShape::ST_None;

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
		{
			setName((*it)->GetValue().c_str());
		}
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Type_Hash)
		{
			unsigned int type_hash = CIwGameString::CalculateHash((*it)->GetValue().c_str());
			if (type_hash == CIwGameXomlNames::box_Hash)
				type = CIwGameGeomShape::ST_Box;
			else
			if (type_hash == CIwGameXomlNames::circle_Hash)
				type = CIwGameGeomShape::ST_Circle;
			else
			if (type_hash == CIwGameXomlNames::polygon_Hash)
				type = CIwGameGeomShape::ST_Polygon;
		}
		else
		if (name_hash == CIwGameXomlNames::Width_Hash)
		{
			width = (*it)->GetValueAsFloat();
		}
		else
		if (name_hash == CIwGameXomlNames::Height_Hash)
		{
			height = (*it)->GetValueAsFloat();
		}
		else
		if (name_hash == CIwGameXomlNames::Radius_Hash)
		{
			radius = (*it)->GetValueAsFloat();
		}
	}

	if (type == CIwGameGeomShape::ST_None)
	{
		CIwGameError::LogError("Warning: XOML - Invalid shape type");
		return true;
	}
	else
	if (type == CIwGameGeomShape::ST_Box)
	{
		CIwGameGeomShapeBox* shape = new CIwGameGeomShapeBox();
		shape->ShapeType = type;
		shape->Width = width;
		shape->Height = height;
		Shape = shape;
	}
	else
	if (type == CIwGameGeomShape::ST_Circle)
	{
		CIwGameGeomShapeCircle* shape = new CIwGameGeomShapeCircle();
		shape->ShapeType = type;
		shape->Radius = radius;
		Shape = shape;
	}
	else
	if (type == CIwGameGeomShape::ST_Polygon)
	{
		CIwGameGeomShapePolygon* shape = new CIwGameGeomShapePolygon();
		shape->ShapeType = type;
		Shape = shape;

		// Calculate number of points
		int num_points = node->Children.size();
		if (num_points == 0)
		{
			CIwGameError::LogError("Warning: XOML - Polygon shape has no points");
			return true;
		}
		shape->Vertices = new CIwFVec2[num_points]();
		shape->NumVertices = num_points;

		// Prrocess polygon points
		int index = 0;
		for (CIwGameXmlNode::_Iterator it2 = node->begin(); it2 != node->end(); ++it2)
		{
			unsigned int name_hash = (*it2)->GetName().getHash();
			if (name_hash == CIwGameXomlNames::Point_Hash)
			{
				// Get points data
				for (CIwGameXmlNode::_AttribIterator it = (*it2)->attribs_begin(); it != (*it2)->attribs_end(); ++it)
				{
					unsigned int attrib_hash = (*it)->getName().getHash();

					if (attrib_hash == CIwGameXomlNames::Value_Hash)
					{
						if (!(*it)->GetValueAsPoint(*(shape->Vertices + index)))
						{
							CIwGameError::LogError("Warning: XOML - Invalid value for point SHape::Polygon");
							return true;
						}
						break;
					}
				}
				index++;
			}
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

