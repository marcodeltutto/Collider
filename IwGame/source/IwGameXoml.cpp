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

#include "IwGameXoml.h"
#include "IwGameUtil.h"
#include "IwGameString.h"
#include "IwGame.h"
#include "IwGameScene.h"
#include "IwGameActor.h"
#include "IwGameActorExt.h"
#include "IwGameActorParticles.h"
#include "IwGameAnim.h"
#include "IwGameShapes.h"
#include "IwGameBox2d.h"
#include "IwGameCamera.h"
#include "IwGameStyles.h"
#include "IwGameTemplates.h"
#include "IwGameActions.h"
#include "IwGameScript.h"
#include "IwGameFont.h"
#include "IwGameAudio.h"
#include "IwGameVideo.h"
#include "IwGameModifier.h"
#include "IwGameModifierExt.h"
#include "IwGameProgram.h"
#include "IwGameUIGrid.h"
#include "IwGameUIPanels.h"
#include "IwGameUIListBox.h"
#include "IwGameUISlider.h"
#include "IwGameUIImageView.h"
#include "IwGameUIWebView.h"
#include "IwGameUITextView.h"
#include "IwGameUITabBar.h"
#include "IwGameRemoteReq.h"
#include "IwGameCam.h"

unsigned int CIwGameXomlNames::Game_Hash					= CIwGameString::CalculateHash("game");
unsigned int CIwGameXomlNames::Scene_Hash					= CIwGameString::CalculateHash("scene");
unsigned int CIwGameXomlNames::Actor_Hash					= CIwGameString::CalculateHash("actor");
unsigned int CIwGameXomlNames::ActorImage_Hash				= CIwGameString::CalculateHash("actorimage");
unsigned int CIwGameXomlNames::ActorText_Hash				= CIwGameString::CalculateHash("actortext");
unsigned int CIwGameXomlNames::ResourceGroup_Hash			= CIwGameString::CalculateHash("resourcegroup");
unsigned int CIwGameXomlNames::Animation_Hash				= CIwGameString::CalculateHash("animation");
unsigned int CIwGameXomlNames::Box2dMaterial_Hash			= CIwGameString::CalculateHash("box2dmaterial");
unsigned int CIwGameXomlNames::Joints_Hash					= CIwGameString::CalculateHash("joints");
unsigned int CIwGameXomlNames::Joint_Hash					= CIwGameString::CalculateHash("joint");
unsigned int CIwGameXomlNames::Shape_Hash					= CIwGameString::CalculateHash("shape");
unsigned int CIwGameXomlNames::Actions_Hash					= CIwGameString::CalculateHash("actions");
unsigned int CIwGameXomlNames::Camera_Hash					= CIwGameString::CalculateHash("camera");
unsigned int CIwGameXomlNames::Image_Hash					= CIwGameString::CalculateHash("image");
unsigned int CIwGameXomlNames::Sound_Hash					= CIwGameString::CalculateHash("sound");
unsigned int CIwGameXomlNames::Brush_Hash					= CIwGameString::CalculateHash("brush");
unsigned int CIwGameXomlNames::Font_Hash					= CIwGameString::CalculateHash("font");
unsigned int CIwGameXomlNames::Video_Hash					= CIwGameString::CalculateHash("video");
unsigned int CIwGameXomlNames::Timeline_Hash				= CIwGameString::CalculateHash("timeline");
unsigned int CIwGameXomlNames::Style_Hash					= CIwGameString::CalculateHash("style");
unsigned int CIwGameXomlNames::Variable_Hash				= CIwGameString::CalculateHash("variable");
unsigned int CIwGameXomlNames::Script_Hash					= CIwGameString::CalculateHash("script");
unsigned int CIwGameXomlNames::Bindings_Hash				= CIwGameString::CalculateHash("bindings");
unsigned int CIwGameXomlNames::Binding_Hash					= CIwGameString::CalculateHash("binding");
unsigned int CIwGameXomlNames::Modifier_Hash				= CIwGameString::CalculateHash("modifier");
unsigned int CIwGameXomlNames::Tabs_Hash					= CIwGameString::CalculateHash("tabs");
unsigned int CIwGameXomlNames::RemoteReq_Hash				= CIwGameString::CalculateHash("remotereq");
unsigned int CIwGameXomlNames::VideoCam_Hash				= CIwGameString::CalculateHash("videocam");

