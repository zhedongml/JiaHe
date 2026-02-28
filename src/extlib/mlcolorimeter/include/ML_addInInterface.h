// Copyright 2023 MLOptic
#pragma once
// #include "ml_addinmanger_global.h"
#include <qobject.h>
#include <map>
#include <list>
#include <string>
#include <opencv2/opencv.hpp>
#include "pluginException.h"

typedef enum PluginType {
    typeDataIO,          // !< base type for data input and output
                         // (cameras, AD-converter,
                         // display windows...)
    typeActuator,        /*!< base type for actuators and motors */
    typeAlgo,            /*!< base type for algorithm plugin */
    typeDataIO_Grabber,  // !< subtype of dataIO for
                         // cameras (grabbers), use this
                         //  type in combination with typeDataIO
                         //  (OR-combination)
    typeDataIO_ADDA,  // !< subtype of dataIO for AD and DA-converters, use this
                      // type in combination with typeDataIO (OR-combination)
    typeDataIO_RawIO,  // !< subtype of dataIO for further input-output-devices
                       //  (like display windows), use this type in combination
                       //  with typeDataIO (OR-combination)
    typeFilterWheel,   // base type for NDFilter and XYZFilter
    typeRxWheel,       // base type for Rx Filter
    typeReflector,     // base type for reflector
    typeSpectrometer,  // base type for spectrometer
    typeSpecbos,       // base type for specbos
} pluginType;

/*
 *   Plugin information, including version, type, id, author, etc. of the plugin
 */
typedef struct PluginInfo {
    pluginType plugin_type;   // plugin type
    std::string version;      // pugin version
    std::string author;       // plugin author
    std::string description;  // a brief description of the plugin
    std::string licenseInfo;  // a detailed description of the plugin license
    std::string pluginName;   // pugin name
    int pluginID;             // plugin id
} pluginInfo;

/*
 * Information about the plugin instance, including the ID and name of the
 * plugin instance A plugin can have multiple plugin instances
 */
typedef struct PluginInstanceInfo {
    int pluginInstanceID;
    std::string pluginInstanceName;
} pluginInstanceInfo;

typedef enum PluginName {
    filterWheel,
    rxFilterWheel,
    motor,
    camera,
    reflector,
    spectrometer,
    specbos
} pluginName;

// device infomation
typedef struct DeviceInfo {
    std::string name;
    std::string sn;
    std::string type;
} deviceInfo;

class ADDINMANGER_API ML_addInBase;
class ADDINMANGER_API ML_addInInterfaceBase : public QObject {
    Q_OBJECT

 public:
    ML_addInInterfaceBase();
    ~ML_addInInterfaceBase();

 public:
    // get Plugin Info
    pluginInfo getPluginInfo(void);

    // set plugin ID
    void setPluginID(int pluginID);

    //! returns the plugin path
    std::string getPluginPath(void);

    // set plugin path
    void setPluginPath(std::string pluginPath);

    // create plugin instance
    virtual ML_addInBase* createPluginInstance(
        std::string pluginInstanceName = "") = 0;

    // close plugin instance
    virtual pluginException closePluginInstance(
        ML_addInBase* pluginInstance) = 0;

    // get plugin instance
    std::map<int, ML_addInBase*> getPluginInstance(
        std::string pluginInstanceName = "");
    ML_addInBase* getPluginInstance(int pluginInstanceID);

    // Obtain a list of base classes for plugin functionality
    std::list<ML_addInBase*> getAddInBaseList();

    // Is the plugin instance in a new thread
    bool isPluginInNewThread(ML_addInBase* base);

    /***********Future development***************/
    // GUI support
    // bool hasGuiSupport()
 protected:
    std::string m_pluginPath;
    pluginInfo m_pluginInfo;
    std::list<ML_addInBase*> pluginInstanceList;
};

// class QThread;
class ADDINMANGER_API ML_addInBase : public QObject {
    Q_OBJECT

 public:
    ML_addInBase();
    ~ML_addInBase();

 public:
    // sets the interface of this instance to base
    void setBasePlugin(ML_addInInterfaceBase* base);

    // returns the interface of this instance
    ML_addInInterfaceBase* getBasePlugin();

    // get plugin instance name
    std::string getPluginInstanceName();

    //! creates new thread for the class instance and moves this instance to the
    //! new thread
    pluginException MoveToThread(void);

