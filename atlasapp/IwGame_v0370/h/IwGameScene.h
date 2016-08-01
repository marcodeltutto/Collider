// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. The only restriction to its usage is that this header must remain intact and visible in all IwGame engine files.
// If you use this engine in your product, whilst it is not mandatory, a mention of the IwGame engine would be nice or a quick email to let us know where it is being used.
//
//

#if !defined(IW_GAME_SCENE_H_)
#define IW_GAME_SCENE_H_

#include "s3e.h"
#include "IwGx.h"
#include "IwGeom.h"

#include "IwGameSprite.h"
#include "IwGameAnim.h"
#include "IwGameImage.h"
#include "IwGameUtil.h"
#include "IwGameString.h"
#include "IwGameXoml.h"
#include "IwGameResources.h"
#include "IwGameBox2d.h"
#include "IwGameActions.h"
#include "IwGameModifier.h"
#include "IwGameProgram.h"

class CIwGameActor;
class CIwGameCamera;

enum eIwGameScene_Docking
{
	DK_None, 
	DK_Top, 
	DK_Bottom, 
	DK_Left, 
	DK_Right, 
	DK_TopLeft, 
	DK_TopRight, 
	DK_BottomLeft, 
	DK_BottomRight, 
};

enum eIwGameScene_Orientation
{
	Orientation_Landscape, 
	Orientation_Portrait, 
};

//
//
//
//
//  CIwGameScene - A scene contains, handles and tracks a group of actors
//
//
//
//
class CIwGameScene : public IIwGameXomlResource, public IIwGameAnimTarget
{
public:
	// Public access to actor iteration
	typedef CIwList<CIwGameActor*>::iterator _Iterator;
	_Iterator					begin() { return Actors.begin(); }
	_Iterator					end() { return Actors.end(); }

	enum eIwGameScene_CanvasFit
	{
		CF_None, 
		CF_Width, 
		CF_Height, 
		CF_Both, 
		CF_Best, 
	};
	enum eIwGameScene_CanvasOrigin
	{
		CO_Centre, 
		CO_Top, 
		CO_Left, 
		CO_TopLeft, 
	};

	// Properties
protected:
	CIwGameSpriteManager*		SpriteManager;		// Manages sprites for the whole scene
	CIwGameXomlResourceManager*	ResourceManager;	// Manages all types of resources
	CIwGameXomlVariableManager*	VariableManager;	// Manages XOML variables
	CIwGameActionsManager*		ActionsManager;		// Manages actions
	CIwGameAnimTimelinesManager* TimelinesManager;	// Manages timelines
	CIwGameProgramManager*		ProgramManager;		// Manages the execution of a collection of programs
	CIwGameXomlBindings*		Bindings;			// Manages any attached bindings
	CIwGame*					Parent;				// Parent game controller
	int							Type;				// Type of scene
	eIwGameScene_Orientation	Orientation;		// Current display orientation
	CIwVec2						ScreenSize;			// Native screen size
	CIwVec2						VirtualSize;		// The virtual size is not the actual size of the scene. but a static pretend size that we can use to render to without having to cater for different sized displays
	CIwMat2D					VirtualTransform;	// Virtual transform is used to scale, translate and rotate scene to fit different display sizes and orientations
	CIwMat2D					Transform;			// Scene transform
	CIwList<CIwGameActor*>		Actors;				// Collection of scene actors
	CIwRect						Extents;			// Extents of scenes world
	CIwGameCamera*				Camera;				// Current camera
	CIwGameActor**				Collidables;		// List of collidable objects built this frame
	bool						AllowSuspend;		// If true then this processing of this scene will be suspended when the scene manager changes away from this scene
	CIwRect						ClippingArea;		// A rectangular area of the virtual screen that this scene will be clipped to
	bool						ClipStatic;			// If ClipStatic is true then camera transform is not applied to clipping rect
	bool						IsActive;			// Active state of scene
	bool						IsVisible;			// Visible state of scene
	bool						AllowFocus;			// If true this scene can receive input events if it is not the current scene
	CIwColour					Colour;				// Colour
	CIwGameAnimTimeline*		Timeline;			// Timeline, controls animation of the scene
	int							RunCount;			// NUmber of times the scene has been updated
	bool						Destroyed;			// A scene is marked asd destroyed when it has been marked for deletion
	int							Layer;				// Scenes layer
	bool						PhysicsEnabled;		// True if physics is enabled ni this scene
	bool						IsDragging;			// True if user is draggnig their finger on the scene
#if defined(IW_GAME_BOX2D)
	float						ConstantTimeStep;	// If 0 then time step will be calculated from frame rate
	CIwGameBox2dWorld*			Box2dWorld;
#endif // IW_GAME_BOX2D_H_
	CIwGameEventManager*		EventsManager;		// List of events that the scene handles
	CIwGameModifierManager*		Modifiers;			// Class modifiers manager
public:

