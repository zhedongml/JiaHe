// Copyright 2025 MLOptic

#ifndef SRC_MLCOLORIMETERIMAGEPROCESS_MLCOLORIMETERALGORITHMS_H_
#define SRC_MLCOLORIMETERIMAGEPROCESS_MLCOLORIMETERALGORITHMS_H_

#include <QtCore/qobject.h>

#include "MLColorimeterCommon.h"
#include "Result.h"
#include "mlcolorimeterimageprocess_global.h"
/**
 * @file MLColorimeterAlgorithms.h
 * @brief Defines the calibration process and mtf pipeline class and associated
 * functionality.
 */

class PipeLine;

namespace ML {
namespace MLCalibrationProcess {
class FFCProcessor;
}
namespace MLColorimeter {
class MLColorimeterConfig;

/**
 * @class MLColorimeterAlgorithms
 * @brief Manages the calibration process and mtf pipeline.
 */
class MLCOLORIMETERIMAGEPROCESS_EXPORT MLColorimeterAlgorithms : public QObject {
    Q_OBJECT

 public:
    MLColorimeterAlgorithms(QObject* parent = nullptr);
    ~MLColorimeterAlgorithms();

    /**
     * @brief Set config path of calibration process.
     * @param path  calibration profile path.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetConfigPath(const char* path);

    /**
     * @brief Load dark images of exposuretime list from external path.
     * @param path  external dark images path.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_LoadDarkByExposureTimeList(const char* path);

    /**
     * @brief Load calibration profile data.
     * @param config  config setting to load calibration data.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_LoadCalibrationData(ML::MLColorimeter::CalibrationConfig config);

    /**
     * @brief get status of loading calibration profile data.
     */
    virtual ML::MLColorimeter::ProcessedStatus ML_GetLoadCalibrationStatus();

    /**
     * @brief Get calibration profile data after calling ML_LoadCalibrationData().
     */
    virtual ML::MLColorimeter::PreCalibrationData ML_GetPreCalibrationData();

    /**
     * @brief Update calibration profile data.
     * @param preCaliData  calibration profile data to update.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_UpdatePreCalibrationData(ML::MLColorimeter::PreCalibrationData preCaliData);

    /**
     * @brief  Set CaptureData map to perform calibration process.
     * @param dataMap  CaptureData map.
     * @param isSubDarkFromList  if set to true, subtract dark image after capturing.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetCaptureDataMap(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap,
                                        bool isSubDarkFromList = false);

    /**
     * @brief  Get CaptureData map after calling ML_SetCaptureDataMap().
     * @return a map of CaptureData.
     */
    virtual std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> ML_GetCaptureDataMap();

    /**
     * @brief  Execute calibration process according to the CalibrationConfig.
     * @param config  config setting to do calibration process.
     * @return the result contains the message, code and status.
     * @note If the parameter isSubDarkFromList is set to true when calling
     * ML_SetCaptureDataMap(). So, when calling ML_Process(), the Dark_flag of the
     * parameter config must be set to false.
     */
    virtual Result ML_Process(ML::MLColorimeter::CalibrationConfig config);

    /**
     * @brief get status of image processimg
     */
    virtual ML::MLColorimeter::ProcessedStatus ML_GetProcessedStatus();

    /**
     * @brief  Get processed data after calling ML_Process().
     * @return the data after calibration process.
     */
    virtual std::map<ML::MLColorimeter::CalibrationEnum,
                     std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::ProcessedData>>
    ML_GetProcessedData();

    /**
     * @brief  Clear processed data cache.
     */
    virtual Result ML_ClearProcessedData();

    /**
     * @brief  Save processed data after calling ML_Process().
     * @param caliData  the calibration data to save, get from ML_GetProcessedData().
     * @param saveconfig  save config setting.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SaveProcessedData(
        std::map<ML::MLColorimeter::CalibrationEnum, std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::ProcessedData>>
            caliData,
        ML::MLColorimeter::SaveDataConfig saveconfig);

    /**
     * @brief  Save processed data to txt file after calling ML_Process().
     * @param caliData  the calibration data to save, get from ML_GetProcessedData().
     * @param saveconfig  save config setting.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SaveTxtResult(
        std::map<ML::MLColorimeter::CalibrationEnum, std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::ProcessedData>>
            caliData,
        ML::MLColorimeter::SaveDataConfig saveconfig);

    /**
     * @brief  Get ROI center based on optical axis center and offset.
     * @param image  input an image containing one cross-hair.
     * @param offset  offset between optical axis center and roi center.
     * @return roi center coordinate.
     */
    virtual std::vector<cv::Point> ML_GetCrossCenter(cv::Mat image, int offset);