    // is plugin instance in new thread
    bool isPluginInstanceInNewThread();

 protected:
    // std::string m_pInstanceName;

 private:
    QThread* m_thread = nullptr;
    ML_addInInterfaceBase* m_addInInterfaceBasePtr;
    bool m_isPluginInstanceInNewThread;
};

namespace ML {
namespace MLFilterWheel {
class MLNDFilterWheel;
class MLRXFilterWheel;
class MLFilterWheelCallback;
struct MLNDFilterConfiguation;
struct MLRXFilterConfiguation;
enum class MLFilterEnum;
enum class MLFilterStatus;
}  // namespace MLFilterWheel
}  // namespace ML
class ADDINMANGER_API FilterWheelBase : public ML_addInBase {
    Q_OBJECT

 public:
    FilterWheelBase();
    ~FilterWheelBase();

 public:
    virtual pluginException SetFilterWheelConfig(
        ML::MLFilterWheel::MLNDFilterConfiguation* config) = 0;

    virtual void Subscribe() = 0;

    virtual void UnSubscribe() = 0;

    virtual pluginException ML_OpenSerial(
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_OpenSerialSync() = 0;

    virtual pluginException ML_CloseSerial() = 0;

    virtual bool ML_IsOpen() = 0;

    virtual bool ML_IsMoving() = 0;

    virtual bool ML_IsAlarm() = 0;

    virtual pluginException ML_StopMove() = 0;

    virtual pluginException ML_ResetFilter(
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_ResetFilterSync() = 0;

    virtual pluginException ML_MoveByIndex(
        int index,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_MoveByIndexSync(int index) = 0;

    virtual pluginException ML_MoveByEnum(
        ML::MLFilterWheel::MLFilterEnum filter_enum,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_MoveByEnumSync(
        ML::MLFilterWheel::MLFilterEnum filter_enum) = 0;

    virtual pluginException ML_MoveByName(
        std::string name,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_MoveByNameSync(std::string name) = 0;

    virtual int ML_GetFilterIndex() = 0;

    virtual ML::MLFilterWheel::MLFilterEnum ML_GetFilterEnum() = 0;

    virtual std::string ML_GetFilterName() = 0;

    virtual int ML_GetCurrPulse() = 0;

    virtual ML::MLFilterWheel::MLFilterStatus ML_GetStatus() = 0;

    virtual pluginException ML_WaitForFinish(int timeout = 20000) = 0;

    virtual pluginException ML_WaitResetFinish(int timeout = 20000) = 0;

    virtual pluginException ML_ClearAlarm() = 0;

    virtual int ML_ReadAlarmCode() = 0;
};
class ADDINMANGER_API RxFilterWheelBase : public ML_addInBase {
    Q_OBJECT

 public:
    RxFilterWheelBase();
    ~RxFilterWheelBase();

 public:
    virtual pluginException SetFilterWheelConfig(
        ML::MLFilterWheel::MLRXFilterConfiguation* config) = 0;

    virtual void Subscribe() = 0;

    virtual void UnSubscribe() = 0;

    virtual pluginException ML_OpenSerial(
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_OpenSerialSync() = 0;

    virtual pluginException ML_CloseSerial() = 0;

    virtual bool ML_IsOpen() = 0;

    virtual bool ML_IsMoving(bool filter = true, bool axis = true) = 0;

    virtual bool ML_IsAlarm() = 0;

    virtual pluginException ML_StopMove(bool filter = true,
                                        bool axis = true) = 0;

    virtual pluginException ML_ResetFilter(
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_ResetFilterSync() = 0;

    virtual pluginException ML_ResetAxis(
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_ResetAxisSync() = 0;

    virtual pluginException ML_MoveByIndex(
        int index,
        int axis,
        bool isResetAxis = false,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_MoveByIndexSync(int index,
                                               int axis,
                                               bool isResetAxis = false) = 0;

    virtual pluginException ML_MoveByName(
        std::string name,
        int axis,
        bool isResetAxis = false,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_MoveByNameSync(std::string name,
                                              int axis,
                                              bool isResetAxis = false) = 0;

    virtual pluginException ML_MoveByIndex(
        int index,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_MoveByIndexSync(int index) = 0;

    virtual pluginException ML_MoveByName(
        std::string name,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_MoveByNameSync(std::string name) = 0;

    virtual pluginException ML_MoveByAxis(
        int axis,
        int index,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_MoveByAxisSync(int axis, int index) = 0;

    virtual pluginException ML_MoveByPulse(
        int pulse,
        int index,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_MoveByPulseSync(int pulse, int index) = 0;

    virtual int ML_GetFilterIndex() = 0;

    virtual std::string ML_GetFilterName() = 0;

    virtual int ML_GetAxisDegree() = 0;

    virtual int ML_GetAxisDegree(int index) = 0;

    virtual int ML_GetCurrPulse() = 0;

    virtual int ML_GetAxisCurrPulse() = 0;

    virtual ML::MLFilterWheel::MLFilterStatus ML_GetStatus() = 0;

    virtual pluginException ML_WaitForFinish(int timeout = 30000,
                                             bool filter = true,
                                             bool axis = true) = 0;

    virtual pluginException ML_WaitResetFinish(int timeout = 20000) = 0;

    virtual pluginException ML_ClearAlarm() = 0;

    virtual int ML_ReadAlarmCode() = 0;

    virtual pluginException ML_ClearAxisAlarm() = 0;

    virtual int ML_ReadAxisAlarmCode() = 0;
};

namespace ML {
namespace Motion {
enum MLMotionState;
enum MLMotionEvent;
enum class ControllerType;
class MLMotionCallback;
struct MotorConfigInfo;
}  // namespace Motion
}  // namespace ML
class ADDINMANGER_API ActuatorBase : public ML_addInBase {
    Q_OBJECT

 public:
    ActuatorBase();
    ~ActuatorBase();

 public:
    // virtual pluginException InitialConfig(std::string configPath) = 0;
    virtual pluginException InitialPluginConfig(
        ML::Motion::MotorConfigInfo* motorConfig) = 0;

    virtual pluginException Connect() = 0;

    virtual pluginException Disconnect() = 0;

    virtual bool IsConnected() = 0;

    virtual pluginException SetEnable(bool enable) = 0;

    virtual bool IsEnabled() = 0;

    virtual pluginException MoveRelAsync(
        std::map<std::string, double> axispos) = 0;

    virtual pluginException MoveAbsAsync(
        std::map<std::string, double> axispos) = 0;

    virtual pluginException MoveRel(std::map<std::string, double> axispos) = 0;

    virtual pluginException MoveAbs(std::map<std::string, double> axispos) = 0;

    virtual pluginException StopMove() = 0;

    virtual pluginException Homing() = 0;

    virtual bool IsHome() = 0;

    virtual pluginException SetSpeed(double speedX) = 0;

    virtual double GetSpeed() = 0;

    virtual bool IsMoving() = 0;

    virtual pluginException GetPosition(
        std::map<std::string, double>* axispos) = 0;

    virtual ML::Motion::MLMotionState GetState() = 0;

    virtual pluginException Subscribe(
        ML::Motion::MLMotionEvent event,
        ML::Motion::MLMotionCallback* callback) = 0;

    virtual pluginException Unsubscribe(
        ML::Motion::MLMotionEvent event,
        ML::Motion::MLMotionCallback* callback) = 0;
};

namespace ML {
namespace AECommon {
struct AEParam;
}
namespace MLAutoExposure {
class AutoExposureAlgorithm;
}
namespace AECommon {
struct AEParam;
enum class GrayStatus;
}
namespace MLAutoExposure {
class AutoExposureAlgorithm;
}
namespace CameraV2 {
class MLCameraCallback;
struct CameraConfigInfo;
struct AEContainer;
enum class MLPixelFormat;
enum class AutoExposureStatus;
enum class MLExposureMode;
enum class MLCameraState;
enum class MLCameraEvent;
enum class BinningSelector;
enum class Binning;
enum class BinningMode;
enum class TriggerSource;
enum class TemperatureSelector;
}  // namespace CameraV2
}  // namespace ML
class ADDINMANGER_API GrabberBase : public ML_addInBase {
    Q_OBJECT

 public:
    GrabberBase();
    ~GrabberBase();

 public:
    virtual pluginException InitialPluginConfig(
        ML::CameraV2::CameraConfigInfo* cameraConfig) = 0;

    virtual pluginException InitializeAEConfig(
        const char* configfilePath,
        std::string cameraNameFromJson) = 0;

    virtual pluginException Open() = 0;

    virtual pluginException Close() = 0;

    virtual bool IsOpened() = 0;

    virtual pluginException GrabOne() = 0;

    virtual pluginException StartGrabbingAsync() = 0;

    virtual pluginException StopGrabbing() = 0;

    virtual bool IsGrabbing() = 0;

    virtual int GetWidth() = 0;

    virtual int GetHeight() = 0;

    virtual pluginException SetWidth(int width) = 0;

    virtual pluginException SetHeight(int height) = 0;

    virtual pluginException SetOffsetX(int offset) = 0;

    virtual pluginException SetOffsetY(int offset) = 0;

    virtual pluginException SetFrameRate(double rate) = 0;

    virtual double GetFrameRate() = 0;

    virtual pluginException SetFormatType(
        ML::CameraV2::MLPixelFormat format) = 0;

    virtual ML::CameraV2::MLPixelFormat GetFormatType() = 0;

    virtual void InitAEParams() = 0;

    virtual ML::AECommon::AEParam GetAEParams() = 0;

    virtual void UpdateAEParams(ML::AECommon::AEParam aeparam) = 0;

    virtual bool WriteAEParmstoJson() = 0;

    virtual void ResetAEParams() = 0;

    virtual ML::CameraV2::AEContainer SetMLExposureAuto() = 0;

    virtual pluginException SetExposureMode(
        ML::CameraV2::MLExposureMode mode) = 0;

    virtual ML::CameraV2::MLExposureMode GetExposureMode() = 0;

    virtual pluginException SetExposureTime(double time) = 0;

    virtual double GetExposureTime() = 0;

    virtual pluginException GetExposureTimeRange(double& min, double& max) = 0;

    virtual pluginException SetGain(double value) = 0;

    virtual double GetGain() = 0;

    virtual pluginException GetGainRange(double& min, double& max) = 0;

    virtual pluginException SetAdjustExposureTime(double time) = 0;

    virtual ML::CameraV2::MLCameraState GetState() = 0;

    virtual pluginException ReverseX(bool reverse) = 0;

    virtual pluginException ReverseY(bool reverse) = 0;

    virtual pluginException Subscribe(
        ML::CameraV2::MLCameraEvent event,
        ML::CameraV2::MLCameraCallback* callback) = 0;

    virtual pluginException Unsubscribe(
        ML::CameraV2::MLCameraEvent event,
        ML::CameraV2::MLCameraCallback* callback) = 0;

    virtual cv::Mat GetImage() = 0;

    virtual pluginException StartCalcGrayLevel() = 0;

    virtual pluginException StopCalcGrayLevel() = 0;

    virtual pluginException Rotate(bool rotate) = 0;

    virtual pluginException SetBinningSelector(
        ML::CameraV2::BinningSelector selector) = 0;

    virtual ML::CameraV2::BinningSelector GetBinningSelector() = 0;

    virtual pluginException SetBinning(ML::CameraV2::Binning binning) = 0;

    virtual pluginException SetBinningMode(
        ML::CameraV2::BinningMode binningMode) = 0;

    virtual ML::CameraV2::Binning GetBinning() = 0;

    virtual ML::CameraV2::BinningMode GetBinningMode() = 0;

    virtual pluginException SetTemperatureSelector(
        ML::CameraV2::TemperatureSelector selector) = 0;

    virtual ML::CameraV2::TemperatureSelector GetTemperatureSelector() = 0;

    virtual double GetDeviceTemperature() = 0;

    virtual pluginException SetTargetTemperature(double temperature) = 0;

    virtual double GetTargetTemperature() = 0;

    virtual pluginException SetTriggerSource(
        ML::CameraV2::TriggerSource source) = 0;

    virtual ML::AECommon::GrayStatus GetGrayStatus(cv::Mat& img) = 0;

    virtual std::string GetDeviceVersion() = 0;

    virtual std::string GetDeviceSN() = 0;

    virtual double GetExposureOffset() = 0;

 public:
    std::string serial_number_;
    std::string cameraNameFromJson;
    int init_Time_;
    bool isSetInitTime;
};

namespace ML {
namespace MLEleReflectorCommon {
class MLEleConfiguration;
enum class MLEleStatus;
enum class MLEleEnum;
};  // namespace MLEleReflectorCommon
};  // namespace ML
class ADDINMANGER_API ReflectorBase : public ML_addInBase {
    Q_OBJECT
 public:
    ReflectorBase();
    ~ReflectorBase();

 public:
    virtual pluginException ML_SetReflectorConfig(
        ML::MLEleReflectorCommon::MLEleConfiguration* config) = 0;

    virtual pluginException ML_OpenSerial() = 0;

    virtual pluginException ML_CloseSerial() = 0;

    virtual bool ML_IsOpen() = 0;

    virtual bool ML_IsMoving() = 0;

    virtual bool ML_IsAlarm() = 0;

    virtual pluginException ML_Home() = 0;

    virtual pluginException ML_MovebyEnum(
        ML::MLEleReflectorCommon::MLEleEnum reflector) = 0;

    virtual pluginException ML_RelMove(double pos) = 0;

    virtual pluginException ML_EStop() = 0;

    virtual pluginException ML_StopMove() = 0;

    virtual pluginException ML_ClearAlarm() = 0;

    virtual ML::MLEleReflectorCommon::MLEleStatus ML_GetStatus() = 0;

    virtual int ML_ReadAlarmCode() = 0;

    virtual double ML_GetPosition() = 0;
};

namespace ML {
namespace MLSpectrometer {
struct SpectrometerInfo;
struct MLCIE;
}  // namespace MLSpectrometer
}  // namespace ML
class ADDINMANGER_API SpectrometerBase : public ML_addInBase {
    Q_OBJECT
 public:
    SpectrometerBase();
    ~SpectrometerBase();

 public:
    virtual pluginException ML_SetSpectrometerConfig(
        ML::MLSpectrometer::SpectrometerInfo* info) = 0;

    virtual bool ML_Connect() = 0;

    virtual bool ML_Disconnect() = 0;

    virtual bool ML_Isconnected() = 0;

    virtual pluginException ML_InitSetting() = 0;

    virtual pluginException ML_SetIntegrationTime(int time = 200000) = 0;

    virtual int ML_GetIntegrationTime() = 0;

    virtual pluginException ML_AdjustIntegrationTime(int iteration = 15) = 0;

    virtual pluginException ML_CalculateCIE(std::string calibration_file,
                                            std::string constantX_file,
                                            std::string constantY_file,
                                            std::string constantZ_file) = 0;

    virtual ML::MLSpectrometer::MLCIE ML_GetCIE() = 0;

    virtual pluginException ML_CalculateSpectrum() = 0;

    virtual pluginException ML_SaveSpectrum(std::string path) = 0;

    virtual std::vector<double> ML_GetWavelength() = 0;

    virtual std::vector<double> ML_GetSpectrum() = 0;
};

namespace ML {
namespace MLSpecbos {
enum class DeviceType;
struct MeasurementResults;
struct CalibrationInfo;
}  // namespace MLSpecbos
}  // namespace ML
class ADDINMANGER_API SpecbosBase : public ML_addInBase {
    Q_OBJECT
 public:
    SpecbosBase();
    ~SpecbosBase();

 public:
    virtual bool ML_Open(std::string port) = 0;

    virtual bool ML_Close() = 0;

    virtual bool ML_Isopen() = 0;

    virtual bool ML_SetDeviceType(ML::MLSpecbos::DeviceType dtype) = 0;

    virtual pluginException ML_InitSetting(std::string ini_path) = 0;

    virtual pluginException ML_LoadCalibrationFile() = 0;

    virtual pluginException ML_SetCalibrationFileNumber(int bCalibNr) = 0;

    virtual pluginException ML_SetAverage(int average) = 0;

    virtual pluginException ML_SetSyncFreq(float freq) = 0;

    virtual pluginException ML_SetSyncMode(int sync_mode) = 0;

    virtual pluginException ML_SetAutoTintConf(float fMaxTint = 60000) = 0;

    virtual pluginException ML_SetFixedTintConf(float fTint) = 0;

    virtual float ML_GetTintConf() = 0;

    virtual pluginException ML_TakeMeasurement() = 0;

    virtual pluginException ML_TakeMeasurementAsy() = 0;

    virtual ML::MLSpecbos::MeasurementResults* ML_GetMeasurementResults() = 0;

    virtual ML::MLSpecbos::CalibrationInfo* ML_GetCalibrationInfo() = 0;
};

// Plug-in identifier
#define AbstractInterface_iid "ML addInInterfaceBase"
// Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(ML_addInInterfaceBase, AbstractInterface_iid)
