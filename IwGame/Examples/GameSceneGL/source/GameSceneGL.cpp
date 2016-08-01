#include "GameSceneGL.h"

#include "IwGameCamera.h"
#include "IwGameInput.h"
#include "IwGame.h"

//
//
//
// GameSceneGL implementation
//
//
//

int GameSceneGL::Init(int max_collidables, int max_layers, bool doSLeep)
{
	CIwGameScene::Init(max_collidables, max_layers, doSLeep);

	return 1;
}

void GameSceneGL::Update(float dt)
{
	CIwGameScene::Update(dt);

}

void GameSceneGL::Draw()
{
	IwGxFlush();

	int w = IwGxGetScreenWidth();
	int h = IwGxGetScreenHeight();

	// GL render test
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glScalef(1, -1, 1);
	glTranslatef(0, -h, 0);

	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);

	GLfloat vertices[] = {1,0,0, 0,1,0, -1,0,0};
	GLfloat colours[] = {1,0,0,1, 0,1,0,1, 0,0,1,1};
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(1, -1, 1);
	glTranslatef(0, -h, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, &colours);
	glVertexPointer(3, GL_FLOAT, 0, &vertices);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glMatrixMode(GL_TEXTURE);
	glPopMatrix();

	CIwGameScene::Draw();
}


bool GameSceneGL::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Load the actor from XOML
	if (!CIwGameScene::LoadFromXoml(parent, load_children, node))
		return false;

	// Process InertActor specific attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

	}

	return true;
}

