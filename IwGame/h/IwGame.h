// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. You must ensure that this whole notice is present in all files and derivatives, so the reader knows its origin.
// If you use this engine in your product then please ensure that you credit IwGame's usage appropriately. Please see http://www.drmop.com/index.php/iwgame-engine/ for licensing details and support
//
//

#if !defined(_IW_GAME_H_)
#define _IW_GAME_H_

// Version 0.400
#define	IW_GAME_VERSION		0x00400

#include "IwGameUtil.h"
#include "IwGameActor.h"
#include "IwGameScene.h"
#include "IwGameCamera.h"
#include "IwGameInput.h"
#include "IwGameAudio.h"
#include "IwGameXoml.h"
#include "IwGameResources.h"

#define	IW_GAME_SYS_SCREEN_WIDTH			0
#define	IW_GAME_SYS_SCREEN_HEIGHT			1
#define	IW_GAME_SYS_SCREEN_MODE				2	// Graphics mode, see IwGameUtils.cpp for list of modes
#define	IW_GAME_SYS_SCREEN_SIZE_HINT		3	// Screen size hint (banded using (width + height) / 400 calculation to give rough idea of what size components such as fonts to use)
#define	IW_GAME_SYS_DEVICE_TYPE				4	// Device type, see IwGameUtils.h for list of types
#define	IW_GAME_SYS_HAS_MULTITOUCH			5	// Supports multi-touch
#define	IW_GAME_SYS_HAS_ACCELEROMETER		6	// Supports accelerometer
#define	IW_GAME_SYS_HAS_COMPASS				7	// Supports compass
#define	IW_GAME_SYS_HAS_KEYS				8	// Supports keys
#define	IW_GAME_SYS_HAS_POINTER				9	// Supports pointer
#define	IW_GAME_SYS_MAX						10

#define	IW_GAME_TOUCHES_POS_X0				0	// Touch x-position 0
#define	IW_GAME_TOUCHES_POS_Y0				1	// Touch y-position 0
#define	IW_GAME_TOUCHES_STATE0				2	// Touch state 0
#define	IW_GAME_TOUCHES_POS_X1				3	// Touch x-position 0
#define	IW_GAME_TOUCHES_POS_Y1				4	// Touch y-position 0
#define	IW_GAME_TOUCHES_STATE1				5	// Touch state 1
#define	IW_GAME_TOUCHES_POS_X2				6	// Touch x-position 0
#define	IW_GAME_TOUCHES_POS_Y2				7	// Touch y-position 0
#define	IW_GAME_TOUCHES_STATE2				8	// Touch state 2
#define	IW_GAME_TOUCHES_POS_X3				9	// Touch x-position 0
#define	IW_GAME_TOUCHES_POS_Y3				10	// Touch y-position 0
#define	IW_GAME_TOUCHES_STATE3				11	// Touch state 3
#define	IW_GAME_TOUCHES_POS_X4				12	// Touch x-position 0
#define	IW_GAME_TOUCHES_POS_Y4				13	// Touch y-position 0
#define	IW_GAME_TOUCHES_STATE4				14	// Touch state 4
#define	IW_GAME_TOUCHES_MAX					15


#define IW_GAME_MAX_SCENE_LAYERS			10

class CIwGameUIBase;

//
//
//
//
// CIwGame is a acts as a controller for the whole game. You should derive your own version of this class
// 
//
// 
//
class CIwGame : public IIwGameXomlResource
{
public:
	// Public access for scene iteration
	typedef CIwList<CIwGameScene*>::iterator	_Iterator;
	_Iterator					begin() { return Scenes.begin(); }
	_Iterator					end() { return Scenes.end(); }

protected:
	//// Properties
	CIwVec2						ScreenSize;								// Native screen size
	CIwGameScene*				CurrentScene;							// Scene that has current input focus
	CIwGameScene*				NextScene;								// Scene that we wish to switch focus to
	CIwList<CIwGameScene*>		Scenes;									// A collection of game scenes
	CIwGameActor*				TouchFocus[IW_GAME_MAX_TOUCHES];		// Actor that has the current touch focus
	CIwGameActor*				LastTappedActor[IW_GAME_MAX_TOUCHES];	// Actor that was last tapped
	CIwGameUIBase*				KeyFocus;								// Actor that has the current key focus
	CIwColour					BackgroundColour;						// Background clear colour
	bool						TouchPanEnabled;						// Global touch pan enabled
	int							MaxTouches;								// Maximum number of tuoches to support
	bool						GLCompatible;							// Determine if this game should be compatible with OpenGL commands

public:
	void						addScene(CIwGameScene *scene, bool bring_to_front = true);
	void						removeScene(CIwGameScene* scene);
	void						removeScene(unsigned int name_hash);
	void						removeAllScenes(CIwGameScene* exclude_scene);
	void						removeAllScenes(unsigned int exclude_name_hash);
	CIwGameScene*				findScene(unsigned int name_hash);
	CIwGameScene*				findScene(const char* name);
	CIwGameScene*				findScene(int type);
	CIwGameScene*				getScene(int index);
	void						clearScenes();
	void						changeScene(CIwGameScene *new_scene);
	bool						changeScene(unsigned int name_hash);
	CIwGameScene*				getCurrentScene()						{ return CurrentScene; }
	void						setTouchFocus(int index, CIwGameActor* focus)	{ TouchFocus[index] = focus; }
	CIwGameActor*				getTouchFocus(int index)				{ return TouchFocus[index]; }
	void						releaseTouchFocus(int index, int x, int y, bool allow_bubble = true);
	CIwGameActor*				getLastTappedActor(int index)			{ return LastTappedActor[index]; }
	void						setKeyFocus(CIwGameUIBase* actor)		{ KeyFocus = actor; }
	CIwGameUIBase*				getKeyFocus()							{ return KeyFocus; }
	void						setTouchPanEnabled(bool enabled)		{ TouchPanEnabled = enabled; }
	bool						isTouchPanEnabled() const				{ return TouchPanEnabled; }
	void						setMaxTouches(int max)					{ MaxTouches = max; }
	int							getMaxTouches() const					{ return MaxTouches; }
	void						setGLCompatible(bool enable)			{ GLCompatible = enable; }
	bool						getGLCompatible() const					{ return GLCompatible; }
	//// Properties end

protected:
	void						CreateSystemArray();	// Creates an array that contains system variables that user can access from XOML
	CIwList<CIwGameScene*>		SceneRemovals;			// Scenes that were removed during this game frame
	uint64						LastFrameTime;			// The time at which the last frame ended
	void						ProcessTouch(CIwGameScene *scene, int touch);
	void						ProcessEvents(CIwGameScene *scane);
	CIwGameActor*				FindTappedActor(int x, int y);
	void						UpdateSystemTouches();
	void						GetAppEasySettings();
	CIwGameSlotArray<CIwGameScene*>	SceneList;				// Temp scene list, stored here to provent many allocs / frees

public:
	virtual void				Init(bool enable_http = false, bool enable_extensions = true, bool gl_conmpatible = false);
	virtual void				Release();
	virtual bool				Update();
	virtual void				Draw();
	virtual void				PostDraw();			// Called by Draw() after all rendering is finished, but before the frame bugger is swapped
	virtual void				Save() {}
	virtual void				Load() {}

public:
	void						SetBackgroundColour(uint8 r, uint8 g, uint8 b, uint8 a);
	void						BringSceneToFront(CIwGameScene* scene);
	void						DisableFocus();										// Disables focus for all scenes
	void						SetAllTimelines(CIwGameAnimTimeline* timeline);		// Sets the timelines of all active scenes

