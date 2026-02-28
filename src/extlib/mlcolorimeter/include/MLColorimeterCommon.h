// Copyright 2024 MLOptic

#ifndef SRC_MLCOLORIMETERCOMMONR_MLCOLORIMETERCOMMON_H_
#define SRC_MLCOLORIMETERCOMMONR_MLCOLORIMETERCOMMON_H_

#include <map>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

#include "MLCamaraCommon.h"
#include "MLEleReflectorCommon.h"
#include "MLFilterWheelCommon.h"
#include "MLSpectrometerCommon.h"
#include "MotorPluginCommon.h"
#include "MLSpecbosCommon.h"

namespace ML {
namespace MLColorimeter {
enum class FFCMethod {
    /** @brief using full RX combination */
    Normal = 1,
    /** @brief using synthetic RX combination */
    Synthetic = 2
};

enum class FocusMethod { Inverse = 1 };

enum class EyeMode { EYE1 = 1, EYE2 = 2, BOTH = 3 };

enum class MappingMethod {
    /** @brief using the principle of proximity for rx mapping */
    Nearby = 1,
    /** @brief using interpolation principle for rx mapping */
    LinearInter = 2
};

enum class CalibrationEnum {
    /** @brief raw data from camera */
    Raw = 1,
    /** @brief data after flat field correction */
    FFC,
    /** @brief data after color shift correction*/
    ColorShift,
    /** @brief data after distortion correction*/
    Distortion,
    /** @brief data after four color calculation*/
    FourColor,
    /** @brief data after luminance calculation*/
    Luminance,
    Radiance,
    /** @brief chromaticity x, y*/
    Chrom,
    /** @brief data after FOVCrop correction*/
    FOVCrop,
    /** @brief data after flip and rotate*/
    Flip_Rotate,
    /** @brief data after Keystone*/
    Keystone,
    /** @brief final data */
    Result
};

enum class OperationMode {
    /** @brief parallel control of binocular modules*/
    Parallel = 1,
    /** @brief serial control of binocular modules*/
    Serial = 2
};

enum class ExposureMode {
    /** @brief fixed exposure*/
    Fixed = 1,
    /** @brief auto exposure*/
    Auto = 2
};

enum class Flipping {
    /** @brief non reverse*/
    NonReverse = 1,
    /** @brief flip the image horizontally*/
    ReverseX,
    /** @brief flip the image vertically*/
    ReverseY,
};

enum class Rotation {
    /** @brief rotate the image to 0 degree*/
    R0 = 1,
    /** @brief rotate the image to 90 degrees*/
    R90,
    /** @brief rotate the image to 180 degree*/
    R180,
    /** @brief rotate the image to 270 degree*/
    R270,
};

struct ExposureSetting {
    /** @brief use fixed or auto exposure*/
    ExposureMode Mode = ExposureMode::Auto;
    /** @brief exposure time for fixed exposure, or initial exposure time for auto
     * exposure (unit: millisecond)*/
    double ExposureTime = 0.0;
};

struct MTFSetting {
    bool isChess = true;
    bool isLpmm = true;
    double pixelSize = 0.00345;
    double focalLength = 50;
    double targetFreq = 34;
    double maxFreq = 150;
};

struct RXCombination {
    /** @brief diopter of spherical*/
    double Sphere = DBL_MAX;
    /** @brief diopter of cylindrical*/
    double Cylinder = DBL_MAX;
    /** @brief axis of astigmatism*/
    int Axis = INT_MAX;
};

struct MotionConfig {
    /** @brief motion enable setting*/
    bool Enable = false;
    /** @brief key word to identify the motion*/
    std::string Key = "CameraMotion";
    /** @brief type of the motion*/
    ML::Motion::ControllerType Type;
    /** @brief name of the motion*/
    std::string Name;
    /** @brief connect address of the motion*/
    std::string ConnectAddress;
    /** @brief Deprecated*/
    std::string ConnectType;
    /** @brief Deprecated*/
    std::string Port;
    /** @brief Deprecated*/
    int DeviceID;
    /** @brief Deprecated*/
    int Axis;
    /** @brief AxisName to create the motion*/
    std::string AxisName;
    /** @brief Deprecated*/
    std::string HomingMethod;
    /** @brief not use now*/
    double Speed;
    /** @brief is the installation direction of the motor reversed or not*/
    bool IsReverse;
    /** @brief Maximum value of soft limit (unit: millimeter)*/
    double SoftwareLimitMax;
    /** @brief Minimum value of soft limit (unit: millimeter)*/
    double SoftwareLimitMin;
    /** @brief go to the home position after connection (unit: millimeter)*/
    double HomingPosition;
    /** @brief infinity position of the module (unit: millimeter)*/
    double InfinityPosition;
    /** @brief focal length of the module (unit: millimeter)*/
    double FocalLength;
    /** @brief focal planes object space (unit: millimeter)*/
    double FocalPlanesObjectSpace;
    /** @brief inf position according to ND filter*/
    std::map<std::string, double> INFPos_Map;
};

struct CameraConfig {
    /** @brief camera enable setting*/
    bool Enable = false;
    /** @brief is a color camera or not*/
    bool ColourCamera;
    /** @brief key word to identify the camra*/
    std::string Key = "VieCamera1";
    /** @brief connect address of the camera*/
    std::string ConnectAddress;
    /** @brief type to create the camera*/
    ML::CameraV2::CameraType Type;
    /** @brief name of the camera*/
    std::string Name;
    std::string CameraSN;
};

struct SpecbosConfig {
    /** @brief specbos enable setting*/
    bool Enable = false;
    std::string Key = "Specbos";
    std::string Port;
    ML::MLSpecbos::DeviceType DType;
};

struct ModuleConfig {
    /** @brief module enable setting*/
    bool Enable = false;
    /** @brief serial number of the module*/
    std::string SerialNumber;
    /** @brief name of the module*/
    std::string Name;
    /** @brief key word to identify the module*/
    std::string Key;
    /** @brief aperture of the module*/
    std::string Aperture;
    /** @brief eye mode of the module*/
    EyeMode EyeMode;
    /** @brief unique ID of the module*/
    int ID;
    /** @brief list of motions contained in the module*/
    std::map<std::string, MotionConfig> MotionConfig_Map;
    /** @brief the camera contained in the module*/
    CameraConfig CameraConfig;
    /** @brief list of filter wheel contained in the module*/
    std::map<std::string, ML::MLFilterWheel::MLNDFilterConfiguation> NDFilterConfig_Map;
    /** @brief lrx filter contained in the module*/
    ML::MLFilterWheel::MLRXFilterConfiguation RXFilterConfig;
    /** @brief ipd motion contained in the binocular module*/
    MotionConfig IPDConfig;
    /** @brief electric reflector contained in the module*/
    ML::MLEleReflectorCommon::MLEleConfiguration ReflectorConfig;
    /** @brief spectrometer contained in the module*/
    ML::MLSpectrometer::SpectrometerInfo SpectrometerConfig;
    SpecbosConfig SpecbosConfig;
};

/** @brief contain the capture data of the module*/
typedef struct CaptureData {
    /** @brief serial number of the module*/
    std::string SerialNumber;
    /** @brief name of the module*/
    std::string ModuleName;
    /** @brief key word to identify the module*/
    std::string Key;
    /** @brief module aperture during capture image*/
    std::string Aperture;
    /** @brief light source during capture image*/
    std::string LightSource;
    /** @brief nd filter during capture image*/
    ML::MLFilterWheel::MLFilterEnum NDFilter;
    /** @brief color filter during capture image*/
    ML::MLFilterWheel::MLFilterEnum ColorFilter;
    /** @brief rx during capture image*/
    RXCombination MovementRX;
    /** @brief vid during capture image (unit: millimeter)*/
    double VID;
    /** @brief exposure time during capture image (unit: millisecond)*/
    double ExposureTime;
    /** @brief the image*/
    cv::Mat Img;
    cv::cuda::GpuMat ImgGPU;
    /** @brief binning during capture image*/
    ML::CameraV2::Binning Binning;
    /** @brief pixel format during capture image*/
    ML::CameraV2::MLPixelFormat PixelFormat;
    double ExposureOffset;
    CaptureData() {
        Aperture = "3mm";
        LightSource = "";
        NDFilter = ML::MLFilterWheel::MLFilterEnum::ND0;
        ColorFilter = ML::MLFilterWheel::MLFilterEnum::X;
        VID = 0;
        ExposureTime = 100;
        Binning = ML::CameraV2::Binning::ONE_BY_ONE;
        PixelFormat = ML::CameraV2::MLPixelFormat::MLMono12;
        ExposureOffset = 0.0;
    }
} ProcessedData;

/** @brief FOVCrop setting*/
struct FOVCrop {
    /** @brief cross center*/
    cv::Point2f Center;
    /** @brief horizontal field of view angle*/
    double WidthDegree;
    /** @brief vertical field of view angle*/
    double HeightDegree;
    /** @brief focal length (unit: millimeter)*/
    double FocalLength;
    /** @brief pixel size of the camera (unit: millimeter)*/
    double PixelSize;
};

/** @brief loaded calibration data*/
struct PreCalibrationData {
    /** @brief ffc image */
    std::map<std::string, cv::Mat> LightFFCMap;
    /** @brief dark image */
    std::map<std::string, cv::Mat> DarkMap;
    /** @brief shift offset of color filter */
    std::map<std::string, cv::Point2f> ColorShiftMap;
    /** @brief matrix of the cametra*/
    std::map<std::string, std::tuple<cv::Mat, cv::Mat>> DistortionMap;
    /** @brief Exposure offset and Gray offset*/
    std::map<std::string, cv::Mat> OffsetMap;
    /** @brief R matrix to calculate four color*/
    std::map<std::string, cv::Mat> RMatrixMap;
    /** @brief */
    std::map<std::string, cv::Mat> LuminanceKMap;
    std::map<std::string, cv::Mat> RadianceMap;
    /** @brief nd filter of the calibration data*/
    ML::MLFilterWheel::MLFilterEnum NDFilter;
    /** @brief rx of the calibration data*/
    RXCombination RX;
    /** @brief light source of the calibration data*/
    std::string LightSource;
    /** @brief aperture of the calibration data*/
    std::string Aperture;
    /** @brief FOVCrop setting the calibration data*/
    FOVCrop Cropping;
    /** @brief flip x setting of the calibration data*/
    Flipping FlipX;
    /** @brief flip y setting of the calibration data*/
    Flipping FlipY;
    /** @brief rotate setting of the calibration data*/
    Rotation Rotate;
};

struct RXMappingMethod {
    MappingMethod SphMapping = MappingMethod::LinearInter;
    MappingMethod CylMapping = MappingMethod::Nearby;
    MappingMethod AxisMapping = MappingMethod::Nearby;
};

struct RXMappingRule {
    std::vector<double> SphMappingList;
    std::vector<double> CylMappingList;
    std::vector<int> AxisMappingList;
    RXMappingMethod RXMethod;
};

struct FolderRule {
    std::string Rule;
    std::string Suffix;
    std::string Path;
    RXMappingRule RXRule;
    FFCMethod ffcMethod;
    std::vector<double> Sphere_Lum_Cofficient;
    std::vector<double> Cylinder_Lum_Cofficient;
};

struct ProcessPathConfig {
    std::string InputPath;
    ML::MLFilterWheel::MLFilterEnum NDFilter;
    ML::MLFilterWheel::MLFilterEnum ColorFilter;
    std::string LightSource;
    std::string Aperture;
    RXCombination MovementRX;
};

struct CalibrationFlag {
    /** @brief is load dark data or not [is do dark correction or not]*/
    bool Dark_Flag = false;
    /** @brief is load ffc image or not [is do ffc correction or not]*/
    bool FFC_Flag = false;
    /** @brief calculate ffc map during measurement*/
    bool CalFFCMap_Flag = false;
    /** @brief is load colorshift data or not [is do colorshift correction or not]*/
    bool ColorShift_Flag = false;
    /** @brief is load distortion data or not [is do distortion correction or not]*/
    bool Distortion_Flag = false;
    /** @brief is do exposure correction or not*/
    bool Exposure_Flag = false;
    /** @brief is load R matrix or not [is do fourcolor calculation or not]*/
    bool FourColor_Flag = false;
    /** @brief is load luminance data or not [is do luminance calculation or not]*/
    bool Luminance_Flag = false;
    bool Radiance_Flag = false;
    /** @brief is load FOVCrop data or not [is do FOVCrop correction or not]*/
    bool FOVCrop_Flag = false;
    /** @brief is load Flip_Rotate data or not [is do Flip_Rotate correction or not]*/
    bool Flip_Rotate_Flag = false;
    /** @brief is load Keystone data or not [is do Keystone correction or not]*/
    bool Keystone_Flag = false;
};

struct CalibrationConfig2 {
    /** @brief path of the calibration data*/
    std::string InputPath = "";
    /** @brief aperture setting of the calibration data*/
    std::string Aperture = "3mm";
    /** @brief binning setting of the calibration data*/
    ML::CameraV2::Binning Bin = ML::CameraV2::Binning::ONE_BY_ONE;
    /** @brief nd filter setting of the calibration data*/
    std::vector<ML::MLFilterWheel::MLFilterEnum> NDFilterList = {ML::MLFilterWheel::MLFilterEnum::ND0};
    /** @brief color filter list setting of the calibration data*/
    std::vector<ML::MLFilterWheel::MLFilterEnum> ColorFilterList = {ML::MLFilterWheel::MLFilterEnum::X, ML::MLFilterWheel::MLFilterEnum::Y,
                                                                    ML::MLFilterWheel::MLFilterEnum::Z};
    /** @brief rx setting of the calibration data*/
    RXCombination RX = RXCombination();
    /** @brief light source setting of the calibration data*/
    std::vector<std::string> LightSourceList = {"W"};
    /** @brief origin image size data*/
    cv::Size OriginImageSize;
};

/** @brief config to load calibration data or take measurement*/
struct CalibrationConfig {
    /** @brief path of the calibration data*/
    std::string InputPath = "";
    /** @brief aperture setting of the calibration data*/
    std::string Aperture = "3mm";
    /** @brief binning setting of the calibration data*/
    ML::CameraV2::Binning Bin = ML::CameraV2::Binning::ONE_BY_ONE;
    /** @brief nd filter setting of the calibration data*/
    std::vector<ML::MLFilterWheel::MLFilterEnum> NDFilterList = {ML::MLFilterWheel::MLFilterEnum::ND0};
    /** @brief color filter list setting of the calibration data*/
    std::vector<ML::MLFilterWheel::MLFilterEnum> ColorFilterList = {ML::MLFilterWheel::MLFilterEnum::X, ML::MLFilterWheel::MLFilterEnum::Y,
                                                                    ML::MLFilterWheel::MLFilterEnum::Z};
    /** @brief rx setting of the calibration data*/
    RXCombination RX = RXCombination();
    /** @brief light source setting of the calibration data*/
    std::vector<std::string> LightSourceList = {"W"};
    /** @brief is load dark data or not [is do dark correction or not]*/
    bool Dark_Flag = false;
    /** @brief is load ffc image or not [is do ffc correction or not]*/
    bool FFC_Flag = false;
    /** @brief calculate ffc map during measurement*/
    bool CalFFCMap_Flag = false;
    /** @brief is load colorshift data or not [is do colorshift correction or not]*/
    bool ColorShift_Flag = false;
    /** @brief is load distortion data or not [is do distortion correction or not]*/
    bool Distortion_Flag = false;
    /** @brief is do exposure correction or not*/
    bool Exposure_Flag = false;
    /** @brief is load R matrix or not [is do fourcolor calculation or not]*/
    bool FourColor_Flag = false;
    /** @brief is load luminance data or not [is do luminance calculation or not]*/
    bool Luminance_Flag = false;
    bool Radiance_Flag = false;
    /** @brief is load FOVCrop data or not [is do FOVCrop correction or not]*/
    bool FOVCrop_Flag = false;
    /** @brief is load Flip_Rotate data or not [is do Flip_Rotate correction or not]*/
    bool Flip_Rotate_Flag = false;
};

/** @brief config to save calibration data*/
struct SaveDataConfig {
    std::string uid;
    /** @brief path to save calibration data*/
    std::string SavePath;
    /** @brief prefix name of the image and file*/
    std::string Prefix;
    /** @brief roi setting to save the images*/
    cv::Rect SaveROI = cv::Rect();

