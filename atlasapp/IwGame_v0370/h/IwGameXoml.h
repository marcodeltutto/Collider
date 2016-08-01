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

#if !defined(_IW_GAME_XOML_H_)
#define _IW_GAME_XOML_H_

#include "s3eFile.h"
#include "IwGameString.h"
#include "IwGameUtil.h"
#include "IwGameXml.h"

class CIwGame;
class CIwGameStyle;
class CIwGameXomlVariableManager;
class CIwGameXomlResourceManager;
class CIwGameScene;
class CIwGameFont;
struct CIwGameAction;
struct CIwGameXomlVariable;
struct CIwGameXomlVariableXML;

// 
// 
//
//
// CIwGameXomlNames - pre-hashed strings of classes, attributes and certain attributes data used to speed up string searchimng
// 
// 
//
//
struct CIwGameXomlNames
{
public:
	static unsigned int Game_Hash;
	static unsigned int Scene_Hash;
	static unsigned int Actor_Hash;
	static unsigned int ActorImage_Hash;
	static unsigned int ActorText_Hash;
	static unsigned int ResourceGroup_Hash;
	static unsigned int Animation_Hash;
	static unsigned int Box2dMaterial_Hash;
	static unsigned int Joints_Hash;
	static unsigned int Joint_Hash;
	static unsigned int Shape_Hash;
	static unsigned int Actions_Hash;
	static unsigned int Camera_Hash;
	static unsigned int Image_Hash;
	static unsigned int Brush_Hash;
	static unsigned int Font_Hash;
	static unsigned int Video_Hash;
	static unsigned int Timeline_Hash;
	static unsigned int Style_Hash;
	static unsigned int Variable_Hash;
	static unsigned int Bindings_Hash;
	static unsigned int Binding_Hash;
	static unsigned int Modifier_Hash;
	static unsigned int Tabs_Hash;

