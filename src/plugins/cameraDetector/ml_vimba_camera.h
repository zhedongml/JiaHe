#ifndef MLVIMBACAMERA_H_
#define MLVIMBACAMERA_H_

#include <opencv2/opencv.hpp>
#include <VimbaCPP/Include/VimbaCPP.h>
#include <Windows.h>
#include <Mutex>
#include <thread>
#include "PrjCommon/service/ml_camera.h"
#include "ml_camera_system.h"
#include "LogPlus.h"
#include "eliminatehotpixel.h"

#define VIMBACAMERA_API __declspec(dllexport)
using namespace cv;
using namespace AVT::VmbAPI;
using namespace CORE;

static Mat CaptureImage(void* instance, int& exposure_time, bool is_auto);

class VIMBACAMERA_API MLVimbaCamera : public MLCamera
{
    Q_OBJECT
public:
    MLVimbaCamera();
    MLVimbaCamera(int index, bool fix_pixel);
    ~MLVimbaCamera();

public:
    MLResult Open();

    MLResult OpenBySN(const char* SN);

    MLCameraInfo GetCameraInfo();

    bool IsOpened();

    virtual MLResult GrabOne();

    MLResult StartGrabbingAsync();

    MLResult StopGrabbing();

    bool IsGrabbing();

    int GetWidth();

    int GetHeight();

    void SetFrameRate(double rate);

    double GetFrameRate();

    void SetFormatType(MLPixelFormat format);

    MLPixelFormat GetFormatType();

    void SetExposureMode(MLExposureMode mode);

    MLResult SetMLExposureAuto();

    //  set exposure time of the camera, when the exposure mode is auto, this method does
    //  not have any effect on exposure time, need to set exposure mode Off first
    MLResult SetExposureTime(double time);

    double GetExposureTime();

    void GetExposureTimeRange(double& min, double& max);

    //  Set the exposure time to be an integer multiple of the AR glasses screen refresh
    //  rate��the camera can capture the complete image data in units of a AR glass refresh rate
    MLResult SetAdjustExposureTime(double time);

    MLCameraState GetState();

    void FlipX(bool flip);

    void FlipY(bool flip);

    void Subscribe(MLCameraEvent event, MLCameraCallback* callback);

    void Unsubscribe(MLCameraEvent event, MLCameraCallback* callback);

    void Close();

    Mat GetImage();

    void StartCalcGrayLevel();

    void StopCalcGrayLevel();

    void Rotate(bool rotate);

    void SetFocusSyn(double vid) {};

    void SetFocusAsy(double vid) {};

    void AutoFocus() {};

    void StopFocus() {};
    void SetBinning(Binning binning, BinningMode binningMode);
    Binning getBinning();
    inline
        Mat SourceImage2CV(FramePtr frame_ptr)
    {
        VmbErrorType res;

        cv::Mat org;

        VmbUchar_t* buffer_ptr;
        VmbUint32_t size;
        res = frame_ptr->GetImage(buffer_ptr);
        res = frame_ptr->GetImageSize(size);

        switch (format_type_)
        {
        case MLPixelFormat::kMLMono8:
            org = cv::Mat(GetHeight(), GetWidth(), CV_8UC1, buffer_ptr);				//put the image date in a mat matrix
            break;
        case MLPixelFormat::kMLMono12:
        case MLPixelFormat::kMLMono16:
            org = cv::Mat(GetHeight(), GetWidth(), CV_16UC1, buffer_ptr);
            break;
        case MLPixelFormat::kMLRGB24:
            org = cv::Mat(GetHeight(), GetWidth(), CV_8UC3, buffer_ptr);
            break;
        default:
            break;
        }
        if (!org.empty())
        {
            m_mutex.lock();

            int rotate = flip_rotate_ % 10;

            switch (rotate)
            {
            case 1:
                break;
            case 2:
                transpose(org, org);
                flip(org, org, 1);
                break;
            case 3:
                flip(org, org, 0);
                flip(org, org, 1);
                break;
            case 4:
                transpose(org, org);
                flip(org, org, 0);
                break;
            default:
                break;
            }
            if (pixel_fix_)
            {
                org = EliminateHotPixel::getInstance()->deHotAndDeadPixel(org, index_);
            }

            if (is_grabbing_)
            {
                img_ = org.clone();
            }
            else
            {
                one_img_ = org.clone();

            }
            m_mutex.unlock();
        }



        return org;
    }

    inline
        void CVImage2MLFrame(Mat& img)
    {
        MLFrame ml_frame;
        ml_frame.format = MLPixelFormat::kMLMono8;
        ml_frame.width = GetWidth();
        ml_frame.height = GetHeight();
        //ml_frame.buffer_ptr = (void*)img.data;
        camera_callback_->NotifyCameraFrameReceived(&ml_frame, (void*)img.data);
    }

private:
    int AdjustTime(double init_time, int rate);

    void InitializeConfigVariables();

    void ThreadFun();

public:

    MLCameraCallback* camera_callback_;

    Mat img_;

    CameraPtr camera_ptr_;

    int exposure_time_;

    int flip_rotate_;

private:

    double black_level_;

    int black_time_;

    float dynamic_range_;

    int max_exposure_time_;

    bool is_open_;

    MLCameraInfo   camera_info_;

    bool is_grabbing_;

    int width_;

    int height_;

    double frame_rate_;

    MLPixelFormat format_type_;

    MLCameraState camera_state_;

    bool state_callback_;

    bool grabbing_callback_;

    bool graylevel_callback_;

    IFrameObserverPtr  frame_observer_ptr_;

    HANDLE m_hThread;

    std::mutex m_mutex;

    Mat one_img_;

    bool m_bThread;

    std::thread m_Thread_;
};

#endif // !MLVIMBACAMERA_H_