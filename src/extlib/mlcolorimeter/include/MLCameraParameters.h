// Copyright 2024 MLOptic

#ifndef SRC_MLCOLORIMETER_MLCAMERAPARAMETERS_H_
#define SRC_MLCOLORIMETER_MLCAMERAPARAMETERS_H_

#include <opencv2/opencv.hpp>

#include "mlcolorimeter_global.h"

#define M_PI 3.14159265358979323846

/**
 * @file MLCameraParameters.h
 * @brief Defines the camera parameters class.
 */

namespace ML {
namespace MLColorimeter {

 /**
 * @class MLCameraParameters
 * @brief contain parameters of the camera.
 */
class MLCOLORIMETER_EXPORT MLCameraParameters {
 public:
    MLCameraParameters();
    ~MLCameraParameters();

    /**
     * @brief Get camera focal length (unit: millimeter).
     * @return the focal length.
     */
    double ML_GetFocalLength();

    /**
     * @brief Get camera pixel size (unit: millimeter).
     * @return the pixel size.
     */
    double ML_GetPixelSize();

    /**
     * @brief Get camera optical axis center.
     * @return the optical axis center.
     */
    cv::Point2f ML_GetCrossCenter();

    /**
     * @brief Enter a coordinate to calculate CRA.
     * @param point  the coordinate to calculate the CRA of optical axis center.
     * @return the CRA value.
     */
    double ML_CalculateCRA(cv::Point2f point);

 private:
    double m_focalLength;
    double m_pixelSize;
    cv::Point2f m_crossCenter;
    double m_CRA;
};
}  // namespace MLColorimeter
}  // namespace ML

#endif  // SRC_MLCOLORIMETER_MLMONOBUSINESSMANAGE_H_