	static unsigned int Resource_Hash;
	static unsigned int Tag_Hash;
	static unsigned int Count_Hash;
	static unsigned int Action_Hash;
	static unsigned int Name_Hash;
	static unsigned int ResourceName_Hash;
	static unsigned int File_Hash;
	static unsigned int Current_Hash;
	static unsigned int CanvasSize_Hash;
	static unsigned int CanvasFit_Hash;
	static unsigned int CanvasOrigin_Hash;
	static unsigned int FixAspect_Hash;
	static unsigned int LockWidth_Hash;
	static unsigned int Extents_Hash;
	static unsigned int AllowSuspend_Hash;
	static unsigned int Clipping_Hash;
	static unsigned int ClipStatic_Hash;
	static unsigned int Active_Hash;
	static unsigned int Visible_Hash;
	static unsigned int Margin_Hash;
	static unsigned int TextMargin_Hash;
	static unsigned int ClipMargin_Hash;
	static unsigned int Layers_Hash;
	static unsigned int Colliders_Hash;
	static unsigned int Type_Hash;
	static unsigned int Position_Hash;
	static unsigned int PositionX_Hash;
	static unsigned int PositionY_Hash;
	static unsigned int Docking_Hash;
	static unsigned int Pitch_Hash;
	static unsigned int Origin_Hash;
	static unsigned int Skew_Hash;
	static unsigned int ShowTimeline_Hash;
	static unsigned int HideTimeline_Hash;
	static unsigned int TextSkew_Hash;
	static unsigned int BeforeChildren_Hash;
	static unsigned int GridPos_Hash;
	static unsigned int Velocity_Hash;
	static unsigned int VelocityDamping_Hash;
	static unsigned int Angle_Hash;
	static unsigned int AngularVelocity_Hash;
	static unsigned int AngularVelocityDamping_Hash;
	static unsigned int Depth_Hash;
	static unsigned int DepthVelocity_Hash;
	static unsigned int DepthVelocityDamping_Hash;
	static unsigned int Scale_Hash;
	static unsigned int ScaleX_Hash;
	static unsigned int ScaleY_Hash;
	static unsigned int ScaleVelocity_Hash;
	static unsigned int ScaleVelocityDamping_Hash;
	static unsigned int Color_Hash;
	static unsigned int Colour_Hash;
	static unsigned int BackgroundColour_Hash;
	static unsigned int SelectedColour_Hash;
	static unsigned int DisabledColour_Hash;
	static unsigned int TextColour_Hash;
	static unsigned int SelectedTextColour_Hash;
	static unsigned int DisabledTextColour_Hash;
	static unsigned int ColourVelocity_Hash;
	static unsigned int ColourVelocityDamping_Hash;
	static unsigned int Layer_Hash;
	static unsigned int Draggable_Hash;
	static unsigned int Collidable_Hash;
	static unsigned int CollisionSize_Hash;
	static unsigned int CollisionRect_Hash;
	static unsigned int WrapPosition_Hash;
	static unsigned int Size_Hash;
	static unsigned int AspectLock_Hash;
	static unsigned int Orientation_Hash;
	static unsigned int Text_Hash;
	static unsigned int SelectedText_Hash;
	static unsigned int DisabledText_Hash;
	static unsigned int GroupFile_Hash;
	static unsigned int Frame_Hash;
	static unsigned int Atlas_Hash;
	static unsigned int Value_Hash;
	static unsigned int ValueRange_Hash;
	static unsigned int Time_Hash;
	static unsigned int Repeat_Hash;
	static unsigned int Duration_Hash;
	static unsigned int SpawnDelay_Hash;
	static unsigned int SrcRect_Hash;
	static unsigned int Location_Hash;
	static unsigned int Preload_Hash;
	static unsigned int Blocking_Hash;
	static unsigned int FileType_Hash;
	static unsigned int Converter_Hash;
	static unsigned int AutoPlay_Hash;
	static unsigned int Volume_Hash;
	static unsigned int PointSize_Hash;
	static unsigned int AutoPointSize_Hash;
	static unsigned int Anim_Hash;
	static unsigned int StartAtTime_Hash;
	static unsigned int TimeScale_Hash;
	static unsigned int Delta_Hash;
	static unsigned int Interpolate_Hash;
	static unsigned int Easing_Hash;
	static unsigned int Target_Hash;
	static unsigned int TargetA_Hash;
	static unsigned int TargetB_Hash;
	static unsigned int UseParentOpacity_Hash;
	static unsigned int Width_Hash;
	static unsigned int Height_Hash;
	static unsigned int Radius_Hash;
	static unsigned int Points_Hash;
	static unsigned int Point_Hash;
	static unsigned int RowDefinition_Hash;
	static unsigned int ColumnDefinition_Hash;
	static unsigned int Particle_Hash;
	static unsigned int Data_Hash;
	static unsigned int Density_Hash;
	static unsigned int Friction_Hash;
	static unsigned int Restitution_Hash;
	static unsigned int IsBullet_Hash;
	static unsigned int FixedRotation_Hash;
	static unsigned int GravityScale_Hash;
	static unsigned int COM_Hash;
	static unsigned int Gravity_Hash;
	static unsigned int DoSleep_Hash;
	static unsigned int Mode_Hash;
	static unsigned int WorldScale_Hash;
	static unsigned int Physics_Hash;
	static unsigned int Sensor_Hash;
	static unsigned int CollisionFlags_Hash;
	static unsigned int Set_Hash;
	static unsigned int Property_Hash;
	static unsigned int HitTest_Hash;
	static unsigned int FlipX_Hash;
	static unsigned int FlipY_Hash;
	static unsigned int LinkedTo_Hash;
	static unsigned int Template_Hash;
	static unsigned int Prefix_Hash;
	static unsigned int AlignH_Hash;
	static unsigned int AlignV_Hash;
	static unsigned int SizeToContent_Hash;
	static unsigned int Wrap_Hash;
	static unsigned int Italic_Hash;
	static unsigned int Underline_Hash;
	static unsigned int Command_Hash;
	static unsigned int Method_Hash;
	static unsigned int Param1_Hash;
	static unsigned int Param2_Hash;
	static unsigned int Param3_Hash;
	static unsigned int Param4_Hash;
	static unsigned int Param5_Hash;
	static unsigned int Condition_Hash;
	static unsigned int BindXML_Hash;
	static unsigned int Local_Hash;
	static unsigned int Format_Hash;
	static unsigned int Codec_Hash;
	static unsigned int Filter_Hash;
	static unsigned int AlphaMode_Hash;
	static unsigned int AutoWidth_Hash;
	static unsigned int AutoHeight_Hash;
	static unsigned int Batch_Hash;
	static unsigned int AllowFocus_Hash;
	static unsigned int TouchPanX_Hash;
	static unsigned int TouchPanY_Hash;
	static unsigned int IgnoreActors_Hash;
	static unsigned int ActorB_Hash;
	static unsigned int OffsetA_Hash;
	static unsigned int OffsetB_Hash;
	static unsigned int GroundA_Hash;
	static unsigned int GroundB_Hash;
	static unsigned int SelfCollide_Hash;
	static unsigned int Distance_Hash;
	static unsigned int Revolute_Hash;
	static unsigned int Prismatic_Hash;
	static unsigned int Pulley_Hash;
	static unsigned int Gear_Hash;
	static unsigned int Wheel_Hash;
	static unsigned int Rope_Hash;
	static unsigned int Length_Hash;
	static unsigned int LengthA_Hash;
	static unsigned int LengthB_Hash;
	static unsigned int Ratio_Hash;
	static unsigned int Frequency_Hash;
	static unsigned int Damping_Hash;
	static unsigned int MotorEnabled_Hash;
	static unsigned int LimitJoint_Hash;
	static unsigned int LowerLimit_Hash;
	static unsigned int UpperLimit_Hash;
	static unsigned int MotorSpeed_Hash;
	static unsigned int MaxMotorTorque_Hash;
	static unsigned int MaxMotorForce_Hash;
	static unsigned int ReferenceAngle_Hash;
	static unsigned int Axis_Hash;
	static unsigned int Force_Hash;
	static unsigned int LinearImpulse_Hash;
	static unsigned int Torque_Hash;
	static unsigned int AngularImpulse_Hash;
	static unsigned int Solid_Hash;
	static unsigned int Gradient_Hash;
	static unsigned int Patch9_Hash;
	static unsigned int ScaleArea_Hash;
	static unsigned int Activate_Hash;
	static unsigned int Deactivate_Hash;
	static unsigned int Toggle_Hash;
	static unsigned int Sticky_Hash;
	static unsigned int Enabled_Hash;
	static unsigned int Remove_Hash;
	static unsigned int ItemsData_Hash;
	static unsigned int ItemsTargetType_Hash;
	static unsigned int ItemsTemplate_Hash;
	static unsigned int SelectType_Hash;
	static unsigned int Selected_Hash;
	static unsigned int Spring_Hash;
	static unsigned int Bubbling_Hash;
	static unsigned int KeyFocus_Hash;
	static unsigned int ScrollRange_Hash;
	static unsigned int ScrollPos_Hash;
	static unsigned int Normal_Hash;
	static unsigned int Background_Hash;
	static unsigned int SelectedBackground_Hash;
	static unsigned int DisabledBackground_Hash;
	static unsigned int CaretColourOffset_Hash;
	static unsigned int TypeHint_Hash;
	static unsigned int Prompt_Hash;
	static unsigned int Icon_Hash;
	static unsigned int Label_Hash;
	static unsigned int TextBox_Hash;
	static unsigned int StackPanel_Hash;
	static unsigned int Grid_Hash;
	static unsigned int ListBox_Hash;
	static unsigned int Canvas_Hash;
	static unsigned int MultiSelect_Hash;
	static unsigned int AutoRun_Hash;
	static unsigned int Priority_Hash;
	static unsigned int Parallel_Hash;
	static unsigned int IfReturn_Hash;
	static unsigned int Selection_Hash;
	static unsigned int SelectedIndex_Hash;
	static unsigned int SliderType_Hash;
	static unsigned int SliderSize_Hash;
	static unsigned int ImageBrush_Hash;
	static unsigned int Area_Hash;
	static unsigned int Zoom_Hash;
	static unsigned int MinZoom_Hash;
	static unsigned int MaxZoom_Hash;
	static unsigned int URI_Hash;
	static unsigned int Modal_Hash;
	static unsigned int JavaScript_Hash;
	static unsigned int Transparent_Hash;
	static unsigned int Opacity_Hash;
	static unsigned int AutoHide_Hash;
	static unsigned int View_Hash;
	
