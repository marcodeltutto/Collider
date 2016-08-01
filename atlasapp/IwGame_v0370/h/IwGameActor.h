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

#if !defined(_IW_GAME_ACTOR_H_)
#define _IW_GAME_ACTOR_H_

#include "IwGx.h"
#include "Iw2D.h"
#include "IwGeom.h"

#include "IwGameUtil.h"
#include "IwGameScene.h"
#include "IwGameSprite.h"
#include "IwGameAnim.h"
#include "IwGameString.h"
#include "IwGameXoml.h"
#include "IwGameBox2d.h"
#include "IwGameShapes.h"
#include "IwGameActions.h"
#include "IwGameModifier.h"



//
//
//
//
// CIwGameActor - A basic game actor. We derive different types of game objects from this type and add them to a IwGameScene to be processed and rendered
//
//
//
//
class CIwGameActor : public IIwGameXomlResource, public IIwGameAnimTarget
{
public:
	enum eIwGameActorCategory
	{
		AC_Generic,  
		AC_UI, 
	};
	enum eIwGameAspectLock
	{
		AL_None,  
		AL_X,  
		AL_Y, 
	};


protected:
	// Properties
	CIwGameScene*		Scene;					// Scene that actor lives in
	eIwGameActorCategory Category;				// Category of actor
	int					Type;					// Type of Actor (use to distinguish beteeen different actor types)
	CIwVec2				Size;					// Visual size
	CIwFVec2			OriginalPosition;		// Original position of actor in the scene (when actor was first spawned)
	CIwFVec2			Position;				// Current position of actor in the scene
	eIwGameScene_Docking Docking;				// Docking position
	CIwRect				Margin;					// Margin (spacing around the actor)
	float				Depth;					// Parallax depth
	CIwFVec2			Origin;					// Origin of actor
	CIwFVec2			Velocity;				// Current velocity of actor
	CIwFVec2			VelocityDamping;		// Dampens the velocity (Values less than 1.0f will reduce velocity over time, whilst values larger than 1.0f will increase velocity over time)
	float				OriginalAngle;			// Original angle in scene (when first spawned)
	float				Angle;					// Orientation in scene (degrees)
	float				AngularVelocity;		// Angular velocity
	float				AngularVelocityDamping;	// Angular velocity damping
	CIwFVec2			OriginalScale;			// Original scale of actor in the scene (when actor was first spawned)
	float				ScaleX, ScaleY;			// Scale
	CIwColour			OriginalColour;			// Original colour
	CIwColour			Colour;					// Colour
	int					Layer;					// Depth layer
	bool				Used;					// Used is used when actors pooled to reduce memory fragmentation
	bool				IsActive;				// Active state of actor
	bool				IsVisible;				// Visible state of actor
	bool				IsCollidable;			// Collidable state of actor
	bool				IsTappable;				// Tappable state of actor
	bool				IsDraggable;			// Draggable state of actor
	bool				IsDragging;				// Dragging state of actor
	bool				IsDragging2;			// Dragging2 state of actor
	bool				ReceiveEventFromChild;	// If true then this actor will receieve events from its children
	bool				WrapPosition;			// If true then if position exits the extents of the scene actor will wrap back around to the opposite side of the scene
	bool				Destroyed;				// An actor is marked asd destroyed when it has been marked for deletion
	bool				ScreenDocking;			// if true then the actors docking value will be applied to the screen and not any parent containers that support edge docking
	bool				HoldFocus;				// When et to true this actor will not lose focus when the user moves out of its range
	bool				UseParentOpacity;		// If true then this actors visuals opacity will be scaled by its parent, if it has one
	eIwGameAspectLock	AspectLock;				// Determines which acis to use to lock aspect ratio of actors size
	CIwGameSprite*		Visual;					// Visual element that represents the actor
	CIwGameAnimTimeline* Timeline;				// Timeline, controls animation of the actor
	CIwGameXomlBindings* Bindings;				// Manages any attached bindings
	CIwGameXomlBinding*	SimpleBinding;			// A simple one property binding
	int					CollisionSize;			// Size of collision area
	CIwRect				CollisionRect;			// Spherical collision size
	float				PreviousAngle;			// Previous updates angle
	CIwFVec2			PreviousPosition;		// Previous updates position
	CIwGameActor*		LinkedTo;				// Actor that this actor links to. Linked actors will utilise their target links colour, visibility and transform etc..
	CIwGameActor*		Target;					// An actor target
	int					TouchIndex;				// Index of last touch on this actor
#if defined(IW_GAME_BOX2D)
	CIwGameBox2dBody*	Box2dBody;				// Box2D physics body
#endif // IW_GAME_BOX2D_H_
	CIwGameActionsManager*	ActionsManager;		// Manages actions
	CIwGameAnimTimelinesManager* TimelinesManager; // Manages timelines
	CIwGameEventManager* EventsManager;			// List of events that the actor handles
	CIwGameModifierManager* Modifiers;			// Class modifiers manager
	unsigned int		GridPos;				// x, y position in grid
public:
	void				setCategory(eIwGameActorCategory cat)	{ Category = cat; }
	eIwGameActorCategory getCategory() const					{ return Category; }
	void				setUsed(bool in_use)					{ Used = in_use; }
	bool				isUsed() const							{ return Used; }
	void				setScene(CIwGameScene* scene)			{ Scene = scene; }
	CIwGameScene*		getScene()								{ return Scene; }
	void				setType(int type)						{ Type = type; }
	int					getType() const							{ return Type; }
	CIwVec2				getSize() const							{ return Size; }
	void				setOriginalPosition(float x, float y)	{ OriginalPosition.x = x; OriginalPosition.y = y; }
	CIwFVec2			getOriginalPosition()					{ return OriginalPosition; }
	void				setPosition(float x, float y);
	void				setPosition(const CIwFVec2& pos)		{ setPosition(pos.x, pos.y); }
	void				setDocking(eIwGameScene_Docking docking);
	void				setDocking(eIwGameScene_Docking docking, int width, int height);
	eIwGameScene_Docking getDocking()							{ return Docking; }
	void				setMargin(int left, int right, int top, int bottom)	{ Margin.x = left, Margin.y = right, Margin.w = top, Margin.h = bottom; }
	CIwRect				getMargin() const						{ return Margin; }
	CIwFVec2			getPosition()							{ return Position; }
	void				setDepth(float depth)					{ Depth = depth; }
	float				getDepth() const						{ return Depth; }
	void				setOrigin(float x, float y)				{ Origin.x = x; Origin.y = y; }
	CIwFVec2			getOrigin()								{ return Origin; }
	void				setOriginalAngle(float angle)			{ OriginalAngle = angle; }
	float				getOriginalAngle()						{ return OriginalAngle; }
	void				setAngle(float angle);
	float				getAngle() const						{ return Angle; }
	void				setVelocity(float x, float y);
	CIwFVec2			getVelocity() const						{ return Velocity; }
	void				setVelocityDamping(float x, float y);	// When using Box2d, the y value is ignored
	void				setVelocityDamping(float damping);
	void				setAngularVelocity(float velocity);
	float				getAngularVelocity() const				{ return AngularVelocity; }
	void				setAngularVelocityDamping(float damping);
	void				setOriginalScale(float x, float y)		{ OriginalScale.x = x; OriginalScale.y = y; }
	CIwFVec2			getOriginalScale()						{ return OriginalScale; }
	void				setScale(float scale)					{ ScaleX = ScaleY = scale; }
	float				getScale() const						{ return ScaleX; }
	void				setScaleX(float scale)					{ ScaleX = scale; }
	float				getScaleX() const						{ return ScaleX; }
	void				setScaleY(float scale)					{ ScaleY = scale; }
	float				getScaleY() const						{ return ScaleY; }
	void				setColour(uint8 r, uint8 g, uint8 b, uint8 a)	{ Colour.r = r; Colour.g = g; Colour.b = b; Colour.a = a; }
	void				setColour(CIwColour& colour)			{ Colour = colour; }
	CIwColour			getColour() const						{ return Colour; }
	CIwColour			getOriginalColour() const				{ return OriginalColour; }
	int					getOpacity() const						{ return Colour.a; }
	void				setLayer(int layer)						{ Layer = layer; }
	int					getLayer() const						{ return Layer; }
	void				setActive(bool active);
	bool				isActive() const						{ return IsActive; }
	virtual void		setVisible(bool visible, bool force_set = false);
	bool				isVisible() const						{ return IsVisible; }
	void				setCollidable(bool collidable);
	bool				isCollidable() const					{ return IsCollidable; }
	void				setTappable(bool tappable)				{ IsTappable = tappable; }
	bool				isTappable() const						{ return IsTappable; }
	void				setDraggable(bool draggable)			{ IsDraggable = draggable; }
	bool				isDraggable() const						{ return IsDraggable; }
	bool				isDragging() const						{ return IsDragging; }
	bool				isDragging2() const						{ return IsDragging2; }
	void				setReceiveEventFromChild(bool enable)	{ ReceiveEventFromChild = enable; }
	bool				canReceiveEventFromChild() const		{ return ReceiveEventFromChild; }
	void				setVisual(CIwGameSprite* visual)		{ Visual = visual; }
	CIwGameSprite*		getVisual()								{ return Visual; }
	void				removeVisual();
	void				setTimeline(CIwGameAnimTimeline* timeline);
	CIwGameAnimTimeline* getTimeline()							{ return Timeline; }
	void				setBindings(CIwGameXomlBindings* bindings);
	CIwGameXomlBindings* getBindings()							{ return Bindings; }
	void				setSimpleBinding(CIwGameXomlBinding* binding);
	CIwGameXomlBinding* getSimpleBinding()						{ return SimpleBinding; }
	void				setCollisionRect(CIwRect& rect);
	void				setCollisionSize(int size);
	CIwRect				getCollisionRect() const				{ return CollisionRect; }
	int					getCollisionSize() const				{ return CollisionSize; }
	void				setPreviousPosition(float x, float y)	{ PreviousPosition.x = x; PreviousPosition.y = y; }
	CIwFVec2			getPreviousPosition() const				{ return PreviousPosition; }
	void				setPreviousAngle(float angle)			{ PreviousAngle = angle; }
	float				getPreviousAngle() const				{ return PreviousAngle; }
	void				setWrapPosition(bool enable)			{ WrapPosition = enable; }
	bool				getWrapPosition() const					{ return WrapPosition; }
	void				setLinkedTo(CIwGameActor* actor);
	CIwGameActor*		getLinkedTo()							{ return LinkedTo; }
	void				setTarget(CIwGameActor* target)			{ Target = target; }
	CIwGameActor*		getTarget()								{ return Target; }
	void				setDestroyed(bool destroyed)			{ Destroyed = destroyed; if (Destroyed) OnDestroy(); }
	bool				isDestroyed() const						{ return Destroyed; }
#if defined(IW_GAME_BOX2D)
	void				setBox2dBody(CIwGameBox2dBody* body)	{ Box2dBody = body; Box2dBody->setUserData(this); }
	CIwGameBox2dBody*	getBox2dBody()							{ return Box2dBody; }
#endif // IW_GAME_BOX2D_H_
	CIwGameActionsManager* getActionsManager()					{ return ActionsManager; }
	CIwGameAnimTimelinesManager* getTimelinesManager()			{ return TimelinesManager; }
	CIwGameEventManager* getEventsManager()						{ return EventsManager; }
	void				setModifiers(CIwGameModifierManager* mods)	{ Modifiers = mods; }
	CIwGameModifierManager* getModifiers()						{ return Modifiers; }
	bool				setProperty(const char* element_name, CIwGameString& data, bool delta);
	virtual bool		setProperty(unsigned int element_name, CIwGameString& data, bool delta);
	void				setGridPos(int x, int y);
	unsigned int		getGridPos() const						{ return GridPos; }
	void				setScreenDocking(bool enabled)			{ ScreenDocking = enabled; }
	bool				getScreenDocking() const				{ return ScreenDocking; }
	void				setHoldFocus(bool hold)					{ HoldFocus = hold; }
	bool				getHoldFocus() const					{ return HoldFocus; }
	void				setTouchindex(int index)				{ TouchIndex = index; }
	int					getTouchIndex() const					{ return TouchIndex; }
	void				setUseParentOpacity(bool enable)		{ UseParentOpacity = enable; }
	bool				getUseParentOpacity() const				{ return UseParentOpacity; }
	void				setAspectLock(eIwGameAspectLock lock)	{ AspectLock = lock; }
	eIwGameAspectLock	getAspectLock() const					{ return AspectLock; }
	// Properties end
protected:
	CIwVec2				OriginalSize;			// Original visual size
	CIwRect				OriginalMargin;			// Original margin
	CIwVec2				DragAnchor;
	virtual bool		UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var);
	virtual void		UpdateBindings(bool force_modified = false);		// Update data bindings
	virtual void		LinkChanged(CIwGameActor* child, bool remove) {}	// Called from an actor when an actor that links to this actor unlinks itself
	CIwGameActor*		FindEventDependentParent();							// Events can be set up to bubble down to parents, this method finds parents that is enabled to receive touch events
	CIwGameActor*		FindClipper();										// Tracks back through actor parents to find any that change the clipping rect
	void				CalculateSizes(int& width, int& height);
	void				CalculateMargins(CIwRect& margin);
	void				UpdateMargins();

