#include "GLMesh.h"



#if !defined(_GLCuboid_H_)
#define _GLCuboid_H_



class GLCuboid: public GLMesh{

public:
	GLCuboid() {};

	virtual ~GLCuboid(){};


	void	Init(float x0, float y0,
                 float x1, float y1,
                 float x2, float y2,
                 float x3, float y3,
                 float zi, float zo, float _worldScale = 2000
				 );
	void	Release();
	void	Draw();
	bool	Update(int dt);

	float vertices[24];
	static unsigned short indices[24];
	static unsigned short indices_triangles[36];

};


class GLCuboidCreator : public IIwGameXomlClassCreator
{
public:
	GLCuboidCreator()
	{
		setClassName("glCuboid");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new GLCuboid(); }
};


#endif