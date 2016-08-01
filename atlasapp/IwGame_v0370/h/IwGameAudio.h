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

#if !defined(_IW_GAME_AUDIO_H_)
#define _IW_GAME_AUDIO_H_

#include "s3e.h"
#include "IwList.h"
#include "IwSound.h"
#include "IwGameUtil.h"
#include "IwGameString.h"

class CIwGameAudio;

#define IW_GAME_AUDIO	CIwGameAudio::getInstance()

class CIwGameSoundCollection;

//
//
// CIwGameSound - A sound effect
//
//
class CIwGameSound
{
	// Proprties
private:
	CIwGameSoundCollection*	Parent;					// Parent sound collection
	unsigned int			NameHash;				// Name hash of this sound effect (as defined in the resource group)
public:
	void					setParent(CIwGameSoundCollection* parent)	{ Parent = parent; }
	void					setName(unsigned int name_hash)				{ NameHash = name_hash; }
	unsigned int			getName()									{ return NameHash; }
	// Properties end

protected:
	CIwSoundInst*			SoundInstance;
	CIwSoundSpec*			SoundSpec;
public:

	CIwGameSound() : SoundSpec(NULL), Parent(NULL), SoundInstance(NULL), NameHash(0) {}
	~CIwGameSound()
	{
	}

	void		Preload();
	void		Play(const CIwSoundParams* pParams = NULL);
	void		Stop()
	{
		if (SoundSpec != NULL)
		{
			IwGetSoundManager()->StopSoundSpec(SoundSpec);
			SoundInstance = NULL;
		}
	}
    void		SetVolume(float vol)
	{
		if (SoundInstance != NULL)
			SoundInstance->SetVol((iwsfixed)(vol * IW_GEOM_ONE));
	}
    void		SetPitch(float pitch)
	{
		if (SoundInstance != NULL)
			SoundInstance->SetPitch((iwsfixed)(pitch * IW_GEOM_ONE));
	}
    void		SetPan(float pan)
	{
		if (SoundInstance != NULL)
			SoundInstance->SetPan((iwsfixed)(pan * IW_GEOM_ONE));
	}
	bool		isPlaying() const
	{
		if (SoundInstance == NULL)
			return true;
		return SoundInstance->IsPlaying();
	}
};

//
//
// CIwGameSoundCollection - A collection of sound effects
//
//
class CIwGameSoundCollection
{
public:
	// Provide public access to iteration of the sound collection
	typedef CIwList<CIwGameSound*>::iterator Iterator;
	Iterator				begin() { return Sounds.begin(); }
	Iterator				end()	{ return Sounds.end(); }

	// Properties
private:
	CIwGameString			Name;							// Name of sound collection
	CIwResGroup*			Group;							// Resource group that contains our sound effects
public:
	void					setName(const char* name)		{ Name.setString(name); }
	CIwGameString&			getName()						{ return Name; }
	void					setGroup(CIwResGroup* group, bool populate = true);
	CIwResGroup*			getGroup()						{ return Group; }
	// Properties end

protected:
	CIwList<CIwGameSound*>	Sounds;

public:
	CIwGameSoundCollection() : Group(NULL) {}
	~CIwGameSoundCollection()
	{
		Release();
	}
	CIwGameSound*	add(unsigned int name_hash)
	{
		// Create and add the named sound effect to the collection
		CIwGameSound* sound = new CIwGameSound();
		sound->setName(name_hash);
		sound->setParent(this);
		Sounds.push_back(sound);

		return sound;
	}
	CIwGameSound*	find(unsigned int name_hash)
	{
		// Find and return eth named sound effect
		for (Iterator it = Sounds.begin(); it != Sounds.end(); it++)
		{
			if ((*it)->getName() == name_hash)
				return *it;
		}

		return NULL;
	}
	CIwGameSound*	find(const char *name)
	{
		return find(CIwGameString::CalculateHash(name));
	}
	void			Release();

	CIwGameSound*	Play(unsigned int name_hash);
	CIwGameSound*	Play(const char* name);
	void			Stop(unsigned int name_hash);
	void			Stop(const char* name);
};


//
//
// CIwGameAudio - The CAudio class is responsible for handling audio playback
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
	CIwGameSoundCollection*	SoundCollection;			// Collection of sound effects
	s3eAudioCodec			MusicCodec;					// Codec to use when playing music (default iis MP3)
public:
	bool					isSoundOn() const						{ return SoundOn; }
	void					setSoundOn(bool on)						{ SoundOn = on; }
	bool					isMusicOn() const						{ return MusicOn; }
	void					setMusicOn(bool on)						{ MusicOn = on; }
	CIwGameSound*			addSound(const char* name);				// Add a sound effect to the sound collection
	CIwGameSoundCollection*	getSoundCollection()					{ return SoundCollection; }
	bool					isMusicCodecSupported(s3eAudioCodec codec);
	bool					setMusicCodec(s3eAudioCodec codec)		{ if (isMusicCodecSupported(codec))  { MusicCodec = codec; return true; } return false; }
	void					setSoundVolume(float vol);
	float					getSoundVolume() const					{ return SoundVolume; }
	void					setMusicVolume(float vol);
	float					getMusicVolume() const					{ return MusicVolume; }
	// Properties end

protected:
public:
	void					Init();											// Initialises the audio system, called only once after the singleton is instanced
	void					Release();										// Releases data used by the audio system, called just before the audio system is torn down
	void					Update();										// Updates the audio system, called every frame
	void					setGroup(CIwResGroup* group, bool populate = true)	// Sets the Marmalade resource group that contains the sound effects
	{
		SoundCollection->setGroup(group, populate);
	}

	// Sound effect handling
	CIwGameSound*			PlaySound(const char* name);
	CIwGameSound*			PlaySound(unsigned int name_hash);
	void					StopSound(const char* name);
	void					StopSound(unsigned int name_hash);
	void					StopAllSounds();
	void					PauseAllSounds();
	void					ResumeAllSounds();

	// Music handling
	bool					PlayMusic(const char* name, int repeat_count = 0);						// Plays music from a local file
	bool					PlayMusic(void* buffer, uint32 buffer_length, uint32 repeat_count);	// Plays music from a memory buffer
	void					StopMusic();
	void					PauseMusic();
	void					ResumeMusic();
	bool					isMusicPlaying();

};




#endif	// _CAUDIO_H_