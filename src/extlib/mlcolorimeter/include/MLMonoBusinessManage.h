// Copyright 2024 MLOptic

#ifndef SRC_MLCOLORIMETER_MLMONOBUSINESSMANAGE_H_
#define SRC_MLCOLORIMETER_MLMONOBUSINESSMANAGE_H_

#include <QtCore/qlist.h>
#include <QtCore/qmap.h>
#include <QtCore/qmutex.h>
#include <QtCore/qobject.h>

#include "AECommon.h"
#include "MLCamaraCommon.h"
#include "MLColorimeterCommon.h"
#include "MLSpecbosCommon.h"
#include "MLSpectrometerCommon.h"
#include "Result.h"
#include "mlcolorimeter_global.h"

/**
 * @file MLMonoBusinessManage.h
 * @brief Defines the monocular module business logic class and associated functionality.
 */

class ML_addInBase;

namespace ML {
namespace MLFilterWheel {
class MLFilterWheelCallback;
}  // namespace MLFilterWheel

namespace MLColorimeter {
class MLColorimeterConfig;

/**
 * @class MLMonoBusinessManage
 * @brief Manages the hardware control and business logic for monocular module.
 *
 * The class provides a common interface to manage business logic and hardware control
 * including NDFilter, ColorFilter, RXFilter, Motor and Camera.
 */
class MLCOLORIMETER_EXPORT MLMonoBusinessManage : public QObject {
    Q_OBJECT

 public:
    MLMonoBusinessManage(QObject* parent = nullptr);
    ~MLMonoBusinessManage();

    /**
     * @brief Initialize config profile of MLMonoBusinessManage.
     * @param path  path of config profile.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetConfigPath(const char* path);

    /**
     * @brief Get module config path.
     * @return module config path.
     */
    virtual std::string ML_GetConfigPath();

    /**
     * @brief Create a MLMonoBusinessManage system.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_CreateModule();

    /**
     * @brief Connect the modules created by ML_CreateModule().
     * @return the result contains the message, code and status.
     */
    virtual Result ML_ConnectModule();

    /**
     * @brief DisConnect the module created by ML_CreateModule().
     * @return the result contains the message, code and status.
     */
    virtual Result ML_DisconnectModule();

    /**
     * @brief Destroy the module created by ML_CreateModule().
     * @return the result contains the message, code and status.
     */
    virtual Result ML_DestroyModule();

    /**
     * @brief Check if all members in the module created by ML_CreateModule() are
     * connected.
     * @return true if all are connected, otherwise false.
     */
    virtual bool ML_IsModuleConnect();

    /**
     * @brief Check if all members in the module created by ML_CreateModule() are moving.
     * @return true if one is moving, otherwise false.
     */
    virtual bool ML_IsModuleMotorsMoving();

    /**
     * @brief Wait for all members' movement finish in the module created by
     * ML_CreateModule().
     * @param timeout  time out limit (unit: millisecond).
     * @return the result contains the message, code and status.
     */
    virtual Result ML_WaitForMovingStop(int timeout = 10000);

    /**
     * @brief Wait for nd and xyz filterwheels movement finish.
     * @param timeout  time out limit (unit: millisecond).
     * @return the result contains the message, code and status.
     */
    virtual Result ML_WaitFilterStop(int timeout = 10000);

    /**
     * @brief Wait for rx filterwheels movement finish.
     * @param timeout  time out limit (unit: millisecond).
     * @return the result contains the message, code and status.
     */
    virtual Result ML_WaitRXFilterStop(int timeout = 10000);

    /**
     * @brief Wait for reflector movement finish.
     * @param timeout  time out limit (unit: millisecond).
     * @return the result contains the message, code and status.
     */
    virtual Result ML_WaitReflectorStop(int timeout = 10000);

    /**
     * @brief Wait for focus motions movement finish.
     * @param timeout  time out limit (unit: millisecond).
     * @return the result contains the message, code and status.
     */
    virtual Result ML_WaitFocusMotionStop(int timeout = 10000);

    /**
     * @brief Stop all movements of module.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_StopModuleMovement();

    /**
     * @brief Get the serial number of the module.
     * @return module's serial number.
     */
    virtual std::string ML_GetModuleSerialNumber();

    /**
     * @brief Get the name of the module.
     * @return  module's name.
     */
    virtual std::string ML_GetModuleName();

    /**
     * @brief Obtain the id of the module.
     * @return module's id.
     */
    virtual int ML_GetModuleID();

    /**
     * @brief Obtain the eyemode of the module.
     * @return module's eyemode.
     */
    virtual ML::MLColorimeter::EyeMode ML_GetModuleEyeMode();

    // Camera Interface

    /**
     * @brief  Check if the calibration file cameraSN matches the SN read by the camera
     */
    virtual Result ML_CheckCameraSN();