public:
	CIwGameActor() : IIwGameXomlResource(), Used(false), Scene(NULL), Visual(NULL), Timeline(NULL), EventsManager(NULL), Modifiers(NULL), LinkedTo(NULL), Bindings(NULL), SimpleBinding(NULL), ActionsManager(NULL), TimelinesManager(NULL)
#if defined(IW_GAME_BOX2D)
	, Box2dBody(NULL)
#endif // IW_GAME_BOX2D_H_
	{
		setClassType("actor");
		Reset();
		ActionsManager = new CIwGameActionsManager();
		ActionsManager->setParent(this);
		TimelinesManager = new CIwGameAnimTimelinesManager();
		TimelinesManager->setParent(this);
		EventsManager = new CIwGameEventManager();
	}
	virtual				~CIwGameActor();

	// Initialise the actor
	virtual void		Init();
	virtual void		SetFromBrush(IIwGameBrush* brush)		{}
	
	// Reset the actor (used by memory pooling systems to save actor re-allocation, usually called after re-allocation to reset the object to a default state)
	virtual void		Reset();

	// Update the actor
	virtual bool		Update(float dt);

	// Update the visual that represents this actor on screen
	virtual bool		UpdateVisual();

	// Actors are responsible for carrying out there own collision checks. Called after all actor updates to check and resolve any collisions
	virtual void		ResolveCollisions() {}

	// NotifyCollision is called by another actor when it collides with this actor
	virtual	void		NotifyCollision(CIwGameActor* other) {}

	// Hit test
	virtual CIwGameActor* HitTest(int x, int y);

	virtual bool		isOutsideFocusRange(int x, int y, float scale = 1.0f);

	// Checks to see if another actor is colliding with this actor
	bool				CheckCollision(CIwGameActor* other);

	// Event handlers
	virtual void		ProcessEventActions(unsigned int event_name);
	virtual void		OnTapped(int index, int x, int y, bool allow_bubble);
	virtual void		OnBeginTouch(int index, int x, int y, bool allow_bubble);
	virtual void		OnEndTouch(int index, int x, int y, bool allow_bubble);
	virtual void		OnCreate();
	virtual void		OnDestroy();
	virtual void		OnOrientationChange(eIwGameScene_Orientation old_orientation, eIwGameScene_Orientation new_orientation);

	// Implementation of IIwGameXomlResource interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
	bool				PostLoadFromXoml(IIwGameXomlResource* parent, CIwGameXmlNode* node);

	// Implementation of IIwGameAnimTarget interface
	bool				UpdateFromAnimation(CIwGameAnimInstance *animation);

	// Utility
	void				RemoveBody();

	// Coordinate tranoform
	CIwVec2				TransformPoint(float x, float y);	// Transform point by actors local angle / scale transform


};

