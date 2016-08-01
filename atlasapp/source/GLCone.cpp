//
//  GLCone.cpp
//  GameSceneGL
//
//  Created by Tom McLaughlan on 20/05/2013.
//
//

#include "GLCone.h"




void GLCone::Init(float et, float eta, float phi, float z0, int type, int nBasePoints, float _worldScale){
    
    GLMesh::Init(_worldScale);

    num_indices = num_vertices  = nBasePoints;
    
    //init the memory
    vertices = new std::vector<float>;
    verts = new std::vector<CIwFVec3>;
    indices = new std::vector<unsigned short>;
    indices_triangles = new std::vector<unsigned short>;
    
    /*
    
                       *
                     *   *            ***
     *              *  *  *           ***
                     *   *            ***
                       *
     
     
     
     */
    vertices->reserve(3*(2*(nBasePoints+1)));
    indices_triangles->reserve((4*(nBasePoints))+3);
    verts->reserve(2*(nBasePoints+1));
    indices->reserve((4*nBasePoints));
    
    //set the vertices
    float fPi = static_cast<float>(PI);
    float angle = 0;
    
    float dR = 0.2*180.0;
    if(type==3)
        dR = 0.4*180.0;
    
    float x_pos = 0.0;
    float y_pos = 0.0;
    float z_pos = 0.0;
    /*
    vertices->push_back(x_pos);
    vertices->push_back(y_pos);
    vertices->push_back(z_pos);
    
    verts->push_back(CIwFVec3(x_pos,y_pos,z_pos));
    */
    
    x_pos = 100 * (1+(fabsf(eta)));
    for (int i=0;i <nBasePoints;i++){
        angle = fPi*static_cast<float>(2)*static_cast<float>(i) / static_cast<float>(nBasePoints);
        y_pos = static_cast<float>(cos(angle)) * dR;
        z_pos = static_cast<float>(sin(angle)) * dR;

        //add vertices
        //z,x,y
        vertices->push_back(x_pos);
        vertices->push_back(y_pos);
        vertices->push_back(z_pos);
        
        verts->push_back(CIwFVec3(x_pos,y_pos,z_pos));
    }
    
    z_pos = 0.0;
    y_pos = 0.0;
    
    vertices->push_back(x_pos);
    vertices->push_back(y_pos);
    vertices->push_back(z_pos);
    
    verts->push_back(CIwFVec3(x_pos,y_pos,z_pos));
    
    
    x_pos = 40 * (1+(fabsf(eta)));
    for (int i=0;i <nBasePoints;i++){
        angle = fPi*static_cast<float>(2)*static_cast<float>(i) / static_cast<float>(nBasePoints);
        y_pos = static_cast<float>(cos(angle)) * 0.5 * dR;
        z_pos = static_cast<float>(sin(angle)) * 0.5 * dR;
        
        //add vertices
        //z,x,y
        vertices->push_back(x_pos);
        vertices->push_back(y_pos);
        vertices->push_back(z_pos);
        
        verts->push_back(CIwFVec3(x_pos,y_pos,z_pos));
    }
    
    z_pos = 0.0;
    y_pos = 0.0;
    
    vertices->push_back(x_pos);
    vertices->push_back(y_pos);
    vertices->push_back(z_pos);
    
    verts->push_back(CIwFVec3(x_pos,y_pos,z_pos));
    
    for(unsigned int i=0; i<nBasePoints-1; ++i) {
        indices->push_back(i);
        indices->push_back(i+1);
    }
    
    indices->push_back(0);
    indices->push_back(nBasePoints-1);
    
    
    for(unsigned int i=0; i<nBasePoints-1; ++i) {
        indices->push_back(nBasePoints+1+i);
        indices->push_back(nBasePoints+1+i+1);
    }
    
    indices->push_back(nBasePoints+1);
    indices->push_back(nBasePoints+1+nBasePoints-1);
    
    
    //triangle indices
    //0,0,0 is first
    //0,1,0 is last
//    for(unsigned int i=1; i<=nBasePoints; ++i) {
//        indices_triangles->push_back(0);
//        indices_triangles->push_back(i);
//    }
//    indices_triangles->push_back(0);
//    indices_triangles->push_back(1);
//    indices_triangles->push_back(1);
//    
//    for(unsigned int i=1; i<=nBasePoints; ++i) {
//        indices_triangles->push_back(nBasePoints+1);
//        indices_triangles->push_back(i);
//    }
//    indices_triangles->push_back(nBasePoints+1);
//    indices_triangles->push_back(1);
    
    for(unsigned int i=0; i<nBasePoints; ++i) {
        indices_triangles->push_back(nBasePoints);
        indices_triangles->push_back(i);
    }
    indices_triangles->push_back(nBasePoints);
    indices_triangles->push_back(0);
    
    for(unsigned int i=0; i<nBasePoints; ++i) {
        indices_triangles->push_back(i);
        indices_triangles->push_back(nBasePoints+i+1);
    }
    indices_triangles->push_back(0);
    indices_triangles->push_back(nBasePoints+1);

    for(unsigned int i=0; i<nBasePoints; ++i) {
        indices_triangles->push_back(nBasePoints+1+i);
        indices_triangles->push_back((2*nBasePoints)+1);
    }
    indices_triangles->push_back(nBasePoints+1);
    indices_triangles->push_back((2*nBasePoints)+1);
    
    
    float theta;// = atan(1/eta); // this one is for cotTheta (for testing versus tracks...)
    theta = 2*atan(exp(-eta));
    //phi = phi;// + fPi;
    
    CIwFMat mat = CIwFMat();
    mat.SetIdentity();
    mat.PreRotateZ(theta);
    mat.PostRotateX(phi);
    // now rotate the cone into place
    //    float cPhi = cosf(phi);
    //    float sPhi = sinf(phi);
    for (int iVert=0;iVert < verts->size();iVert++){
        
        verts->at(iVert) = mat.RotateVec(verts->at(iVert));
        verts->at(iVert).x += z0;
        
    }
    
    
    //scale the vertice coordinates
    for (unsigned int i=0 ; i<vertices->size();i++) vertices->at(i) /= worldScale;
    
    for (unsigned int i=0 ; i<verts->size();i++){
        verts->at(i).x /= worldScale;
        verts->at(i).y /= worldScale;
        verts->at(i).z /= worldScale;
    }
    
    // set the addresses of the vertex / index arrays to be read by gl
    FirstIndex = &indices->at(0);
    FirstVertex = &verts->at(0);
    FirstTriangleIndex = &indices_triangles->at(0);
    
    num_indices = indices->size();
    num_vertices = vertices->size();
    num_triangle_indices = indices_triangles->size();
    
}

bool GLCone::Update(int dt){
	delete vertices;
	delete indices;
	delete indices_triangles;
	delete verts;
    
	return  GLMesh::Update(dt);
}

void GLCone::Release(){};
void GLCone::Draw(){
	return GLMesh::Draw();
};