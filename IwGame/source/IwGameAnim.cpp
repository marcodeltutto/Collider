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

#include "IwGameAnim.h"
#include "IwGameString.h"
#include "IwGameScene.h"
#include "IwGameActor.h"
#include "IwGameActions.h"

// Global timeline run count (used to prevent the same timeline being updated twice in one frame by different systems, which allows sharing of timeslines across objects)
int CIwGameAnimTimeline::GlobalTimelineRunCount = 0;

//
//
//
//
// Tween easing functions
//
//
//
float CIwGameAnim_QuadEaseIn(float d)
{
	return d * d;
}
float CIwGameAnim_QuadEaseOut(float d)
{
	return d * (2 - d);
}
float CIwGameAnim_CubicEaseIn(float d)
{
	return d * d * d;
}
float CIwGameAnim_CubicEaseOut(float d)
{
	d -= 1;
	return d * d * d + 1;
}
float CIwGameAnim_QuarticEaseIn(float d)
{
	return d * d * d * d;
}
float CIwGameAnim_QuarticEaseOut(float d)
{
	d -= 1;
	return -(d * d * d * d - 1);
}

//
//
//
//
//
// CIwGameAnimFrame implementation
//
//
//
//

void CIwGameAnimFrame::setEasing(eIwGameAnimEase effect)
{
	EasingFunc = NULL;
	switch (effect)
	{
	case AT_Linear:
		break;
	case AT_QuadIn:
		EasingFunc = CIwGameAnim_QuadEaseIn;
		break;
	case AT_QuadOut:
		EasingFunc = CIwGameAnim_QuadEaseOut;
		break;
	case AT_CubicIn:
		EasingFunc = CIwGameAnim_CubicEaseIn;
		break;
	case AT_CubicOut:
		EasingFunc = CIwGameAnim_CubicEaseOut;
		break;
	case AT_QuarticIn:
		EasingFunc = CIwGameAnim_QuarticEaseIn;
		break;
	case AT_QuarticOut:
		EasingFunc = CIwGameAnim_QuarticEaseOut;
		break;
	}
}


//
//
//
//
//
// CIwGameAnim implementation
//
//
//
//

void CIwGameAnim::addFrame(CIwGameAnimFrame* frame)
{
	Frames.push_back(frame);
}

void CIwGameAnim::CalculateCurrentData(float current_Time, CIwGameAnimFrame* current_data, bool interpolate)
{
	if (current_data == NULL)
		return;

	CIwGameAnimFrame* current = NULL;
	CIwGameAnimFrame* next = NULL;

	if (!getCurrentFrame(&current, &next, current_Time))
		return;

	float dt2 = next->Time - current->Time;
	float dt = (current_Time - current->Time) / dt2;
	CIwGameTweenEffect easing = current->EasingFunc;

	if (easing != NULL)
		dt = easing(dt);

	switch (Type)
	{
	case CIwGameAnimFrame::FT_Bool:
		{
			CIwGameAnimFrameBool* c = (CIwGameAnimFrameBool*)current;
			((CIwGameAnimFrameBool*)current_data)->data = c->data;
		}
		break;
	case CIwGameAnimFrame::FT_Float:
		{
			CIwGameAnimFrameFloat* c = (CIwGameAnimFrameFloat*)current;
			CIwGameAnimFrameFloat* n = (CIwGameAnimFrameFloat*)next;
			if (c == n || !interpolate)
				((CIwGameAnimFrameFloat*)current_data)->data = c->data;
			else
			{
				float d = n->data - c->data;
				((CIwGameAnimFrameFloat*)current_data)->data = c->data + dt * d;
			}
		}
		break;
	case CIwGameAnimFrame::FT_Vec2:
		{
			CIwGameAnimFrameVec2* c = (CIwGameAnimFrameVec2*)current;
			CIwGameAnimFrameVec2* n = (CIwGameAnimFrameVec2*)next;
			if (c == n || !interpolate)
				((CIwGameAnimFrameVec2*)current_data)->data = c->data;
			else
			{
				float dx = n->data.x - c->data.x;
				float dy = n->data.y - c->data.y;
				((CIwGameAnimFrameVec2*)current_data)->data.x = c->data.x + dt * dx;
				((CIwGameAnimFrameVec2*)current_data)->data.y = c->data.y + dt * dy;
			}
		}
		break;
	case CIwGameAnimFrame::FT_Vec3:
		{
			CIwGameAnimFrameVec3* c = (CIwGameAnimFrameVec3*)current;
			CIwGameAnimFrameVec3* n = (CIwGameAnimFrameVec3*)next;
			if (c == n || !interpolate)
				((CIwGameAnimFrameVec3*)current_data)->data = c->data;
			else
			{
				float dx = n->data.x - c->data.x;
				float dy = n->data.y - c->data.y;
				float dz = n->data.z - c->data.z;
				((CIwGameAnimFrameVec3*)current_data)->data.x = c->data.x + dt * dx;
				((CIwGameAnimFrameVec3*)current_data)->data.y = c->data.y + dt * dy;
				((CIwGameAnimFrameVec3*)current_data)->data.z = c->data.z + dt * dz;
			}
		}
		break;
	case CIwGameAnimFrame::FT_Vec4:
		{
			CIwGameAnimFrameVec4* c = (CIwGameAnimFrameVec4*)current;
			CIwGameAnimFrameVec4* n = (CIwGameAnimFrameVec4*)next;
			if (c == n || !interpolate)
				((CIwGameAnimFrameVec4*)current_data)->data = c->data;
			else
			{
				float dx = n->data.x - c->data.x;
				float dy = n->data.y - c->data.y;
				float dz = n->data.z - c->data.z;
				float dw = n->data.w - c->data.w;
				((CIwGameAnimFrameVec4*)current_data)->data.x = (int16)(c->data.x + dt * dx);
				((CIwGameAnimFrameVec4*)current_data)->data.y = (int16)(c->data.y + dt * dy);
				((CIwGameAnimFrameVec4*)current_data)->data.z = (int16)(c->data.z + dt * dz);
				((CIwGameAnimFrameVec4*)current_data)->data.w = (int16)(c->data.w + dt * dw);
			}
		}
		break;
	case CIwGameAnimFrame::FT_Rect:
		{
			CIwGameAnimFrameRect* c = (CIwGameAnimFrameRect*)current;
			CIwGameAnimFrameRect* n = (CIwGameAnimFrameRect*)next;
			((CIwGameAnimFrameRect*)current_data)->data = c->data;
		}
		break;
	case CIwGameAnimFrame::FT_String:
		{
			CIwGameAnimFrameString* c = (CIwGameAnimFrameString*)current;
			((CIwGameAnimFrameString*)current_data)->data = c->data;
		}
		break;
	}
}

