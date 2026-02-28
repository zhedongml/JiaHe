// Copyright 2024 MLOptic

#ifndef SRC_MLCOLORIMETER_MLCENTERFIT_H_
#define SRC_MLCOLORIMETER_MLCENTERFIT_H_

#include <opencv2/opencv.hpp>

#include "MLColorimeterCommon.h"
#include "json.hpp"
#include "mlcolorimeter_global.h"

#define M_PI 3.14159265358979323846

/**
 * @file MLCenterFit.h
 * @brief Cylinder Axis Circle fit for optical axis center.
 */

namespace ML {
namespace MLColorimeter {
using json = nlohmann::ordered_json;

/**
 * @class MLCameraParameters
 * @brief contain parameters of the camera.
 */
class MLCOLORIMETER_EXPORT MLCenterFit {
 public:
    MLCenterFit();
    ~MLCenterFit();

    /**
     * @brief Load Circle Fit config.
     * @param filename  config path
     * @return bool.
     */
    virtual bool ML_LoadCircleFitConfig(std::string filename);

    /**
     * @brief Load Polynomial Fit config.filename
     * @param filename  config path
     * @return bool.
     */
    virtual bool ML_LoadPolynomialFitConfig(std::string filename);

    virtual cv::Mat ML_CircleFit(cv::Mat img, ML::MLColorimeter::RXCombination rx,
                                 ML::MLFilterWheel::MLFilterEnum colorFilter, int binNum = 1);

    virtual cv::Mat ML_PolynomialFit(cv::Mat img, ML::MLColorimeter::RXCombination rx,
                                     ML::MLFilterWheel::MLFilterEnum colorFilter,
                                     int binNum = 1);

 private:
    json m_Json;
    std::map<double, ML::MLColorimeter::CircleFitParameters> m_CircleMap;
    std::map<double, ML::MLColorimeter::PolynomialFitParameters> m_PolyMap;
    std::map<ML::MLFilterWheel::MLFilterEnum, cv::Point2f> m_OpticalMap;
};
}  // namespace MLColorimeter
}  // namespace ML

#endif  // SRC_MLCOLORIMETER_MLCENTERFIT_H_
