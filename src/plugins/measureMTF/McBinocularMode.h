#pragma once

#include <windows.h>
#include <QMap>
#include <QObject>
#include <opencv2\opencv.hpp>

#include "MLBinoBusinessManage.h"
#include "MLMonoBusinessManage.h"
#include "MLColorimeterAlgorithms.h"
#include "MLCamaraCommon.h"
#include "MLColorimeter.h"
#include "MLColorimeterConfig.h"

#include "MLCameraParameters.h"
#include "MLColorimeterHelp.h"
#include "MLColorimeterPlugin.h"
#include "ML_addInInterface.h"
#include "pluginException.h"

#include "PrjCommon/metricsdata.h"
#include "pluginsystem/Services.h"
#include "PrjCommon/service/McCommon.h"
#include <QWaitCondition>
//#include"ml_image_public.h"
#include"ImageDetection/ml_image_public.h"
#include "PrjCommon/service/ml.h"
#include <json.hpp>
#include <QFutureSynchronizer>
#include "./AutoExposure/AutoExposureAlgorithmA.h"
#include "PrjCommon/DeviceInterface.h"
#include "McUtils.h"

using Json = nlohmann::json;

using namespace std;
using namespace ML::MLColorimeter;
using namespace ML::MLFilterWheel;
using namespace ML::CameraV2;

struct BinocularInfo
{
    double vid;
    string nd;
    string color;
    string revolutionCylPos;
    int selfRotationCylPos;
    ML::CameraV2::MLPixelFormat format;
    double et;
    ML::CameraV2::Binning binning;
    ML::CameraV2::BinningMode binmode;
    string RX;
};

struct XyzExposureCache
{
    bool isAuto = false;
    QMap<QString, float> initTime; // key:X,Y,Z
};

struct ImgExposureCache
{
    bool isSet = false;
    bool isAuto = false;
    float initTime;
    int OverExposureFactor = 0;
};

class McBinocularMode : public QObject, public DeviceInterface
{
    Q_OBJECT

  public:
    ~McBinocularMode();
    static McBinocularMode *instance(QObject *parent = nullptr);
    Result connectDevice() override;
    Result disconnectDevice() override;
    Result connectBefore() override;
    Result connectAfter() override;

    Result connect();
    Result initConfig();
    Result disconnect();
    bool isConnect();
    bool isColorCamera();
    GrabberBase *getCamera();
    ActuatorBase *getCameraMotion();

    Result updateAperture(const QString& aperture);

    Result setNdFilter(QString nd, bool judge = false);
    Result setXxzFilter(QString xyz, bool judge = false);

    Result setNdFilterAsyn(QString nd);
    Result setXxzFilterAsyn(QString xyz);

    Result setFocus(double focus, bool judge = false);
    Result setFocusAsyn(double focus);
    float getFocus();
    Result setDiopters(double diopters, bool judge = false);

    bool isGrabbing();
    Result stopGrabbing(bool stop, bool judge = false);
    Result setExposureTime(double ms, bool judge = false);
    Result setAdjustExposureTime(double ms, bool judge = false);
    Result setBitDepth(int bitDepth, bool judge = false);
    Result setBitDepth(ML::CameraV2::MLPixelFormat bitDepth, bool judge = false);
    Result setBinning(int binning, bool judge = false);
    Result setBinning(ML::CameraV2::Binning binning, bool judge = false);
    Result setBinningMode(const QString &binningMode, bool judge = false);
    Result setBinningMode(ML::CameraV2::BinningMode binningMode, bool judge = false);
    Result autoExposure_old(int initTime, bool isSet);
    Result autoExposure(int initTime, bool isSet);

    Result saveImage(QString fileName, CaptureData &dataRet, Result autoExpInfo, bool saveRawImage = true, bool judgeET = true);
    Result saveImage_YY(QString fileName, ImgExposureCache aeCache, bool saveRawImage = true, int imgSaveBit = -1);
    Result saveImage_XYZ(QString fileName, XyzExposureCache aeCache = XyzExposureCache(), bool saveRawImage = true, bool saveSLBYY = false, bool luminanceSave = false, int imgSaveBit = -1);
    Result saveImage_xyY(QString fileName, XyzExposureCache aeCache = XyzExposureCache(), bool saveRawImage = true, bool saveSLBYY = false);
    Result saveImage_raw(QString fileName, ImgExposureCache aeCache);
    Result saveImage_FFC(QString fileName, ImgExposureCache aeCache, QString imgPaths);

    //============ color camera ============//
    Result color_saveImage_YY(QString fileName, ImgExposureCache aeCache, bool saveRawImage = true, int imgSaveBit = -1);
    Result color_saveImage_XYZ(QString fileName, ImgExposureCache aeCache, bool saveRawImage = true, bool saveSLBYY = false, bool luminanceSave = false, int imgSaveBit = -1);
    Result color_saveImage_xyY(QString fileName, ImgExposureCache aeCache, bool saveRawImage = true, bool saveSLBYY = false, bool luminanceSave = false, int imgSaveBit = -1);

