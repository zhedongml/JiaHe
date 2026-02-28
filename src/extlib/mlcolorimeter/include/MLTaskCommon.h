#pragma once
#include <QObject>
#include "MLColorimeterCommon.h"
#include "MLProcessorCommon.h"
using namespace ML::MLColorimeter;
using namespace ML::MLFilterWheel;
using namespace ML::CameraV2;

namespace ML_Task {

struct ExposureCheckParam {
    bool isExposureCheck = false;
    int topN = 1000;
    int threadCount = 4;
    int minGray = 0;
    double over_ratio = 0.9;
    double low_ratio = 0.6;
};

struct ImageCaptureConfig {
    std::string aperture;
    std::string lightSource;
    Binning binning;
    MLFilterEnum ndFilter;
    std::map<::MLFilterEnum, ExposureSetting> colorFilterToExposureMap;
    CalibrationFlag calibrationFlag;
    SaveDataMeta saveDataMeta;
    bool isSaveCali = false;
    bool isSaveRaw = false;
    bool is_undistort_method = false;
    HDRConfig hdrConfig;
    DUT_Type dutType = DUT_Type::Left;
    ExposureCheckParam exposureCheckParam;
};

struct ImageAlgoMetaData {
    int binning;
    std::string dutId;
    std::string eyeboxId;
    std::string lightSource;  //"R/G/B/W"
    std::string imageType;    //"Solid/Checkerboard/XHI/Flare/Ghost/Grid"
    std::string imageName;
    cv::Mat image;
};
Q_DECLARE_METATYPE(ImageAlgoMetaData)

}  // namespace ML_Task