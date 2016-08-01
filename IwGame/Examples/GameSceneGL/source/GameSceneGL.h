#if !defined(_GAME_SCENE_GL_H_)
#define _GAME_SCENE_GL_H_

#include "IwGameScene.h"

//
//
// GameSceneGL - Open GL compatible game scene
//
//
class GameSceneGL : public CIwGameScene
{
public:

protected:
	// Properties
public:
	// Properties end

protected:

public:
	int				Init(int max_collidables = 128, int max_layers = 10, bool doSleep = true);
	void			Update(float dt);
	void			Draw();

	// Implementation of IIwGameXomlClass interface
	bool			LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

};

//
//  GameSceneGLCreator - Creates an instance of a game scene GL object
//
class GameSceneGLCreator : public IIwGameXomlClassCreator
{
public:
	GameSceneGLCreator()
	{
		setClassName("gamescenegl");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new GameSceneGL(); }
};

#endif // _GAME_SCENE_H_
