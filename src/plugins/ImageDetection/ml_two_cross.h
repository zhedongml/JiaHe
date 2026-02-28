#ifndef MLALGORITHM_MLTWOCROSS_H_
#define MLALGORITHM_MLTWOCROSS_H_

#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif

#include "ml_cross_center.h"
#include "LogPlus.h"

namespace algorithm {
	struct MLRect
	{
		std::vector<cv::Point> contour;
		cv::Point center = cv::Point(0,0);
		double len;
	};




	class ALGORITHM_API MLTwoCross: public MLCrossCenter
	{
	public:
		MLTwoCross();
		~MLTwoCross();
		cv::Point2f GetTwoCrossCenter(cv::Mat img);
		std::vector<cv::Point2f> GetTwoCrossROICenter(int off);

	private:
		cv::Mat img_;
		int img_rows_, img_cols_;
		cv::Point2f center_;
		cv::Point2f center_offset_ = cv::Point2f(0, 0);
		std::vector<cv::Point2f> roi_center_;

		MLRect CheckRect(const std::vector<std::vector<cv::Point>> contours, std::vector<cv::Vec4i> hierarchy);
		cv::Point GetCenter(const std::vector<cv::Point> contour);
		int GetLen(const std::vector<cv::Point> contour);
		double GetDistance(cv::Point a, cv::Point b);
	};




}


#endif // !MLALGORITHM_MLTWOCROSS_H_
