#include "GLMesh.h"
#include <vector>


#if !defined(_GLDonut_H_)
#define _GLDonut_H_



/**@brief
 
 Almost identical to GLCylinder, except the shape is a toroid i.e. a cylinder with a finite width. Also the ends of this cylinder are also painted.
 */
class GLDonut: public GLMesh{

public:
	GLDonut() {
		vertices = NULL;
		indices = NULL;
		indices_triangles = NULL;
		verts = NULL;
		
	};

	virtual ~GLDonut(){
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


	void	Init(float zmin, float zmax, float radius_min, float radius_max, int nBasePoints = 20,
				 float _worldScale = 2000, unsigned int _nameHash =0);
	void	Release();
	void	Draw();
	bool	Update(int dt);

	
	std::vector<float> * vertices;
	std::vector<CIwFVec3> * verts;
	std::vector<unsigned short> * indices, * indices_triangles;
	
};


class GLDonutCreator : public IzXomlClassCreator
{
public:
	GLDonutCreator()
	{
		setClassName("gldonut");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new GLDonut(); }
};


#endif
