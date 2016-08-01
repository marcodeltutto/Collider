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

#include "IwGameUISlider.h"
#include "IwGameUtil.h"
#include "IwGameString.h"
#include "IwGame.h"

void CIwGameUISlider::setValue(float pos)
{
	if (pos < ValueRange.x)
		pos = ValueRange.x;
	if (pos > ValueRange.y)
		pos = ValueRange.y;

	Value = pos;

	float range = ValueRange.y - ValueRange.x;
	if (SliderType == SliderType_Horizontal)
	{
		float x = ((Value - ValueRange.x) * SliderSize) / range - SliderSize / 2;
		setPosition(x, Position.y);
	}
	else
	if (SliderType == SliderType_Vertical)
	{
		float y = ((ValueRange.y - (Value - ValueRange.x)) * SliderSize) / range - SliderSize / 2;
		setPosition(Position.x, y);
	}
}

void CIwGameUISlider::setValueRange(CIwFVec2& range)
{
	ValueRange = range;

	float drange = ValueRange.y - ValueRange.x;

	if (SliderType == SliderType_Horizontal)
	{
		float x = ((Value - ValueRange.x) * SliderSize) / drange - SliderSize / 2;
		setPosition(x, Position.y);
	}
	else
	if (SliderType == SliderType_Vertical)
	{
		float y = ((ValueRange.y - (Value - ValueRange.x)) * SliderSize) / drange - SliderSize / 2;
		setPosition(Position.x, y);
	}

}

bool CIwGameUISlider::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - Slider needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameUIBase::LoadFromXoml(parent, false, node))
		return false;

	// Process slider specific attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Value_Hash)
		{
			Value = (*it)->GetValueAsFloat();
		}
		else
		if (name_hash == CIwGameXomlNames::ValueRange_Hash)
		{
			if (!(*it)->GetValueAsPoint(ValueRange))
				CIwGameError::LogError("Warning: XOML - Invalid value for Slider::ValueRange expected vec2");
		}
		else
		if (name_hash == CIwGameXomlNames::SliderType_Hash)
		{
			unsigned int type_hash = (*it)->GetValue().getHash();

			if (type_hash == IW_GAME_HASH("vertical"))
				SliderType = SliderType_Vertical;
			else
				SliderType = SliderType_Horizontal;
		}
		else
		if (name_hash == CIwGameXomlNames::SliderSize_Hash)
		{
			SliderSize = (*it)->GetValueAsInt();
		}
		else
		if (name_hash == CIwGameXomlNames::OnValueChanged_Hash)
			EventsManager->addEvent("OnValueChanged", (*it)->GetValue().c_str());
	}

	OriginalSliderSize = SliderSize;

	if (LinkedTo != NULL)
	{
		if (SliderSize < 0 && SliderType == SliderType_Horizontal)
			SliderSize = (LinkedTo->getSize().x * -SliderSize) / 100;
		else
		if (SliderSize < 0 && SliderType == SliderType_Vertical)
			SliderSize = (LinkedTo->getSize().y * -SliderSize) / 100;
	}
	else
	{
		// If negative size has been specified then base size on a percentage of virtual canvas
		if (SliderSize < 0 && SliderType == SliderType_Horizontal)
			SliderSize = ((Scene->getScreenSize().x * IW_GEOM_ONE) / Scene->getVirtualTransform().m[0][0] * -SliderSize) / 100;
		else
		if (SliderSize < 0 && SliderType == SliderType_Vertical)
			SliderSize = ((Scene->getScreenSize().y * IW_GEOM_ONE) / Scene->getVirtualTransform().m[1][1] * -SliderSize) / 100;
	}

	setValue(Value);

	if (load_children)
	{
		// Process inner tags
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}
	return true;
}

bool CIwGameUISlider::setProperty(unsigned int element_name, CIwGameString& data, bool delta)
{
	if (CIwGameUIBase::setProperty(element_name, data, delta))
		return true;

	float float_pool[8];

	if (element_name == CIwGameXomlNames::SliderSize_Hash)
	{
		SliderSize = data.GetAsInt();
	}
	else
	if (element_name == CIwGameXomlNames::Value_Hash)
	{
		Value = data.GetAsFloat();
	}
	else
	if (element_name == CIwGameXomlNames::ValueRange_Hash)
	{
		data.GetAsListOfFloat(float_pool);
		ValueRange.x = float_pool[0];
		ValueRange.y = float_pool[1];
	}
	else
	if (element_name == CIwGameXomlNames::SliderType_Hash)
	{
		unsigned int type_hash = data.getHash();

		if (type_hash == IW_GAME_HASH("vertical"))
			SliderType = SliderType_Vertical;
		else
			SliderType = SliderType_Horizontal;
	}
	else
		return false;

	return true;
}