	static unsigned int OnTapped_Hash;
	static unsigned int OnTapped2_Hash;
	static unsigned int OnTapped3_Hash;
	static unsigned int OnTapped4_Hash;
	static unsigned int OnTapped5_Hash;
	static unsigned int OnBeginTouch_Hash;
	static unsigned int OnBeginTouch2_Hash;
	static unsigned int OnBeginTouch3_Hash;
	static unsigned int OnBeginTouch4_Hash;
	static unsigned int OnBeginTouch5_Hash;
	static unsigned int OnEndTouch_Hash;
	static unsigned int OnEndTouch2_Hash;
	static unsigned int OnEndTouch3_Hash;
	static unsigned int OnEndTouch4_Hash;
	static unsigned int OnEndTouch5_Hash;
	static unsigned int OnSuspend_Hash;
	static unsigned int OnResume_Hash;
	static unsigned int OnPause_Hash;
	static unsigned int OnLostFocus_Hash;
	static unsigned int OnGainedFocus_Hash;
	static unsigned int OnStart_Hash;
	static unsigned int OnEnd_Hash;
	static unsigned int OnRepeat_Hash;
	static unsigned int OnCreate_Hash;
	static unsigned int OnDestroy_Hash;
	static unsigned int OnOrientationChange_Hash;
	static unsigned int OnKeyBack_Hash;
	static unsigned int OnKeyMenu_Hash;
	static unsigned int OnTextChanged_Hash;
	static unsigned int OnToggledOn_Hash;
	static unsigned int OnToggledOff_Hash;
	static unsigned int OnBackKey_Hash;
	static unsigned int OnMenuKey_Hash;
	static unsigned int OnSelectionChanged_Hash;
	static unsigned int OnValueChanged_Hash;
	static unsigned int OnViewChanged_Hash;
	static unsigned int OnCollisionStart_Hash;
	static unsigned int OnCollisionEnd_Hash;

	static unsigned int ChangeScene_Hash;
	static unsigned int SuspendScene_Hash;
	static unsigned int ResumeScene_Hash;
	static unsigned int HideScene_Hash;
	static unsigned int ShowScene_Hash;
	static unsigned int ActivateScene_Hash;
	static unsigned int DeactivateScene_Hash;
	static unsigned int KillScene_Hash;
	static unsigned int HideActor_Hash;
	static unsigned int ShowActor_Hash;
	static unsigned int ActivateActor_Hash;
	static unsigned int DeactivateActor_Hash;
	static unsigned int KillActor_Hash;
	static unsigned int PlayTimeline_Hash;
	static unsigned int StopTimeline_Hash;
	static unsigned int SetTimeline_Hash;
	static unsigned int PlaySound_Hash;
	static unsigned int PlayMusic_Hash;
	static unsigned int StopMusic_Hash;
	static unsigned int SetVar_Hash;

	static unsigned int Start_Hash;
	static unsigned int Stop_Hash;
	static unsigned int Pause_Hash;
	static unsigned int Next_Hash;
	static unsigned int Goto_Hash;

	static unsigned int bool_Hash;
	static unsigned int float_Hash;
	static unsigned int int_Hash;
	static unsigned int vec2_Hash;
	static unsigned int vec3_Hash;
	static unsigned int vec4_Hash;
	static unsigned int rect_Hash;
	static unsigned int string_Hash;
	static unsigned int condition_Hash;
	static unsigned int arraystring_Hash;
	static unsigned int arraybool_Hash;
	static unsigned int arrayfloat_Hash;
	static unsigned int arrayint_Hash;
	static unsigned int xml_Hash;
	static unsigned int box_Hash;
	static unsigned int circle_Hash;
	static unsigned int polygon_Hash;
	static unsigned int static_Hash;
	static unsigned int dynamic_Hash;
	static unsigned int kinematic_Hash;
	static unsigned int Linear_Hash;
	static unsigned int QuadIn_Hash;
	static unsigned int QuadOut_Hash;
	static unsigned int CubicIn_Hash;
	static unsigned int CubicOut_Hash;
	static unsigned int QuarticIn_Hash;
	static unsigned int QuarticOut_Hash;
	static unsigned int left_Hash;
	static unsigned int right_Hash;
	static unsigned int centre_Hash;
	static unsigned int top_Hash;
	static unsigned int middle_Hash;
	static unsigned int bottom_Hash;
};