unsigned int CIwGameXomlNames::Count_Hash					= CIwGameString::CalculateHash("count");
unsigned int CIwGameXomlNames::Action_Hash					= CIwGameString::CalculateHash("action");
unsigned int CIwGameXomlNames::Name_Hash					= CIwGameString::CalculateHash("name");
unsigned int CIwGameXomlNames::ResourceName_Hash			= CIwGameString::CalculateHash("resourcename");
unsigned int CIwGameXomlNames::File_Hash					= CIwGameString::CalculateHash("file");
unsigned int CIwGameXomlNames::Current_Hash					= CIwGameString::CalculateHash("current");
unsigned int CIwGameXomlNames::CanvasSize_Hash				= CIwGameString::CalculateHash("canvassize");
unsigned int CIwGameXomlNames::CanvasFit_Hash				= CIwGameString::CalculateHash("canvasfit");
unsigned int CIwGameXomlNames::CanvasOrigin_Hash			= CIwGameString::CalculateHash("canvasorigin");
unsigned int CIwGameXomlNames::FixAspect_Hash				= CIwGameString::CalculateHash("fixaspect");
unsigned int CIwGameXomlNames::LockWidth_Hash				= CIwGameString::CalculateHash("lockwidth");
unsigned int CIwGameXomlNames::Extents_Hash					= CIwGameString::CalculateHash("extents");
unsigned int CIwGameXomlNames::AllowSuspend_Hash			= CIwGameString::CalculateHash("allowsuspend");
unsigned int CIwGameXomlNames::Clipping_Hash				= CIwGameString::CalculateHash("clipping");
unsigned int CIwGameXomlNames::ClipStatic_Hash				= CIwGameString::CalculateHash("clipstatic");
unsigned int CIwGameXomlNames::Active_Hash					= CIwGameString::CalculateHash("active");
unsigned int CIwGameXomlNames::Visible_Hash					= CIwGameString::CalculateHash("visible");
unsigned int CIwGameXomlNames::IgnoreCamera_Hash			= CIwGameString::CalculateHash("ignorecamera");
unsigned int CIwGameXomlNames::Margin_Hash					= CIwGameString::CalculateHash("margin");
unsigned int CIwGameXomlNames::TextMargin_Hash				= CIwGameString::CalculateHash("textmargin");
unsigned int CIwGameXomlNames::ClipMargin_Hash				= CIwGameString::CalculateHash("clipmargin");
unsigned int CIwGameXomlNames::Layers_Hash					= CIwGameString::CalculateHash("layers");
unsigned int CIwGameXomlNames::Colliders_Hash				= CIwGameString::CalculateHash("colliders");
unsigned int CIwGameXomlNames::Type_Hash					= CIwGameString::CalculateHash("type");
unsigned int CIwGameXomlNames::UserData_Hash				= CIwGameString::CalculateHash("userdata");
unsigned int CIwGameXomlNames::PercPos_Hash					= CIwGameString::CalculateHash("percpos");
unsigned int CIwGameXomlNames::Position_Hash				= CIwGameString::CalculateHash("position");
unsigned int CIwGameXomlNames::PositionX_Hash				= CIwGameString::CalculateHash("positionx");
unsigned int CIwGameXomlNames::PositionY_Hash				= CIwGameString::CalculateHash("positiony");
unsigned int CIwGameXomlNames::Docking_Hash					= CIwGameString::CalculateHash("docking");
unsigned int CIwGameXomlNames::Pitch_Hash					= CIwGameString::CalculateHash("pitch");
unsigned int CIwGameXomlNames::Pan_Hash						= CIwGameString::CalculateHash("pan");
unsigned int CIwGameXomlNames::Origin_Hash					= CIwGameString::CalculateHash("origin");
unsigned int CIwGameXomlNames::Skew_Hash					= CIwGameString::CalculateHash("skew");
unsigned int CIwGameXomlNames::ShowTimeline_Hash			= CIwGameString::CalculateHash("showtimeline");
unsigned int CIwGameXomlNames::HideTimeline_Hash			= CIwGameString::CalculateHash("hidetimeline");
unsigned int CIwGameXomlNames::TextSkew_Hash				= CIwGameString::CalculateHash("textskew");
unsigned int CIwGameXomlNames::BeforeChildren_Hash			= CIwGameString::CalculateHash("beforechildren");
unsigned int CIwGameXomlNames::GridPos_Hash					= CIwGameString::CalculateHash("gridpos");
unsigned int CIwGameXomlNames::Velocity_Hash				= CIwGameString::CalculateHash("velocity");
unsigned int CIwGameXomlNames::VelocityDamping_Hash			= CIwGameString::CalculateHash("velocitydamping");
unsigned int CIwGameXomlNames::Angle_Hash					= CIwGameString::CalculateHash("angle");
unsigned int CIwGameXomlNames::AngularVelocity_Hash			= CIwGameString::CalculateHash("angularvelocity");
unsigned int CIwGameXomlNames::AngularVelocityDamping_Hash	= CIwGameString::CalculateHash("angularvelocitydamping");
unsigned int CIwGameXomlNames::Depth_Hash					= CIwGameString::CalculateHash("depth");
unsigned int CIwGameXomlNames::DepthVelocity_Hash			= CIwGameString::CalculateHash("depthvelocity");
unsigned int CIwGameXomlNames::DepthVelocityDamping_Hash	= CIwGameString::CalculateHash("depthvelocitydamping");
unsigned int CIwGameXomlNames::Scale_Hash					= CIwGameString::CalculateHash("scale");
unsigned int CIwGameXomlNames::ScaleX_Hash					= CIwGameString::CalculateHash("scalex");
unsigned int CIwGameXomlNames::ScaleY_Hash					= CIwGameString::CalculateHash("scaley");
unsigned int CIwGameXomlNames::ScaleVelocity_Hash			= CIwGameString::CalculateHash("scalevelocity");
unsigned int CIwGameXomlNames::ScaleVelocityDamping_Hash	= CIwGameString::CalculateHash("scalevelocitydamping");
unsigned int CIwGameXomlNames::Colour_Hash					= CIwGameString::CalculateHash("colour");
unsigned int CIwGameXomlNames::BackgroundColour_Hash		= CIwGameString::CalculateHash("backgroundcolour");
unsigned int CIwGameXomlNames::SelectedColour_Hash			= CIwGameString::CalculateHash("selectedcolour");
unsigned int CIwGameXomlNames::DisabledColour_Hash			= CIwGameString::CalculateHash("disabledcolour");
unsigned int CIwGameXomlNames::TextColour_Hash				= CIwGameString::CalculateHash("textcolour");
unsigned int CIwGameXomlNames::SelectedTextColour_Hash		= CIwGameString::CalculateHash("selectedtextcolour");
unsigned int CIwGameXomlNames::DisabledTextColour_Hash		= CIwGameString::CalculateHash("disabledtextcolour");
unsigned int CIwGameXomlNames::ColourVelocity_Hash			= CIwGameString::CalculateHash("colourvelocity");
unsigned int CIwGameXomlNames::ColourVelocityDamping_Hash	= CIwGameString::CalculateHash("colourvelocitydamping");
unsigned int CIwGameXomlNames::Color_Hash					= CIwGameString::CalculateHash("color");
unsigned int CIwGameXomlNames::Layer_Hash					= CIwGameString::CalculateHash("layer");
unsigned int CIwGameXomlNames::Orphan_Hash					= CIwGameString::CalculateHash("orphan");
unsigned int CIwGameXomlNames::Draggable_Hash				= CIwGameString::CalculateHash("draggable");
unsigned int CIwGameXomlNames::Collidable_Hash				= CIwGameString::CalculateHash("collidable");
unsigned int CIwGameXomlNames::CollisionSize_Hash			= CIwGameString::CalculateHash("collisionsize");
unsigned int CIwGameXomlNames::CollisionRect_Hash			= CIwGameString::CalculateHash("collisionrect");
unsigned int CIwGameXomlNames::WrapPosition_Hash			= CIwGameString::CalculateHash("wrapposition");
unsigned int CIwGameXomlNames::Size_Hash					= CIwGameString::CalculateHash("size");
unsigned int CIwGameXomlNames::AspectLock_Hash				= CIwGameString::CalculateHash("aspectlock");
unsigned int CIwGameXomlNames::Orientation_Hash				= CIwGameString::CalculateHash("orientation");
unsigned int CIwGameXomlNames::Text_Hash					= CIwGameString::CalculateHash("text");
unsigned int CIwGameXomlNames::SelectedText_Hash			= CIwGameString::CalculateHash("selectedtext");
unsigned int CIwGameXomlNames::DisabledText_Hash			= CIwGameString::CalculateHash("disabledtext");
unsigned int CIwGameXomlNames::GroupFile_Hash				= CIwGameString::CalculateHash("groupfile");
unsigned int CIwGameXomlNames::Frame_Hash					= CIwGameString::CalculateHash("frame");
unsigned int CIwGameXomlNames::Atlas_Hash					= CIwGameString::CalculateHash("atlas");
unsigned int CIwGameXomlNames::Value_Hash					= CIwGameString::CalculateHash("value");
unsigned int CIwGameXomlNames::ValueRange_Hash				= CIwGameString::CalculateHash("valuerange");
unsigned int CIwGameXomlNames::Time_Hash					= CIwGameString::CalculateHash("time");
unsigned int CIwGameXomlNames::Repeat_Hash					= CIwGameString::CalculateHash("repeat");
unsigned int CIwGameXomlNames::Duration_Hash				= CIwGameString::CalculateHash("duration");
unsigned int CIwGameXomlNames::SpawnDelay_Hash				= CIwGameString::CalculateHash("spawndelay");
unsigned int CIwGameXomlNames::SrcRect_Hash					= CIwGameString::CalculateHash("srcrect");
unsigned int CIwGameXomlNames::UV_Hash						= CIwGameString::CalculateHash("uv");
unsigned int CIwGameXomlNames::Tiled_Hash					= CIwGameString::CalculateHash("tiled");
unsigned int CIwGameXomlNames::Location_Hash				= CIwGameString::CalculateHash("location");
unsigned int CIwGameXomlNames::Resource_Hash				= CIwGameString::CalculateHash("resource");
unsigned int CIwGameXomlNames::Tag_Hash						= CIwGameString::CalculateHash("tag");
unsigned int CIwGameXomlNames::Preload_Hash					= CIwGameString::CalculateHash("preload");
unsigned int CIwGameXomlNames::Blocking_Hash				= CIwGameString::CalculateHash("blocking");
unsigned int CIwGameXomlNames::FileType_Hash				= CIwGameString::CalculateHash("filetype");
unsigned int CIwGameXomlNames::Converter_Hash				= CIwGameString::CalculateHash("converter");
unsigned int CIwGameXomlNames::AutoPlay_Hash				= CIwGameString::CalculateHash("autoplay");
unsigned int CIwGameXomlNames::Volume_Hash					= CIwGameString::CalculateHash("volume");
unsigned int CIwGameXomlNames::PointSize_Hash				= CIwGameString::CalculateHash("pointsize");
unsigned int CIwGameXomlNames::AutoPointSize_Hash			= CIwGameString::CalculateHash("autopointsize");
unsigned int CIwGameXomlNames::Anim_Hash					= CIwGameString::CalculateHash("anim");
unsigned int CIwGameXomlNames::StartAtTime_Hash				= CIwGameString::CalculateHash("startattime");
unsigned int CIwGameXomlNames::TimeScale_Hash				= CIwGameString::CalculateHash("timescale");
unsigned int CIwGameXomlNames::Delta_Hash					= CIwGameString::CalculateHash("delta");
unsigned int CIwGameXomlNames::Interpolate_Hash				= CIwGameString::CalculateHash("interpolate");
unsigned int CIwGameXomlNames::Easing_Hash					= CIwGameString::CalculateHash("easing");
unsigned int CIwGameXomlNames::Target_Hash					= CIwGameString::CalculateHash("target");
unsigned int CIwGameXomlNames::TargetA_Hash					= CIwGameString::CalculateHash("targeta");
unsigned int CIwGameXomlNames::TargetB_Hash					= CIwGameString::CalculateHash("targetb");
unsigned int CIwGameXomlNames::UseParentOpacity_Hash		= CIwGameString::CalculateHash("useparentopacity");
unsigned int CIwGameXomlNames::TextUseParentOpacity_Hash	= CIwGameString::CalculateHash("textuseparentopacity");
unsigned int CIwGameXomlNames::Width_Hash					= CIwGameString::CalculateHash("width");
unsigned int CIwGameXomlNames::Height_Hash					= CIwGameString::CalculateHash("height");
unsigned int CIwGameXomlNames::Radius_Hash					= CIwGameString::CalculateHash("radius");
unsigned int CIwGameXomlNames::Points_Hash					= CIwGameString::CalculateHash("points");
unsigned int CIwGameXomlNames::Point_Hash					= CIwGameString::CalculateHash("point");
unsigned int CIwGameXomlNames::RowDefinition_Hash			= CIwGameString::CalculateHash("rowdefinition");
unsigned int CIwGameXomlNames::ColumnDefinition_Hash		= CIwGameString::CalculateHash("columndefinition");
unsigned int CIwGameXomlNames::Particle_Hash				= CIwGameString::CalculateHash("particle");
unsigned int CIwGameXomlNames::Data_Hash					= CIwGameString::CalculateHash("data");
unsigned int CIwGameXomlNames::Density_Hash					= CIwGameString::CalculateHash("density");
unsigned int CIwGameXomlNames::Friction_Hash				= CIwGameString::CalculateHash("friction");
unsigned int CIwGameXomlNames::Restitution_Hash				= CIwGameString::CalculateHash("restitution");
unsigned int CIwGameXomlNames::IsBullet_Hash				= CIwGameString::CalculateHash("isbullet");
unsigned int CIwGameXomlNames::FixedRotation_Hash			= CIwGameString::CalculateHash("fixedrotation");
unsigned int CIwGameXomlNames::GravityScale_Hash			= CIwGameString::CalculateHash("gravityscale");
unsigned int CIwGameXomlNames::COM_Hash						= CIwGameString::CalculateHash("com");
unsigned int CIwGameXomlNames::Gravity_Hash					= CIwGameString::CalculateHash("gravity");
unsigned int CIwGameXomlNames::DoSleep_Hash					= CIwGameString::CalculateHash("dosleep");
unsigned int CIwGameXomlNames::Mode_Hash					= CIwGameString::CalculateHash("mode");
unsigned int CIwGameXomlNames::WorldScale_Hash				= CIwGameString::CalculateHash("worldscale");
unsigned int CIwGameXomlNames::Physics_Hash					= CIwGameString::CalculateHash("physics");
unsigned int CIwGameXomlNames::PhysicsTimestep_Hash			= CIwGameString::CalculateHash("physicstimestep");
unsigned int CIwGameXomlNames::Sensor_Hash					= CIwGameString::CalculateHash("sensor");
unsigned int CIwGameXomlNames::CollisionFlags_Hash			= CIwGameString::CalculateHash("collisionflags");
unsigned int CIwGameXomlNames::Attached_Hash				= CIwGameString::CalculateHash("attached");
unsigned int CIwGameXomlNames::Set_Hash						= CIwGameString::CalculateHash("set");
unsigned int CIwGameXomlNames::Property_Hash				= CIwGameString::CalculateHash("property");
unsigned int CIwGameXomlNames::HitTest_Hash					= CIwGameString::CalculateHash("hittest");
unsigned int CIwGameXomlNames::FlipX_Hash					= CIwGameString::CalculateHash("flipx");
unsigned int CIwGameXomlNames::FlipY_Hash					= CIwGameString::CalculateHash("flipy");
unsigned int CIwGameXomlNames::LinkedTo_Hash				= CIwGameString::CalculateHash("linkedto");
unsigned int CIwGameXomlNames::Template_Hash				= CIwGameString::CalculateHash("template");
unsigned int CIwGameXomlNames::Prefix_Hash					= CIwGameString::CalculateHash("prefix");
unsigned int CIwGameXomlNames::AlignH_Hash					= CIwGameString::CalculateHash("alignh");
unsigned int CIwGameXomlNames::AlignV_Hash					= CIwGameString::CalculateHash("alignv");
unsigned int CIwGameXomlNames::SizeToContent_Hash			= CIwGameString::CalculateHash("sizetocontent");
unsigned int CIwGameXomlNames::Wrap_Hash					= CIwGameString::CalculateHash("wrap");
unsigned int CIwGameXomlNames::Italic_Hash					= CIwGameString::CalculateHash("italic");
unsigned int CIwGameXomlNames::Underline_Hash				= CIwGameString::CalculateHash("underline");
unsigned int CIwGameXomlNames::IsPlaying_Hash				= CIwGameString::CalculateHash("isplaying");
unsigned int CIwGameXomlNames::Started_Hash					= CIwGameString::CalculateHash("started");
unsigned int CIwGameXomlNames::Command_Hash					= CIwGameString::CalculateHash("command");
unsigned int CIwGameXomlNames::Method_Hash					= CIwGameString::CalculateHash("method");
unsigned int CIwGameXomlNames::Param1_Hash					= CIwGameString::CalculateHash("param1");
unsigned int CIwGameXomlNames::Param2_Hash					= CIwGameString::CalculateHash("param2");
unsigned int CIwGameXomlNames::Param3_Hash					= CIwGameString::CalculateHash("param3");
unsigned int CIwGameXomlNames::Param4_Hash					= CIwGameString::CalculateHash("param4");
unsigned int CIwGameXomlNames::Param5_Hash					= CIwGameString::CalculateHash("param5");
unsigned int CIwGameXomlNames::P1_Hash						= CIwGameString::CalculateHash("p1");
unsigned int CIwGameXomlNames::P2_Hash						= CIwGameString::CalculateHash("p2");
unsigned int CIwGameXomlNames::P3_Hash						= CIwGameString::CalculateHash("p3");
unsigned int CIwGameXomlNames::P4_Hash						= CIwGameString::CalculateHash("p4");
unsigned int CIwGameXomlNames::P5_Hash						= CIwGameString::CalculateHash("p5");
unsigned int CIwGameXomlNames::Condition_Hash				= CIwGameString::CalculateHash("condition");
unsigned int CIwGameXomlNames::BindXML_Hash					= CIwGameString::CalculateHash("bindxml");
unsigned int CIwGameXomlNames::Persist_Hash					= CIwGameString::CalculateHash("persist");
unsigned int CIwGameXomlNames::Local_Hash					= CIwGameString::CalculateHash("local");
unsigned int CIwGameXomlNames::Format_Hash					= CIwGameString::CalculateHash("format");
unsigned int CIwGameXomlNames::Codec_Hash					= CIwGameString::CalculateHash("codec");
unsigned int CIwGameXomlNames::Filter_Hash					= CIwGameString::CalculateHash("filter");
unsigned int CIwGameXomlNames::AlphaMode_Hash				= CIwGameString::CalculateHash("alphamode");
unsigned int CIwGameXomlNames::AutoWidth_Hash				= CIwGameString::CalculateHash("autowidth");
unsigned int CIwGameXomlNames::AutoHeight_Hash				= CIwGameString::CalculateHash("autoheight");
unsigned int CIwGameXomlNames::Batch_Hash					= CIwGameString::CalculateHash("batch");
unsigned int CIwGameXomlNames::AllowFocus_Hash				= CIwGameString::CalculateHash("allowfocus");
unsigned int CIwGameXomlNames::TouchPanX_Hash				= CIwGameString::CalculateHash("touchpanx");
unsigned int CIwGameXomlNames::TouchPanY_Hash				= CIwGameString::CalculateHash("touchpany");
unsigned int CIwGameXomlNames::IgnoreActors_Hash			= CIwGameString::CalculateHash("ignoreactors");
unsigned int CIwGameXomlNames::ActorB_Hash					= CIwGameString::CalculateHash("actorb");
unsigned int CIwGameXomlNames::OffsetA_Hash					= CIwGameString::CalculateHash("offseta");
unsigned int CIwGameXomlNames::OffsetB_Hash					= CIwGameString::CalculateHash("offsetb");
unsigned int CIwGameXomlNames::GroundA_Hash					= CIwGameString::CalculateHash("grounda");
unsigned int CIwGameXomlNames::GroundB_Hash					= CIwGameString::CalculateHash("groundb");
unsigned int CIwGameXomlNames::SelfCollide_Hash				= CIwGameString::CalculateHash("selfcollide");
unsigned int CIwGameXomlNames::Distance_Hash				= CIwGameString::CalculateHash("distance");
unsigned int CIwGameXomlNames::Revolute_Hash				= CIwGameString::CalculateHash("revolute");
unsigned int CIwGameXomlNames::Prismatic_Hash				= CIwGameString::CalculateHash("prismatic");
unsigned int CIwGameXomlNames::Pulley_Hash					= CIwGameString::CalculateHash("pulley");
unsigned int CIwGameXomlNames::Gear_Hash					= CIwGameString::CalculateHash("gear");
unsigned int CIwGameXomlNames::Wheel_Hash					= CIwGameString::CalculateHash("wheel");
unsigned int CIwGameXomlNames::Rope_Hash					= CIwGameString::CalculateHash("rope");
unsigned int CIwGameXomlNames::Length_Hash					= CIwGameString::CalculateHash("length");
unsigned int CIwGameXomlNames::LengthA_Hash					= CIwGameString::CalculateHash("lengtha");
unsigned int CIwGameXomlNames::LengthB_Hash					= CIwGameString::CalculateHash("lengthb");
unsigned int CIwGameXomlNames::Ratio_Hash					= CIwGameString::CalculateHash("ratio");
unsigned int CIwGameXomlNames::Frequency_Hash				= CIwGameString::CalculateHash("frequency");
unsigned int CIwGameXomlNames::Damping_Hash					= CIwGameString::CalculateHash("damping");
unsigned int CIwGameXomlNames::MotorEnabled_Hash			= CIwGameString::CalculateHash("motorenabled");
unsigned int CIwGameXomlNames::LimitJoint_Hash				= CIwGameString::CalculateHash("limitjoint");
unsigned int CIwGameXomlNames::LowerLimit_Hash				= CIwGameString::CalculateHash("lowerlimit");
unsigned int CIwGameXomlNames::UpperLimit_Hash				= CIwGameString::CalculateHash("upperlimit");
unsigned int CIwGameXomlNames::MotorSpeed_Hash				= CIwGameString::CalculateHash("motorspeed");
unsigned int CIwGameXomlNames::MaxMotorTorque_Hash			= CIwGameString::CalculateHash("maxmotortorque");
unsigned int CIwGameXomlNames::MaxMotorForce_Hash			= CIwGameString::CalculateHash("maxmotorforce");
unsigned int CIwGameXomlNames::ReferenceAngle_Hash			= CIwGameString::CalculateHash("refangle");
unsigned int CIwGameXomlNames::Axis_Hash					= CIwGameString::CalculateHash("axis");
unsigned int CIwGameXomlNames::Force_Hash					= CIwGameString::CalculateHash("force");
unsigned int CIwGameXomlNames::LinearImpulse_Hash			= CIwGameString::CalculateHash("linearimpulse");
unsigned int CIwGameXomlNames::Torque_Hash					= CIwGameString::CalculateHash("torque");
unsigned int CIwGameXomlNames::AngularImpulse_Hash			= CIwGameString::CalculateHash("angularimpulse");
unsigned int CIwGameXomlNames::Solid_Hash					= CIwGameString::CalculateHash("solid");
unsigned int CIwGameXomlNames::Gradient_Hash				= CIwGameString::CalculateHash("gradient");
unsigned int CIwGameXomlNames::Patch9_Hash					= CIwGameString::CalculateHash("9patch");
unsigned int CIwGameXomlNames::ScaleArea_Hash				= CIwGameString::CalculateHash("scalearea");
unsigned int CIwGameXomlNames::Activate_Hash				= CIwGameString::CalculateHash("activate");
unsigned int CIwGameXomlNames::Deactivate_Hash				= CIwGameString::CalculateHash("deactivate");
unsigned int CIwGameXomlNames::Toggle_Hash					= CIwGameString::CalculateHash("toggle");
unsigned int CIwGameXomlNames::Sticky_Hash					= CIwGameString::CalculateHash("sticky");
unsigned int CIwGameXomlNames::Enabled_Hash					= CIwGameString::CalculateHash("enabled");
unsigned int CIwGameXomlNames::Remove_Hash					= CIwGameString::CalculateHash("remove");
unsigned int CIwGameXomlNames::ItemsData_Hash				= CIwGameString::CalculateHash("itemsdata");
unsigned int CIwGameXomlNames::ItemsTargetType_Hash			= CIwGameString::CalculateHash("itemstargettype");
unsigned int CIwGameXomlNames::ItemsTemplate_Hash			= CIwGameString::CalculateHash("itemstemplate");
unsigned int CIwGameXomlNames::SelectType_Hash				= CIwGameString::CalculateHash("selecttype");
unsigned int CIwGameXomlNames::Selected_Hash				= CIwGameString::CalculateHash("selected");
unsigned int CIwGameXomlNames::Spring_Hash					= CIwGameString::CalculateHash("spring");
unsigned int CIwGameXomlNames::Bubbling_Hash				= CIwGameString::CalculateHash("bubbling");
unsigned int CIwGameXomlNames::KeyFocus_Hash				= CIwGameString::CalculateHash("keyfocus");
unsigned int CIwGameXomlNames::ScrollRange_Hash				= CIwGameString::CalculateHash("scrollrange");
unsigned int CIwGameXomlNames::ScrollPos_Hash				= CIwGameString::CalculateHash("scrollpos");
unsigned int CIwGameXomlNames::Normal_Hash					= CIwGameString::CalculateHash("normal");
unsigned int CIwGameXomlNames::Background_Hash				= CIwGameString::CalculateHash("background");
unsigned int CIwGameXomlNames::SelectedBackground_Hash		= CIwGameString::CalculateHash("selectedbackground");
unsigned int CIwGameXomlNames::DisabledBackground_Hash		= CIwGameString::CalculateHash("disabledbackground");
unsigned int CIwGameXomlNames::CaretColourOffset_Hash		= CIwGameString::CalculateHash("caretcolouroffset");
unsigned int CIwGameXomlNames::TypeHint_Hash				= CIwGameString::CalculateHash("typehint");
unsigned int CIwGameXomlNames::Prompt_Hash					= CIwGameString::CalculateHash("prompt");
unsigned int CIwGameXomlNames::Icon_Hash					= CIwGameString::CalculateHash("icon");
unsigned int CIwGameXomlNames::Label_Hash					= CIwGameString::CalculateHash("label");
unsigned int CIwGameXomlNames::TextBox_Hash					= CIwGameString::CalculateHash("textbox");
unsigned int CIwGameXomlNames::StackPanel_Hash				= CIwGameString::CalculateHash("stackpanel");
unsigned int CIwGameXomlNames::Grid_Hash					= CIwGameString::CalculateHash("grid");
unsigned int CIwGameXomlNames::ListBox_Hash					= CIwGameString::CalculateHash("listbox");
unsigned int CIwGameXomlNames::Canvas_Hash					= CIwGameString::CalculateHash("canvas");
unsigned int CIwGameXomlNames::MultiSelect_Hash				= CIwGameString::CalculateHash("multiselect");
unsigned int CIwGameXomlNames::AutoRun_Hash					= CIwGameString::CalculateHash("autorun");
unsigned int CIwGameXomlNames::Priority_Hash				= CIwGameString::CalculateHash("priority");
unsigned int CIwGameXomlNames::Parallel_Hash				= CIwGameString::CalculateHash("parallel");
unsigned int CIwGameXomlNames::IfReturn_Hash				= CIwGameString::CalculateHash("ifreturn");
unsigned int CIwGameXomlNames::Selection_Hash				= CIwGameString::CalculateHash("selection");
unsigned int CIwGameXomlNames::SelectedIndex_Hash			= CIwGameString::CalculateHash("selectedindex");
unsigned int CIwGameXomlNames::SliderType_Hash				= CIwGameString::CalculateHash("slidertype");
unsigned int CIwGameXomlNames::SliderSize_Hash				= CIwGameString::CalculateHash("slidersize");
unsigned int CIwGameXomlNames::ImageBrush_Hash				= CIwGameString::CalculateHash("imagebrush");
unsigned int CIwGameXomlNames::Area_Hash					= CIwGameString::CalculateHash("area");
unsigned int CIwGameXomlNames::Zoom_Hash					= CIwGameString::CalculateHash("zoom");
unsigned int CIwGameXomlNames::MinZoom_Hash					= CIwGameString::CalculateHash("minzoom");
unsigned int CIwGameXomlNames::MaxZoom_Hash					= CIwGameString::CalculateHash("maxzoom");
unsigned int CIwGameXomlNames::URI_Hash						= CIwGameString::CalculateHash("uri");
unsigned int CIwGameXomlNames::Modal_Hash					= CIwGameString::CalculateHash("modal");
unsigned int CIwGameXomlNames::JavaScript_Hash				= CIwGameString::CalculateHash("javascript");
unsigned int CIwGameXomlNames::Transparent_Hash				= CIwGameString::CalculateHash("transparent");
unsigned int CIwGameXomlNames::Html_Hash					= CIwGameString::CalculateHash("html");
unsigned int CIwGameXomlNames::Opacity_Hash					= CIwGameString::CalculateHash("opacity");
unsigned int CIwGameXomlNames::AutoHide_Hash				= CIwGameString::CalculateHash("autohide");
unsigned int CIwGameXomlNames::View_Hash					= CIwGameString::CalculateHash("view");
unsigned int CIwGameXomlNames::ScriptEngine_Hash			= CIwGameString::CalculateHash("scriptengine");
unsigned int CIwGameXomlNames::Program_Hash					= CIwGameString::CalculateHash("program");
unsigned int CIwGameXomlNames::URL_Hash						= CIwGameString::CalculateHash("url");
unsigned int CIwGameXomlNames::Post_Hash					= CIwGameString::CalculateHash("post");
unsigned int CIwGameXomlNames::Parent_Hash					= CIwGameString::CalculateHash("parent");
unsigned int CIwGameXomlNames::Quality_Hash					= CIwGameString::CalculateHash("quality");
unsigned int CIwGameXomlNames::Resolution_Hash				= CIwGameString::CalculateHash("resolution");
unsigned int CIwGameXomlNames::Direction_Hash				= CIwGameString::CalculateHash("direction");