//
//
//
//
// CIwGameActorImage - An image based game actor. Acts as an helper class for creating image sprite based actors
//
//
//
//
class CIwGameActorImage : public CIwGameActor
{
	// Properties
protected:
public:
	bool				setProperty(unsigned int element_name, CIwGameString& data, bool delta);
	// Properties end
protected:
	bool				InitWithImage(CIwGameImage* image, int width, int height);
	bool				InitWithBrush(IIwGameBrush* brush, int width, int height);
	bool				UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var);
public:
	CIwGameActorImage() : CIwGameActor()	{ setActualClassType("actorimage"); }
	virtual ~CIwGameActorImage()			{}

	virtual bool		Init(IIwGameBrush* brush, int width, int height);
	virtual bool		Init(CIwGameImage* image, int width, int height);
	virtual bool		Init(CIwGameImage* image, CIwRect* src_rect);
	virtual bool		Init(CIwGameImage* image, int x, int y, int w, int h);
	bool				UpdateVisual();

	void				SetSrcDest(int x, int y, int w, int h);
	void				SetSrcRect(int x, int y, int w, int h);
	void				SetDestSize(int w, int h);
	void				SetImage(CIwGameImage* image);
	void				SetFromBrush(IIwGameBrush* brush);

	// Event handlers
	virtual void		OnOrientationChange(eIwGameScene_Orientation old_orientation, eIwGameScene_Orientation new_orientation);

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	// Implementation of IIwGameAnimTarget interface
	bool				UpdateFromAnimation(CIwGameAnimInstance *animation);
};

