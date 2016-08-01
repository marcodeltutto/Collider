#include "GLDonut.h"
#include <cmath>
#include <iostream>



void GLDonut::Init(float zmin, float zmax, float radius_min, float radius_max, int nBasePoints,
                   float _worldScale, unsigned int _nameHash){
    
    GLMesh::Init(_worldScale, _nameHash);
    num_indices = num_vertices  = nBasePoints;
    
    //init the memory
    vertices = new std::vector<float>;
    verts = new std::vector<CIwFVec3>;
    
    indices = new std::vector<unsigned short>;
    indices_triangles = new std::vector<unsigned short>;
    
    //need multiple of 4 points to be drawn properly
    if (nBasePoints%2==1) nBasePoints+=1;
//    
//    vertices->reserve(8*nBasePoints +1);
//    indices->reserve(8*nBasePoints  +10);
//    indices_triangles->reserve(12*nBasePoints);

    vertices->reserve(12*(nBasePoints+1));
    verts->reserve(4*(nBasePoints+1));
    indices_triangles->reserve(8*(nBasePoints+1));
    
    num_indices = 4*(nBasePoints+1);
    
    //verts->reserve(2*nBasePoints);
    //set the vertices
    float fPi = static_cast<float>(PI);
    //std::vector<float> x_pos, y_pos, z_pos;
    
    //define the vertices
//    float depth_angle = 0;
//    static float x_pos1, y_pos1, z_pos1, y_pos2, x_pos2, z_pos2;
    
    //CIwFVec3 outvs[nBasePoints+1]; marco's modification
    //CIwFVec3 invs[nBasePoints+1];
    //CIwFVec3 outvs2[nBasePoints+1];
    //CIwFVec3 invs2[nBasePoints+1];
  
      CIwFVec3 * outvs  = new CIwFVec3[nBasePoints+1];
      CIwFVec3 * invs   = new CIwFVec3[nBasePoints+1];
      CIwFVec3 * outvs2 = new CIwFVec3[nBasePoints+1];
      CIwFVec3 * invs2  = new CIwFVec3[nBasePoints+1];
  
  
    // calculate the points for the outer circle and inner circle
    // do this twice, separated by z
    for(int i=0; i<nBasePoints+1; ++i) {
        float angle = 2*fPi*(i/(float)nBasePoints);
        float x = sin(angle)*radius_max;
        float y = -1*cos(angle)*radius_max;
        float inx = sin(angle)*radius_min;
        float iny = -1*cos(angle)*radius_min;
        outvs[i] = CIwFVec3(zmax,x,y);
        invs[i] = CIwFVec3(zmax,inx,iny);
        outvs2[i] = CIwFVec3(zmin,x,y);
        invs2[i] = CIwFVec3(zmin,inx,iny);
    }
    
    for(int i=0; i<nBasePoints+1; ++i) {
        vertices->push_back(outvs[i].y);
        vertices->push_back(outvs[i].z);
        vertices->push_back(outvs[i].x);
        verts->push_back(outvs[i]);
    }
    for(int i=0; i<nBasePoints+1; ++i) {
        vertices->push_back(invs[i].y);
        vertices->push_back(invs[i].z);
        vertices->push_back(invs[i].x);
        verts->push_back(invs[i]);
    }
    for(int i=0; i<nBasePoints+1; ++i) {
        vertices->push_back(outvs2[i].y);
        vertices->push_back(outvs2[i].z);
        vertices->push_back(outvs2[i].x);
        verts->push_back(outvs2[i]);
    }
    for(int i=0; i<nBasePoints+1; ++i) {
        vertices->push_back(invs2[i].y);
        vertices->push_back(invs2[i].z);
        vertices->push_back(invs2[i].x);
        verts->push_back(invs2[i]);
    }
    
    for(int i=0; i<nBasePoints+1; ++i) {
        
        indices_triangles->push_back(i+nBasePoints+1);
        indices_triangles->push_back(i);
//        
//        verts[i*2] = invs[i];
//        verts[(i*2)+1] = outvs[i];
    }
    for(int i=0; i<nBasePoints+1; ++i) {
        
        indices_triangles->push_back(i);
        indices_triangles->push_back(i+(2*(nBasePoints+1)));
//        
//        verts[ioffset+(i*2)] = outvs[i];
//        verts[ioffset+((i*2)+1)] = outvs2[i];
    }
    for(int i=0; i<nBasePoints+1; ++i) {
        
        indices_triangles->push_back(i+(2*(nBasePoints+1)));
        indices_triangles->push_back(i+(3*(nBasePoints+1)));
//        
//        verts[ioffset+(i*2)] = outvs2[i];
//        verts[ioffset+((i*2)+1)] = invs2[i];
    }
    for(int i=0; i<nBasePoints+1; ++i) {
        
        indices_triangles->push_back(i+(3*(nBasePoints+1)));
        indices_triangles->push_back(i+nBasePoints+1);
//        verts[ioffset+(i*2)] = invs2[i];
//        verts[ioffset+((i*2)+1)] = invs[i];
    }

    
    
    
    
    //need even number of base points for face drawing
    //
    //for (int i=0;i <nBasePoints;i++){
    //	depth_angle = fPi*static_cast<float>(2)*static_cast<float>(i) / static_cast<float>(nBasePoints);
    //
    //	z_pos1 = cosf(depth_angle) * radius_min;
    //	x_pos1 = sinf(depth_angle) * radius_min;
    //
    //	z_pos2 = cosf(depth_angle) * radius_max;
    //	x_pos2 = sinf(depth_angle) * radius_max;
    //
    //	y_pos1 = zmin;
    //	y_pos2 = zmax;
    //
    //	//add vertices
    //	//z,x,y
    //	vertices->push_back(y_pos1);
    //	vertices->push_back(z_pos1);
    //	vertices->push_back(x_pos1);
    //	
    //	vertices->push_back(y_pos1);
    //	vertices->push_back(z_pos2);
    //	vertices->push_back(x_pos2);
    //	
    //	vertices->push_back(y_pos2);
    //	vertices->push_back(z_pos1);
    //	vertices->push_back(x_pos1);
    //	
    //	vertices->push_back(y_pos2);
    //	vertices->push_back(z_pos2);
    //	vertices->push_back(x_pos2);
    //	
    //	verts->push_back(CIwFVec3(y_pos1,z_pos1,x_pos1));
    //	verts->push_back(CIwFVec3(y_pos1,z_pos2,x_pos2));
    //	verts->push_back(CIwFVec3(y_pos2,z_pos1,x_pos1));
    //	verts->push_back(CIwFVec3(y_pos2,z_pos2,x_pos2));
    //
    //
    //}
    //
    ////define the indices
    //unsigned short n_points = static_cast<unsigned short>(vertices->size()/3);
    //
    ////wire frame indices
    ///*for (unsigned short i=0; i< n_points-2; i++){
    //	indices->push_back(i);
    //	indices->push_back(i+2);
    //
    //	//final connectors
    //	if (i==0){
    //		indices->push_back(i);
    //		indices->push_back(n_points-2);
    //	}
    //	if (i==1){
    //		indices->push_back(i);
    //		indices->push_back(n_points-1);
    //	}
    //	
    //	if (i%2==0){
    //		indices->push_back(i);
    //		indices->push_back(i+1);
    //	}
    //}
    //*/
    //for (unsigned short i=0; i< n_points-4; i++){
    //	  //connect two base points
    //            indices->push_back((short) i);
    //            indices->push_back((short) (i + 4));
    //
    //            ///connections
    //            if (i % 4 == 0) {
    //                indices->push_back((short) i);
    //                indices->push_back((short) (i + 4));
    //
    //                indices->push_back((short) i);
    //                indices->push_back((short) (i + 1));
    //
    //                indices->push_back((short) (i + 2));
    //                indices->push_back((short) (i + 3));
    //
    //                indices->push_back((short) i);
    //                indices->push_back((short) (i + 2));
    //
    //                indices->push_back((short) (i + 1));
    //                indices->push_back((short) (i + 3));
    //
    //
    //            }
    //
    //}
    //
    ////triangle indices
    //for (unsigned short i=0; i< n_points-7; i=i+4){
    //	//one flat chord
    //	indices_triangles->push_back(i);
    //	indices_triangles->push_back(i+1);
    //	indices_triangles->push_back(i+5);
    //	
    //	indices_triangles->push_back(i+5);
    //	indices_triangles->push_back(i+4);
    //	indices_triangles->push_back(i);
    //	
    //	//another chord
    //	indices_triangles->push_back(i+2);
    //	indices_triangles->push_back(i+3);
    //	indices_triangles->push_back(i+7);
    //	
    //	indices_triangles->push_back(i+2);
    //	indices_triangles->push_back(i+7);
    //	indices_triangles->push_back(i+6);
    //	
    //	//outer and inner edges
    //	indices_triangles->push_back(i+1);
    //	indices_triangles->push_back(i+3);
    //	indices_triangles->push_back(i+5);
    //	
    //	indices_triangles->push_back(i+3);
    //	indices_triangles->push_back(i+7);
    //	indices_triangles->push_back(i+5);
    //	
    //	
    //	indices_triangles->push_back(i);
    //	indices_triangles->push_back(i+2);
    //	indices_triangles->push_back(i+6);
    //	
    //	indices_triangles->push_back(i);
    //	indices_triangles->push_back(i+4);
    //	indices_triangles->push_back(i+6);
    //	
    //}
    ////one flat chord
    //
    //
    //
    //	
    //	indices_triangles->push_back(n_points-4);
    //	indices_triangles->push_back(n_points-3);
    //	indices_triangles->push_back(+1);
    //	
    //	indices_triangles->push_back(+1);
    //	indices_triangles->push_back(0);
    //	indices_triangles->push_back(n_points-4);
    //	
    //	//another chord
    //	indices_triangles->push_back(n_points-2);
    //	indices_triangles->push_back(n_points-1);
    //	indices_triangles->push_back(+3);
    //	
    //	indices_triangles->push_back(n_points-2);
    //	indices_triangles->push_back(+3);
    //	indices_triangles->push_back(+2);
    //
    //	//last outer and iner section
    //	indices_triangles->push_back(n_points-3);
    //	indices_triangles->push_back(n_points-1);
    //	indices_triangles->push_back(+1);
    //	
    //	indices_triangles->push_back(n_points-1);
    //	indices_triangles->push_back(+3);
    //	indices_triangles->push_back(+1);
    //	
    //	
    //	indices_triangles->push_back(n_points-4);
    //	indices_triangles->push_back(n_points-2);
    //	indices_triangles->push_back(+2);
    //	
    //	indices_triangles->push_back(n_points-4);
    //	indices_triangles->push_back(0);
    //	indices_triangles->push_back(+2);
    //	
    ////last lines
    //		
    //
    //
    //				n_points = n_points-1;
    //				indices->push_back((short)n_points-3);
    //                indices->push_back((short) (0));
    //
    //                indices->push_back((short) n_points-2);
    //                indices->push_back((short) (1));
    //
    //                indices->push_back((short) (n_points));
    //                indices->push_back((short) (3));
    //
    //                indices->push_back((short) n_points-1);
    //                indices->push_back((short) (2));
    //
    //                indices->push_back((short) (n_points-3));
    //                indices->push_back((short) (n_points-1));
    //
    //				indices->push_back((short) (n_points-2));
    //                indices->push_back((short) (n_points));
    //
    //				indices->push_back((short) (n_points-3));
    //                indices->push_back((short) (n_points-2));
    //
    //				indices->push_back((short) (n_points));
    //                indices->push_back((short) (n_points-1));
    //
    //
    //
    //
    //
    ////scale the vertice coordinates
    for (unsigned int i=0 ; i<vertices->size();i++) vertices->at(i) /= worldScale;
    for (unsigned int i=0 ; i<verts->size();i++){ 
    	verts->at(i).x /= worldScale;
    	verts->at(i).y /= worldScale;
    	verts->at(i).z /= worldScale;
    }
    
    // set the addresses of the vertex / index arrays to be read by gl
    //FirstVertex = &vertices->at(0);
    FirstVertex = &verts->at(0);
    //FirstIndex = &indices->at(0);
    FirstTriangleIndex = &indices_triangles->at(0);
    
    num_vertices = vertices->size();
    //num_indices = indices->size();
    num_triangle_indices = indices_triangles->size();
    
    //CIwGameError::LogError("!!glDonut, nvert = ",CIwGameString(num_vertices).c_str());
    //CIwGameError::LogError("!!glDonut, nind = ",CIwGameString(num_indices).c_str());
    //CIwGameError::LogError("!!glDonut, nind tri= ",CIwGameString(num_triangle_indices).c_str());
}


bool GLDonut::Update(int dt){
	delete vertices;
	//delete indices;
	delete indices_triangles;
	delete verts;
    
	return  GLMesh::Update(dt);
}

void GLDonut::Release(){};
void GLDonut::Draw(){
	return GLMesh::Draw();
};