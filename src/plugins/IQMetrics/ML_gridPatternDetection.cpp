#include "pch.h"
#include "ML_gridPatternDetection.h"
#include"ml_image_public.h"
using namespace MLImageDetection;
using namespace MLIQMetrics;
using namespace cv;
MLIQMetrics::MLGridPatternDetection::MLGridPatternDetection()
{
}

MLIQMetrics::MLGridPatternDetection::~MLGridPatternDetection()
{
}

void MLIQMetrics::MLGridPatternDetection::setFOVType(FOVTYPE type)
{
	m_FOVType = type;
}

void MLIQMetrics::MLGridPatternDetection::setBinNum(int num)
{
	m_binNum = num;
}

MLImageDetection::GridRe MLIQMetrics::MLGridPatternDetection::getGridLoc(cv::Mat img)
{
	GridRe gridRe;
	if (isGridDetectionFlagBig && m_FOVType == BIGFOV)
		gridRe = readGridLocation(BIGFOV);
	else if (isGridDetectionFlagSmall && m_FOVType == SMALLFOV)
		gridRe = readGridLocation(SMALLFOV);
	else
	{
		gridRe=getGridLocRe(img);
	}

	return gridRe;
}

MLImageDetection::GridRe MLIQMetrics::MLGridPatternDetection::getGridLocRe(cv::Mat img)
{
	MLimagePublic pl;
	int binNum = m_binNum;
	//int binNum = IQMetricUtl::instance()->getBinNum(img.size());
	//cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	//pl.updateRectByRatio1(ROIRect, 1.0 / binNum);
	//img = pl.GetROIMat(img, ROIRect);
	MLGridDetect grid;
	grid.setAccurateDetectionFlag(false);
	cv::Mat img8 = pl.convertToUint8(img);
	cv::Mat imgResize;
	int resizeNum = IQMetricsParameters::ResizeNum / binNum;
	int reiseNum = resizeNum / binNum;
	cv::resize(img8, imgResize, img.size() / reiseNum);
	grid.SetbinNum(resizeNum);
	grid.SetChessboardUpdateFlag(false);
	GridRe gridRe = grid.getGridContour(imgResize);
	gridRe.xLocMat = gridRe.xLocMat * resizeNum;
	gridRe.yLocMat = gridRe.yLocMat * resizeNum;
	return gridRe;
}

void MLIQMetrics::MLGridPatternDetection::saveGridLocation(FOVTYPE type, MLImageDetection::GridRe re)
{
	MLimagePublic pl;
	string fovstr = IQMetricUtl::instance()->fovTypeToString(m_FOVType);
	std::mutex mtx;
	mtx.lock();
	string filepathx= "./config/ALGConfig/"+fovstr+"_gridLocx.csv";
	string filepathy = "./config/ALGConfig/" + fovstr + "_gridLocy.csv";
	pl.writeMatTOCSV(filepathx, re.xLocMat);
	pl.writeMatTOCSV(filepathy, re.yLocMat);
	mtx.unlock();
	if (m_FOVType == BIGFOV)
		isGridDetectionFlagBig = true;
	else if (m_FOVType == SMALLFOV)
		isGridDetectionFlagSmall = true;
}

MLImageDetection::GridRe MLIQMetrics::MLGridPatternDetection::readGridLocation(FOVTYPE type)
{
	MLImageDetection::GridRe re;
	MLimagePublic pl;
	string fovstr = IQMetricUtl::instance()->fovTypeToString(m_FOVType);
	string filepathx = "./config/ALGConfig/" + fovstr + "_gridLocx.csv";
	string filepathy = "./config/ALGConfig/" + fovstr + "_gridLocy.csv";
	re.xLocMat = pl.readCSVToMat(filepathx);
	re.yLocMat = pl.readCSVToMat(filepathy);
	return re;
}
