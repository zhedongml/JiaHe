#pragma once

#include <windows.h>
#include <QMap>
#include <QObject>
#include <opencv2\opencv.hpp>

#include "MLBinoBusinessManage.h"
#include "MLCamaraCommon.h"
#include "ML_addInInterface.h"
#include "pluginException.h"
#include "MLColorimeter.h"
#include "MLColorimeterConfig.h"
#include "MLColorimeterCommon.h"
#include "MLCameraParameters.h"
#include "MLColorimeterPlugin.h"
#include "MLTaskManager.h"

#include <QWaitCondition>

using namespace std;
using namespace ML::MLColorimeter;
using namespace ML::MLFilterWheel;
using namespace ML::CameraV2;
using namespace ML_Task;

class ML::MLColorimeter::MLMonoBusinessManage;

class MLColorimeterMode : public QObject
{
    Q_OBJECT

public:
    ~MLColorimeterMode();
    static MLColorimeterMode* Instance(QObject* parent = nullptr);
    Result Connect();
    Result ConnectAsync();
    Result InitConfig();
    Result Disconnect();
    bool IsConnect();

    GrabberBase* GetCamera();
    ActuatorBase* GetCameraMotion();
    MLBinoBusinessManage* GetBinocular();
    MLMonoBusinessManage* GetMonocular();
    ML_Task::MLTaskManager* GetCaptureTaskManager();

    ML::MLColorimeter::ModuleConfig GetModuleConfig();
    ML::MLColorimeter::MotionConfig GetCameraMotionConfig();

    // ================ND XYZ Filter==================//
    Result SetNDFilter(QString nd);
    Result SetColorFilter(QString xyz);
    Result SetNDFilterAsyn(QString nd);
    Result SetXYZFilterAsyn(QString xyz);
    Result SetNDFilter(MLFilterEnum nd);
    Result SetColorFilter(MLFilterEnum xyz);
    QString GetNDFilter();
    QString GetXYZFilter();
    MLFilterEnum GetNDFilterEnum();
    MLFilterEnum GetXYZFilterEnum();
    Result ClearAlarmFilter();
    Result ClearAlarmRx();

    // ================Cylinder Filter==================//
    Result SetRXSync(RXCombination rx);
    Result SetRXAsync(RXCombination rx);
    Result SetCylinder(QString cylinder, int axis);
    RXCombination GetRX();

    // ================Focus Motion==================//
    Result SetVidSync(double vid);
    Result SetVidAsync(double vid);
    Result SetSphereSync(double sphere);
    Result SetSphereAsync(double sphere);
    Result SetFocusMotionPosAsync(double position);
    Result SetFocusMotionPosSync(double position);
    Result SetFocusMotionPosRelAsync(double position);
    Result SetFocusMotionPosRelSync(double position);
    double GetVid();
    double GetFocusMotionPos();
	// x:sphere y:vid z:motion pos
    cv::Point3f TransVidToFocusMotionSph(double vid);
    cv::Point3f TransSphToFocusMotionVid(double sphere);
    cv::Point3f TransFocusMotionToSphVid(double position);

    // ================Measure Camera==================//
    bool IsGrabbing();
    Result StopGrabbing(bool stop);
    Result SetExposure(bool is_auto_et, double et, double et_factor);
    Result SetExposureTime(double ms);
    Result SetExposureTime(ExposureSetting exposure);
    Result TransExposureSetting(ExposureMode mode, double et_factor, ExposureSetting& setting);
    Result SetBitDepth(int bitDepth);
    Result SetBitDepth(ML::CameraV2::MLPixelFormat bitDepth);
    Result SetBinning(int binning);
    Result SetBinning(ML::CameraV2::Binning binning);
    Result SetBinningMode(const QString& binningMode);
    Result SetBinningMode(ML::CameraV2::BinningMode binningMode);
    Result SetBinningSelector(ML::CameraV2::BinningSelector binningSelector);
    double GetExposureTime();
    int GetBinning();
    std::string GetBitDepth();
    ML::CameraV2::BinningMode GetBinningMode();
    Result GetImage(cv::Mat& image);

    QString GetProcessPath();

    // ================Through Focus==================//
    //DiopterScan
    //Result DiopterScanProcess(DiopterScanConfig config, std::string path = "", std::string prefix = "");
    //double GetDiopterScanHResult();
    //double GetDiopterScanVResult();
    //double GetDiopterScanAVGResult();
    // CameraMotion Scan
    Result MotionScanProcess(FocusScanConfig config, std::string path);
    double GetMotionScanHResult();
    double GetMotionScanVResult();
    double GetMotionScanAVGResult();
    void StopThroughFocusProcess();


    //Capture Image
    Result LoadCalibrationData(const CalibrationConfig2& c_config, const CalibrationFlag& c_flag);

    // ================Other Methods==================//
    cv::Mat Flip_Rotation(cv::Mat srcImg);
    ML::CameraV2::Binning TransIntToBinning(int bin);
    bool isDirExist(QString fullPath);
    bool UpdateFlipRotation(Flipping flip_x = Flipping::NonReverse, Flipping flip_y = Flipping::NonReverse, ML::MLColorimeter::Rotation rotate = ML::MLColorimeter::Rotation::R0);
    void parseConfigPathPointer(const QString& filePath);

    //Fix Exposure
    Result GetFixExposureTime(std::string name, std::string eyebox, double& time);
    Result UpdateFixExposureTimeConfig(const QString& inputFilePath, bool isReplace = true);
    bool ReadFixExposureTime();
    QString AnalyzeImageName(QString imageNameRule, QString nd, QString lightSource, QString imageType, QString eyeboxID, QString colorFilter);
    Result ClearFixExposureTimeConfig();
    bool DeleteFixExposureTimeCsv();
    void SetFixExposureTimeMatchRule(bool isMatchEB);

private:
    Result initAfterConnect();
    bool isEnabled(std::string key);

private:
    static MLColorimeterMode* self;
    MLColorimeterMode(QObject* parent = nullptr);
    ML::MLColorimeter::MLBinoBusinessManage* m_bino = nullptr;
    ML_Task::MLTaskManager* m_taskManager = nullptr;

    QWaitCondition m_waitCondition;
    QMutex m_mutex;

    bool m_IsConnected = false;
    const string ND_FILTER_KEY = "NDFilter";
    const string XYZ_FILTER_KEY = "ColorFilter";
    const string FOCUS_KEY = "CameraMotion";
    const string CAMERA_KEY = "VieCamera1";
    const string RX_KEY = "RXFilter";
    QString CONFIG_PATH = ".\\config\\mlcolorimeter";
    const string FIXEXPOSURE_FILE = ".\\config\\exposureTime\\ET.csv";

    QString IMAGE_FILE_EXT = ".tif";

    std::vector<std::tuple<std::string, std::string, double>> m_FixExposureTupleVec;
    std::map<std::string, double> m_FixExposureMap;
    bool m_isConfigPtr = false;
    QString m_PointConfigPath;
    bool m_isMatchEB = false;

signals:
    void connectStatus(bool connected);
    void stopGrab(bool isStopGrab);
    void updateCameraImageSignal(const cv::Mat& image);
    void updateCameraGrayLevelSignal(int gray);
};