unsigned int CIwGameXomlNames::OnTapped_Hash				= CIwGameString::CalculateHash("ontapped");
unsigned int CIwGameXomlNames::OnTapped2_Hash				= CIwGameString::CalculateHash("ontapped2");
unsigned int CIwGameXomlNames::OnTapped3_Hash				= CIwGameString::CalculateHash("ontapped3");
unsigned int CIwGameXomlNames::OnTapped4_Hash				= CIwGameString::CalculateHash("ontapped4");
unsigned int CIwGameXomlNames::OnTapped5_Hash				= CIwGameString::CalculateHash("ontapped5");
unsigned int CIwGameXomlNames::OnBeginTouch_Hash			= CIwGameString::CalculateHash("onbegintouch");
unsigned int CIwGameXomlNames::OnBeginTouch2_Hash			= CIwGameString::CalculateHash("onbegintouch2");
unsigned int CIwGameXomlNames::OnBeginTouch3_Hash			= CIwGameString::CalculateHash("onbegintouch3");
unsigned int CIwGameXomlNames::OnBeginTouch4_Hash			= CIwGameString::CalculateHash("onbegintouch4");
unsigned int CIwGameXomlNames::OnBeginTouch5_Hash			= CIwGameString::CalculateHash("onbegintouch5");
unsigned int CIwGameXomlNames::OnEndTouch_Hash				= CIwGameString::CalculateHash("onendtouch");
unsigned int CIwGameXomlNames::OnEndTouch2_Hash				= CIwGameString::CalculateHash("onendtouch2");
unsigned int CIwGameXomlNames::OnEndTouch3_Hash				= CIwGameString::CalculateHash("onendtouch3");
unsigned int CIwGameXomlNames::OnEndTouch4_Hash				= CIwGameString::CalculateHash("onendtouch4");
unsigned int CIwGameXomlNames::OnEndTouch5_Hash				= CIwGameString::CalculateHash("onendtouch5");
unsigned int CIwGameXomlNames::OnSuspend_Hash				= CIwGameString::CalculateHash("onsuspend");
unsigned int CIwGameXomlNames::OnResume_Hash				= CIwGameString::CalculateHash("onresume");
unsigned int CIwGameXomlNames::OnPause_Hash					= CIwGameString::CalculateHash("onpause");
unsigned int CIwGameXomlNames::OnLostFocus_Hash				= CIwGameString::CalculateHash("onlostfocus");
unsigned int CIwGameXomlNames::OnGainedFocus_Hash			= CIwGameString::CalculateHash("ongainedfocus");
unsigned int CIwGameXomlNames::OnStart_Hash					= CIwGameString::CalculateHash("onstart");
unsigned int CIwGameXomlNames::OnEnd_Hash					= CIwGameString::CalculateHash("onend");
unsigned int CIwGameXomlNames::OnRepeat_Hash				= CIwGameString::CalculateHash("onrepeat");
unsigned int CIwGameXomlNames::OnCreate_Hash				= CIwGameString::CalculateHash("oncreate");
unsigned int CIwGameXomlNames::OnDestroy_Hash				= CIwGameString::CalculateHash("ondestroy");
unsigned int CIwGameXomlNames::OnOrientationChange_Hash		= CIwGameString::CalculateHash("onorientationchange");
unsigned int CIwGameXomlNames::OnTick_Hash					= CIwGameString::CalculateHash("ontick");
unsigned int CIwGameXomlNames::OnKeyBack_Hash				= CIwGameString::CalculateHash("onkeyback");
unsigned int CIwGameXomlNames::OnKeyMenu_Hash				= CIwGameString::CalculateHash("onkeymenu");
unsigned int CIwGameXomlNames::OnTextChanged_Hash			= CIwGameString::CalculateHash("ontextchanged");
unsigned int CIwGameXomlNames::OnToggledOn_Hash				= CIwGameString::CalculateHash("ontoggledon");
unsigned int CIwGameXomlNames::OnToggledOff_Hash			= CIwGameString::CalculateHash("ontoggledoff");
unsigned int CIwGameXomlNames::OnBackKey_Hash				= CIwGameString::CalculateHash("onbackkey");
unsigned int CIwGameXomlNames::OnMenuKey_Hash				= CIwGameString::CalculateHash("onmenukey");
unsigned int CIwGameXomlNames::OnSelectionChanged_Hash		= CIwGameString::CalculateHash("onselectionchanged");
unsigned int CIwGameXomlNames::OnValueChanged_Hash			= CIwGameString::CalculateHash("onvaluechanged");
unsigned int CIwGameXomlNames::OnViewChanged_Hash			= CIwGameString::CalculateHash("onviewchanged");
unsigned int CIwGameXomlNames::OnCollisionStart_Hash		= CIwGameString::CalculateHash("oncollisionstart");
unsigned int CIwGameXomlNames::OnCollisionEnd_Hash			= CIwGameString::CalculateHash("oncollisionend");
unsigned int CIwGameXomlNames::OnResponse_Hash				= CIwGameString::CalculateHash("onresponse");
unsigned int CIwGameXomlNames::OnError_Hash					= CIwGameString::CalculateHash("onerror");

