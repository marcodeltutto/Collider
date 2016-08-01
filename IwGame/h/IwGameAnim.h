#if !defined(_CIW_GAME_ANIMATION_H_)
#define _CIW_GAME_ANIMATION_H_

#include "IwGeom.h"
#include "IwList.h"
#include "IwHashString.h"
#include "IwGameUtil.h"
#include "IwGameXoml.h"
#include "IwGameActions.h"

class CIwGameAnimInstance;
class CIwGameAnimTimeline;

//
//
//
//
// CIwGameTweenEffect - Animation easing effect
//
//
// 
//
typedef float (*CIwGameTweenEffect)(float);


//
//
//
//
// Animation frame data types
//
//
//
//
struct CIwGameAnimFrame
{
	enum eIwGameAnimType
	{
		FT_Invalid,
		FT_Bool,
		FT_Float,
		FT_Vec2,
		FT_Vec3,
		FT_Vec4,
		FT_Rect,
		FT_String,
		FT_Custom,
	};
	enum eIwGameAnimEase
	{
		AT_Linear, 
		AT_QuadIn, 
		AT_QuadOut, 
		AT_CubicIn, 
		AT_CubicOut, 
		AT_QuarticIn, 
		AT_QuarticOut, 
	};

	float					Time;					// Time at which this frame should be active
	CIwGameTweenEffect		EasingFunc;				// Easing function

	CIwGameAnimFrame() : EasingFunc(NULL) {}
	virtual ~CIwGameAnimFrame() {}

	void				setEasing(eIwGameAnimEase effect);
};

struct CIwGameAnimFrameBool: public CIwGameAnimFrame
{
	bool	data;
	CIwGameAnimFrameBool() : CIwGameAnimFrame() {}
};

struct CIwGameAnimFrameFloat: public CIwGameAnimFrame
{
	float	data;
	CIwGameAnimFrameFloat() : CIwGameAnimFrame() {}
};

struct CIwGameAnimFrameVec2: public CIwGameAnimFrame
{
	CIwFVec2 data;
	CIwGameAnimFrameVec2() : CIwGameAnimFrame() {}
};

struct CIwGameAnimFrameVec3: public CIwGameAnimFrame
{
	CIwFVec3 data;
	CIwGameAnimFrameVec3() : CIwGameAnimFrame() {}
};

struct CIwGameAnimFrameVec4: public CIwGameAnimFrame
{
	CIwFVec4 data;
	CIwGameAnimFrameVec4() : CIwGameAnimFrame() {}
};

struct CIwGameAnimFrameRect: public CIwGameAnimFrame
{
	CIwRect data;
	CIwGameAnimFrameRect() : CIwGameAnimFrame() {}
};

struct CIwGameAnimFrameString: public CIwGameAnimFrame
{
	CIwGameString data;
	CIwGameAnimFrameString() : CIwGameAnimFrame() {}
};

//
//
//
//
// IIwGameAnimTarget - An animation target object
//
// 
// 
//
class IIwGameAnimTarget
{
public:
	virtual bool	UpdateFromAnimation(CIwGameAnimInstance *animation) = 0;
};

//
//
//
//
// CIwGameAnim - A collection of animation frames
//
//
// 
//
class CIwGameAnim : public IIwGameXomlResource
{
public:
	enum eIwGameAnimStatus
	{
		AS_Stopped, 
		AS_Playing, 
		AS_Paused, 
	};
	typedef CIwList<CIwGameAnimFrame*>::iterator _Iterator;
	_Iterator begin()					{ return Frames.begin(); }
	_Iterator end()						{ return Frames.end(); }

protected:
	// Properties
	CIwGameAnimFrame::eIwGameAnimType	Type;						// Type of animation
	float								Duration;					// Duration of animation
	CIwList<CIwGameAnimFrame*>			Frames;						// Animation frames
	int									Category;					// User defined category

public:
	void								setType(CIwGameAnimFrame::eIwGameAnimType type)	{ Type = type; }
	CIwGameAnimFrame::eIwGameAnimType	getType() const									{ return Type; }
	float								getDuration() const								{ return Duration; }
	void								setDuration(float duration)						{ Duration = duration; }
	void								addFrame(CIwGameAnimFrame* frame);
	bool								getCurrentFrame(CIwGameAnimFrame** current, CIwGameAnimFrame** next, float current_time);
	void								CalculateCurrentData(float current_time, CIwGameAnimFrame* current_data, bool interpolate = true);
	int									getFrameCount()									{ return Frames.size(); }
	CIwGameAnimFrame*					getFrame(int index)								{ return Frames.element_at(index); }
	void								setCategory(int category)						{ Category = category; }
	int									getCategory() const								{ return Category; }
	// Properties end

private:
	void								CalculateCurrentData();

public:
	CIwGameAnim() : IIwGameXomlResource(), Duration(0), Category(0) { setClassType("animation"); }
	virtual ~CIwGameAnim()
	{
		for (_Iterator it = Frames.begin(); it != Frames.end(); it++)
		{
			delete (*it);
		}
		Frames.clear();
	}