bool CIwGameAnim::getCurrentFrame(CIwGameAnimFrame** current, CIwGameAnimFrame** next, float current_Time)
{
	int size = Frames.size();
	if (size == 0)
		return false;				// Return false if no frames exist
	if (size < 2)
	{
		*current = *(Frames.begin());	// Return first frame if only one frame exists
		*next = *current;
		return true;
	}

	_Iterator prev = Frames.begin();
	for (_Iterator it = Frames.begin(); it != Frames.end(); it++)
	{
		if (current_Time < (*it)->Time)
		{
			*current = *prev;
			*next = *it;
			return true;
		}
		prev = it;
	}

	*current = *prev;
	*next = *prev;

	return true;	// return last frame
}

void CIwGameAnim::GenerateAtlasFrames(int count, int frame_w, int frame_h, int start_x, int start_y, int pitch_x, int pitch_y, int image_width, float frame_duration)
{
	setType(CIwGameAnimFrame::FT_Rect);

	int			x = start_x; 
	int			y = start_y;
	float		time = 0;

	while (count != 0)
	{
		CIwGameAnimFrameRect* frame = new CIwGameAnimFrameRect();

		// Set frames data
		frame->data.x = x;
		frame->data.y = y;
		frame->data.w = frame_w;
		frame->data.h = frame_h;
		frame->Time = time;

		// Set to next frame horizontally
		x += pitch_x;
		if (x >= image_width)
		{
			// Step down to next line of frames
			y += pitch_y;
			x = start_x;
		}

		addFrame(frame);

		count--;
		time += frame_duration;
	}
}

