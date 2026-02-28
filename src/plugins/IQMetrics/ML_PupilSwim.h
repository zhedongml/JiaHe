#pragma once
#ifdef IQMETRICS_EXPORTS
#define IQMETRICS_API __declspec(dllexport)
#else
#define IQMETRICS_API __declspec(dllimport)
#endif
//#include "ml_image_public.h"
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include"ImageDetection/MLCherkerboardDetect.h"
#include "IQMetricUtl.h"
namespace MLIQMetrics
{
	struct PupilSwimRe
	{
		double rms=0;
		double p90 = 0;
		cv::Mat xLoc1;
		cv::Mat yLoc1;
		cv::Mat xLoc2;
		cv::Mat yLoc2;
		cv::Mat imgdraw1;
		cv::Mat imgdraw2;
		bool flag = true;
		string errMsg = "";
	}; 
	class IQMETRICS_API MLPupilSwim:public MLImageDetection::MLimagePublic
	{
	public:
		MLPupilSwim();
		~MLPupilSwim();
	public:
		void setFOVType(FOVTYPE type);
		PupilSwimRe getPupilSwim(const cv::Mat img1,const cv::Mat img2);
		void updateCornersExactly(cv::Mat img, MLImageDetection::CheckerboardRe& checkerRe,cv::Mat&imgdraw);
		cv::Point2f getCornerLocation(cv::Mat roi);
		cv::Point2f getCornerLocation1(cv::Mat roi);

	private:
		 int m_binNum = 1;
		 int m_len = 80;
		 cv::Size m_size=cv::Size(39, 29);
		 FOVTYPE m_fovType;
	};
}
