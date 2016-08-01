/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#if !defined(_GAME_SCENE_H_)
#define _GAME_SCENE_H_

#include "IwGameScene.h"
#include "IwGameActorParticles.h"

class CounterActor;
class FloaterActor;
class ConnectActor;

//
// Game Mode - Mode of play
//
enum eGameMode
{
	GAM_Normal		= 0,		// No additional frogs are added to play
};

//
// Counter colour
//
enum eCounterColour
{
	CC_Blue, 
	CC_Green, 
	CC_Orange, 
	CC_Purple, 
	CC_Red, 
	CC_Yellow, 
};

//
// Actor types - We use this type to determine which types of actor to spawn and collide with etc..
//
enum eActorTypes
{
	ACT_Inert		= 0,		// An inetr actor
	ACT_Counter		= 1,		// A counter actor
	ACT_Obstacle	= 2,		// An obstacle
};

//
//
// GameScene - Main game scene
//
//
class GameScene : public CIwGameScene
{
public:

protected:
	// Properties
	eGameMode			GameMode;				// Current game mode
	CounterActor*		LastSelectedCounter;	// Counter that was last selected by the player
	int					MaxCounters;			// Maximum number of counters in this scene
	int					Connecticons;			// Number of connecicons left
	float				ShootTimer;				// Used to time down shot
	bool				ShootTimerStarted;
	float				BonusTimer;				// Used as a player bbonus
public:
	void				setGameMode(eGameMode mode)				{ GameMode = mode; }
	eGameMode			getGameMode() const						{ return GameMode; }
	bool				isEndOfRound();
	void				setLastSelectedCounter(CounterActor* counter)	{ LastSelectedCounter = counter; }
	CounterActor*		getLastSelectedCounter()						{ return LastSelectedCounter; }
	int					getMaxCounters() const					{ return MaxCounters; }
	void				setMaxCounters(int count)				{ MaxCounters = count;  }
	void				setConnecticons(int count)				{ Connecticons = count; UpdateConnecticons(); }
	int					getConnecticons() const					{ return Connecticons; }
	bool				hasShootTimerStarted() const			{ return ShootTimerStarted; }
	void				startShootTimer()						{ ShootTimer = 0; ShootTimerStarted = true; ShowShootTimer(); }
	void				stopShootTimer()						{ ShootTimerStarted = false; }
	// Properties end

protected:
	bool				FirstFrame;
	bool				EndOfRound;
	int					PrevBonusTimer;

public:
	virtual int			Init(int max_collidables = 128, int max_layers = 10, bool doSleep = true);
	virtual void		Update(float dt);
	void				ResetLevel();

	// Adding new objects
	CIwGameActorParticles* AddExplosion(int num_patricles, float x, float y, float scale, float depth, int layer, float gravity);
	CIwGameActorParticles* AddStream(int num_particles, float x, float y, float scale, float depth, int layer, float gravity);
	FloaterActor*		AddScoreFloater(int score, float x, float y, float scale, float depth, int layer);
	ConnectActor*		AddConnector(CounterActor* actor1, CounterActor* actor2, float depth, int layer);

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	// End of round
	bool				CheckforEndOfRound();
	void				EndofRound();


	// HUD update
	void				UpdateScore();
	void				UpdateRecord();
	void				UpdateRoundNumber();
	void				UpdateConnecticons();
	void				UpdateTimer();
	void				ShowShootTimer();
	void				UpdateShootTimer();

	// Start simulation
	void				Shoot();
};

//
//  GameSceneCreator - Creates an instance of a game scene object
//
class GameSceneCreator : public IIwGameXomlClassCreator
{
public:
	GameSceneCreator()
	{
		setClassName("gamescene");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new GameScene(); }
};

#endif // _GAME_SCENE_H_