    /**
     * @brief  Init AutoExposure params.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_InitAEParams();

    /**
     * @brief  Get AutoExposure params.
     */
    virtual ML::AECommon::AEParam ML_GetAEParams();

    /**
     * @brief  Update AutoExposure params.
     * @param aeParams  AutoExposure parameters setting
     * @return the result contains the message, code and status.
     */
    virtual Result ML_UpdateAEParams(ML::AECommon::AEParam aeParams);

    /**
     * @brief  Write AutoExposure params to json file.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_WriteAEParamstoJson();

    /**
     * @brief  Reset AutoExposure params to initial params (read from json).
     * @return the result contains the message, code and status.
     */
    virtual Result ML_ResetAEParams();

    /**
     * @brief  Set exposure for camera, contain auto exposure and fixed exposure.
     * @param exposure  exposure setting, include exposure mode and exposure time
     * (unit: millisecond)
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetExposure(ExposureSetting exposure);

    /**
     * @brief  get auto exposure status.
     * @return
     */
    virtual ML::CameraV2::AutoExposureStatus ML_GetAutoExposureStatus();

    /**
     * @brief  Get the exposure time from camera.
     * @return the exposure time of camera.
     */
    virtual double ML_GetExposureTime();

    /**
     * @brief  selects the logic or sensor binning engine features.
     * @param binningSelector  binning selector, logic or sensor
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetBinningSelector(ML::CameraV2::BinningSelector binningSelector);

    /**
     * @brief  get the binning selector
     * @return ML::CameraV2::BinningSelector
     */
    virtual ML::CameraV2::BinningSelector ML_GetBinningSelector();

    /**
     * @brief  Number of vertical and horizontal photo-sensitive cells to combine
     together. This increases the intensity (or signal to noise ratio) of the pixels and
     reduces the vertical resolution (height) and horizontal resolution (width) of the
     image.
     * @param binning  binning to set.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetBinning(ML::CameraV2::Binning binning);

    /**
     * @brief  Get the binning from camera.
     * @return the binning of camera.
     */
    virtual ML::CameraV2::Binning ML_GetBinning();

    /**
     * @brief  Sets the mode to use to combine photo-sensitive cells together.
     * @param binningMode  binning mode to set.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetBinningMode(ML::CameraV2::BinningMode binningMode);

    /**
     * @brief  Get the binning mode from camera.
     * @return the binning mode of camera.
     */
    virtual ML::CameraV2::BinningMode ML_GetBinningMode();

    /**
     * @brief  Format of the pixel to use for acquisition.
     * @param pixelFormat  pixel format to set.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetPixelFormat(ML::CameraV2::MLPixelFormat pixelFormat);

    /**
     * @brief  Get the pixel format from camera.
     * @return the pixel format of camera.
     */
    virtual ML::CameraV2::MLPixelFormat ML_GetPixelFormat();

    /**
     * @brief  Flip horizontally the image sent by the device. The Region of interest is applied after the flipping.
     * @param reverse  flag to reverse
     * @return the result contains the message, code and status.
     */
    virtual Result ML_ReverseX(bool reverse);

    /**
     * @brief  Flip vertically the image sent by the device. The Region of interest is applied after the flipping.
     * @param reverse  flag to reverse
     * @return the result contains the message, code and status.
     */
    virtual Result ML_ReverseY(bool reverse);


    /**
     * @brief  Capture single image asynchronously.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_CaptureImageAsync();

    /**
     * @brief  Capture single image synchronously.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_CaptureImageSync();

    /**
     * @brief  flag of getting image from auto exposure.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_GetAEImage();

    /**
     * @brief  Get an image after calling ML_CaptureImageAsync() or ML_CaptureImageSync().
     * @return a signle channel image for mono camera, or a three-channel image
     * for color camera.
     */
    virtual cv::Mat ML_GetImage();

    /**
     * @brief  calculate image gray status
     * @param img  image to calculate gray status
     */
    virtual ML::AECommon::GrayStatus ML_CalGrayStatus(cv::Mat& img);

    /**
     * @brief  img after software binning.
     * @param img  input image
     * @param bin  software binning setting
     * @param binmode  software binning mode setting
     * @return the result contains the message, code and status.
     */
    virtual cv::Mat ML_SetSoftwareBinning(cv::Mat img, ML::CameraV2::Binning bin, ML::CameraV2::BinningMode binmode);

    /**
     * @brief  Selects the location within the device, where the temperature will be
     * measured.
     * @param selector  device temperature selector to set.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetTemperatureSelector(ML::CameraV2::TemperatureSelector selector);

    /**
     * @brief  Get the device temperature selector from camera.
     * @return ML::CameraV2::TemperatureSelector
     */
    virtual ML::CameraV2::TemperatureSelector ML_GetTemperatureSelector();

