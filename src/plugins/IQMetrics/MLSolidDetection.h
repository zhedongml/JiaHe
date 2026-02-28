#pragma once
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include "IQMetricUtl.h"
#include"ml_rectangleDetection.h"
namespace MLIQMetrics
{
	static bool isSolidDetectionFlagBig = false;
	static bool isSolidDetectionFlagSmall = false;
	struct SolidDetectionRe
	{
		double rotationAngle;
		cv::Rect rectAf;
		bool flag = true;
		string errMsg = "";
	};
	class IQMETRICS_API MLSolidDetection
	{
	public:
		MLSolidDetection();
		~MLSolidDetection();
		void setBinNum(int num);
		void setFOVType(FOVTYPE type);
		SolidDetectionRe getSolidLocation(cv::Mat img);
		SolidDetectionRe getSolidDtectionRe(cv::Mat img);
		cv::Rect getSolidExactRect(cv::Mat img8, cv::   Rect rect);
		void setDetectionFlag(bool flag,FOVTYPE type);
	private:
		void saveSolidRect(FOVTYPE type, SolidDetectionRe re);
		SolidDetectionRe readSolidRect(FOVTYPE type);
	private:
		int m_len = 300;
		FOVTYPE m_FOVType;
		int m_binNum = 1;
	};
}