bool CIwGameAnim::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Process animation attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
			setName((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Type_Hash)
		{
			unsigned int type_hash = CIwGameString::CalculateHash((*it)->GetValue().c_str());
			if (type_hash == CIwGameXomlNames::bool_Hash)
				setType(CIwGameAnimFrame::FT_Bool);
			else
			if (type_hash == CIwGameXomlNames::float_Hash)
				setType(CIwGameAnimFrame::FT_Float);
			else
			if (type_hash == CIwGameXomlNames::vec2_Hash)
				setType(CIwGameAnimFrame::FT_Vec2);
			else
			if (type_hash == CIwGameXomlNames::vec3_Hash)
				setType(CIwGameAnimFrame::FT_Vec3);
			else
			if (type_hash == CIwGameXomlNames::vec4_Hash)
				setType(CIwGameAnimFrame::FT_Vec4);
			else
			if (type_hash == CIwGameXomlNames::rect_Hash)
				setType(CIwGameAnimFrame::FT_Rect);
			else
			if (type_hash == CIwGameXomlNames::string_Hash)
				setType(CIwGameAnimFrame::FT_String);
		}
		else
		if (name_hash == CIwGameXomlNames::Duration_Hash)
		{
			setDuration((*it)->GetValueAsFloat());
		}
	}

	if (Type == CIwGameAnimFrame::FT_Invalid)
	{
		CIwGameError::LogError("Error: XOML - Invalid animation type for animation");
		return false;
	}

	// Prrocess anim frames
	CIwGameAnimFrame* current_frame = NULL;
	for (CIwGameXmlNode::_Iterator it2 = node->begin(); it2 != node->end(); ++it2)
	{
		unsigned int name_hash = (*it2)->GetName().getHash();
		if (name_hash == CIwGameXomlNames::Atlas_Hash)
		{
			int			count = 0;
			CIwFVec2	size = CIwFVec2(0, 0);
			CIwFVec2	pos = CIwFVec2(0, 0);
			CIwFVec2	pitch = CIwFVec2(0, 0);
			int			width = 0;
			float		duration = 0.1f;

			for (CIwGameXmlNode::_AttribIterator it = (*it2)->attribs_begin(); it != (*it2)->attribs_end(); ++it)
			{
				unsigned int attrib_hash = (*it)->getName().getHash();

				if (attrib_hash == CIwGameXomlNames::Count_Hash)
					count = (*it)->GetValueAsInt();
				else
				if (attrib_hash == CIwGameXomlNames::Size_Hash)
				{
					if (!(*it)->GetValueAsPoint(size))
						CIwGameError::LogError("Error: XOML - Invalid value for Vec2 Atlas::Size");
				}
				else
				if (attrib_hash == CIwGameXomlNames::Position_Hash)
				{
					if (!(*it)->GetValueAsPoint(pos))
						CIwGameError::LogError("Error: XOML - Invalid value for Vec2 Atlas::Position");
				}
				else
				if (attrib_hash == CIwGameXomlNames::Pitch_Hash)
				{
					if (!(*it)->GetValueAsPoint(pitch))
						CIwGameError::LogError("Error: XOML - Invalid value for Vec2 Atlas::Pitch");
				}
				else
				if (attrib_hash == CIwGameXomlNames::Width_Hash)
					width = (*it)->GetValueAsInt();
				else
				if (attrib_hash == CIwGameXomlNames::Duration_Hash)
					duration = (*it)->GetValueAsFloat();
			}

			if (count != 0)
				GenerateAtlasFrames(count, (int)size.x, (int)size.y, (int)pos.x, (int)pos.y, (int)pitch.x, (int)pitch.y, width, duration);
			else
				CIwGameError::LogError("Error: XOML - Atlas frame count cannot be 0");

		}
		else
		if (name_hash == CIwGameXomlNames::Frame_Hash)
		{
			// Get frame values
			int index = 0;
			CIwGameXmlAttribute* value = NULL;
			CIwGameXmlAttribute* time = NULL;
			CIwGameXmlAttribute* easing = NULL;

			for (CIwGameXmlNode::_AttribIterator it = (*it2)->attribs_begin(); it != (*it2)->attribs_end(); ++it)
			{
				unsigned int attrib_hash = (*it)->getName().getHash();

				if (attrib_hash == CIwGameXomlNames::Value_Hash)
					value = *it;
				else
				if (attrib_hash == CIwGameXomlNames::Time_Hash)
					time = *it;
				else
				if (attrib_hash == CIwGameXomlNames::Easing_Hash)
					easing = *it;
			}
			if (value != NULL && time != NULL)
			{
				// Create animation frame
				switch (Type)
				{
				case CIwGameAnimFrame::FT_Bool:
					{
						CIwGameAnimFrameBool* frame = new CIwGameAnimFrameBool();
						current_frame = frame;
						addFrame(frame);
						frame->data = value->GetValueAsBool();
						break;
					}
				case CIwGameAnimFrame::FT_Float:
					{
						CIwGameAnimFrameFloat* frame = new CIwGameAnimFrameFloat();
						current_frame = frame;
						addFrame(frame);
						frame->data = value->GetValueAsFloat();
						break;
					}
					break;
				case CIwGameAnimFrame::FT_Vec2:
					{
						CIwGameAnimFrameVec2* frame = new CIwGameAnimFrameVec2();
						current_frame = frame;
						addFrame(frame);
						if (!value->GetValueAsPoint(frame->data))
						{
							CIwGameError::LogError("Error: XOML - Invalid value for Vec2 Frame::Values");
							return false;
						}
					}
					break;
				case CIwGameAnimFrame::FT_Vec3:
					{
						CIwGameAnimFrameVec3* frame = new CIwGameAnimFrameVec3();
						current_frame = frame;
						addFrame(frame);
						if (!value->GetValueAsPoint3(frame->data))
						{
							CIwGameError::LogError("Error: XOML - Invalid value for Vec3 Frame::Values");
							return false;
						}
					}
					break;
				case CIwGameAnimFrame::FT_Vec4:
					{
						CIwGameAnimFrameVec4* frame = new CIwGameAnimFrameVec4();
						current_frame = frame;
						addFrame(frame);
						if (!value->GetValueAsPoint4(frame->data))
						{
							CIwGameError::LogError("Error: XOML - Invalid value for Vec4 Frame::Values");
							return false;
						}
					}
					break;
				case CIwGameAnimFrame::FT_Rect:
					{
						CIwGameAnimFrameRect* frame = new CIwGameAnimFrameRect();
						current_frame = frame;
						addFrame(frame);
						if (!value->GetValueAsRect(frame->data))
						{
							CIwGameError::LogError("Error: XOML - Invalid value for Rect Frame::Values");
							return false;
						}
					}
					break;
				case CIwGameAnimFrame::FT_String:
					{
						CIwGameAnimFrameString* frame = new CIwGameAnimFrameString();
						current_frame = frame;
						addFrame(frame);
						frame->data = value->GetValue();
						break;
					}
					break;
				}

				// Set current time
				current_frame->Time = time->GetValueAsFloat();

				// Set easing
				if (easing != NULL)
				{
					unsigned int ease_hash = easing->GetValue().getHash();
					if (ease_hash == CIwGameXomlNames::Linear_Hash)
						current_frame->setEasing(CIwGameAnimFrame::AT_Linear);
					else
					if (ease_hash == CIwGameXomlNames::QuadIn_Hash)
						current_frame->setEasing(CIwGameAnimFrame::AT_QuadIn);
					else
					if (ease_hash == CIwGameXomlNames::QuadOut_Hash)
						current_frame->setEasing(CIwGameAnimFrame::AT_QuadOut);
					else
					if (ease_hash == CIwGameXomlNames::CubicIn_Hash)
						current_frame->setEasing(CIwGameAnimFrame::AT_CubicIn);
					else
					if (ease_hash == CIwGameXomlNames::CubicOut_Hash)
						current_frame->setEasing(CIwGameAnimFrame::AT_CubicOut);
					else
					if (ease_hash == CIwGameXomlNames::QuarticIn_Hash)
						current_frame->setEasing(CIwGameAnimFrame::AT_QuarticIn);
					else
					if (ease_hash == CIwGameXomlNames::QuarticOut_Hash)
						current_frame->setEasing(CIwGameAnimFrame::AT_QuarticOut);
					else
					{
						CIwGameError::LogError("Warning: XOML - Easing type not found, defaulting to linear - ", easing->GetValue().c_str());
					}
				}
			}
		}
	}

	// If we are declared inside a scene then animation is local to the scene
	CIwGameScene* scene = NULL;
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		scene = ((CIwGameActor*)parent)->getScene();
	else
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	if (scene != NULL)
		return scene->getResourceManager()->addResource(this);
	else
		return IW_GAME_GLOBAL_RESOURCES->getResourceManager()->addResource(this);

	return true;
}

