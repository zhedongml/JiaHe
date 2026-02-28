// Copyright 2024 MLOptic

#ifndef SRC_MLCOLORIMETER_MLBINOBUSINESSMANAGE_H_
#define SRC_MLCOLORIMETER_MLBINOBUSINESSMANAGE_H_

#include <QtCore/qobject.h>

#include <map>
#include <vector>

#include "AECommon.h"
#include "MLColorimeterCommon.h"
#include "Result.h"
#include "mlcolorimeter_global.h"

/**
 * @file MLBinoBusinessManage.h
 * @brief Defines the binocular module business logic class and associated functionality.
 */

namespace ML {

namespace MLFilterWheel {
class MLFilterWheelCallback;
}  // namespace MLFilterWheel

namespace MLColorimeter {

class MLMonoBusinessManage;
class MLColorimeterAlgorithms;

/**
 * @class MLBinoBusinessManage
 * @brief Manages the hardware control and business logic for binocular module.
 *
 * The class provides a common interface to manage business logic and hardware control
 * including NDFilter, ColorFilter, RXFilter, Motor and Camera.
 */
class MLCOLORIMETER_EXPORT MLBinoBusinessManage : public QObject {
    Q_OBJECT

 public:
    MLBinoBusinessManage(QObject* parent = nullptr);
    ~MLBinoBusinessManage();

    /**
     * @brief Add a monocular businessmanage instance.
     * @param path  module's config path.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_AddModule(const char* path);

    virtual Result ML_AddModule(MLMonoBusinessManage* mono);

    /**
     * @brief Remove a monocular businessmanage instance added by ML_AddModule().
     * @param moduleID  module's id, from ModuleConfig.json
     * @return the result contains the message, code and status.
     */
    virtual Result ML_RemoveModule(int moduleID);

    /**
     * @brief Add a ipd module instance.
     * @param path  module's config path.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_AddIPDMotion(const char* path);

    /**
     * @brief Remove the ipd module added by ML_AddIPDMotion().
     * @return the result contains the message, code and status.
     */
    virtual Result ML_RemoveIPDMotion();

    /**
     * @brief Create and connect modules added by ML_AddModule() and ML_AddIPDMotion().
     * @return the result contains the message, code and status.
     */
    virtual Result ML_ConnectModules();

    /**
     * @brief Disconnect and destory modules added by ML_AddModule() and
     * ML_AddIPDMotion().
     * @return the result contains the message, code and status.
     */
    virtual Result ML_DisconnectModules();

    /**
     * @brief Check if all module created by ML_CreateModules() are connected.
     * @return true if all are connected, otherwise false.
     */
    virtual bool ML_IsModulesConnect();

    /**
     * @brief Check if all modules created by ML_CreateModules() are moving.
     * @return true if one is moving, otherwise false.
     */
    virtual bool ML_IsModulesMoving();

    /**
     * @brief Wait for all modules' movement finish.
     * @param timeout  time out limit (unit: millisecond).
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_WaitForMovingStop(int timeout = 10000, ML::MLColorimeter::OperationMode mode =
                                                                 ML::MLColorimeter::OperationMode::Parallel);

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
     * @brief Stop all movements of modules.
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_StopModulesMovement(
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief Get the serial number of the modules.
     * @return a map of modules' serial number.
     */
    virtual std::map<int, std::string> ML_GetModulesSerialNumber();

    /**
     * @brief Get the name of the modules.
     * @return a map of modules' name.
     */
    virtual std::map<int, std::string> ML_GetModulesName();

    /**
     * @brief Get the modules id list.
     * @return a list of modules' id.
     */
    virtual std::vector<int> ML_GetModulesIDList();

    /**
     * @brief Get the modules number.
     * @return the number of module in the MLBinoBusinessManage system.
     */
    virtual int ML_GetModulesNumber();

    /**
     * @brief Get a MLMonoBusinessManage instance by module serial number.
     * @param serialNumber  the serial number of the MLMonoBusinessManage instance.
     * @return the MLMonoBusinessManage instance.
     */
    virtual MLMonoBusinessManage* ML_GetModuleBySN(std::string serialNumber);

    /**
     * @brief Get a MLMonoBusinessManage instance by module name.
     * @param name  the name of the MLMonoBusinessManage instance.
     * @return the MLMonoBusinessManage instance.
     */
    virtual MLMonoBusinessManage* ML_GetModuleByName(std::string name);

    /**
     * @brief Get a MLMonoBusinessManage instance by id.
     * @param ID  the ID of the MLMonoBusinessManage instance.
     * @return the MLMonoBusinessManage instance.
     */
    virtual MLMonoBusinessManage* ML_GetModuleByID(int ID);