// 
// 
//
//
// IIwGameXomlResource - Base class that is used by classes that can be instanced from XOML markup
// 
// To create a class that can be instantiated from XOML you should:
// 1. Derive the class from CIwGameXomlClass
// 2. Set its XOML base type (game, actor, scene, animation etc..)
// 3. Implement a class derived from IIwGameXomlClassCreator that implements CreateInstance() which creates an instance of the class
// 4. Add the class type to the list of class creators in IW_GAME_XOML using addClass()
//
//
//
//
class IIwGameXomlResource
{
	// Properties
protected:
#if defined (_DEBUG)
	CIwGameString			Name;					// Resource name
	CIwGameString			ClassType;				// Resource class type
	CIwGameString			ActualClassType;		// Actual class type
	CIwGameString			Tag;					// Tag name
#endif	// _DEBUG
	CIwGameXomlResourceManager* Parent;				// Parent manager
	bool					Managed;				// True is managed by the resource system. Managed resources are auto clean ed up by the resource system
	unsigned int			NameHash;				// Resource name hash
	unsigned int			ClassTypeHash;			// Base type of XOML class (e.g. Scene, Actor, Animation, Game etc..)
	unsigned int			ActualClassTypeHash;	// Actual type of XOML class
	unsigned int			TagHash;				// Tag hash, used to put resources into loadnig groups
public:
	void					setParent(CIwGameXomlResourceManager* parent)	{ Parent = parent; }
	CIwGameXomlResourceManager*getParent()							{ return Parent; }
	void					setName(const char* name)
	{
#if defined (_DEBUG)
		Name = name;
#endif	// _DEBUG
		NameHash = CIwGameString::CalculateHash(name);
	}
#if defined (_DEBUG)
	CIwGameString&			getName()								{ return Name; }
#endif	// _DEBUG
 	unsigned int			getNameHash()							{ return NameHash; }
	void					setClassType(const char* type_name)
	{
#if defined (_DEBUG)
		ClassType = type_name;
		ActualClassType = type_name;
#endif	// _DEBUG
		ClassTypeHash = CIwGameString::CalculateHash(type_name);
		ActualClassTypeHash = ClassTypeHash;
	}
	unsigned int			getClassTypeHash()						{ return ClassTypeHash; }
	void					setActualClassType(const char* type_name)
	{
#if defined (_DEBUG)
		ActualClassType = type_name;
#endif	// _DEBUG
		ActualClassTypeHash = CIwGameString::CalculateHash(type_name);
	}
	unsigned int			getActualClassTypeHash()				{ return ActualClassTypeHash; }
	void					setTag(const char* Tag)
	{
#if defined (_DEBUG)
		Tag = Tag;
#endif	// _DEBUG
		TagHash = CIwGameString::CalculateHash(Tag);
	}
#if defined (_DEBUG)
	CIwGameString&			getTag()								{ return Tag; }
#endif	// _DEBUG
	unsigned int			getTagHash()							{ return TagHash; }
	void					setManaged(bool managed)				{ Managed = managed; }
	bool					isManaged() const						{ return Managed; }
	// Properties end
protected:
public:
	IIwGameXomlResource() : Managed(false) {}
	virtual ~IIwGameXomlResource()
	{
	}

	virtual bool LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node) = 0;
	virtual bool PostLoadFromXoml(IIwGameXomlResource* parent, CIwGameXmlNode* node) { return true; }

	// Internal
};

// 
// 
//
//
// IIwGameXomlClassCreator - Base class that is used by classes that create an instance of a XOML resource type
//
//
//
//
class IIwGameXomlClassCreator
{
	// Properties
protected:
#if defined(_DEBUG)
	CIwGameString			ClassName;				// Name of class
#endif
	unsigned int			ClassNameHash;			// Hashed name of class
public:
	void					setClassName(const char* name)
	{
#if defined(_DEBUG)
		ClassName = name;
#endif
		ClassNameHash = CIwGameString::CalculateHash(name);
	}
	unsigned int			getClassNameHash() const		{ return ClassNameHash; }
#if defined(_DEBUG)
	CIwGameString&			getClassName()					{ return ClassName; }
#endif
	// Properties end
public:
	virtual IIwGameXomlResource* CreateInstance(IIwGameXomlResource* parent) = 0;
};

//
//
//
//
//  CIwGameXomlCondition - A condition represents the true or false state of a specific set of variables, operators and values
//
//
//
//
enum eIwGameXomlConditionOperator
{
	CO_None, 
	CO_Equal, 
	CO_NotEqual, 
	CO_Greater, 
	CO_GreaterEqual, 
	CO_Less, 
	CO_LessEqual, 
	CO_And, 
};
enum eIwGameXomlConditionJoinOp
{
	CJO_None, 
	CJO_And, 
	CJO_Or, 
};
struct CIwGameXomlCondition
{
	CIwGameXomlVariable*			Variable;			// The variable to check
	CIwGameXomlVariable*			Variable2;			// The variable to check against
	eIwGameXomlConditionOperator	Operator;			// Determines how the variable and its value should be checked
	eIwGameXomlConditionJoinOp		Join;				// Join specifies the operator that should be applied to the next condition in a list of conditions
	CIwGameString					Value;				// Value to compare variable against

	CIwGameXomlCondition() : Operator(CO_None), Join(CJO_None), Variable(NULL), Variable2(NULL)	{}
};

typedef CIwList<CIwGameXomlCondition*> CIwGameXomlConditions;

// 
// 
//
//
// CIwGameXomlVariable - A basic XOML variable
//
//
//
//
enum eCIwGameXomlVariableType
{
	VT_String, 
	VT_Bool, 
	VT_Float, 
	VT_Int, 
	VT_Vec2, 
	VT_Vec3, 
	VT_Vec4, 
	VT_ArrayString, 
	VT_ArrayBool, 
	VT_ArrayFloat, 
	VT_ArrayInt, 
	VT_Condition, 
	VT_XML, 
};