    /**
     * @brief  Calculate mtf based on one image containing one cross-hair.
     * @param image  input an image containing one cross-hair.
     * @param name  ROI name
     * @param focusLength  focal length of the module.
     * @param pixelSize  pixel size of the camera
     * @param lpmmUnit  if set to true, use lp/mm unit to calculate mtf, else use
     * lp/degree.
     * @param chessMode  if set to true, calculate mtf for chessboard, else for
     * cross-hair.
     * @return a map of curve data, including mtf, lsf, esf.
     */
    virtual std::map<std::string, double*> ML_CalculateMTF(cv::Mat image, std::string name, double focusLength, double pixelSize,
                                                           bool lpmmUnit, bool chessMode);

    /**
     * @brief  Clear pipeline cache before calling ML_GetMTFByFreq().
     * @return a map of curve data, including mtf, lsf, esf.
     */
    virtual Result ML_ClearMTFPipeline();

    /**
     * @brief  Get mtf curve by freq after calling ML_CalculateMTF().
     * @param freq  input freq to get mtf curve
     * @return the mtf curve.
     */
    virtual std::map<std::string, double> ML_GetMTFByFreq(double freq);

 public:
    virtual double CalculateMTF(cv::Mat image, double freq, double focusLength, double pixelSize, bool lpmmUnit = true,
                                bool chessMode = false, int binNum = 1);

    virtual double CalculateStd(cv::Mat img);

    virtual Result Smooth(double* value, int half_window, int len);

    virtual int FindGaussianFitMaxValue(std::vector<cv::Point2f> pts, std::vector<double>& posList, std::vector<double>& mtfList);

    virtual int FindPolynomialFitMaxValue(std::vector<double>& posList, std::vector<double>& mtfList, int step = 101);

    virtual bool UpdateFlip_RotateJson(ML::MLColorimeter::Flipping flipx, ML::MLColorimeter::Flipping flipy,
                                       ML::MLColorimeter::Rotation rotation);
 signals:
    void sendProcessStatusSignal(ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CalibrationEnum, bool);

 private:
    virtual bool GaussiantCurveFit(const std::vector<cv::Point2f>& cvRawPointVec, int n, cv::Mat& A);

 private:
    virtual Result DarkCorrect(ProcessedData config);

    virtual Result FFCCorrect(ProcessedData config);

    virtual Result ColorShiftCorrect(ProcessedData config);

    virtual Result DistortionCorrect(ProcessedData config);

    virtual Result ExposureCorrect(ProcessedData config);

    virtual Result FourColorCalculation(ProcessedData config);

    virtual Result LuminanceCalculation(ProcessedData config);

    virtual Result RadianceCalculation(ProcessedData config);

    virtual Result FOVCropCorrect(ProcessedData config);

    virtual Result Flip_RotateCorrect(ProcessedData config);

 private:
    MLColorimeterConfig* m_BMConfig = nullptr;
    std::map<std::string, PipeLine*> pipelinevec;
    std::string m_configPath;
    ML::MLColorimeter::PreCalibrationData m_preCaliData;
    std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::InUseCaliData> m_InUseCaliData;
    std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> m_CaptureData;
    std::map<ML::MLColorimeter::CalibrationEnum, std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::ProcessedData>>
        m_afterCaliData;
    PipeLine* m_throughFocusPipLine = nullptr;
    std::vector<double> m_darkETList;
    bool m_useDarkList = false;
    std::map<ML::MLFilterWheel::MLFilterEnum, std::string> m_darkPathMap;
    std::string m_prevDarkPath = "";
    std::map<double, std::string> m_darkMap;

    std::map<std::string, ML::MLCalibrationProcess::FFCProcessor*> m_ffcProcesser;
    ML::MLColorimeter::ProcessedStatus m_loadStatus;
    ML::MLColorimeter::ProcessedStatus m_processStatus;
    ML::CameraV2::Binning m_blurBinning = ML::CameraV2::Binning::ONE_BY_ONE;
};
}  // namespace MLColorimeter
}  // namespace ML

#endif  // SRC_MLCOLORIMETERIMAGEPROCESS_MLCOLORIMETERALGORITHMS_H_
