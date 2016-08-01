#include <vector>



#if !defined(_MESHGROUP_H_)
#define _MESHGROUP_H_

#include "GLMesh.h"
/**@brief
 This class contains a vector to pointers of GLMesh objects and an OpenGL style matrix used when drawn.
 In IwGxScene there are two MeshGroup instances; one to store the detector meshes and one for the event meshes.
 
 Note that GLMesh instances created dynamically in the *fromXML classes are freed in the destructor of this class,
 since they are always appended to a MeshGroup children vector.

 The Draw function simply iterates over the children vector and calls GLMesh::Draw() of each element of the vector.
 */
class MeshGroup{
public:
	virtual ~MeshGroup(){
#ifdef _DEBUG
		CIwGameError::LogError("Entering MeshGroup destructor");
#endif
		if (children==NULL) return;
		
		for (children_It = children->begin(); children_It != children->end(); children_It++){
			delete *children_It;
		}

		children->clear();

		delete children;
		// marco CIwGameError::LogError("Leaving destructor");
	};

	MeshGroup(){
		children=NULL;
	};
	
	

	std::vector<GLMesh*> * children;
	std::vector<GLMesh*>::iterator children_It;
	
	float worldScale;
	//translation & rotation params
	float x,y,z, rx, ry, rz;
	CIwFVec3 pos, rot;
	CIwFMat rotX, rotY, rotZ;
	void Init();
	void Release();
	bool Update(int dt);
	void Draw();
	void UpdateMeshesToDraw(unsigned int targetName, bool isDrawn);
	


};
#endif //_MESHGROUP_H