	// Implementation of IIwGameXomlClass interface
	bool						LoadFromXoml(IIwGameXomlResource* parebt, bool load_children, CIwGameXmlNode* node);
};

class CIwGameCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameCreator()
	{
		setClassName("game");
	}
	IIwGameXomlResource* CreateInstance(IIwGameXomlResource* parent);
};


//
//
//
//
//  CIwGameXomlAction_Global - A collection of global actions
//
//
//
//
class CIwGameXomlAction_Global : public IIwGameXomlAction
{
public:
	enum ActionType
	{
		Action_PlayTimeline		= 0, 
		Action_StopTimeline		= 1, 
		Action_SetTimeline		= 2, 
		Action_PlaySound		= 3, 
		Action_PlayMusic		= 4, 
		Action_StopMusic		= 5, 
		Action_LoadXOML			= 6, 
		Action_FromTemplate		= 7, 
		Action_SetVariable		= 8, 
		Action_AddVariable		= 9, 
		Action_Launch			= 10, 
		Action_SetBGColour		= 11, 
		Action_SetCurrentScene	= 12, 
		Action_BringtSceneToFront= 13, 
		Action_EnterValue		= 14, 
		Action_UpdateText		= 15, 
		Action_Exit				= 16, 
		Action_RemoveResource	= 17, 
		Action_RemoveResources	= 18, 
		Action_ChangeProgram	= 19, 
		Action_DebugText		= 20, 
		Action_LoadFile			= 21, 
		Action_PauseTimeline	= 22, 
		Action_CallGlobalScript	= 23, 
		Action_ChangeVideoCam	= 24, 
		Action_Max				= 25, 
	};
protected:
	ActionType	Type;
	CIwGameXomlAction_Global() {}
public:
	CIwGameXomlAction_Global(ActionType type)
	{
		Type = type;
		switch (Type)
		{
		case Action_PlayTimeline:
			setActionName("playtimeline");
			break;
		case Action_StopTimeline:
			setActionName("stoptimeline");
			break;
		case Action_SetTimeline:
			setActionName("settimeline");
			break;
		case Action_PauseTimeline:
			setActionName("pausetimeline");
			break;
		case Action_PlaySound:
			setActionName("playsound");
			break;
		case Action_PlayMusic:
			setActionName("playmusic");
			break;
		case Action_StopMusic:
			setActionName("stopmusic");
			break;
		case Action_LoadXOML:
			setActionName("loadxoml");
			break;
		case Action_FromTemplate:
			setActionName("fromtemplate");
			break;
		case Action_SetVariable:
			setActionName("setvar");
			break;
		case Action_AddVariable:
			setActionName("addvar");
			break;
		case Action_Launch:
			setActionName("launch");
			break;
		case Action_SetBGColour:
			setActionName("setbgcolour");
			break;
		case Action_SetCurrentScene:
			setActionName("setcurrentscene");
			break;
		case Action_BringtSceneToFront:
			setActionName("bringscenetofront");
			break;
		case Action_EnterValue:
			setActionName("entervalue");
			break;
		case Action_UpdateText:
			setActionName("updatetext");
			break;
		case Action_Exit:
			setActionName("exit");
			break;
		case Action_RemoveResource:
			setActionName("removeresource");
			break;
		case Action_RemoveResources:
			setActionName("removeresources");
			break;
		case Action_ChangeProgram:
			setActionName("changeprogram");
			break;
		case Action_DebugText:
			setActionName("debugtext");
			break;
		case Action_LoadFile:
			setActionName("loadfile");
			break;
		case Action_CallGlobalScript:
			setActionName("callglobalscript");
			break;
		case Action_ChangeVideoCam:
			setActionName("changevideocam");
			break;
		}
	}
	void Execute(IIwGameXomlResource* source, CIwGameAction* action);
};


#endif // _IW_GAME_H_