	void								GenerateAtlasFrames(int count, int frame_w, int frame_h, int start_x, int start_y, int pitch_x, int pitch_y, int image_width, float frame_speed);

	// Implementation of IIwGameXomlClass interface
	bool					LoadFromXoml(IIwGameXomlResource* parebt, bool load_children, CIwGameXmlNode* node);
};

//
//  CIwGameAnimCreator - Creates an instance of an animation object
//
class CIwGameAnimCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameAnimCreator()
	{
		setClassName("animation");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameAnim(); }
};

//
//
//
//
// CUIAnimationInstance - An instance of an animation object
//
//
// 
//
class CIwGameAnimInstance
{
public:
protected:
	// Properties
	CIwGameAnimTimeline*			Parent;					// Parent timeline
	CIwGameAnim*					Animation;				// Animation to use
	bool							Managed;				// True if managed by a CIwGameAnimInstanceManager
	int								RepeatCount;			// Number of times to replay the animation
	float							CurrentTime;			// Current animation time (0 is start of animation)
	CIwGameAnimFrame*				CurrentData;			// The current interpolated data for the current time
	CIwGameAnim::eIwGameAnimStatus	Status;					// Status of the animation
	IIwGameAnimTarget*				Target;					// Target object to update
#if defined(_DEBUG)
	CIwGameString					TargetProperty;			// Target object property name to update
#endif
	unsigned int					TargetPropertyHash;		// Target object property name hash to update
	float							Delay;					// Amount of time to delay starting the animation
	bool							DelayExpired;			// true if initial delay has expired
	CIwGameCallback					StartedCallback;		// Callback which is called when the animation starts playing
	CIwGameCallback					StoppedCallback;		// Callback which is called when the animation stops playing
	CIwGameCallback					LoopedCallback;			// Callback which is called when the animation loops
	bool							CurrentDataValid;		// True if current frame data is valid
	bool							IsDelta;				// Delta animation should update the target variables value instead of replacing it
	bool							Interpolate;			// If true then the interpolated valiue between key frames will be calculated and applied
	CIwGameEventManager*			EventsManager;			// List of events that the animation handles

public:
	void				setParent(CIwGameAnimTimeline* parent)			{ Parent = parent; }
	CIwGameAnimTimeline* getParent()									{ return Parent; }
	CIwGameAnim*		getAnimation()									{ return Animation; }
	void				setAnimation(CIwGameAnim* anim);
	void				setManaged(bool managed)						{ Managed = managed; }
	bool				isManaged() const								{ return Managed; }
	int					getRepeatCount() const							{ return RepeatCount; }
	void				setRepeatCount(int repeat_count)				{ RepeatCount = repeat_count; }
	void				setCurrentTime(float current_time)				{ CurrentTime = current_time; }
	float				getCurrentTime() const							{ return CurrentTime; }
	bool				isFinished() const								{ if (RepeatCount == 0) return false; else return RepeatedCount >= RepeatCount; }
	CIwGameAnimFrame*	getCurrentData()								{ return CurrentData; }
#if defined(_DEBUG)
	CIwGameString&		getTargetProperty()								{ return TargetProperty; }
#endif
	unsigned int		getTargetPropertyHash()							{ return TargetPropertyHash; }
	IIwGameAnimTarget*	getTargetElement()								{ return Target; }
	void				setTarget(IIwGameAnimTarget* element, const char* property_name)
	{
		Target = element;
#if defined(_DEBUG)
		TargetProperty = property_name;
#endif
		TargetPropertyHash = CIwGameString::CalculateHash(property_name);
	}
	void				setTargetElement(IIwGameAnimTarget* element)	{ Target = element; }
	float				getDelay() const								{ return Delay; }
	void				setDelay(float delay)							{ Delay = delay; }
	bool				isPlaying() const								{ return Status == CIwGameAnim::AS_Playing; }
	bool				isPaused() const								{ return Status == CIwGameAnim::AS_Paused; }
	bool				isDelayed() const								{ return !DelayExpired; }
	void				restart()										{ Status = CIwGameAnim::AS_Playing; CurrentTime = 0; DelayExpired = false; RepeatedCount = 0; }
	void				play()											{ Status = CIwGameAnim::AS_Playing; }
	void				stop()											{ Status = CIwGameAnim::AS_Stopped; CurrentTime = 0; DelayExpired = false; RepeatedCount = 0; }
	void				pause()											{ Status = CIwGameAnim::AS_Paused; }
	void				setStartedCallback(CIwGameCallback callback)	{ StartedCallback = callback; }
	void				setStoppedCallback(CIwGameCallback callback)	{ StoppedCallback = callback; }
	void				setLoopedCallback(CIwGameCallback callback)		{ LoopedCallback = callback; }
	bool				isCurrentDataValid() const						{ return CurrentDataValid; }
	void				setDelta(bool is_delta)							{ IsDelta = is_delta; }
	bool				isDelta() const									{ return IsDelta; }
	void				setInterpolate(bool interpolate)				{ Interpolate = interpolate; }
	bool				isInterpolated() const							{ return Interpolate; }
	void				addEventsManager();
	CIwGameEventManager* getEventsManager()								{ return EventsManager; }
	// Properties end

protected:
	int					RepeatedCount;

public:
	CIwGameAnimInstance() : Parent(NULL), Managed(false), Animation(NULL), RepeatCount(0), CurrentTime(0), RepeatedCount(0), Delay(0), DelayExpired(false), Status(CIwGameAnim::AS_Stopped), CurrentDataValid(false), 
							StartedCallback(NULL), StoppedCallback(NULL), LoopedCallback(NULL), IsDelta(false), Interpolate(true), EventsManager(NULL), CurrentData(NULL), Target(NULL), TargetPropertyHash(0) { }
	virtual ~CIwGameAnimInstance()
	{
		// Delete events manager
		SAFE_DELETE(EventsManager)

		SAFE_DELETE(CurrentData)
	}

