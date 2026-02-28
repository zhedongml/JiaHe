#pragma once

#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include "IQMetricUtl.h"
#include"ml_gridDetect.h"
namespace MLIQMetrics
{
	static bool isGridDetectionFlagBig = false;
	static bool isGridDetectionFlagSmall = false;
	class MLGridPatternDetection
	{
	public:
		MLGridPatternDetection();
		~MLGridPatternDetection();
		void setFOVType(FOVTYPE type);
		void setBinNum(int num);
		MLImageDetection::GridRe getGridLoc(cv::Mat img);
		MLImageDetection::GridRe getGridLocRe(cv::Mat img);

	private:
		void saveGridLocation(FOVTYPE type, MLImageDetection::GridRe re);
		MLImageDetection::GridRe readGridLocation(FOVTYPE type);
		int m_len = 300;
		FOVTYPE m_FOVType;
		int m_binNum = 1;
	};
}


