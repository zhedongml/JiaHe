#ifndef CORE_MLCAMERA_H_
#define CORE_MLCAMERA_H_

#include "MLFrame.h"
#include  <QObject>
#include <opencv2/core.hpp>
#include "ml.h"
#include "../prjcommon_global.h"

namespace CORE
{
class MLCameraCallback
{
  public:
    virtual void NotifyCameraStateChanged(MLCameraState old_state, MLCameraState new_state) = 0;

    virtual void NotifyCameraFrameReceived(MLFrame *frame_ptr, void *buffer_ptr) = 0;

    virtual void NotifyCameraGrayLevel(int gray_level) = 0;

    virtual void NotifyAutoFocus(MLStatus status, double progress, int *steparray, double *distance, double *data,
                                 int len) = 0;
};

class PRJCOMMON_EXPORT MLCamera : public QObject
{
    Q_OBJECT
  public:
    MLCamera(){}
    virtual ~MLCamera(){}

  public:
    virtual MLResult Open() = 0;

    virtual MLResult OpenBySN(const char *SN) = 0;

    virtual void Close() = 0;

    virtual bool IsOpened() = 0;

    virtual MLCameraInfo GetCameraInfo() = 0;

    virtual MLResult GrabOne() = 0;

    virtual MLResult StartGrabbingAsync() = 0;

    virtual MLResult StopGrabbing() = 0;

    virtual bool IsGrabbing() = 0;

    virtual int GetWidth() = 0;

    virtual int GetHeight() = 0;

    virtual void SetFrameRate(double rate) = 0;

    virtual double GetFrameRate() = 0;

    virtual void SetFormatType(MLPixelFormat format) = 0;

    virtual MLPixelFormat GetFormatType() = 0;

    virtual MLResult SetMLExposureAuto() = 0;

    virtual void SetExposureMode(MLExposureMode mode) = 0;

    //  set exposure time of the camera, when the exposure mode is auto, this method does
    //  not have any effect on exposure time, need to set exposure mode Off first
    virtual MLResult SetExposureTime(double time) = 0;

    virtual double GetExposureTime() = 0;

    virtual void GetExposureTimeRange(double &min, double &max) = 0;

    //  Set the exposure time to be an integer multiple of the AR glasses screen refresh
    //  rate��the camera can capture the complete image data in units of a AR glass refresh rate
    virtual MLResult SetAdjustExposureTime(double time) = 0;

    virtual MLCameraState GetState() = 0;

    virtual void FlipX(bool flip) = 0;

    virtual void FlipY(bool flip) = 0;

    virtual void Subscribe(MLCameraEvent event, MLCameraCallback *callback) = 0;

    virtual void Unsubscribe(MLCameraEvent event, MLCameraCallback *callback) = 0;

    virtual cv::Mat GetImage() = 0;

    virtual void StartCalcGrayLevel() = 0;

    virtual void StopCalcGrayLevel() = 0;

    virtual void Rotate(bool rotate) = 0;

    // motor control
    virtual void SetFocusSyn(double vid) = 0;

    virtual void SetFocusAsy(double vid) = 0;

    virtual void AutoFocus() = 0;

    virtual void StopFocus() = 0;
    virtual void SetBinning(Binning binning, BinningMode binningMode) =0;
    virtual float CoarseThruFocus(float expectFocus = -1){return 0;}
    virtual Binning getBinning()=0;
    virtual void recipeStop(bool isStop){};
    virtual void SetBinning(Binning binning){};
    virtual void SetBinningMode(BinningMode binningMode){};
    virtual BinningMode GetBinningMode()
    {
        return SUM;
    }
    virtual float AutoFocusRough(int min = 0, int max = 0)
    {
        return 0;
    };
  public:
    bool disconnect_;

    std::string serial_number_;

    int index_;

    bool pixel_fix_;

    bool m_isImageUpdate;

    double initExposureTime;

    std::string m_AEendStatus;

    AutoExposureStatus AEStatus_ = AE_NONE;

    std::string cameraName_;

    int init_Time_ = 11111;

    bool isSetInitTime = true;

    std::string autoExposureConfigPath;
};
}

#endif // !COMMON_MLCAMER_H_