	virtual void	Update(float dt, IIwGameAnimTarget* target = NULL);	// Passing none NULL for target allows you to retarget the animation update to another object
	virtual void	UpdateTargetOnly(IIwGameAnimTarget* target);
	virtual void	ForceUpdateTargetOnly(IIwGameAnimTarget* target);

	// Event handlers
	virtual void		ProcessEventActions(unsigned int event_name);
	virtual void		OnStart();
	virtual void		OnEnd();
	virtual void		OnRepeat();
};


//
//
//
//
// CIwGameAnimTimeline - An animation timeline
//
//
// 
//
class CIwGameAnimTimeline : public IIwGameXomlResource
{
public:
	typedef CIwList<CIwGameAnimInstance*>::iterator _Iterator;
	_Iterator begin()						{ return Animations.begin(); }
	_Iterator end()							{ return Animations.end(); }

protected:
	// Properties
	IIwGameXomlResource*			Parent;			// Parent container
	CIwList<CIwGameAnimInstance*>	Animations;		// Animation intsnces
	float							TimeScale;				// How to scale time (1 - noo change, > 1 speed up time, < 1 slow down time)
public:
	void							setParent(IIwGameXomlResource* parent)		{ Parent = parent; }
	IIwGameXomlResource*			getParent()									{ return Parent; }
	void							addAnimation(CIwGameAnimInstance* anim) { Animations.push_back(anim); anim->setManaged(true); anim->setParent(this); }
	void							removeAnimation(CIwGameAnimInstance* anim)
	{
		for (_Iterator it = begin(); it != end(); it++)
		{
			if (*it == anim)
			{
				delete (*it);
				Animations.erase(it);
				break;
			}
		}
	}
	CIwGameAnimInstance*			findAnimation(const char* name);
	CIwGameAnimInstance*			findAnimation(unsigned int name_hash);
	void							play();
	void							stop();
	void							pause();
	void							restart();
	int								getAnimationCount()					{ return Animations.size(); }
	void							setTargetElement(IIwGameAnimTarget* target)
	{
		for (_Iterator it = begin(); it != end(); it++)
			(*it)->setTargetElement(target);
	}
	bool							isPlaying();
	void							setTimeScale(float time_scale)		{ TimeScale = time_scale; }
	float							getTimeScale() const				{ return TimeScale; }
	// Properties end

private:
	static int						GlobalTimelineRunCount;			// Global timeline run count (used to prevent the same timeline being updated twice in one frame by different systems, which allows sharing of timeslines across objects)
	int								TimelineRunCount;

public:
	CIwGameAnimTimeline() : IIwGameXomlResource(), TimelineRunCount(-1), TimeScale(1.0f) { setClassType("timeline"); }
	virtual ~CIwGameAnimTimeline()
	{
		for (_Iterator it = begin(); it != end(); it++)
			delete (*it);
		Animations.clear();
	}

