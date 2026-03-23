#ifndef MLALGORITHM_MLCROSSCENTER_H_
#define MLALGORITHM_MLCROSSCENTER_H_


#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>

#include <vector>
#include <stdlib.h>
#include<numeric>
#include <string>
#include <algorithm>
#include <functional>

#ifdef MLMATH_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif


namespace algorithm {

	class ALGORITHM_API MLCrossCenter
	{
	public:
		MLCrossCenter();
		~MLCrossCenter();
		//MLCrossCenter(Mat img, int off=100, int size = 17);
		cv::Point2f GetCrossCenter(cv::Mat img, int size = 17);
		std::vector<cv::Point2f> GetROICenter(int off);
		int GetLineWidth();
		cv::Point2f CalculateCross(std::vector<cv::Vec4i>lines, int& width1, int& width2);

	private:

		std::vector<float> my_angle1_, my_angle2_;
		cv::Point2f center_;
		cv::Point2f center_offset_ = cv::Point2f(0, 0);
		cv::Mat img_;
		int img_rows_, img_cols_;
		int off_;
		//int roiLen;
		int size_;
		int line_width_;
		std::vector<cv::Point2f> roi_center_;
		float a1_, b1_, c1_, a2_, b2_, c2_;

		std::vector<cv::Point2f> FindROICenter(cv::Point2f center, float a1, float b1, float c1, float a2, float b2, float c2);
		cv::Point2f CalculateLinesCross(float a1, float b1, float c1, float a2, float b2, float c2);
		int CalculateLine(std::vector<cv::Vec4i>& lines, std::vector<float>& ks, float& a, float& b, float& c);
		void SeparateLines(std::vector<cv::Vec4i>& lines, std::vector<cv::Vec4i>& line1, std::vector<float>& ks1, std::vector<cv::Vec4i>& line2, std::vector<float>& ks2);
		bool FindCenter();
		float STDVector(std::vector<float> resultSet);
		void ReduceError(std::vector<float>& x);
	};
}
#endif // !MLALGORITHM_MLCROSSCENTER_H_