    /** @brief save raw images*/
    bool SaveRaw = true;
    /** @brief save final images*/
    bool SaveResult = true;
    /** @brief save intermediate data during measurement*/
    bool SaveCalibration = false;
};

/** @brief config of auto through focus*/
struct FocusScanConfig {
    /** @brief maximum motion position of rough auto focus (unit: millimeter)*/
    double FocusMax = DBL_MAX;
    /** @brief minimum motion position of rough auto focus (unit: millimeter)*/
    double FocusMin = DBL_MAX;
    /** @brief infinity position of the module (unit: millimeter)*/
    double InfinityPosition = DBL_MAX;
    /** @brief focal length of the module (unit: millimeter)*/
    double FocalLength = DBL_MAX;
    /** @brief pixel size of the camera (unit: millimeter)*/
    double PixelSize = DBL_MAX;
    /** @brief focal planes object space (unit: millimeter)*/
    double FocalPlanesObjectSpace = DBL_MAX;
    /** @brief motion position step during rough auto focus (unit: millimeter)*/
    double RoughStep = DBL_MAX;
    /** @brief set true to perform fine foucs adjustment, set false not to perform*/
    bool IsUseFineAdjust = true;
    /** @brief Deprecated*/
    double FineRange = DBL_MAX;
    /** @brief motion position step during fine auto focus (unit: millimeter)*/
    double FineStep = DBL_MAX;
    /** @brief freq to calculate mtf*/
    double Freq = DBL_MAX;
    /** @brief Deprecated*/
    double Smooth = DBL_MAX;
    /** @brief roi setting to calculate mtf*/
    std::vector<cv::Rect> ROIs = {};
    /** @brief target type used to calculate mtf, true for chess board, false for
     * crosshair*/
    bool ChessMode = true;
    /** @brief unit used to calculate mtf, true for lp/mm, false for lp/degree*/
    bool LpmmUnit = true;
    int AverageCount = 1;
    /** @brief store corase images and fine images in the memory*/
    bool StoreResultImg = false;
};

/** @brief diopter config of auto through focus*/
struct DiopterScanConfig {
    /** @brief maximum diopter of rough focus (unit: diopter)*/
    double DiopterMax = DBL_MAX;
    /** @brief minimum diopter of rough focus (unit: diopter)*/
    double DiopterMin = DBL_MAX;
    /** @brief infinity position of the module (unit: millimeter)*/
    double InfinityPosition = DBL_MAX;
    /** @brief focal length of the module (unit: millimeter)*/
    double FocalLength = DBL_MAX;
    /** @brief pixel size of the camera (unit: millimeter)*/
    double PixelSize = DBL_MAX;
    /** @brief focal planes object space (unit: millimeter)*/
    double FocalPlanesObjectSpace = DBL_MAX;
    /** @brief RoughStepNumber (unit: diopter)*/
    int RoughStepNumber = INT_MAX;
    /** @brief set true to perform fine foucs adjustment, set false not to perform*/
    bool IsUseFineAdjust = true;
    /** @brief diopter step during fine auto focus (unit: diopter)*/
    int FineStepNumber = INT_MAX;
    /** @brief freq to calculate mtf*/
    double Freq = DBL_MAX;
    /** @brief roi setting to calculate mtf*/
    std::vector<cv::Rect> ROIs = {};
    /** @brief target type used to calculate mtf, true for chess board, false for
     * crosshair*/
    bool ChessMode = true;
    /** @brief unit used to calculate mtf, true for lp/mm, false for lp/degree*/
    bool LpmmUnit = true;
    /** @brief store corase images and fine images in the memory*/
    bool StoreResultImg = false;
    int AverageCount = 1;
};

struct CircleFitParameters {
    double OriginDeg;
    double CenterX;
    double CenterY;
    double Radius;
    bool IsClockwise;
};

struct PolynomialFitParameters {
    std::vector<double> PolynomialX;
    std::vector<double> PolynomialY;
};

struct InUseCaliData {
    std::string SerialNumber;
    std::string ModuleName;
    std::string Aperture;
    std::string LightSource;
    ML::MLFilterWheel::MLFilterEnum NDFilter;
    ML::MLFilterWheel::MLFilterEnum ColorFilter;
    RXCombination MovementRX;
    RXCombination FFCSphereRX;
    RXCombination FFCAxisRX;
    RXCombination ColorShiftRX;
    RXCombination DistortionRX;
    RXCombination FourColorRX;
    RXCombination LuminanceRX;
    RXCombination RadianceRX;
    double VID;
    double ExposureTime;
    ML::CameraV2::Binning Binning;
    ML::CameraV2::MLPixelFormat PixelFormat;
    double DarkGray;
    cv::Mat ColorShift;
    cv::Mat CameraMatrix;
    cv::Mat Coefficient;
    cv::Mat Offset;
    cv::Mat RMatrix;
    cv::Mat LuminanceK;
    cv::Mat RadianceK;
};

struct SpecbosResult {
    float ChromX;
    float ChromY;
    float X;
    float Y;
    float Z;
};

struct ProcessedStatus {
    bool DarkStatus = true;
    bool FFCStatus = true;
    bool ColorShiftStatus = true;
    bool DistortionStatus = true;
    bool ExposureStatus = true;
    bool FourColorStatus = true;
    bool LuminanceStatus = true;
    bool RadianceStatus = true;
    bool FlipRotateStatus = true;
    bool FOVCropStatus = true;
};

}  // namespace MLColorimeter
}  // namespace ML

#endif  // SRC_MLCOLORIMETERCOMMONR_MLCOLORIMETERCOMMON_H_
