// Copyright 2023 MLOptic

#ifndef LOGIC_CAMERAV2_CAMERAV2COMMON_H_
#define LOGIC_CAMERAV2_CAMERAV2COMMON_H_
#include <opencv2/opencv.hpp>

// This interface represents an older ML camera controller, The
// data structure are defined here which provides control over a camera.

namespace ML {
namespace CameraV2 {

/// <summary>
/// This enumeration is used to specify the camera status.
/// </summary>
enum class MLCameraState {
    kIsOpen,

    kIsGrabbing,

    kDisconnected,

    kConnected,

    kError

};

/// <summary>
/// This enumeration is used to specify the camera exposure status.
/// </summary>
enum class AutoExposureStatus {
    AE_normal_,
    AE_max_iterations_reached_,
    AE_too_dark_,
    AE_too_bright_,
    AE_single_acquisition_error,
    AE_continuous_acquisition_error,
    AE_stop_acquisition_error,
    AE_NONE,
    AE_time_out
};

/// <summary>
/// This enumeration is used to specify the camera exposure mode.
/// </summary>
enum class MLExposureMode {
    kOff = 0,

    kOnce = 1,

    kContinuous = 2,

    kMLAuto = 3,

    kMLAuto2

};

/// <summary>
/// This enumeration is used to specify the event.
/// </summary>
enum class MLCameraEvent {
    kStateChanged,

    kFrameReceived,

    kGrayLevel
};

enum class BinningSelector { Logic, Sensor };

/// <summary>
/// This enumeration is used to specify the camera binning.
/// </summary>
enum class Binning {
    ONE_BY_ONE = 0,  // 1*1
    TWO_BY_TWO = 1,
    FOUR_BY_FOUR = 2,
    EIGHT_BY_EIGHT = 3,
    SIXTEEN_BY_SIXTEEN = 4,
    UnKnown = -1
};

/// <summary>
/// This enumeration is used to specify the camera binning mode.
/// </summary>
enum class BinningMode { SUM, AVERAGE, UnKnown = -1 };

/// <summary>
/// This enumeration is used to specify the camera pixel format.
/// </summary>
enum class MLPixelFormat {
    MLMono8 = 0,  // 8 bits per pixel
    MLMono10,
    MLMono12,  // 12 bits per pixel
    MLMono16,  // 16 bits per pixel
    MLRGB24,   // RGB data format
    MLBayer = 10,
    MLBayerGB8,
    MLBayerGB10,
    MLBayerGB12,
    MLBayerRG8,
    MLBayerRG10,
    MLBayerRG12,
};

/** @brief Status Code */
enum class MLStatus {
    /** @brief operate successfully */
    ML_STATUS_OK = 1,

    /** @brief operation failure */
    ML_STATUS_FAILED = 0,

    ML_STATUS_NULL = -1,

    /** @brief unknown error */
    ML_STATUS_UNKNOWN = -2,

    /** @brief CAMERA ERROR */
    ML_STATUS_CAMERA_ERROR = -3,

    ML_STATUS_MOTION_ERROR = -4,

    /** @brief  */
    ML_STATUS_INVALID = -5,

    ML_STATUS_CAMERA_CLOSED = -6,

    ML_STATUS_MOTION_CLOSED = -7,

    ML_STATUS_PARAM_INVALID = -8,

    ML_STATUS_CLOSED = -9,

    ML_STATUS_TIMEOUT = -10,

    ML_STATUS_OVER_DARK = -11,

    ML_STATUS_OVER_BRIGHT = -12,

    ML_STATUS_Max_Iterations_Reached = -13

};

/// <summary>
/// This enumeration is used to specify the camera trigger source.
/// </summary>
enum class TriggerSource { Software, Hardware, None };

enum class TemperatureSelector {
    Sensor = 0,
    Mainboard = 1,
};

enum class CameraType { Matrox_Vieworks };

/// <summary>
/// @param code  status code
/// @param msg  error message
/// </summary>
struct MLResult {
    MLStatus code;

    std::string msg;
};

struct MLCameraResult {
    bool code = false;
    MLStatus status = MLStatus::ML_STATUS_OK;
    std::string msg = "";
};

struct AEContainer {
    AutoExposureStatus AE_status;
    cv::Mat final_img;
    MLPixelFormat bit_depth;
    double exposure_time;
    int iteration;
    int take_time;
    bool AE_done;
};

/// <summary>
/// MLCameraCallback class implementing the camera event callback
/// </summary>
class MLCameraCallback {
 public:
    virtual void NotifyCameraStateChanged(MLCameraState old_state,
                                          MLCameraState new_state) = 0;

    virtual void NotifyCameraFrameReceived(cv::Mat, MLPixelFormat format) = 0;

    virtual void NotifyCameraGrayLevel(int gray_level) = 0;
};

}  // namespace CameraV2
}  // namespace ML
#endif  // LOGIC_CAMERAV2_CAMERAV2COMMON_H_
