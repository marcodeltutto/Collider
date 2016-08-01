
#include "GLMesh.h"
#include "math.h"
#include <iostream>

#ifndef GLCONE_h
#define GLCONE_h



/**@brief
 Defines the vertex and index buffers for a 3D cone (used for electrons, photons and jets).
  */
class GLCone : public GLMesh{
public:
	GLCone() {
		vertices = NULL;
		indices = NULL;
		indices_triangles = NULL;
		verts = NULL;
	};
    
	virtual ~GLCone(){
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
    
    void Init(float et, float eta, float phi, float z0, int type, int nBasePoints = 20,
              float _worldScale = 2000);
	
    void	Release();
	void	Draw();
	bool	Update(int dt);
    
	std::vector<float> * vertices;
	std::vector<CIwFVec3> * verts;
	std::vector<unsigned short> * indices, * indices_triangles;
    
};

#endif
