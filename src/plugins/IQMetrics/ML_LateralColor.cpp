#include "pch.h"
#include "ML_LateralColor.h"
#include"ml_multiCrossHairDetection.h"
#include"LogPlus.h"
#include"ml_gridDetect.h"
#include<armadillo>
using namespace MLImageDetection;
using namespace MLIQMetrics;
using namespace cv;

MLIQMetrics::MLLateralColor::MLLateralColor()
{
}

MLIQMetrics::MLLateralColor::~MLLateralColor()
{
}

void MLIQMetrics::MLLateralColor::setIsSLB(bool flag)
{
	m_IsSLB = flag;
}

void MLIQMetrics::MLLateralColor::setPatternCenter(cv::Point2f cen)
{
	m_center = cen;
}

void MLIQMetrics::MLLateralColor::setIsUpdateSLB(bool flag)
{
	m_updateSLB = flag;
}

LateralColorRe MLIQMetrics::MLLateralColor::getLateralColorGrid(cv::Mat rImg, cv::Mat gImg, cv::Mat bImg)
{

	string info = "--getLateralColorGrid---";
	LateralColorRe re;
	double rotation = -1;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "LateralColor calculate start");
	if (rImg.empty() || gImg.empty() || bImg.empty())
	{
		re.flag = false;
		re.errMsg = info + "Input image is NULL!";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(rImg.size());
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	double pixel2Arcmin = IQMetricUtl::instance()->getPix2Arcmin(rImg.size());
	updateRectByRatio1(ROIRect, 1.0 / binNum);
	bImg = GetROIMat(bImg, ROIRect);
	rImg = GetROIMat(rImg, ROIRect);
	gImg = GetROIMat(gImg, ROIRect);
	cv::Mat rImg8 = convertToUint8(rImg);
	cv::Mat rImgdraw = convertTo3Channels(rImg8);
	cv::Mat gImg8 = convertToUint8(gImg);
	cv::Mat gImgdraw = convertTo3Channels(gImg8);
	cv::Mat bImg8 = convertToUint8(bImg);
	cv::Mat bImgdraw = convertTo3Channels(bImg8);
	cv::Mat imgdraw = rImgdraw * 0.5 + gImgdraw * 0.5 + bImgdraw * 0.5;

	MLGridDetect grid;
	grid.setAccurateDetectionFlag(false);
	grid.SetbinNum(binNum);
	GridRe gridR = grid.getGridContour(rImg);
	if (gridR.flag == false)
	{
		re.flag = false;
		re.errMsg = info + "Red grid image detection fail" + gridR.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Red grid image detection successfully");
	GridRe gridG = grid.getGridContour(gImg);
	//GridRe gridG = grid.getGridPreLoc(gImg,gridR.xLocMat,gridR.yLocMat);

	//if (gridG.flag == false)
	//	gridG = grid.getGridContour(gImg);
	if (gridG.flag == false)
	{
		re.flag = false;
		re.errMsg = info + "Green grid image detection fail" + gridG.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Green grid image detection successfully");
	GridRe gridB = grid.getGridContour(bImg);
	//GridRe gridB = grid.getGridPreLoc(bImg,gridR.xLocMat,gridR.yLocMat);

	//if(gridB.flag==false)
	//	gridB = grid.getGridContour(bImg);
	if (gridB.flag == false)
	{
		re.flag = false;
		re.errMsg = info + "Blue grid image detection fail" + gridB.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Blue grid image detection successfully");


	re.locxR = gridR.xLocMat;
	re.locyR = gridR.yLocMat;
	re.locxG = gridG.xLocMat;
	re.locyG = gridG.yLocMat;
	re.locxB = gridB.xLocMat;
	re.locyB = gridB.yLocMat;
	cv::Mat xlocMatR = gridR.xLocMat;
	cv::Mat ylocMatR = gridR.yLocMat;
	cv::Mat xlocMatG = gridG.xLocMat;
	cv::Mat ylocMatG = gridG.yLocMat;
	cv::Mat xlocMatB = gridB.xLocMat;
	cv::Mat ylocMatB = gridB.yLocMat;

	cv::Mat subXRG = xlocMatR - xlocMatG;
	cv::Mat subYRG = ylocMatR - ylocMatG;
	cv::Mat subXRB = xlocMatR - xlocMatB;
	cv::Mat subYRB = ylocMatR - ylocMatB;
	cv::Mat subXGB = xlocMatG - xlocMatB;
	cv::Mat subYGB = ylocMatG - ylocMatB;

	if (m_IsSLB == true)
	{
		std::mutex mtx;
		mtx.lock();
		writeMatTOCSV("./config/AlgConfig/slbInfo/lateralColor_subXRG.csv", subXRG);
		writeMatTOCSV("./config/AlgConfig/slbInfo/lateralColor_subYRG.csv", subYRG);
		writeMatTOCSV("./config/AlgConfig/slbInfo/lateralColor_subXRB.csv", subXRB);
		writeMatTOCSV("./config/AlgConfig/slbInfo/lateralColor_subYRB.csv", subYRB);
		writeMatTOCSV("./config/AlgConfig/slbInfo/lateralColor_subXGB.csv", subXGB);
		writeMatTOCSV("./config/AlgConfig/slbInfo/lateralColor_subYGB.csv", subXGB);
		mtx.unlock();
	}
	else
	{
		cv::Mat subXRGSLB = readCSVToMat("./config/AlgConfig/slbInfo/lateralColor_subXRG.csv");
		cv::Mat subYRGSLB = readCSVToMat("./config/AlgConfig/slbInfo/lateralColor_subYRG.csv");
		cv::Mat subXRBSLB = readCSVToMat("./config/AlgConfig/slbInfo/lateralColor_subXRB.csv");
		cv::Mat subYRBSLB = readCSVToMat("./config/AlgConfig/slbInfo/lateralColor_subYRB.csv");
		cv::Mat subXGBSLB = readCSVToMat("./config/AlgConfig/slbInfo/lateralColor_subXGB.csv");
		cv::Mat subYGBSLB = readCSVToMat("./config/AlgConfig/slbInfo/lateralColor_subYGB.csv");
		subXRG = subXRG - subXRGSLB;
		subYRG = subYRG - subYRGSLB;
		subXRB = subXRB - subXRBSLB;
		subYRB = subYRB - subYRBSLB;
		subXGB = subXGB - subXGBSLB;
		subYGB = subYGB - subYGBSLB;
	}



	vector<double>disVec;
	for (int i = 0; i < gridR.xLocMat.rows; i++)
	{
		for (int j = 0; j < gridR.xLocMat.cols; j++)
		{
			//cv::Point2f ptR, ptG, ptB;
			//ptR.x = gridR.xLocMat.at<float>(i, j);
			//ptR.y = gridR.yLocMat.at<float>(i, j);
			//ptG.x = gridG.xLocMat.at<float>(i, j);
			//ptG.y = gridG.yLocMat.at<float>(i, j);
			//ptB.x = gridB.xLocMat.at<float>(i, j);
			//ptB.y = gridB.yLocMat.at<float>(i, j);
			//double disRB = Getdistance(ptR, ptB);
			//double disGB = Getdistance(ptG, ptB);
			//double disRG = Getdistance(ptR, ptG);

			cv::Point2f ptRB(subXRB.at<float>(i, j), subYRB.at<float>(i, j));
			cv::Point2f ptRG(subXRG.at<float>(i, j), subYRG.at<float>(i, j));
			cv::Point2f ptGB(subXGB.at<float>(i, j), subYGB.at<float>(i, j));
			double disRB = sqrt(ptRB.x * ptRB.x + ptRB.y * ptRB.y);
			double disGB = sqrt(ptGB.x * ptGB.x + ptGB.y * ptGB.y);
			double disRG = sqrt(ptRG.x * ptRG.x + ptRG.y * ptRG.y);

			disVec.push_back(disRB);
			disVec.push_back(disGB);
			disVec.push_back(disRG);
			//circle(imgdraw, ptR, 1, Scalar(0, 0, 255), -1);
			//circle(imgdraw, ptG, 1, Scalar(0, 255, 0), -1);
			//circle(imgdraw, ptB, 1, Scalar(255, 0, 0), -1);
		}
	}



	double max = *max_element(disVec.begin(), disVec.end());
	double mean = accumulate(disVec.begin(), disVec.end(), 0.0) / disVec.size();
	re.subXRGArcmin = subXRG * pixel2Arcmin;
	re.subYRGArcmin = subYRG * pixel2Arcmin;
	re.subXRBArcmin = subXRB * pixel2Arcmin;
	re.subYRBArcmin = subYRB * pixel2Arcmin;
	re.subXGBArcmin = subXGB * pixel2Arcmin;
	re.subYGBArcmin = subYGB * pixel2Arcmin;

	re.maxdis = max * pixel2Arcmin;
	re.meandis = mean * pixel2Arcmin;
	re.imgdrawR = gridR.imgdraw;
	re.imgdrawG = gridG.imgdraw;
	re.imgdrawB = gridB.imgdraw;
	re.imgdraw = imgdraw;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Lateral color calculation successfully");
	return re;


}

LateralColorRe MLIQMetrics::MLLateralColor::getLateralColorGridNew(cv::Mat rImg, cv::Mat gImg, cv::Mat bImg)
{
	string info = "--getLateralColorGrid---";
	LateralColorRe re;
	double rotation = -1;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "LateralColor calculate start");
	if (rImg.empty() || gImg.empty() || bImg.empty())
	{
		re.flag = false;
		re.errMsg = info + "Input image is NULL!";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	rImg = IQMetricUtl::instance()->getRotationAndFlipImg(rImg, m_IsSLB);
	gImg = IQMetricUtl::instance()->getRotationAndFlipImg(gImg, m_IsSLB);
	bImg = IQMetricUtl::instance()->getRotationAndFlipImg(bImg, m_IsSLB);
	int binNum = IQMetricUtl::instance()->getBinNum(rImg.size());
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	double pixel2Arcmin = IQMetricUtl::instance()->getPix2Arcmin(rImg.size());
	//updateRectByRatio1(ROIRect, 1.0 / binNum);
	//bImg = GetROIMat(bImg, ROIRect);
	//rImg = GetROIMat(rImg, ROIRect);
	//gImg = GetROIMat(gImg, ROIRect);


	int binNumCen = IQMetricUtl::instance()->getBinNum(cv::Size(m_center.x * 2, m_center.y * 2));
	double ratio = double(binNumCen) / double(binNum);
	cv::Point2f cen = m_center * ratio;
	int len = m_len / binNum;
	cv::Rect rect0(cen.x - len / 2, cen.y - len / 2, len, len);
	bImg = GetROIMat(bImg, rect0);
	rImg = GetROIMat(rImg, rect0);
	gImg = GetROIMat(gImg, rect0);


	cv::Mat rImg8 = convertToUint8(rImg);
	cv::Mat rImgdraw = convertTo3Channels(rImg8);
	cv::Mat gImg8 = convertToUint8(gImg);
	cv::Mat gImgdraw = convertTo3Channels(gImg8);
	cv::Mat bImg8 = convertToUint8(bImg);
	cv::Mat bImgdraw = convertTo3Channels(bImg8);
	cv::Mat imgdraw = rImgdraw * 0.5 + gImgdraw * 0.5 + bImgdraw * 0.5;

	MLGridDetect grid;
	grid.setAccurateDetectionFlag(false);
	grid.SetbinNum(binNum);
	GridRe gridR = grid.getGridContour(rImg);
	if (gridR.flag == false)
	{
		re.flag = false;
		re.errMsg = info + "Red grid image detection fail" + gridR.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Red grid image detection successfully");
	//GridRe gridG = grid.getGridContour(gImg);
	GridRe gridG = grid.getGridPreLoc(gImg, gridR.xLocMat, gridR.yLocMat);

	if (gridG.flag == false)
	{
		re.flag = false;
		re.errMsg = info + "Green grid image detection fail" + gridG.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Green grid image detection successfully");
	//GridRe gridB = grid.getGridContour(bImg);
	GridRe gridB = grid.getGridPreLoc(bImg, gridR.xLocMat, gridR.yLocMat);

	if (gridB.flag == false)
	{
		re.flag = false;
		re.errMsg = info + "Blue grid image detection fail" + gridB.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Blue grid image detection successfully");

	int row = gridR.xLocMat.rows;
	int col = gridR.xLocMat.cols;
	re.locxR = gridR.xLocMat(cv::Range(1, row - 1), Range(1, col - 1));
	re.locyR = gridR.yLocMat(cv::Range(1, row - 1), Range(1, col - 1));;
	re.locxG = gridG.xLocMat(cv::Range(1, row - 1), Range(1, col - 1));
	re.locyG = gridG.yLocMat(cv::Range(1, row - 1), Range(1, col - 1));
	re.locxB = gridB.xLocMat(cv::Range(1, row - 1), Range(1, col - 1));
	re.locyB = gridB.yLocMat(cv::Range(1, row - 1), Range(1, col - 1));
	cv::Mat xlocMatR = gridR.xLocMat(cv::Range(1, row - 1), Range(1, col - 1));
	cv::Mat ylocMatR = gridR.yLocMat(cv::Range(1, row - 1), Range(1, col - 1));
	cv::Mat xlocMatG = gridG.xLocMat(cv::Range(1, row - 1), Range(1, col - 1));
	cv::Mat ylocMatG = gridG.yLocMat(cv::Range(1, row - 1), Range(1, col - 1));
	cv::Mat xlocMatB = gridB.xLocMat(cv::Range(1, row - 1), Range(1, col - 1));
	cv::Mat ylocMatB = gridB.yLocMat(cv::Range(1, row - 1), Range(1, col - 1));

	cv::Mat subXRG = xlocMatR - xlocMatG;
	cv::Mat subYRG = ylocMatR - ylocMatG;
	cv::Mat subXRB = xlocMatR - xlocMatB;
	cv::Mat subYRB = ylocMatR - ylocMatB;
	cv::Mat subXGB = xlocMatG - xlocMatB;
	cv::Mat subYGB = ylocMatG - ylocMatB;

	if (m_IsSLB == true)
	{
		std::mutex mtx;
		mtx.lock();
		writeMatTOCSV("./config/AlgConfig/slbInfo/lateralColor_subXRG.csv", subXRG);
		writeMatTOCSV("./config/AlgConfig/slbInfo/lateralColor_subYRG.csv", subYRG);
		writeMatTOCSV("./config/AlgConfig/slbInfo/lateralColor_subXRB.csv", subXRB);
		writeMatTOCSV("./config/AlgConfig/slbInfo/lateralColor_subYRB.csv", subYRB);
		writeMatTOCSV("./config/AlgConfig/slbInfo/lateralColor_subXGB.csv", subXGB);
		writeMatTOCSV("./config/AlgConfig/slbInfo/lateralColor_subYGB.csv", subXGB);
		mtx.unlock();
	}
	else
	{
		cv::Mat subXRGSLB = readCSVToMat("./config/AlgConfig/slbInfo/lateralColor_subXRG.csv");
		cv::Mat subYRGSLB = readCSVToMat("./config/AlgConfig/slbInfo/lateralColor_subYRG.csv");
		cv::Mat subXRBSLB = readCSVToMat("./config/AlgConfig/slbInfo/lateralColor_subXRB.csv");
		cv::Mat subYRBSLB = readCSVToMat("./config/AlgConfig/slbInfo/lateralColor_subYRB.csv");
		cv::Mat subXGBSLB = readCSVToMat("./config/AlgConfig/slbInfo/lateralColor_subXGB.csv");
		cv::Mat subYGBSLB = readCSVToMat("./config/AlgConfig/slbInfo/lateralColor_subYGB.csv");

		int row = subXRGSLB.rows;
		int col = subXRGSLB.cols;
		if (subXRGSLB.size() == cv::Size(15, 11))
		{
			subXRGSLB = subXRGSLB(cv::Range(1, row - 1), Range(1, col - 1));
			subYRGSLB = subYRGSLB(cv::Range(1, row - 1), Range(1, col - 1));
			subXRBSLB = subXRBSLB(cv::Range(1, row - 1), Range(1, col - 1));
			subYRBSLB = subYRBSLB(cv::Range(1, row - 1), Range(1, col - 1));
			subXGBSLB = subXGBSLB(cv::Range(1, row - 1), Range(1, col - 1));
			subYGBSLB = subYGBSLB(cv::Range(1, row - 1), Range(1, col - 1));
		}
		subXRG = subXRG - subXRGSLB;
		subYRG = subYRG - subYRGSLB;
		subXRB = subXRB - subXRBSLB;
		subYRB = subYRB - subYRBSLB;
		subXGB = subXGB - subXGBSLB;
		subYGB = subYGB - subYGBSLB;
	}



	vector<double>disVec;
	for (int i = 0; i < re.locxR.rows; i++)
	{
		for (int j = 0; j < re.locxR.cols; j++)
		{

			cv::Point2f ptRB(subXRB.at<float>(i, j), subYRB.at<float>(i, j));
			cv::Point2f ptRG(subXRG.at<float>(i, j), subYRG.at<float>(i, j));
			cv::Point2f ptGB(subXGB.at<float>(i, j), subYGB.at<float>(i, j));
			double disRB = sqrt(ptRB.x * ptRB.x + ptRB.y * ptRB.y);
			double disGB = sqrt(ptGB.x * ptGB.x + ptGB.y * ptGB.y);
			double disRG = sqrt(ptRG.x * ptRG.x + ptRG.y * ptRG.y);
			disVec.push_back(disRB);
			disVec.push_back(disGB);
			disVec.push_back(disRG);
		}
	}

	double max = *max_element(disVec.begin(), disVec.end());
	double mean = accumulate(disVec.begin(), disVec.end(), 0.0) / disVec.size();
	re.subXRGArcmin = subXRG * pixel2Arcmin;
	re.subYRGArcmin = subYRG * pixel2Arcmin;
	re.subXRBArcmin = subXRB * pixel2Arcmin;
	re.subYRBArcmin = subYRB * pixel2Arcmin;
	re.subXGBArcmin = subXGB * pixel2Arcmin;
	re.subYGBArcmin = subYGB * pixel2Arcmin;

	re.maxdis = max * pixel2Arcmin;
	re.meandis = mean * pixel2Arcmin;
	re.imgdrawR = gridR.imgdraw;
	re.imgdrawG = gridG.imgdraw;
	re.imgdrawB = gridB.imgdraw;
	re.imgdraw = imgdraw;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Lateral color calculation successfully");
	return re;




}

LateralColorRe MLIQMetrics::MLLateralColor::getLateralColorCross(cv::Mat rImg, cv::Mat gImg, cv::Mat bImg)
{
	string info = "--getLateralColor---";
	LateralColorRe re;
	double rotation = -1;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "LateralColor calculate start");
	if (rImg.empty() || gImg.empty() || bImg.empty())
	{
		re.flag = false;
		re.errMsg = info + "Input image is NULL!";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	int binNum = IQMetricUtl::instance()->getBinNum(rImg.size());
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	double pixel2Arcmin = IQMetricUtl::instance()->getPix2Arcmin(rImg.size());
	bImg = GetROIMat(bImg, ROIRect);
	rImg = GetROIMat(rImg, ROIRect);
	gImg = GetROIMat(gImg, ROIRect);

	MultiCrossHairDetection md;
	int crossBinNum = IQMetricsParameters::crossBinNum;
	md.setBinNum(binNum);
	MultiCrossHairRe cenB = md.getMuliCrossHairCenter(bImg, crossBinNum / binNum, true);
	if (cenB.flag == false)
	{
		re.flag = false;
		re.errMsg = info + "Blue crosshair detection fail" + cenB.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Blue crosshair detection successfully");

	MultiCrossHairRe cenR = md.getMuliCrossHairCenter(rImg, crossBinNum / binNum, true);
	if (cenR.flag == false)
	{
		re.flag = false;
		re.errMsg = info + "Red crosshair detection fail" + cenR.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Red crosshair detection successfully");

	MultiCrossHairRe cenG = md.getMuliCrossHairCenter(gImg, crossBinNum / binNum, true);
	if (cenG.flag == false)
	{
		re.flag = false;
		re.errMsg = info + "Green crosshair detection fail" + cenG.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Green crosshair detection successfully");
	map<string, cv::Point2f>rg, rb, bg;
	//map<string, cv::Point2f>* it;
	for (auto it = cenR.centerMap.begin(); it != cenR.centerMap.end(); ++it)
	{
		string str = it->first;
		cv::Point2f cenr = it->second;
		cv::Point2f ceng = cenG.centerMap[str];
		cv::Point2f cenb = cenB.centerMap[str];
		rg.insert(std::make_pair(str, cenr - ceng));
		//rb.insert(std::make_pair(str, cenr - cenb));
		bg.insert(std::make_pair(str, cenb - ceng));
		//draw
		updateImgdraw(cenr + cv::Point2f(-1100 / binNum, 400 / binNum), cenR.img_draw, binNum);
		updateImgdraw(ceng + cv::Point2f(-1100 / binNum, 400 / binNum), cenG.img_draw, binNum);
		updateImgdraw(cenb + cv::Point2f(-1100 / binNum, 400 / binNum), cenB.img_draw, binNum);
	}
	double grLCS = 0;
	double gbLCS = 0;
	vector<double>grDis, gbDis;
	for (int i = 0; i < m_str.size(); i++)
	{
		string str = m_str[i];
		cv::Point2f gr = rg[str];
		grDis.push_back(sqrt(gr.x * gr.x + gr.y * gr.y));
		cv::Point2f gb = bg[str];
		gbDis.push_back(sqrt(gb.x * gb.x + gb.y * gb.y));
	}
	grLCS = *max_element(grDis.begin(), grDis.end());
	gbLCS = *max_element(gbDis.begin(), gbDis.end());
	grLCS = grLCS * pixel2Arcmin;
	gbLCS = gbLCS * pixel2Arcmin;
	re.grLCS = grLCS;
	re.gbLCS = gbLCS;
	re.RLoc = cenR.centerMap;
	re.GLoc = cenG.centerMap;
	re.BLoc = cenB.centerMap;
	re.imgdrawR = cenR.img_draw;
	re.imgdrawG = cenG.img_draw;
	re.imgdrawB = cenB.img_draw;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Lateral color calculation successfully");
	return re;
}

LateralColorRe MLIQMetrics::MLLateralColor::getLateralColorGridCenter(const cv::Mat rImgRaw, const cv::Mat gImgRaw, const cv::Mat bImgRaw)
{

	string info = "--getLateralColorGridCenter---";
	LateralColorRe re;
	double rotation = -1;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "LateralColor calculate start");
	if (rImgRaw.empty() || gImgRaw.empty() || bImgRaw.empty())
	{
		re.flag = false;
		re.errMsg = info + "Input image is NULL!";
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	cv::Mat rImg = IQMetricUtl::instance()->getRotationAndFlipImg(rImgRaw, m_IsSLB);
	cv::Mat gImg = IQMetricUtl::instance()->getRotationAndFlipImg(gImgRaw, m_IsSLB);
	cv::Mat bImg = IQMetricUtl::instance()->getRotationAndFlipImg(bImgRaw, m_IsSLB);
	int binNum = IQMetricUtl::instance()->getBinNum(rImg.size());
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	double pixel2Arcmin = IQMetricUtl::instance()->getPix2Arcmin(rImg.size());
	updateRectByRatio1(ROIRect, 1.0 / binNum);
	bImg = GetROIMat(bImg, ROIRect);
	rImg = GetROIMat(rImg, ROIRect);
	gImg = GetROIMat(gImg, ROIRect);
	cv::Mat rImg8 = convertToUint8(rImg);
	cv::Mat rImgdraw = convertTo3Channels(rImg8);
	cv::Mat gImg8 = convertToUint8(gImg);
	cv::Mat gImgdraw = convertTo3Channels(gImg8);
	cv::Mat bImg8 = convertToUint8(bImg);
	cv::Mat bImgdraw = convertTo3Channels(bImg8);
	cv::Mat imgdraw = rImgdraw * 0.5 + gImgdraw * 0.5 + bImgdraw * 0.5;

	int resizeNum = IQMetricsParameters::GridResizeNum / binNum;
	cv::Mat imgResizeR, imgResizeG,imgResizeB;
	cv::resize(rImg8, imgResizeR, rImg8.size() / resizeNum);
	cv::resize(gImg8, imgResizeG,gImg8.size() / resizeNum);
	cv::resize(bImg8, imgResizeB, bImg8.size() / resizeNum);

	MLGridDetect grid;
	grid.SetbinNum(resizeNum);
	GridRe gridR = grid.getGridCenter(imgResizeR);
	//GridRe gridR = grid.getGridContour(imgResizeR);

	if (gridR.flag)
	{
		writeLateralGridCenter(gridR);
	}
	else
	{
		readLateralGridCenter(gridR);
	}


	grid.setAccurateDetectionFlag(true);
	grid.SetbinNum(binNum);
	gridR = grid.getGridCenterPreLoc(rImg8, gridR.center * resizeNum);
	if (gridR.flag == false)
	{
		re.flag = false;
		re.errMsg = info + "Red grid image detection fail" + gridR.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}

	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Red grid image detection successfully");
	//GridRe gridG = grid.getGridContour(gImg);
	GridRe gridG = grid.getGridCenterPreLoc(gImg8, gridR.center);
	//GridRe gridG = grid.getGridContour(imgResizeG);
	if (gridG.flag == false)
	{
		re.flag = false;
		re.errMsg = info + "Green grid image detection fail" + gridG.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Green grid image detection successfully");
	//GridRe gridB = grid.getGridContour(bImg);
	GridRe gridB = grid.getGridCenterPreLoc(bImg8, gridR.center);
	//GridRe gridB = grid.getGridContour(imgResizeB);

	if (gridB.flag == false)
	{
		re.flag = false;
		re.errMsg = info + "Blue grid image detection fail" + gridB.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Blue grid image detection successfully");

	cv::Mat subRG = (cv::Mat_<float>(1, 2) << gridR.center.x - gridG.center.x, gridR.center.y - gridG.center.y);
	cv::Mat subRB = (cv::Mat_<float>(1, 2) << gridR.center.x - gridB.center.x, gridR.center.y - gridB.center.y);
	cv::Mat subGB = (cv::Mat_<float>(1, 2) << gridG.center.x - gridB.center.x, gridG.center.y - gridB.center.y);

	if (m_IsSLB == true && m_updateSLB == true)
	{
		std::mutex mtx;
		mtx.lock();
		writeMatTOCSV("./config/AlgConfig/slbInfo/lateralColor_subRG.csv", subRG);
		writeMatTOCSV("./config/AlgConfig/slbInfo/lateralColor_subRB.csv", subRB);
		writeMatTOCSV("./config/AlgConfig/slbInfo/lateralColor_subGB.csv", subGB);
		mtx.unlock();
	}
	else
	{
		cv::Mat subRGSLB = readCSVToMat("./config/AlgConfig/slbInfo/lateralColor_subRG.csv");
		cv::Mat subRBSLB = readCSVToMat("./config/AlgConfig/slbInfo/lateralColor_subRB.csv");
		cv::Mat subGBSLB = readCSVToMat("./config/AlgConfig/slbInfo/lateralColor_subGB.csv");
		subRG = subRG - subRGSLB;
		subRB = subRB - subRBSLB;
		subGB = subGB - subGBSLB;
	}
	double disRG = sqrt(subRG.at<float>(0, 0) * subRG.at<float>(0, 0) + subRG.at<float>(0, 1) * subRG.at<float>(0, 1));
	double disRB = sqrt(subRB.at<float>(0, 0) * subRB.at<float>(0, 0) + subRB.at<float>(0, 1) * subRB.at<float>(0, 1));
	double disGB = sqrt(subGB.at<float>(0, 0) * subGB.at<float>(0, 0) + subGB.at<float>(0, 1) * subGB.at<float>(0, 1));
	vector<double>disvec;
	disvec.push_back(disRG);
	disvec.push_back(disRB);
	disvec.push_back(disGB);
	double max = *max_element(disvec.begin(), disvec.end());  // 求最大值
	double mean = (disRG + disRB + disGB) / 3.0;  // 均值
	re.maxdis = max * pixel2Arcmin;   
	re.meandis = mean * pixel2Arcmin;
	re.disRGArcmin = disRG * pixel2Arcmin;
	re.disRBArcmin = disRB * pixel2Arcmin;
	re.disGBArcmin = disGB * pixel2Arcmin;

	circle(imgdraw, gridR.center, 5, Scalar(0, 0, 255), -1);
	circle(imgdraw, gridG.center, 5, Scalar(0, 255, 0), -1);
	circle(imgdraw, gridB.center, 5, Scalar(255, 0, 0), -1);
	string strR = numToString(gridR.center.x) + "," + numToString(gridR.center.y);
	cv::putText(imgdraw, strR, gridR.center, FONT_HERSHEY_PLAIN, 16 / binNum, Scalar(0, 0, 255), 8 / binNum);
	string strG = numToString(gridG.center.x) + "," + numToString(gridG.center.y);
	cv::putText(imgdraw, strG, gridG.center+cv::Point2f(0,400/binNum), FONT_HERSHEY_PLAIN, 16 / binNum, Scalar(0, 255, 0), 8 / binNum);
	string strB = numToString(gridB.center.x) + "," + numToString(gridB.center.y);
	cv::putText(imgdraw, strB, gridB.center + cv::Point2f(0, 800 / binNum), FONT_HERSHEY_PLAIN, 16 / binNum, Scalar(255, 0, 0), 8 / binNum);
	cv::putText(imgdraw, "disRG(Arcmin):"+to_string(re.disRGArcmin), gridB.center + cv::Point2f(0, 1200 / binNum), FONT_HERSHEY_PLAIN, 16 / binNum, Scalar(255, 0, 255), 8 / binNum);
	cv::putText(imgdraw, "disRB(Arcmin):" + to_string(re.disRGArcmin), gridB.center + cv::Point2f(0, 1400 / binNum), FONT_HERSHEY_PLAIN, 16 / binNum, Scalar(255, 0, 255), 8 / binNum);
	cv::putText(imgdraw, "disGB(Arcmin):" + to_string(re.disRGArcmin), gridB.center + cv::Point2f(0, 1600 / binNum), FONT_HERSHEY_PLAIN, 16 / binNum, Scalar(255, 0, 255), 8 / binNum);
	cv::putText(imgdraw, "maxdis(Arcmin):" + to_string(re.maxdis), gridB.center + cv::Point2f(0, 1800 / binNum), FONT_HERSHEY_PLAIN, 16 / binNum, Scalar(255, 0, 255), 8 / binNum);
	cv::putText(imgdraw, "meandis(Arcmin):" + to_string(re.meandis), gridB.center + cv::Point2f(0, 2000 / binNum), FONT_HERSHEY_PLAIN, 16 / binNum, Scalar(255, 0, 255), 8 / binNum);
	re.imgdrawR = gridR.imgdraw;
	re.imgdrawG = gridG.imgdraw;
	re.imgdrawB = gridB.imgdraw;
	re.imgdraw = imgdraw;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Lateral color calculation successfully");
	return re;
}

void MLIQMetrics::MLLateralColor::updateImgdraw(cv::Point2f cen, cv::Mat& imgdraw, int binNum)
{
	string str = numToString(cen.x) + "," + numToString(cen.y);
	putTextOnImage(imgdraw, str, cen, 24 / binNum);
}

void MLIQMetrics::MLLateralColor::writeLateralGridCenter(GridRe re)
{
	string filepath = "./config/AlgConfig/slbInfo/LateralCen.csv";
	vector<double>cenVec;
	cenVec.push_back(re.center.x);
	cenVec.push_back(re.center.y);
	cv::Mat romat(cenVec);
	std::mutex mtx;
	mtx.lock();
	writeMatTOCSV(filepath, romat);
	mtx.unlock();
}

void MLIQMetrics::MLLateralColor::readLateralGridCenter(GridRe& re)
{
	string filepath = "./config/AlgConfig/slbInfo/LateralCen.csv";
	cv::Mat cenmat = readCSVToMat(filepath);
	re.center.x = cenmat.at<float>(0, 0);
	re.center.y = cenmat.at<float>(1, 0);
	re.flag = true;
}