    Result color_captureYImage(std::map<MLFilterEnum, CaptureData>& data, Result& exposeRet,
        QString fileName, ImgExposureCache aeCache,
        bool saveRawImage, bool judgeET = true);
    Result color_captureXYZImages(std::map<MLFilterEnum, CaptureData>& data, Result& exposeRet,
        QString fileName, ImgExposureCache aeCache,
        bool saveRawImage, bool luminanceSave, bool judgeET = true);

    //============ color camera end============//

    Result captureImages(CaptureData& data);
    Result captureImages_old(CaptureData &data);
    Result captureImageFast(cv::Mat& img);
    cv::Mat takeImage();
    Result takeImage(cv::Mat &img);

    MLBinoBusinessManage *getIInstrument();

    Result getBinocularInfo(BinocularInfo &info);

    void notifyPause(bool isPause);
    // continue or stop
    void continueRun(bool isContinue);

    Result getCaptureData(CaptureData &captureData);
    bool createResutCSVFile(QString cvFileName);

    //manual IQ
    Result calculateYImage(cv::Mat &imageY, QString lightColor = "g");
    Result calculateYImage(cv::Mat &imageY, CaptureData &rawData, QString lightColor = "g");
    Result calculateXYZImage(QMap<QString, cv::Mat> &imageXYZ, QMap<QString, CaptureData> &rawDataMap,
                             QString lightColor = "g");

    double getExposureTime(bool judge = false);
    ML::CameraV2::BinningMode getBinningMode();
    int getBinning();
    QString getNDFilter();
    QString getXYZFilter();

    Result setCameraMotion(int pos);

    Result imageRotation(cv::Mat matSrc, cv::Mat &matDest);

    Result getRotationMirror(float& rotation, CORE::Mirror& mirror);
    Result getRotationMirror(CORE::Rotation &rotation, CORE::Mirror &mirror);
    Result getRotationMirror(int &angle, MLImageDetection::MirrorALG &mirror);

    MLMonoBusinessManage *getMonocular();

    Result setCylinder(QString astigma, int degree, bool judge = false);
    Result setCylinderFilter(QString astigma);
    Result setRxRotation(int degree);
    Result getCylinder(QString &astigma, int &degree);
    Result clearAlarmFilter();

    QString getMcSn();
    ML::MLColorimeter::FOVCrop getFOVCrop();
    cv::Point2f getFOVCenter();

    float getCurrentDiopters(QString color = "");
    float calPiByVid(float VID);
    float calVidByPi(float piCoord);
    QString calCurrentVidByPi();
    float calPiByDiopters(float diopters);
    float calDioptersByPi(float piCoord);

    Result updateFocusByColor();
    Result updateFocusByColor(QString color);
    float getBestFocus(QString color = "");
    bool getSyncBestFocus();
    void setSyncBestFocus(bool sync, bool isUpdateDiopter = true);

    Result updateOutputImageName();

    Result loadPatternNameFile();

    ML::MLColorimeter::MLColorimeterAlgorithms *getProcess();

    QString getProcessPath();

  private:
    McBinocularMode(QObject *parent = nullptr);
    Result captureImage();

    ML::MLFilterWheel::MLFilterEnum ndFilterToEnum(QString filter);
    ML::MLFilterWheel::MLFilterEnum xyzFilterToEnum(QString filter);
    QString ndFilterToStr(ML::MLFilterWheel::MLFilterEnum filter);
    QString xyzFilterToStr(ML::MLFilterWheel::MLFilterEnum filter);

    Result saveImage(ImageNameInfo imageInfo, const cv::Mat &image, QString fileName);
    Result saveRawImg(bool isSave, QString &imgName, const CaptureData &data);

    Result waitPause(bool isPause);

    Result saveImageInfo(const QString &imgName, const ML::MLColorimeter::CaptureData &data, Result autoExpInfo);

  public:
    Result saveCalibrationImage(QString fileName, const std::map<MLFilterEnum, CaptureData>& dataMap,
        bool luminanceXYZ = true, bool saveSLBYY = false, bool luminanceSave = false, int imgSaveBit = -1);
    Result calibration_YY(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap, 
        std::map<CalibrationEnum, std::map<MLFilterEnum, CaptureData>> &calibrationDatas,
                          QString fileName, bool saveImage = true, int imgSaveBit = -1);
    Result calibration_XYZ(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap, 
        std::map<ML::MLColorimeter::CalibrationEnum,
        std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>> &calibrationDatas,
        QString fileName, bool saveSLBYY = false, int imgSaveBit = -1);
    Result calibration_xyY(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap,
        std::map<ML::MLColorimeter::CalibrationEnum,
                 std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>> &calibrationDatas,
        QString fileName, bool saveSLBYY = false);

