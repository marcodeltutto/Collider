#include "GLCuboid.h"

//indices for drawing the edges of a Cuboid with GL_LINES
unsigned short GLCuboid::indices[24] = {0,1,1,2,2,3,3,0,4,5,5,6,6,7,7,4,0,4,3,7,2,6,1,5};

//indices for drawing the faces of a Cuboid with GL_TRIANGLES
unsigned short GLCuboid::indices_triangles[36] = {0,1,2,2,3,0,0,3,4,4,7,3,0,1,4,4,5,1,
												4,5,6,6,7,4,3,7,2,2,6,7,1,5,6,6,2,1};
	

void GLCuboid::Init(float x0, float y0,
                 float x1, float y1,
                 float x2, float y2,
                 float x3, float y3,
				 float zi, float z0, float _worldScale
              ){

GLMesh::Init(_worldScale);
num_indices = num_vertices  = 24;
num_triangle_indices = 36;

//set the vertices

vertices[0] = zi; vertices[1] = x0; vertices[2] = y0;
vertices[3] = zi; vertices[4] = x1; vertices[5] = y1;
vertices[6] = zi; vertices[7] = x2; vertices[8] = y2;
vertices[9] = zi; vertices[10] = x3; vertices[11] = y3;

vertices[12] = z0; vertices[13] = x0; vertices[14] = y0;
vertices[15] = z0; vertices[16] = x1; vertices[17] = y1;
vertices[18] = z0; vertices[19] = x2; vertices[20] = y2;
vertices[21] = z0; vertices[22] = x3; vertices[23] = y3;

/*
vertices[0] = x0; vertices[1] = y0; vertices[2] = zi;
vertices[3] = x0; vertices[4] = y0; vertices[5] = zi;
vertices[6] = x0; vertices[7] = y0; vertices[8] = zi;
vertices[9] = x0; vertices[10] = y0; vertices[11] = zi;

vertices[12] = x1; vertices[13] = y1; vertices[14] = z0;
vertices[15] = x1; vertices[16] = y1; vertices[17] = z0;
vertices[18] = x1; vertices[19] = y1; vertices[20] = z0;
vertices[21] = x1; vertices[22] = y1; vertices[23] = z0;
*/

//scale the vertice coordinates
for (int i=0 ; i<num_vertices;i++) vertices[i] /= worldScale;

// set the addresses of the vertex / index arrays to be read by gl
FirstVertex = &vertices[0];
FirstIndex = &indices[0];
FirstTriangleIndex = &indices_triangles[0];

}


bool GLCuboid::Update(int dt){return GLMesh::Update(dt);};
void GLCuboid::Release(){};
void GLCuboid::Draw(){
	return GLMesh::Draw();
};