//
//
//
//
// CIwGameAnimInstance implementation
//
//
//
//
void CIwGameAnimInstance::setAnimation(CIwGameAnim* anim)
{
	if (anim != NULL)
	{
		switch (anim->getType())
		{
		case CIwGameAnimFrame::FT_Bool:
			CurrentData = new CIwGameAnimFrameBool();
			break;
		case CIwGameAnimFrame::FT_Float:
			CurrentData = new CIwGameAnimFrameFloat();
			break;
		case CIwGameAnimFrame::FT_Vec2:
			CurrentData = new CIwGameAnimFrameVec2();
			break;
		case CIwGameAnimFrame::FT_Vec3:
			CurrentData = new CIwGameAnimFrameVec3();
			break;
		case CIwGameAnimFrame::FT_Vec4:
			CurrentData = new CIwGameAnimFrameVec4();
			break;
		case CIwGameAnimFrame::FT_Rect:
			CurrentData = new CIwGameAnimFrameRect();
			break;
		case CIwGameAnimFrame::FT_String:
			CurrentData = new CIwGameAnimFrameString();
			break;
		}
	}
	Animation = anim;
}
void CIwGameAnimInstance::addEventsManager()
{
	if (EventsManager == NULL)
		EventsManager = new CIwGameEventManager();
}

void CIwGameAnimInstance::Update(float dt, IIwGameAnimTarget* target)
{
	if (Status != CIwGameAnim::AS_Playing || Animation == NULL)
		return;

	IIwGameAnimTarget* current_target = (target == NULL) ? Target : target;

	dt = (dt * FRAME_SPEED_LOCK_MS) / 1000.0f;	// Convert to seconds

	if (RepeatCount != 0)
	{
		if (RepeatedCount >= RepeatCount)
		{
			Status = CIwGameAnim::AS_Stopped;

			// Notify user that the animation has stopped
			OnEnd();
			return;
		}
	}

	if (!DelayExpired && Delay != 0)
	{
		CurrentTime += dt;
		if (CurrentTime >= Delay)
		{
			DelayExpired = true;
			CurrentTime = 0;

			// Notify the user know that the animation has started
			OnStart();
		}
		else
		{
			return;
		}
	}

	float duration = Animation->getDuration();
	CurrentTime += dt;
	if (CurrentTime >= duration)
	{
		if (RepeatCount != 0)
		{
			RepeatedCount++;
			if (RepeatedCount >= RepeatCount)
			{
				Animation->CalculateCurrentData(CurrentTime, CurrentData, Interpolate);
				CurrentDataValid = true;
				if (current_target != NULL)
					current_target->UpdateFromAnimation(this);

				return;
			}
		}

		// Notify the user that the animation has looped
		OnRepeat();

		CurrentTime -= duration;
	}

	Animation->CalculateCurrentData(CurrentTime, CurrentData, Interpolate);
	CurrentDataValid = true;

	if (current_target != NULL)
		current_target->UpdateFromAnimation(this);
}

