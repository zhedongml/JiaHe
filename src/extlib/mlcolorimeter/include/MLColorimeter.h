// Copyright 2024 MLOptic

#ifndef SRC_MLCOLORIMETER_MLCOLORIMETER_H_
#define SRC_MLCOLORIMETER_MLCOLORIMETER_H_

#include <QtCore/qobject.h>

#include <map>
#include <vector>

#include "MLColorimeterCommon.h"
#include "MLColorimeterPlugin.h"
#include "Result.h"
#include "mlcolorimeter_global.h"

/**
 * @file MLColorimeter.h
 * @brief Defines the colorimeter system class and associated functionality.
 */

namespace ML {

namespace MLFilterWheel {
class MLFilterWheelCallback;
}  // namespace MLFilterWheel

namespace MLColorimeter {

class MLBinoBusinessManage;
class MLColorimeterAlgorithms;
class MLColorimeterHelp;

/**
 * @class MLColorimeter
 * @brief Manages the colorimeter system.
 */
class MLCOLORIMETER_EXPORT MLColorimeter : public QObject, public MLColorimeterPlugin {
    Q_OBJECT
    /*
     *Use Q_INTERFACES() macro tells Qt's meta-object system about the
     interface *and tells the meta-object system "I want to use this interface
     */
    Q_INTERFACES(MLColorimeterPlugin)
    /*
     * Use Q_ PLUGIN_ METADATA() macro export plug-in
     */
    Q_PLUGIN_METADATA(IID AbstractInterface_iid FILE "MLColorimeterInterface.json")

 public:
    MLColorimeter(QObject* parent = nullptr);
    ~MLColorimeter();

    /**
     * @brief Get the businessmanage instance.
     * @return the result contains the message, code and status.
     */
    virtual MLBinoBusinessManage* ML_GetBusinessManageModule();

