#include <memory.h>

#include "IwGameCam.h"
#include "IwGameActor.h"


//
//
// IwGameCam implementation
//
//
CDECLARE_SINGLETON(CIwGameCam)

static int32 CIwGameCam_CameraUpdate(void* systemData, void* userData)
{
    if (!IW_GAME_CAM->isStarted())
        return 0;

    s3eCameraFrameData* frame = (s3eCameraFrameData*)systemData;

	if (IW_GAME_CAM->getImageBrush() == NULL)
	{
		// Create image to hold the camera data
		CIwGameImage* image = new CIwGameImage();
		IW_GAME_CAM->setImageBrush(image);
		IW_GAME_CAM->setImageCreated(true);
	}

	if (!IW_GAME_CAM->isImageValid())
	{
		CIwGameImage* image = IW_GAME_CAM->getImageBrush();
		if (s3eCameraIsFormatSupported(S3E_CAMERA_PIXEL_TYPE_RGB565) || s3eCameraIsFormatSupported(S3E_CAMERA_PIXEL_TYPE_RGB565_CONVERTED))
			image->Init((uint8*)frame->m_Data, frame->m_Width, frame->m_Height, frame->m_Pitch, CIwImage::RGB_565, true);
		else
		if (s3eCameraIsFormatSupported(S3E_CAMERA_PIXEL_TYPE_RGB888))
			image->Init((uint8*)frame->m_Data, frame->m_Width, frame->m_Height, frame->m_Pitch, CIwImage::RGB_888, true);

		IW_GAME_CAM->setImageValid(true);
	}

    // Copy the camera image data into the texture. Note that it does not get copied to VRAM at this point.
	if (IW_GAME_CAM->isImageValid())
	{
		CIwTexture* texture = IW_GAME_CAM->getImageBrush()->getTexture();
		if (texture != NULL)
		{
			texture->ChangeTexels((uint8*)frame->m_Data, texture->GetFormat());
			IW_GAME_CAM->setUpdated(true);
		}
	}

	int angle = 0;
	switch (frame->m_Rotation)
	{
	case S3E_CAMERA_FRAME_ROT90:
		angle = 90;
		break;
	case S3E_CAMERA_FRAME_ROT180:
		angle = 180;
		break;
	case S3E_CAMERA_FRAME_ROT270:
		angle = 270;
		break;
	}
	if (IW_GAME_CAM->getCameraType() == CIwGameCam::CCT_Front)
		angle = -angle;
	IW_GAME_CAM->setLastFrameOrientation(angle);

    return 0;
}

void CIwGameCam::Init()
{
	Started = false;
	Updated = false;
	ImageValid = false;
	ImageCreated = false;
	ImageBrush = NULL;
	LastFrameOrientation = 0;
}

bool CIwGameCam::Start(eCameraCaptureSize size_hint, eCameraCaptureQuality quality)
{
	if (isAvailable() && !Started)
	{
		// if cam already started then stop it
		if (Started)
			Stop();

		if (ImageBrush != NULL)
		{
			// Release previous image data so image can be re-initialised
			IW_GAME_CAM->setImageValid(false);
			ImageBrush->Release();
		}

		if (s3eCameraRegister(S3E_CAMERA_UPDATE_STREAMING, CIwGameCam_CameraUpdate, 0) != S3E_RESULT_ERROR)
		{
			if (s3eCameraStart((s3eCameraStreamingSizeHint)size_hint, S3E_CAMERA_PIXEL_TYPE_RGB565_CONVERTED, (s3eCameraQualityHint)quality) == S3E_RESULT_SUCCESS)
				Started = true;
			else
			{
				s3eCameraUnRegister(S3E_CAMERA_UPDATE_STREAMING, CIwGameCam_CameraUpdate);
				return false;
			}
		}
		else
			return false;
	}

	return true;
}

void CIwGameCam::Stop()
{
	if (Started)
	{
		s3eCameraStop();
		s3eCameraUnRegister(S3E_CAMERA_UPDATE_STREAMING, CIwGameCam_CameraUpdate);
		Started = false;
	}

}

bool CIwGameVideoCam::InstanceCreated = false;