    /**
     * @brief  Device temperature in degrees Celsius (C). It is measured at the location
     * selected by ML_SetTemperatureSelector().
     * @return device temperature
     */
    virtual double ML_GetDeviceTemperature();

    /**
     * @brief  Set reference temperature (in degrees Celsius) for cooling operation.
     * @param temperature  target temperature to set.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetTargetTemperature(double temperature);

    /**
     * @brief  Get reference temperature (in degrees Celsius) for cooling operation from
     * camera.
     * @return device temperature
     */
    virtual double ML_GetTargetTemperature();

    /**
     * @brief  Controls the acquisition rate (in Hertz) at which the frames are captured.
     * @param frame_rate  frame rate to set.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetAcquisitionFrameRate(double frame_rate);

    /**
     * @brief  Get the acquisition rate (in Hertz) from camera.
     * @return device temperature
     */
    virtual double ML_GetAcquisitionFrameRate();

    /**
     * @brief  Controls the selected gain as an absolute physical value. This is an
     * amplification factor applied to the video signal.
     * @param gain  digital gain to set.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetCameraGain(double gain);

    /**
     * @brief  Get the digital gain from camera.
     * @return digital gain
     */
    virtual double ML_GetCameraGain();
    
    /**
     * @brief  capture High Dynamic Range image
     * @param maxExposureTime  max exposure setting during HDR
     * @param thresholdArray  gray value threshold array
     * @param thresholdPercent  pixel number threshold precent
     * @return cv::Mat
     */
    virtual cv::Mat ML_CaptureHDRImage(double maxExposureTime = 5000, std::vector<int> thresholdArray = {2919, 1946, 973},
                                       double thresholdPercent = 0.00025);

    // Filter Interface

    /**
     * @brief  Switch ND/XYZ filter wheel's channel by enum asynchronously.
     * @param keyName  the key name of FilterWheel, from the config.
     * @param channel  the enum of channel to switch
     * @param cb  callback function
     * @return the result contains the message, code and status.
     */
    virtual Result ML_MoveND_XYZFilterByEnumAsync(std::string keyName,
                                                  ML::MLFilterWheel::MLFilterEnum channel,
                                                  ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr);

    /**
     * @brief  Switch ND/XYZ filter wheel's channel by enum synchronously.
     * @param keyName  the key name of FilterWheel, from the config.
     * @param channel  the enum of channel to switch
     * @return the result contains the message, code and status.
     */
    virtual Result ML_MoveND_XYZFilterByEnumSync(std::string keyName,
                                                 ML::MLFilterWheel::MLFilterEnum channel);

    /**
     * @brief  Get the channel of ND/XYZ FilterWheel.
     * @param keyName  the key name of FilterWheel, from the config.
     * @return the channel enum of ND/XYZ FilterWheel.
     */
    virtual ML::MLFilterWheel::MLFilterEnum ML_GetND_XYZFilterChannel(std::string keyName);

    /**
     * @brief  Clear Alarm for ND and XYZ FilterWheel.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_ND_XYZFilterClearAlarm();

    /**
     * @brief  Switch RX filter wheel's channel , and change RX axis degree
     * asynchronously.
     * @param channel  the channel to switch
     * @param degree  the degree of axis to change
     * @param isResetAxis  whether reset axis to origin before moving to target position
     * @param cb  callback function
     * @return the result contains the message, code and status.
     */
    virtual Result ML_MoveRXFilterByNameAsync(std::string channel, int degree, bool isResetAxis = false,
                                              ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr);

    /**
     * @brief  Switch RX filter wheel's channel, and change RX axis degree
     * synchronously.
     * @param channel  the channel to switch
     * @param degree  the degree of axis to change
     * @param isResetAxis  whether reset axis to origin before moving to target position
     * @return the result contains the message, code and status.
     */
    virtual Result ML_MoveRXFilterByNameSync(std::string channel, int degree, bool isResetAxis = false);

    /**
     * @brief  Get the channel of RXFilterWheel.
     * @return RXFilter's channel
     */
    virtual std::string ML_GetRXFilterChannel();

    /**
     * @brief  Get the degree of RXFilterWheel's axis.
     * @return RXFilter's axis degree
     */
    virtual int ML_GetRXFilterAxis();

    /**
     * @brief  Clear Alarm for RXFilter and RX Axis.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_RXFilterClearAlarm();

    /**
     * @brief  Set Spherical diopter asynchronously.
     * @param sphere  sphere to set.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetSphericalAsync(double sphere);

    /**
     * @brief  Set Spherical diopter synchronously.
     * @param sphere  sphere to set.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetSphericalSync(double sphere);

    /**
     * @brief  Get Spherical diopter.
     * @return spherical power.
     */
    virtual double ML_GetSpherical();