    /**
     * @brief Capture FFC images for SDK calibration config.
     * @param moduleID  select a module to save measurement result.
     * @param savePath  path to save ffc images
     * @param ndKey  nd filter keyword, from the "Key" field in NDFilterConfig.json.
     * @param xyzKey  color filter keyword, from the "Key" field in NDFilterConfig.json.
     * @param nd  nd filter to switch during capture.
     * @param xyzList  xyz filter list to switch during capture.
     * @param rx  rx to switch during capture.
     * @param avgCount count to calculate average image for one color filter.
     * @param exposure  exposure setting during capture.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_CaptureFFCImg(int moduleID, std::string savePath, std::string ndKey, std::string xyzKey,
                                    ML::MLFilterWheel::MLFilterEnum nd, std::vector<ML::MLFilterWheel::MLFilterEnum> xyzList,
                                    RXCombination rx = RXCombination(), int avgCount = 5,
                                    ExposureSetting exposure = {ExposureMode::Auto, 1000});

    /**
     * @brief Capture FFC images for SDK calibration config.
     * @param moduleID  select a module to save measurement result.
     * @param savePath  path to save ffc images.
     * @param ndKey  nd filter keyword, from the "Key" field in NDFilterConfig.json.
     * @param xyzKey  color filter keyword, from the "Key" field in NDFilterConfig.json.
     * @param nd  nd filter to switch during capture.
     * @param xyzList  xyz filter list to switch during capture.
     * @param rx  rx to switch during capture.
     * @param avgCount count to calculate average image for one color filter.
     * @param exposureMap  exposure map of color filter during capture.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_CaptureFFCImg(int moduleID, std::string savePath, std::string ndKey, std::string xyzKey,
                                    ML::MLFilterWheel::MLFilterEnum nd, std::vector<ML::MLFilterWheel::MLFilterEnum> xyzList,
                                    RXCombination rx = RXCombination(), int avgCount = 5,
                                    std::map<ML::MLFilterWheel::MLFilterEnum, ExposureSetting> exposureMap = {
                                        {ML::MLFilterWheel::MLFilterEnum::X, {ExposureMode::Auto, 1000}},
                                        {ML::MLFilterWheel::MLFilterEnum::Y, {ExposureMode::Auto, 1000}},
                                        {ML::MLFilterWheel::MLFilterEnum::Z, {ExposureMode::Auto, 1000}}});

    /**
     * @brief Capture FFC images of color camera for SDK calibration config.
     * @param moduleID  select a module to save measurement result.
     * @param savePath  path to save ffc images.
     * @param ndKey  nd filter keyword, from the "Key" field in NDFilterConfig.json.
     * @param nd  nd filter to switch during capture.
     * @param rx  rx to switch during capture.
     * @param avgCount count to calculate average image for one color filter.
     * @param exposure  exposure setting during capture.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_CaptureFFCImgForColorCamera(int moduleID, std::string savePath, std::string ndKey,
                                                  ML::MLFilterWheel::MLFilterEnum nd, RXCombination rx = RXCombination(),
                                                  int avgCount = 5, ExposureSetting exposure = {ExposureMode::Auto, 100});

    /**
     * @brief Capture FFC images of color camera for SDK calibration config.
     * @param moduleID  select a module to save measurement result.
     * @param savePath  path to save ffc images.
     * @param ndKey  nd filter keyword, from the "Key" field in NDFilterConfig.json.
     * @param nd  nd filter to switch during capture.
     * @param rx  rx to switch during capture.
     * @param avgCount count to calculate average image for one color filter.
     * @param exposureMap  fixed exposure map setting during capture.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_CaptureFFCImgForColorCamera(int moduleID, std::string savePath, std::string ndKey,
                                                  ML::MLFilterWheel::MLFilterEnum nd, RXCombination rx = RXCombination(),
                                                  int avgCount = 5,
                                                  std::map<ML::MLFilterWheel::MLFilterEnum, ExposureSetting> exposureMap = {
                                                      {ML::MLFilterWheel::MLFilterEnum::X, {ExposureMode::Fixed, 100}},
                                                      {ML::MLFilterWheel::MLFilterEnum::Y, {ExposureMode::Fixed, 100}},
                                                      {ML::MLFilterWheel::MLFilterEnum::Z, {ExposureMode::Fixed, 100}},
                                                  });

    /**
     * @brief Calculate mean image after calling ML_CaptureFFCImg() when using Synthetic
     * method.
     * @param moduleID  select a module to save measurement result.
     * @param savePath  images' path saved by ML_CaptureFFCImg().
     * @param aperture  lens aperture setting
     * @param nd  nd filter setting to calculate.
     * @param xyz  xyz filter setting to calculate.
     * @param sphere_list  sphere list to calculate
     * @param lightSource  lightSource setting to calcuat.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_CalSyntheticMeanImg(int moduleID, std::string savePath, std::string aperture,
                                          ML::MLFilterWheel::MLFilterEnum nd, ML::MLFilterWheel::MLFilterEnum xyz,
                                          std::vector<double> sphere_list = {0}, std::string lightSource = "");

    /**
     * @brief Capture Images for calculating M matrix.
     * @param moduleID  select a module to capture M images.
     * @param savePath  path to save M images.
     * @param lightSource  light source setting
     * @param ndKey  nd filter keyword, from the "Key" field in NDFilterConfig.json.
     * @param xyzKey  color filter keyword, from the "Key" field in NDFilterConfig.json.
     * @param nd  nd filter to switch during capture.
     * @param rx  rx to switch during capture.
     * @param avgCount count to calculate average image for one color filter.
     * @param exposure  exposure setting during capture.
     * @param rect  ROI setting to calculate M matrix.
     * @param isDoFFC  if set to true, doing flat filed for M images before calculating M
     * matrix.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_CaptureImgAndCalMMatrix(int moduleID, std::string savePath, std::string lightSource, std::string ndKey,
                                              std::string xyzKey, ML::MLFilterWheel::MLFilterEnum nd,
                                              RXCombination rx = RXCombination(), int avgCount = 5,
                                              ExposureSetting exposure = {ExposureMode::Auto, 1000}, cv::Rect rect = cv::Rect(),
                                              bool isDoFFC = false);

    /**
     * @brief Capture Images for calculating M matrix.
     * @param moduleID  select a module to capture M images.
     * @param savePath  path to save M images.
     * @param lightSource  light source setting
     * @param ndKey  nd filter keyword, from the "Key" field in NDFilterConfig.json.
     * @param xyzKey  color filter keyword, from the "Key" field in NDFilterConfig.json.
     * @param nd  nd filter to switch during capture.
     * @param rx  rx to switch during capture.
     * @param avgCount count to calculate average image for one color filter.
     * @param exposureMap  exposure map of color filter during capture.
     * @param rect  ROI setting to calculate M matrix.
     * @param isDoFFC  if set to true, doing flat filed for M images before calculating M
     * matrix.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_CaptureImgAndCalMMatrix(int moduleID, std::string savePath, std::string lightSource, std::string ndKey,
                                              std::string xyzKey, ML::MLFilterWheel::MLFilterEnum nd,
                                              RXCombination rx = RXCombination(), int avgCount = 5,
                                              std::map<ML::MLFilterWheel::MLFilterEnum, ExposureSetting> exposureMap =
                                                  {{ML::MLFilterWheel::MLFilterEnum::X, {ExposureMode::Auto, 1000}},
                                                   {ML::MLFilterWheel::MLFilterEnum::Y, {ExposureMode::Auto, 1000}},
                                                   {ML::MLFilterWheel::MLFilterEnum::Z, {ExposureMode::Auto, 1000}}},
                                              cv::Rect rect = cv::Rect(), bool isDoFFC = false);

    /**
     * @brief Capture Images for calculating M matrix for color camera.
     * @param moduleID  select a module to capture M images.
     * @param savePath  path to save M images.
     * @param lightSource  light source setting
     * @param ndKey  nd filter keyword, from the "Key" field in NDFilterConfig.json.
     * @param nd  nd filter to switch during capture.
     * @param rx  rx to switch during capture.
     * @param avgCount count to calculate average image for one color filter.
     * @param exposure  exposure setting during capture.
     * @param rect  ROI setting to calculate M matrix.
     * @param isDoFFC  if set to true, doing flat filed for M images before calculating M
     * matrix.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_CaptureImgAndCalMMatrixForColorCamera(int moduleID, std::string savePath, std::string lightSource,
                                                            std::string ndKey, ML::MLFilterWheel::MLFilterEnum nd,
                                                            RXCombination rx = RXCombination(), int avgCount = 5,
                                                            ExposureSetting exposure = {ExposureMode::Auto, 1000},
                                                            cv::Rect rect = cv::Rect(), bool isDoFFC = false);

    /**
     * @brief Save M matrix and exposure time of M images.
     * @param moduleID  select a module to save M matrix and exposure time.
     * @param savePath  path to save M matrix and exposure.
     * @param aperture  aperture setting to save M matrix and exposure time.
     * @param nd  nd setting to save M matrix and exposure time.
     * @param rx  rx setting to save M matrix and exposure time.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SaveMMatrixAndExposure(int moduleID, std::string savePath, std::string aperture,
                                             ML::MLFilterWheel::MLFilterEnum nd, RXCombination rx = RXCombination());

    /**
     * @brief Get M matrix after calling ML_CaptureImgForMMatrix().
     * @param moduleID  select a module to get M matrix.
     * @return the result contains the message, code and status.
     */
    virtual cv::Mat ML_GetMMatrix(int moduleID);

