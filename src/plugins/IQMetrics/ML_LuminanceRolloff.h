#pragma once
#ifdef IQMETRICS_EXPORTS
#define IQMETRICS_API __declspec(dllexport)
#else
#define IQMETRICS_API __declspec(dllimport)
#endif
#include "IQMetricUtl.h"
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
namespace MLIQMetrics
{
	struct RolloffRe
	{
		double rollOffH;
		double rollOffV;
		cv::Mat imgdraw;
		bool flag = true;
		string errMsg = "";
	};
	struct RolloffJiaXingRe
	{
		double CV = 0;
		double meanLumi;
		double p50 = 0;
		double p5 = 0;
		double p95 = 0;
		double ralativeBrightnessLow = 0;
		double ralativeBrightnessHigh = 0;
		cv::Mat imgdraw;
		bool flag = true;
		string errMsg = "";
	};

	class IQMETRICS_API MLLuminanceRolloff : public MLImageDetection::MLimagePublic
	{
	public:
		MLLuminanceRolloff();
		~MLLuminanceRolloff();
	public:
		void setIsUpdateSLB(bool flag);
		void setFOVType(FOVTYPE type);
		void setIsSLB(bool flag);
		void setColor(string color);
		RolloffJiaXingRe getRelativeBrightness(const cv::Mat img);
		RolloffRe LuminanceRolloffRotation(cv::Mat img);
	private:
		void saveSLBLuminane(double lumi);

	private:
		double m_rotationAngle = 0;
		bool m_isSLB = false;
		FOVTYPE m_fovType;
		string m_color;
		bool m_IsUpdateSLB = false;
	};
} // namespace MLIQMetrics