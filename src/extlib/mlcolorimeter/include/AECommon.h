// Copyright 2025 MLOptic
#ifndef LOGIC_AECOMMON_H_
#define LOGIC_AECOMMON_H_

#include <string>
#include <map>
#include <vector>
#include <opencv2/opencv.hpp>

using std::map;
using std::vector;
using std::string;
using cv::Mat;

#pragma warning(disable : 4251)
#pragma warning(disable : 4244)
#pragma warning(disable : 4838)

namespace ML {
	namespace AECommon {
		enum class AEStatus {
			AE_normal,
			AE_max_iterations_reached,
			AE_too_dark,
			AE_too_bright,
			AE_single_acquisition_error,
			AE_continuous_acquisition_error,
			AE_stop_acquisition_error,
			AE_None,
			AE_time_out
		};

		enum class GrayCalculationMode {
			average,
			weighted_average,
		};

		enum class GrayStatus {
			gray_target,
			gray_normal,
			gray_near_normal_min,
			gray_near_normal_max,
			gray_large,
			gray_small,
			too_dark,
			too_bright
		};

		struct AEInfo {
			AEStatus k_AEstatus;
			cv::Mat final_img;
			double exposure_time;
			int iteration;
			int take_time;
			bool AE_done;
		};

		struct FixedParameters_ {
			int bit_depth;
			double dark_level;
			double dark_time;
			double dynamic_range;
			int multiChannel_roi;
		};

		struct InitialParameters_ {
			bool manualInitTime;
			int InitialTime;
			// Deprecated
			bool isAutoUpdateInitTime;
			// Deprecated
			bool isAutoUpdateMaxMinTime;
			int rate;
		};

		struct JudgmentMechanism_ {
			int max_time;
			int min_time;
			double rangeMax;
			double rangeMin;
			double target_max;
			double target_min;
		};

		struct IterationStepLength_ {
			double belowNormalRange;
			double aboveNormalRange;
			int totalIterations;
		};

		struct ROIS {
			std::string name;
			double weight;
			int x;
			int y;
			int width;
			int height;
		};

		struct CalculateGrayParameters_ {
			bool useROIs;
			bool calEntireROIGray;
			int pixelCount;
			bool useWeightedAverage;
			std::map<std::string, ROIS> rois;
		};

		struct BadPixelsParameters_
		{
			bool filtBadPixels;
			int badPixelCount;
		};

		struct AEParam {
			FixedParameters_ fixedParameters;
			InitialParameters_ initialParameters;
			JudgmentMechanism_ judgmentMechanism;
			IterationStepLength_ iterationStepLength;
			CalculateGrayParameters_ calGrayParameters;
			BadPixelsParameters_ badPixelsParameters;
			std::string Name;
			std::string SN;
		};
	}
}

#endif  // LOGIC_AECOMMON_H_