//
//  CIwGameActorImageCreator - Creates an instance of an image based actor object
//
class CIwGameActorImageCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameActorImageCreator()
	{
		setClassName("actorimage");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameActorImage(); }
};


//
//
//
//
// CIwGameActorText - A textbased game actor. Acts as an helper class for creating text sprite based actors
//
//
//
//
class CIwGameActorText : public CIwGameActor
{
public:
	// Properties
protected:
	bool				AutoHeight;					// When true the contained text will decide the height of the text display rect
public:
	void				setText(const char* text);
	void				setRect(CIwRect& rect);
	void				setFlags(IwGxFontFlags flags);
	void				setFont(CIwGameFont* font);
	void				setAlignH(IwGxFontAlignHor align);
	void				setAlignV(IwGxFontAlignVer align);
	bool				setProperty(unsigned int element_name, CIwGameString& data, bool delta);
	void				setAutoHeight(bool enable)					{ AutoHeight = enable; }
	bool				isAutoHeight() const						{ return AutoHeight; }
	bool				isFontLoaded() const						{ return FontLoaded; }
	// Properties end
protected:
	CIwRect				OriginallRect;
	bool				FontLoaded;
	bool				UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var);

public:
	CIwGameActorText() : CIwGameActor(), AutoHeight(false), FontLoaded(false)			{ setActualClassType("actortext"); IsTappable = false; }
	virtual ~CIwGameActorText()			{}

	virtual bool		Init(CIwGameFont* font, CIwRect& rect, CIwGameString& text, int flags);
	virtual bool		UpdateVisual();

	// Event handlers
	virtual void		OnOrientationChange(eIwGameScene_Orientation old_orientation, eIwGameScene_Orientation new_orientation);

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	// Implementation of IIwGameAnimTarget interface
	bool				UpdateFromAnimation(CIwGameAnimInstance *animation);
};

