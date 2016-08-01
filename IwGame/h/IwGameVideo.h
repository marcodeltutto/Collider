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

#if !defined(_IW_GAME_VIDEO_H_)
#define _IW_GAME_VIDEO_H_

#include "s3e.h"
#include "IwList.h"
#include "IwSound.h"
#include "IwGameUtil.h"
#include "IwGameXoml.h"

class CIwGameVideo;

#define IW_GAME_VIDEO	CIwGameVideoPlayer::getInstance()

//
//
//
//
// CIwGameVideo - A video file
//
//
//
//
class CIwGameVideo : public IIwGameXomlResource
{
public:
	enum eCIwGameVideo_State
	{
		State_Invalid, 
		State_Loaded, 
	};

	// Proprties
private:
	eCIwGameVideo_State		State;				// State of video
	CIwGameString			Filename;			// Name of video file
	int						RepeatCount;		// Total times to repeat play
	CIwRect					Rect;				// Position and size of video on screen
	s3eVideoCodec			Codec;				// Codec to use
	float					Volume;				// Volume of audio
public:
	eCIwGameVideo_State		getState() const					{ return State; }
	CIwGameString&			getFilename()						{ return Filename; }
	void					setRepeatCount(int count)			{ RepeatCount = count; }
	int						getRepeatCuont() const				{ return RepeatCount; }
	CIwRect					getRect() const						{ return Rect; }
	void					setRect(int x, int y, int w, int h)	{ Rect.x = x; Rect.y = y; Rect.w = w; Rect.h = h; }
	void					setRect(CIwRect &rect)				{ Rect = rect; }
	s3eVideoCodec			getCodec() const					{ return Codec; }
	void					setVolume(float volume)				{ Volume = volume; }
	float					getVolume() const					{ return Volume; }
	// Properties end

protected:
	char*					VideoData;			// Raw video data
	uint32					VideoDataLen;		// Length of raw video data
	CIwGameFile*			File;				// File object (if video if file based)
public:

	CIwGameVideo() : IIwGameXomlResource(), File(NULL), State(State_Invalid), VideoData(NULL), VideoDataLen(0), Rect(0, 0, 0, 0), Codec(S3E_VIDEO_CODEC_MPEG4), Volume(1.0f)	{ setClassType("video"); }
	~CIwGameVideo();

	bool		Init(const char* filename, bool preload, bool blocking, s3eVideoCodec codec);
	bool		Init(char* video_data, uint32 video_data_len, s3eVideoCodec codec);
	bool		Play();
	bool		Load(bool blocking = true);						// Force load the video file

	// Implementation of IIwGameXomlResource interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	// Internal
	void			FinishLoad();									// Called back when aysnc loading is completed
};
//
//  CIwGameVideoCreator - Creates an instance of a video object
//
class CIwGameVideoCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameVideoCreator()
	{
		setClassName("video");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameVideo(); }
};


//
//
//
//
// CIwGameVideoPlayer - Class responsible for playing back video
//
//
//
//
class CIwGameVideoPlayer
{
	CDEFINE_SINGLETON(CIwGameVideoPlayer)

	// Properties
protected:
public:
	int						getPlayPosition() const;
	bool					isPlaying() const;
	void					setVolume(float volume);
	float					getVolume() const;
	// Properties end

protected:
public:
	void					Init();											// Initialises the bvideo playback system, called only once after the singleton is instanced
	void					Release();										// Releases data used by the video system, called just before the audio system is torn down

	bool					Play(const char* filename, CIwRect& rect, int repeat_count, s3eVideoCodec codec);
	bool					PlayFromMemory(char* mem, uint32 len, CIwRect& rect, int repeat_count, s3eVideoCodec codec);
	void					Stop();
	void					Pause();
	void					Resume();

	static bool				isCodecSupported(s3eVideoCodec codec);
	static bool				isCodecSupported(unsigned int codec_hash, bool &found);
};




#endif	// _IW_GAME_VIDEO_H_