struct CIwGameXomlVariable
{
	// Properties
protected:
	CIwGameString				Value;				// Value of variable
	bool						Changed;			// Internal changed status
public:
	virtual void				setValue(const char* value)		{ Value = value; Changed = true; }
	virtual void				addValue(const char* value)		{ Value += value; Changed = true; }
	virtual void				addValue(const char* value, const char* limit)	{ Value += value; Changed = true; }
	CIwGameString&				getValue()						{ return Value; }
	virtual bool				isTrue()						{ return !Value.IsEmpty(); }
	virtual bool				checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val);
	// Properties end
	CIwGameXomlVariableManager* Parent;				// Parent manager
	CIwGameString				Name;				// Name of variable
	eCIwGameXomlVariableType	Type;				// Type of variable
	bool						Modified;			// Modified since last read flag

	CIwGameXomlVariable() : Type(VT_String), Modified(false), Changed(true) {}
	virtual ~CIwGameXomlVariable() {}

	virtual void				Update()
	{
		if (Changed)
		{
			Modified = true;
			Changed = false;
		}
		else
			Modified = false;
	}

	bool						isArray() const			{ return Type >= VT_ArrayString && Type <= VT_ArrayInt; }
	bool						isXML() const			{ return Type == VT_XML; }

	// Utility
	static CIwGameXomlVariable* GetVariable(CIwGameString& value, IIwGameXomlResource* parent);

};

struct CIwGameXomlVariableBool : CIwGameXomlVariable
{
	bool						NativeValue;		// Native value of the variable
	CIwGameXomlVariableBool() : CIwGameXomlVariable() { Type = VT_Bool; }
	void						setValue(const char* value)
	{
		Value = value;
		NativeValue = Value.GetAsBool();
		Changed = true;
	}
	void					addValue(const char* value)
	{
		NativeValue = !NativeValue;
		if (NativeValue)
			Value = "true";
		else
			Value = "false";
		Changed = true;
	}
	bool					isTrue()			{ return NativeValue; }
	bool					checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val);
};


struct CIwGameXomlVariableFloat : CIwGameXomlVariable
{
	float						NativeValue;		// Native value of the variable
	CIwGameXomlVariableFloat() : CIwGameXomlVariable()  { Type = VT_Float; }
	void						setValue(const char* value)
	{
		Value = value;
		NativeValue = Value.GetAsFloat();
		Changed = true;
	}
	void						addValue(const char* value)
	{
		NativeValue += CIwGameString(value).GetAsFloat();
		Value = NativeValue;
		Changed = true;
	}
	void						addValue(const char* value, const char* limit)
	{
		float val = CIwGameString(value).GetAsFloat();
		if (val == 0)
			return;
		float lim = CIwGameString(limit).GetAsFloat();
	
		NativeValue += val;
		if (val < 0)
		{
			if (NativeValue < lim)
				NativeValue = lim;
		}
		else
		if (val > 0)
		{
			if (NativeValue > lim)
				NativeValue = lim;
		}
		Value = NativeValue;
		Changed = true;
	}
	bool						isTrue()			{ return NativeValue != 0; }
	bool						checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val);
};

struct CIwGameXomlVariableInt : CIwGameXomlVariable
{
	int							NativeValue;		// Native value of the variable
	CIwGameXomlVariableInt() : CIwGameXomlVariable()  { Type = VT_Int; }
	void						setValue(const char* value)
	{
		Value = value;
		NativeValue = Value.GetAsInt();
		Changed = true;
	}
	void						addValue(const char* value)
	{
		NativeValue += CIwGameString(value).GetAsInt();
		Value = NativeValue;
		Changed = true;
	}
	void						addValue(const char* value, const char* limit)
	{
		int val = CIwGameString(value).GetAsInt();
		int lim = CIwGameString(limit).GetAsInt();
		NativeValue += val;
		if (val < 0)
		{
			if (NativeValue < lim)
				NativeValue = lim;
		}
		else
		if (val > 0)
		{
			if (NativeValue > lim)
				NativeValue = lim;
		}
		Value = NativeValue;
		Changed = true;
	}
	bool						isTrue()			{ return NativeValue != 0; }
	bool						checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val);
};

struct CIwGameXomlVariableVec2 : CIwGameXomlVariable
{
	CIwFVec2					NativeValue;		// Native value of the variable
	CIwGameXomlVariableVec2() : CIwGameXomlVariable()  { Type = VT_Vec2; }
	void						setValue(const char* value)
	{
		Value = value;
		float components[8];
		if (Value.GetAsListOfFloat(components) == 2)
		{
			NativeValue.x = components[0];
			NativeValue.y = components[1];
			Changed = true;
		}
	}
	void						addValue(const char* value)
	{
		CIwGameString add = value;
		float components[8];
		if (add.GetAsListOfFloat(components) == 2)
		{
			NativeValue.x += components[0];
			NativeValue.y += components[1];
			Changed = true;
			Value = NativeValue.x;
			Value += ",";
			Value += NativeValue.y;
		}
	}
	bool						isTrue()			{ return NativeValue.x != 0 && NativeValue.y != 0; }
	bool						checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val);
};

struct CIwGameXomlVariableVec3 : CIwGameXomlVariable
{
	CIwFVec3					NativeValue;		// Native value of the variable
	CIwGameXomlVariableVec3() : CIwGameXomlVariable()  { Type = VT_Vec3; }
	void						setValue(const char* value)
	{
		Value = value;
		float components[8];
		if (Value.GetAsListOfFloat(components) == 3)
		{
			NativeValue.x = components[0];
			NativeValue.y = components[1];
			NativeValue.z = components[2];
			Changed = true;
		}
	}
	void						addValue(const char* value)
	{
		CIwGameString add = value;
		float components[8];
		if (add.GetAsListOfFloat(components) == 3)
		{
			NativeValue.x += components[0];
			NativeValue.y += components[1];
			NativeValue.z += components[2];
			Changed = true;
			Value = NativeValue.x;
			Value += ",";
			Value += NativeValue.y;
			Value += ",";
			Value += NativeValue.z;
		}
	}
	bool						isTrue()			{ return NativeValue.x != 0 && NativeValue.y != 0 && NativeValue.z != 0; }
	bool						checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val);
};

