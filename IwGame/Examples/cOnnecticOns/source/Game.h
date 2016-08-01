/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

#if !defined(_GAME_H_)
#define _GAME_H_

#include "IwGame.h"

#define	GAME	Game::getInstance()

#define	MAX_EDITOR_LEVELS	20
#define	MAX_RECORDS			120

//
//
// Game is a singleton class that acts as a controller for the whole game
// 
// 
//
class Game : public CIwGame
{
	CDEFINE_SINGLETON(Game)

public:

protected:
	// Properties
	int				CurrentZone;						// Current zone
	int				CurrentRound;						// Current in play
	int				CurrentScore;						// Players current score
	int				Records[MAX_RECORDS];				// Record scores per round
	bool			Completed[MAX_RECORDS];				// Completed state of each round
	int				MaxZones;							// Maximum zones

public:
	void			setCurrentZone(int zone)		{ CurrentZone = zone; }
	int				getCurrentZone() const			{ return CurrentZone; }
	void			setCurrentRound(int round)		{ CurrentRound = round; }
	int				getCurrentRound() const			{ return CurrentRound; }
	void			setCurrentScore(int score);
	int				getCurrentScore() const			{ return CurrentScore; }
	bool			setRecord(int index, int score);
	int				getRecord(int index) const		{ return Records[index]; }
	void			resetRecords();
	bool			setCompleted(int index, bool completed);
	int				isCompleted(int index) const	{ return Completed[index]; }
	bool			isZoneComplete(int zone) const;
	bool			isAllZonesComplete() const;
	void			setMaxZones(int count)			{ MaxZones = count; }
	int				getMaxZones() const				{ return MaxZones; }
	// Properties end

protected:
	bool			PrevTouched;
	void			LoadResources();

public:
	void			Init(bool enable_http);
	void			Release();
	bool			Update();
	void			Draw();
	void			SaveRecords();
	void			LoadRecords();
	void			SaveCompleted();
	void			LoadCompleted();

	// Implementation of IIwGameXomlResource interface
	bool				LoadFromXoml(bool load_children, CIwGameXmlNode* node);
	
	// Ad engine test
	void			AdTest_Init();
	void			AdTest_Release();
	void			AdTest_Update();

	// Actions
	void			StartRound(int round);

};

//
//
//
//
//  GameXomlAction_Global - A collection of global game specific actions
//
//
//
//
class GameXomlAction_Global : public IIwGameXomlAction
{
public:
	enum ActionType
	{
		Action_StartRound		= 0, 
		Action_NextRound		= 1, 
		Action_Shoot			= 2, 
		Action_ResetRecords		= 3, 
		Action_PostFacebook		= 4, 
		Action_Max				= 5, 
	};
protected:
	ActionType	Type;
	GameXomlAction_Global() {}
public:
	GameXomlAction_Global(ActionType type)
	{
		Type = type;
		switch (Type)
		{
		case Action_StartRound:
			setActionName("startround");
			break;
		case Action_NextRound:
			setActionName("nextround");
			break;
		case Action_Shoot:
			setActionName("shoot");
			break;
		case Action_ResetRecords:
			setActionName("resetrecords");
			break;
		case Action_PostFacebook:
			setActionName("postfacebook");
			break;
		}
	}
	void Execute(IIwGameXomlResource* source, CIwGameAction* action);
};


#endif // _GAME_H_
