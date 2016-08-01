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

#include "IwGameAudio.h"
#include "IwGameScene.h"
#include "IwGameActor.h"

//
//
//
//
// CIwGameSound implementation
//
//
//
//

bool isLittleEndian()
{
	int test = 1;
	return (*((char*)&test) == 1);
}

CIwSoundInst* CIwGameSound::Play(float volume, float pitch, float pan)
{
	if (SoundSpec == NULL)
		Load();
	if (SoundSpec == NULL)
		return NULL;

	CIwSoundParams params;
	params.m_Vol = (iwfixed)(volume * IW_GEOM_ONE);
	params.m_Pan = (iwfixed)(pan * IW_GEOM_ONE);
	params.m_Pitch = (iwfixed)(pitch * IW_GEOM_ONE);

	return SoundSpec->Play(&params);
}

int32 CIwGameSound_FileRetrievedAsyncCallback(void* caller, void* data)
{
	CIwGameSound* sound = (CIwGameSound*)data;
	sound->FinishLoad();

	return 0;
}

bool CIwGameSound::Load(bool blocking)
{
	// If already loaded return true
	if (SoundSpec != NULL)
		return true;

	// Image is file based
	if (File != NULL)
	{
		File->setFileAvailableCallback(CIwGameSound_FileRetrievedAsyncCallback, this);
		if (File->Open(NULL, "rb", blocking))
			return true;
		else
			return false;
	}

	if (ResourceGroup != NULL)
	{
		// Create image from the resource group
		IwGetResManager()->SetCurrentGroup(ResourceGroup);
		SoundSpec = (CIwSoundSpec*)ResourceGroup->GetResHashed(NameHash, IW_SOUND_RESTYPE_SPEC);
		State = State_Loaded;
		return true;
	}

	return false;
}

void CIwGameSound::FinishLoad()
{
	if (File != NULL)
	{
		if (CIwGameFile::isHttp(File->getFilename().c_str(), File->getFilename().GetLength()))
		{
			if (File->isFileAvailable() && File->getError() == CIwGameFile::ErrorNone)
				Init(File->getContent(), File->getContentLength());
		}
		else
		{
			char* data = new char [File->getFileSize()];
			File->Read(data, File->getFileSize());
			Init(data, File->getFileSize());
			delete data;
		}
		State = State_Loaded;
		SAFE_DELETE(File)
	}
}

bool CIwGameSound::Init(void* memory_file, int memory_file_size)
{
	CIwGameFile file;

	SoundData = CIwSoundWAV::Create("in_memory", memory_file, memory_file_size);
	if (SoundData == NULL)
	{
		CIwGameError::LogError("Error: CIwGameSound::Init() - Could not create sound!");
		return false;
	}
	SoundSpec = new CIwSoundSpec();
	SoundSpec->SetData(SoundData);

	State = State_Loaded;

	return true;
}

bool CIwGameSound::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	// Process image attributes
	CIwGameString* name = NULL;
	CIwGameString* location = NULL;
	bool preload = true;
	bool blocking = false;
	CIwGameString* condition = NULL;

	CIwGameScene* scene = NULL;
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		scene = ((CIwGameActor*)parent)->getScene();
	else
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
		{
			name = &(*it)->GetValue();
			setName(name->c_str());
		}
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Location_Hash)
			location = &(*it)->GetValue();
		else
 		if (name_hash == CIwGameXomlNames::Preload_Hash)
			preload = (*it)->GetValueAsBool();
		else
 		if (name_hash == CIwGameXomlNames::Blocking_Hash)
			blocking = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::Condition_Hash)
			condition = &(*it)->GetValue();
	}

	if (location == NULL || name == NULL)
	{
		CIwGameError::LogError("Error: A sound requires a location and a name to be specified");
		return false;
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

	// Check to see if image is located externally
	if (CIwGameFile::isHttp(location->c_str(), location->GetLength()))
	{
		Init(location->c_str());
	}
	else
	if (CIwGameFile::isFile(location->c_str(), location->GetLength()))
	{
		Init(location->c_str());
	}
	else
	{
		// Find resuorce group
		CIwGameResourceGroup* group = NULL;
		if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
			group = (CIwGameResourceGroup*)scene->getResourceManager()->findResource(location->c_str(), CIwGameXomlNames::ResourceGroup_Hash);
		else
			group = (CIwGameResourceGroup*)IW_GAME_GLOBAL_RESOURCES->getResourceManager()->findResource(location->c_str(), CIwGameXomlNames::ResourceGroup_Hash);

		if (group != NULL)
			Init(name->c_str(), group->getResourceGroup());
		else
		{
			CIwGameError::LogError("Error: XOML - Invalid sound resource group name - ", location->c_str());
			return false;
		}
	}
	if (preload)
	{
		Load(blocking);
	}

	// If we are declared inside a scene then image is local to the scene
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
}

void CIwGameAudio::Release()
{
	// Stop sound and music
	StopMusic();
	StopAllSounds();

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

CIwSoundInst* CIwGameAudio::PlaySound(const char* name, CIwGameScene* scene, float volume, float pitch, float pan)
{
	// Play the named sound effect
	return PlaySound(CIwGameString::CalculateHash(name), scene, volume, pitch, pan);
}

CIwSoundInst* CIwGameAudio::PlaySound(unsigned int name_hash, CIwGameScene* scene, float volume, float pitch, float pan)
{
	if (!Available)
		return NULL;

	// find the sound effect
	CIwGameSound* sound = (CIwGameSound*)CIwGameXomlResourceManager::FindResource(name_hash, CIwGameXomlNames::Sound_Hash, scene);
	if (sound == NULL)
		return NULL;
	
	return sound->Play(volume, pitch, pan);
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