    /**
     * @brief  Set RX asynchronously.
     * @param rx  RXCombination setting containing spherical power, cylindrical power and
     * cylindrical axis.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetRXAsync(ML::MLColorimeter::RXCombination rx);

    /**
     * @brief  Set RX synchronously.
     * @param rx  RXCombination setting containing spherical power, cylindrical power and
     * cylindrical axis.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetRXSync(ML::MLColorimeter::RXCombination rx);

    /**
     * @brief  Get RX.
     * @return RXCombination containing spherical power,
     * cylindrical power and cylindrical axis.
     */
    virtual ML::MLColorimeter::RXCombination ML_GetRX();

    // Focus Interface

    /**
     * @brief  focus motion absolute movement asynchronously.
     * @param keyName  the key name of Motion, from the config.
     * @param pos  absolute motion position to set (unit: millimeter).
     * @return the result contains the message, code and status
     */
    virtual Result ML_SetPositionAbsAsync(std::string keyName, double pos);

    /**
     * @brief  focus motion absolute movement synchronously.
     * @param keyName  the key name of Motion, from the config.
     * @param pos  absolute motion position to set (unit: millimeter).
     * @return the result contains the message, code and status
     */
    virtual Result ML_SetPositionAbsSync(std::string keyName, double pos);

    /**
     * @brief  focus motion relative movement asynchronously.
     * @param keyName  the key name of Motion, from the config.
     * @param pos  relative motion position to set (unit: millimeter).
     * @return the result contains the message, code and status
     */
    virtual Result ML_SetPositionRelAsync(std::string keyName, double pos);

    /**
     * @brief  focus motion relative movement synchronously.
     * @param keyName  the key name of Motion, from the config.
     * @param pos  relative motion position to set (unit: millimeter).
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetPositionRelSync(std::string keyName, double pos);

    /**
     * @brief  Get focus motion position (unit: millimeter).
     * @param keyName  the key name of Motion, from the config.
     * @return position of keyName motion (unit: millimeter).
     */
    virtual double ML_GetMotionPosition(std::string keyName);

    /**
     * @brief  Stop focus motion movement.
     * @param keyName  the key name of Motion, from the config.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_StopMotionMovement(std::string keyName);

    /**
     * @brief  Set focus by Virtual Image Distance asynchronously.
     * @param vid  Virtual Image Distance setting (unit: millimeter)
     * @param method  focus method (default: Inverse)
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetVIDAsync(double vid, ML::MLColorimeter::FocusMethod method = ML::MLColorimeter::FocusMethod::Inverse);

    /**
     * @brief  Set focus by Virtual Image Distance synchronously.
     * @param vid  Virtual Image Distance setting (unit: millimeter)
     * @param method  focus method (default: Inverse)
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetVIDSync(double vid, ML::MLColorimeter::FocusMethod method = ML::MLColorimeter::FocusMethod::Inverse);

    /**
     * @brief  Get Virtual Image Distance (unit: millimeter).
     * @param method  focus method (default: Inverse)
     * @return vid of focus
     */
    virtual double ML_GetVID(ML::MLColorimeter::FocusMethod method = ML::MLColorimeter::FocusMethod::Inverse);

    virtual Result ML_UpdateInfinityPosition(std::string keyStr);

    /**
     * @brief  Perform through focus (contains coarse and fine adjustment) and return the
     * vid and position on best mtf.
     * @param keyName  the key name of Motion to perform through focus, from the
     * MotionConfig.json
     * @param focusconfig  through focus config, default from FocusScanConfig.json
     * @return the result contains the message, code and status.
     */
    virtual Result ML_FocusScan(std::string keyName,
                                ML::MLColorimeter::FocusScanConfig focusconfig = ML::MLColorimeter::FocusScanConfig());

    /**
     * @brief  Initial through focus config and parameters setting.
     * @param keyName  the key name of Motion to perform through focus, from the
     * MotionConfig.json
     * @param focusconfig  through focus config, default from FocusScanConfig.json
     * @return the result contains the message, code and status.
     */
    virtual Result ML_InitFocusScanConfig(std::string keyName,
                                          ML::MLColorimeter::FocusScanConfig focusconfig = ML::MLColorimeter::FocusScanConfig());

    /**
     * @brief  Get through focus config after initialization.
     */
    virtual ML::MLColorimeter::FocusScanConfig ML_GetFocusScanConfig();

    /**
     * @brief  Perform through focus coarse adjustment.
     * @param keyName  the key name of Motion to perform through focus, from the
     * MotionConfig.json
     * @param focusconfig  through focus config, default from FocusScanConfig.json
     * @param useMeanStdDev use MeanStdDev func to calculate the mean and the standard
     * deviation M of the image
     * @return the result contains the message, code and status.
     */
    virtual Result ML_Coarse_FocusScan(std::string keyName,
                                       ML::MLColorimeter::FocusScanConfig focusconfig = ML::MLColorimeter::FocusScanConfig(),
                                       bool useMeanStdDev = false);

