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

#if !defined(_IW_GAME_CAM_H_)
#define _IW_GAME_CAM_H_

//
// Classes in this file
//
//	CIwGameCam				- Device camera wrapper
//  CIwGameVideoCam			- A class that is used to control the video camera
//  CIwGameVideoCamCreator	- Creates an instance of a VideoCam
//

#include "s3eCamera.h"
#include "IwTexture.h"

#include "IwGameUtil.h"
#include "IwGameImage.h"
#include "IwGameActions.h"

#define	IW_GAME_CAM	CIwGameCam::getInstance()

//
//
//  CIwGameCam - Device Camera wrapper
//
//
class CIwGameCam
{
public:
	enum eCameraCaptureSize
	{
		CCS_Small = S3E_CAMERA_STREAMING_SIZE_HINT_SMALLEST,	// Approx 1/4 screen size
		CCS_Medium = S3E_CAMERA_STREAMING_SIZE_HINT_MEDIUM,		// Approx 1/2 screen size
		CCS_Large = S3E_CAMERA_STREAMING_SIZE_HINT_LARGEST,		// Approx full screen size
	};

	enum eCameraCaptureQuality
	{
		CCQ_Low =  S3E_CAMERA_STREAMING_QUALITY_HINT_LOW, 
		CCQ_Medium = S3E_CAMERA_STREAMING_QUALITY_HINT_MED, 
		CCQ_High = S3E_CAMERA_STREAMING_QUALITY_HINT_HIGH, 
	};

	enum eCameraType
	{
		CCT_Rear =  S3E_CAMERA_TYPE_REAR, 
		CCT_Front = S3E_CAMERA_TYPE_FRONT, 
	};

	enum eCameraStatus
	{
		CS_Idle = S3E_CAMERA_IDLE,				// Camera is idle
		CS_Streaming = S3E_CAMERA_STREAMING,    // Camera is streaming
		CS_Failed = S3E_CAMERA_FAILED,			// Error occurred during capture.
		CS_Suspended = S3E_CAMERA_SUSPENDED,    // Camera is paused due to device suspend
		CS_Restarting = S3E_CAMERA_RESTARTING   // Camera is being restarted after a device suspend
	};

	CDEFINE_SINGLETON(CIwGameCam)

public:
	// Properties
private:
	bool				Started;
	eCameraCaptureSize	SizeHint;
	eCameraCaptureQuality Quality;
	bool				ImageValid;
	bool				ImageCreated;
	bool				Updated;
	CIwGameImage*		ImageBrush;
	int					LastFrameOrientation;
	eCameraType			CameraType;
public:
	static bool			isAvailable() { return s3eCameraAvailable() != 0; }
	bool				isStarted() const { return Started; }
	void				setUpdated(bool updated) { Updated = updated; }
	bool				isUpdated() const { return Updated; }
	void				setImageValid(bool valid) { ImageValid = valid; }
	bool				isImageValid() const { return ImageValid; }
	void				setImageCreated(bool valid) { ImageCreated = valid; }
	bool				isImageCreated() const { return ImageCreated; }
	eCameraCaptureSize	getSizeHint() const { return SizeHint; }
	eCameraCaptureQuality getQuality() const { return Quality; }

	void				setImageBrush(CIwGameImage* brush)
	{
		if (ImageBrush != NULL)
			delete ImageBrush;
		ImageBrush = brush;
	}
	CIwGameImage*		getImageBrush() { return ImageBrush; }
	eCameraStatus		getStatus()
	{
		if (!isAvailable() || !Started)
			return CS_Failed;
		return (eCameraStatus)s3eCameraGetInt(S3E_CAMERA_STATUS);
	}
	bool				setFrontFacing()
	{
		CameraType = CCT_Front;
		if (CIwGameUtils::GetDeviceType() == CIwGameUtils::DeviceType_Windows)
			return true;
		else
			return s3eCameraSetInt(S3E_CAMERA_TYPE, S3E_CAMERA_TYPE_FRONT) == S3E_RESULT_SUCCESS;
	}
	bool				setRearFacing()
	{
		CameraType = CCT_Rear;
		if (CIwGameUtils::GetDeviceType() == CIwGameUtils::DeviceType_Windows)
			return true;
		else
			return s3eCameraSetInt(S3E_CAMERA_TYPE, S3E_CAMERA_TYPE_REAR) == S3E_RESULT_SUCCESS;
	}
	int					getLastFrameOrientation() const	{ return LastFrameOrientation; }
	void				setLastFrameOrientation(int angle)	{ LastFrameOrientation = angle; }
	eCameraType			getCameraType() const	{ return CameraType; }
	// Properties end

private:

public:
	void	Init();
	void	Release()
	{
		Stop();
		if (ImageCreated)
			SAFE_DELETE(ImageBrush);
	}

	bool	Start(eCameraCaptureSize size_hint, eCameraCaptureQuality quality = CCQ_Medium);

	void	Stop();

};

//
//
//
//
//  CIwGameVideoCam - A class that is used to control the video camera
//
//
//
//
class CIwGameVideoCam : public IIwGameXomlResource
{
public:
	// Properties
protected:
	static bool		InstanceCreated;				// Used to ensure that only noe instance can be created
	CIwGameCam::eCameraCaptureQuality	Quality;
	CIwGameCam::eCameraCaptureSize		Resolution;
	CIwGameCam::eCameraType				CameraType;
	CIwGameEventManager*				EventsManager;			// List of events that the actor handles
public:
	static bool							getInstanceCreated()			{ return InstanceCreated; }
	CIwGameCam::eCameraCaptureQuality	getQuality() const				{ return Quality; }
	void								setQuality(CIwGameCam::eCameraCaptureQuality quality)		{ Quality = quality; }
	void								setResolution(CIwGameCam::eCameraCaptureSize resolution)	{ Resolution = resolution; }
	CIwGameCam::eCameraCaptureSize		getResolution() const			{ return Resolution; }
	void								setFrontFacing() 				{ CameraType = CIwGameCam::CCT_Front; }
	void								setRearFacing()					{ CameraType = CIwGameCam::CCT_Rear;  }
	CIwGameCam::eCameraType				getCameraType() const			{ return CameraType; }
	CIwGameEventManager*				getEventsManager()				{ return EventsManager; }
	// Properties end

public:
	CIwGameVideoCam() : IIwGameXomlResource(), Quality(CIwGameCam::CCQ_Medium), Resolution(CIwGameCam::CCS_Medium), CameraType(CIwGameCam::CCT_Rear)
	{
		setClassType("videocam");
		EventsManager = new CIwGameEventManager();
	}
	virtual ~CIwGameVideoCam()
	{
		if (IW_GAME_CAM != NULL)
		{
			IW_GAME_CAM->Release();
			CIwGameCam::Destroy();
		}
		SAFE_DELETE(EventsManager);
	}

	// Implementation of IIwGameXomlClass interface
	bool				LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	void				Stop();
	bool				Start();

	// Event handlers
	virtual void		ProcessEventActions(unsigned int event_name, IIwGameXomlResource* parent);
};

//
//
//
//
//  CIwGameVideoCamCreator - Creates an instance of a VideoCam
//
//
//
//
class CIwGameVideoCamCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameVideoCamCreator()
	{
		setClassName("videocam");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameVideoCam(); }
};




#endif // _IW_GAME_CAM_H_
