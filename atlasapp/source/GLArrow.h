
#include "GLMesh.h"
#include "math.h"
#include <iostream>

#ifndef GLARROW_h
#define GLARROW_h



/**@brief
  Defines the vertex and index buffers for a 2-D arrow (used for ptmiss).
 	 Geometry defined as:\n

	 1                .
          				 . .
  arrowHeadY      . . .
										.
										.
									  .
	 0                .         
 									<--->
								arrowWidthX
	*	For ptmiss, arrow points from center of coord. system in transverse planem, pointing at phi = atan(pxmiss,pymiss), tip of arrow extends beyond Muon systems.
 */
class GLArrow : public GLMesh{
public:
	GLArrow() {};

	virtual ~GLArrow(){};

    void Init(float arrowHeadY, float arrowWidthX, float worldScale);
    
	void arrowProportions(float arrowHeadY, float arrowWidthX, float scale);
	void scaleBy(float factor);
	void set(float etx, float ety);
	
	void Release(){};
	float maximumR;
	static unsigned short indices [14];
	static unsigned short indices_triangles [14];
	CIwFVec3 verts [6];
    
	void Draw();
    

};

#endif
