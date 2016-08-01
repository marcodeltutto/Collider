



#if !defined(_GLMESH_H_)
#define _GLMESH_H_


#include <vector>
#include "IwGx.h"
//#include "IwGameXoml.h"
#include "CzXoml.h"

/**
@brief This is the base class of all 3-D objects drawn via the IwGxScene. The Init() and Draw() functions are also used by GLArrow, GLCube, GLDonut, GLCylinder and GLEndcap.
The API calls in the Draw function are basically the same as the OpenGL calls with similar names (in fact the OpenGL names are above since I tried using them originally). 
The rendering pipeline is a vector of CIwFVec3 points that define the object. The pipeline starts at FirstVertex and continues for num_vertices.

Two draw methods are used: GL_LINES (IW_GX_LINE_LIST) and GL_TRIANGLES (IW_GX_TRI_LIST).
GL_LINES takes a vector (or array) of integers that correspond to the points in the vertex vector and draws sequential pairs so 0,1,2,3 are connected with 0,1,1,2,2,3.
GL_TRIANGLES renders a surface using groups of three so 0,1,2, 2,3,4 etc.

These index lists are (usually) re-usable so are usually static arrays to save memory.

IwGxDrawPrims does the drawing given an iterable sequence of indices, vertices and the draw mode (GL_LINES or GL_TRIANGLES). The colour is set immediately before with RGBA values given to the function IwGxSetColAmbient.
 */
class GLMesh: public IzXomlResource{


public:
	//ctor and dtor
	GLMesh() : IzXomlResource() { setClassType("glmesh"); }
	virtual ~GLMesh(){};


	virtual void	Init(float _worldScale = 25, unsigned int _nameHash=0);
	virtual void	Release();
	virtual void	Draw(bool drawStrip = false, int nIndicesToDraw=0);
	bool			Update(int dt);
	int             LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node);


	//colors
	float  rgba[4];
	float * rgba_fromScheme;
	void setRGBA(float r, float g, float b, float a);
	void setRGBA(float colours[4]);
	void setRGBA(float * colourFirst,bool);

	float worldScale;
	//translation params
	float x,y,z, rx, ry, rz;
	int num_vertices, num_indices, num_triangle_indices;
	//float * FirstVertex;
	CIwFVec3 * FirstVertex;
	unsigned short * FirstIndex;
	unsigned short * FirstTriangleIndex;
	float * FirstColourIndex;
	float * FirstTriangleColourIndex;
	unsigned int nameHash;
	bool isDrawn;
	

};



class GLMeshCreator : public IzXomlClassCreator
{
public:
	GLMeshCreator()
	{
		setClassName("glmesh");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new GLMesh(); }
};
#endif