	void					setParent(CIwGame* parent)				{ Parent = parent; }
	CIwGame*				getParent()								{ return Parent; }
	CIwGameSpriteManager*	getSpriteManager()						{ return SpriteManager; }
	CIwGameXomlResourceManager*	getResourceManager()				{ return ResourceManager; }
	CIwGameActionsManager*	getActionsManager()						{ return ActionsManager; }
	CIwGameAnimTimelinesManager* getTimelinesManager()				{ return TimelinesManager; }
	CIwGameXomlVariableManager* getVariableManager()				{ return VariableManager; }
	CIwGameProgramManager*	getProgrameManager()					{ return ProgramManager; }
	void					setType(int type)						{ Type = type; }
	int						getType() const							{ return Type; }
	CIwVec2					getScreenSize() const					{ return ScreenSize; }
	eIwGameScene_Orientation getOrientation() const					{ return Orientation; }
	CIwVec2					getVirtualSize() const					{ return VirtualSize; }
	void					setVirtualTransform(int required_width, int required_height, float angle, bool fix_aspect = false, bool lock_width = false, eIwGameScene_CanvasOrigin canvas_origin = CO_Centre);
	void					setVirtualTransform(int required_width, int required_height, float angle, eIwGameScene_CanvasFit fit = CF_Best, eIwGameScene_CanvasOrigin canvas_origin = CO_Centre);
	CIwMat2D&				getVirtualTransform()					{ return VirtualTransform; }
	CIwMat2D&				getTransform()							{ return Transform; }
	eIwGameScene_CanvasFit	getCanvasFit() const					{ return CanvasFit; }
	eIwGameScene_CanvasOrigin getCanvasOrigin() const				{ return CanvasOrigin; }
	void					addActor(CIwGameActor *actor);
	void					removeActor(CIwGameActor* actor, bool instant_delete = false);
	void					removeActor(unsigned int name_hash);
	void					removeLinkedActors(CIwGameActor* actor);
	CIwGameActor*			findActor(const char* name);
	CIwGameActor*			findActor(unsigned int name_hash);
	CIwGameActor*			findActor(int type);
	CIwGameActor*			findClosestActor(int x, int y, int type);
	CIwGameActor*			findFurthestActor(int x, int y, int type);
	void					clearActors();
	void					setExtents(int x, int y, int w, int h)	{ Extents.x = x; Extents.y = y; Extents.w = w; Extents.h = h; }
	CIwRect					getExtents() const						{ return Extents; }
	void					setCamera(CIwGameCamera* camera);
	CIwGameCamera*			getCamera()								{ return Camera; }
	bool					addCollideable(CIwGameActor* actor);
	CIwGameActor**			getCollidables()						{ return Collidables; }
	int						getTotalCollidables() const				{ return NextFreeCollidable; }
	void					setAllowSuspend(bool allow_suspend)		{ AllowSuspend = allow_suspend; }
	bool					getAllowSuspend() const					{ return AllowSuspend; }
	void					setClippingArea(int x, int y, int w, int h)		{ ClippingArea.x = x; ClippingArea.y = y; ClippingArea.w = w; ClippingArea.h = h; }
	CIwRect					getClippingArea()						{ return ClippingArea; }
	void					setClipStatic(bool is_static)			{ ClipStatic = is_static; }
	bool					isClipStatic() const					{ return ClipStatic; }
	void					setActive(bool active)					{ IsActive = active; }
	bool					isActive() const						{ return IsActive; }
	void					setVisible(bool visible)				{ IsVisible = visible; }
	bool					isVisible() const						{ return IsVisible; }
	void					setColour(CIwColour& colour)			{ Colour = colour; }
	void					setColour(uint8 r, uint8 g, uint8 b, uint8 a) { Colour.r = r; Colour.g = g; Colour.b = b; Colour.a = a; }
	CIwColour				getColour() const						{ return Colour; }
	void					setTimeline(CIwGameAnimTimeline* timeline) { Timeline = timeline; }
	CIwGameAnimTimeline*	getTimeline()							{ return Timeline; }
	void					setBindings(CIwGameXomlBindings* bindings)	{ Bindings = bindings; }
	CIwGameXomlBindings*	getBindings()							{ return Bindings; }
	void					setRunCount(int count)					{ RunCount = count; }
	int						getRunCount() const						{ return RunCount; }
	void					setDestroyed(bool destroyed)			{ Destroyed = destroyed; }
	bool					isDestroyed() const						{ return Destroyed; }
	void					setAllowFocus(bool allow)				{ AllowFocus = allow; }
	bool					getAllowFocus() const					{ return AllowFocus; }
	void					setLayer(int layer)						{ Layer = layer; }
	int						getLayer() const						{ return Layer; }
	bool					isDragging() const						{ return IsDragging; }
#if defined(IW_GAME_BOX2D)
	void					setConstantTimeStep(float step)			{ ConstantTimeStep = step; }
	float					getConstantTimeStep() const				{ return ConstantTimeStep; }
	CIwGameBox2dWorld*		getBox2dWorld()							{ return Box2dWorld; }
#endif // IW_GAME_BOX2D_H_
	CIwGameEventManager*	getEventsManager()						{ return EventsManager; }
	void					setModifiers(CIwGameModifierManager* mods)	{ Modifiers = mods; }
	CIwGameModifierManager* getModifiers()							{ return Modifiers; }
	// Properties end

protected:
	CIwVec2					OriginalSize;				// Original size of scene
	eIwGameScene_CanvasFit	CanvasFit;					// Fit method for scalnig canvas
	eIwGameScene_CanvasOrigin CanvasOrigin;				// Origin type
	CIwSVec2				TransformedClipRect[2];		// Transformed clipping rect for the scene
	CIwList<CIwGameActor*>	Removals;
	int						MaxCollidables;				// Maximum allowed collidables
	int						NextFreeCollidable;			// Points to next free slot in sollidables list pool
	int						MaxLayers;					// Maximum visible layers
	virtual void			UpdateBindings(bool force_modified = false);	// Update data bindings
	void					UpdateActorResizing();		// Updates the parents of any actors that have resized themselves


public:
	CIwGameScene() :	IIwGameXomlResource(), Parent(NULL), Collidables(NULL), ResourceManager(NULL), NextFreeCollidable(0), Camera(NULL), MaxCollidables(0), MaxLayers(0), AllowSuspend(true), IsActive(true), 
						IsVisible(true), ClippingArea(0, 0, 0, 0), Extents(0, 0, 0, 0), RunCount(0), EventsManager(NULL), VariableManager(NULL), ActionsManager(NULL), Bindings(NULL), Type(0), AllowFocus(false), 
						Destroyed(false), Timeline(NULL), SpriteManager(NULL), ClipStatic(false), Layer(0), Modifiers(NULL), ConstantTimeStep(2.0f), CanvasFit(CF_Best), CanvasOrigin(CO_Centre), TimelinesManager(NULL), 
						ProgramManager(NULL), IsDragging(false)
#if defined(IW_GAME_BOX2D)
	, Box2dWorld(NULL), PhysicsEnabled(true)
#else
	, PhysicsEnabled(true)
#endif // IW_GAME_BOX2D_H_
	{
		setClassType("scene");
		EventsManager = new CIwGameEventManager();
	}

