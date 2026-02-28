#pragma once
#include <json.hpp>
#include <QObject>
#include "ImageDetection/ml_distortion_correction.h"


using Json = nlohmann::json;

struct AlgorithmCalibrationConfig
{
    double camera_matrix[3][3];
    double dist_coeffs[5][1];
    double pixelSize;
    double magnification;

};
class GrayscaleImageCapture  : public QObject
{
	Q_OBJECT

public:
	GrayscaleImageCapture(QObject *parent=nullptr);
	~GrayscaleImageCapture();
    Json settingJsonObj;
    AlgorithmCalibrationConfig GetAlgorithmConfigInfo(std::string binningstr = "2*2");
    cv::Mat getUndistortImage(std::string binningstr ="2*2");
    void load(const char* path);
private:
    MLImageDetection::MLDistortionCorrection undistortAlg;
    cv::Size img_size_ = cv::Size(0, 0);
};
