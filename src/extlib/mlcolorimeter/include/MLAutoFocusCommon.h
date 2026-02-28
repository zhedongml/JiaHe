// Copyright 2025 MLOptic

#ifndef SRC_MLAUTOFOCUSPRCESS_MLAUTOFOCUSCOMMON_H_
#define SRC_MLAUTOFOCUSPRCESS_MLAUTOFOCUSCOMMON_H_

#include <string.h>

#include <map>
#include <opencv2/opencv.hpp>
#include <vector>

namespace ML {
namespace AutoFocus {
enum class ScanEnum { Position = 1, VID, Diopter, MTF };

struct FocusScanConfig {
    double FocusMax = DBL_MAX;          /** @brief maximum motion position of rough auto focus (unit: millimeter)*/
    double FocusMin = DBL_MAX;          /** @brief minimum motion position of rough auto focus (unit: millimeter)*/
    double InfinityPosition = DBL_MAX;  /** @brief infinity position of the module (unit: millimeter)*/
    double FocalLength = DBL_MAX;       /** @brief focal length of the module (unit: millimeter)*/
    double FocalSpace = DBL_MAX;        /** @brief focal planes object space (unit: millimeter)*/
    double PixelSize = DBL_MAX;         /** @brief pixel size of the camera (unit: millimeter)*/
    double CoarseStep = DBL_MAX;        /** @brief motion position step during coarse auto focus (unit: millimeter)*/
    bool UseFineAdjust = false;         /** @brief use fine focus scan or not */
    double FineStep = DBL_MAX;          /** @brief motion position step during fine auto focus (unit: millimeter)*/
    double Freq = DBL_MAX;              /** @brief freq to calculate mtf*/
    std::vector<cv::Rect> ROIList = {}; /** @brief roi setting to calculate mtf*/
    bool UseChessMode = true;           /** @brief target type used to calculate mtf, true for chess board, false for crosshair*/
    bool UseLpmmUnit = true;            /** @brief unit used to calculate mtf, true for lp/mm, false for lp/degree*/
    bool SaveResultImg = false;         /** @brief save corase images and fine images to local*/
    cv::Rect SaveROI = cv::Rect(-1, -1, -1, -1); /** @brief save images as ROI*/
    std::string ImageSavePath = "";              /** @brief images save path*/
    int AverageCount = 1;                        /** @brief capture multi-frame, and calculate average*/
};

struct DiopterScanConfig {
    double DiopterMax = DBL_MAX;        /** @brief maximum diopter of rough focus (unit: diopter)*/
    double DiopterMin = DBL_MAX;        /** @brief minimum diopter of rough focus (unit: diopter)*/
    double InfinityPosition = DBL_MAX;  /** @brief infinity position of the module (unit: millimeter)*/
    double FocalLength = DBL_MAX;       /** @brief focal length of the module (unit: millimeter)*/
    double FocalSpace = DBL_MAX;        /** @brief focal planes object space (unit: millimeter)*/
    double PixelSize = DBL_MAX;         /** @brief pixel size of the camera (unit: millimeter)*/
    int CoarseStepNumber = INT_MAX;     /** @brief the step number of coarse diopter scan*/
    bool UseFineAdjust = false;         /** @brief use fine DIOPTER scan or not */
    int FineStepNumber = INT_MAX;       /** @brief the step number of fine diopter scan*/
    double Freq = DBL_MAX;              /** @brief freq to calculate mtf*/
    std::vector<cv::Rect> ROIList = {}; /** @brief roi setting to calculate mtf*/
    bool UseChessMode = true;           /** @brief target type used to calculate mtf, true for chess board, false for crosshair*/
    bool UseLpmmUnit = true;            /** @brief unit used to calculate mtf, true for lp/mm, false for lp/degree*/
    bool SaveResultImg = false;         /** @brief save images to local*/
    cv::Rect SaveROI = cv::Rect(-1, -1, -1, -1); /** @brief save images as ROI*/
    std::string ImageSavePath = "";              /** @brief images save path*/
    int AverageCount = 1;                        /** @brief capture multi-frame, and calculate average*/
};
}  // namespace AutoFocus
}  // namespace ML

#endif  // SRC_MLAUTOFOCUSPRCESS_MLAUTOFOCUSCOMMON_H_