void CIwGameAnimInstance::UpdateTargetOnly(IIwGameAnimTarget* target)
{
	if (Status != CIwGameAnim::AS_Playing)
		return;

	if (target == NULL)
		target = Target;

	if (target != NULL && CurrentData != NULL)
	{
		if (!CurrentDataValid)
		{
			Animation->CalculateCurrentData(CurrentTime, CurrentData, Interpolate);
			CurrentDataValid = true;
		}
		target->UpdateFromAnimation(this);
	}
}

void CIwGameAnimInstance::ForceUpdateTargetOnly(IIwGameAnimTarget* target)
{
	if (target == NULL)
		target = Target;

	if (target != NULL && CurrentData != NULL)
	{
		if (!CurrentDataValid)
		{
			Animation->CalculateCurrentData(CurrentTime, CurrentData, Interpolate);
			CurrentDataValid = true;
		}
		target->UpdateFromAnimation(this);
	}
}

void CIwGameAnimInstance::ProcessEventActions(unsigned int event_name)
{
	if (EventsManager == NULL)
		return;

	// Find the event
	CIwGameEvent* evt = EventsManager->findEvent(event_name);
	if (evt != NULL)
	{
		// Find actions list
		IIwGameXomlResource* parent = Parent->getParent();
		CIwGameActions* actions = CIwGameActionsManager::FindActions(evt->ActionsName, parent);
		if (actions != NULL)
			actions->Execute(parent);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Error: Action not found - ", evt->_ActionsName.c_str());
#endif	//_DEBUG
	}
}

void CIwGameAnimInstance::OnStart()
{
	ProcessEventActions(CIwGameXomlNames::OnStart_Hash);
	if (StartedCallback != NULL)
		StartedCallback((void*)this, NULL);
}

void CIwGameAnimInstance::OnEnd()
{
	ProcessEventActions(CIwGameXomlNames::OnEnd_Hash);
	if (StoppedCallback != NULL)
		StoppedCallback((void*)this, NULL);
}

void CIwGameAnimInstance::OnRepeat()
{
	ProcessEventActions(CIwGameXomlNames::OnRepeat_Hash);
	if (LoopedCallback != NULL)
		LoopedCallback((void*)this, NULL);
}

//
//
//
//
// CIwGameAnimTimeline implementation
//
//
//
//
CIwGameAnimInstance* CIwGameAnimTimeline::findAnimation(const char* name)
{
	return findAnimation(CIwGameString::CalculateHash(name));
}
CIwGameAnimInstance* CIwGameAnimTimeline::findAnimation(unsigned int name_hash)
{
	for (_Iterator it = begin(); it != end(); it++)
	{
		if ((*it)->getAnimation()->getNameHash() == name_hash)
			return *it;
	}

	return NULL;
}

void CIwGameAnimTimeline::play()
{
	for (_Iterator it = begin(); it != end(); it++)
	{
		(*it)->play();
	}
}
void CIwGameAnimTimeline::stop()
{
	for (_Iterator it = begin(); it != end(); it++)
	{
		(*it)->stop();
	}
}
void CIwGameAnimTimeline::pause()
{
	for (_Iterator it = begin(); it != end(); it++)
	{
		(*it)->pause();
	}
}
void CIwGameAnimTimeline::restart()
{
	for (_Iterator it = begin(); it != end(); it++)
	{
		(*it)->restart();
	}
//	UpdateTargetOnly();
}

bool CIwGameAnimTimeline::isPlaying()
{
	for (_Iterator it = begin(); it != end(); it++)
	{
		if ((*it)->isPlaying())
			return true;
	}

	return false;
}

void CIwGameAnimTimeline::ForceUpdateTargetOnly(IIwGameAnimTarget* target)
{
	for (_Iterator it = begin(); it != end(); it++)
		(*it)->ForceUpdateTargetOnly(target);
}

void CIwGameAnimTimeline::UpdateTargetOnly(IIwGameAnimTarget* target)
{
	for (_Iterator it = begin(); it != end(); it++)
		(*it)->UpdateTargetOnly(target);
}