    /**
     * @brief  Perform through focus fine adjustment.
     * @param keyName  the key name of Motion to perform through focus, from the
     * MotionConfig.json
     * @param focusconfig  through focus config, default from FocusScanConfig.json
     * @return the result contains the message, code and status.
     */
    virtual Result ML_Fine_FocusScan(std::string keyName,
                                     ML::MLColorimeter::FocusScanConfig focusconfig = ML::MLColorimeter::FocusScanConfig());

    /**
     * @brief  Get position, vid and mtf best value after calling ML_FocusScan().
     * @return x: position, y: vid, z: mtf.
     */
    virtual std::vector<cv::Point3f> ML_GetFocusScanResult();

    /**
     * @brief  Save through focus result to csv file after calling ML_FocusScan().
     * @param path  file path to save mtf curve.
     * @param saveImages  save coarse and fine images to local
     * @param prefix  prefix name of the images and csv
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SaveFocusScanResult(std::string path, bool saveImages = false, std::string prefix = "");

    /**
     * @brief  Scan with diopter to find the best focus. ROIs parameter order: roi_H1, roi_H2, roi_V1, roi_V2
     * @param keyName  the key name of Motion, from the MotionConfig.json
     * @param dscanconfig  diopter scan config, default from DiopterScanConfig.json.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_DiopterScan(std::string keyName,
                                  ML::MLColorimeter::DiopterScanConfig dscanconfig = ML::MLColorimeter::DiopterScanConfig());

    /**
     * @brief  Initial diopter scan config and parameters setting.
     * @param keyName  the key name of Motion, from the MotionConfig.json
     * @param dscanconfig  diopter scan config, default from DiopterScanConfig.json
     * @return the result contains the message, code and status.
     */
    virtual Result ML_InitDiopterScanConfig(
        std::string keyName, ML::MLColorimeter::DiopterScanConfig dscanconfig = ML::MLColorimeter::DiopterScanConfig());

    /**
     * @brief  Get diopter scan config after initialization.
     */
    virtual ML::MLColorimeter::DiopterScanConfig ML_GetDiopterScanConfig();

    /**
     * @brief  Perform coarse adjustment for diopter scan.
     * @param keyName  the key name of Motion, from the MotionConfig.json
     * @param dscanconfig  diopter scan config, default from DiopterScanConfig.json
     * @param useMeanStdDev use MeanStdDev func to calculate the mean and the standard
     * deviation M of the image
     * @return the result contains the message, code and status.
     */
    virtual Result ML_Coarse_DiopterScan(
        std::string keyName, ML::MLColorimeter::DiopterScanConfig dscanconfig = ML::MLColorimeter::DiopterScanConfig(),
        bool useMeanStdDev = false);

    /**
     * @brief  Perform fine adjustment for diopter scan.
     * @param keyName  the key name of Motion, from the MotionConfig.json
     * @param dscanconfig  diopter scan config, default from DiopterScanConfig.json
     * @return the result contains the message, code and status.
     */
    virtual Result ML_Fine_DiopterScan(std::string keyName,
                                       ML::MLColorimeter::DiopterScanConfig dscanconfig = ML::MLColorimeter::DiopterScanConfig());

    /**
     * @brief  Get diopter, vid and mtf best value after calling ML_DiopterScan().
     * @return x: diopter, y: vid, z: mtf.
     */
    virtual std::vector<cv::Point3f> ML_GetDiopterScanResult();

    /**
     * @brief  Get horizontal average result
     * @return x: diopter, y: vid, z: mtf.
     */
    virtual cv::Point3f ML_GetDiopterScanResult_H();

    /**
     * @brief  Get vertical average result
     * @return x: diopter, y: vid, z: mtf.
     */
    virtual cv::Point3f ML_GetDiopterScanResult_V();

    /**
     * @brief  Save diopter result to csv file and images after calling ML_DiopterScan().
     * @param path  file path to save mtf curve.
     * @param saveImages  save coarse and fine images to local
     * @param prefix  prefix name of the images and csv
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SaveDiopterScanResult(std::string path, bool saveImages = false, std::string prefix = "");

    /**
     * @brief  trans diopter to motion position.
     * @param diopter  input diopter
     * @param focalLength  focal length of module, from the DiopterScanConfig.json
     * @param infinityPosition  infinity position of module, from the
     * DiopterScanConfig.json
     * @param focalSpace  focal planes object space of module, from the
     * DiopterScanConfig.json
     * @param isReverse  is the installation direction of the motor reversed or not, from
     * the MotionConfig.json
     * @return the result contains the message, code and status.
     */
    virtual double ML_TransDiopterToPosition(double diopter, double focalLength, double infinityPosition, double focalSpace,
                                             bool isReverse = false);

