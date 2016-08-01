#include "GLEndcap.h"




void GLEndcap::Init(float zmin, float zmax, float radius_min, float radius_max, int nBasePoints,
				 float _worldScale, unsigned int _nameHash){

GLMesh::Init(_worldScale,_nameHash);
num_indices = num_vertices  = nBasePoints;

//init the memory
vertices = new std::vector<float>;
indices = new std::vector<unsigned short>;
indices_triangles = new std::vector<unsigned short>;
verts = new std::vector<CIwFVec3>;
//need multiple of 4 points to be drawn properly
if (nBasePoints%2==1) nBasePoints+=1;

    vertices->reserve(6*(nBasePoints+1));
    verts->reserve(2*(nBasePoints+1));
    indices_triangles->reserve(4*(nBasePoints+1));
    
    //verts->reserve(2*nBasePoints);
    //set the vertices
    float fPi = static_cast<float>(PI);
    //std::vector<float> x_pos, y_pos, z_pos;
    
    //define the vertices
    //    float depth_angle = 0;
    //    static float x_pos1, y_pos1, z_pos1, y_pos2, x_pos2, z_pos2;
    
    // CIwFVec3 outvs[nBasePoints+1];  marco's modification
    // CIwFVec3 invs[nBasePoints+1];
  
    CIwFVec3 * outvs = new CIwFVec3[nBasePoints+1];
    CIwFVec3 * invs = new CIwFVec3[nBasePoints+1];
  
    // calculate the points for the outer circle and inner circle
    // do this twice, separated by z
    for(int i=0; i<nBasePoints+1; ++i) {
        float angle = 2*fPi*(i/(float)nBasePoints);
        float x = sin(angle)*radius_max;
        float y = -1*cos(angle)*radius_max;
        float inx = sin(angle)*radius_min;
        float iny = -1*cos(angle)*radius_min;
        float z = (zmax+zmin)/2.0;
        outvs[i] = CIwFVec3(z,x,y);
        invs[i] = CIwFVec3(z,inx,iny);
//        outvs2[i] = CIwFVec3(zmin,x,y);
//        invs2[i] = CIwFVec3(zmin,inx,iny);
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
//    for(int i=0; i<nBasePoints+1; ++i) {
//        vertices->push_back(outvs2[i].y);
//        vertices->push_back(outvs2[i].z);
//        vertices->push_back(outvs2[i].x);
//        verts->push_back(outvs2[i]);
//    }
//    for(int i=0; i<nBasePoints+1; ++i) {
//        vertices->push_back(invs2[i].y);
//        vertices->push_back(invs2[i].z);
//        vertices->push_back(invs2[i].x);
//        verts->push_back(invs2[i]);
//    }
    
    for(int i=0; i<nBasePoints+1; ++i) {
        
        indices_triangles->push_back(i+nBasePoints+1);
        indices_triangles->push_back(i);
        //        
        //        verts[i*2] = invs[i];
        //        verts[(i*2)+1] = outvs[i];
    }
//    for(int i=0; i<nBasePoints+1; ++i) {
//        
//        indices_triangles->push_back(i);
//        indices_triangles->push_back(i+(2*(nBasePoints+1)));
//        //        
//        //        verts[ioffset+(i*2)] = outvs[i];
//        //        verts[ioffset+((i*2)+1)] = outvs2[i];
//    }
    for(int i=0; i<nBasePoints+1; ++i) {
        
        indices_triangles->push_back(i);
        indices_triangles->push_back(i+(nBasePoints+1));
        //        
        //        verts[ioffset+(i*2)] = outvs2[i];
        //        verts[ioffset+((i*2)+1)] = invs2[i];
    }
//    for(int i=0; i<nBasePoints+1; ++i) {
//        
//        indices_triangles->push_back(i+(3*(nBasePoints+1)));
//        indices_triangles->push_back(i+nBasePoints+1);
//        //        verts[ioffset+(i*2)] = invs2[i];
//        //        verts[ioffset+((i*2)+1)] = invs[i];
//    }
    

    
//vertices->reserve(8*nBasePoints +1);
//indices->reserve(8*nBasePoints  +10);
//indices_triangles->reserve(12*nBasePoints);
//verts->reserve(2*nBasePoints);
////set the vertices
//float fPi = static_cast<float>(PI);
////std::vector<float> x_pos, y_pos, z_pos;
//
////define the vertices
//float depth_angle = 0;
//static float x_pos1, y_pos1, z_pos1, y_pos2, x_pos2, z_pos2;
//float z_ave = static_cast<float>((zmin + zmax)/2.0);
//
////need even number of base points for face drawing
//
//for (int i=0;i <nBasePoints;i++){
//	depth_angle = fPi*static_cast<float>(2)*static_cast<float>(i) / static_cast<float>(nBasePoints);
//
//	z_pos1 = static_cast<float>(cos(depth_angle)) * radius_min;
//	x_pos1 = static_cast<float>(sin(depth_angle)) * radius_min;
//
//	z_pos2 = static_cast<float>(cos(depth_angle)) * radius_max;
//	x_pos2 = static_cast<float>(sin(depth_angle)) * radius_max;
//
//
//	y_pos1 = z_ave;
//	//y_pos2 = zmax;
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
//	verts->push_back(CIwFVec3(y_pos1,z_pos1,x_pos1));
//	verts->push_back(CIwFVec3(y_pos1,z_pos2,x_pos2));
//
//}
//
////define the indices
//unsigned short n_points = static_cast<unsigned short>(vertices->size()/3);
//
////wire frame indices
//
// for (unsigned short i = 0; i < n_points - 2; i++) {
//            //connect two base points
//            indices->push_back((unsigned short) i);
//            indices->push_back((unsigned short) (i + 2));
//            //connect different discs
//
//            
//                //connect the first and last base points
//                if (i == 0) {
//                    indices->push_back((unsigned short) i);
//                    indices->push_back((unsigned short) ((n_points - 2)));
//                }
//                if (i == 1) {
//                    indices->push_back((unsigned short) 1);
//                    indices->push_back((unsigned short) ((n_points - 1)));
//                }
//            
//            if (i % 2 == 0) {
//                indices->push_back((unsigned short) i);
//                indices->push_back((unsigned short) (i + 1));
//            }
//
//        }
//
//
//
////triangle indices
//for (unsigned short i=0; i< n_points-2; i=i+2){
//	indices_triangles->push_back((unsigned short)i);
//	indices_triangles->push_back((unsigned short)(i+1));
//	indices_triangles->push_back((unsigned short)(i+2));	
//
//	indices_triangles->push_back((unsigned short)(i+3));
//	indices_triangles->push_back((unsigned short)(i+1));
//	indices_triangles->push_back((unsigned short)(i+2));	
//}
//
//	indices_triangles->push_back((unsigned short)(n_points-2));
//	indices_triangles->push_back((unsigned short)(n_points-1));
//	indices_triangles->push_back((unsigned short)(0));	
//
//	indices_triangles->push_back((unsigned short)(0));
//	indices_triangles->push_back((unsigned short)(1));
//	indices_triangles->push_back((unsigned short)(n_points-2));
//
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
//FirstIndex = &indices->at(0);
FirstTriangleIndex = &indices_triangles->at(0);

num_vertices = vertices->size();
//num_indices = indices->size();
num_triangle_indices = indices_triangles->size();

//CIwGameError::LogError("!!glEndcap, nvert = ",CIwGameString(num_vertices).c_str());
//CIwGameError::LogError("!!glEndcap, nind = ",CIwGameString(num_indices).c_str());
//CIwGameError::LogError("!!glEndcap, nind tri= ",CIwGameString(num_triangle_indices).c_str());
}


bool GLEndcap::Update(int dt){
	delete vertices;
	delete indices;
	delete indices_triangles;
	delete verts;

	return  GLMesh::Update(dt);
}

void GLEndcap::Release(){};
void GLEndcap::Draw(){
	return GLMesh::Draw();
};