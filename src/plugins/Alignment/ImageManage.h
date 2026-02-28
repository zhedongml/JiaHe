#pragma once

#include <opencv2/opencv.hpp>
#include <QString>

#define FIDUCIAL_ERROR_IMAGE_FULL_PATH "./config/alignInputGratingImage/";
#define FIDUCIAL_IDENTIFY_IMAGE_FULL_PATH "./config/fiducialIdentifyImage/";

class ImageManage
{
public:
    explicit ImageManage();
    ~ImageManage();

    bool saveFiducialErrorImage(cv::Mat image);

    bool saveFiducialIdentifyImage(cv::Mat image);

private:
    bool saveImage(cv::Mat image, QString fileName, QString filePath);

};
