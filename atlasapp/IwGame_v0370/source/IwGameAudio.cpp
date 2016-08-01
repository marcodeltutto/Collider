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

#include "IwGameAudio.h"

//
//
//
//
// CIwGameSound implementation
//
//
//
//
void CIwGameSound::Preload()
{
	// Get the sound effect resource from the resource group
	if (SoundSpec == NULL)
		SoundSpec = (CIwSoundSpec*)Parent->getGroup()->GetResHashed(NameHash, IW_SOUND_RESTYPE_SPEC);
}
void CIwGameSound::Play(const CIwSoundParams* pParams)
{
	if (SoundSpec == NULL)
		Preload();
	if (SoundSpec != NULL)
		SoundInstance = SoundSpec->Play(pParams);
}

//
//
//
//
// CIwGameSoundCollection implementation
//
//
//
//
CIwGameSound* CIwGameSoundCollection::Play(unsigned int name_hash)
{
	// if sound is off then return and play nothing
	if (!IW_GAME_AUDIO->isSoundOn())
		return NULL;

	// Find the sound effect in the sound collection and play it if found
	CIwGameSound* sound = find(name_hash);
	if (sound != NULL)
	{
		sound->Play();
	}

	return sound;
}

//
//
//
//
// CIwGameSoundCollection implementation
//
//
//
//
void CIwGameSoundCollection::setGroup(CIwResGroup* group, bool populate)
{
	// Stop all sound effects
	IW_GAME_AUDIO->StopAllSounds();

	// Remove any previous sounds
	for (Iterator it = Sounds.begin(); it != Sounds.end(); it++)
		delete *it;
	Sounds.clear();

	// Assign the group
	Group = group;

	// Add all sound effect resources in the group
	if (populate)
	{
		CIwResList* list = group->GetListNamed(IW_SOUND_RESTYPE_SPEC);
		if (list != NULL)
		{
			int count = list->m_Resources.GetSize();
			for (int t = 0; t < count; t++)
			{
				CIwGameSound* sound = add(list->m_Resources[t]->m_Hash);
//				sound->Preload();
			}
		}
	}
}

void CIwGameSoundCollection::Release()
{
	// Stop all sound effects
	IW_GAME_AUDIO->StopAllSounds();

	// Cleanup sound effects
	for (Iterator it = Sounds.begin(); it != Sounds.end(); it++)
		delete *it;
	Sounds.clear();

/*	// Destroy the resource group
	if (Group != NULL)
	{
		IwGetResManager()->DestroyGroup(Group);
		Group = NULL;
	}*/
}

CIwGameSound* CIwGameSoundCollection::Play(const char* name)
{
	// Play the specified sound effect
	return Play(CIwGameString::CalculateHash(name));
}

void CIwGameSoundCollection::Stop(unsigned int name_hash)
{
	// Find the sound effect in the sound collection and play it if found
	CIwGameSound* sound = find(name_hash);

	// Stop the sound from playing
	if (sound != NULL)
		sound->Stop();
}

void CIwGameSoundCollection::Stop(const char* name)
{
	// Stop the sound from playing
	Stop(CIwGameString::CalculateHash(name));
}

//
//
//
//
// CIwGameAudio implementation
//
//
//
//
CDECLARE_SINGLETON(CIwGameAudio)

bool		CIwGameAudio::Available = false;

void CIwGameAudio::Init()
{
	// initialise IwSound
	IwSoundInit();

	// Turn music and sound on
	SoundOn = true;
	MusicOn = true;
	SoundVolume = 1.0f;
	MusicVolume = 1.0f;

	// Used to convert our WAV's located in the resource group files
#ifdef IW_BUILD_RESOURCES
	IwGetResManager()->AddHandler(new CIwResHandlerWAV);
#endif

	// Mark audio as available
	Available = true;
//	Available = false;

	// Set default music codec to MP3
	MusicCodec = S3E_AUDIO_CODEC_MP3;

	// Create a sound collection
	SoundCollection = new CIwGameSoundCollection();
}

void CIwGameAudio::Release()
{
	// Stop sound and music
	StopMusic();
	StopAllSounds();

	// Clean-up sound collection
	SAFE_DELETE(SoundCollection)

	// Cleanup IwSound
	IwSoundTerminate();
}

void CIwGameAudio::Update()
{
	if (!Available)
		return;

	// Update IwSound
	IwGetSoundManager()->Update();
}

CIwGameSound* CIwGameAudio::PlaySound(const char* name)
{
	// Play the named sound effect
	return PlaySound(CIwGameString::CalculateHash(name));
}

CIwGameSound* CIwGameAudio::PlaySound(unsigned int name_hash)
{
	if (!Available)
		return NULL;

	// Play the named sound effect
	return SoundCollection->Play(name_hash);
}

void CIwGameAudio::StopSound(const char* name)
{
	// Stop the named sound effect from playing
	StopSound(CIwGameString::CalculateHash(name));
}

void CIwGameAudio::StopSound(unsigned int name_hash)
{
	if (!Available)
		return;

	// Stop the named sound effect from playing
	SoundCollection->Stop(name_hash);
}
void CIwGameAudio::StopAllSounds()
{
	if (!Available)
		return;

	// Stop all audio
	IwGetSoundManager()->PauseAll();
	IwGetSoundManager()->StopAll();
}

void CIwGameAudio::PauseAllSounds()
{
	if (!Available)
		return;

	// Pause all sound effects
	IwGetSoundManager()->PauseAll();
}

void CIwGameAudio::ResumeAllSounds()
{
	if (!Available)
		return;

	// Resume all sound effects
	IwGetSoundManager()->ResumeAll();
}

bool CIwGameAudio::isMusicCodecSupported(s3eAudioCodec codec)
{
	return s3eAudioIsCodecSupported(codec) ? true : false;
}

bool CIwGameAudio::PlayMusic(const char* name, int repeat_count)
{
	if (!Available)
		return false;

	// Only play music is on
	if (!IW_GAME_AUDIO->isMusicOn())
		return false;

	// Use s3eAudio to play our music using the devices music player
	if (s3eAudioIsCodecSupported(S3E_AUDIO_CODEC_MP3))
	{
		if (s3eAudioPlay(name, repeat_count) != S3E_RESULT_SUCCESS)
			return false;
	}
	else
		return false;

	return true;
}

bool CIwGameAudio::PlayMusic(void* buffer, uint32 buffer_length, uint32 repeat_count)
{
	if (s3eAudioPlayFromBuffer(buffer, buffer_length, repeat_count) != S3E_RESULT_SUCCESS)
		return false;

	return true;
}

void CIwGameAudio::StopMusic()
{
	if (!Available)
		return;

	// Use s3eAudio to stop the devices music player from playing
	if (s3eAudioIsPlaying())
		s3eAudioStop();
}

void CIwGameAudio::PauseMusic()
{
	// Use s3eAudio to pause the devices music player
	s3eAudioPause();
}

void CIwGameAudio::ResumeMusic()
{
	// Use s3eAudio to resume the devices music player
	s3eAudioResume();
}

bool CIwGameAudio::isMusicPlaying()
{
	return s3eAudioIsPlaying() ? true : false;
}

void CIwGameAudio::setSoundVolume(float vol)
{
	SoundVolume = vol;
	s3eSoundSetInt(S3E_SOUND_VOLUME, (int)(SoundVolume * S3E_SOUND_MAX_VOLUME));
}

void CIwGameAudio::setMusicVolume(float vol)
{
	MusicVolume = vol;
	s3eAudioSetInt(S3E_AUDIO_VOLUME, (int)(MusicVolume * S3E_AUDIO_MAX_VOLUME));
}