void CIwGameAnimTimeline::Update(float time_delta, IIwGameAnimTarget* target)
{
	// Prevents timeline from being more than once per game loop
	if (TimelineRunCount == GlobalTimelineRunCount)
	{
		UpdateTargetOnly(target);
		return;
	}

	for (_Iterator it = begin(); it != end(); it++)
		(*it)->Update(time_delta * TimeScale, target);

	TimelineRunCount = GlobalTimelineRunCount;
}


bool CIwGameAnimTimeline::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Process animation attributes
	bool auto_play = false;
	bool local = true;

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
			setName((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::AutoPlay_Hash)
			auto_play = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::TimeScale_Hash)
			setTimeScale((*it)->GetValueAsFloat());
		else
		if (name_hash == CIwGameXomlNames::Local_Hash)
			local = (*it)->GetValueAsBool();
	}

	CIwGameScene* scene = NULL;
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		scene = ((CIwGameActor*)parent)->getScene();
	else
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	// Calculate number of animations
	int num_animations = node->Children.size();

	// Prrocess animations
	for (CIwGameXmlNode::_Iterator it2 = node->begin(); it2 != node->end(); ++it2)
	{
		bool valid_anim = false;
		CIwGameAnimInstance* anim_instance = new CIwGameAnimInstance();
		addAnimation(anim_instance);

		unsigned int name_hash = (*it2)->GetName().getHash();
		if (name_hash == CIwGameXomlNames::Animation_Hash)
		{
			for (CIwGameXmlNode::_AttribIterator it = (*it2)->attribs_begin(); it != (*it2)->attribs_end(); ++it)
			{
				unsigned int attrib_hash = (*it)->getName().getHash();

				if (attrib_hash == CIwGameXomlNames::Anim_Hash)
				{
					// Search for the animation
					CIwGameAnim* anim = NULL;
					if (scene != NULL)
						anim = (CIwGameAnim*)scene->getResourceManager()->findResource((*it)->GetValue().c_str(), CIwGameXomlNames::Animation_Hash);
					else
						anim = (CIwGameAnim*)IW_GAME_GLOBAL_RESOURCES->getResourceManager()->findResource((*it)->GetValue().c_str(), CIwGameXomlNames::Animation_Hash);
					if (anim != NULL)
						valid_anim = true;
					else
						CIwGameError::LogError("Warning: XOML - Could not find named animation in the scene or in the global resources - ", (*it)->GetValue().c_str());

					anim_instance->setAnimation(anim);
				}
				else
				if (attrib_hash == CIwGameXomlNames::Target_Hash)
					anim_instance->setTarget(NULL, (*it)->GetValue().c_str());
				else
				if (attrib_hash == CIwGameXomlNames::Repeat_Hash)
					anim_instance->setRepeatCount((*it)->GetValueAsInt());
				else
				if (attrib_hash == CIwGameXomlNames::StartAtTime_Hash)
					anim_instance->setDelay((*it)->GetValueAsFloat());
				else
				if (attrib_hash == CIwGameXomlNames::Delta_Hash)
					anim_instance->setDelta((*it)->GetValueAsBool());
				else
				if (attrib_hash == CIwGameXomlNames::Interpolate_Hash)
					anim_instance->setInterpolate((*it)->GetValueAsBool());
				else
				if (attrib_hash == CIwGameXomlNames::OnStart_Hash)
				{
					anim_instance->addEventsManager();
					anim_instance->getEventsManager()->addEvent("OnStart", (*it)->GetValue().c_str());
				}
				else
				if (attrib_hash == CIwGameXomlNames::OnEnd_Hash)
				{
					anim_instance->addEventsManager();
					anim_instance->getEventsManager()->addEvent("OnEnd", (*it)->GetValue().c_str());
				}
				else
				if (attrib_hash == CIwGameXomlNames::OnRepeat_Hash)
				{
					anim_instance->addEventsManager();
					anim_instance->getEventsManager()->addEvent("OnRepeat", (*it)->GetValue().c_str());
				}
			}
		}
		if (!valid_anim)
			removeAnimation(anim_instance);
	}

	// If we are declared inside a scene or actor then actions are local to the scene
	setParent(parent);
	if (parent != NULL && (parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash || parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash))
	{
		if (parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
			((CIwGameScene*)parent)->getTimelinesManager()->addTimeline(this);
		else
		{
			if (local)
				((CIwGameActor*)parent)->getTimelinesManager()->addTimeline(this);
			else
				((CIwGameActor*)parent)->getScene()->getTimelinesManager()->addTimeline(this);
		}
	}
	else
	{
		IW_GAME_GLOBAL_RESOURCES->getTimelinesManager()->addTimeline(this);
	}


	if (auto_play)
		play();

	return true;
}

