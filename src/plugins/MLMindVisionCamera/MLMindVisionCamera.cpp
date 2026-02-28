#include "MLMindVisionCamera.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


MLMindVisionCamera::MLMindVisionCamera()
{
    m_bIsCameraOpen = false;
}

MLMindVisionCamera::~MLMindVisionCamera()
{
    Unsubscribe(MLCameraEvent::kFrameReceived, nullptr);
    Close();
    //SAFE_DELETE_ARRAY(m_pRawBuffer);
    //SAFE_DELETE_ARRAY(m_pRgbBuffer);
    //SAFE_DELETE_ARRAY(m_cUserImagePtr);
}

MLResult MLMindVisionCamera::Open()
{
    int                     iCameraCounts = 4;
    int                     iStatus = -1;
    tSdkCameraDevInfo       tCameraEnumList[4];

    // sdk initialization  0 English 1 chinese
    CameraSdkStatus status = CameraSdkInit(0);

    // Enumerate devices and establish a device list
    CameraEnumerateDevice(tCameraEnumList, &iCameraCounts);

    // No device connected
    if (iCameraCounts == 0)
    {
        m_bIsCameraOpen = false;
        return MLResult{ MLStatus::ML_STATUS_CAMERA_ERROR,"" };
    }

    // Camera initialization. After successful initialization, any other camera related operation interfaces can be called
    iStatus = CameraInit(&tCameraEnumList[0], -1, -1, &m_hCamera);

    //initialization failed
    if (iStatus != CAMERA_STATUS_SUCCESS)
    {
        m_bIsCameraOpen = false;
        return MLResult{ MLStatus::ML_STATUS_CAMERA_UNOPEN,"" };
    }
    /*
    // Obtain the characteristic description structure of the camera.
    // This structure contains range information for various parameters that can be set by the camera.
    // Determines the parameters of the correlation function
    */
    CameraGetCapability(m_hCamera, &m_tCapability);

    m_pRgbBuffer = (unsigned char*)malloc(m_tCapability.sResolutionRange.iHeightMax * m_tCapability.sResolutionRange.iWidthMax * 3);
    //g_readBuf = (unsigned char*)malloc(m_tCapability.sResolutionRange.iHeightMax * m_tCapability.sResolutionRange.iWidthMax * 3);

    /*
    //Put the SDK into working modeand start receiving image data sent from the camera.
    //If the current camera is in trigger mode, the image will not be updated until the trigger frame is received.
    */
    CameraPlay(m_hCamera);


    // Set the output format for image processing. Both color black and white support RGB 24 bits
    if (m_tCapability.sIspCapacity.bMonoSensor) {
        CameraSetIspOutFormat(m_hCamera, CAMERA_MEDIA_TYPE_MONO8);
    }
    else {
        CameraSetIspOutFormat(m_hCamera, CAMERA_MEDIA_TYPE_RGB8);
    }

    CameraSetTriggerMode(m_hCamera, 0);
    CameraSetStrobeMode(m_hCamera, 0);

    int isOpen;

    iStatus = CameraIsOpened(&tCameraEnumList[0], &isOpen);

    if (iStatus != CAMERA_STATUS_SUCCESS)
    {
        m_bIsCameraOpen = false;
        return MLResult{ MLStatus::ML_STATUS_CAMERA_UNOPEN,"" };
    }
    m_bIsCameraOpen = true;
    return MLResult{ MLStatus::ML_STATUS_CAMERA_OPENED,"" };
}

MLResult MLMindVisionCamera::OpenBySN(const char* SN)
{
    if (m_bIsCameraOpen) return MLResult{ 1, "" };

    int cameraIndex = atoi(SN);
    int                     iCameraCounts = 4;
    int                     iStatus = -1;
    tSdkCameraDevInfo       tCameraEnumList[4];

    // sdk initialization  0 English 1 chinese
    CameraSdkStatus status = CameraSdkInit(0);

    // Enumerate devices and establish a device list
    CameraEnumerateDevice(tCameraEnumList, &iCameraCounts);

    // No device connected
    if (iCameraCounts == 0)
    {
        m_bIsCameraOpen = false;
        return MLResult{ 0,"MindVisionCamera connection failed!" };
    }

    // Camera initialization. After successful initialization, any other camera related operation interfaces can be called
    iStatus = CameraInit(&tCameraEnumList[cameraIndex], -1, -1, &m_hCamera);

    //initialization failed
    if (iStatus != CAMERA_STATUS_SUCCESS)
    {
        m_bIsCameraOpen = false;
        return MLResult{ 0,"MindVisionCamera connection failed!" };
    }
    /*
    // Obtain the characteristic description structure of the camera.
    // This structure contains range information for various parameters that can be set by the camera.
    // Determines the parameters of the correlation function
    */
    CameraGetCapability(m_hCamera, &m_tCapability);

    m_pRgbBuffer = (unsigned char*)malloc(m_tCapability.sResolutionRange.iHeightMax * m_tCapability.sResolutionRange.iWidthMax * 3);
    //g_readBuf = (unsigned char*)malloc(m_tCapability.sResolutionRange.iHeightMax * m_tCapability.sResolutionRange.iWidthMax * 3);

    /*
    //Put the SDK into working modeand start receiving image data sent from the camera.
    //If the current camera is in trigger mode, the image will not be updated until the trigger frame is received.
    */
    CameraPlay(m_hCamera);


    // Set the output format for image processing. Both color black and white support RGB 24 bits
    if (m_tCapability.sIspCapacity.bMonoSensor) {
        CameraSetIspOutFormat(m_hCamera, CAMERA_MEDIA_TYPE_MONO8);
    }
    else {
        CameraSetIspOutFormat(m_hCamera, CAMERA_MEDIA_TYPE_RGB8);
    }

    CameraSetTriggerMode(m_hCamera, 0);
    CameraSetStrobeMode(m_hCamera, 0);

    int isOpen;

    iStatus = CameraIsOpened(&tCameraEnumList[0], &isOpen);

    if (iStatus != CAMERA_STATUS_SUCCESS)
    {
        m_bIsCameraOpen = false;
        return MLResult{ 0,"MindVisionCamera connection failed!" };
    }
    m_bIsCameraOpen = true;
    return MLResult{ 1,"" };
}

