


#include "GLMesh.h"
//#include "Tracks.h"
#ifdef _DEBUG
#include <iostream>
#endif


void GLMesh::setRGBA(float r, float g, float b, float a){
		rgba[0] = r;
		rgba[1] = g;
		rgba[2] = b;
		rgba[3] = a;
	}

void GLMesh::setRGBA(float colours[4]){
		rgba[0] = colours[0];
		rgba[1] = colours[1];
		rgba[2] = colours[2];
		rgba[3] = colours[3];
		rgba_fromScheme = &colours[0];
	}
void GLMesh::setRGBA(float * colourFirst,bool){
		
		rgba[0] = *colourFirst;
		rgba[1] = *(colourFirst+1);
		rgba[2] = *(colourFirst+2);
		rgba[3] = *(colourFirst+3);
		//std::cout <<"!!!!!!!!!!! setting colours"<<rgba[0] <<" "<<rgba[1] <<" "<<rgba[2] <<" "<<rgba[3] <<" \n";
	}
void GLMesh::Init(float _worldScale, unsigned int _nameHash){
	rgba[0] = rgba[1] = rgba[2] = rgba[3] = 1.0f;
	num_vertices = 0;
	num_indices = num_triangle_indices = 0;
	worldScale = _worldScale;
	x = y = z = 0;
	rx = ry = rz = 0;
	z = 0;
	FirstVertex = NULL;
	FirstIndex = NULL;
	FirstTriangleIndex = NULL;
    
	FirstColourIndex = NULL;
	FirstTriangleColourIndex = NULL;
	
	rgba_fromScheme = NULL;
	nameHash = _nameHash;
	isDrawn = true;
};

void GLMesh::Release(){


};

bool GLMesh::Update(int dt){
	return false;};

void GLMesh::Draw(bool drawStrip, int nIndicesToDraw){
	//are we skipping this mesh?
	if (!isDrawn) return;

	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnable (GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    if (!rgba_fromScheme){
        //glColor4f(rgba[0],rgba[1],rgba[2],rgba[3]);
        IwGxSetColAmbient(rgba[0],rgba[1],rgba[2],rgba[3]);
    }
    else{
        //glColor4f(*rgba_fromScheme,*(rgba_fromScheme+1),*(rgba_fromScheme+2),*(rgba_fromScheme+3));
        IwGxSetColAmbient(*rgba_fromScheme,*(rgba_fromScheme+1),*(rgba_fromScheme+2),*(rgba_fromScheme+3));
    }
	
	if (FirstVertex){ //glVertexPointer(3, GL_FLOAT, 0, FirstVertex);
		if (drawStrip) IwGxSetVertStream(FirstVertex,nIndicesToDraw);
		else IwGxSetVertStream(FirstVertex,num_indices);
	}


	if (FirstTriangleIndex){ 
	
		//glDrawElements(GL_TRIANGLES, num_triangle_indices, GL_UNSIGNED_SHORT,FirstTriangleIndex);
		//IwGxSetVertStream(FirstVertex,num_triangle_indices);
		IwGxDrawPrims(IW_GX_TRI_STRIP, FirstTriangleIndex, num_triangle_indices);
		//IwGxDrawPrims(IW_GX_LINE_LIST, FirstTriangleIndex, num_triangle_indices);
		//glColor4f(1,1,1,1);
        
		//IwGxSetColAmbient(255,255,255,255);
	}
	
	
	//glLineWidth(1.5);
	//if (FirstIndex)glDrawElements(GL_LINES, num_indices, GL_UNSIGNED_SHORT,FirstIndex);
	if (FirstIndex){
		//line list
		if (!drawStrip) IwGxDrawPrims(IW_GX_LINE_LIST, FirstIndex, num_indices);
		//strip (for tracks)
		else {
            IwGxDrawPrims(IW_GX_LINE_STRIP, FirstIndex, nIndicesToDraw);
        }

	}
    

//    //draw an axis
//    uint16 l[2] =
//    {
//        0,1,
//    };
//    
//    IwGxSetColAmbient(255,255,255,255);
//    CIwFVec3 xs[2] = {
//        CIwFVec3(0, 0, 0),
//        CIwFVec3(50, 0, 0),
//    };
//    IwGxSetVertStream(xs, 2);
//    IwGxDrawPrims(IW_GX_LINE_STRIP, l, 2);
//    CIwFVec3 ys[2] = {
//        CIwFVec3(0, 0, 0),
//        CIwFVec3(0, -50, 0),
//    };
//    IwGxSetVertStream(ys, 2);
//    IwGxDrawPrims(IW_GX_LINE_STRIP, l, 2);
//    CIwFVec3 zs[2] = {
//        CIwFVec3(0, 0, 0),
//        CIwFVec3(0, 0, 50),
//    };
//    IwGxSetVertStream(zs, 2);
//    IwGxDrawPrims(IW_GX_LINE_STRIP, l, 2);
//    IwGxFlush();

	//glDisable(GL_BLEND);
	//glDisableClientState(GL_VERTEX_ARRAY);

};

int GLMesh::LoadFromXoml(IzXomlResource* parent, bool load_children, CzXmlNode* node){

	// Process InertActor specific attributes
	for (CzXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

	}
	return true;

};