struct CIwGameXomlVariableVec4 : CIwGameXomlVariable
{
	CIwFVec4					NativeValue;		// Native value of the variable
	CIwGameXomlVariableVec4() : CIwGameXomlVariable()  { Type = VT_Vec4; }
	void						setValue(const char* value)
	{
		Value = value;
		float components[8];
		if (Value.GetAsListOfFloat(components) == 4)
		{
			NativeValue.x = components[0];
			NativeValue.y = components[1];
			NativeValue.z = components[2];
			NativeValue.w = components[3];
			Changed = true;
		}
	}
	void						addValue(const char* value)
	{
		CIwGameString add = value;
		float components[8];
		if (add.GetAsListOfFloat(components) == 4)
		{
			NativeValue.x += components[0];
			NativeValue.y += components[1];
			NativeValue.z += components[2];
			NativeValue.w += components[3];
			Changed = true;
			Value = NativeValue.x;
			Value += ",";
			Value += NativeValue.y;
			Value += ",";
			Value += NativeValue.z;
			Value += ",";
			Value += NativeValue.w;
		}
	}
	bool						isTrue()			{ return NativeValue.x != 0 && NativeValue.y != 0 && NativeValue.z != 0 && NativeValue.w != 0; }
	bool						checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val);
};

struct CIwGameXomlVariableCondition : CIwGameXomlVariable
{
	bool						NativeValue;		// Native value of a condition is boolean
	CIwGameXomlConditions		Conditions;			// Conditions that should be checked to determine true or false
	CIwGameXomlVariableCondition() : CIwGameXomlVariable()  { Type = VT_Condition; }
	~CIwGameXomlVariableCondition()
	{
		for (CIwGameXomlConditions::iterator it = Conditions.begin(); it != Conditions.end(); ++it)
			delete *it;
	}
	void						setValue(const char* value);
	void						addValue(const char* value)	{ }
	bool						isTrue();
};

struct CIwGameXomlVariableArray : public CIwGameXomlVariable
{
	// Properties
protected:
	CIwGameSlotArray<CIwGameXomlVariable*>	Values;				// Array of values
	CIwGameXomlVariableXML*		XmlVar;							// XML variable to copy attributes from
	unsigned int				XmlTagNameHash;					// XML Tag name hash
	unsigned int				XmlAttribNameHash;				// XML Attribute name hash
#if defined(_DEBUG)
	CIwGameString				XmlTagName;
	CIwGameString				XmlAttribName;
#endif
protected:
	void						UpdateFromXML();
public:
	CIwGameXomlVariable*		getElement(int index)					{ return Values.element_at(index); }
	void						addElement(CIwGameXomlVariable* var)	{ Values.add(var); }
	int							getSize() const							{ return Values.getSize(); }
	void						setValue(const char* value);
	virtual void				setValue(int index, const char* value)
	{
		if (index < 0 || index >= Values.getSize())
		{
#if defined(_DEBUG)
			CIwGameError::LogError("Error: Array index out of bounds - ", Name.c_str());
#endif	// _DEBUG
			return;
		}
		Changed = true;

		Values.element_at(index)->setValue(value);
	}
	CIwGameXomlVariableXML*		getXmlVar()								{ return XmlVar; }
	void						setXmlTagName(const char* name)
	{
#if defined(_DEBUG)
		XmlTagName = name;
#endif
		XmlTagNameHash = IW_GAME_HASH(name);
	}
	void						setXmlAttribName(const char* name)
	{
#if defined(_DEBUG)
		XmlAttribName = name;
#endif
		XmlAttribNameHash = IW_GAME_HASH(name);
	}
	void						BindXML(const char* binding);
	// Properties end

	CIwGameXomlVariableArray()
	{
		XmlVar = NULL;
		Type = VT_ArrayString;
		Modified = false;
		Changed = false;
		XmlTagNameHash = 0;
		XmlAttribNameHash = 0;
	}
	~CIwGameXomlVariableArray()
	{
		for (int t = 0; t < Values.getSize(); t++)
		{
			if (Values.element_at(t) != NULL)
				delete Values.element_at(t);
		}
	}

	virtual void	Init(int size)
	{
		Values.resize(size);

		for (int t = 0; t < size; t++)
		{
			if (Values.element_at(t) == NULL)
				Values.set(t, new CIwGameXomlVariable());
		}
	}

	void			Update()
	{
		CIwGameXomlVariable::Update();
		UpdateFromXML();
		for (int t = 0; t < Values.getSize(); t++)
		{
			CIwGameXomlVariable* var = Values.element_at(t);
			if (var != NULL)
				var->Update();
		}
	}


};

struct CIwGameXomlVariableArrayBool : public CIwGameXomlVariableArray
{
	// Properties
protected:
public:
	// Properties end

	CIwGameXomlVariableArrayBool()
	{
		Type = VT_ArrayBool;
		Modified = false;
		Changed = false;
	}

	virtual void Init(int size)
	{
		Values.resize(size);

		for (int t = 0; t < Values.getSize(); t++)
			Values.set(t, new CIwGameXomlVariableBool());
	}
};

struct CIwGameXomlVariableArrayFloat : public CIwGameXomlVariableArray
{
	// Properties
protected:
public:
	// Properties end

	CIwGameXomlVariableArrayFloat()
	{
		Type = VT_ArrayFloat;
		Modified = false;
		Changed = false;
	}

