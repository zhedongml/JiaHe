#pragma once
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include <vector>
#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif

namespace MLImageDetection
{
	class ALGORITHM_API ImageFilter
	{
	public:
		ImageFilter();
		~ImageFilter();
	public:

		void GaussianLowPassFilter(const cv::Mat src, cv::Mat &dst);
		void dftML(cv::Mat input_image, cv::Mat& output_image, cv::Mat& transform_image);
	};
}
