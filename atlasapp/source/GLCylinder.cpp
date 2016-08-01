#include "GLCylinder.h"




void GLCylinder::Init(float zmin, float zmax, float radius, int nBasePoints,
                      float _worldScale){
    
    GLMesh::Init(_worldScale);
    
    z_min = zmin;
    z_max = zmax;
    rad = radius;
    
    num_indices = num_vertices  = nBasePoints;
    
    //init the memory
    vertices = new std::vector<float>;
    verts = new std::vector<CIwFVec3>;
    indices = new std::vector<unsigned short>;
    indices_triangles = new std::vector<unsigned short>;
    
    //need multiple of 4 points to be drawn properly
    if (nBasePoints%2==1) nBasePoints+=1;
    
    vertices->reserve(8*nBasePoints +1);
    indices->reserve(8*nBasePoints  +10);
    indices_triangles->reserve(12*nBasePoints);
    verts->reserve(nBasePoints*2);
    
    //set the vertices
    float fPi = static_cast<float>(PI);
    //std::vector<float> x_pos, y_pos, z_pos;
    
    //define the vertices
    float depth_angle = 0;
    float x_pos, y_pos1, z_pos, y_pos2;
    
    //need even number of base points for face drawing
    
    for (int i=0;i <nBasePoints;i++){
        depth_angle = fPi*static_cast<float>(2)*static_cast<float>(i) / static_cast<float>(nBasePoints);
        z_pos = static_cast<float>(cos(depth_angle)) * radius;
        x_pos = static_cast<float>(sin(depth_angle)) * radius;
        y_pos1 = zmin;
        y_pos2 = zmax;
        
        //add vertices
        //z,x,y
        vertices->push_back(y_pos1);
        vertices->push_back(z_pos);
        vertices->push_back(x_pos);
        
        
        vertices->push_back(y_pos2);
        vertices->push_back(z_pos);
        vertices->push_back(x_pos);
        
        verts->push_back(CIwFVec3(y_pos1,z_pos,x_pos));
        verts->push_back(CIwFVec3(y_pos2,z_pos,x_pos));
    }
    
    //define the indices
    unsigned short n_points = static_cast<unsigned short>(vertices->size()/3);
    
    //wire frame indices
    for (unsigned short i=0; i< n_points-2; i++){
        indices->push_back(i);
        indices->push_back(i+2);
        
        //final connectors
        if (i==0){
            indices->push_back(i);
            indices->push_back(n_points-2);
        }
        if (i==1){
            indices->push_back(i);
            indices->push_back(n_points-1);
        }
        
        if (i%2==0){
            indices->push_back(i);
            indices->push_back(i+1);
        }
    }
    
    //triangle indices
    for (unsigned short i=0; i< n_points-3; i=i+1){
        indices_triangles->push_back(i);
        indices_triangles->push_back(i+1);
        indices_triangles->push_back(i+2);
        
        indices_triangles->push_back(i+2);
        indices_triangles->push_back(i+3);
        indices_triangles->push_back(i+1);
    }
    
    //last connection
    indices_triangles->push_back(0);
    indices_triangles->push_back(1);
    indices_triangles->push_back(n_points-1);
	
    indices_triangles->push_back(n_points-1);
    indices_triangles->push_back(n_points-2);
    indices_triangles->push_back(0);
	
    
    
    
    
    //scale the vertice coordinates
    for (unsigned int i=0 ; i<vertices->size();i++) vertices->at(i) /= worldScale;
    
    for (unsigned int i=0 ; i<verts->size();i++){
        verts->at(i).x /= worldScale;
        verts->at(i).y /= worldScale;
        verts->at(i).z /= worldScale;
    }
    
    
    // set the addresses of the vertex / index arrays to be read by gl
    //FirstVertex = &vertices->at(0);
    FirstVertex = &verts->at(0);
    FirstIndex = &indices->at(0);
    FirstTriangleIndex = &indices_triangles->at(0);
    
    num_vertices = vertices->size();
    num_indices = indices->size();
    num_triangle_indices = indices_triangles->size();
    
    //marco CIwGameError::LogError("!!glcylinder, nvert = ",CIwGameString(num_vertices).c_str());
    //marco CIwGameError::LogError("!!glcylinder, nind = ",CIwGameString(num_indices).c_str());
    //marco CIwGameError::LogError("!!glcylinder, nind tri= ",CIwGameString(num_triangle_indices).c_str());
}


bool GLCylinder::Update(int dt){
	delete vertices;
	delete indices;
	delete indices_triangles;
    
	return  GLMesh::Update(dt);
}

void GLCylinder::Release(){};
void GLCylinder::Draw(){
	return GLMesh::Draw();
};