    // Reflector Interface

    /**
     * @brief  Move electric reflector by enum.
     * @param reflector  reflector enum to set
     * @return the result contains the message, code and status.
     */
    virtual Result ML_MoveReflectorByEnum(ML::MLEleReflectorCommon::MLEleEnum reflector);

    /**
     * @brief  electric reflector relative move.
     * @param pos  relative position to move (unit: millimeter).
     * @return the result contains the message, code and status.
     */
    virtual Result ML_RelMoveReflector(double pos);

    /**
     * @brief  Get the position of reflector (uni: millimeter).
     */
    virtual double ML_GetReflectorPosition();

    /**
     * @brief  Clear alarm for reflector.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_ReflectorClearAlarm();

    // Spectrometer and Specbos Interface

    /**
     * @brief  Set integration time for spectrometer.
     * @param time integration time to set. (unit: millisecond)
     * @return
     */
    virtual Result ML_SetIntegrationTime(double time = 200);

    /**
     * @brief  Get integration time from spectrometer. (unit: millisecond)
     * @return
     */
    virtual double ML_GetIntegrationTime();

    /**
     * @brief  Auto adjust integration time for spectrometer.
     * @param iteration  number of iteration during adjustment.
     * @return
     */
    virtual Result ML_AdjustIntegrationTime(int iteration = 15);

    /**
     * @brief  Calculate X,Y,Z,Cx,Cy data through spectrometer.
     * @param calibration_file  standard light source calibration file path
     * @param constantX_file  CIE constant X function file path
     * @param constantY_file  CIE constant Y function file path
     * @param constantZ_file  CIE constant Z function file path
     * @return
     */
    virtual Result ML_CalculateCIE(std::string calibration_file, std::string constantX_file, std::string constantY_file,
                                   std::string constantZ_file);

    /**
     * @brief  Get X,Y,Z,Cx,Cy data after calling ML_CalculateCIE().
     * @return
     */
    virtual ML::MLSpectrometer::MLCIE ML_GetCIE();

    /**
     * @brief  Calculate spectrum through spectrometer.
     * @return
     */
    virtual Result ML_CalculateSpectrum();

    /**
     * @brief  Save wavelengths and spectrums after calling ML_CalculateSpectrum().
     * @param path  file path to save data
     * @return
     */
    virtual Result ML_SaveSpectrum(std::string path);

    /**
     * @brief  Get formatted wavelengths from spectrometer.
     * @return
     */
    virtual std::vector<double> ML_GetWavelength();

    /**
     * @brief  Get formatted spectrums from spectrometer.
     * @return
     */
    virtual std::vector<double> ML_GetSpectrum();

    /**
     * @brief  select specbos calibration file number
     * @param num  calibration file number setting
     * @return
     */
    virtual Result ML_SetCalibrationFileNumber(int num);

    /**
     * @brief  set average count for specbos measurement
     * @param average  average count setting
     * @return
     */
    virtual Result ML_SetAverage(int average);

    /**
     * @brief  set sync freq for specbos measurement
     * @param freq  sync freq setting
     * @return
     */
    virtual Result ML_SetSyncFreq(int freq);

    /**
     * @brief  auto adjust integration time for specbos measurement
     * @param fMaxTint  max integration time for auto adjustment
     * @return
     */
    virtual Result ML_SetAutoTintConf(float fMaxTint = 60000);

    /**
     * @brief  set fixed integration time for specbos measurement
     * @param fTint  fixed integration time setting
     * @return
     */
    virtual Result ML_SetFixedTintConf(float fTint);

    /**
     * @brief  get integration time
     * @return
     */
    virtual float ML_GetTintConf();

    /**
     * @brief  take specbos measurement
     * @return
     */
    virtual Result ML_TakeSpecbosMeasurement();

    /**
     * @brief  get the result after calling ML_TakeSpecbosMeasurement()
     * @return
     */
    virtual ML::MLColorimeter::SpecbosResult ML_GetSpecbosResult();

    // module interface

    /**
     * @brief  Set aperture for MLMonoBusinessManage system.
     * @param aperture  aperture to set, example: 3mm.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetAperture(std::string aperture);

    /**
     * @brief  Get aperture from MLMonoBusinessManage system.
     * @return aperture of MLMonoBusinessManage system.
     */
    virtual std::string ML_GetAperture();

    /**
     * @brief  Set dut lightSource for MLMonoBusinessManage system.
     * @param lightSource  lightSource to set, example: R/G/B/W.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetLightSource(std::string lightSource);

    /**
     * @brief  Get dut lightSource from MLMonoBusinessManage system.
     * @return lightSource of MLMonoBusinessManage system.
     */
    virtual std::string ML_GetLightSource();