	virtual void			ForceUpdateTargetOnly(IIwGameAnimTarget* target = NULL);
	virtual void			UpdateTargetOnly(IIwGameAnimTarget* target = NULL);
	virtual void			Update(float time_delta, IIwGameAnimTarget* target = NULL);	// target will ovrride the current set target for the animation only if the animation was not updated by some other ojject this frame

	// Implementation of IIwGameXomlClass interface
	bool					LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	static void				UpdateRunCount()
	{
		GlobalTimelineRunCount++;
	}
	void					SetCurrentTime(float current_time)
	{
		for (_Iterator it = begin(); it != end(); it++)
			(*it)->setCurrentTime(current_time);
	}
};

//
//  CIwGameAnimTimelineCreator - Creates an instance of an animation object
//
class CIwGameAnimTimelineCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameAnimTimelineCreator()
	{
		setClassName("timeline");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameAnimTimeline(); }
};

// 
// 
//
//
// CIwGameAnimTimelinesManager - Manages a collection of timelines
//
//
//
//
class CIwGameAnimTimelinesManager
{
public:
	// Public access to iteration
	typedef CIwList<CIwGameAnimTimeline*>::iterator _Iterator;
	_Iterator				begin()		{ return Timelines.begin(); }
	_Iterator				end()		{ return Timelines.end(); }

protected:
	// Properties
	CIwList<CIwGameAnimTimeline*>	Timelines;			// A collection of timelines
	IIwGameXomlResource*			Parent;				// Parent container
public:
	void							addTimeline(CIwGameAnimTimeline* timeline);
	void							removeTimeline(CIwGameAnimTimeline* timeline);
	void							removeTimeline(unsigned int name_hash);
	CIwGameAnimTimeline*			findTimeline(unsigned int name_hash);
	CIwGameAnimTimeline*			findTimeline(const char* name);
	void							clearTimelines();
	void							setParent(IIwGameXomlResource* scene)	{ Parent = scene; }
	IIwGameXomlResource*			getParent()								{ return Parent; }
	// Properties end

public:
	CIwGameAnimTimelinesManager() : Parent(NULL) {}
	~CIwGameAnimTimelinesManager() { clearTimelines(); }

	// Utility
	static CIwGameAnimTimeline*	FindTimeline(const char* name, IIwGameXomlResource* container);
	static CIwGameAnimTimeline*	FindTimeline(unsigned int name_hash, IIwGameXomlResource* container);
	static CIwGameAnimTimeline*	FindTimeline(unsigned int name_hash, CIwGameScene* container, unsigned int actor_hash);
	static CIwGameAnimTimeline*	FindTimeline(const char* name, CIwGameScene* container, const char* actor_name);
};



//
//
//
//
// CIwGameAnimUtil - Animation utility class
//
//
// 
//
class CIwGameAnimUtil
{
public:
	static CIwGameAnim*			CreateFloatAnim(const char* name, float source, float target, float duration, CIwGameAnimFrame::eIwGameAnimEase easing, int category = 0);
	static CIwGameAnim*			CreateVec2Anim(const char* name, CIwFVec2& source, CIwFVec2& target, float duration, CIwGameAnimFrame::eIwGameAnimEase easing, int category = 0);
	static CIwGameAnim*			CreateVec3Anim(const char* name, CIwFVec3& source, CIwFVec3& target, float duration, CIwGameAnimFrame::eIwGameAnimEase easing, int category = 0);
	static CIwGameAnim*			CreateVec4Anim(const char* name, CIwFVec4& source, CIwFVec4& target, float duration, CIwGameAnimFrame::eIwGameAnimEase easing, int category = 0);
	static CIwGameAnim*			CreateRectAnim(const char* name, CIwRect& source, CIwRect& target, float duration, CIwGameAnimFrame::eIwGameAnimEase easing, int category = 0);
	static CIwGameAnimTimeline*	CreateTimeline(const char* name, CIwGameAnim* anim, IIwGameAnimTarget* target, const char* target_property, int repeat = 1, bool delta = false, float start_at_time = 0, bool interpolate = true);

};



#endif	// _CANIM_H_