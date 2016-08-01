#include "GLMesh.h"
#include <vector>


#if !defined(_GLCYLINDER_H_)
#define _GLCYLINDER_H_


/**@brief
 Defines an open ended cylinder for the event-display rendering pipeline, used to draw ID barrel components. Assumes symmetry in Z plane. Roundness of the `circular' endges is defines by nBasePoints; the higher this is the more points are generated (and hence more round). Same draw method as GLCube (inherited from GLMesh). Since the vertex buffer length is in principal dynamic, vectors are used for the vertices and indices.
 */
class GLCylinder: public GLMesh{

public:
	GLCylinder() {
		vertices = NULL;
		indices = NULL;
		indices_triangles = NULL;
		verts = NULL;
	};

	virtual ~GLCylinder(){
		if (vertices){
			vertices->clear();
			delete vertices;
		}
		if (verts){
			verts->clear();
			delete verts;
		}
		if (indices){
			indices->clear();
			delete indices;
		}
		if (indices_triangles){
			indices_triangles->clear();
			delete indices_triangles;
		}

	};


	void	Init(float zmin, float zmax, float radius, int nBasePoints = 20,
				 float _worldScale = 2000);
	void	Release();
	void	Draw();
	bool	Update(int dt);

	
	std::vector<float> * vertices;
	std::vector<CIwFVec3> * verts;
	std::vector<unsigned short> * indices, * indices_triangles;
    
    float z_min;
    float z_max;
    float rad;
};


class GLCylinderCreator : public IzXomlClassCreator
{
public:
	GLCylinderCreator()
	{
		setClassName("glcylinder");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new GLCylinder(); }
};


#endif
