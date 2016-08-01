#include "GLArrow.h"


//unsigned short GLArrow::indices[7] = {0,1,2,3,4,5,6};
//unsigned short GLArrow::indices[14] = {0,1,1,2,2,3,3,4,4,5,5,6,6,0};
//unsigned short GLArrow::indices_triangles[6] = {0,1,5,2,3,4};
unsigned short GLArrow::indices[14] = {0,1,1,2,2,3,3,4,4,5,5,0,0,0};
unsigned short GLArrow::indices_triangles[14] = {0,1,1,2,2,3,3,4,4,5,5,0,0,0};

void GLArrow::Init(float arrowHeadY, float arrowWidthX, float worldScale){
    
    GLMesh::Init(worldScale);
    
    num_vertices = 6;
    maximumR = 800.0;
    
    //set proportions
    arrowProportions(arrowHeadY,arrowWidthX,maximumR);
    
    
    for (int i=0 ; i<num_vertices;i++) {
        verts[i].x /= worldScale;
        verts[i].y /= worldScale;
        verts[i].z /= worldScale;
    }
    
    
    FirstVertex = &verts[0];
    FirstIndex = &indices[0];
    FirstTriangleIndex = &indices_triangles[0];
    num_indices = 14;
    num_triangle_indices = 14;
}

void GLArrow::arrowProportions(float arrowHeadY, float arrowWidthX, float scale){
    verts[0] = CIwFVec3(0,0,0);
    verts[1] = CIwFVec3(0,-0.75*arrowHeadY,arrowWidthX/2);
    verts[2] = CIwFVec3(0,-0.75*arrowHeadY,arrowWidthX);
    verts[3] = CIwFVec3(0,-arrowHeadY,0);
    verts[4] = CIwFVec3(0,-0.75*arrowHeadY,-arrowWidthX);
    verts[5] = CIwFVec3(0,-0.75*arrowHeadY,-arrowWidthX/2);
    
//    verts[0] = CIwFVec3(0,0,0);
//    verts[1] = CIwFVec3(0,-arrowHeadY,arrowWidthX/2);
//    verts[2] = CIwFVec3(0,-arrowHeadY,arrowWidthX);
//    verts[3] = CIwFVec3(0,-1.0,0);
//    verts[4] = CIwFVec3(0,-arrowHeadY,-arrowWidthX);
//    verts[5] = CIwFVec3(0,-arrowHeadY,-arrowWidthX/2);
    //verts[6] = CIwFVec3(0,0,0);
    scaleBy(scale);
    
}

void GLArrow::scaleBy(float factor){
    for (int iVert=0;iVert < num_vertices; iVert++){
        verts[iVert].x *= factor;
        verts[iVert].y *= factor;
        verts[iVert].z *= factor;
    }
}

void GLArrow::set(float etx, float ety){
    float phi = atan2f(ety,etx);
    float ptmiss = sqrtf(etx*etx + ety*ety);
    //reset arrow-ness based on ptmiss
    float arrowWidth = (ptmiss*ptmiss)/(50*50);
    if(arrowWidth > 1.1)
        arrowWidth = 1.1;
    //std::cout << "arrowWidth = " << arrowWidth << std::endl;
    //define how fat the arrow is
    arrowProportions(arrowWidth,arrowWidth/4.0,maximumR/worldScale);
    //scale coordinates to fit the world
    //scaleBy(1.0/worldScale);
    //std::cout <<"setting arrow proportions, phi = "<<phi<<"<< ptmiss = "<<ptmiss<<" etx = "<<etx <<" ety = "<< ety<<std::endl;
    //orient coordinates along phi
    
    float cPhi = cosf(phi);
    float sPhi = sinf(phi);
    for (int iVert=0;iVert < num_vertices;iVert++){

        float oz = verts[iVert].z;
        float oy = verts[iVert].y;
        
        verts[iVert].z = oz*cPhi - oy*sPhi;
        verts[iVert].y = oz*sPhi + oy*cPhi;
    }
    //std::cout << "done arrow" << std::endl;
}

void GLArrow::Draw(){
    GLMesh::Draw();
}