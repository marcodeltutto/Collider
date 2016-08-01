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

#if !defined(_IW_GAME_AUDIO_H_)
#define _IW_GAME_AUDIO_H_

#include "s3e.h"
#include "IwList.h"
#include "IwSound.h"
#include "IwGameUtil.h"
#include "IwGameString.h"
#include "IwGameXoml.h"
#include "IwGameFile.h"

class CIwGameAudio;
class CIwGameScene;

#define IW_GAME_AUDIO	CIwGameAudio::getInstance()

class CIwGameSoundCollection;

//
//
// CIwGameSound - A sound effect
//
//
class CIwGameSound : public IIwGameXomlResource
{
public:
	enum eCIwGameSound_State
	{
		State_Invalid, 
		State_Loaded, 
	};

	// Proprties
protected:
	eCIwGameSound_State		State;				// State of sound
	CIwResGroup*			ResourceGroup;		// Resource group that our image lives in
	CIwSoundInst*			SoundInstance;		// Sound instance
public:
	eCIwGameSound_State		getState() const					{ return State; }
	CIwSoundSpec*			getSoundSpec()						{ if (Load()) return SoundSpec; return NULL; }
	// Properties end

protected:
	CIwSoundData*			SoundData;			// Sound data
	CIwSoundSpec*			SoundSpec;			// Sound data
	CIwGameFile*			File;				// File object (if sound is file based)
public:

	CIwGameSound() : IIwGameXomlResource(), File(NULL), SoundSpec(NULL), SoundData(NULL), ResourceGroup(NULL), State(State_Invalid) { setClassType("sound"); }
	~CIwGameSound()
	{
		if (ResourceGroup == NULL)
		{
			SAFE_DELETE(SoundSpec);
			SAFE_DELETE(SoundData);
		}
		SAFE_DELETE(File)
	}

	void			Init(const char* ResourceName, CIwResGroup* resource_group)		// Init an sound from an soundspec located within a resource group
	{
		setName(ResourceName);
		ResourceGroup = resource_group;
	}

	bool			Init(void* memory_file, int memory_file_size);	// Init a sound from a memory based file (sound is loaded)
	void			Init(const char* filename)						// Init a sound from a file (sound is not loaded)
	{
		File = new CIwGameFile();
		File->setFilename(filename);
	}

	bool			Load(bool blocking = true);						// Force load the sound

	CIwSoundInst*	Play(float volume = 1.0f, float pitch = 1.0f, float pan = 0);

	// Implementation of IIwGameXomlResource interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	// Internal
	void			FinishLoad();									// Called back when aysnc loading is completed
};

//
//  CIwGameSoundCreator - Creates an instance of a sound object
//
class CIwGameSoundCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameSoundCreator()
	{
		setClassName("sound");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameSound(); }
};


//
//
//
// CIwGameAudio - The CAudio class is responsible for handling audio playback
//
//
//
class CIwGameAudio
{
	CDEFINE_SINGLETON(CIwGameAudio)

	// Properties
protected:
	static bool				Available;					// Determines if audio is supported
	bool					SoundOn;					// Sound on flag (will not play sound effects if off)
	bool					MusicOn;					// Musc flag (will not play music if off(
	float					SoundVolume;				// Volume of sound effects (1.0f is max)
	float					MusicVolume;				// Volume of music (1.0f is max)
	s3eAudioCodec			MusicCodec;					// Codec to use when playing music (default iis MP3)
public:
	bool					isSoundOn() const						{ return SoundOn; }
	void					setSoundOn(bool on)						{ SoundOn = on; }
	bool					isMusicOn() const						{ return MusicOn; }
	void					setMusicOn(bool on)						{ MusicOn = on; }
	bool					isMusicCodecSupported(s3eAudioCodec codec);
	bool					setMusicCodec(s3eAudioCodec codec)		{ if (isMusicCodecSupported(codec))  { MusicCodec = codec; return true; } return false; }
	void					setSoundVolume(float vol);
	float					getSoundVolume() const					{ return SoundVolume; }
	void					setMusicVolume(float vol);
	float					getMusicVolume() const					{ return MusicVolume; }
	// Properties end

protected:
public:
	void					Init();						// Initialises the audio system, called only once after the singleton is instanced
	void					Release();					// Releases data used by the audio system, called just before the audio system is torn down
	void					Update();					// Updates the audio system, called every frame

	// Sound effect handling
	CIwSoundInst*			PlaySound(const char* name, CIwGameScene* scene = NULL, float volume = 1.0f, float pitch = 1.0f, float pan = 0);
	CIwSoundInst*			PlaySound(unsigned int name_hash, CIwGameScene* scene = NULL, float volume = 1.0f, float pitch = 1.0f, float pan = 0);
	void					StopAllSounds();
	void					PauseAllSounds();
	void					ResumeAllSounds();

	// Music handling
	bool					PlayMusic(const char* name, int repeat_count = 0);						// Plays music from a local file
	bool					PlayMusic(void* buffer, uint32 buffer_length, uint32 repeat_count);		// Plays music from a memory buffer
	void					StopMusic();
	void					PauseMusic();
	void					ResumeMusic();
	bool					isMusicPlaying();

};




#endif	// _CAUDIO_H_