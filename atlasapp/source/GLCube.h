#include "GLMesh.h"



#if !defined(_GLCUBE_H_)
#define _GLCUBE_H_


/** @brief
 Defines a cuboid for the rendering pipeline. Two init. methods; one has symmetry in z-plane, other specifies every point in cuboid (for calo-histogram bins). 
 */
class GLCube: public GLMesh{

public:
	GLCube() {};

	virtual ~GLCube(){};

	//cuboid
	void	Init(float x0, float y0,
                 float x1, float y1,
                 float x2, float y2,
                 float x3, float y3,
                 float zi, float zo, float _worldScale = 2000
				 );

	//parallelipiped
	void	Init(float x0, float y0,float z0,
                 float x1, float y1, float z1,
                 float x2, float y2, float z2,
                 float x3, float y3, float z3,
				 float x4, float y4, float z4,
				 float x5, float y5, float z5,
				 float x6, float y6, float z6,
				 float x7, float y7, float z7,
				 float _worldScale = 2000);
	void	Release();
	void	Draw();
	bool	Update(int dt);

	float vertices[24];
	static unsigned short indices[24];
	static unsigned short indices_triangles[28];
	//if drawn as a topo-cluster type tower, these are the inner / outer radii of the towers
	float inner_radius, outer_radius;
	CIwFVec3 verts[8];
};


class GLCubeCreator : public IzXomlClassCreator
{
public:
	GLCubeCreator()
	{
		setClassName("glcube");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new GLCube(); }
};


#endif