    /**
     * @brief Calculate N matrix.
     * @param NMatrixPath  N matrix json file to calculate N matrix.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_CalculateNMatrix(std::string NMatrixPath);

    /**
     * @brief Set NMatrix for calculate R matrix.
     * @param NMatrix  N matrix setting
     */
    virtual Result ML_SetNMatrix(cv::Mat NMatrix);

    /**
     * @brief Get N matrix after calling ML_CalculateNMatrix().
     * @return the N matrix.
     */
    virtual cv::Mat ML_GetNMatrix();

    /**
     * @brief Calculate and save R matrix to calibration profile.
     * @param moduleID  select a module to calculate R matrix.
     * @param nd  nd filter setting to calculate R matrix.
     * @param saveToJson  save R matrix to json file.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_CalRMatrix(int moduleID, ML::MLFilterWheel::MLFilterEnum nd, bool saveToJson = true);

    /**
     * @brief Get R matrix after calling ML_CalRMatrix().
     * @param moduleID  select a module to get R matrix.
     * @return the result contains the message, code and status.
     */
    virtual cv::Mat ML_GetRMatrix(int moduleID);

    /**
     * @brief Load offline images to calculate M matrix.
     * @param imgPath  save path for images called by ML_CaptureImgAndCalMMatrix() or same
     * structure.
     * @param SN  serial nubmer to load images.
     * @param aperture  aperture setting to load images.
     * @param nd  nd setting to load images.
     * @param rx  rx setting to load images.
     * @param rect  ROI setting to calculate M matrix.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_LoadImgAndCalMMatrix(std::string imgPath, std::string SN, std::string aperture,
                                           ML::MLFilterWheel::MLFilterEnum nd, RXCombination rx = RXCombination(),
                                           cv::Rect rect = cv::Rect());

    /**
     * @brief Get M matrix offline after calling ML_LoadImgAndCalMMatrix().
     * @return the M matrix.
     */
    virtual cv::Mat ML_GetMMatrix();

    /**
     * @brief Calculate and save R matrix to calibration profile.
     * @param path  configuration path.
     * @param SN  serial nubmer to calculate R matrix.
     * @param aperture  aperture setting to calculate R matrix.
     * @param nd  nd filter setting to calculate R matrix.
     * @param rx  rx setting to calculate R matrix.
     * @param saveToJson  save R matrix to json file.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_CalRMatrixOffline(std::string path, std::string SN, std::string aperture,
                                        ML::MLFilterWheel::MLFilterEnum nd, RXCombination rx = RXCombination(),
                                        bool saveToJson = true);

    /**
     * @brief Get R matrix offline after calling ML_CalRMatrixOffline().
     * @return the R matrix.
     */
    virtual cv::Mat ML_GetRMatrix();

 private:
    virtual int TransLightSourceToInt(std::string lightSource);

    virtual int TransFilterEnumToInt(ML::MLFilterWheel::MLFilterEnum fenum);

 private:
    MLBinoBusinessManage* m_manage = nullptr;
    std::map<int, cv::Mat> m_ETMap;
    std::map<int, cv::Mat> m_MMatrix;
    std::map<int, cv::Mat> m_RMatrix;
    cv::Mat m_offlineMMatrix;
    cv::Mat m_offlineRMatrix;
    cv::Mat m_NMatrix;
    double m_Lw;
};
}  // namespace MLColorimeter
}  // namespace ML

#endif  //  SRC_MLCOLORIMETER_MLCOLORIMETER_H_