bool CIwGameVideoCam::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (InstanceCreated)
	{
		CIwGameError::LogError("Warning: Cannot create more than one instance of VideoCam per app session");
		IW_GAME_XOML->setExitOnError(false);
		return true;
	}

	CIwGameString* quality = NULL;
	CIwGameString* resolution = NULL;
	CIwGameString* target = NULL;
	CIwGameString* start = NULL;
	CIwGameString* direction = NULL;

	// Parse attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
			setName((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Target_Hash)
			target = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Quality_Hash)
			quality = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Resolution_Hash)
			resolution = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Start_Hash)
			start = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::Direction_Hash)
			direction = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::OnError_Hash)
			EventsManager->addEvent("OnError", (*it)->GetValue().c_str(), true);
	}

	CIwGameCam::Create();
	IW_GAME_CAM->Init();
	InstanceCreated = true;

	// If we are declared inside a scene then we are local to the scene
	CIwGameScene* scene = NULL;
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		scene = ((CIwGameActor*)parent)->getScene();
	else
	if (parent != NULL && parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	// Find the target image
	if (target != NULL)
	{
		CIwGameImage* image = (CIwGameImage*)CIwGameXomlResourceManager::FindResource(target->getHash(), CIwGameXomlNames::Image_Hash, scene);
		if (image == NULL)
			CIwGameError::LogError("Warning: XOML - VideoCam - Could not find target image - ", target->c_str());
		IW_GAME_CAM->setImageBrush(image);
	}
	else
		CIwGameError::LogError("Warning: XOML - VideoCam - No target image specified");

	Quality = CIwGameCam::CCQ_Medium;
	Resolution = CIwGameCam::CCS_Medium;

	if (quality != NULL)
	{
		unsigned int hash = quality->getHash();
		if (hash == IW_GAME_HASH("low"))
			Quality = CIwGameCam::CCQ_Low;
		else
		if (hash == IW_GAME_HASH("high"))
			Quality = CIwGameCam::CCQ_High;
	}

	if (resolution != NULL)
	{
		unsigned int hash = resolution->getHash();
		if (hash == IW_GAME_HASH("low"))
			Resolution = CIwGameCam::CCS_Small;
		else
		if (hash == IW_GAME_HASH("high"))
			Resolution = CIwGameCam::CCS_Large;
	}

	if (direction != NULL)
	{
		if (direction->getHash() == IW_GAME_HASH("front"))
			setFrontFacing();
		else
			setRearFacing();
	}

	if (start != NULL && start->GetAsBool())
	{
		if (!IW_GAME_CAM->Start(Resolution, Quality))
			ProcessEventActions(IW_GAME_HASH("OnError"), parent);
	}

	if (scene != NULL)
		return scene->getResourceManager()->addResource(this);
	else
		return IW_GAME_GLOBAL_RESOURCES->getResourceManager()->addResource(this);

	return true;
}

void CIwGameVideoCam::Stop()
{
	if (IW_GAME_CAM != NULL)
		IW_GAME_CAM->Stop();
}

bool CIwGameVideoCam::Start()
{
	if (IW_GAME_CAM == NULL)
		return false;

	if (!CIwGameCam::isAvailable())
	{
		CIwGameError::LogError("Warning: This platform does not support VideoCam");
		ProcessEventActions(IW_GAME_HASH("OnError"), Parent->getParent());
		return false;
	}
	else
	{
		if (CameraType == CIwGameCam::CCT_Front)
		{
			if (!IW_GAME_CAM->setFrontFacing())
			{
				ProcessEventActions(IW_GAME_HASH("OnError"), Parent->getParent());
				return false;
			}
		}
/*		else
		{
			if (!IW_GAME_CAM->setRearFacing())
			{
				ProcessEventActions(IW_GAME_HASH("OnError"), Parent->getParent());
				return false;
			}
		}*/
		if (!IW_GAME_CAM->Start(Resolution, Quality))
		{
			ProcessEventActions(IW_GAME_HASH("OnError"), Parent->getParent());
			return false;
		}
	}

	return true;
}

void CIwGameVideoCam::ProcessEventActions(unsigned int event_name, IIwGameXomlResource* parent)
{
	// Find the event
	CIwGameEvent* evt = EventsManager->findEvent(event_name);
	if (evt != NULL)
	{
		// Find actions list
		CIwGameActions* actions = CIwGameActionsManager::FindActions(evt->ActionsName, parent);
		if (actions != NULL)
			actions->Execute(parent);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Error: Action not found - ", evt->_ActionsName.c_str());
#endif	//_DEBUG
	}
}