    /**
     * @brief  Get CaptureData, containing the info of aperture, lightsource, nd filter, color filter, rx,
     * vid, exposure time, image, binning, pixel format.
     * @return ML::MLColorimeter::CaptureData
     */
    virtual ML::MLColorimeter::CaptureData ML_GetCaptureData();

    /**
     * @brief  Get a map of CaptureData object for color camera.
     * @return a map of CaptureData (map format: {color filter channel enum, capture
     * data}).
     */
    virtual std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>
    ML_GetColorCameraCaptureData();

    /**
     * @brief  Get a ML_addInBase object by name.
     * @param name  the Key field of module, from the config file.
     * @return a ML_addInbase object of the MLMonoBusinessManage system.
     */
    virtual ML_addInBase* ML_GetOneModuleByName(std::string name);

    /**
     * @brief  Get a MLColorimeterConfig object.
     * @return the MLColorimeterConfig object of the MLMonoBusinessManage system..
     */
    virtual MLColorimeterConfig* ML_GetBusinessManageConfig();

 public:
    virtual Result FocusScan(std::string keyName, ML::MLColorimeter::FocusScanConfig focusconfig);

    virtual Result DiopterScan(std::string keyName, ML::MLColorimeter::DiopterScanConfig dscanConfig);

    virtual void StartThroughFocus();

    virtual void StopThroughFocus();

    virtual void SetROIsName(std::vector<std::string> vec);

    // diopter scan curve
    virtual std::vector<double> GetDiopterScan_DiopterCurve();

    virtual std::vector<double> GetDiopterScan_VIDCurve();

    virtual std::vector<std::vector<double>> GetDiopterScan_MTFCurve();

    virtual std::vector<double> GetCoarseDiopterScan_DiopterCurve();

    virtual std::vector<double> GetCoarseDiopterScan_VIDCurve();

    virtual std::vector<std::vector<double>> GetCoarseDiopterScan_MTFCurve();

    virtual std::vector<double> GetCoarseDiopterScan_DiopterCurve_H();

    virtual std::vector<double> GetCoarseDiopterScan_MTFCurve_H();

    virtual std::vector<double> GetCoarseDiopterScan_MTFCurve_V();

    // focus motion position scan curve
    virtual std::vector<double> GetFocusScan_PositionCurve();

    virtual std::vector<double> GetFocusScan_VIDCurve();

    virtual std::vector<std::vector<double>> GetFocusScan_MTFCurve();

    virtual std::vector<double> GetCoarseFocusScan_PositionCurve();

    virtual std::vector<double> GetCoarseFocusScan_VIDCurve();

    virtual std::vector<std::vector<double>> GetCoarseFocusScan_MTFCurve();

    virtual double GetExposureOffset();

    virtual double GetInfinityPosition();

 public:
    Result CreateFilterWheel(ML::MLFilterWheel::MLNDFilterConfiguation configuration);

    Result CreateRXFilterWheel(ML::MLFilterWheel::MLRXFilterConfiguation configuration);

    Result CreateMotion(ML::MLColorimeter::MotionConfig config);

    Result CreateCamera(ML::MLColorimeter::CameraConfig config);

    Result CreateReflector(ML::MLEleReflectorCommon::MLEleConfiguration config);

    Result CreateSpectrometer(ML::MLSpectrometer::SpectrometerInfo config);

    Result CreateSpecbos(ML::MLColorimeter::SpecbosConfig config);

    Result ConnectFilterWheel();

    Result ConnectRXFilterWheel();

    Result ConnectMotion();

    Result ConnectCamera();

    Result ConnectReflector();

    Result ConnectSpectrometer();

    Result ConnectSpecbos();

    Result DisconnectFilterWheel();

    Result DisconnectRXFilterWheel();

    Result DisconnectMotion();

    Result DisconnectCamera();

    Result DisconnectReflector();

    Result DisconnectSpectrometer();

    Result DisconnectSpecbos();

    bool IsFilterWheelConnect();

    bool IsRXFilterWheelConnect();

    bool IsMotionConnect();

    bool IsCameraConnect();

    bool IsReflectorConnect();

    bool IsSpectrometerConnect();

    bool IsSpecbosConnect();

    bool IsFilterWheelMoving();

    bool IsRXFilterWheelMoving();

    bool IsMotionMoving();

    bool IsReflectorMoving();

    /**
     * @brief  Set fixed exposure time for camera.
     * @param expTime  exposure time to set (unit: millisecond).
     * @return the result contains the message, code and status.
     */
    Result FixedExposure(double expTime);

    /**
     * @brief  Set auto exposure time for camera.
     * @param initialTime  initial time of auto exposure (unit: millisecond).
     * @return the result contains the message, code and status.
     */
    Result AutoExposure(double initialTime = 0.0);