unsigned int CIwGameXomlNames::ChangeScene_Hash				= CIwGameString::CalculateHash("changescene");
unsigned int CIwGameXomlNames::SuspendScene_Hash			= CIwGameString::CalculateHash("suspendscene");
unsigned int CIwGameXomlNames::ResumeScene_Hash				= CIwGameString::CalculateHash("resumescene");
unsigned int CIwGameXomlNames::ShowScene_Hash				= CIwGameString::CalculateHash("showscene");
unsigned int CIwGameXomlNames::HideScene_Hash				= CIwGameString::CalculateHash("hidescene");
unsigned int CIwGameXomlNames::ActivateScene_Hash			= CIwGameString::CalculateHash("activatescene");
unsigned int CIwGameXomlNames::DeactivateScene_Hash			= CIwGameString::CalculateHash("deactivatescene");
unsigned int CIwGameXomlNames::KillScene_Hash				= CIwGameString::CalculateHash("killscene");
unsigned int CIwGameXomlNames::ShowActor_Hash				= CIwGameString::CalculateHash("showactor");
unsigned int CIwGameXomlNames::HideActor_Hash				= CIwGameString::CalculateHash("hideactor");
unsigned int CIwGameXomlNames::ActivateActor_Hash			= CIwGameString::CalculateHash("activateactor");
unsigned int CIwGameXomlNames::DeactivateActor_Hash			= CIwGameString::CalculateHash("deactivateactor");
unsigned int CIwGameXomlNames::KillActor_Hash				= CIwGameString::CalculateHash("killactor");
unsigned int CIwGameXomlNames::PlayTimeline_Hash			= CIwGameString::CalculateHash("playtimeline");
unsigned int CIwGameXomlNames::StopTimeline_Hash			= CIwGameString::CalculateHash("stoptimeline");
unsigned int CIwGameXomlNames::SetTimeline_Hash				= CIwGameString::CalculateHash("settimeline");
unsigned int CIwGameXomlNames::PlaySound_Hash				= CIwGameString::CalculateHash("playsound");
unsigned int CIwGameXomlNames::PlayMusic_Hash				= CIwGameString::CalculateHash("playmusic");
unsigned int CIwGameXomlNames::SetVar_Hash					= CIwGameString::CalculateHash("setvar");
unsigned int CIwGameXomlNames::Rand_Hash					= CIwGameString::CalculateHash("rand");
unsigned int CIwGameXomlNames::RandChar_Hash				= CIwGameString::CalculateHash("randchar");

unsigned int CIwGameXomlNames::Start_Hash					= CIwGameString::CalculateHash("start");
unsigned int CIwGameXomlNames::Restart_Hash					= CIwGameString::CalculateHash("restart");
unsigned int CIwGameXomlNames::Stop_Hash					= CIwGameString::CalculateHash("stop");
unsigned int CIwGameXomlNames::Play_Hash					= CIwGameString::CalculateHash("play");
unsigned int CIwGameXomlNames::Pause_Hash					= CIwGameString::CalculateHash("pause");
unsigned int CIwGameXomlNames::Next_Hash					= CIwGameString::CalculateHash("next");
unsigned int CIwGameXomlNames::Goto_Hash					= CIwGameString::CalculateHash("goto");

unsigned int CIwGameXomlNames::bool_Hash					= CIwGameString::CalculateHash("bool");
unsigned int CIwGameXomlNames::float_Hash					= CIwGameString::CalculateHash("float");
unsigned int CIwGameXomlNames::int_Hash						= CIwGameString::CalculateHash("int");
unsigned int CIwGameXomlNames::vec2_Hash					= CIwGameString::CalculateHash("vec2");
unsigned int CIwGameXomlNames::vec3_Hash					= CIwGameString::CalculateHash("vec3");
unsigned int CIwGameXomlNames::vec4_Hash					= CIwGameString::CalculateHash("vec4");
unsigned int CIwGameXomlNames::rect_Hash					= CIwGameString::CalculateHash("rect");
unsigned int CIwGameXomlNames::string_Hash					= CIwGameString::CalculateHash("string");
unsigned int CIwGameXomlNames::condition_Hash				= CIwGameString::CalculateHash("condition");
unsigned int CIwGameXomlNames::arraystring_Hash				= CIwGameString::CalculateHash("arraystring");
unsigned int CIwGameXomlNames::arraybool_Hash				= CIwGameString::CalculateHash("arraybool");
unsigned int CIwGameXomlNames::arrayfloat_Hash				= CIwGameString::CalculateHash("arrayfloat");
unsigned int CIwGameXomlNames::arrayint_Hash				= CIwGameString::CalculateHash("arrayint");
unsigned int CIwGameXomlNames::xml_Hash						= CIwGameString::CalculateHash("xml");
unsigned int CIwGameXomlNames::box_Hash						= CIwGameString::CalculateHash("box");
unsigned int CIwGameXomlNames::circle_Hash					= CIwGameString::CalculateHash("circle");
unsigned int CIwGameXomlNames::polygon_Hash					= CIwGameString::CalculateHash("polygon");
unsigned int CIwGameXomlNames::static_Hash					= CIwGameString::CalculateHash("static");
unsigned int CIwGameXomlNames::dynamic_Hash					= CIwGameString::CalculateHash("dynamic");
unsigned int CIwGameXomlNames::kinematic_Hash				= CIwGameString::CalculateHash("kinematic");
unsigned int CIwGameXomlNames::Linear_Hash					= CIwGameString::CalculateHash("linear");
unsigned int CIwGameXomlNames::QuadIn_Hash					= CIwGameString::CalculateHash("quadin");
unsigned int CIwGameXomlNames::QuadOut_Hash					= CIwGameString::CalculateHash("quadout");
unsigned int CIwGameXomlNames::CubicIn_Hash					= CIwGameString::CalculateHash("cubicin");
unsigned int CIwGameXomlNames::CubicOut_Hash				= CIwGameString::CalculateHash("cubicout");
unsigned int CIwGameXomlNames::QuarticIn_Hash				= CIwGameString::CalculateHash("quarticin");
unsigned int CIwGameXomlNames::QuarticOut_Hash				= CIwGameString::CalculateHash("quarticout");
unsigned int CIwGameXomlNames::left_Hash					= CIwGameString::CalculateHash("left");
unsigned int CIwGameXomlNames::right_Hash					= CIwGameString::CalculateHash("right");
unsigned int CIwGameXomlNames::centre_Hash					= CIwGameString::CalculateHash("centre");
unsigned int CIwGameXomlNames::top_Hash						= CIwGameString::CalculateHash("top");
unsigned int CIwGameXomlNames::middle_Hash					= CIwGameString::CalculateHash("middle");
unsigned int CIwGameXomlNames::bottom_Hash					= CIwGameString::CalculateHash("bottom");


//
//
//
//
// CIwGameXomlResourceManager implementation
//
//
//
//
/*void CIwGameXomlResourceManager::addResource(const char* resource_name, const char* resource_filename, bool preload)
{
	IIwGameXomlResource* resource = new CIwGameResourceResource();
	resource->setResourceName(resource_name);
	resource->setResourceFilename(resource_filename);
	Resources.push_back(resource);

	if (preload)
	{
		resource->Load();
	}
}*/

bool CIwGameXomlResourceManager::addResource(IIwGameXomlResource *resource)
{
#if defined(IW_REMOVE_DUPLICATES)
	IIwGameXomlResource* res = findResource(resource->getNameHash(), resource->getClassTypeHash(), false);
	if (res != NULL)
	{
		CIwGameError::LogError("Warning: Resource already exists, resource was replaced - ", resource->getName().c_str());
		removeResource(res);
	}
#endif
	Resources.push_back(resource);
	resource->setParent(this);
	resource->setManaged(true);

	return true;
}

void CIwGameXomlResourceManager::removeResource(IIwGameXomlResource* resource)
{
	removeResource(resource->getNameHash(), resource->getClassTypeHash());
}

void CIwGameXomlResourceManager::removeResource(unsigned int name_hash, unsigned int type_hash)
{
	// Renove named resource from the manager
	for (_Iterator it = Resources.begin(); it != Resources.end(); ++it)
	{
		if (name_hash == (*it)->getNameHash() && type_hash == (*it)->getClassTypeHash())
		{
			delete *it;
			Resources.erase(it);
			break;
		}
	}
}

void CIwGameXomlResourceManager::removeTaggedResources(unsigned int tag_hash)
{
	CIwList<IIwGameXomlResource*>		Removals;

	// Buildl ist of resources that are to be removed for tag
	for (_Iterator it = Resources.begin(); it != Resources.end(); ++it)
	{
		if (tag_hash == (*it)->getTagHash())
		{
			Removals.push_back(*it);
		}
	}

	// Remove the resources
	for (_Iterator it = Removals.begin(); it != Removals.end(); ++it)
	{
		for (_Iterator it2 = Resources.begin(); it2 != Resources.end(); ++it2)
		{
			if (*it2 == *it)
			{
				delete *it2;
				Resources.erase(it2);
				break;
			}
		}
	}
	Removals.clear();
}

void CIwGameXomlResourceManager::removeTaggedResources(const char* tag)
{
	removeTaggedResources(CIwGameString::CalculateHash(tag));
}

IIwGameXomlResource* CIwGameXomlResourceManager::findResource(unsigned int name_hash, unsigned int type_hash, bool global_search)
{
	// Find and return named image of specific type
	for (_Iterator it = Resources.begin(); it != Resources.end(); ++it)
	{
		if (name_hash == (*it)->getNameHash() && type_hash == (*it)->getClassTypeHash())
			return *it;
	}

	if (global_search)
	{
		// Not found so check global resources
		CIwGameXomlResourceManager* m = IW_GAME_GLOBAL_RESOURCES->getResourceManager();
		if (m != this)	// Prevent global resource manager from calling itself
			return m->findResource(name_hash, type_hash, false);
	}

	return NULL;
}

#if defined (_DEBUG)
IIwGameXomlResource* CIwGameXomlResourceManager::findResourceDebug(unsigned int name_hash, unsigned int type_hash, bool global_search)
{
	if (IW_GAME_GLOBAL_RESOURCES->getResourceManager() != this)
		CIwGameError::LogError("**** Searching local resources ****");
	else
		CIwGameError::LogError("**** Searching global resources ****");
	// Find and return named image of specific type
	for (_Iterator it = Resources.begin(); it != Resources.end(); ++it)
	{
		CIwGameError::LogError("- ", (*it)->getName().c_str());
		if (name_hash == (*it)->getNameHash() && type_hash == (*it)->getClassTypeHash())
			return *it;
	}

	if (global_search)
	{
		// Not found so check global resources
		CIwGameXomlResourceManager* m = IW_GAME_GLOBAL_RESOURCES->getResourceManager();
		if (m != this)	// Prevent global resource manager from calling itself
			return m->findResourceDebug(name_hash, type_hash, false);
	}

	return NULL;
}
#endif // _DEBUG

IIwGameXomlResource* CIwGameXomlResourceManager::findResource(const char* name, unsigned int type_hash, bool global_search)
{
	return findResource(CIwGameString::CalculateHash(name), type_hash, global_search);
}

IIwGameXomlResource* CIwGameXomlResourceManager::findResource(const char* name, const char* type, bool global_search)
{
	unsigned int name_hash = CIwGameString::CalculateHash(name);
	unsigned int type_hash = 0;

	if (type != NULL)
		type_hash = CIwGameString::CalculateHash(type);

	// Find and return named resource of specific type
	for (_Iterator it = Resources.begin(); it != Resources.end(); ++it)
	{
		if (type == NULL)
		{
			if (name_hash == (*it)->getNameHash())
				return *it;
		}
		else
		{
			if (name_hash == (*it)->getNameHash() && type_hash == (*it)->getClassTypeHash())
				return *it;
		}
	}

	if (global_search)
	{
		// Not found so check global resources
		CIwGameXomlResourceManager* m = IW_GAME_GLOBAL_RESOURCES->getResourceManager();
		if (m != this)	// Prevent global resource manager from calling itself
		{
			if (type != NULL)
				return m->findResource(name_hash, type_hash, false);
			else
				return m->findResource(name, type, false);
		}
	}

	return NULL;
}