//
//
//
//
// CIwGameAnimUtil implementation
//
//
// 
//
CIwGameAnim* CIwGameAnimUtil::CreateVec2Anim(const char* name, CIwFVec2& source, CIwFVec2& target, float duration, CIwGameAnimFrame::eIwGameAnimEase easing, int category)
{
	CIwGameAnim* anim = new CIwGameAnim();
	anim->setName(name);
	anim->setCategory(category);
	anim->setDuration(duration);
	anim->setManaged(true);
	anim->setType(CIwGameAnimFrame::FT_Vec2);

	CIwGameAnimFrameVec2* frame = new CIwGameAnimFrameVec2();
	frame->data.x = source.x;
	frame->data.y = source.y;
	frame->Time = 0;
	frame->setEasing(easing);
	anim->addFrame(frame);

	frame = new CIwGameAnimFrameVec2();
	frame->data.x = target.x;
	frame->data.y = target.y;
	frame->Time = duration;
	anim->addFrame(frame);

	return anim;
}
CIwGameAnim* CIwGameAnimUtil::CreateVec3Anim(const char* name, CIwFVec3& source, CIwFVec3& target, float duration, CIwGameAnimFrame::eIwGameAnimEase easing, int category)
{
	CIwGameAnim* anim = new CIwGameAnim();
	anim->setName(name);
	anim->setCategory(category);
	anim->setDuration(duration);
	anim->setManaged(true);
	anim->setType(CIwGameAnimFrame::FT_Vec3);

	CIwGameAnimFrameVec3* frame = new CIwGameAnimFrameVec3();
	frame->data.x = source.x;
	frame->data.y = source.y;
	frame->data.z = source.z;
	frame->Time = 0;
	frame->setEasing(easing);
	anim->addFrame(frame);

	frame = new CIwGameAnimFrameVec3();
	frame->data.x = target.x;
	frame->data.y = target.y;
	frame->data.z = target.z;
	frame->Time = duration;
	anim->addFrame(frame);

	return anim;
}
CIwGameAnim* CIwGameAnimUtil::CreateVec4Anim(const char* name, CIwFVec4& source, CIwFVec4& target, float duration, CIwGameAnimFrame::eIwGameAnimEase easing, int category)
{
	CIwGameAnim* anim = new CIwGameAnim();
	anim->setName(name);
	anim->setCategory(category);
	anim->setDuration(duration);
	anim->setManaged(true);
	anim->setType(CIwGameAnimFrame::FT_Vec4);

	CIwGameAnimFrameVec4* frame = new CIwGameAnimFrameVec4();
	frame->data.x = source.x;
	frame->data.y = source.y;
	frame->data.z = source.z;
	frame->data.w = source.w;
	frame->Time = 0;
	frame->setEasing(easing);
	anim->addFrame(frame);

	frame = new CIwGameAnimFrameVec4();
	frame->data.x = target.x;
	frame->data.y = target.y;
	frame->data.z = target.z;
	frame->data.w = target.w;
	frame->Time = duration;
	anim->addFrame(frame);

	return anim;
}
CIwGameAnim* CIwGameAnimUtil::CreateFloatAnim(const char* name, float source, float target, float duration, CIwGameAnimFrame::eIwGameAnimEase easing, int category)
{
	CIwGameAnim* anim = new CIwGameAnim();
	anim->setName(name);
	anim->setCategory(category);
	anim->setDuration(duration);
	anim->setManaged(true);
	anim->setType(CIwGameAnimFrame::FT_Float);

	CIwGameAnimFrameFloat* frame = new CIwGameAnimFrameFloat();
	frame->data = source;
	frame->Time = 0;
	frame->setEasing(easing);
	anim->addFrame(frame);

	frame = new CIwGameAnimFrameFloat();
	frame->data = target;
	frame->Time = duration;
	anim->addFrame(frame);

	return anim;
}
CIwGameAnim* CIwGameAnimUtil::CreateRectAnim(const char* name, CIwRect& source, CIwRect& target, float duration, CIwGameAnimFrame::eIwGameAnimEase easing, int category)
{
	CIwGameAnim* anim = new CIwGameAnim();
	anim->setName(name);
	anim->setCategory(category);
	anim->setDuration(duration);
	anim->setManaged(true);
	anim->setType(CIwGameAnimFrame::FT_Vec4);

	CIwGameAnimFrameRect* frame = new CIwGameAnimFrameRect();
	frame->data.x = source.x;
	frame->data.y = source.y;
	frame->data.w = source.w;
	frame->data.h = source.h;
	frame->Time = 0;
	frame->setEasing(easing);
	anim->addFrame(frame);

	frame = new CIwGameAnimFrameRect();
	frame->data.x = target.x;
	frame->data.y = target.y;
	frame->data.w = target.w;
	frame->data.h = target.h;
	frame->Time = duration;
	anim->addFrame(frame);

	return anim;
}