    bool startsWithNumber(std::string str);

 signals:
    void connectStatus(bool);
    void filterWheelStatus(bool);
    void RXFilterWheelStatus(bool);
    void motionStatus(bool);
    void reflectorStatus(bool);
    void spectrometerStatus(bool);
    void specbosStatus(bool);
    void coarseScanProgress(double);
    void fineScanProgress(double);

 private:
    std::map<std::string, ML_addInBase*> m_addInBase;
    MLColorimeterConfig* m_BMConfig = nullptr;
    std::string m_ConfigPath;
    std::map<std::string, ML::MLFilterWheel::MLFilterEnum> m_Filter;
    int m_AxisDeg = 0;
    std::string m_RXFilter = "0d";
    std::string m_Aperture;
    std::string m_LightSource;
    ML::MLColorimeter::RXCombination m_inputRX;
    ML::MLColorimeter::RXCombination m_movementRX;

    // throughfocus property
    bool m_isAutoFocusStart = false;
    std::vector<std::string> m_ROIsName;
    ML::MLColorimeter::FocusScanConfig m_focusScanConfig;
    ML::MLColorimeter::DiopterScanConfig m_diopterScanConfig;
    double m_absPosition = 0;
    double m_absDiopter = 0;
    std::vector<cv::Mat> m_coarse_ImageArr;
    std::vector<cv::Mat> m_fine_ImageArr;

    // coarse vid scan parameters
    std::vector<double> m_coarseFocusScan_PosArr;
    std::vector<double> m_coarseFocusScan_VIDArr;
    std::vector<std::vector<double>> m_coarseFocusScan_MTFArr;
    std::vector<double> m_coarseFocusScan_PosArrRaw;
    std::vector<double> m_coarseFocusScan_VIDArrRaw;
    std::vector<std::vector<double>> m_coarseFocusScan_MTFArrRaw;
    std::vector<cv::Point3f> m_focusScan_ResultArr;

    // fine vid scan parameters
    std::vector<double> m_focusScan_PosArr;
    std::vector<double> m_focusScan_VIDArr;
    std::vector<std::vector<double>> m_focusScan_MTFArr;
    std::vector<double> m_focusScan_PosArrRaw;
    std::vector<double> m_focusScan_VIDArrRaw;
    std::vector<std::vector<double>> m_focusScan_MTFArrRaw;

    // coarse diopter scan parameters
    std::vector<double> m_coarseDiopterScan_DiopterArr;
    std::vector<double> m_coarseDiopterScan_VIDArr;
    std::vector<std::vector<double>> m_coarseDiopterScan_MTFArr;
    std::vector<double> m_coarseDiopterScan_DiopterArrRaw;
    std::vector<double> m_coarseDiopterScan_VIDArrRaw;
    std::vector<std::vector<double>> m_coarseDiopterScan_MTFArrRaw;
    // x : diopter, y : vid, z : mtf.
    std::vector<cv::Point3f> m_diopterScan_ResultArr;
    // coarse diopter scan horizontal and vertical parameters
    cv::Point3f m_coarseDiopterScan_ResultArr_H;
    cv::Point3f m_coarseDiopterScan_ResultArr_V;
    std::vector<double> m_coarseDiopterScan_DiopterArr_H;
    std::vector<double> m_coarseDiopterScan_DiopterArr_V;
    std::vector<double> m_coarseDiopterScan_MTFArr_H;
    std::vector<double> m_coarseDiopterScan_MTFArr_V;
    std::vector<double> m_coarseDiopterScan_VIDArr_H;
    std::vector<double> m_coarseDiopterScan_VIDArr_V;

    // fine diopter scan parameters
    std::vector<double> m_diopterScan_DiopterArr;
    std::vector<double> m_diopterScan_VIDArr;
    std::vector<std::vector<double>> m_diopterScan_MTFArr;
    std::vector<double> m_diopterScan_DiopterArrRaw;
    std::vector<double> m_diopterScan_VIDArrRaw;
    std::vector<std::vector<double>> m_diopterScan_MTFArrRaw;

    

    QMutex m_mutex;
    bool m_isFirstConnect = false;
    ML::CameraV2::AEContainer m_AEContainer;
    double m_INFPos = 0;
    std::map<std::string, double> m_INFPosMap;
    std::string m_FreqStr;

    bool m_getAEImage = false;

    ML::CameraV2::Binning m_swBinning = ML::CameraV2::Binning::ONE_BY_ONE;
    ML::CameraV2::BinningMode m_swBinningMode = ML::CameraV2::BinningMode::AVERAGE;
};
}  // namespace MLColorimeter
}  // namespace ML

#endif  // SRC_MLCOLORIMETER_MLMONOBUSINESSMANAGE_H_