CIwGameFont* CIwGameXomlResourceManager::findFontWithFilename(const char* filename, bool global_search)
{
	unsigned int name_hash = CIwGameString::CalculateHash(filename);
	unsigned int type_hash = CIwGameXomlNames::Font_Hash;

	// Find and return named resource of specific type
	for (_Iterator it = Resources.begin(); it != Resources.end(); ++it)
	{
		if (type_hash == (*it)->getClassTypeHash())
		{
			CIwGameFont* font =	(CIwGameFont*)*it;
			if (font->getFilename().getHash() == name_hash && font->getUsingFont() == NULL)
				return (CIwGameFont*)*it;
		}
	}

	if (global_search)
	{
		// Not found so check global resources
		CIwGameXomlResourceManager* m = IW_GAME_GLOBAL_RESOURCES->getResourceManager();
		if (m != this)	// Prevent global resource manager from calling itself
			return m->findFontWithFilename(filename, false);
	}

	return NULL;
}

IIwGameXomlResource* CIwGameXomlResourceManager::FindResource(unsigned int name_hash, unsigned int type_hash, IIwGameXomlResource* container)
{
	IIwGameXomlResource* res = NULL;

	// If container class is an actor then check the actors scenes resource manager
	if (container != NULL && container->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		container = ((CIwGameActor*)container)->getScene();

	// If container class is a scene then check the scenes resource manager
	if (container != NULL && container->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		res = ((CIwGameScene*)container)->getResourceManager()->findResource(name_hash, type_hash, true);
	else
		res = IW_GAME_GLOBAL_RESOURCES->getResourceManager()->findResource(name_hash, type_hash, false);

	return res;
}

IIwGameXomlResource* CIwGameXomlResourceManager::FindResource(const char* name, unsigned int type_hash, IIwGameXomlResource* container)
{
	return FindResource(IW_GAME_HASH(name), type_hash, container);
}


void CIwGameXomlResourceManager::clearResources()
{
	// Remove and delete all images from the manager
	for (_Iterator it = Resources.begin(); it != Resources.end(); ++it)
	{
		delete *it;
	}
	Resources.clear();
}


//
// 
// 
//
// CIwGameXoml Implementation
// 
// 
// 
//
CDECLARE_SINGLETON(CIwGameXoml)

IIwGameXomlClassCreator* CIwGameXoml::findClass(unsigned int name_hash)
{
	for (_Iterator it = ClassCreators.begin(); it != ClassCreators.end(); ++it)
	{
		if ((*it)->getClassNameHash() == name_hash)
			return *it;
	}

	return NULL;
}

IIwGameXomlClassCreator* CIwGameXoml::findClass(const char* name)
{
	return findClass(CIwGameString::CalculateHash(name));
}

IIwGameXomlAction* CIwGameXoml::findAction(unsigned int name_hash)
{
	for (_ActionsIterator it = Actions.begin(); it != Actions.end(); ++it)
	{
		if ((*it)->getActionNameHash() == name_hash)
			return *it;
	}

	return NULL;
}

IIwGameXomlAction* CIwGameXoml::findAction(const char* name)
{
	return findAction(CIwGameString::CalculateHash(name));
}

void CIwGameXoml::Init()
{
	ExitOnError = true;

	// Add some basic classes to the XOML tags namespace
	addClass(new CIwGameTemplateFromCreator());		// CIwGameTemplateFrom object 
	addClass(new CIwGameActionsCreator());			// CIwGameActions object 
	addClass(new CIwGameAnimCreator());				// CIwGameAnim object 
	addClass(new CIwGameAnimTimelineCreator());		// CIwGameAnimTimeline object 
	addClass(new CIwGameActorImageCreator());		// CIwGameActorImage object
	addClass(new CIwGameActorTextCreator());		// CIwGameActorText object
	addClass(new CIwGameModifierManagerCreator());	// CIwGameModifierManager object
	addClass(new CIwGameXomlBindingsCreator());		// CIwGameXomlBindings object 
	addClass(new CIwGameStyleCreator());			// CIwGameStyle object 
	addClass(new CIwGameTemplateCreator());			// CIwGameTemplate object 
#if defined(IW_GAME_BOX2D)
	addClass(new CIwGameBox2dMaterialCreator());	// CIwGameBox2dMaterial object 
	addClass(new CIwGameBox2dJointsCreator());		// CIwGameBox2dJoints object 
#endif // IW_GAME_BOX2D_H_
	addClass(new CIwGameSceneCreator());			// CIwGameScene object 
	addClass(new CIwGameCameraCreator());			// CIwGameCamera object 
	addClass(new CIwGameBrushCreator());			// CIwGameImage object 
	addClass(new CIwGameShapeCreator());			// CIwGameShape object 
	addClass(new CIwGameImageCreator());			// CIwGameImage object 
	addClass(new CIwGameSoundCreator());			// CIwGameSound object 
	addClass(new CIwGameResourceGroupCreator());	// CIwGameResourceGroup object 
	addClass(new CIwGameXomlAddVariableCreator());	// CIwGameXomlAddVariable object 
	addClass(new CIwGameXomlLoadCreator());			// CIwGameXomlLoad object 
	addClass(new CIwGameFontCreator());				// CIwGameFont object 
	addClass(new CIwGameVideoCreator());			// CIwGameVideo object 
	addClass(new CIwGameDataFileCreator());			// CIwGameDataFile object 
	addClass(new CIwGameCreator());					// CIwGame object

	// Add scene actions
	for (int t = 0; t < CIwGameXomlAction_Scenes::Action_Max; t++)
		addAction(new CIwGameXomlAction_Scenes((CIwGameXomlAction_Scenes::ActionType)t));
	// Add actor actions
	for (int t = 0; t < CIwGameXomlAction_Actors::Action_Max; t++)
		addAction(new CIwGameXomlAction_Actors((CIwGameXomlAction_Actors::ActionType)t));
	// Add global actions
	for (int t = 0; t < CIwGameXomlAction_Global::Action_Max; t++)
		addAction(new CIwGameXomlAction_Global((CIwGameXomlAction_Global::ActionType)t));

}

void CIwGameXoml::InitExtensions()
{
	// Add extensions to XOML tags namescape
	addClass(new CIwGameUILabelCreator());
	addClass(new CIwGameUIIconCreator());
	addClass(new CIwGameUITextBoxCreator());
	addClass(new CIwGameUICanvasCreator());
	addClass(new CIwGameUIStackPanelCreator());
	addClass(new CIwGameUIWrapPanelCreator());
	addClass(new CIwGameUIGridCreator());
	addClass(new CIwGameUIListBoxCreator());
	addClass(new CIwGameUISliderCreator());
	addClass(new CIwGameUIImageViewCreator());
	addClass(new CIwGameUITextViewCreator());
	addClass(new CIwGameUITabCreator());
	addClass(new CIwGameUITabsCreator());
	addClass(new CIwGameUITabBarCreator());
	addClass(new CIwGameActorParticlesCreator());
	addClass(new CIwGameActorConnectorCreator());
	addClass(new CIwGameProgramCreator());
	addClass(new CIwGameUIWebViewCreator());
	addClass(new CIwGameUIVideoOverlayCreator());
	addClass(new CIwGameScriptCreator());
	addClass(new CIwGameRemoteReqCreator());
	addClass(new CIwGameVideoCamCreator());

	// Add modifiers to modifiers list
	IW_GAME_MODS->addModifier(new CIwGameModCollisionNotifyCreator());
	IW_GAME_MODS->addModifier(new CIwGameModFollowHeadingCreator());
	IW_GAME_MODS->addModifier(new CIwGameModScriptCreator());
}

void CIwGameXoml::Release()
{
	// Clean up actions
	for (_ActionsIterator it = actions_begin(); it != actions_end(); ++it)
	{
		delete *it;
	}
	Actions.clear();

	// Clean up class creators
	for (_Iterator it = begin(); it != end(); ++it)
	{
		delete *it;
	}
	ClassCreators.clear();
}

bool CIwGameXoml::Process(IIwGameXomlResource* parent, const char* filename, bool reset_xml_pools)
{
//	if (reset_xml_pools)
//		CIwGameXmlParser::PoolsReset();
	CIwGameXmlParser*	xml = new CIwGameXmlParser();

	// Load and parse the xml file
	if (xml->Parse(filename) == eXMLParserError_None)
	{
		// Process the XOML data
		bool error = Process(parent, xml->GetRoot()->GetFirstNode());
		delete xml;
		return error;
	}
	delete xml;

	return false;
}

bool CIwGameXoml::Process(IIwGameXomlResource* parent, CIwGameXmlNode* node, CIwGameString* prefix)
{
CIwGameError::LogError("CIwGameXoml::ProcessEnter: Processing: ", node->GetName().c_str());
//CIwGameError::LogError("CIwGameXoml::ProcessEnter: Memory used: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_USED)).c_str());

	node->ToLower(true);
	CIwGameString out;

	for (CIwGameXmlNode::_Iterator it = node->begin(); it != node->end(); it++)
	{
		unsigned int name_hash = (*it)->GetName().getHash();

		// Check to see if we support the class
		bool found = false;
		for (_Iterator it2 = begin(); it2 != end(); ++it2)
		{
			if ((*it2)->getClassNameHash() == name_hash)
			{
				found = true;

				if (s3eMemoryGetInt(S3E_MEMORY_FREE) < 512 * 1024)
					CIwGameError::LogError("Warning: Free memory is low: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_USED)).c_str());
				if (s3eMemoryGetInt(S3E_MEMORY_LFB) < 128 * 1024)
					CIwGameError::LogError("Warning: Largest free memory black is low: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_LFB)).c_str());

				if (g_ShowTracing > 1)
				{
					CIwGameError::LogError("Creating instance: ", (*it)->GetName().c_str());
					out = "    ";
					(*it)->PrintAttributes(out);
					CIwGameError::LogError(out.c_str());
				}

				// We do support the class so call the classes creator to instantiate it
				IIwGameXomlResource* instance = (*it2)->CreateInstance(parent);
				if (instance != NULL)
				{
					bool success = false;
					// Load up from XOML
					success = instance->LoadFromXoml(parent, true, *it);
					if (success)
						success = instance->PostLoadFromXoml(parent, *it);
					if (!success)
					{
						delete instance;
						if (ExitOnError)
						{
							CIwGameError::LogError("Error: Near XOML tag ", (*it)->GetName().c_str());
							return false;
						}
						ExitOnError = true;
					}
				}

				if (g_ShowTracing > 1)
				{
					CIwGameError::LogError("Instance created: ", (*it)->GetName().c_str());
					out = "    ";
					(*it)->PrintAttributes(out);
					CIwGameError::LogError(out.c_str());
				}

				break;
			}
		}

//		if (!found)
//			CIwGameError::LogError("Warning: No XOML creator present for tag - ", (*it)->GetName().c_str());

	}

//CIwGameError::LogError("CIwGameXoml::ProcessExit: Memory used: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_USED)).c_str());
//CIwGameError::LogError("CIwGameXoml::ProcessExit: Memory free: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_FREE)).c_str());
//CIwGameError::LogError("CIwGameXoml::ProcessExit: Memory LFB: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_LFB)).c_str());

	return true;
}

void CIwGameXoml::ApplyStyle(CIwGameStyle* style, CIwGameXmlNode* node)
{
	// Add style properties to nodes
	for (CIwGameStyle::_Iterator it = style->begin(); it != style->end(); ++it)
	{
		node->AddAttribute((*it)->Name, (*it)->Value, node->getParser());
	}
}

// 
// 
//
//
// CIwGameXomlVariable implementation
//
//
//
//
bool CIwGameXomlVariable::checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val)
{
	switch (condition)
	{
	case CO_Equal:
		if (Value == val)
			return true;
		break;
	case CO_NotEqual:
		if (!(Value == val))
			return true;
		break;
	case CO_Greater:
		if (Value.GetLength() > val.GetLength())
			return true;
		break;
	case CO_GreaterEqual:
		if (Value.GetLength() >= val.GetLength())
			return true;
		break;
	case CO_Less:
		if (Value.GetLength() < val.GetLength())
			return true;
		break;
	case CO_LessEqual:
		if (Value.GetLength() <= val.GetLength())
			return true;
		break;
	case CO_And:
		Value.FindReset();
		if (Value.Find(val.c_str()) >= 0)
			return true;
		break;
	}

	return false;
}

CIwGameXomlVariable* CIwGameXomlVariable::GetVariable(CIwGameString& value, IIwGameXomlResource* parent)
{
	if (value.GetLength() < 2)
		return NULL;
	if (CIwGameString::IsNumber(*value.c_str()))
		return NULL;

	CIwGameXomlVariable* var = NULL;
	CIwGameString variable_name;
	int index = -1;
	if (value.SplitVarIndex(variable_name, index))
	{
		var = CIwGameXomlVariableManager::FindVariable(variable_name.getHash(), parent);
		if (var != NULL)
		{
			if (var->isArray() && index >= 0)
				var = ((CIwGameXomlVariableArray*)var)->getElement(index);
		}
	}

	return var;
}

bool CIwGameXomlVariable::Save()
{
	if (Persist && Value.c_str() != NULL)
	{
		CIwGameString name = Name;
		name += ".var";
		CIwGameFile file;
		if (file.Open(name.c_str(), "wb"))
		{
			if (!file.Write((void *)Value.c_str(), Value.GetLength() + 1))
				return false;
		}
		else
			return false;
	}

	return true;
}

bool CIwGameXomlVariable::Load()
{
	if (Persist)
	{
		CIwGameString name = Name;
		name += ".var";
		CIwGameFile file;
		if (file.Open(name.c_str(), "rb"))
		{
			int len = file.getFileSize();
			if (len <= 0)
				return false;
			CIwGameString s;
			s.allocString(len);
			if (!file.Read((void *)s.c_str(), len))
				return false;
			setValue(s.c_str());
		}
		else
			return false;
	}

	return true;
}

// 
// 
//
//
// CIwGameXomlVariableBool implementation
//
//
//
//
bool CIwGameXomlVariableBool::checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val)
{
	switch (condition)
	{
	case CO_Equal:
	case CO_And:
		if (Value == val)
			return true;
		break;
	case CO_NotEqual:
		if (!(Value == val))
			return true;
		break;
	}

	return false;
}

// 
// 
//
//
// CIwGameXomlVariableFloat implementation
//
//
//
//
bool CIwGameXomlVariableFloat::checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val)
{
	float value = val.GetAsFloat();
	switch (condition)
	{
	case CO_Equal:
		if (NativeValue == value)
			return true;
		break;
	case CO_NotEqual:
		if (NativeValue != value)
			return true;
		break;
	case CO_Greater:
		if (NativeValue > value)
			return true;
		break;
	case CO_GreaterEqual:
		if (NativeValue >= value)
			return true;
		break;
	case CO_Less:
		if (NativeValue < value)
			return true;
		break;
	case CO_LessEqual:
		if (NativeValue <= value)
			return true;
		break;
	case CO_And:
		break;
	}

	return false;
}

// 
// 
//
//
// CIwGameXomlVariableInt implementation
//
//
//
//
bool CIwGameXomlVariableInt::checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val)
{
	int value = val.GetAsInt();
	switch (condition)
	{
	case CO_Equal:
		if (NativeValue == value)
			return true;
		break;
	case CO_NotEqual:
		if (NativeValue != value)
			return true;
		break;
	case CO_Greater:
		if (NativeValue > value)
			return true;
		break;
	case CO_GreaterEqual:
		if (NativeValue >= value)
			return true;
		break;
	case CO_Less:
		if (NativeValue < value)
			return true;
		break;
	case CO_LessEqual:
		if (NativeValue <= value)
			return true;
		break;
	case CO_And:
		if ((NativeValue & value) == value)
			return true;
		break;
	}

	return false;
}

