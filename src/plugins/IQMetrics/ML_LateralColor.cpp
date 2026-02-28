#include "pch.h"
#include "ML_LateralColor.h"
#include"ml_multiCrossHairDetection.h"
#include"LogPlus.h"
#include"ml_gridDetect.h"
#include<armadillo>
#include"ML_NineCrossDetection.h"
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

void MLIQMetrics::MLLateralColor::setFOVType(FOVTYPE type)
{
	m_FOVType = type;
}

void MLIQMetrics::MLLateralColor::setIsUpdateSLB(bool flag)
{
	m_IsUpdateSLB = flag;
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
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	int binNum = IQMetricUtl::instance()->getBinNum(rImg.size());
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
	grid.setAccurateDetectionFlag(true);
	grid.SetbinNum(binNum);
	GridRe gridR = grid.getGridContour(rImg);
	//if (gridR.flag == false)
	//	gridR = grid.getGridContour(rImg);
	if (gridR.flag == false)
	{
		re.flag = false;
		re.errMsg = info + "Red grid image detection fail" + gridR.errMsg;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Red grid image detection successfully");
	GridRe gridG = grid.getGridContour(gImg);
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
		writeMatTOCSV("./config/ALGConfig/lateralColor_subXRG.csv", subXRG);
		writeMatTOCSV("./config/ALGConfig/lateralColor_subYRG.csv", subYRG);
		writeMatTOCSV("./config/ALGConfig/lateralColor_subXRB.csv", subXRB);
		writeMatTOCSV("./config/ALGConfig/lateralColor_subYRB.csv", subYRB);
		writeMatTOCSV("./config/ALGConfig/lateralColor_subXGB.csv", subXGB);
		writeMatTOCSV("./config/ALGConfig/lateralColor_subYGB.csv", subXGB);
	}
	else
	{
		cv::Mat subXRGSLB = readCSVToMat("./config/ALGConfig/lateralColor_subXRG.csv");
		cv::Mat subYRGSLB = readCSVToMat("./config/ALGConfig/lateralColor_subYRG.csv");
		cv::Mat subXRBSLB = readCSVToMat("./config/ALGConfig/lateralColor_subXRB.csv");
		cv::Mat subYRBSLB = readCSVToMat("./config/ALGConfig/lateralColor_subYRB.csv");
		cv::Mat subXGBSLB = readCSVToMat("./config/ALGConfig/lateralColor_subXGB.csv");
		cv::Mat subYGBSLB = readCSVToMat("./config/ALGConfig/lateralColor_subYGB.csv");
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
	cv::Rect ROIRect = IQMetricsParameters::ROIRect;
	int binNum = IQMetricUtl::instance()->getBinNum(rImg.size());
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

LateralColorRe MLIQMetrics::MLLateralColor::getLateralColorCrossPreLoc(const cv::Mat rImg, const cv::Mat gImg, const cv::Mat bImg)
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
	if (m_FOVType == BIGFOV)
		ROIRect = cv::Rect(0, 0, -1, -1);
	cv::Mat bImgROI = GetROIMat(bImg, ROIRect);
	cv::Mat rImgROI = GetROIMat(rImg, ROIRect);
	cv::Mat gImgROI = GetROIMat(gImg, ROIRect);

	cv::Mat rimgdraw = convertToUint8(rImgROI);
	rimgdraw = convertTo3Channels(rimgdraw);
	cv::Mat gimgdraw = convertToUint8(gImgROI);
	gimgdraw = convertTo3Channels(gimgdraw);
	cv::Mat bimgdraw = convertToUint8(bImgROI);
	bimgdraw = convertTo3Channels(bimgdraw);

	MLNineCrossDetection cross;
	cross.setBinNum(binNum);
	map<string, cv::Point2f>cenMap = cross.getNineCrosshairLocation(rImgROI, m_FOVType);
	map<string, cv::Point2f>cenMapR =cross.getNineCrossLocationPreLoc(rImgROI, cenMap);
	map<string, cv::Point2f>cenMapG = cross.getNineCrossLocationPreLoc(gImgROI, cenMap);
	map<string, cv::Point2f>cenMapB = cross.getNineCrossLocationPreLoc(bImgROI, cenMap);
	if (cenMapB.size() != 9)
	{
		re.flag = false;
		re.errMsg = info + "Blue crosshair detection fail" ;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Blue crosshair detection successfully");

	if (cenMapR.size() != 9)
	{
		re.flag = false;
		re.errMsg = info + "Red crosshair detection fail" ;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Red crosshair detection successfully");
	if (cenMapG.size() != 9)
	{
		re.flag = false;
		re.errMsg = info + "Green crosshair detection fail" ;
		LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Green crosshair detection successfully");
	map<string, cv::Point2f>rg, rb, bg;
	//map<string, cv::Point2f>* it;
	vector<double>locxR, locyR, locxG, locyG, locxB, locyB;
	vector<double>subXRG, subYRG, subXRB, subYRB, subXGB, subYGB;
	vector<double>disVec;
	for (auto it = cenMapR.begin(); it != cenMapR.end(); ++it)
	{
		string str = it->first;
		cv::Point2f cenr = it->second;
		cv::Point2f ceng = cenMapG[str];
		cv::Point2f cenb = cenMapB[str];
		double disRB = Getdistance(cenr,cenb);
		double disGB = Getdistance(ceng, cenb);
		double disRG = Getdistance(cenr, ceng);

		disVec.push_back(disRB);
		disVec.push_back(disGB);
		disVec.push_back(disRG);
		locxR.push_back(cenr.x);
		locyR.push_back(cenr.y);
		locxG.push_back(ceng.x);
		locyG.push_back(ceng.y);
		locxB.push_back(cenb.x);
		locyB.push_back(cenb.y);
		subXRG.push_back((cenr.x - ceng.x));
		subYRG.push_back((cenr.y - ceng.y));
		subXRB.push_back((cenr.x - cenb.x));
		subYRB.push_back((cenr.y - cenb.y));
		subXGB.push_back((ceng.x - cenb.x));
		subYGB.push_back((ceng.y - cenb.y));

		//draw
		drawPointOnImage(rimgdraw, cenr);
		drawPointOnImage(gimgdraw, ceng);
		drawPointOnImage(bimgdraw, cenb);
		updateImgdraw(cenr + cv::Point2f(-1100 / binNum, 400 / binNum), rimgdraw, binNum);
		updateImgdraw(ceng + cv::Point2f(-1100 / binNum, 400 / binNum), gimgdraw, binNum);
		updateImgdraw(cenb + cv::Point2f(-1100 / binNum, 400 / binNum), bimgdraw, binNum);
	}
	re.subXRGArcmin = convertVecToMat(subXRB)* pixel2Arcmin;
	re.subYRGArcmin = convertVecToMat(subXRB) * pixel2Arcmin;
	re.subXRBArcmin = convertVecToMat(subXRB) * pixel2Arcmin;
	re.subYRBArcmin = convertVecToMat(subYRB) * pixel2Arcmin;
	re.subXGBArcmin = convertVecToMat(subXGB) * pixel2Arcmin;
	re.subYGBArcmin = convertVecToMat(subYGB) * pixel2Arcmin;
	re.locxB = convertVecToMat(locxB);
	re.locyB = convertVecToMat(locyB);
	re.locxR = convertVecToMat(locxR);
	re.locyR = convertVecToMat(locyR);
	re.locxG = convertVecToMat(locxG);
	re.locyG = convertVecToMat(locyG);
	re.maxdis = *max_element(disVec.begin(), disVec.end()) * pixel2Arcmin;
	re.meandis= accumulate(disVec.begin(), disVec.end(), 0.0) / disVec.size()*pixel2Arcmin;
	updateLateralColorRe(re, m_IsSLB, m_FOVType);
	re.imgdrawR = rimgdraw;
	re.imgdrawG = gimgdraw;
	re.imgdrawB = bimgdraw;
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Lateral color calculation successfully");
	return re;
}

void MLIQMetrics::MLLateralColor::updateImgdraw(cv::Point2f cen, cv::Mat& imgdraw, int binNum)
{
	string str = numToString(cen.x) + "," + numToString(cen.y);
	putTextOnImage(imgdraw, str, cen, 12 / binNum);
}

cv::Mat MLIQMetrics::MLLateralColor::convertVecToMat(vector<double> subvec)
{
	cv::Mat mtfmat = cv::Mat(3, 3, CV_64FC1, subvec.data()).clone();
	mtfmat.convertTo(mtfmat, CV_32FC1);
	return mtfmat;
}

void MLIQMetrics::MLLateralColor::updateLateralColorRe(LateralColorRe& re, bool isSLB, FOVTYPE type)
{

	if (isSLB)
	{
		if (type == BIGFOV&& m_IsUpdateSLB)
		{
			std::mutex mtx;
			mtx.lock();
			writeMatTOCSV("./config/ALGConfig/BIGFOV_lateralColor_subXRG.csv", re.subXRGArcmin);
			writeMatTOCSV("./config/ALGConfig/BIGFOV_lateralColor_subYRG.csv", re.subYRGArcmin);
			writeMatTOCSV("./config/ALGConfig/BIGFOV_lateralColor_subXRB.csv", re.subXRBArcmin);
			writeMatTOCSV("./config/ALGConfig/BIGFOV_lateralColor_subYRB.csv", re.subYRBArcmin);
			writeMatTOCSV("./config/ALGConfig/BIGFOV_lateralColor_subXGB.csv", re.subXGBArcmin);
			writeMatTOCSV("./config/ALGConfig/BIGFOV_lateralColor_subYGB.csv", re.subXGBArcmin);
			mtx.unlock();
		}
		else if (type == SMALLFOV&&m_IsUpdateSLB)
		{
			std::mutex mtx;
			mtx.lock();
			writeMatTOCSV("./config/ALGConfig/SMALLFOV_lateralColor_subXRG.csv", re.subXRGArcmin);
			writeMatTOCSV("./config/ALGConfig/SMALLFOV_lateralColor_subYRG.csv", re.subYRGArcmin);
			writeMatTOCSV("./config/ALGConfig/SMALLFOV_lateralColor_subXRB.csv", re.subXRBArcmin);
			writeMatTOCSV("./config/ALGConfig/SMALLFOV_lateralColor_subYRB.csv",re.subYRBArcmin);
			writeMatTOCSV("./config/ALGConfig/SMALLFOV_lateralColor_subXGB.csv", re.subXGBArcmin);
			writeMatTOCSV("./config/ALGConfig/SMALLFOV_lateralColor_subYGB.csv", re.subXGBArcmin);
			mtx.unlock();
		}

	}
	else
	{
		cv::Mat subXRGSLB, subYRGSLB, subXRBSLB, subYRBSLB, subXGBSLB, subYGBSLB;
		if (type == BIGFOV)
		{
			std::mutex mtx;
			mtx.lock();
			 subXRGSLB = readCSVToMat("./config/ALGConfig/BIGFOV_lateralColor_subXRG.csv");
			 subYRGSLB = readCSVToMat("./config/ALGConfig/BIGFOV_lateralColor_subYRG.csv");
			 subXRBSLB = readCSVToMat("./config/ALGConfig/BIGFOV_lateralColor_subXRB.csv");
			 subYRBSLB = readCSVToMat("./config/ALGConfig/BIGFOV_lateralColor_subYRB.csv");
			 subXGBSLB = readCSVToMat("./config/ALGConfig/BIGFOV_lateralColor_subXGB.csv");
			 subYGBSLB = readCSVToMat("./config/ALGConfig/BIGFOV_lateralColor_subYGB.csv");
			 mtx.unlock();
		}
		else if (type == SMALLFOV)
		{
			std::mutex mtx;
			mtx.lock();
			 subXRGSLB = readCSVToMat("./config/ALGConfig/SMALLFOV_lateralColor_subXRG.csv");
			 subYRGSLB = readCSVToMat("./config/ALGConfig/SMALLFOV_lateralColor_subYRG.csv");
			 subXRBSLB = readCSVToMat("./config/ALGConfig/SMALLFOV_lateralColor_subXRB.csv");
			 subYRBSLB = readCSVToMat("./config/ALGConfig/SMALLFOV_lateralColor_subYRB.csv");
			 subXGBSLB = readCSVToMat("./config/ALGConfig/SMALLFOV_lateralColor_subXGB.csv");
			 subYGBSLB = readCSVToMat("./config/ALGConfig/SMALLFOV_lateralColor_subYGB.csv");
			 mtx.unlock();
		}
		re.subXRGArcmin = re.subXRGArcmin - subXRGSLB;
		re.subYRGArcmin = re.subYRGArcmin - subYRGSLB;
		re.subXRBArcmin = re.subXRBArcmin - subXRBSLB;
		re.subYRBArcmin = re.subYRBArcmin - subYRBSLB;
		re.subXGBArcmin = re.subXGBArcmin - subXGBSLB;
		re.subYGBArcmin = re.subYGBArcmin - subYGBSLB;
		cv::Mat disRG, disRB, disGB;
		disRG = re.subXRGArcmin * re.subXRGArcmin + re.subYRGArcmin * re.subYRGArcmin;
		cv::sqrt(disRG, disRG);
		disRB = re.subXRBArcmin * re.subXRBArcmin + re.subYRBArcmin * re.subYRBArcmin;
		cv::sqrt(disRB, disRB);
		disGB = re.subXGBArcmin * re.subXGBArcmin + re.subYGBArcmin * re.subYGBArcmin;
		cv::sqrt(disGB, disGB);
		cv::Mat dism;
		cv::hconcat(disRG, disRB, dism);
		cv::hconcat(dism, disGB, dism);
		re.meandis = cv::mean(dism)(0);
		double maxV;
		cv::minMaxLoc(dism, NULL, &maxV);
		re.maxdis = maxV;
	}
}