	virtual ~CIwGameScene();

	// After creating the scene, call Init() to initialise it, passing the maximum number of actors that you expect can collide 
	virtual int		Init(int max_collidables = 128, int max_layers = 10, bool doSleep = true);

	// Update() will update the scene and all of its contained actors
	virtual void	Update(float dt);

	// Draw() will draw all of the scenes actors
	virtual void	Draw();

	// PreDestroy() is called when the scene has been marked for deletion
	virtual void	PreDestroy() {}

	// Coordinate transform helper method
	CIwFVec2		ScreenToVirtual(float screen_x, float screen_y, bool ignore_translation = false);	// Returns a virtual position from a screen position
	CIwVec2			VirtualToScreen(float virtual_x, float virtual_y, bool ignore_translation = false);	// Returns a screen position frmo a virtual position
	CIwFVec2		ScreenToCamera(float pos_x, float pos_y, bool ignore_translation = false);			// Returns a virtual position from a position taking into account camera position

	// Event handlers (Only implement these if you need to know about these specific events)
	void			ProcessEventActions(unsigned int event_name);
	virtual void	NotifyCreate();											// This event is called when this scene is being created
	virtual void	NotifyDestroy();										// This event is called when this scene is being destroyed
	virtual void	NotifySuspending(CIwGameScene* new_scene);				// This event is called when this scene is being suspended
	virtual void	NotifyResuming(CIwGameScene* old_scene);				// This event is called when this scene is being resumed
	virtual void	NotifyLostFocus(CIwGameScene* new_scene);				// This event is called when this scene has just lost focus
	virtual void	NotifyGainedFocus(CIwGameScene* old_scene);				// This event is called when this scene has just gained focus
	virtual void	NotifyKeyBack();										// This event is called when this scene receives a back key press
	virtual void	NotifyKeyMenu();										// This event is called when this scene receives a menu key press
	virtual void	NotifyOrientationChanged();								// This event is called when this scene receives an orientatin changed event

