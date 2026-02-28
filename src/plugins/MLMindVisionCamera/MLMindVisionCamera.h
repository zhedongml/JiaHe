#pragma once

#include "ml_camera.h"
#include "CameraApi.h"
#include <QObject>

#include "mlmindvisioncamera_global.h"

using namespace cv;
using namespace CORE;

class MLMINDVISIONCAMERA_EXPORT MLMindVisionCamera : public CORE::MLCamera
{
    Q_OBJECT
public:
    MLMindVisionCamera();
    ~MLMindVisionCamera();

public:
    virtual MLResult Open();

    virtual MLResult OpenBySN(const char* SN);

    virtual void Close();

    virtual bool IsOpened();

    virtual MLCameraInfo GetCameraInfo();

    virtual MLResult GrabOne();

    virtual MLResult StartGrabbingAsync();

    virtual MLResult StopGrabbing();

    virtual bool IsGrabbing();

    virtual int GetWidth();

    virtual int GetHeight();

    virtual void SetFrameRate(double rate);

    virtual double GetFrameRate();

    virtual void SetFormatType(MLPixelFormat format);

    virtual MLPixelFormat GetFormatType();

    virtual MLResult SetMLExposureAuto();

    virtual void SetExposureMode(MLExposureMode mode);

    //  set exposure time of the camera, when the exposure mode is auto, this method does
    //  not have any effect on exposure time, need to set exposure mode Off first
    virtual MLResult SetExposureTime(double time);

    virtual double GetExposureTime();

    virtual void GetExposureTimeRange(double& min, double& max);

    //  Set the exposure time to be an integer multiple of the AR glasses screen refresh
    //  rate??the camera can capture the complete image data in units of a AR glass refresh rate
    virtual MLResult SetAdjustExposureTime(double time);

    virtual MLCameraState GetState();

    virtual void FlipX(bool flip);

    virtual void FlipY(bool flip);

    virtual void Subscribe(CORE::MLCameraEvent event, CORE::MLCameraCallback* callback);

    virtual void Unsubscribe(CORE::MLCameraEvent event, CORE::MLCameraCallback* callback);

    virtual cv::Mat GetImage();

    virtual void StartCalcGrayLevel();

    virtual void StopCalcGrayLevel();

    virtual void Rotate(bool rotate);

    virtual void SetCameraShift(ML_Point left, ML_Point right);

    virtual void SetCameraShiftType(CameraShiftType);

    virtual void SetBinningMode(BinningMode binningMode);

    virtual BinningMode GetBinningMode();

    virtual void SetBandwidth(int bandwidth);

    virtual void SetBinning(Binning binning);

    virtual Binning GetBinning();

    virtual void SetBinning(Binning binning, BinningMode binningMode);

    virtual void SetFocusSyn(double vid);

    virtual void SetFocusAsy(double vid);

    virtual void AutoFocus();

    virtual void StopFocus();

    virtual Binning getBinning();

private:
    //SDK
    int m_hCamera = -1;     // device handle
    unsigned char* m_pRawBuffer = nullptr;     // raw data
    unsigned char* m_pRgbBuffer = nullptr;     // Post processing data buffer
    tSdkFrameHead m_tFrameHead;       // Image frame header information
    tSdkCameraCapbility m_tCapability;      // Device Description Information

    //unsigned  char* m_cUserImagePtr = nullptr;
    int m_nWidth;
    int m_nHeight;
    bool m_bIsCameraOpen;

    MLCameraCallback* camera_callback_ = nullptr;
};