    /**
     * @brief Get a MLMonoBusinessManage instance by eyemode.
     * @param eyemode  the eyemode of the MLMonoBusinessManage instance.
     * @return the MLMonoBusinessManage instance.
     */
    virtual MLMonoBusinessManage* ML_GetModuleByEyeMode(ML::MLColorimeter::EyeMode eyemode);

    /**
     * @brief Get modules' config.
     * @return a map of module config of the MLBinoBusinessMange system.
     */
    virtual std::map<int, ML::MLColorimeter::ModuleConfig> ML_GetModulesConfig();

    /**
     * @brief  Switch ND/XYZ filter wheel's channel by enum for all modules
     * asynchronously.
     * @param keyName  the key name of FilterWheel, from the config.
     * @param channelName  the enum of channel to switch
     * @param mode  operation mode between multiple modules.
     * @param cb  callback function
     * @return the result contains the message, code and status.
     */
    virtual Result ML_MoveModulesND_XYZFilterAsync(
        std::string keyName, ML::MLFilterWheel::MLFilterEnum channelName,
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr);

    /**
     * @brief  Switch ND/XYZ filter wheel's channel by enum for all modules
     * synchronously.
     * @param keyName  the key name of FilterWheel, from the config.
     * @param channelName  the enum of channel to switch
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_MoveModulesND_XYZFilterSync(
        std::string keyName, ML::MLFilterWheel::MLFilterEnum channelName,
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Get the channel of ND/XYZ FilterWheel.
     * @param keyName  the key name of FilterWheel, from the config.
     * @return a map of Filter channel (format: {module id, enum of channel}).
     */
    virtual std::map<int, ML::MLFilterWheel::MLFilterEnum> ML_GetND_XYZFilterChannel(std::string keyName);

    /**
     * @brief  Clear Alarm for ND and XYZ FilterWheel.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_ND_XYZFilterClearAlarm();

    /**
     * @brief  Switch RX filter wheel's channel by name and change RX axis degree
     * asynchronously.
     * @param channelName  the name of channel to switch
     * @param degree  the degree of axis to change
     * @param isResetAxis  whether reset axis to origin before moving to target position
     * @param mode  operation mode between multiple modules.
     * @param cb  callback function
     * @return the result contains the message, code and status.
     */
    virtual Result ML_MoveModulesRXFilterAsync(
        std::string channelName, int degree, bool isResetAxis = false,
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr);

    /**
     * @brief  Switch RX filter wheel's channel by name and change RX axis degree
     * synchronously.
     * @param channelName  the name of channel to switch
     * @param degree  the degree of axis to change
     * @param isResetAxis  whether reset axis to origin before moving to target position
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_MoveModulesRXFilterSync(
        std::string channelName, int degree, bool isResetAxis = false,
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Get the channel name of RX FilterWheel.
     * @return a map of Filter channel (format: {module id, name of channel}).
     */
    virtual std::map<int, std::string> ML_GetRXFilterChannel();

    /**
     * @brief  Get  the degree of RXFilterWheel's axis.
     * @return a map of Filter channel (format: {module id, axis degree}).
     */
    virtual std::map<int, int> ML_GetRXFilterAxis();

    /**
     * @brief  Clear Alarm for RXFilter and RX Axis.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_RXFilterClearAlarm();

    /**
     * @brief  Move electric reflector by enum.
     * @param reflector  reflector enum to set
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_MoveReflectorByEnum(
        ML::MLEleReflectorCommon::MLEleEnum reflector,
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Let electric reflector relative move.
     * @param pos  relative position to move (unit: millimeter).
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_RelMoveReflector(
        double pos, ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Get the position of reflector (uni: millimeter).
     */
    virtual std::map<int, double> ML_GetReflectorPosition();

    /**
     * @brief  Clear alarm for reflector.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_ReflectorClearAlarm();

    /**
     * @brief  Set focus by Virtual Image Distance asynchronously.
     * @param vid  the vid to set (unit: millimeter).
     * @param mode  operation mode between multiple modules.
     * @param method  focus method (default: Inverse)
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetVIDAsync(
        double vid, ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel,
        ML::MLColorimeter::FocusMethod method = ML::MLColorimeter::FocusMethod::Inverse);

    /**
     * @brief  Set focus by Virtual Image Distance synchronously.
     * @param vid  the vid to set (unit: millimeter).
     * @param mode  operation mode between multiple modules.
     * @param method  focus method (default: Inverse)
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetVIDSync(
        double vid, ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel,
        ML::MLColorimeter::FocusMethod method = ML::MLColorimeter::FocusMethod::Inverse);

    /**
     * @brief  Get modules' Virtual Image Distance (unit: millimeter).
     * @param method  focus method (default: Inverse)
     * @return a map of vid (format: {module id, vid value}).
     */
    virtual std::map<int, double> ML_GetVID(
        ML::MLColorimeter::FocusMethod method = ML::MLColorimeter::FocusMethod::Inverse);

