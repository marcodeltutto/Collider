#include "GLMesh.h"
#include <vector>


#if !defined(_GLEndcap_H_)
#define _GLEndcap_H_


/**@brief
 Defines an annulus shape for event-display rendering pipeline, used to define the ID Endcap detectors. 
 */
class GLEndcap: public GLMesh{

public:
	GLEndcap() {
		vertices = NULL;
		indices = NULL;
		indices_triangles = NULL;
		verts = NULL;
		
	};

	virtual ~GLEndcap(){
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
				 float _worldScale = 2000, unsigned int _nameHash=0);
	void	Release();
	void	Draw();
	bool	Update(int dt);
	

	
	std::vector<float> * vertices;
	std::vector<CIwFVec3> * verts;
	std::vector<unsigned short> * indices, * indices_triangles;
	
};


class GLEndcapCreator : public IzXomlClassCreator
{
public:
	GLEndcapCreator()
	{
		setClassName("glendcap");
	}
	IzXomlResource*	CreateInstance(IzXomlResource* parent)	{ return new GLEndcap(); }
};


#endif