void MLMindVisionCamera::Close()
{
    if (m_bIsCameraOpen)
    {
        if (m_pRgbBuffer != NULL)
        {
            free(m_pRgbBuffer);
            m_pRgbBuffer = NULL;
        }
        if (m_hCamera > 0)
        {
            CameraStop(m_hCamera);
            // Camera deinitialization. Release resources.
            CameraUnInit(m_hCamera);
            m_hCamera = -1;
            m_bIsCameraOpen = false;
        }
        else
        {
            m_bIsCameraOpen = true;
        }
    }
}

bool MLMindVisionCamera::IsOpened()
{
    return m_bIsCameraOpen;
}

MLCameraInfo MLMindVisionCamera::GetCameraInfo()
{
    return MLCameraInfo();
}

MLResult MLMindVisionCamera::GrabOne()
{
    if (m_bIsCameraOpen)
    {
        CameraSdkStatus sdkStatus;
        int wTimes = 10;

        ////CameraSetTriggerMode(m_hCamera, 0);
        //m_cUserImagePtr = CameraGetImageBufferEx(m_hCamera, &m_nWidth, &m_nHeight, wTimes);

        CameraSetTriggerMode(m_hCamera, 0);
        if (CameraGetImageBuffer(m_hCamera, &m_tFrameHead, &m_pRawBuffer, 2000) == CAMERA_STATUS_SUCCESS)
        {
            CameraImageProcess(m_hCamera, m_pRawBuffer, m_pRgbBuffer, &m_tFrameHead);
            CameraReleaseImageBuffer(m_hCamera, m_pRawBuffer);

            return MLResult{ MLStatus::ML_STATUS_OK,"" };
        }
        else
            return MLResult{ MLStatus::ML_STATUS_CAMERA_ERROR,"" };
    }
    else
        return MLResult{ MLStatus::ML_STATUS_MOTION_UNOPEN,"" };

}

MLResult MLMindVisionCamera::StartGrabbingAsync()
{
    return MLResult{ 1,"" };
}

MLResult MLMindVisionCamera::StopGrabbing()
{
    return MLResult{ 1,"" };
}

bool MLMindVisionCamera::IsGrabbing()
{
    return false;
}

int MLMindVisionCamera::GetWidth()
{
    return 0;
}

int MLMindVisionCamera::GetHeight()
{
    return 0;
}

void MLMindVisionCamera::SetFrameRate(double rate)
{
}

double MLMindVisionCamera::GetFrameRate()
{
    return 0.0;
}

void MLMindVisionCamera::SetFormatType(MLPixelFormat format)
{
}

MLPixelFormat MLMindVisionCamera::GetFormatType()
{
    return MLPixelFormat();
}

MLResult MLMindVisionCamera::SetMLExposureAuto()
{
    //bool FlickEnable = false;
    int FlickEnable = 0;
    int piFrequencySel;

    // Obtain the enabled state of the anti strobe function during automatic exposure.
    CameraGetAntiFlick(m_hCamera, &FlickEnable);
    // When obtaining automatic exposure, select the frequency of strobe suppression.
    CameraGetLightFrequency(m_hCamera, &piFrequencySel);

    CameraSetAeState(m_hCamera, true);

    // Sets the enabled state of the anti strobe function during automatic exposure. Invalid for manual exposure mode.
    CameraSetAntiFlick(m_hCamera, true);

    if (piFrequencySel) {
        // Sets the frequency of strobe suppression during automatic exposure. 0:50HZ , 1:60HZ
        CameraSetLightFrequency(m_hCamera, piFrequencySel);
    }

    return MLResult{ 1,"" };
}