    Result calibration_YY(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap, 
        CalibrationConfig config,
        std::map<CalibrationEnum, std::map<MLFilterEnum, CaptureData>>&calibrationDatas,
        QString fileName, QString fileDir, QString calibrationDir, ML::MLColorimeter::MLColorimeterAlgorithms* process = nullptr, int imgSaveBit = -1);
    Result calibration_XYZ(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap, 
        CalibrationConfig config,
        std::map<ML::MLColorimeter::CalibrationEnum,
        std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>> &calibrationDatas,
        QString fileName, QString fileDir, QString calibrationDir, bool saveSLBYY = false, ML::MLColorimeter::MLColorimeterAlgorithms* process = nullptr, int imgSaveBit = -1);
    Result calibration_xyY(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap, 
        CalibrationConfig config,
        std::map<ML::MLColorimeter::CalibrationEnum,
        std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>> &calibrationDatas,
        QString fileName, bool saveSLBYY = false, ML::MLColorimeter::MLColorimeterAlgorithms* process = nullptr);
    
    Result calibration(CalibrationConfig config, std::map<ML::MLColorimeter::CalibrationEnum, std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>> &calibrationDatas,
        ML::MLColorimeter::MLColorimeterAlgorithms* process = nullptr);
    Result calibrationRotation(std::map<ML::MLColorimeter::CalibrationEnum,
        std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>> &calibrationDatas);
    Result distortionCompensation(std::map<ML::MLColorimeter::CalibrationEnum,
        std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>> &calibrationDatas);
    Result setDisCompMap();

    Result captureXYZFilterImages(std::map<MLFilterEnum, CaptureData> &xyzData, Result &exposeRet, QString fileName,
                                  XyzExposureCache aeCache, bool saveRawImage, bool luminanceSave = false);

    Result checkLightSourceLC();

 private:
    Result saveXYZImage(std::map<ML::MLColorimeter::CalibrationEnum,
        std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>> calibrationDatas,
        QString fileName, QString fileDir, QString calibrationDir, bool saveSLBYY = false, int imgSaveBit = -1);
    Result savexyImage(std::map<CalibrationEnum, std::map<MLFilterEnum, CaptureData>> calibrationDatas,
                       QString fileName);

    cv::Mat postporce(cv::Mat mat, CORE::Rotation rotate, CORE::Mirror mirror);

    Result initAfterConnect();
    Result initAfterConnect_1();
    Result initAfterConnect_2();

    bool isEnabled(std::string key);
    Result clearAlarmRx();

    Result loadFile();

    // new image name 2025.3.7
    // imgType=0: raw image; imgType=1: YY image; 
    // imgType=2: XYZ image;imgType=3: xy image 
    // imgType=4: json file
    QString getImageName(int imgType, QString oldName = "", QString lumChroma = "");
    Result saveImageJson(QString filePath);
    QString reticleNameNew(QString name);

    QString pixelFormatToString(ML::CameraV2::MLPixelFormat pixelFormat);
    Result renameExistFile(QString& filePath);

    Result saveISluminance(QString XYZFilter);
    Result saveImagelumCsv(cv::Mat image);

    Result judgeExposure(const CaptureData &data, const QString &fileName);

    Result SetMLExposureAuto(int initTime, bool manual = true);

    int updateCylindricalRotationOffset(int angle);
    Result moveOtherCylinder(QString astigmaCurrent, bool& existPre);

    void compensationExposure(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> &xyzData);

    Result getCameraParam(McCameraParam &cameraParam);

  signals:
    void connectStatus(bool connected);
    void initColorimeter();
    void stopGrab(bool isStopGrab);
    void updateCameraImageSignal(const cv::Mat &image);
    void updateCameraGrayLevelSignal(int gray);

  private:
    static McBinocularMode *self;
    ML::MLColorimeter::MLBinoBusinessManage *m_bino = nullptr;
    ML::MLColorimeter::MLColorimeterAlgorithms *m_process = nullptr;
    bool m_isConnected = false;

    QWaitCondition m_waitCondition;
    QMutex m_mutex;
    bool m_isStop = false;
    bool m_recipePause = false;
 
    const QString CONFIG_PATH = ".\\config\\measureCamera";
    QString IMAGE_FILE_EXT = ".tif";

    const string ND_FILTER_KEY = "NDFilter";
    const string XYZ_FILTER_KEY = "ColorFilter";
    const string FOCUS_KEY = "CameraMotion";
    const string CAMERA_KEY = "VieCamera1";
    const string RX_KEY = "RXFilter";

    // key:a,b,c
    QMap<QString, float> m_vidFormulaMap;
    float m_compensationET = 0.0f; //23.5ms
    bool m_syncBestFocus = false;

    //lumin-chrom check R--lumin,chromx,chromy
    QMap<QString, std::array<float, 3>> m_LuminChromCig;
    float luminBiasPerCent = 0.0f;
    float chromXBias = 0.0f;
    float chromYBias = 0.0f;

    QMap<QString, QString> m_oldNewImageMap;
    Json m_patternJson;

    bool m_calibrationAsync = true;
    bool m_isColorCamera = false;
    // distortion compensation Map: "DUT_LeftEye", binning, X/Y, map
    QMap<QString, QMap<int, QMap<QString, cv::Mat>>> m_disCompMap;

    //CaptureData m_cameraParam;
    McCameraParam m_cameraParam;
    const float ZERO_JUDGE = 1e-4;
};
