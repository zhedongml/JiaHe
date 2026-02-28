#pragma once
#include <random>
#include "MLColorimeterCommon.h"
using namespace ML::MLColorimeter;
using namespace ML::MLFilterWheel;
using namespace ML::CameraV2;

using CalibrationData =
    std::map<ML::MLColorimeter::CalibrationEnum,
             std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>>;


enum class DUT_Type { Left, Right };

enum class ProcessType {
    Dark,
    FFC,
    ColorShift,
    Distortion,
    Exposure,
    FourColor,
    Luminance,
    Keystone,
    FOVCrop,
    Flip_Rotate,
};

/** @brief config to save calibration data*/
struct SaveDataMeta {
    std::string uid;
    std::string DutID;
    std::string EyeboxID;
    std::string ImageType;  // reticle type
    std::string SavePath;   // path to save calibration data
    std::string ImageNameRule;
    cv::Rect SaveROI = cv::Rect();
    bool Is_Convert = false;
    int ConvertMethod = 0;
};

struct ImageNameRuleInfo {
    std::string imageNameRule = "";
    std::string customName = "";
};

struct ProcessorData {
    CalibrationData caliData;
    SaveDataMeta saveDM;
    DUT_Type dutType;
    bool isSaveCali;
    bool is_undistort_method = false;
};

struct HDRConfig {
    bool isHDR = false;
    double maxExposure = 5000;
};


class ProcessorHelper {
 public:
    static ProcessorHelper& getInstance() {
        static ProcessorHelper instance;
        return instance;
    }

    std::string getUid() const {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);  // 0-F

        std::stringstream ss;
        for (size_t i = 0; i < 6; ++i) {
            ss << std::hex << std::nouppercase << dis(gen);
        }
        return ss.str();
    }

    std::string transProcessTypeToStr(ProcessType type){
        std::string str;
        switch (type) {
            case ProcessType::Dark:
                str = "Dark";
                break;
            case ProcessType::FFC:
                str = "FFC";
                break;
            case ProcessType::ColorShift:
                str = "ColorShift";
                break;
            case ProcessType::Distortion:
                str = "Distortion";
                break;
            case ProcessType::Exposure:
                str = "Exposure";
                break;
            case ProcessType::FourColor:
                str = "FourColor";
                break;
            case ProcessType::Luminance:
                str = "Luminance";
                break;
            case ProcessType::FOVCrop:
                str = "FOVCrop";
                break;
            case ProcessType::Flip_Rotate:
                str = "Flip_Rotate";
                break;
            default:
                str = "Unkown type";
                break;
        }
        return str;
    };

 private:
    ProcessorHelper() = default;
    ~ProcessorHelper() = default;
    ProcessorHelper(const ProcessorHelper&) = delete;
    ProcessorHelper& operator=(const ProcessorHelper&) = delete;
};