CIwGameAnimTimeline* CIwGameAnimUtil::CreateTimeline(const char* name, CIwGameAnim* anim, IIwGameAnimTarget* target, const char* target_property, int repeat, bool delta, float start_at_time, bool interpolate)
{
	CIwGameAnimTimeline* timeline = new CIwGameAnimTimeline();
	timeline->setName(name);
	timeline->setManaged(false);

	CIwGameAnimInstance* inst = new CIwGameAnimInstance();
	inst->setAnimation(anim);
	inst->setDelay(start_at_time);
	inst->setDelta(delta);
	inst->setInterpolate(interpolate);
	inst->setParent(timeline);
	inst->setTarget(target, target_property);
	inst->setRepeatCount(repeat);
	inst->setManaged(true);

	timeline->addAnimation(inst);

	return timeline;
}

// 
// 
//
//
// CIwGameAnimTimelinesManager implementation
//
//
//
//
void CIwGameAnimTimelinesManager::addTimeline(CIwGameAnimTimeline* timelines)
{
#if defined(IW_REMOVE_DUPLICATES)
	CIwGameAnimTimeline* tl = findTimeline(timelines->getNameHash());
	if (tl != NULL)
	{
		CIwGameError::LogError("Warning: Timeline already exists, timeline was replaced - ", timelines->getName().c_str());
		removeTimeline(tl);
	}
#endif
	Timelines.push_back(timelines);
	timelines->setManaged(true);
}

void CIwGameAnimTimelinesManager::removeTimeline(CIwGameAnimTimeline* timeline)
{
	removeTimeline(timeline->getNameHash());
}

void CIwGameAnimTimelinesManager::removeTimeline(unsigned int name_hash)
{
	// Renove named Timeline from the manager
	for (_Iterator it = Timelines.begin(); it != Timelines.end(); ++it)
	{
		if (name_hash == (*it)->getNameHash())
		{
			delete *it;
			Timelines.erase(it);
			break;
		}
	}
}

CIwGameAnimTimeline* CIwGameAnimTimelinesManager::findTimeline(unsigned int name_hash)
{
	for (_Iterator it = Timelines.begin(); it != Timelines.end(); ++it)
	{
		if ((*it)->getNameHash() == name_hash)
			return *it;
	}

	return NULL;
}

CIwGameAnimTimeline* CIwGameAnimTimelinesManager::findTimeline(const char* name)
{
	return findTimeline(CIwGameString::CalculateHash(name));
}

void CIwGameAnimTimelinesManager::clearTimelines()
{
	// Remove and delete all timelines from the manager
	for (_Iterator it = Timelines.begin(); it != Timelines.end(); ++it)
		delete *it;
	Timelines.clear();
}

CIwGameAnimTimeline* CIwGameAnimTimelinesManager::FindTimeline(const char* name, IIwGameXomlResource* container)
{
	return FindTimeline(IW_GAME_HASH(name), container);
}

CIwGameAnimTimeline* CIwGameAnimTimelinesManager::FindTimeline(unsigned int name_hash, IIwGameXomlResource* container)
{
	CIwGameAnimTimeline* timeline = NULL;
	
	if (container != NULL)
	{
		// If container class is an actor then check the actors scenes timelines manager
		if (container->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		{
			timeline = ((CIwGameActor*)container)->getTimelinesManager()->findTimeline(name_hash);

			// If not found in the actor then check the actors scene
			if (timeline == NULL)
				container = ((CIwGameActor*)container)->getScene();
		}

		// If container class is a scene then check the scenes timelines manager
		if (timeline == NULL && container->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
			timeline = ((CIwGameScene*)container)->getTimelinesManager()->findTimeline((name_hash));
	}

	// If the timelines has no container or the timelines was not found in the supplied container then check the global timelines manager
	if (timeline == NULL)
		timeline = (CIwGameAnimTimeline*)IW_GAME_GLOBAL_RESOURCES->getTimelinesManager()->findTimeline((name_hash));

	return timeline;
}

CIwGameAnimTimeline*	CIwGameAnimTimelinesManager::FindTimeline(const char* name, CIwGameScene* container, const char* actor_name)
{
	return FindTimeline(IW_GAME_HASH(name), container, IW_GAME_HASH(actor_name));
}

CIwGameAnimTimeline* CIwGameAnimTimelinesManager::FindTimeline(unsigned int name_hash, CIwGameScene* scene, unsigned int actor_hash)
{
	CIwGameAnimTimeline* timeline = NULL;
	
	if (scene != NULL)
	{
		CIwGameActor* actor = scene->findActor(actor_hash);
		if (actor != NULL)
			timeline = actor->getTimelinesManager()->findTimeline((name_hash));
		else
			timeline = scene->getTimelinesManager()->findTimeline((name_hash));
	}

	if (timeline == NULL)
		timeline = (CIwGameAnimTimeline*)IW_GAME_GLOBAL_RESOURCES->getTimelinesManager()->findTimeline((name_hash));

	return timeline;
}












