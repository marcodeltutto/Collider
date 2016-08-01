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

#include "s3eOSExec.h"
#include "IwGameScriptLua.h"
#include "lua/IwGameLuaGen.h"
#include "IwGameActor.h"
#include "IwGameScene.h"
#include "IwGameProgram.h"
#include "IwGameUI.h"
#include "IwGameBrush.h"
#include "IwGame.h"
#include "IwGameTemplates.h"
#include "IwGameCam.h"

//
// LUA_PlaySound sound (object), volume (number), pitch (number), pan (number)
//
static int LUA_PlaySound(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: media.playSound() not enough parameters, expected sound (object) (volume, pitch, pan optional)");
		lua_pushnil(lua);
		return 1;
	}

	// Get the target object
	CIwGameSound* sound = NULL;
	if (lua_isuserdata(lua, 1))
		sound = (CIwGameSound*)lua_touserdata(lua, 1);
	if (sound == NULL || sound->getClassTypeHash() != CIwGameXomlNames::Sound_Hash)
	{
        CIwGameError::LogError("Warning: media.playSound() Invalid sound object for Param0");
		lua_pushnil(lua);
		return 1;
	}

	float volume = 1.0f;
	float pitch = 1.0f;
	float pan = 1.0f;

	if (count > 1)
	{
		if (lua_isnumber(lua, 2))
			volume = (float)lua_tonumber(lua, 2);
		else
		{
			CIwGameError::LogError("Warning: media.playSound() volume must be a number (Param1) - ", sound->getName().c_str());
			lua_pushnil(lua);
			return 1;
		}
	}
	if (count > 2)
	{
		if (lua_isnumber(lua, 3))
			pitch = (float)lua_tonumber(lua, 3);
		else
		{
			CIwGameError::LogError("Warning: media.playSound() pitch must be a number (Param2) - ", sound->getName().c_str());
			lua_pushnil(lua);
			return 1;
		}
	}
	if (count > 3)
	{
		if (lua_isnumber(lua, 4))
			pan = (float)lua_tonumber(lua, 4);
		else
		{
			CIwGameError::LogError("Warning: media.playSound() volume must be a number (Param3) - ", sound->getName().c_str());
			lua_pushnil(lua);
			return 1;
		}
	}

	CIwSoundInst* inst = sound->Play(volume, pitch, pan);

	lua_pushlightuserdata(lua, inst);
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_ChangeSound sound-instance (object), paramater-name (string), value (number)
//
static int LUA_ChangeSound(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 3)
	{
        CIwGameError::LogError("Warning: media.changeSound() not enough parameters, expected sound-instance (object) property (volume, pitch, pan), value (number)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	CIwSoundInst* sound_instance = NULL;
	if (lua_isuserdata(lua, 1))
		sound_instance = (CIwSoundInst*)lua_touserdata(lua, 1);
	if (sound_instance == NULL)
	{
        CIwGameError::LogError("Warning: media.changeSound()  invalid sound-instance for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get parameter to change
	const char* param = NULL;
	if (lua_isstring(lua, 2))
		param = lua_tostring(lua, 2);
	else
	{
		CIwGameError::LogError("Warning: media.changeSound() invalid sound parameter (Param1), expected property (string)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get value
	float value;
	if (lua_isnumber(lua, 3))
		value = (float)lua_tonumber(lua, 3);
	else
	{
		CIwGameError::LogError("Warning: media.changeSound() invalid sound parameter value (Param2), expected value (number)");
		lua_pushboolean(lua, false);
		return 1;
	}

	unsigned int param_hash = IW_GAME_HASH(param);
	if (param_hash == CIwGameXomlNames::Volume_Hash)
		sound_instance->SetVol((iwfixed)(value * IW_GEOM_ONE));
	else
	if (param_hash == CIwGameXomlNames::Pitch_Hash)
		sound_instance->SetPitch((iwfixed)(value * IW_GEOM_ONE));
	else
	if (param_hash == CIwGameXomlNames::Pan_Hash)
		sound_instance->SetPan((iwfixed)(value * IW_GEOM_ONE));
	else
	{
		CIwGameError::LogError("Warning: media.changeSound() invalid sound parameter instance (Param1) - ", param);
		lua_pushboolean(lua, false);
		return 1;
	}

	sound_instance->IsPlaying();

	lua_pushboolean(lua, true);
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_isSoundPlaying sound-instance (object)
//
static int LUA_isSoundPlaying(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: media.soundPlaying() not enough parameters, expected sound-instance (object)");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	CIwSoundInst* sound_instance = NULL;
	if (lua_isuserdata(lua, 1))
		sound_instance = (CIwSoundInst*)lua_touserdata(lua, 1);
	if (sound_instance == NULL)
	{
		lua_pushboolean(lua, false);
		return 1;
	}

	lua_pushboolean(lua, sound_instance->IsPlaying());
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_PlayMusic file-name (string)
//
static int LUA_PlayMusic(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: media.playMusic() not enough parameters, expected file-name (string), repeat-count (number, optional) ");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	const char* filename = NULL;
	if (lua_isstring(lua, 1))
		filename = lua_tostring(lua, 1);
	if (filename == NULL)
	{
        CIwGameError::LogError("Warning: media.playMusic() invalid music file-name for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	int repeat_count = 0;
	if (count > 1)
	{
		// Get the target object
		const char* filename = NULL;
		if (lua_isnumber(lua, 2))
			repeat_count = (int)lua_tonumber(lua, 2);
		else
		{
			CIwGameError::LogError("Warning: media.playMusic() invalid value for repeat-count in Param1");
			lua_pushboolean(lua, false);
			return 1;
		}
	}

	lua_pushboolean(lua, IW_GAME_AUDIO->PlayMusic(filename, repeat_count));
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

//
// LUA_StopMusic - Stops music from playing
//
static int LUA_StopMusic(lua_State *lua)
{
	int count = lua_gettop(lua);
	IW_GAME_AUDIO->StopMusic();

	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_PauseMusic - Pauses music
//
static int LUA_PauseMusic(lua_State *lua)
{
	int count = lua_gettop(lua);
	IW_GAME_AUDIO->PauseMusic();

	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_ResumeMusic - resume music
//
static int LUA_ResumeMusic(lua_State *lua)
{
	int count = lua_gettop(lua);
	IW_GAME_AUDIO->ResumeMusic();

	lua_pushboolean(lua, true);
    return 1;
}

//
// LUA_isMusicPlaying - Checks if music is currently playing
//
static int LUA_isMusicPlaying(lua_State *lua)
{
	int count = lua_gettop(lua);
	lua_pushboolean(lua, IW_GAME_AUDIO->isMusicPlaying());
    return 1;
}

//
// LUA_isAudioCodecSupported - Checks if music codec is supported
//
static int LUA_isAudioCodecSupported(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: media.audioCodecSupported() not enough parameters, expected codec (string) ");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	const char* codec = NULL;
	if (lua_isstring(lua, 1))
		codec = lua_tostring(lua, 1);
	if (codec == NULL)
	{
        CIwGameError::LogError("Warning: media.audioCodecSupported() invalid codec for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	unsigned int codec_hash = IW_GAME_HASH(codec);
	s3eAudioCodec cod;

	if (codec_hash == IW_GAME_HASH("midi"))
		cod = S3E_AUDIO_CODEC_MIDI;
	else
	if (codec_hash == IW_GAME_HASH("mp3"))
		cod = S3E_AUDIO_CODEC_MP3;
	else
	if (codec_hash == IW_GAME_HASH("aac"))
		cod = S3E_AUDIO_CODEC_AAC;
	else
	if (codec_hash == IW_GAME_HASH("aacplus"))
		cod = S3E_AUDIO_CODEC_AACPLUS;
	else
	if (codec_hash == IW_GAME_HASH("qcp"))
		cod = S3E_AUDIO_CODEC_QCP;
	else
	if (codec_hash == IW_GAME_HASH("pcm"))
		cod = S3E_AUDIO_CODEC_PCM;
	else
	if (codec_hash == IW_GAME_HASH("spf"))
		cod = S3E_AUDIO_CODEC_SPF;
	else
	if (codec_hash == IW_GAME_HASH("amr"))
		cod = S3E_AUDIO_CODEC_AMR;
	else
	if (codec_hash == IW_GAME_HASH("mp4"))
		cod = S3E_AUDIO_CODEC_MP4;
	else
	{
        CIwGameError::LogError("Warning: media.audioCodecSupported() invalid codec for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	lua_pushboolean(lua, IW_GAME_AUDIO->isMusicCodecSupported(cod));
    return 1;
}

//
// LUA_isVideoCodecSupported - Checks if video codec is supported
//
static int LUA_isVideoCodecSupported(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 1)
	{
        CIwGameError::LogError("Warning: media.videoCodecSupported() not enough parameters, expected codec (string) ");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	const char* codec = NULL;
	if (lua_isstring(lua, 1))
		codec = lua_tostring(lua, 1);
	if (codec == NULL)
	{
        CIwGameError::LogError("Warning: media.videoCodecSupported() invalid codec for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	unsigned int codec_hash = IW_GAME_HASH(codec);
	bool found = false;
	if (CIwGameVideoPlayer::isCodecSupported(codec_hash, found))
		lua_pushboolean(lua, true);
	else
	{
		if (!found)
			CIwGameError::LogError("Warning: media.videoCodecSupported() invalid codec for Param0");

		lua_pushboolean(lua, false);
	}

    return 1;
}

//
// LUA_ChangeVideoCam videocam-instance (object), command (string (start, stop))
//
static int LUA_ChangeVideoCam(lua_State *lua)
{
	int count = lua_gettop(lua);
	if (count < 2)
	{
        CIwGameError::LogError("Warning: media.changeVideoCam() not enough parameters, expected video-cam (object) command (string (start, stop))");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get the target object
	CIwGameVideoCam* cam = NULL;
	if (lua_isuserdata(lua, 1))
		cam = (CIwGameVideoCam*)lua_touserdata(lua, 1);
	if (cam == NULL || cam->getClassTypeHash() != CIwGameXomlNames::VideoCam_Hash)
	{
        CIwGameError::LogError("Warning: media.changeVideoCam()  invalid video-cam object for Param0");
		lua_pushboolean(lua, false);
		return 1;
	}

	// Get command
	const char* param = NULL;
	if (lua_isstring(lua, 2))
		param = lua_tostring(lua, 2);
	else
	{
		CIwGameError::LogError("Warning: media.changeVideoCam() invalid command (Param1)  ");
		lua_pushboolean(lua, false);
		return 1;
	}

	unsigned int param_hash = IW_GAME_HASH(param);
	if (param_hash == CIwGameXomlNames::Start_Hash)
		cam->Start();
	else
	if (param_hash == CIwGameXomlNames::Stop_Hash)
		cam->Stop();
	else
	{
		CIwGameError::LogError("Warning: media.changeVideoCam() invalid command (Param1) - ", param);
		lua_pushboolean(lua, false);
		return 1;
	}

	lua_pushboolean(lua, true);
 
    // Return the number of result arguments that were passed back to lua
    return 1;
}

static const luaL_Reg g_medialib[] =
{
	{"playSound",			LUA_PlaySound},
	{"changeSound",			LUA_ChangeSound},
	{"soundPlaying",		LUA_isSoundPlaying},
	{"playMusic",			LUA_PlayMusic},
	{"stopMusic",			LUA_StopMusic},
	{"pauseMusic",			LUA_PauseMusic},
	{"resumeMusic",			LUA_ResumeMusic},
	{"musicPlaying",		LUA_isMusicPlaying},
	{"audioCodecSupported", LUA_isAudioCodecSupported},
	{"videoCodecSupported", LUA_isVideoCodecSupported},
	{"changeVideoCam",		LUA_ChangeVideoCam},
	{NULL, NULL}
};

LUALIB_API int luaopen_media(lua_State *L)
{
  luaL_register(L, "media", g_medialib);
  
  return 1;
}
