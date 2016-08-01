#include "GLCube.h"

//indices for drawing the edges of a cube with GL_LINES
unsigned short GLCube::indices[24] = {0,1,1,2,2,3,3,0,4,5,5,6,6,7,7,4,0,4,3,7,2,6,1,5};

//indices for drawing the faces of a cube with GL_TRIANGLES
//unsigned short GLCube::indices_triangles[36] = {0,1,2,2,3,0,0,3,4,4,7,3,0,1,4,4,5,1,
//    4,5,6,6,7,4,3,7,2,2,6,7,1,5,6,6,2,1};
////indices for drawing the faces of a cube with GL_TRIANGLES
unsigned short GLCube::indices_triangles[28] = {
    5,6,1,2,0,3,4,7,5,6,6,2,2,1,1,0,0,4,1,5,5,6,6,7,7,3,6,2};
	
void GLCube::Init(float x0, float y0,
                 float x1, float y1,
                 float x2, float y2,
                 float x3, float y3,
				 float zi, float z0, float _worldScale
              ){

GLMesh::Init(_worldScale);
num_indices = num_vertices  = 24;
num_triangle_indices = 28;

    
//set the vertices

vertices[0] = zi; vertices[1] = -x0; vertices[2] = y0;
vertices[3] = zi; vertices[4] = -x1; vertices[5] = y1;
vertices[6] = zi; vertices[7] = -x2; vertices[8] = y2;
vertices[9] = zi; vertices[10] = -x3; vertices[11] = y3;

vertices[12] = z0; vertices[13] = -x0; vertices[14] = y0;
vertices[15] = z0; vertices[16] = -x1; vertices[17] = y1;
vertices[18] = z0; vertices[19] = -x2; vertices[20] = y2;
vertices[21] = z0; vertices[22] = -x3; vertices[23] = y3;


verts[0].x = zi; verts[0].y = -x0; verts[0].z = y0;
verts[1].x = zi; verts[1].y = -x1; verts[1].z = y1;
verts[2].x = zi; verts[2].y = -x2; verts[2].z = y2;
verts[3].x = zi; verts[3].y = -x3; verts[3].z = y3;

verts[4].x = z0; verts[4].y = -x0; verts[4].z = y0;
verts[5].x = z0; verts[5].y = -x1; verts[5].z = y1;
verts[6].x = z0; verts[6].y = -x2; verts[6].z = y2;
verts[7].x = z0; verts[7].y = -x3; verts[7].z = y3;



//scale the vertice coordinates
for (int i=0 ; i<num_vertices;i++) vertices[i] /= worldScale;
for (int i=0 ; i<8;i++) {
	verts[i].x /= worldScale;
	verts[i].y /= worldScale;
	verts[i].z /= worldScale;

}
// set the addresses of the vertex / index arrays to be read by gl
//FirstVertex = &vertices[0];
FirstVertex = &verts[0];
FirstIndex = &indices[0];
FirstTriangleIndex = &indices_triangles[0];

}
void GLCube::Init(float x0, float y0,float z0,
                 float x1, float y1, float z1,
                 float x2, float y2, float z2,
                 float x3, float y3, float z3,
				 float x4, float y4, float z4,
				 float x5, float y5, float z5,
				 float x6, float y6, float z6,
				 float x7, float y7, float z7,
				 float _worldScale
              ){

GLMesh::Init(_worldScale);
num_indices = num_vertices  = 24;
num_triangle_indices = 28;

//set the vertices

vertices[0] = z0; vertices[1] = -x0; vertices[2] = y0;
vertices[3] = z1; vertices[4] = -x1; vertices[5] = y1;
vertices[6] = z2; vertices[7] = -x2; vertices[8] = y2;
vertices[9] = z3; vertices[10] = -x3; vertices[11] = y3;

vertices[12] = z4; vertices[13] = -x4; vertices[14] = y4;
vertices[15] = z5; vertices[16] = -x5; vertices[17] = y5;
vertices[18] = z6; vertices[19] = -x6; vertices[20] = y6;
vertices[21] = z7; vertices[22] = -x7; vertices[23] = y7;


verts[0].x = z0; verts[0].y = -x0; verts[0].z = y0;
verts[1].x = z1; verts[1].y = -x1; verts[1].z = y1;
verts[2].x = z2; verts[2].y = -x2; verts[2].z = y2;
verts[3].x = z3; verts[3].y = -x3; verts[3].z = y3;

verts[4].x = z4; verts[4].y = -x4; verts[4].z = y4;
verts[5].x = z5; verts[5].y = -x5; verts[5].z = y5;
verts[6].x = z6; verts[6].y = -x6; verts[6].z = y6;
verts[7].x = z7; verts[7].y = -x7; verts[7].z = y7;
    
//scale the vertice coordinates
for (int i=0 ; i<num_vertices;i++) vertices[i] /= worldScale;

for (int i=0 ; i<8;i++) {
	verts[i].x /= worldScale;
	verts[i].y /= worldScale;
	verts[i].z /= worldScale;

}


// set the addresses of the vertex / index arrays to be read by gl

//FirstVertex = &vertices[0];
FirstVertex = &verts[0];
FirstIndex = &indices[0];
FirstTriangleIndex = &indices_triangles[0];

//radii for towers, init to un-physical value
inner_radius = outer_radius = -1;
}


bool GLCube::Update(int dt){return GLMesh::Update(dt);};
void GLCube::Release(){};
void GLCube::Draw(){
	return GLMesh::Draw();
};