    /**
     * @brief  Perform through focus and return the vid and position on best mtf.
     * @param keyName  the key name of Motion to perform through focus, from the config
     * @param focusconfig  through focus config, default from ThroughFocus.json
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_FocusScan(
        std::string keyName,
        ML::MLColorimeter::FocusScanConfig focusconfig = ML::MLColorimeter::FocusScanConfig(),
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Initial through focus config and parameters setting.
     * @param keyName  the key name of Motion to perform through focus, from the config
     * @param focusconfig  through focus config, default from ThroughFocus.json
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_InitFocusScanConfig(
        std::string keyName,
        ML::MLColorimeter::FocusScanConfig focusconfig = ML::MLColorimeter::FocusScanConfig(),
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Get through focus config after initialization.
     */
    virtual std::map<int, ML::MLColorimeter::FocusScanConfig> ML_GetFocusScanConfig();

    /**
     * @brief  Perform through focus coarse adjustment.
     * @param keyName  the key name of Motion to perform through focus, from the config
     * @param focusconfig  through focus config, default from ThroughFocus.json
     * @param useMeanStdDev use MeanStdDev func to calculate the mean and the standard
     * deviation M of the image
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_Coarse_FocusScan(
        std::string keyName,
        ML::MLColorimeter::FocusScanConfig focusconfig = ML::MLColorimeter::FocusScanConfig(),
        bool useMeanStdDev = false,
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Perform through focus fine adjustment.
     * @param keyName  the key name of Motion to perform through focus, from the config
     * @param focusconfig  through focus config, default from ThroughFocus.json
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_Fine_FocusScan(
        std::string keyName,
        ML::MLColorimeter::FocusScanConfig focusconfig = ML::MLColorimeter::FocusScanConfig(),
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Get position, vid and mtf best value after calling ML_FocusScan().
     * @return x: position, y: vid, z: mtf.
     */
    virtual std::map<int, std::vector<cv::Point3f>> ML_GetFocusScanResult();