void MLMindVisionCamera::SetExposureMode(MLExposureMode mode)
{
}

MLResult MLMindVisionCamera::SetExposureTime(double us)
{
	if (!m_bIsCameraOpen)
	{
		return MLResult{ MLStatus::ML_STATUS_MOTION_UNOPEN,"Mind Vision camera set exposure " + std::to_string(us) + "us error, camera is not connect!" };
	}
	if (CameraSetExposureTime(m_hCamera, us) == CAMERA_STATUS_SUCCESS)
		return MLResult{ 1,"" };
	return MLResult{ 0,"Mind Vision camera set exposure " + std::to_string(us) + "us error!" };
}

double MLMindVisionCamera::GetExposureTime()
{
    if (m_bIsCameraOpen)
    {
        double exposure;
        CameraGetExposureTime(m_hCamera, &exposure);
        return (exposure / 1000);
    }
    else
        return 0.0;
}

void MLMindVisionCamera::GetExposureTimeRange(double& min, double& max)
{
}

MLResult MLMindVisionCamera::SetAdjustExposureTime(double ms)
{
    if (m_bIsCameraOpen)
    {
        double m_fExpLineTime = 0; // Current row exposure time, in us

        CameraSetAeState(m_hCamera, false);

        /*
        // Obtain an exposure time for one line.
        // For CMOS sensors, the exposure unit is calculated in rows, so the exposure time cannot be continuously adjustable at the microsecond level.
        // Instead, they will choose based on the entire line. The function returns the time corresponding to the exposure line of the CMOS camera.
        */
        CameraGetExposureLineTime(m_hCamera, &m_fExpLineTime);

        SetExposureTime(ms * 1000);

        return MLResult{ 1,"" };
    }
    else
    {
        return MLResult{ MLStatus::ML_STATUS_MOTION_UNOPEN,"Mind Vision camera set adjust exposure time is not connect!" };
    }
}

MLCameraState MLMindVisionCamera::GetState()
{
    return MLCameraState();
}

void MLMindVisionCamera::FlipX(bool flip)
{
}

void MLMindVisionCamera::FlipY(bool flip)
{
}

void MLMindVisionCamera::Subscribe(CORE::MLCameraEvent event, CORE::MLCameraCallback* callback)
{
    if (camera_callback_ == nullptr || callback == camera_callback_)
    {
        if (camera_callback_ == nullptr)
        {
            camera_callback_ = callback;
        }
    }
}

void MLMindVisionCamera::Unsubscribe(CORE::MLCameraEvent event, CORE::MLCameraCallback* callback)
{
    if (camera_callback_ != nullptr)
    {
        camera_callback_ = NULL;
    }
}

cv::Mat MLMindVisionCamera::GetImage()
{
    if (!m_bIsCameraOpen)
        return cv::Mat();

    if (!m_pRgbBuffer)
        return cv::Mat();

    cv::Mat imageNew = cv::Mat(m_tFrameHead.iHeight, m_tFrameHead.iWidth, CV_8UC1, m_pRgbBuffer).clone();

    //cv::Mat imageNew = cv::Mat(m_nHeight, m_nWidth, CV_8UC1, m_cUserImagePtr).clone();

    if (camera_callback_ != nullptr && !imageNew.empty())
    {
        MLFrame ml_frame;
        ml_frame.format = MLPixelFormat::kMLMono8;
        ml_frame.width = imageNew.cols;
        ml_frame.height = imageNew.rows;

        camera_callback_->NotifyCameraFrameReceived(&ml_frame, (void*)imageNew.data);
    }

    return imageNew;
}

void MLMindVisionCamera::StartCalcGrayLevel()
{
}

void MLMindVisionCamera::StopCalcGrayLevel()
{
}

void MLMindVisionCamera::Rotate(bool rotate)
{
}

void MLMindVisionCamera::SetCameraShift(ML_Point left, ML_Point right)
{
}

void MLMindVisionCamera::SetCameraShiftType(CameraShiftType)
{
}

void MLMindVisionCamera::SetBinningMode(BinningMode binningMode) {
}

BinningMode MLMindVisionCamera::GetBinningMode()
{
    return BinningMode();
}

void MLMindVisionCamera::SetBandwidth(int bandwidth)
{
}

void MLMindVisionCamera::SetBinning(Binning binning)
{
}

Binning MLMindVisionCamera::GetBinning()
{
    return Binning();
}

void MLMindVisionCamera::SetBinning(Binning binning, BinningMode binningMode)
{
}

void MLMindVisionCamera::SetFocusSyn(double vid)
{

}

void MLMindVisionCamera::SetFocusAsy(double vid)
{

}

void MLMindVisionCamera::AutoFocus()
{

}

void MLMindVisionCamera::StopFocus()
{

}

Binning MLMindVisionCamera::getBinning()
{
    return Binning();
}