// 
// 
//
//
// CIwGameXomlVariableVec2 implementation
//
//
//
//
bool CIwGameXomlVariableVec2::checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val)
{
	float values[8];
	int count = ((CIwGameString&)val).GetAsListOfFloat(values);
	if (count != 2)
		return false;
	CIwFVec2 value = CIwFVec2(values[0], values[1]);
	switch (condition)
	{
	case CO_Equal:
		if (NativeValue == value)
			return true;
		break;
	case CO_NotEqual:
		if (NativeValue != value)
			return true;
		break;
	case CO_Greater:
		if (NativeValue.GetLengthSquared() > value.GetLengthSquared())
			return true;
		break;
	case CO_GreaterEqual:
		if (NativeValue.GetLengthSquared() >= value.GetLengthSquared())
			return true;
		break;
	case CO_Less:
		if (NativeValue.GetLengthSquared() < value.GetLengthSquared())
			return true;
		break;
	case CO_LessEqual:
		if (NativeValue.GetLengthSquared() <= value.GetLengthSquared())
			return true;
		break;
	}

	return false;
}

// 
// 
//
//
// CIwGameXomlVariableVec3 implementation
//
//
//
//
bool CIwGameXomlVariableVec3::checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val)
{
	float values[8];
	int count = ((CIwGameString&)val).GetAsListOfFloat(values);
	if (count != 3)
		return false;
	CIwFVec3 value = CIwFVec3(values[0], values[1], values[2]);
	switch (condition)
	{
	case CO_Equal:
		if (NativeValue == value)
			return true;
		break;
	case CO_NotEqual:
		if (NativeValue != value)
			return true;
		break;
	case CO_Greater:
		if (NativeValue.GetLengthSquared() > value.GetLengthSquared())
			return true;
		break;
	case CO_GreaterEqual:
		if (NativeValue.GetLengthSquared() >= value.GetLengthSquared())
			return true;
		break;
	case CO_Less:
		if (NativeValue.GetLengthSquared() < value.GetLengthSquared())
			return true;
		break;
	case CO_LessEqual:
		if (NativeValue.GetLengthSquared() <= value.GetLengthSquared())
			return true;
		break;
	}

	return false;
}

// 
// 
//
//
// CIwGameXomlVariableVec4 implementation
//
//
//
//
bool CIwGameXomlVariableVec4::checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val)
{
	float values[8];
	int count = ((CIwGameString&)val).GetAsListOfFloat(values);
	if (count != 4)
		return false;
	CIwFVec4 value = CIwFVec4(values[0], values[1], values[2], values[3]);
	switch (condition)
	{
	case CO_Equal:
		if (NativeValue == value)
			return true;
		break;
	case CO_NotEqual:
		if (NativeValue != value)
			return true;
		break;
	case CO_Greater:
		if (NativeValue.GetLengthSquared() > value.GetLengthSquared())
			return true;
		break;
	case CO_GreaterEqual:
		if (NativeValue.GetLengthSquared() >= value.GetLengthSquared())
			return true;
		break;
	case CO_Less:
		if (NativeValue.GetLengthSquared() < value.GetLengthSquared())
			return true;
		break;
	case CO_LessEqual:
		if (NativeValue.GetLengthSquared() <= value.GetLengthSquared())
			return true;
		break;
	}

	return false;
}

// 
// 
//
//
// CIwGameXomlVariableCondition implementation
//
//
//
//
void CIwGameXomlVariableCondition::setValue(const char* value)
{
	Value = value;

	// Build condition list
	// Parse out string parameters
	CIwList<CIwSVec2> params;
	int pos, len;
	while (1)
	{
		pos = Value.getFindIndex();
		len = Value.GetNextString();
		if (len <= 0) break;
		Value.StepFindIndex(1);
		params.push_back(CIwSVec2(pos, len));
	}

	Changed = true;

	IIwGameXomlResource* parent = Parent->getParent();

	// Go through parameters to find out what they are
	CIwGameString variable;
	CIwGameString op;
	CIwGameString val;
	CIwGameString join_op;
	for (CIwList<CIwSVec2>::iterator it = params.begin(); it != params.end(); it++)
	{
		// Get variable name
		variable = Value.getSubString((*it).x, (*it).y); it++;
		if (it == params.end())
		{
			CIwGameError::LogError("Warning: Variable found but no operator or value");
			return;
		}

		// Get operator
		op = Value.getSubString((*it).x, (*it).y); it++;
		if (it == params.end())
		{
			CIwGameError::LogError("Warning: operator found but no value");
			return;
		}

		// Determine operator
		eIwGameXomlConditionOperator type;
		op.ToLower();
		if (op == "==")	type = CO_Equal; else
		if (op == "!=")	type = CO_NotEqual; else
		if (op == "gt")	type = CO_Greater; else
		if (op == "lt")	type = CO_Less; else
		if (op == "gte") type = CO_GreaterEqual; else
		if (op == "lte") type = CO_LessEqual; else
		if (op == "and") type = CO_And; else
		{
			CIwGameError::LogError("Warning: operator invalid - ", op.c_str());
			return;
		}

		// Get value
		val = Value.getSubString((*it).x, (*it).y); it++;

		// Create the condition
		CIwGameXomlCondition* condition = new CIwGameXomlCondition();
		condition->Value = val;
		condition->Operator = type;
		condition->Variable2 = CIwGameXomlVariable::GetVariable(val, parent);

		// Find the variable
		condition->Variable = CIwGameXomlVariable::GetVariable(variable, parent);
		if (condition->Variable == NULL)
		{
			CIwGameError::LogError("Warning: Condition variable could not be found ", variable.c_str());
			return;
		}

		// Add condition to the conditions list
		Conditions.push_back(condition);

		// If no more data left then exit
		if (it == params.end())
			break;

		// Get join operator if its present
		join_op = Value.getSubString((*it).x, (*it).y);
		join_op.ToLower();
		if (join_op == "and")
			condition->Join = CJO_And;
		else
		if (join_op == "or")
			condition->Join = CJO_Or;
		else
		{
			CIwGameError::LogError("Warning: joining operator invalid", join_op.c_str());
			break;
		}

		if (it == params.end())
		{
			CIwGameError::LogError("Warning: joining operator is present but no additional conditions available");
			break;
		}
	}
}

bool CIwGameXomlVariableCondition::isTrue()
{
	bool final_result = false;

	// Check the condition variables
	CIwGameXomlCondition* prev = NULL;
	for (CIwList<CIwGameXomlCondition*>::iterator it = Conditions.begin(); it != Conditions.end(); ++it)
	{
		// Find the variable
		CIwGameXomlVariable* var = (*it)->Variable;
		CIwGameXomlVariable* var2 = (*it)->Variable2;
		if (var != NULL)
		{
			bool t1 = false;

			if (var->Type == VT_Condition)	// If condition var then evaluate it
				t1 = var->isTrue();

			bool result = false;
			
			if (var2 != NULL)
			{
				if (var2->Type == VT_Condition)	// If condition var then evaluate it
				{
					bool t2 = var2->isTrue();
					if (var->Type == VT_Condition)
					{
						if ((*it)->Operator == CO_NotEqual)
							result = t1 != t2;
						else
							result = t1 == t2;
					}
					else
						result = var->checkCondition((*it)->Operator, t2 ? "1" : "0");
				}
				else
					result = var->checkCondition((*it)->Operator, var2->getValue());
			}
			else
			{
				if (var->Type == VT_Condition)
				{
					bool val = (*it)->Value.GetAsBool();
					if ((*it)->Operator == CO_NotEqual)
						result = t1 != val;
					else
						result = t1 == val;
				}
				else
					result = var->checkCondition((*it)->Operator, (*it)->Value);
			}

			// If joined by AND operator then rest of conditions are irrelevant
			if (!result && (*it)->Join == CJO_And)
			{
				NativeValue = false;
				return false;
			}

			if (prev == NULL)
				final_result = result;
			else
			{
				if (prev->Join == CJO_And)
					final_result = final_result && result;
				else
				if (prev->Join == CJO_Or)
					final_result = final_result || result;
			}
		}
		else
		{
#if defined(_DEBUG)
			CIwGameError::LogError("Warning: variable could not be fuond - ", var->Name.c_str());
#endif	// _DEBUG
		}
		prev = *it;
	}
	NativeValue = final_result;

	return NativeValue;
}

// 
// 
//
//
// CIwGameXomlVariableArray implementation
//
//
//
//
void CIwGameXomlVariableArray::setValue(const char* value)
{
	CIwGameString values = value;
	Changed = true;

	if (values.Contains(',') >= 0)
	{
		// Check to see if we have enough space, if not then enlarge the array to fit
		int occurs = values.Occurrences(',') + 1;
		int diff = occurs - Values.getSize();
		if (diff > 0)
		{
			Values.grow(diff);
			for (int t = 0; t < diff; t++)
			{
				CIwGameXomlVariable* var2 = NULL;
				switch (Type)
				{
				case VT_ArrayString:
					var2 = new CIwGameXomlVariable();
					break;
				case VT_ArrayBool:
					var2 = new CIwGameXomlVariableBool();
					break;
				case VT_ArrayFloat:
					var2 = new CIwGameXomlVariableFloat();
					break;
				case VT_ArrayInt:
					var2 = new CIwGameXomlVariableInt();
					break;
				}

				// Append the variable
				addElement(var2);
			}
		}

		for (int t = 0; t < Values.getSize(); t++)
		{
			int offset = values.getFindIndex();
			int end = values.FindNext(',');
			values.StepFindIndex(1);
			if (end > 0)
				Values.element_at(t)->setValue(values.c_str() + offset, end - offset);
			else
			if (values.getFindIndex() >= values.GetLength())
			{
				Values.element_at(t)->setValue(values.c_str() + offset, values.GetLength() - offset);
				break;
			}
			if (values.StepFindIndexNoneWhiteSpace() < 0)
				break;
		}
	}
	else
	{
		for (int t = 0; t < Values.getSize(); t++)
			Values.element_at(t)->setValue(value);
	}
}