bool CIwGameUISlider::UpdateBinding(unsigned int element_name, CIwGameXomlVariable* var)
{
	// Two way binding for Value (passes the value back to the variable)
	if (ThumbChanged)
	{
		if (element_name == CIwGameXomlNames::Value_Hash)
		{
			if (var->Type == VT_Float)
				var->setValue(CIwGameString(Value).c_str());	// Write Value back to bound variable
			return true;
		}
	}

	if (!var->Modified || var->getValue().IsEmpty())
		return false;
	if (CIwGameUIBase::UpdateBinding(element_name, var))
		return true;

	if (element_name == CIwGameXomlNames::SliderSize_Hash)
	{
		if (var->Type == VT_Int)
			SliderSize = ((CIwGameXomlVariableInt*)var)->NativeValue;
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Slider Incorrect binding variable type, expected int for SliderSize");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::Value_Hash)
	{
		if (var->Type == VT_Float)
			setValue(((CIwGameXomlVariableFloat*)var)->NativeValue);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Slider Incorrect binding variable type, expected float for Value");
#endif // _DEBUG
	}
	else
	if (element_name == CIwGameXomlNames::ValueRange_Hash)
	{
		if (var->Type == VT_Vec2)
			ValueRange = ((CIwGameXomlVariableVec2*)var)->NativeValue;
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Slider Incorrect binding variable type, expected vec2 for ValueRange");
#endif // _DEBUG
	}
	else
		return false;

	return true;
}

bool CIwGameUISlider::Update(float dt)
{
	CIwFVec2 pos = Position;
	
	if (!CIwGameUIBase::Update(dt))
		return false;
	
	float old = Value;
	float range = ValueRange.y - ValueRange.x;
	float x1 = -(float)SliderSize / 2;
	float x2 = (float)SliderSize / 2;

	if (SliderType == SliderType_Horizontal)
	{
		float x = Position.x;
		if (x < x1)
			x = x1;
		if (x > x2)
			x = x2;
		Value = ValueRange.x + ((x - x1) * range) / SliderSize;

		setPosition(x, pos.y);
	}
	else
	if (SliderType == SliderType_Vertical)
	{
		float y = Position.y;
		if (y < x1)
			y = x1;
		if (y > x2)
			y = x2;
		Value = ValueRange.y - (ValueRange.x + ((y - x1) * range) / SliderSize);

		setPosition(pos.x, y);
	}

	if (fabs(old - Value) > 0.0001f)
	{
		ThumbChanged = true;
		OnValueChanged(old, Value);
	}
	else
		ThumbChanged = false;

	return true;
}

void CIwGameUISlider::OnBeginTouch(int index, int x, int y, bool allow_bubble)
{
	if (TouchIndex >= 0)
		return;

	CIwGameUIBase::OnBeginTouch(index, x, y, allow_bubble);
}

void CIwGameUISlider::OnEndTouch(int index, int x, int y, bool allow_bubble)
{
	CIwGameUIBase::OnEndTouch(index, x, y, allow_bubble);
}

void CIwGameUISlider::OnValueChanged(float old_value, float new_value)
{
	ProcessEventActions(CIwGameXomlNames::OnValueChanged_Hash);
}

void CIwGameUISlider::OnOrientationChange(eIwGameScene_Orientation old_orientation, eIwGameScene_Orientation new_orientation)
{
	CIwGameActorImage::OnOrientationChange(old_orientation, new_orientation);

	if (LinkedTo != NULL)
	{
		if (OriginalSliderSize < 0 && SliderType == SliderType_Horizontal)
			SliderSize = (LinkedTo->getSize().x * -OriginalSliderSize) / 100;
		else
		if (OriginalSliderSize < 0 && SliderType == SliderType_Vertical)
			SliderSize = (LinkedTo->getSize().y * -OriginalSliderSize) / 100;
	}
	else
	{
		// If negative size has been specified then base size on a percentage of virtual canvas
		if (OriginalSliderSize < 0 && SliderType == SliderType_Horizontal)
			SliderSize = ((Scene->getScreenSize().x * IW_GEOM_ONE) / Scene->getVirtualTransform().m[0][0] * -OriginalSliderSize) / 100;
		else
		if (OriginalSliderSize < 0 && SliderType == SliderType_Vertical)
			SliderSize = ((Scene->getScreenSize().y * IW_GEOM_ONE) / Scene->getVirtualTransform().m[1][1] * -OriginalSliderSize) / 100;
	}

}
