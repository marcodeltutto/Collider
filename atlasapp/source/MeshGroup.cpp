#include <vector>
#include "MeshGroup.h"




void MeshGroup::Init(){

	worldScale = 5000;
	x = y = z = 0;
	rx = ry = rz = 0;
	pos = CIwFVec3(x,y,z);
	rot = CIwFVec3(rx,ry,rz);
	children = new std::vector<GLMesh*>;
	//z = 0.5;
	
}

void MeshGroup::Release(){
	if (children){
		for (children_It = children->begin(); children_It != children->end(); children_It++)
			(*children_It)->Release();

		delete children;
	}
}
void MeshGroup::Draw(){

	for (children_It = children->begin(); children_It != children->end(); children_It++){
	
		(*children_It)->Draw();
		
	}

}
bool MeshGroup::Update(int dt){
	//rx +=0.5;
	//ry +=0.5;
	
	
	for (children_It = children->begin(); children_It != children->end(); children_It++)
		(*children_It)->Update(dt);
	return true;

}

void MeshGroup::UpdateMeshesToDraw(unsigned int targetNameHash, bool isDrawn){
	
	for (children_It = children->begin(); children_It != children->end(); children_It++){
		//is this mesh a target
		if ((*children_It)->nameHash == targetNameHash)
			(*children_It)->isDrawn = isDrawn;
	}
		 
	return;


}