//
//  CIwGameActorTextCreator - Creates an instance of a text based text actor object
//
class CIwGameActorTextCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameActorTextCreator()
	{
		setClassName("actortext");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameActorText(); }
};

//
//
//
//
//  CIwGameXomlAction_Actors - A collection of actor actions
//
//
//
//
class CIwGameXomlAction_Actors : public IIwGameXomlAction
{
public:
	enum ActionType
	{
		Action_HideActor		= 0, 
		Action_ShowActor		= 1, 
		Action_ActivateActor	= 2, 
		Action_DeactivateActor	= 3, 
		Action_KillActor		= 4, 
		Action_SetProperty		= 5, 
		Action_AddProperty		= 6, 
		Action_SetKeyFocus		= 7, 
		Action_Max				= 8, 
	};
protected:
	ActionType	Type;
	CIwGameXomlAction_Actors() {}
public:
	CIwGameXomlAction_Actors(ActionType type)
	{
		Type = type;
		switch (Type)
		{
		case Action_HideActor:
			setActionName("hideactor");
			break;
		case Action_ShowActor:
			setActionName("showactor");
			break;
		case Action_ActivateActor:
			setActionName("activateactor");
			break;
		case Action_DeactivateActor:
			setActionName("deactivateactor");
			break;
		case Action_KillActor:
			setActionName("killactor");
			break;
		case Action_SetProperty:
			setActionName("setproperty");
			break;
		case Action_AddProperty:
			setActionName("addproperty");
			break;
		case Action_SetKeyFocus:
			setActionName("setkeyfocus");
			break;
		}
	}
	void Execute(IIwGameXomlResource* source, CIwGameAction* action);
};


#endif // _IW_GAME_ACTOR_H_
