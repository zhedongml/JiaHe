#ifndef MLVIMBACAMERA_EXPOSURE_AUTO_H_
#define MLVIMBACAMERA_EXPOSURE_AUTO_H_

#include <opencv2\opencv.hpp>


#include <stdlib.h>
#include<numeric>
#include <string>
#include <algorithm>
#include <functional>
#include "PrjCommon/service/ml.h"

using namespace cv;
using namespace CORE;

typedef Mat(*CaptureCallback)(void* instance, int& exposure_time, bool is_auto);


class MLExposureAuto
{
public:
    MLExposureAuto(float dynamic_range, double dark_level, int dark_time, int min_time, int max_time, MLPixelFormat format);
    ~MLExposureAuto();

    int MLAutoExposure(void* instance, CaptureCallback capture, int max_num = 5);
    double MaxNValue(Mat img, int n);
private:
    void* instance_;
    float dynamic_range_;
    double black_level_;
    int black_time_;
    int min_time_;
    int max_time_;
    MLPixelFormat format_;
    int max_value_;

    //Mean value of max first N pixels 


    // Max value of pixels based on the format of image
    int MaxCamValue(MLPixelFormat format);

    // Number of pixels higher than threhold_value
    int NumOver(Mat img, int threhold_value);

    double CalculateTime(int vtime, double value);

    int SingleAutoExposure(int initial_time,double max_value,Mat img, CaptureCallback capture, int num);


};

#endif // !MLVIMBACAMERA_EXPOSURE_AUTO_H_