bool CIwGameXomlVariableArray::checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val)
{
	switch (condition)
	{
	case CO_Equal:			// Check to see if array contains the supplied value
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (Values.element_at(t)->getValue() == val)
						return true;
				}
			}
		}
		break;
	case CO_NotEqual:
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (Values.element_at(t)->getValue() == val)
						return false;
				}
			}
			return true;
		}
		break;
	case CO_And:
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				CIwGameXomlVariable* var = Values.element_at(t);
				if (var != NULL)
				{
					CIwGameString& value = var->getValue();
					value.FindReset();
					if (value.Find(val.c_str()) >= 0)
						return true;
				}
			}
		}
		break;
	}

	return false;
}

void CIwGameXomlVariableArray::BindXML(const char* binding)
{
	CIwGameString s = binding;
	CIwGameSlotArray<CIwGameString*>* split = s.Split(':');
	if (split->element_at(0) != NULL)
	{
		IIwGameXomlResource* parent = Parent->getParent();
		CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(*split->element_at(0), parent);
		if (var == NULL)
		{
			CIwGameError::LogError("Warning: XML variable could not be found ", split->element_at(0)->c_str());
			split->clear(true);
			delete split;
			return;
		}
		if (!var->isXML())
		{
			CIwGameError::LogError("Warning: Variable is not an XML variable ", split->element_at(0)->c_str());
			split->clear(true);
			delete split;
			return;
		}
		XmlVar = (CIwGameXomlVariableXML*)var;

		if (split->element_at(1) != NULL)
			setXmlTagName(split->element_at(1)->c_str());
		else
		{
			CIwGameError::LogError("Warning: BindXML tag name is missing");
			split->clear(true);
			delete split;
			XmlVar = NULL;
			return;
		}
		if (split->element_at(2) != NULL)
			setXmlAttribName(split->element_at(2)->c_str());
		else
		{
			CIwGameError::LogError("Warning: BindXML attribute name is missing");
			split->clear(true);
			delete split;
			XmlVar = NULL;
			return;
		}
	}

	split->clear(true);
	delete split;
}

void CIwGameXomlVariableArray::UpdateFromXML()
{
	if (XmlVar != NULL && XmlVar->Modified && XmlVar->getRoot() != NULL)
	{
		Values.clear(true);
		CIwGameXmlNodeList*	pNodes = new CIwGameXmlNodeList;
		XmlVar->getRoot()->GetNamedNodes(XmlTagNameHash, pNodes);

		for (CIwGameXmlNodeList::iterator it = pNodes->begin(); it != pNodes->end(); ++it)
		{
			CIwGameXmlAttribute* attrib = (*it)->GetAttribute(XmlAttribNameHash);
			if (attrib != NULL)
			{
				CIwGameXomlVariable* var = new CIwGameXomlVariable();
				var->setValue(attrib->GetValue().c_str());
				Values.add(var);
			}
		}
		Values.resize(Values.count());
		delete pNodes;
	}
}

bool CIwGameXomlVariableArray::Save()
{
	if (Persist)
	{
		CIwGameString name = Name;
		name += ".var";
		CIwGameFile file;
		if (file.Open(name.c_str(), "wb"))
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				CIwGameXomlVariable* var = Values.element_at(t);
				int len = var->getValue().GetLength() + 1;
				if (!file.Write((void *)&len, 4))
					return false;

				if (len > 0)
				{
					if (!file.Write((void *)var->getValue().c_str(), len))
						return false;
				}
				
			}
		}
		else
			return false;
	}

	return true;
}

bool CIwGameXomlVariableArray::Load()
{
	if (Persist)
	{
		CIwGameString name = Name;
		name += ".var";
		CIwGameFile file;
		CIwGameString s;
		if (file.Open(name.c_str(), "rb"))
		{
			int len = file.getFileSize();
			if (len <= 0)
				return false;

			for (int t = 0; t < Values.getSize(); t++)
			{
				CIwGameXomlVariable* var = Values.element_at(t);
				int len = 0;
				if (!file.Read(&len, 4))
					break;
				if (len > 0)
				{
					s.allocString(len);
					if (!file.Read((void*)s.c_str(), len))
						break;
					var->setValue(s.c_str());
				}
			}
		}
		else
			return false;
	}

	return true;
}

// 
// 
//
//
// CIwGameXomlVariableArrayBool implementation
//
//
//
//
bool CIwGameXomlVariableArrayBool::checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val)
{
	bool res = val.GetAsBool();
	switch (condition)
	{
	case CO_Equal:			// Check to see if array contains the supplied value
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (((CIwGameXomlVariableBool*)Values.element_at(t))->NativeValue != res)
						return false;
				}
			}
			return true;
		}
		break;
	case CO_NotEqual:
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (((CIwGameXomlVariableBool*)Values.element_at(t))->NativeValue == res)
						return false;
				}
			}
			return true;
		}
		break;
	case CO_And:
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (((CIwGameXomlVariableBool*)Values.element_at(t))->NativeValue == res)
						return true;
				}
			}
		}
		break;
	}

	return false;
}

// 
// 
//
//
// CIwGameXomlVariableArrayInt implementation
//
//
//
//
bool CIwGameXomlVariableArrayInt::checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val)
{
	int res = val.GetAsInt();
	switch (condition)
	{
	case CO_Equal:			// Check to see if array contains the supplied value
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (((CIwGameXomlVariableInt*)Values.element_at(t))->NativeValue == res)
						return true;
				}
			}
		}
		break;
	case CO_NotEqual:
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (((CIwGameXomlVariableInt*)Values.element_at(t))->NativeValue == res)
						return false;
				}
			}
			return true;
		}
		break;
	case CO_Greater:
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (((CIwGameXomlVariableInt*)Values.element_at(t))->NativeValue > res)
						return true;
				}
			}
		}
		break;
	case CO_GreaterEqual:
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (((CIwGameXomlVariableInt*)Values.element_at(t))->NativeValue >= res)
						return true;
				}
			}
		}
		break;
	case CO_Less:
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (((CIwGameXomlVariableInt*)Values.element_at(t))->NativeValue < res)
						return true;
				}
			}
		}
		break;
	case CO_LessEqual:
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (((CIwGameXomlVariableInt*)Values.element_at(t))->NativeValue <= res)
						return true;
				}
			}
		}
		break;
	case CO_And:
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if ((((CIwGameXomlVariableInt*)Values.element_at(t))->NativeValue & res) == res)
						return true;
				}
			}
		}
		break;
	}

	return false;
}

// 
// 
//
//
// CIwGameXomlVariableArrayFloat implementation
//
//
//
//
bool CIwGameXomlVariableArrayFloat::checkCondition(eIwGameXomlConditionOperator condition, const CIwGameString& val)
{
	float res = val.GetAsFloat();
	switch (condition)
	{
	case CO_Equal:			// Check to see if array contains the supplied value
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (((CIwGameXomlVariableFloat*)Values.element_at(t))->NativeValue == res)
						return true;
				}
			}
		}
		break;
	case CO_NotEqual:
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (((CIwGameXomlVariableFloat*)Values.element_at(t))->NativeValue == res)
						return false;
				}
			}
			return true;
		}
		break;
	case CO_Greater:
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (((CIwGameXomlVariableFloat*)Values.element_at(t))->NativeValue > res)
						return true;
				}
			}
		}
		break;
	case CO_GreaterEqual:
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (((CIwGameXomlVariableFloat*)Values.element_at(t))->NativeValue >= res)
						return true;
				}
			}
		}
		break;
	case CO_Less:
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (((CIwGameXomlVariableFloat*)Values.element_at(t))->NativeValue < res)
						return true;
				}
			}
		}
		break;
	case CO_LessEqual:
		{
			for (int t = 0; t < Values.getSize(); t++)
			{
				if (Values.element_at(t) != NULL)
				{
					if (((CIwGameXomlVariableFloat*)Values.element_at(t))->NativeValue <= res)
						return true;
				}
			}
		}
		break;
	}

	return false;
}

// 
// 
//
//
// CIwGameXomlVariableXML implementation
//
//
//
//
void CIwGameXomlVariableXML::setValue(const char* value)
{
	Value = value;
	Changed = true;
	if (value == NULL)
		return;

	SAFE_DELETE(Parser);
	Parser = new CIwGameXmlParser();

	// Load and parse the xml file then create a copy of the tree (we need a copy because the pooled xml tags may get re-used)
	CIwGameDataInput* data = new CIwGameDataInput(true);
	data->setData((char*)Value.c_str(), Value.GetLength());
	if (Parser->Parse(data) != eXMLParserError_None)
		CIwGameError::LogError("Warning: Error parsing XML file for variable - ", Name.c_str());
	else
		Root = Parser->GetRoot();

}

// 
// 
//
//
// CIwGameXomlVariableManager implementation
//
//
//
//

void CIwGameXomlVariableManager::addVariable(CIwGameXomlVariable* variable)
{
#if defined(IW_REMOVE_DUPLICATES)
	if (findVariable(variable->Name.getHash()))
	{
		CIwGameError::LogError("Warning: Variable already exists, variable was replaced - ", variable->Name.c_str());
		removeVariable(variable->Name.getHash());
	}
#endif
	Variables.push_back(variable);
	variable->Parent = this;
}

CIwGameXomlVariable* CIwGameXomlVariableManager::addVariable(const char* name, eCIwGameXomlVariableType type)
{
#if defined(IW_REMOVE_DUPLICATES)
	CIwGameXomlVariable* var = findVariable(CIwGameString::CalculateHash(name));
	if (var != NULL)
	{
		CIwGameError::LogError("Warning: Variable already exists, variable was replaced - ", name);
		removeVariable(var);
	}
#endif
	CIwGameXomlVariable* v = CreateVariable(name, type);
	Variables.push_back(v);
	v->Parent = this;

	return v;
}

CIwGameXomlVariable* CIwGameXomlVariableManager::addVariable(const char* name, eCIwGameXomlVariableType type, const char* value, int size)
{
#if defined(IW_REMOVE_DUPLICATES)
	CIwGameXomlVariable* var = findVariable(CIwGameString::CalculateHash(name));
	if (var != NULL)
	{
		CIwGameError::LogError("Warning: Variable already exists, variable was replaced - ", name);
		removeVariable(var);
	}
#endif
	CIwGameXomlVariable* v = CreateVariable(name, type);
	if (v->isArray())
	{
		if (size == 0)
			size = 1;
		((CIwGameXomlVariableArray*)v)->Init(size);
	}
	v->setValue(value);
	Variables.push_back(v);
	v->Parent = this;

	return v;
}

void CIwGameXomlVariableManager::removeVariable(CIwGameXomlVariable* var)
{
	removeVariable(var->Name.getHash());
}

void CIwGameXomlVariableManager::removeVariable(unsigned int name_hash)
{
	// Renove named Actions from the manager
	for (_Iterator it = Variables.begin(); it != Variables.end(); ++it)
	{
		if (name_hash == (*it)->Name.getHash())
		{
			delete *it;
			Variables.erase(it);
			break;
		}
	}
}

void CIwGameXomlVariableManager::setVariable(unsigned int name_hash, const char* value)
{
	CIwGameXomlVariable* var = findVariable(name_hash);
	if (var != NULL)
	{
		var->setValue(value);
	}
}

void CIwGameXomlVariableManager::setVariable(const char* name, const char* value)
{
	setVariable(CIwGameString::CalculateHash(name), value);
}

CIwGameXomlVariable* CIwGameXomlVariableManager::findVariable(unsigned int name_hash)
{
	for (_Iterator it = Variables.begin(); it != Variables.end(); ++it)
	{
		if ((*it)->Name.getHash() == name_hash)
		{
			return *it;
		}
	}

	return NULL;
}

CIwGameXomlVariable* CIwGameXomlVariableManager::findVariable(const char* name)
{
	return findVariable(CIwGameString::CalculateHash(name));
}

void CIwGameXomlVariableManager::clearVariables()
{
	// Remove and delete all images from the manager
	for (_Iterator it = Variables.begin(); it != Variables.end(); ++it)
	{
		delete *it;
	}
	Variables.clear();
}