    /**
     * @brief  Save through focus result to csv file by id after calling
     * ML_FocusScan().
     * @param module_id  select a moudle to save through focus result.
     * @param filename  file path to save mtf curve.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SaveFocusScanResult(int module_id, std::string filename);

    /**
     * @brief  focus motion absolute movement asynchronously.
     * @param keyName  the key name of Motion, from the config.
     * @param pos  absolute motion position to set (unit: millimeter).
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status
     */
    virtual Result ML_SetPositionAbsAsync(
        std::string keyName, double pos,
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  focus motion absolute movement synchronously.
     * @param keyName  the key name of Motion, from the config.
     * @param pos  absolute motion position to set (unit: millimeter).
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status
     */
    virtual Result ML_SetPositionAbsSync(
        std::string keyName, double pos,
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  focus motion relative movement asynchronously.
     * @param keyName  the key name of Motion, from the config.
     * @param pos  relative motion position to set (unit: millimeter).
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status
     */
    virtual Result ML_SetPositionRelAsync(
        std::string keyName, double pos,
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  focus motion relative movement synchronously.
     * @param keyName  the key name of Motion, from the config.
     * @param pos  relative motion position to set (unit: millimeter).
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status
     */
    virtual Result ML_SetPositionRelSync(
        std::string keyName, double pos,
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Get focus motion position (unit: millimeter).
     * @param keyName  the key name of Motion, from the config.
     * @return a map of position for keyName motion (format: {module id, position}).
     */
    virtual std::map<int, double> ML_GetMotionPosition(std::string keyName);

    /**
     * @brief  Stop focus motion movement.
     * @param keyName  the key name of Motion, from the config.
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_StopMotionMovement(std::string keyName, ML::MLColorimeter::OperationMode mode =
                                                                  ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Set Spherical diopter asynchronously.
     * @param sphere  sphere to set.
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetSphericalAsync(
        double sphere, ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Set Spherical diopter synchronously.
     * @param sphere  sphere to set.
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetSphericalSync(
        double sphere, ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Get Spherical diopter.
     * @return a map of spherical power (format: {module id, spherical value}).
     */
    virtual std::map<int, double> ML_GetSpherical();

    /**
     * @brief  Set RX asynchronously.
     * @param rx  RXCombination setting containing spherical power, cylindrical power and
     * cylindrical axis.
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetRXAsync(
        ML::MLColorimeter::RXCombination rx,
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Set RX synchronously.
     * @param rx  RXCombination setting containing spherical power, cylindrical power and
     * cylindrical axis.
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetRXSync(
        ML::MLColorimeter::RXCombination rx,
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Get RX.
     * @return a map of RXCombination (format: {module id, RXCombination}).
     */
    virtual std::map<int, ML::MLColorimeter::RXCombination> ML_GetRX();

    /**
     * @brief  Set interpupillary distance for binocular module asynchronously.
     * @param ipd  the interpupillary distance to set (unit: millimeter).
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetIPDAsync(double ipd);

    /**
     * @brief  Set interpupillary distance for binocular system synchronously.
     * @param ipd  the interpupillary distance to set (unit: millimeter).
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetIPDSync(double ipd);

    /**
     * @brief  Get the interpupillary distance.
     * @return the interpupillary distance of binocular system.
     */
    virtual double ML_GetIPD();

    /**
     * @brief  Set aperture for MLBinoBusinessManage system.
     * @param aperture  aperture to set, example: 3mm.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetAperture(std::string aperture);

    /**
     * @brief  Get aperture from MLBinoBusinessManage system.
     * @return a map of aperture (format: {module id, aperture}).
     */
    virtual std::map<int, std::string> ML_GetAperture();

    /**
     * @brief  Set dut lightSource for MLBinoBusinessManage system.
     * @param lightSource  lightSource to set, example: R/G/B/W.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetLightSource(std::string lightSource);

    /**
     * @brief  Get dut lightSource from MLBinoBusinessManage system.
     * @return a map of lightSource (format: {module id, lightSource}).
     */
    virtual std::map<int, std::string> ML_GetLightSource();

    /**
     * @brief  Get AutoExposure params.
     * @param moduleID  select a module to get autoexposure parameters.
     */
    virtual ML::AECommon::AEParam ML_GetAEParams(int moduleID);

    /**
     * @brief  Update AutoExposure params.
     * @param moduleID  select a module to update autoexposure parameters.
     * @param aeParams  AutoExposure parameters setting
     * @return the result contains the message, code and status.
     */
    virtual Result ML_UpdateAEParams(int moduleID, ML::AECommon::AEParam aeParams);

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
     * @brief  Set exposure for camera, contain auto and fixed.
     * @param exposure  exposure setting to set, include exposure mode and exposure time
     * (unit: millisecond)
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetExposure(ExposureSetting exposure, ML::MLColorimeter::OperationMode mode =
                                                                ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  get auto exposure status.
     * @return
     */
    virtual std::map<int, ML::CameraV2::AutoExposureStatus> ML_GetAutoExposureStatus();

    /**
     * @brief  Get the exposure time from camera.
     * @return a map of the exposure time (format: {module id, exposure time}).
     */
    virtual std::map<int, double> ML_GetExposureTime();

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
    virtual std::map<int, ML::CameraV2::BinningSelector> ML_GetBinningSelector();

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
     * @return a map of the binning (format: {module id, Binning}).
     */
    virtual std::map<int, ML::CameraV2::Binning> ML_GetBinning();

    /**
     * @brief  Sets the mode to use to combine photo-sensitive cells together.
     * @param binningMode  binning mode to set.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetBinningMode(ML::CameraV2::BinningMode binningMode);

    /**
     * @brief  Get the binning mode from camera.
     * @return a map of the binning mode (format: {module id, BinningMode}).
     */
    virtual std::map<int, ML::CameraV2::BinningMode> ML_GetBinningMode();

    /**
     * @brief  Format of the pixel to use for acquisition.
     * @param pixelFormat  pixel format to set.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetPixelFormat(ML::CameraV2::MLPixelFormat pixelFormat);

    /**
     * @brief  Get the pixel format from camera.
     * @return a map of the pixel format (format: {module id, MLPixelFormat}).
     */
    virtual std::map<int, ML::CameraV2::MLPixelFormat> ML_GetPixelFormat();

    /**
     * @brief  Capture single image asynchronously.
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_CaptureImageAsync(
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Capture single image synchronously.
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_CaptureImageSync(
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Get an image after calling ML_CaptureImageAsync() or ML_CaptureImageSync().
     * @return a map of signle channel image for mono camera, or a map a three-channel
     * image for color camera (format: {module id, cv::Mat}).
     */
    virtual std::map<int, cv::Mat> ML_GetImage();

    /**
     * @brief  Selects the location within the device, where the temperature will be
     * measured.
     * @param selector  device temperature selector to set.
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetTemperatureSelector(
        ML::CameraV2::TemperatureSelector selector,
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Get the device temperature selector from camera.
     * @return ML::CameraV2::TemperatureSelector
     */
    virtual std::map<int, ML::CameraV2::TemperatureSelector> ML_GetTemperatureSelector();

    /**
     * @brief  Device temperature in degrees Celsius (C). It is measured at the location
     * selected by ML_SetTemperatureSelector().
     * @return device temperature
     */
    virtual std::map<int, double> ML_GetDeviceTemperature();

    /**
     * @brief  Set reference temperature (in degrees Celsius) for cooling operation.
     * @param temperature  target temperature to set.
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetTargetTemperature(
        double temperature,
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Get reference temperature (in degrees Celsius) for cooling operation from
     * camera.
     * @return device temperature
     */
    virtual std::map<int, double> ML_GetTargetTemperature();

    /**
     * @brief  Controls the acquisition rate (in Hertz) at which the frames are captured.
     * @param frame_rate  frame rate to set.
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetAcquisitionFrameRate(
        double frame_rate,
        ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Get the acquisition rate (in Hertz) from camera.
     * @return device temperature
     */
    virtual std::map<int, double> ML_GetAcquisitionFrameRate();

    /**
     * @brief  Controls the selected gain as an absolute physical value. This is an
     * amplification factor applied to the video signal.
     * @param gain  digital gain to set.
     * @param mode  operation mode between multiple modules.
     * @return the result contains the message, code and status.
     */
    virtual Result ML_SetCameraGain(
        double gain, ML::MLColorimeter::OperationMode mode = ML::MLColorimeter::OperationMode::Parallel);

    /**
     * @brief  Get the digital gain from camera.
     * @return digital gain
     */
    virtual std::map<int, double> ML_GetCameraGain();

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
    virtual std::map<int, double> ML_GetIntegrationTime();

    /**
     * @brief  Auto adjust integration time for spectrometer.
     * @param iteration  number of iteration during adjustment.
     * @return
     */
    virtual Result ML_AdjustIntegrationTime(int iteration = 15);

    /**
     * @brief  Calculate X,Y,Z,Cx,Cy data through spectrometer.
     * @param calibration_file standard light source calibration file path
     * @param constantX_file CIE constant X function file path
     * @param constantY_file CIE constant Y function file path
     * @param constantZ_file CIE constant Z function file path
     * @return
     */
    virtual Result ML_CalculateCIE(std::string calibration_file, std::string constantX_file,
                                   std::string constantY_file, std::string constantZ_file);

    /**
     * @brief  Get X,Y,Z,Cx,Cy data after calling ML_CalculateCIE().
     * @return
     */
    virtual std::map<int, ML::MLSpectrometer::MLCIE> ML_GetCIE();

    /**
     * @brief  Calculate spectrum through spectrometer.
     * @return
     */
    virtual Result ML_CalculateSpectrum();

    /**
     * @brief  Save wavelengths and spectrums after calling ML_CalculateSpectrum().
     * @param moduleID   select a module to save calibration data.
     * @param path  file path to save data
     * @return
     */
    virtual Result ML_SaveSpectrum(int moduleID, std::string path);

    /**
     * @brief  Get formatted wavelengths from spectrometer.
     * @return
     */
    virtual std::map<int, std::vector<double>> ML_GetWavelength();

    /**
     * @brief  Get formatted spectrums from spectrometer.
     * @return
     */
    virtual std::map<int, std::vector<double>> ML_GetSpectrum();

    /**
     * @brief  Get CaptureData, containing the info of aperture, lightsource, nd filter, color filter, rx,
     * vid, exposure time, image, binning, pixel format.
     * @return std::map<int, ML::MLColorimeter::CaptureData>
     */
    virtual std::map<int, ML::MLColorimeter::CaptureData> ML_GetCaptureData();

    /**
     * @brief  Get modules' CaptureData for color camera.
     * @return a map of CaptureData for color camera (map format: {module id, {color
     * filter channel enum, capture data}}).
     */
    virtual std::map<int, std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>>
    ML_GetColorCameraCaptureData();

 public:
    virtual Result FocusScan(std::string keyName, ML::MLColorimeter::FocusScanConfig focusconfig);

    virtual void StartThroughFocus();

    virtual void StopThroughFocus();

 private:
    MLMonoBusinessManage* m_IPD;

    std::map<int, MLMonoBusinessManage*> m_moduleList;
    std::vector<std::string> m_pathList;
    std::string m_configPath;
};
}  // namespace MLColorimeter
}  // namespace ML

#endif  //  SRC_MLCOLORIMETER_MLBINOBUSINESSMANAGE_H_