	virtual void Init(int size)
	{
		Values.resize(size);

		for (int t = 0; t < Values.getSize(); t++)
			Values.set(t, new CIwGameXomlVariableFloat());
	}
};

struct CIwGameXomlVariableArrayInt : public CIwGameXomlVariableArray
{
	// Properties
protected:
public:
	// Properties end

	CIwGameXomlVariableArrayInt()
	{
		Type = VT_ArrayInt;
		Modified = false;
		Changed = false;
	}

	virtual void Init(int size)
	{
		Values.resize(size);

		for (int t = 0; t < Values.getSize(); t++)
			Values.set(t, new CIwGameXomlVariableInt());
	}
};

struct CIwGameXomlVariableXML : CIwGameXomlVariable
{
	// Properties
protected:
	CIwGameXmlNode*		Root;					// Root node
public:
	CIwGameXmlNode*		getRoot()		{ return Root; }
	// Properties end
public:
	CIwGameXomlVariableXML() : CIwGameXomlVariable(), Root(NULL) { Type = VT_XML; }
	~CIwGameXomlVariableXML()
	{
		SAFE_DELETE(Root);
	}
	void						setValue(const char* value);
};


// 
// 
//
//
// CIwGameXomlVariableManager - Manages a collection of XOML variables
//
//
//
//
class CIwGameXomlVariableManager
{
public:
	// Public access to iteration
	typedef CIwList<CIwGameXomlVariable*>::iterator _Iterator;
	_Iterator				begin()		{ return Variables.begin(); }
	_Iterator				end()		{ return Variables.end(); }

protected:
	// Properties
	CIwList<CIwGameXomlVariable*>	Variables;			// A collection of xoml variables
	CIwGameScene*					Parent;				// Parent container scene
public:
	void					addVariable(CIwGameXomlVariable* variable);
	CIwGameXomlVariable*	addVariable(const char* name, eCIwGameXomlVariableType type = VT_String);
	CIwGameXomlVariable*	addVariable(const char* name, eCIwGameXomlVariableType type, const char* value, int size);
	void					setVariable(unsigned int name_hash, const char* value);
	void					setVariable(const char* name, const char* value);
	CIwGameXomlVariable*	findVariable(unsigned int name_hash);
	CIwGameXomlVariable*	findVariable(const char* name);
	void					clearVariables();
	void					setParent(CIwGameScene* scene)		{ Parent = scene; }
	CIwGameScene*			getParent()							{ return Parent; }
	// Properties end

public:
	CIwGameXomlVariableManager() : Parent(NULL) {}
	~CIwGameXomlVariableManager() { clearVariables(); }

	CIwGameXomlVariable*	CreateVariable(const char* name, eCIwGameXomlVariableType type = VT_String);

	void					Update()
	{
		for (_Iterator it = begin(); it != end(); ++it)
			(*it)->Update();
	}

	// Utility
	static CIwGameXomlVariable*	FindVariable(const char* name, IIwGameXomlResource* container);
	static CIwGameXomlVariable*	FindVariable(unsigned int name_hash, IIwGameXomlResource* container);
};

// 
// 
//
//
// CIwGameXomlBinding - Represents a bindable property and variable
//
//
//
//
struct CIwGameXomlBinding
{
	unsigned int			Property;			// Target property that is updatd with the variables value
#if defined(_DEBUG)
	CIwGameString			_Property;
#endif	// _DEBUG
	CIwGameXomlVariable*	Variable;			// Variable that cnotains the value that is bound to the target property
	int						Index;				// Index into array of binding variables

	CIwGameXomlBinding() : Index(-1) {}
};

// 
// 
//
//
// CIwGameXomlBindings - Manages a collection of XOML bindings. If an object contains a bindings manager then it should traverse the bindings list 
// and update the specified properties within the object with the vaukes located in the paired variables
//
//
//
//
class CIwGameXomlBindings : public IIwGameXomlResource
{
public:
	// Public access to iteration
	typedef CIwList<CIwGameXomlBinding*>::iterator _Iterator;
	_Iterator				begin()		{ return Bindings.begin(); }
	_Iterator				end()		{ return Bindings.end(); }

protected:
	// Properties
	CIwList<CIwGameXomlBinding*>	Bindings;			// A collection of bindings
public:
	void					addBinding(CIwGameXomlBinding* binding);
	void					addBinding(const CIwGameString& propetty_name, CIwGameXomlVariable* variable);
	void					clearBindings();
	// Properties end

public:
	CIwGameXomlBindings() : IIwGameXomlResource()	{ setClassType("bindings"); }
	~CIwGameXomlBindings()							{ clearBindings(); }

	// Implementation of IIwGameXomlClass interface
	bool					LoadFromXoml(IIwGameXomlResource* parebt, bool load_children, CIwGameXmlNode* node);
};

//
//  CIwGameXomlBindingsCreator - Creates an instance of a bingings manager object
//
class CIwGameXomlBindingsCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameXomlBindingsCreator()
	{
		setClassName("bindings");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameXomlBindings(); }
};


// 
// 
//
//
// IIwGameXomlAction - Base class that is used by classes that create a actions that can be executed in XOML
//
//
//
//
class IIwGameXomlAction
{
	// Properties
protected:
#if defined(_DEBUG)
	CIwGameString			ActionName;				// Name of action
#endif
	unsigned int			ActionNameHash;			// Name of action (hashed)
public:
	void					setActionName(const char* name)
	{
#if defined(_DEBUG)
		ActionName = name;
#endif
		ActionNameHash = CIwGameString::CalculateHash(name);
	}
	unsigned int			getActionNameHash() const		{ return ActionNameHash; }
#if defined(_DEBUG)
	CIwGameString&			getActionName()					{ return ActionName; }
#endif
	// Properties end
public:
	virtual void Execute(IIwGameXomlResource* source, CIwGameAction* action) = 0;
};