CIwGameXomlVariable* CIwGameXomlVariableManager::CreateVariable(const char* name, eCIwGameXomlVariableType type)
{
	// Create variable of specified type
	switch (type)
	{
		case VT_String: 
			{
				CIwGameXomlVariable* var = new CIwGameXomlVariable();
				var->Parent = this;
				var->Name = name;
				var->Type = type;

				return var;
			}
			break;
		case VT_Bool: 
			{
				CIwGameXomlVariableBool* var = new CIwGameXomlVariableBool();
				var->Parent = this;
				var->Name = name;
				var->Type = type;

				return var;
			}
			break;
		case VT_Float: 
			{
				CIwGameXomlVariableFloat* var = new CIwGameXomlVariableFloat();
				var->Parent = this;
				var->Name = name;
				var->Type = type;

				return var;
			}
			break;
		case VT_Int: 
			{
				CIwGameXomlVariableInt* var = new CIwGameXomlVariableInt();
				var->Parent = this;
				var->Name = name;
				var->Type = type;

				return var;
			}
			break;
		case VT_Vec2: 
			{
				CIwGameXomlVariableVec2* var = new CIwGameXomlVariableVec2();
				var->Parent = this;
				var->Name = name;
				var->Type = type;

				return var;
			}
			break;
		case VT_Vec3: 
			{
				CIwGameXomlVariableVec3* var = new CIwGameXomlVariableVec3();
				var->Parent = this;
				var->Name = name;
				var->Type = type;

				return var;
			}
			break;
		case VT_Vec4: 
			{
				CIwGameXomlVariableVec4* var = new CIwGameXomlVariableVec4();
				var->Parent = this;
				var->Name = name;
				var->Type = type;

				return var;
			}
			break;
		case VT_Condition: 
			{
				CIwGameXomlVariableCondition* var = new CIwGameXomlVariableCondition();
				var->Parent = this;
				var->Name = name;
				var->Type = type;

				return var;
			}
			break;
		case VT_ArrayString: 
			{
				CIwGameXomlVariableArray* var = new CIwGameXomlVariableArray();
				var->Parent = this;
				var->Name = name;
				var->Type = type;

				return var;
			}
			break;
		case VT_ArrayBool: 
			{
				CIwGameXomlVariableArrayBool* var = new CIwGameXomlVariableArrayBool();
				var->Parent = this;
				var->Name = name;
				var->Type = type;

				return var;
			}
			break;
		case VT_ArrayFloat: 
			{
				CIwGameXomlVariableArrayFloat* var = new CIwGameXomlVariableArrayFloat();
				var->Parent = this;
				var->Name = name;
				var->Type = type;

				return var;
			}
			break;
		case VT_ArrayInt: 
			{
				CIwGameXomlVariableArrayInt* var = new CIwGameXomlVariableArrayInt();
				var->Parent = this;
				var->Name = name;
				var->Type = type;

				return var;
			}
			break;
		case VT_XML: 
			{
				CIwGameXomlVariableXML* var = new CIwGameXomlVariableXML();
				var->Parent = this;
				var->Name = name;
				var->Type = type;

				return var;
			}
			break;
	}

	return NULL;
}

CIwGameXomlVariable* CIwGameXomlVariableManager::FindVariable(const char* name, IIwGameXomlResource* container)
{
	return FindVariable(CIwGameString::CalculateHash(name), container);
}

CIwGameXomlVariable* CIwGameXomlVariableManager::FindVariable(unsigned int name_hash, IIwGameXomlResource* container)
{
	CIwGameXomlVariable* var = NULL;
	
	// If container class is an actor then check the actors scenes variables manager
	if (container != NULL && container->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		container = ((CIwGameActor*)container)->getScene();
	// If container class is a scene then check the scenes variables manager
	if (container != NULL && container->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		var = (CIwGameXomlVariable*)((CIwGameScene*)container)->getVariableManager()->findVariable((name_hash));

	// If the variable has no container or the variable was not found in the supplied container then check the global variables manager
	if (var == NULL)
		var = (CIwGameXomlVariable*)IW_GAME_GLOBAL_RESOURCES->getVariableManager()->findVariable((name_hash));

	return var;
}

// 
// 
//
//
// CIwGameXomlBindings implementation
//
//
//
//

void CIwGameXomlBindings::addBinding(CIwGameXomlBinding* binding)
{
	Bindings.push_back(binding);
}

void CIwGameXomlBindings::addBinding(const CIwGameString& property_name, CIwGameXomlVariable* variable)
{
	CIwGameXomlBinding* binding = new CIwGameXomlBinding();
	binding->Property = property_name.getHash();
#if defined(_DEBUG)
	binding->_Property = property_name;
#endif	// _DEBUG
	binding->Variable = variable;
	Bindings.push_back(binding);
}


void CIwGameXomlBindings::clearBindings()
{
	// Remove and delete all images from the manager
	for (_Iterator it = Bindings.begin(); it != Bindings.end(); ++it)
	{
		delete *it;
	}
	Bindings.clear();
}

bool CIwGameXomlBindings::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	CIwGameScene* scene = NULL;
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		scene = ((CIwGameActor*)parent)->getScene();
	else
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	// Process bindings attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
		{
			setName((*it)->GetValue().c_str());
		}
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
	}

	// Prrocess bindings
	for (CIwGameXmlNode::_Iterator it2 = node->begin(); it2 != node->end(); ++it2)
	{
		unsigned int name_hash = (*it2)->GetName().getHash();
		if (name_hash == CIwGameXomlNames::Binding_Hash)
		{
			CIwGameXomlBinding* binding = new CIwGameXomlBinding();

			// Get binding data
			for (CIwGameXmlNode::_AttribIterator it = (*it2)->attribs_begin(); it != (*it2)->attribs_end(); ++it)
			{
				unsigned int attrib_hash = (*it)->getName().getHash();

				if (attrib_hash == CIwGameXomlNames::Property_Hash)
				{
					(*it)->GetValue().ToLower();
					binding->Property = (*it)->GetValue().getHash();
#if defined(_DEBUG)
					binding->_Property = (*it)->GetValue();
#endif	//_DEBUG
				}
				else
				if (attrib_hash == CIwGameXomlNames::Variable_Hash)
				{
					binding->Variable = CIwGameXomlVariable::GetVariable((*it)->GetValue(), parent);
					if (binding->Variable == NULL)
						CIwGameError::LogError("Error: binding variable not found - ", (*it)->GetValue().c_str());
				}
			}
			// Save action
			Bindings.push_back(binding);
		}
	}

	// If we are declared inside a scene then bindings are local to the scene
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
// CIwGameXomlAddVariable Implementation
// 
// 
// 
//
bool CIwGameXomlAddVariable::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	CIwGameString name, value, type;
	int size = 0;
	CIwGameString* condition = NULL;
	CIwGameString* bind_xml = NULL;
	bool persist = false;

	// Process variable specific attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
			name = (*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Value_Hash)
			value = (*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Type_Hash)
			type = (*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Size_Hash)
			size = (*it)->GetValueAsInt();
		else
		if (name_hash == CIwGameXomlNames::Condition_Hash)
			condition = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::BindXML_Hash)
			bind_xml = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Persist_Hash)
			persist = (*it)->GetValueAsBool();
	}

	if (condition != NULL)
	{
		// Find the condition variable
		CIwGameScene* scene = NULL;
		if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
			scene = (CIwGameScene*)parent;
		bool condition_not = false;
		CIwGameXomlVariable* var = NULL;
		if (*(condition->c_str()) == '!')
		{
			condition_not = true;
			CIwGameString cond = condition->c_str() + 1;
			var = CIwGameXomlVariable::GetVariable(cond, scene);
		}
		else
			var = CIwGameXomlVariable::GetVariable(*condition, scene);
		if (var != NULL)
		{
			bool res = var->isTrue();
			if (condition_not)
				res = !res;
			if (!res)
			{
				IW_GAME_XOML->setExitOnError(false);
				return false;
			}
		}
#if defined (_DEBUG)
		else
			CIwGameError::LogError("Warning: condition variable not found - ", condition->c_str());
#endif // _DEBUG
	}

	if (!name.IsEmpty())
	{
		// Determine type of variable
		eCIwGameXomlVariableType vtype = VT_String;
		if (!type.IsEmpty())
		{
			unsigned int type_hash = type.getHash();
			if (type_hash == CIwGameXomlNames::bool_Hash)
				vtype = VT_Bool;
			else
			if (type_hash == CIwGameXomlNames::float_Hash)
				vtype = VT_Float;
			else
			if (type_hash == CIwGameXomlNames::int_Hash)
				vtype = VT_Int;
			else
			if (type_hash == CIwGameXomlNames::vec2_Hash)
				vtype = VT_Vec2;
			else
			if (type_hash == CIwGameXomlNames::vec3_Hash)
				vtype = VT_Vec3;
			else
			if (type_hash == CIwGameXomlNames::vec4_Hash)
				vtype = VT_Vec4;
			else
			if (type_hash == CIwGameXomlNames::condition_Hash)
				vtype = VT_Condition;
			else
			if (type_hash == CIwGameXomlNames::arraystring_Hash)
				vtype = VT_ArrayString;
			else
			if (type_hash == CIwGameXomlNames::arraybool_Hash)
				vtype = VT_ArrayBool;
			else
			if (type_hash == CIwGameXomlNames::arrayfloat_Hash)
				vtype = VT_ArrayFloat;
			else
			if (type_hash == CIwGameXomlNames::arrayint_Hash)
				vtype = VT_ArrayInt;
			else
			if (type_hash == CIwGameXomlNames::xml_Hash)
				vtype = VT_XML;
		}

		// If we are declared inside a scene then variable is local to the scene else global
		CIwGameXomlVariable* var = NULL;
		if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
			var = ((CIwGameScene*)parent)->getVariableManager()->addVariable(name.c_str(), vtype, value.c_str(), size);
		else
			var = IW_GAME_GLOBAL_RESOURCES->getVariableManager()->addVariable(name.c_str(), vtype, value.c_str(), size);

		if (var != NULL)
		{
			if (bind_xml != NULL && var->isArray())
				((CIwGameXomlVariableArray*)var)->BindXML(bind_xml->c_str());

			if (persist)
			{
				var->setPersistent(persist);
				var->Load();
			}
		}
	}
	else
	{
		CIwGameError::LogError("Error: XOML - Variable must be named");
		return false;
	}

	IW_GAME_XOML->setExitOnError(false);
	return false;
}

//
// 
// 
//
// CIwGameXomlLoad Implementation
// 
// 
// 
//
bool CIwGameXomlLoad::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	CIwGameScene* scene = NULL;
	if (parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	CIwGameString* file = NULL;
	CIwGameString* condition = NULL;

	// Process LoadXoml specific attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::File_Hash)
		{
			file = &(*it)->GetValue();
		}
		else
		if (name_hash == CIwGameXomlNames::Condition_Hash)
		{
			condition = &(*it)->GetValue();
		}
	}

	if (condition != NULL)
	{
		// Find the condition variable
		bool condition_not = false;
		CIwGameXomlVariable* var = NULL;
		if (*(condition->c_str()) == '!')
		{
			condition_not = true;
			CIwGameString cond = condition->c_str() + 1;
			var = CIwGameXomlVariable::GetVariable(cond, scene);
		}
		else
			var = CIwGameXomlVariable::GetVariable(*condition, scene);
		if (var != NULL)
		{
			bool res = var->isTrue();
			if (condition_not)
				res = !res;
			if (!res)
			{
				IW_GAME_XOML->setExitOnError(false);
				return false;
			}
		}
#if defined (_DEBUG)
		else
			CIwGameError::LogError("Warning: condition variable not found - ", condition->c_str());
#endif // _DEBUG
	}


	if (file != NULL)
		IW_GAME_XOML->Process(parent, file->c_str(), false);

	IW_GAME_XOML->setExitOnError(false);
	return false;
}

bool CIwGameXomlLoad::CreateInstance(IIwGameXomlResource* parent, CIwGameScene* scene, CIwGameString* template_name, CIwGameString* parameters)
{
	if (template_name == NULL)
		return false;

	// Get the template
	CIwGameTemplate* temp = (CIwGameTemplate*)CIwGameXomlResourceManager::FindResource(template_name->getHash(), CIwGameXomlNames::Template_Hash, parent);
	if (temp != NULL)
	{
		if (parameters != NULL && !parameters->IsEmpty())
		{
			// Create a set of XML attributes that will replace the template parameters
			CIwGameXmlNode* replacements = new CIwGameXmlNode();
			replacements->Managed = false;

			// Split parameter two into paramater=value pairs
			CIwGameSlotArray<CIwGameString*>* split = parameters->Split(':');
			CIwGameSlotArray<CIwGameString*>* strings = new CIwGameSlotArray<CIwGameString*>();
			for (int t = 0; t < split->getSize(); t++)
			{
				CIwGameString* pair = split->element_at(t);
				if (pair != NULL)
				{
					// Split paramater / value pair
					pair->Split('=', strings);
					CIwGameString* name = strings->element_at(0);
					CIwGameString* value = strings->element_at(1);

					if (name != NULL && value != NULL)
					{
						// Set template paramater
						CIwGameXmlAttribute* attrib = new CIwGameXmlAttribute();
						attrib->Managed = false;
						attrib->setName(name->c_str());
						attrib->setValue(value->c_str());
						replacements->AddAttribute(attrib);
					}
				}
				strings->clear(true);
			}
			temp->Instantiate(scene, replacements);

			delete replacements;
			delete strings;
			split->clear(true);
			delete split;
		}
		else
			temp->Instantiate(scene, NULL);

	}
#if defined (_DEBUG)
	else
	{
		CIwGameError::LogError("Warning: Actions - FromTemplate - Cannot find template - ", template_name->c_str());
		return false;
	}
#endif

	return true;
}