	// Utility 
	void			Suspend(CIwGameScene* new_scene);
	void			Resume(CIwGameScene* old_scene);
	CIwGameActor*	FindTappedActor(int x, int y);
	void			CleanupRemovedActors();

	// Implementation of IIwGameXomlResource interface
	bool			LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
	bool			PostLoadFromXoml(IIwGameXomlResource* parent, CIwGameXmlNode* node);

	// Implementation of IIwGameAnimTarget interface
	bool			UpdateFromAnimation(CIwGameAnimInstance *animation);

	// Internal
	void			RestoreClipping();
};

//
//
//
//
//  CIwGameSceneCreator - Creates an instance of a scene ovject
//
//
//
//
class CIwGameSceneCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameSceneCreator()
	{
		setClassName("scene");
	}
	IIwGameXomlResource* CreateInstance(IIwGameXomlResource* parent) { return new CIwGameScene(); }
};


//
//
//
//
//  CIwGameXomlAction_Scenes - A collection of scene actions
//
//
//
//
class CIwGameXomlAction_Scenes : public IIwGameXomlAction
{
public:
	enum ActionType
	{
		Action_ChangeScene			= 0, 
		Action_SuspendScene			= 1, 
		Action_ResumeScene			= 2, 
		Action_HideScene			= 3, 
		Action_ShowScene			= 4, 
		Action_ActivateScene		= 5, 
		Action_DeactivateScene		= 6, 
		Action_KillScene			= 7, 
		Action_KillAllScenes		= 8, 
		Action_CallActions			= 9, 
		Action_SetAllSceneTimelines	= 10, 
		Action_AddModifier			= 11, 
		Action_ChangeModifier		= 12, 
		Action_Max					= 13, 
	};
protected:
	ActionType	Type;
	CIwGameXomlAction_Scenes() {}
public:
	CIwGameXomlAction_Scenes(ActionType type)
	{
		Type = type;
		switch (Type)
		{
		case Action_ChangeScene:
			setActionName("changescene");
			break;
		case Action_SuspendScene:
			setActionName("suspendscene");
			break;
		case Action_ResumeScene:
			setActionName("resumescene");
			break;
		case Action_HideScene:
			setActionName("hidescene");
			break;
		case Action_ShowScene:
			setActionName("showscene");
			break;
		case Action_ActivateScene:
			setActionName("activatescene");
			break;
		case Action_DeactivateScene:
			setActionName("deactivatescene");
			break;
		case Action_KillScene:
			setActionName("killscene");
			break;
		case Action_KillAllScenes:
			setActionName("killallscenes");
			break;
		case Action_CallActions:
			setActionName("callactions");
			break;
		case Action_SetAllSceneTimelines:
			setActionName("setallscenetimelines");
			break;
		case Action_AddModifier:
			setActionName("addmodifier");
			break;
		case Action_ChangeModifier:
			setActionName("changemodifier");
			break;
		}
	}
	void Execute(IIwGameXomlResource* source, CIwGameAction* action);
};

#endif // IW_GAME_SCENE_H_