// 
// 
//
//
// CIwGameXomlResourceManager - CIwGameResourceManager manages a collection of resource groups
// 
// 
// 
//
class CIwGameXomlResourceManager
{
public:
	// Public access to class creator iteration
	typedef CIwList<IIwGameXomlResource*>::iterator _Iterator;
	_Iterator				begin()		{ return Resources.begin(); }
	_Iterator				end()		{ return Resources.end(); }

protected:
	// Properties
	CIwGameScene*					Parent;				// Parent container scene
	CIwList<IIwGameXomlResource*>	Resources;			// A collection of resources resources
public:
	void							setParent(CIwGameScene* scene)		{ Parent = scene; }
	CIwGameScene*					getParent()							{ return Parent; }
	// Properties end

public:
	CIwGameXomlResourceManager() {}
	~CIwGameXomlResourceManager() { clearResources(); }

	void					addResource(IIwGameXomlResource* resource);
	void					removeResource(IIwGameXomlResource* resource);
	void					removeResource(unsigned int name_hash, unsigned int type_hash);
	void					removeTaggedResources(const char* tag);
	void					removeTaggedResources(unsigned int tag_hash);
#if defined (_DEBUG)
	IIwGameXomlResource*	findResourceDebug(unsigned int name_hash, unsigned int type_hash, bool global_search = true);
#endif
	IIwGameXomlResource*	findResource(unsigned int name_hash, unsigned int type_hash, bool global_search = true);
	IIwGameXomlResource*	findResource(const char* name, unsigned int type_hash, bool global_search = true);
	IIwGameXomlResource*	findResource(const char* name, const char* type, bool global_search = true);
	CIwGameFont*			findFontWithFilename(const char* filename, bool global_search = true);
	void					clearResources();
};



// 
// 
//
//
// CIwGameXoml - CIwGameXoml is the main controller responsible for instantiating objects from XOML based files
// 
// 
// 
//
#define IW_GAME_XOML	CIwGameXoml::getInstance()
class CIwGameXoml
{
public:
	CDEFINE_SINGLETON(CIwGameXoml)

	// Public access to class creator iteration
	typedef CIwList<IIwGameXomlClassCreator*>::iterator _Iterator;
	_Iterator				begin() { return ClassCreators.begin(); }
	_Iterator				end() { return ClassCreators.end(); }
	typedef CIwList<IIwGameXomlAction*>::iterator _ActionsIterator;
	_ActionsIterator		actions_begin() { return Actions.begin(); }
	_ActionsIterator		actions_end() { return Actions.end(); }

protected:
	// Properties
	bool			ExitOnError;
public:
	void			addClass(IIwGameXomlClassCreator* creator)
	{
#if defined(_DEBUG)
		if (findClass(creator->getClassNameHash()))
		{
			CIwGameError::LogError("Warning: XOML - Class creator already exists - ", creator->getClassName().c_str());
			delete creator;
			return;
		}
#endif	//(_DEBUG)
		ClassCreators.push_back(creator);
	}
	void			addAction(IIwGameXomlAction* action)
	{
#if defined(_DEBUG)
		if (findAction(action->getActionNameHash()))
		{
			CIwGameError::LogError("Warning: XOML - Actions already exist - ", action->getActionName().c_str());
			delete action;
			return;
		}
#endif	//(_DEBUG)
		Actions.push_back(action);
	}
	IIwGameXomlClassCreator*	findClass(unsigned int name_hash);
	IIwGameXomlClassCreator*	findClass(const char* name);
	IIwGameXomlAction*			findAction(unsigned int name_hash);
	IIwGameXomlAction*			findAction(const char* name);
	void						setExitOnError(bool enable)				{ ExitOnError = enable; }
	// Properties end

protected:
	CIwList<IIwGameXomlClassCreator*> ClassCreators;
	CIwList<IIwGameXomlAction*>		Actions;

public:
	void			Init();
	void			InitExtensions();	// Adds extension actors and other extensions to the XOML namespace
	void			Release();

	bool			Process(IIwGameXomlResource* parent, const char* filename, bool reset_xml_pools = true);	// Pass false if you have other XML files in memory that are in use or they may be written over
	bool			Process(IIwGameXomlResource* parent, CIwGameXmlNode* node, CIwGameString* prefix = NULL);

	// Utility
	static void		ApplyStyle(CIwGameStyle* style, CIwGameXmlNode* node);
};

//
//
//
//
// CIwGameXomlAddVariable utility class to allow XOML files to add variables
// 
//
// 
//
class CIwGameXomlAddVariable : public IIwGameXomlResource
{
public:
protected:
	//// Properties
public:
	//// Properties end
public:
	CIwGameXomlAddVariable() : IIwGameXomlResource() { setClassType("variable"); }
	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parebt, bool load_children, CIwGameXmlNode* node);
};

class CIwGameXomlAddVariableCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameXomlAddVariableCreator()
	{
		setClassName("variable");
	}
	IIwGameXomlResource* CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameXomlAddVariable(); }
};

//
//
//
//
// CIwGameXomlLoad utility class to allow loading of XOMl files from XOML
// 
//
// 
//
class CIwGameXomlLoad : public IIwGameXomlResource
{
public:
protected:
	//// Properties
public:
	//// Properties end
public:
	CIwGameXomlLoad() : IIwGameXomlResource() { setClassType("loadxoml"); }
	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parebt, bool load_children, CIwGameXmlNode* node);
};

class CIwGameXomlLoadCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameXomlLoadCreator()
	{
		setClassName("loadxoml");
	}
	IIwGameXomlResource* CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameXomlLoad(); }
};


#endif	// _IW_GAME_XOML_H_
