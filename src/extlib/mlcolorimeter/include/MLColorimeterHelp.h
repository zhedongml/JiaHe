// Copyright 2025 MLOptic

#ifndef SRC_MLCOLORIMETERCOMMON_MLCOLORIMETERHELP_H_
#define SRC_MLCOLORIMETERCOMMON_MLCOLORIMETERHELP_H_

#include <ctime>
#include <fstream>
#include <map>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

#include "MLColorimeterCommon.h"
#include "json.hpp"
#include "mlcolorimetercommon_global.h"

namespace ML {
namespace MLColorimeter {
using json = nlohmann::ordered_json;

class MLCOLORIMETERCOMMON_EXPORT MLColorimeterHelp {
 public:
    static MLColorimeterHelp* instance();

    ~MLColorimeterHelp();

    cv::Mat ReadJsonArrayToMat(const char* filename, std::string objectName);

    cv::Mat ReadOffsetJson(const char* filename);

    ML::MLColorimeter::FOVCrop ReadJsonToFOVCrop(const char* filename, std::string objectName);

    bool ReadFlip_RotateJson(const char* filename, std::string objectName);

    bool WriteFlip_RotateToJson(const char* filename, std::string objectName,
                                ML::MLColorimeter::Flipping flipx, ML::MLColorimeter::Flipping flipy,
                                ML::MLColorimeter::Rotation rotation);

    ML::MLColorimeter::Flipping GetFlippingX();

    ML::MLColorimeter::Flipping GetFlippingY();

    ML::MLColorimeter::Rotation GetRotation();

    cv::Mat Flip(cv::Mat srcImg, ML::MLColorimeter::Flipping flip);

    cv::Mat Rotation(cv::Mat srcImg, ML::MLColorimeter::Rotation);

    bool WriteMatToJsonFile(const char* filename, std::string objectName, cv::Mat mat);

    ML::MLColorimeter::RXCombination RXMapping(ML::MLColorimeter::RXCombination rx,
                                               ML::MLColorimeter::RXMappingRule rxRule,
                                               std::string session = "");

    std::string TransRXToStr(ML::MLColorimeter::RXCombination rx);

    std::string TransRXToStr2(ML::MLColorimeter::RXCombination rx);

    std::string TransSynRXToStr(ML::MLColorimeter::RXCombination rx);

    ML::MLColorimeter::RXCombination TransStrToRX(std::string str);

    ML::MLFilterWheel::MLFilterEnum TransStrToFilterEnum(std::string str);

    std::string TransFilterEnumToStr(ML::MLFilterWheel::MLFilterEnum fenum);

    std::string TransCaliEnumToStr(ML::MLColorimeter::CalibrationEnum caliEnum);

    std::string TransBinningSelectorToStr(ML::CameraV2::BinningSelector selector);

    std::string TransBinningToStr(ML::CameraV2::Binning binning);

    std::string TransBinningModeToStr(ML::CameraV2::BinningMode binningmode);

    std::string TransMLPixelFormatToStr(ML::CameraV2::MLPixelFormat format);

    std::string TransTemperatureSelectorToStr(ML::CameraV2::TemperatureSelector selector);

    std::string TransMLEleStatusToStr(ML::MLEleReflectorCommon::MLEleStatus status);

    std::string TransMLFilterStatusToStr(ML::MLFilterWheel::MLFilterStatus status);

    int TransBinningToInt(ML::CameraV2::Binning binning);

    nlohmann::ordered_json TransMatToJson(cv::Mat mat);

    std::string TransFlippingToStr(ML::MLColorimeter::Flipping flip);

    std::string TransRotationToStr(ML::MLColorimeter::Rotation rotation);

    ML::MLColorimeter::Flipping TransStrToFlipping(std::string str);

    ML::MLColorimeter::Rotation TransStrToRotation(std::string str);

    std::string TransAEStatusToStr(ML::CameraV2::AutoExposureStatus status);

    std::string TransDeviceTypeToStr(ML::MLSpecbos::DeviceType dtype);

    ML::MLSpecbos::DeviceType TransStrToDeviceType(std::string str);

    double TransPosToVID(double position, double infinity, double focalLength, double focalSpace, bool isReverse = false);

    double TransVIDToPos(double VID, double infinity, double focalLength, double focalSpace, bool isReverse = false);

    double TransPosToDiopter(double position, double infinity, double focalLength, double focalSpace, bool isReverse = false);

    double TransDiopterToPos(double diopter, double infinity, double focalLength, double focalSpace, bool isReverse = false);

    bool WriteMetaDataToJson(std::string filename, ML::MLColorimeter::InUseCaliData data);

    bool SaveMatToTxt(std::string filename, cv::Mat img);

    cv::Mat CalculateAverageImage(std::vector<cv::Mat> image_list);

    bool GetNeedFile(std::string path, std::string fileType, std::vector<std::string>* fileList);

    double Get_Time();

    bool In_Range(double x, double a, double b);

    template <typename K, typename V>
    std::pair<K, V> GetMaximumValue(const std::map<K, V>& map);

    std::vector<int> Find_Peak(std::vector<double>& data);

    std::vector<int> Find_Valley(std::vector<double>& data);

    bool resizeML(cv::Mat src, cv::Mat& dst, cv::Size size);

    static int iniHandler(void* user, const char* section, const char* name, const char* value);

    bool readIniFile(const std::string& filename,
                     std::map<std::string, std::map<std::string, std::string>>& config);

    bool writeIniFile(const std::string& filename,
                      const std::map<std::string, std::map<std::string, std::string>>& config);
					  
    double Convert32To16Scale(cv::Mat& image);

    double Convert32To16ScaleFunc(cv::Mat& image);

 private:
    MLColorimeterHelp();

 private:
    static MLColorimeterHelp* m_instance;
    ML::MLColorimeter::Flipping m_FlipX = ML::MLColorimeter::Flipping::NonReverse;
    ML::MLColorimeter::Flipping m_FlipY = ML::MLColorimeter::Flipping::NonReverse;
    ML::MLColorimeter::Rotation m_Rotation = ML::MLColorimeter::Rotation::R0;
};
}  // namespace MLColorimeter
}  // namespace ML

#endif  // SRC_MLCOLORIMETERCOMMON_MLCOLORIMETERHELP_H_
