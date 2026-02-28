#include "MLColorimeterMode.h"
#include "grayscaleimagecapture.h"
#include <fstream>
#include <iostream>
#include <QHash>
#include <QVariant>
#include "pluginsystem/Services.h"
#include "Core/loggingwrapper.h"

GrayscaleImageCapture::GrayscaleImageCapture(QObject *parent)
	: QObject(parent)
{
    load("config/lens_calibration.json");
}

GrayscaleImageCapture::~GrayscaleImageCapture()
{
}

AlgorithmCalibrationConfig GrayscaleImageCapture::GetAlgorithmConfigInfo(std::string binningstr)
{
    AlgorithmCalibrationConfig algorithmConfigInfo;
    Json& mvLensCfg = settingJsonObj["LensCamera"]["MV_Lens_DistortionCorrection"];
    Json& matrixInfo = mvLensCfg[binningstr];
    if (matrixInfo.is_null())
    {
        return algorithmConfigInfo;
    }
    std::vector<std::vector<double>> camera_Matrix = matrixInfo["cameraMatrix"];
    std::vector<std::vector<double>> dist_Coeffs = matrixInfo["distCoeffs"];
    int i = 0;
    for (std::vector<std::vector<double>>::iterator it = camera_Matrix.begin(); it != camera_Matrix.end(); it++)
    {

        int j = 0;
        for (std::vector<double>::iterator gt = it->begin(); gt != it->end(); gt++)
        {
            algorithmConfigInfo.camera_matrix[i][j++] = *gt;
        }
        i++;
        j = 0;
    }

    int a = 0;
    for (std::vector<std::vector<double>>::iterator it = dist_Coeffs.begin(); it != dist_Coeffs.end(); it++)
    {
        int b = 0;
        for (std::vector<double>::iterator gt = it->begin(); gt != it->end(); gt++)
        {
            algorithmConfigInfo.dist_coeffs[a][b++] = *gt;
        }
        a++;
        b = 0;
    }
    algorithmConfigInfo.pixelSize = settingJsonObj["LensCamera"]["PixelSize"].get<double>();
    algorithmConfigInfo.magnification = settingJsonObj["LensCamera"]["magnification"].get<double>();

    return algorithmConfigInfo;
}

cv::Mat GrayscaleImageCapture::getUndistortImage(std::string binningstr)
{
    cv::Mat img;
    Result ret = MLColorimeterMode::Instance()->takeImage(img);
    if(!ret.success){
        return cv::Mat();
    }

     if (img.size().width != img_size_.width || img.size().height != img_size_.height)
     {
         img_size_ = img.size();
         AlgorithmCalibrationConfig matInfo = GetAlgorithmConfigInfo(binningstr);
         cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << matInfo.camera_matrix[0][0], matInfo.camera_matrix[0][1], matInfo.camera_matrix[0][2], matInfo.camera_matrix[1][0], matInfo.camera_matrix[1][1], matInfo.camera_matrix[1][2], matInfo.camera_matrix[2][0], matInfo.camera_matrix[2][1], matInfo.camera_matrix[2][2]);
         cv::Mat distCoeffs = (cv::Mat_<double>(5, 1) << matInfo.dist_coeffs[0][0], matInfo.dist_coeffs[0][1], matInfo.dist_coeffs[0][2], matInfo.dist_coeffs[0][3], matInfo.dist_coeffs[0][4]);
         undistortAlg.setDistortionMat(cameraMatrix, distCoeffs);
         undistortAlg.setImageSize(img_size_);
     }
     return undistortAlg.GetUndistortedImage(img);
}

void GrayscaleImageCapture::load(const char* path)
{
    std::ifstream jsonFile(path);
    if (jsonFile.is_open())
    {
        std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)),
            (std::istreambuf_iterator<char>()));
        jsonFile.close();
        settingJsonObj = Json::parse(contents);
    }
}
