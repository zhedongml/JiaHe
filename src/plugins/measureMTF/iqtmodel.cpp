#include "iqtmodel.h"
#include "GenCSVHeader.h"
#include "ImageRotationConfig.h"
#include "MLPythonSDKController/PyController.h"
#include "SlbManage.h"
#include "PrjCommon/Session.h"
#include "Core/loggingwrapper.h"
#include "PrjCommon/metricsdata.h"
//#include "grayscaleimagecapture.h"
#include "icecream.hpp"
#include "metricsdatabase.h"
#include "metricsdatacache.h"
#include <QFileInfo>
#include <QThread>
#include <QtConcurrent>
#include <filesystem>
#include <stdio.h>

// using namespace algorithm;
// using namespace algorithm;
using namespace MLIQMetrics;
IQTModel* IQTModel::self = nullptr;
QHash<QString, IQTModel::MetricCalculator> IQTModel::metricCalculators;

bool IQTModel::createDir(QString fullPath)
{
	QDir dir(fullPath);
	if (dir.exists())
	{
		return true;
	}
	else
	{
		bool ok = dir.mkpath(fullPath);
		return ok;
	}
}
IQTModel::IQTModel(QObject* parent) : QObject(parent)
{
	initPythonSDKCall();
	qRegisterMetaType<cv::Mat>("cv::Mat");
	qRegisterMetaType<std::vector<cv::Point>>("std::vector<cv::Point>");
	metricCalculators.clear();
	metricVec = IQMetricConfig::instance()->getMetricDescripts();
	MetricCalculator metricFunc = &IQTModel::calculateFOV;
	metricCalculators.insert("FOV", &IQTModel::calculateFOV);
	metricCalculators.insert("FOVBig", &IQTModel::calculateFOVBig);
	metricCalculators.insert("Rotation", &IQTModel::calculateRotation);
	metricCalculators.insert("FOVOffset", &IQTModel::calculateFOVOffset);
	metricCalculators.insert("GridDistortion", &IQTModel::calculateGridDistortion);
	metricCalculators.insert("SequentialContrast", &IQTModel::calculateSequentialContrast);
	metricCalculators.insert("LuminanceUniformity", &IQTModel::calculateLuminanceUniformity);
	metricCalculators.insert("LuminanceRolloff", &IQTModel::calculateLuminanceRolloff);
	metricCalculators.insert("CheckerContrast", &IQTModel::calculateSmallContrastChessboard);
	metricCalculators.insert("LuminanceEfficiency", &IQTModel::calculateLuminanceEfficiency);
	metricCalculators.insert("Flare", &IQTModel::calculateFlareC);
	metricCalculators.insert("FlareHDR", &IQTModel::calculateFlareHDRC);
	metricCalculators.insert("Ghost", &IQTModel::calculateGhostC);
	metricCalculators.insert("PupilSwim", &IQTModel::calculatePupilSwimC);
	metricCalculators.insert("GridDenseMTF", &IQTModel::calculateGridDenseMTF);
	metricCalculators.insert("2degCheckerDenseMTF", &IQTModel::calculate2degCheckerDenseMTF);
	metricCalculators.insert("0.5degCheckerDenseMTF", &IQTModel::calculate5degCheckerDenseMTF);
	metricCalculators.insert("CrossMTF", &IQTModel::calculateCrossMTF);
	metricCalculators.insert("LateralColor", &IQTModel::calculateLateralColor);
	metricCalculators.insert("ColorUniformity", &IQTModel::calculateColorUniformity);
	// python sdk call metrics
	//metricCalculators.insert("CheckboardDistortion", &IQTModel::calculateCheckboardDistortion);
  //  metricCalculators.insert("Ghost", &IQTModel::calculateGhost);
   // metricCalculators.insert("StrayLight", &IQTModel::calculateStrayLight);
   // metricCalculators.insert("PupilSwim", &IQTModel::calculatePupilSwim);
   // metricCalculators.insert("Flare", &IQTModel::calculateFlare);

}
QString IQTModel::calculateFOV(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);

	if (params.size() == 0)
	{
		return "";
	}
	cv::Mat img = params[0];

	if (img.data != NULL)
	{
		//FOVCenter infor = MetricsDataCache::getInstance()->getFOVCenter(color);

		clock_t start = clock();
		MLUtils::TestState state = MetricsData::instance()->GetTestState();
		FOVTYPE type = getFOVType();
		fov.setFOVType(type);
		MLIQMetrics::FovRe re = fov.getFOVSolid(img);
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		if (re.flag == false)
		{
			LoggingWrapper::instance()->error(QString("%1 calculation error,%2").arg(test_info).arg(QString::fromStdString(re.errMsg)));
			return "";
		}

		if (isDebug && re.FovH > 0)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}
			cv::Mat outImg = re.imgdraw;
			if (outImg.data != NULL)
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
			string csvName = (qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".csv").toStdString();
			fov.writePointsFile(csvName, re.EdgePoint);
		}
		//updateFOVPoints(re.EdgePoint);
		MetricsDataCache::getInstance()->updateFOVAngle(color, re.FovH, re.FovV, re.FovDiag);
		Mat src1 = (Mat_<double>(2, 1) << re.FovH, re.FovV);
		cv::Mat b;
		src1.convertTo(b, CV_32FC1);
		if (id != "")
		{
			IQTModel::updateCSVHeaderValues(id.toInt(), b, mtcName, color);
			emit updateResult(b);
		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "";
	}
}

QString IQTModel::calculateFOVBig(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);

	if (params.size() == 0)
	{
		return "";
	}
	cv::Mat img = params[0];
	if (img.data != NULL)
	{
		FOVCenter infor = MetricsDataCache::getInstance()->getFOVCenter(color);

		clock_t start = clock();
		MLIQMetrics::FovRe re = fov.getFOVSolid(img);
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		// emit updateFOVPoints(re.EdgePoint);

		if (isDebug & re.FovH > 0)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}
			cv::Mat outImg = re.imgdraw;
			// cross.getImgDraw();
			if (outImg.data != NULL)
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
			string csvName = (qTempPath + "\\" + mtcName + "Pts_" + color + "_eb" + id + ".csv").toStdString();
			fov.writePointsFile(csvName, re.EdgePoint);
		}
		updateFOVPoints(re.EdgePoint);
		MetricsDataCache::getInstance()->updateFOVAngle(color, re.FovH, re.FovV, re.FovDiag);
		Mat src1 = (Mat_<double>(2, 1) << re.FovH, re.FovV);
		cv::Mat b;
		src1.convertTo(b, CV_32FC1);
		if (id != "")
		{
			IQTModel::updateCSVHeaderValues(id.toInt(), b, mtcName, color);
			emit updateResult(b);
		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "";
	}
}

QString IQTModel::calculateFOVOffset(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);

	if (params.size() == 0)
	{
		return "";
	}
	cv::Mat img = params[0];
	if (img.data != NULL)
	{
		clock_t start = clock();
		bool flag = ImageRotationConfig::instance()->isRotation();
		MLIQMetrics::FovOffsetRe re;
		//if (!flag)
		//{
		//    re = offset.BoresightNoBorder(img);
		//}
		//else
		{
			MLIQMetrics::ROIParaNew para;
			//MLColorimeterMode::Instance()->getRotationMirror(para.rotationAngle, para.mirror);
			//bool flag = MetricsData::instance()->getIQSLB();
			MLUtils::TestState state = MetricsData::instance()->GetTestState();
			FOVTYPE type = getFOVType();
			//MLIQMetrics::MLFOVOffset offset;
			offset.setIsUpdateSLB(state.IsUpdateSLB);
			offset.setIsSLB(!state.IsDut);
			offset.setFOVType(type);
			offset.setColor(color.toLower().toStdString());
			re = offset.getBoresightNineCross(img);
			// re = offset.getBoresightGrid(img);
		}

		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));
		if (re.flag == false)
		{
			LoggingWrapper::instance()->error(QString("%1 calculation error,%2").arg(test_info).arg(QString::fromStdString(re.errMsg)));
			return "";
		}
		if (isDebug)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}
			cv::Mat outImg = re.imgdraw;
			if (outImg.data != NULL)
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
			string csvName = (qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".csv").toStdString();
			//fov.writePointsFile(csvName, re.crossCenter);
		}
		Mat src1 = (Mat_<float>(2, 1) << re.H, re.V);
		if (id != "")
		{
			IQTModel::updateCSVHeaderValues(id.toInt(), src1, mtcName, color);
			emit updateResult(src1);
		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "";
	}
}

QString IQTModel::calculateRotation(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);

	if (params.size() == 0)
	{
		return "";
	}
	cv::Mat img = params[0];
	if (img.data != NULL)
	{
		clock_t start = clock();
		//MLIQMetrics::RotationRe re = rotation.getCrossRotationNoBorder(img);
	   // MLIQMetrics::RotationRe re = rotation.getMultiCrossRotation(img, MetricsData::instance()->getReticleEyeType());
		MLUtils::TestState state = MetricsData::instance()->GetTestState();
		//MLIQMetrics::MLRotation rotation;
		rotation.setIsSLB(!state.IsDut);
		rotation.setIsUpdateSLB(state.IsUpdateSLB);
		FOVTYPE type = getFOVType();
		rotation.setFOVType(type);
		rotation.setColor(color.toLower().toStdString());
		MLIQMetrics::RotationRe re = rotation.getNineCrossRotation(img);
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		if (re.flag == false)
		{
			LoggingWrapper::instance()->error(QString("%1 calculation error,%2").arg(test_info).arg(QString::fromStdString(re.errMsg)));
			return "";
		}

		double degree = re.angle;
		MetricsDataCache::getInstance(this)->updateRotation(color, degree);
		if (isDebug)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}

			cv::Mat outImg = re.imgdraw;
			if (outImg.data != NULL)
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
			string csvName = (qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + "_Pts.csv").toStdString();
			//rotation.writePointsFile(csvName, re.rotaionPts);
		}
		if (id != "")
			IQTModel::updateCSVHeaderValues(id.toInt(), mtcName, degree, color);
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "";
	}
}

QString IQTModel::calculateGridDistortion(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);

	if (params.size() == 0)
	{
		return "";
	}
	cv::Mat img_grid = params[0];
	if (img_grid.data != NULL)
	{
		MLIQMetrics::DistortionRe re;
		//float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
		clock_t start = clock();
		//distortion.setRotationAngle(degree);
	   // re = distortion.GridDistortionFourCorner(img_grid);
		//bool flag = MetricsData::instance()->getIQSLB();
		MLUtils::TestState state = MetricsData::instance()->GetTestState();
		distortion.setIsSLB(!state.IsDut);
		distortion.setIsUpdateSLB(state.IsUpdateSLB);
		distortion.setColor(color.toLower().toStdString());
		FOVTYPE type = getFOVType();
		distortion.setFOVType(type);
		if(type==BIGFOV)
			re = distortion.GridDistortionFourCornerBigFOV(img_grid);
		else
		re = distortion.GridDistortionFourCorner(img_grid);
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		if (re.flag == false)
		{
			LoggingWrapper::instance()->error(QString("%1 calculation error,%2").arg(test_info).arg(QString::fromStdString(re.errMsg)));
			return "";
		}
		Mat discorner = cv::Mat(1, 1, CV_32FC1);
		//discorner.at<float>(0, 0) = re.disCorner[0];
		//discorner.at<float>(0, 1) = re.disCorner[1];
		//discorner.at<float>(0, 2) = re.disCorner[2];
		//discorner.at<float>(0, 3) = re.disCorner[3];
		discorner.at<float>(0, 0) = re.disAvg;

		if (isDebug)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}
			cv::Mat outImg = re.imgdraw;
			if (outImg.data != NULL)
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
			string csvNameX = (qTempPath + "\\" + mtcName + "XPosition_" + color + "_eb" + id + ".csv").toStdString();
			string csvNameY = (qTempPath + "\\" + mtcName + "YPosition_" + color + "_eb" + id + ".csv").toStdString();
			writeMatTOCSV(csvNameX, re.xPos);
			writeMatTOCSV(csvNameY, re.yPos);
		}
		cv::Mat xPos = re.xPos;
		if (xPos.empty())
		{
			//exception e("incorrect Distortion");
			//LoggingWrapper::instance()->warn("incorrect Distortion");
		}
		// myRotateAntiClockWise90(xPos);
		if (id != "")
		{
			// IQTModel::updateCSVHeaderValues(id.toInt(), xPos, mtcName, color);
			IQTModel::updateCSVHeaderValues(id.toInt(), discorner, mtcName, color);
			// IQTModel::updateCSVHeaderValues(id.toInt(), re.disAvg, mtcName, color);

			emit updateResult(xPos);
		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "";
	}
}

QString IQTModel::calculateSequentialContrast(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);

	if (params.size() == 0)
	{
		return "";
	}
	cv::Mat img, img_black;
	img = params[0];
	img_black = params[1];

	if (img.data != NULL && img_black.data != NULL)
	{

		clock_t start = clock();
		//FOVCenter infor = MetricsDataCache::getInstance()->getFOVCenter(color);
		//float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
		MLIQMetrics::SeqCRRe re = seqCR.getSequentialContrast(img, img_black);
		if (re.seqCR > 3999.999)
		{
			re.seqCR = 3999.999;
		}
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		if (isDebug)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}
			cv::Mat outImg = re.imgdrawColor;
			cv::Mat outblack = re.imgdrawBlack; //
			if (outImg.data != NULL && outblack.data != NULL)
			{
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "K_eb" + id + ".jpg").toStdString(), outblack);
			}
		}
		Mat roicr = re.Roicr;
		if (roicr.empty())
		{
			std::exception e(QString("1% calculation error.").arg(test_info).toStdString().c_str());
			LoggingWrapper::instance()->error(QString("1% calculation error.").arg(test_info));
			// throw e;
		}
		//myRotateAntiClockWise90(roicr);
		if (id != "")
		{
			// IQTModel::updateCSVHeaderValues(id.toInt(), roicr, mtcName, color);
			IQTModel::updateCSVHeaderValues(id.toInt(), mtcName, re.seqCR, color);

			emit updateResult(roicr);
		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "0.0";
	}
}

QString IQTModel::calculateLuminanceEfficiency(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);

	if (params.size() == 0)
	{
		return "";
	}
	cv::Mat lumImage = params[0];
	if (lumImage.data != NULL)
	{
		float degree = MetricsDataCache::getInstance()->getRotationDeg(color);

		clock_t start = clock();
		QString colorLower = color.toLower();
		FOVTYPE type = getFOVType();
		efficiency.setFOVType(type);
		MLIQMetrics::LumiEfficencyRe re = efficiency.getLuminanceEfficiency(lumImage, colorLower.toStdString());
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		if (re.flag == false)
		{
			LoggingWrapper::instance()->error(QString("%1 calculation error,%2").arg(test_info).arg(QString::fromStdString(re.errMsg)));
			return "";
		}
		double efficiency = re.efficicncy;
		vector<double>effVec;
		effVec.push_back(re.efficicncy);
		//effVec.push_back(re.p5);
	   // effVec.push_back(re.p50);
		//effVec.push_back(re.p95);
		cv::Mat effMat(effVec);
		effMat.convertTo(effMat, CV_32FC1);
		if (isDebug)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}
			cv::Mat outImg = re.imgdraw;
			cv::Mat slb_draw = re.slb_draw;
			if (outImg.data != NULL)
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
			if (slb_draw.data != NULL)
				imwrite((qTempPath + "\\" + mtcName + "_SLB_" + color + "_eb" + id + ".jpg").toStdString(), slb_draw);
		}
		if (id != "")
		{
			// IQTModel::updateCSVHeaderValues(id.toInt(), mtcName, efficiency, color);
			IQTModel::updateCSVHeaderValues(id.toInt(), effMat, mtcName, color);

		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "0.0";
	}
}

QString IQTModel::calculateFlareC(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);

	if (params.size() == 0)
	{
		return "";
	}
	cv::Mat imgAuto = params[0];
	cv::Mat imgOver = params[1];
	if (imgAuto.data != NULL && imgOver.data != NULL)
	{
		// FOVCenter inform = MetricsDataCache::getInstance()->getFOVCenter(color);
		 //float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
		clock_t start = clock();
		FlareRe re = flare.getFlare(imgAuto, imgOver);
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		if (re.flag == false)
		{
			LoggingWrapper::instance()->error(QString("%1 calculation error,%2").arg(test_info).arg(QString::fromStdString(re.errMsg)));
			return "";
		}

		vector<double> flareVec;
		for (auto it = re.flareMap.begin(); it != re.flareMap.end(); it++)
		{
			vector<FlareSecRe> fre = it->second;
			for (int i = 0; i < fre.size(); i++)
			{
				flareVec.push_back(fre[i].glowMedianL);
				flareVec.push_back(fre[i].flareMedianL);
				flareVec.push_back(fre[i].glowMedianR);
				flareVec.push_back(fre[i].flareMedianR);
			}
		}

		if (isDebug)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}
			cv::Mat outImg = re.imgdraw;
			if (outImg.data != NULL)
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
		}
		Mat flaremat = cv::Mat(flareVec);
		flaremat.convertTo(flaremat, CV_32FC1);
		if (id != "")
		{
			IQTModel::updateCSVHeaderValues(id.toInt(), flaremat, mtcName, color);
			emit updateResult(flaremat);
		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "";
	}
}

QString IQTModel::calculateFlareHDRC(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{	
		QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);
		if (params.size() == 0)
		{
			return "";
		}
		cv::Mat imgHDR = params[0];
		//cv::Mat imgOver = params[1];
		if (imgHDR.data != NULL )
		{
			// FOVCenter inform = MetricsDataCache::getInstance()->getFOVCenter(color);
			 //float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
			clock_t start = clock();
			FlareRe re = flare.getFlare(imgHDR);
			double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
			LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

			if (re.flag == false)
			{
				LoggingWrapper::instance()->error(QString("%1 calculation error,%2").arg(test_info).arg(QString::fromStdString(re.errMsg)));
				return "";
			}

			vector<double> flareVec;
			for (auto it = re.flareMap.begin(); it != re.flareMap.end(); it++)
			{
				vector<FlareSecRe> fre = it->second;
				for (int i = 0; i < fre.size(); i++)
				{
					flareVec.push_back(fre[i].glowMedianL);
					flareVec.push_back(fre[i].flareMedianL);
					flareVec.push_back(fre[i].glowMedianR);
					flareVec.push_back(fre[i].flareMedianR);
				}
			}

			if (isDebug)
			{
				QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
				if (!createDir(qTempPath))
				{
					LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
				}
				cv::Mat outImg = re.imgdraw;
				if (outImg.data != NULL)
					imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
			}
			Mat flaremat = cv::Mat(flareVec);
			flaremat.convertTo(flaremat, CV_32FC1);
			if (id != "")
			{
				IQTModel::updateCSVHeaderValues(id.toInt(), flaremat, mtcName, color);
				emit updateResult(flaremat);
			}
			return "";
		}
		else
		{
			LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
			return "";
		}
	

}

QString IQTModel::calculateGhostC(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);

	if (params.size() == 0)
	{
		return "";
	}
	cv::Mat img = params[0];
	if (img.data != NULL)
	{
		// FOVCenter inform = MetricsDataCache::getInstance()->getFOVCenter(color);
		 //float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
		clock_t start = clock();
		GhostRe re = ghost.getGhostSquares(img);
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		if (re.flag == false)
		{
			LoggingWrapper::instance()->error(QString("%1 calculation error,%2").arg(test_info).arg(QString::fromStdString(re.errMsg)));
			return "";
		}
		vector<double> rolloff;

		if (isDebug)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}
			cv::Mat outImg = re.imgdraw;
			if (outImg.data != NULL)
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
		}
		cv::Mat rollmat;
		Mat roll = cv::Mat(rolloff);
		roll.convertTo(rollmat, CV_32FC1);
		if (id != "")
		{
			IQTModel::updateCSVHeaderValues(id.toInt(), rollmat, mtcName, color);
			emit updateResult(rollmat);
		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "";
	}
}

QString IQTModel::calculatePupilSwimC(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);

	if (params.size() == 0 || params.size() != 3)
	{
		return "";
	}
	cv::Mat img = params[0];
	cv::Mat imgPos = params[1];
	cv::Mat imgNeg = params[2];

	if (img.data != NULL && imgPos.data != NULL && imgNeg.data != NULL)
	{
		// FOVCenter inform = MetricsDataCache::getInstance()->getFOVCenter(color);
		 //float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
		clock_t start = clock();
		FOVTYPE type = getFOVType();
		pupilswim.setFOVType(type);
		PupilSwimRe reLocal = pupilswim.getPupilSwim(img, imgNeg);
		PupilSwimRe reGlobal = pupilswim.getPupilSwim(imgNeg, imgPos);
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		if (reLocal.flag == false || reGlobal.flag == false)
		{
			LoggingWrapper::instance()->error(QString("%1 calculation error, %2, %3")
				.arg(test_info)
				.arg(QString::fromStdString(reLocal.errMsg))
				.arg(QString::fromStdString(reGlobal.errMsg)));
			return "";
		}

		vector<double> rolloff;
		rolloff.push_back(reLocal.p90);
		rolloff.push_back(reLocal.rms);
		rolloff.push_back(reGlobal.p90);
		rolloff.push_back(reGlobal.rms);
		if (isDebug)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}
			cv::Mat outImg = reLocal.imgdraw1;
			if (outImg.data != NULL)
			{
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
				imwrite((qTempPath + "\\" + mtcName + "Neg_" + color + "_eb" + id + ".jpg").toStdString(), reLocal.imgdraw2);
				imwrite((qTempPath + "\\" + mtcName + "Pos_" + color + "_eb" + id + ".jpg").toStdString(), reGlobal.imgdraw2);
			}
		}
		cv::Mat rollmat;
		Mat roll = cv::Mat(rolloff);
		roll.convertTo(rollmat, CV_32FC1);
		if (id != "")
		{
			IQTModel::updateCSVHeaderValues(id.toInt(), rollmat, mtcName, color);
			emit updateResult(rollmat);
		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "";
	}
}

QString IQTModel::calculateGridDenseMTF(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);

	if (params.size() == 0)
	{
		return "";
	}
	cv::Mat img = params[0];
	if (img.data != NULL)
	{
		// FOVCenter inform = MetricsDataCache::getInstance()->getFOVCenter(color);
		 //float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
	  //  GhostRe re = ghost.getGhostSquares(img);
		clock_t start = clock();
		MLIQMetrics::MLdenseMTF denseMTF;
		denseMTF.setCheckerDegree(0);
		FOVTYPE type = getFOVType();
		denseMTF.setFOVType(type);
		DenseMTFGridRe re = denseMTF.getDenseMTFGrid(img);
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		if (re.flag == false)
		{
			LoggingWrapper::instance()->error(QString("%1 calculation error,%2").arg(test_info).arg(QString::fromStdString(re.errMsg)));
			return "";
		}
		vector<double> rolloff;
		rolloff.push_back(re.meanH);
		rolloff.push_back(re.meanV);
		rolloff.push_back(re.minH);
		rolloff.push_back(re.minV);
		rolloff.push_back(cv::mean(re.mtfMapH2)(0));
		rolloff.push_back(cv::mean(re.mtfMapV2)(0));
		rolloff.push_back(denseMTF.calculateMatMin(re.mtfMapH2));
		rolloff.push_back(denseMTF.calculateMatMin(re.mtfMapV2));

		if (isDebug)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}
			cv::Mat outImg = re.imgdraw;
			if (outImg.data != NULL)
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
			string csvNameX = (qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + "_gridMtfmap.csv").toStdString();

			vector<cv::Mat>matvec; vector<string>strvec;
			matvec.push_back(re.mtfMapH);
			matvec.push_back(re.mtfMapV);
			matvec.push_back(re.mtfMapH2);
			matvec.push_back(re.mtfMapV2);
			strvec.push_back("mtfH_10.625");
			strvec.push_back("mtfV_10.625");
			strvec.push_back("mtfH_21.25");
			strvec.push_back("mtfV_21.25");
			writeMatTOCSV(csvNameX, matvec, strvec);
		}
		cv::Mat rollmat;
		Mat roll = cv::Mat(rolloff);
		roll.convertTo(rollmat, CV_32FC1);
		if (id != "")
		{
			IQTModel::updateCSVHeaderValues(id.toInt(), rollmat, mtcName, color);
			emit updateResult(rollmat);
		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "";
	}
}

QString IQTModel::calculate2degCheckerDenseMTF(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);
	if (params.size() == 0)
	{
		return "";
	}
	cv::Mat img = params[0];
	if (img.data != NULL)
	{
		// FOVCenter inform = MetricsDataCache::getInstance()->getFOVCenter(color);
		 //float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
	  //  GhostRe re = ghost.getGhostSquares(img);
		clock_t start = clock();
		MLIQMetrics::MLdenseMTF denseMTF;
		//denseMTF.setCheckerDegree(1);
		FOVTYPE type = getFOVType();
		denseMTF.setFOVType(type);
		DenseMTFRe re = denseMTF.getDenseMTFChecker(img,1);
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		if (re.flag == false)
		{
			LoggingWrapper::instance()->error(QString("%1 calculation error,%2").arg(test_info).arg(QString::fromStdString(re.errMsg)));
			return "";
		}
		vector<double> rolloff;
		rolloff.push_back(cv::mean(re.mtfMapH)(0));
		rolloff.push_back(cv::mean(re.mtfMapV)(0));
		rolloff.push_back(denseMTF.calculateMatMin(re.mtfMapH2));
		rolloff.push_back(denseMTF.calculateMatMin(re.mtfMapV2));
		rolloff.push_back(cv::mean(re.mtfMapH2)(0));
		rolloff.push_back(cv::mean(re.mtfMapV2)(0));
		rolloff.push_back(denseMTF.calculateMatMin(re.mtfMapH2));
		rolloff.push_back(denseMTF.calculateMatMin(re.mtfMapV2));

		if (isDebug)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}
			cv::Mat outImg = re.imgdraw;
			if (outImg.data != NULL)
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
			string csvNameX = (qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + "_gridMtfmap.csv").toStdString();

			vector<cv::Mat>matvec; vector<string>strvec;
			matvec.push_back(re.mtfMapH);
			matvec.push_back(re.mtfMapV);
			matvec.push_back(re.mtfMapH2);
			matvec.push_back(re.mtfMapV2);
			strvec.push_back("mtfH_10.625");
			strvec.push_back("mtfV_10.625");
			strvec.push_back("mtfH_21.25");
			strvec.push_back("mtfV_21.25");
			writeMatTOCSV(csvNameX, matvec, strvec);
		}
		cv::Mat rollmat;
		Mat roll = cv::Mat(rolloff);
		roll.convertTo(rollmat, CV_32FC1);
		if (id != "")
		{
			IQTModel::updateCSVHeaderValues(id.toInt(), rollmat, mtcName, color);
			emit updateResult(rollmat);
		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "";
	}


}

QString IQTModel::calculate5degCheckerDenseMTF(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{

	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);
	if (params.size() == 0)
	{
		return "";
	}
	cv::Mat img = params[0];
	if (img.data != NULL)
	{
		// FOVCenter inform = MetricsDataCache::getInstance()->getFOVCenter(color);
		 //float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
	  //  GhostRe re = ghost.getGhostSquares(img);
		clock_t start = clock();
		MLIQMetrics::MLdenseMTF denseMTF;
		//denseMTF.setCheckerDegree(2);
		FOVTYPE type = getFOVType();
		denseMTF.setFOVType(type);
		DenseMTFRe re = denseMTF.getDenseMTFChecker(img,2);
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		if (re.flag == false)
		{
			LoggingWrapper::instance()->error(QString("%1 calculation error,%2").arg(test_info).arg(QString::fromStdString(re.errMsg)));
			return "";
		}
		vector<double> rolloff;
		rolloff.push_back(cv::mean(re.mtfMapH)(0));
		rolloff.push_back(cv::mean(re.mtfMapV)(0));
		rolloff.push_back(denseMTF.calculateMatMin(re.mtfMapH2));
		rolloff.push_back(denseMTF.calculateMatMin(re.mtfMapV2));
		rolloff.push_back(cv::mean(re.mtfMapH2)(0));
		rolloff.push_back(cv::mean(re.mtfMapV2)(0));
		rolloff.push_back(denseMTF.calculateMatMin(re.mtfMapH2));
		rolloff.push_back(denseMTF.calculateMatMin(re.mtfMapV2));

		if (isDebug)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}
			cv::Mat outImg = re.imgdraw;
			if (outImg.data != NULL)
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
			string csvNameX = (qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + "_gridMtfmap.csv").toStdString();

			vector<cv::Mat>matvec; vector<string>strvec;
			matvec.push_back(re.mtfMapH);
			matvec.push_back(re.mtfMapV);
			matvec.push_back(re.mtfMapH2);
			matvec.push_back(re.mtfMapV2);
			strvec.push_back("mtfH_10.625");
			strvec.push_back("mtfV_10.625");
			strvec.push_back("mtfH_21.25");
			strvec.push_back("mtfV_21.25");
			writeMatTOCSV(csvNameX, matvec, strvec);
		}
		cv::Mat rollmat;
		Mat roll = cv::Mat(rolloff);
		roll.convertTo(rollmat, CV_32FC1);
		if (id != "")
		{
			IQTModel::updateCSVHeaderValues(id.toInt(), rollmat, mtcName, color);
			emit updateResult(rollmat);
		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "";
	}




}

QString IQTModel::calculateCrossMTF(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);

	if (params.size() == 0)
	{
		return "";
	}
	cv::Mat img = params[0];
	if (img.data != NULL)
	{
		// FOVCenter inform = MetricsDataCache::getInstance()->getFOVCenter(color);
		 //float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
	  //  GhostRe re = ghost.getGhostSquares(img);
		clock_t start = clock();
		FOVTYPE type = getFOVType();
		crossMTF.setFOVType(type);
		CrossMTFRe re = crossMTF.getCrossMTF(img, type);
		// DenseMTFGridRe re = denseMTF.getDenseMTFGrid(img);
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		if (re.flag == false)
		{
			LoggingWrapper::instance()->error(QString("%1 calculation error,%2").arg(test_info).arg(QString::fromStdString(re.errMsg)));
			return "";
		}
		vector<double> rolloff;
		rolloff.push_back(re.meanH);
		rolloff.push_back(re.meanV);
		rolloff.push_back(re.minH);
		rolloff.push_back(re.minV);
		rolloff.push_back(re.meanH2);
		rolloff.push_back(re.meanV2);
		rolloff.push_back(re.minH2);
		rolloff.push_back(re.minV2);
		if (isDebug)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}
			cv::Mat outImg = re.imgdraw;
			if (outImg.data != NULL)
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
			string csvNameX = (qTempPath + "\\" + mtcName + "map_" + color + "_eb" + id + ".csv").toStdString();
			vector<cv::Mat>matvec; vector<string>strvec;
			matvec.push_back(re.mtfMapH);
			matvec.push_back(re.mtfMapV);
			matvec.push_back(re.mtfMapH2);
			matvec.push_back(re.mtfMapV2);
			strvec.push_back("mtfH_6.25");
			strvec.push_back("mtfV_6.25");
			strvec.push_back("mtfH_12.5");
			strvec.push_back("mtfV_12.5");
			writeMatTOCSV(csvNameX, matvec, strvec);
		}
		cv::Mat rollmat;
		Mat roll = cv::Mat(rolloff);
		roll.convertTo(rollmat, CV_32FC1);
		if (id != "")
		{
			IQTModel::updateCSVHeaderValues(id.toInt(), rollmat, mtcName, color);
			emit updateResult(rollmat);
		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "";
	}
}

QString IQTModel::calculateLateralColor(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);

	if (params.size() != 3)
	{
		return "";
	}
	cv::Mat imgR = params[0];
	cv::Mat imgG = params[1];
	cv::Mat imgB = params[2];

	if (imgR.data != NULL && imgG.data != NULL && imgB.data != NULL)
	{
		clock_t start = clock();
		//bool flag = MetricsData::instance()->getIQSLB();
		MLUtils::TestState state = MetricsData::instance()->GetTestState();
		//MLIQMetrics::MLLateralColor lateralColor;
		lateralColor.setIsSLB(!state.IsDut);
		lateralColor.setIsUpdateSLB(state.IsUpdateSLB);
		FOVTYPE type = getFOVType();
		lateralColor.setFOVType(type);
		// LateralColorRe re=lateralColor.getLateralColorGrid(imgR, imgG, imgB);
		LateralColorRe re = lateralColor.getLateralColorCrossPreLoc(imgR, imgG, imgB);
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		if (re.flag == false)
		{
			LoggingWrapper::instance()->error(QString("%1 calculation error,%2").arg(test_info).arg(QString::fromStdString(re.errMsg)));
			return "";
		}
		vector<double> rolloff;
		rolloff.push_back(re.maxdis);
		rolloff.push_back(re.meandis);
		rolloff.push_back(mean(re.subXRGArcmin)(0));
		rolloff.push_back(mean(re.subYRGArcmin)(0));
		rolloff.push_back(mean(re.subXGBArcmin)(0));
		rolloff.push_back(mean(re.subYGBArcmin)(0));
		rolloff.push_back(mean(re.subXRBArcmin)(0));
		rolloff.push_back(mean(re.subYRBArcmin)(0));

		if (isDebug)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}
			cv::Mat outImg = re.imgdrawR;
			//if (outImg.data != NULL)
			imwrite((qTempPath + "\\" + mtcName + "_R_eb" + id + ".jpg").toStdString(), re.imgdrawR);
			imwrite((qTempPath + "\\" + mtcName + "_G_eb" + id + ".jpg").toStdString(), re.imgdrawG);
			imwrite((qTempPath + "\\" + mtcName + "_B_eb" + id + ".jpg").toStdString(), re.imgdrawB);
			//imwrite((qTempPath + "\\" + "lateralColorRGB.jpg").toStdString(), re.imgdraw);

			vector<cv::Mat>submat;
			vector<string>substrvec;
			submat.push_back(re.subXRGArcmin);
			submat.push_back(re.subYRGArcmin);
			submat.push_back(re.subXRBArcmin);
			submat.push_back(re.subYRBArcmin);
			submat.push_back(re.subXGBArcmin);
			submat.push_back(re.subYGBArcmin);
			substrvec.push_back("subXRGArcmin");
			substrvec.push_back("subYRGArcmin");
			substrvec.push_back("subXRBArcmin");
			substrvec.push_back("subYRBArcmin");
			substrvec.push_back("subXGBArcmin");
			substrvec.push_back("subYGBArcmin");
			string csvName = (qTempPath + "\\" + mtcName + "_eb" + id + "subMapAcmin.csv").toStdString();
			writeMatTOCSV(csvName, submat, substrvec);

			string csvNameXR = (qTempPath + "\\" + mtcName + "_eb" + id + "locxR.csv").toStdString();
			string csvNameYR = (qTempPath + "\\" + mtcName + "_eb" + id + "locyR.csv").toStdString();
			writeMatTOCSV(csvNameXR, re.locxR);
			writeMatTOCSV(csvNameYR, re.locyR);

			string csvNameXG = (qTempPath + "\\" + mtcName + "_eb" + id + "locxG.csv").toStdString();
			string csvNameYG = (qTempPath + "\\" + mtcName + "_eb" + id + "locyG.csv").toStdString();
			writeMatTOCSV(csvNameXG, re.locxG);
			writeMatTOCSV(csvNameYG, re.locyG);

			string csvNameXB = (qTempPath + "\\" + mtcName + "_eb" + id + "locxB.csv").toStdString();
			string csvNameYB = (qTempPath + "\\" + mtcName + "_eb" + id + "locyB.csv").toStdString();
			writeMatTOCSV(csvNameXB, re.locxB);
			writeMatTOCSV(csvNameYB, re.locyB);

		}
		cv::Mat rollmat;
		Mat roll = cv::Mat(rolloff);
		roll.convertTo(rollmat, CV_32FC1);
		if (id != "")
		{
			IQTModel::updateCSVHeaderValues(id.toInt(), rollmat, mtcName, color);
			emit updateResult(rollmat);
		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "";
	}
}

QString IQTModel::calculateLuminanceUniformity(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);

	if (params.size() == 0)
	{
		return "";
	}
	cv::Mat img = params[0];
	if (img.data != NULL)
	{
		clock_t start = clock();
		//  float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
		MLIQMetrics::LuminanceRe re = luminance.getLuminance(img);
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		if (isDebug)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}
			cv::Mat outImg = re.imgdraw;
			if (outImg.data != NULL)
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
		}
		std::vector<double> reVec;
		// re = lu.MinMaxPer;
		reVec.push_back(re.rectMean);
		reVec.push_back(re.rectCov);
		if (reVec.size() == 0)
		{
			exception e(QString("1% calculation error.").arg(test_info).toStdString().c_str());
			LoggingWrapper::instance()->error(QString("1% calculation error.").arg(test_info));
			// throw e;
		}
		cv::Mat MinMaxAvg;
		Mat minmaxavg = cv::Mat(reVec);
		minmaxavg.convertTo(MinMaxAvg, CV_32FC1);

		if (id != "")
		{
			IQTModel::updateCSVHeaderValues(id.toInt(), MinMaxAvg, mtcName, color);
			emit updateResult(MinMaxAvg);
		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "";
	}
}

QString IQTModel::calculateSmallContrastChessboard(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);

	//QString infoPath = MetricsData::instance()->getMTFImgsDir() + "info.txt";
	//QString name = color.toLower() + "_ChessBoardNeg_" + id;
	//float exptime = getExposureTime(infoPath, name);
	if (params.size() == 0)
	{
		return "";
	}
	cv::Mat imgP = params[0];
	cv::Mat imgN = params[1];
	if (imgP.data != NULL && imgN.data != NULL)
	{
		clock_t start = clock();
		MLUtils::TestState state = MetricsData::instance()->GetTestState();
		checkerCR.setColor(color.toLower().toStdString());
		FOVTYPE type = getFOVType();
		checkerCR.setFOVType(type);
		checkerCR.setIsUpdateSLB(state.IsUpdateSLB);
		checkerCR.setEyeboxLocation(id.toStdString()); // Left or Right
		MLIQMetrics::ContrastRatioRe re = checkerCR.getContrastRatio(imgP, imgN, !state.IsDut);
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		if (re.flag == false)
		{
			LoggingWrapper::instance()->error(QString("%1 calculation error,%2").arg(test_info).arg(QString::fromStdString(re.errMsg)));
			return "";
		}

		if (isDebug)
		{
			cv::Mat outImgP = re.imgdrawP;
			cv::Mat outImgN = re.imgdrawN;
			if (outImgP.data != NULL && outImgN.data != NULL)
			{
				QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
				if (!createDir(qTempPath))
				{
					LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
				}
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + "_ImgP.jpg").toStdString(), outImgP);
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + "_ImgN.jpg").toStdString(), outImgN);
				//string name1 = (qTempPath + "\\" + color + "_greyLevelP.csv").toStdString();
				//string name2 = (qTempPath + "\\" + color + "_greyLevelN.csv").toStdString();
				string name3 = (qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + "_CR.csv").toStdString();
				// writeMatTOCSV(name1, re.greyLevelP);
				 //writeMatTOCSV(name2, re.greyLevelN);
				vector<cv::Mat>crVec;
				crVec.push_back(re.greyLevelP);
				crVec.push_back(re.greyLevelN);
				crVec.push_back(re.crMat);
				vector<string>infoVec;
				infoVec.push_back("luminance of positive checker");
				infoVec.push_back("luminance of negitive checker");
				infoVec.push_back("CR");
				writeMatTOCSV(name3, crVec, infoVec);
			}
		}

		Mat src1 = (Mat_<double>(2, 1) << re.meanCR, re.minCR);
		cv::Mat b;
		src1.convertTo(b, CV_32FC1);
		if (id != "")
		{
			IQTModel::updateCSVHeaderValues(id.toInt(), b, mtcName, color);
			emit updateResult(b);
		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "0.0";
	}
}

QString IQTModel::calculateLuminanceRolloff(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);

	if (params.size() == 0)
	{
		return "";
	}
	cv::Mat img = params[0];
	if (img.data != NULL)
	{
		// FOVCenter inform = MetricsDataCache::getInstance()->getFOVCenter(color);
		 //float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
		// MLIQMetrics::RolloffRe re = rollOff.LuminanceRolloffRotation(img);
		QString colorLower = color.toLower();
		clock_t start = clock();
		MLUtils::TestState state = MetricsData::instance()->GetTestState();
		rollOff.setIsSLB(!state.IsDut);
		FOVTYPE type = getFOVType();
		rollOff.setFOVType(type);
		rollOff.setIsUpdateSLB(state.IsUpdateSLB);
		rollOff.setColor(color.toLower().toStdString());
		MLIQMetrics::RolloffJiaXingRe re = rollOff.getRelativeBrightness(img);
		double duration = double(clock() - start) / CLOCKS_PER_SEC * 1000;
		LoggingWrapper::instance()->debug(QString("%1 calculation takes time :%2 ms").arg(test_info).arg(QString::number(duration)));

		if (re.flag == false)
		{
			LoggingWrapper::instance()->error(QString("%1 calculation error,%2").arg(test_info));
			return "";
		}
		vector<double> rolloff;
		rolloff.push_back(re.meanLumi);
		rolloff.push_back(re.CV);
		rolloff.push_back(re.p5);
		rolloff.push_back(re.p50);
		rolloff.push_back(re.p95);
		//rolloff.push_back(re.rollOffH);
	   // rolloff.push_back(re.rollOffV);
		if (isDebug)
		{
			QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Result_" + id;
			if (!createDir(qTempPath))
			{
				LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
			}
			cv::Mat outImg = re.imgdraw;
			if (outImg.data != NULL)
				imwrite((qTempPath + "\\" + mtcName + "_" + color + "_eb" + id + ".jpg").toStdString(), outImg);
		}
		cv::Mat rollmat;
		Mat roll = cv::Mat(rolloff);
		roll.convertTo(rollmat, CV_32FC1);
		if (id != "")
		{
			IQTModel::updateCSVHeaderValues(id.toInt(), rollmat, mtcName, color);
			emit updateResult(rollmat);
		}
		return "";
	}
	else
	{
		LoggingWrapper::instance()->error(QString("1% calculation error, the image is empty.").arg(test_info));
		return "";
	}
}

// python SDK call metrics
QString IQTModel::calculateCheckboardDistortion(QString color, QString mtcName, QString id)
{
	// QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "CheckboardDistortion_" + id;
	// if (!createDir(qTempPath))
	//{
	//    LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
	//}
	if (params.size() == 0)
	{
		return "";
	}

	// QMutexLocker locker(&mutex);
	////emit sigCalculateDistortion(color.toStdString(), id.toInt());
	// condition.wait(&mutex);
	// if (!m_CalResult)
	//{
	//    exception e("incorrect Checkboard Distortion");
	//    throw e;
	//}
	return "";
}

QString IQTModel::calculateFlare(QString color, QString mtcName, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);
	// QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Flare_" + id;
	// if (!createDir(qTempPath))
	//{
	//    LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
	//}
	if (params.size() == 0)
	{
		return "";
	}
	float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
	QMutexLocker locker(&mutex);
	emit sigCalculateFlare(color.toStdString(), id.toInt(), degree);
	condition.wait(&mutex);

	if (!m_CalResult)
	{
		exception e(QString("1% calculation error.").arg(test_info).toStdString().c_str());
		throw e;
	}
	return "";
}

QString IQTModel::calculateGhost(QString color, QString mtcName, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);
	// QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "Ghost_" + id;
	// if (!createDir(qTempPath))
	//{
	//    LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
	//}
	if (params.size() == 0)
	{
		return "";
	}
	float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
	QMutexLocker locker(&mutex);
	emit sigCalculateGhost(color.toStdString(), id.toInt(), degree);
	condition.wait(&mutex);

	if (!m_CalResult)
	{
		exception e(QString("1% calculation error.").arg(test_info).toStdString().c_str());
		throw e;
	}
	return "";
}

QString IQTModel::calculatePupilSwim(QString color, QString mtcName, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);
	// QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "PupilSwim_" + id;
	// if (!createDir(qTempPath))
	//{
	//    LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
	//}
	if (params.size() == 0)
	{
		return "";
	}
	float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
	QMutexLocker locker(&mutex);
	emit sigCalculatePupilSwim(color.toStdString(), id.toInt(), degree);
	condition.wait(&mutex);

	if (!m_CalResult)
	{
		exception e(QString("1% calculation error.").arg(test_info).toStdString().c_str());
		throw e;
	}
	return "";
}

QString IQTModel::calculateStrayLight(QString color, QString mtcName, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);
	// QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "StrayLight_" + id;
	// if (!createDir(qTempPath))
	//{
	//    LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
	//}
	if (params.size() == 0)
	{
		return "";
	}
	float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
	QMutexLocker locker(&mutex);
	emit sigCalculateStrayLight(color.toStdString(), id.toInt(), degree);
	condition.wait(&mutex);

	if (!m_CalResult)
	{
		exception e(QString("1% calculation error.").arg(test_info).toStdString().c_str());
		throw e;
	}
	return "";
}

QString IQTModel::calculateColorUniformity(QString color, QString mtcName, std::vector<cv::Mat> params, QString id)
{
	QString test_info = QString("Metrics %1, eyeboxid:%2, color:%3").arg(mtcName).arg(id).arg(color);
	// QString qTempPath = MetricsData::instance()->getMTFImgsDir() + "ColorUniformity_" + id;
	// if (!createDir(qTempPath))
	//{
	//    LoggingWrapper::instance()->error("new dirctory error:" + qTempPath);
	//}
	if (params.size() == 0)
	{
		return "";
	}
	float degree = MetricsDataCache::getInstance()->getRotationDeg(color);
	FOVTYPE type = getFOVType();
	MLUtils::TestState state = MetricsData::instance()->GetTestState();
	offset.setIsSLB(!state.IsDut);
	if (type == SMALLFOV&& !state.IsDut)
	{
		degree = 22.5;
	}

	string ndstr;
	ndstr = MetricsData::instance()->getImageNDFilter().toStdString();
	QMutexLocker locker(&mutex);
	emit sigCalculateColorUniformity(color.toStdString(), id.toInt(), degree, ndstr);
	condition.wait(&mutex);
	if (!m_CalResult)
	{
		exception e(QString("1% calculation error.").arg(test_info).toStdString().c_str());
		throw e;
	}
	return "";
}

IQTModel* IQTModel::instance(QObject* parent)
{
	if (self == nullptr)
	{
		self = new IQTModel();
	}
	self->setParent(parent);
	return self;
}

IQTModel::~IQTModel()
{
	metricCalculators.clear();
	manualImgs.clear();
	// m_thread->quit();
}

QString IQTModel::calculateMetric(QString metricName, QString color)
{
	IC(metricName.toStdString(), color.toStdString());
	isOffline = false;
	curTargetMetric.name = "";
	for each (MetricDescription * var in *metricVec)
	{
		if (var->name == metricName.toStdString())
		{
			curTargetMetric = *var;
			break;
		}
	}
	if (curTargetMetric.name.empty())
	{
		LoggingWrapper::instance()->error("metric is empty");
		return "0";
	}
	std::vector<std::string> tarsImg = curTargetMetric.images;
	std::string firstImg = tarsImg.at(0);
	QString qFirstImg = QString::fromStdString(firstImg);
	QFileInfo fi(qFirstImg);
	if (fi.exists() && fi.isFile())
	{
		isOffline = true;
	}
	MetricCalculator metricFunc = metricCalculators.value(metricName);
	if (metricFunc == nullptr)
	{
		LoggingWrapper::instance()->error("can not find metric function");
		return "0";
	}
	// collectCalcultorParamImg(color);
	// QString result = (this->*metricFunc)(color, metricName);
	QString result = (this->*metricFunc)(color, metricName, params, "4");
	return result;
}

QString IQTModel::generateCsvHeader(QString metricName, QString suffix, QString color)
{
	if (metricName == "")
		return "";
	QString result = "";
	std::string stdName = metricName.toStdString();
	for each (MetricDescription * var in *metricVec)
	{
		if (var->name == stdName)
		{
			QString csvHeader = QString::fromStdString(var->logheader);
			result = csvHeader;
			if (!suffix.isEmpty())
			{
				result = csvHeader.replace("$suffix$", suffix);
			}
			else
				result = csvHeader.replace("$suffix$", "");
			if (!color.isEmpty())
			{
				result = result.replace("$color$", color);
			}
			break;
		}
	}
	return result;
}
void IQTModel::updateCSVHeaderValues(int id, std::vector<cv::Mat> val, QString metricName, QString color)
{
	std::string stdName = metricName.toStdString();
	MetricDescription* curMetricPtr = nullptr;
	for each (MetricDescription * var in *metricVec)
	{
		if (var->name == stdName)
		{
			curMetricPtr = var;
			break;
		}
	}
	if (curMetricPtr == nullptr)
	{
		return;
	}
	QString csvHeader = QString::fromStdString(curMetricPtr->logheader);
	QStringList csvHeaderLst = csvHeader.split(",");
	if (val.size() >= csvHeaderLst.size())
	{
		for (int n = 0; n < csvHeaderLst.size(); n++)
		{
			cv::Mat oneVal = val.at(n);
			const std::vector<std::string>& suffixs = curMetricPtr->suffix;
			int type = oneVal.type();
			if (type == CV_32FC1)
			{
				int cols = oneVal.cols;
				int rows = oneVal.rows;
				for (int i = 0; i < rows; i++)
				{
					for (int j = 0; j < cols; j++)
					{
						double metricVal = oneVal.at<float>(i, j);
						int index = i * cols + j;
						if (index < suffixs.size())
						{
							QString header = csvHeaderLst.at(n);
							QString colorCsvHeader = header.replace("$color$", color);
							QString eidCsvHeader = colorCsvHeader.replace("$eyeboxId$", QString::number(id));
							std::string suf = suffixs.at(index);
							QString qSuf = QString::fromStdString(suf);
							QString resultHeader = eidCsvHeader.replace("$suffix$", qSuf);
							updateCSVHeaderItem(QString::number(id), resultHeader, metricVal);
						}
						else
						{
							LoggingWrapper::instance()->error("out of index:" + QString::number(index));
						}
					}
				}
			}
		}
	}
	else
	{
		LoggingWrapper::instance()->error("mismatch between result value and log headers");
	}
}
void IQTModel::updateCSVHeaderValues(int id, cv::Mat val, QString metricName, QString color)
{
	int type = val.type();
	std::string stdName = metricName.toStdString();
	MetricDescription* curMetricPtr = nullptr;
	for each (MetricDescription * var in *metricVec)
	{
		if (var->name == stdName)
		{
			curMetricPtr = var;
			break;
		}
	}
	if (curMetricPtr == nullptr)
	{
		return;
	}
	const std::vector<std::string>& suffixs = curMetricPtr->suffix;
	if (type == CV_32FC1)
	{
		int cols = val.cols;
		int rows = val.rows;
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				double metricVal = val.at<float>(i, j);
				int index = i * cols + j;
				if (index < suffixs.size())
				{
					QString csvHeader = QString::fromStdString(curMetricPtr->logheader);
					QString colorCsvHeader = csvHeader.replace("$color$", color);
					QString eidCsvHeader = colorCsvHeader.replace("$eyeboxId$", QString::number(id));
					std::string suf = suffixs.at(index);
					QString qSuf = QString::fromStdString(suf);
					QString resultHeader = eidCsvHeader.replace("$suffix$", qSuf);
					updateCSVHeaderItem(QString::number(id), resultHeader, metricVal);
				}
				else
				{
					LoggingWrapper::instance()->error("out of index:" + QString::number(index));
				}
			}
		}
	}
}
void IQTModel::updateCSVHeaderValues(int id, QString metricName, double val, QString color)
{
	if (metricName == "")
		return;
	QString result = "";
	std::string stdName = metricName.toStdString();

	MetricDescription* curMetricPtr = nullptr;
	for each (MetricDescription * var in *metricVec)
	{
		if (var->name == stdName)
		{
			curMetricPtr = var;
			break;
		}
	}
	if (curMetricPtr == nullptr)
	{
		return;
	}


	const std::vector<std::string>& suffixs = curMetricPtr->suffix;

	for each (MetricDescription * var in *metricVec)
	{
		if (var->name == stdName)
		{
			QString csvHeader = QString::fromStdString(var->logheader);
			// if (!color.isEmpty())
			{
				csvHeader = csvHeader.replace("$eyeboxId$", QString::number(id));
				result = csvHeader.replace("$color$", color);
				updateCSVHeaderItem(QString::number(id), result, val);
			}
			// else
			//{
			//    updateCSVHeaderItem(QString::number(id), csvHeader, val);
			//}
			break;
		}
	}
}

// Function: updating the comparison table
void IQTModel::updateCompareCSVHeaderValues()
{
	// std::vector<QStringList> mList;
	// mList = MetricsDataBase::getInstance()->getCompareMetricKeyValue();
	std::vector<MetricDescription*>* desVec = IQMetricConfig::instance()->getMetricDescripts();
	for (int i = 0; i < desVec->size(); i++)
	{
		MetricDescription* item = desVec->at(i);
		std::string name = item->name;
		std::string logHeader = item->logheader;
		const std::vector<std::string>& eyeboxId = item->eyeboxId;

		if (name == "DiffCompare")
		{
			for each (std::string eId in eyeboxId)
			{
				QString qLogHeader = QString::fromStdString(logHeader);
				qLogHeader = qLogHeader.replace("$eyeboxId$", QString::fromStdString(eId));
				QString modelHeader = "test_dis_0$eyeboxId$_cuniform_ave";
				calCompareMetric(modelHeader, qLogHeader, QString::fromStdString(eId));
			}
		}
		else if (name == "RatioCompare")
		{
			for each (std::string eId in eyeboxId)
			{
				QString qLogHeader = QString::fromStdString(logHeader);
				qLogHeader = qLogHeader.replace("$eyeboxId$", QString::fromStdString(eId));
				QString modelHeader = "test_lum_0$eyeboxId$_avg";
				calCompareMetric(modelHeader, qLogHeader, QString::fromStdString(eId));
			}
		}
	}
}

void IQTModel::calCompareMetric(QString source_name, QString target_name, QString id)
{
	QString s1 = source_name;
	QString s2 = s1;
	double value_n = MetricsDataBase::getInstance()->queryOneMetricValue(id, s1.replace("$eyeboxId$", id), "value");
	double value_center =
		MetricsDataBase::getInstance()->queryOneMetricValue("5", s2.replace("$eyeboxId$", "5"), "value");
	if (target_name.contains("ratio"))
	{
		double ratiovalue = 0.0;
		if (value_center != 0)
		{
			ratiovalue = (value_n / value_center) * 100;
			MetricsDataBase::getInstance()->updateMetricValue(target_name, ratiovalue, id);
		}
		else
			MetricsDataBase::getInstance()->updateMetricValue(target_name, -1, id);
	}
	else if (target_name.contains("diff"))
	{
		double diffvalue = 0.0;
		diffvalue = value_n - value_center;
		MetricsDataBase::getInstance()->updateMetricValue(target_name, diffvalue, id);
	}
}

void IQTModel::updateCSVHeaderItem(QString id, QString header, double val)
{
	MetricsDataBase::getInstance()->updateMetricValue(header, val, id);
}

void IQTModel::updatepathItem(QString path, QString id)
{
	QString dutId = MetricsData::instance()->getDutBarCode();
	MetricsDataBase::getInstance()->updatePathValue(dutId, path, id);
}

QString IQTModel::getCSVHeaderValue(QString header)
{
	double val = MetricsDataBase::getInstance()->getMetricValue(header);

	return QString::number(val, 'f', 5);
}

void IQTModel::setManualCollectImg(bool val)
{
	manualCollectImgs = val;
}

void IQTModel::saveMetricsToCsv(QString PathName)
{
	QStringList iqList = MetricsDataBase::getInstance()->exportToCsv();
	// TODO
	QString tmpStr;
	QStringList strList = iqList;
	for (int j = 0; j < strList.size(); j++)
	{
		QStringList subStrList = strList.at(j).split(";");
		for (int n = 0; n < subStrList.size(); n++)
		{
			tmpStr.append(subStrList.at(n));
			if (n < (subStrList.size() - 1))
				tmpStr.append(',');
			else
				tmpStr.append('\n');
		}
	}
	QFile file(PathName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
	{
		qDebug() << "Open " << PathName << " Error!";
		return;
	}
	else
	{
		QTextStream in(&file);
		in << tmpStr;
		file.close();
	}
}
bool IQTModel::createMetricsResutCSVFile(QString PathName)
{
	QFile csvFile(PathName);
	if (!csvFile.open(QIODevice::ReadWrite))
	{
		return false;
	}
	else
	{
		QStringList testInfoList = MetricsDataBase::getInstance()->getTestInfoHeaders();
		QStringList placeHolder;
		for (int i = 0; i < testInfoList.size() - 1; i++)
		{
			placeHolder.append(" ");
		}
		csvFile.write(testInfoList.join(",").toUtf8());
		csvFile.write(",");

		QString eyeboxQueue = MetricsData::instance()->getEyeboxQueue();
		if (eyeboxQueue.isEmpty())
			eyeboxQueue = (Session::getInstance()->getData()).value("$eyebox$");

		QStringList HeadersList = MetricsDataBase::getInstance()->getAllMetricsToCsv("name", eyeboxQueue);

		// TODO:: change the  QList<QString>  to  QString
		// QStringList HeadersList1 =MetricsDataBase::getInstance()->getAllMetricsToCsv("name",
		// GenCSVHeader::getMetricsEyeboxIds()) ;
		csvFile.write(HeadersList.join(",").toUtf8());
		csvFile.write("\n");

		csvFile.write("uplimit,");
		csvFile.write(placeHolder.join(",").toUtf8());
		csvFile.write(",");

		QStringList uplimitList = MetricsDataBase::getInstance()->getAllMetricsToCsv("uplimit", eyeboxQueue);
		// QStringList uplimitList =
		//    MetricsDataBase::getInstance()->getAllMetricsToCsv("uplimit", GenCSVHeader::getMetricsEyeboxIds());

		csvFile.write(uplimitList.join(",").toUtf8());
		csvFile.write("\n");

		csvFile.write("lowlimit,");
		csvFile.write(placeHolder.join(",").toUtf8());
		csvFile.write(",");

		QStringList lowlimitList = MetricsDataBase::getInstance()->getAllMetricsToCsv("lowlimit", eyeboxQueue);
		// QStringList lowlimitList =
		//    MetricsDataBase::getInstance()->getAllMetricsToCsv("lowlimit", GenCSVHeader::getMetricsEyeboxIds());

		csvFile.write(lowlimitList.join(",").toUtf8());
		csvFile.write("\n");
		csvFile.close();
		return true;
	}
}

Result IQTModel::saveRecipeMetricsToCsv(QString PathName)
{
	QFile csvFile(PathName);
	if (!csvFile.open(QIODevice::Append))
	{
		return Result(false, QString("Open csv file error, %1.").arg(PathName).toStdString());
	}
	else
	{
		QString dutId = MetricsData::instance()->getDutTestSeqName();
		MetricsDataBase::getInstance()->updateOneFieldValue("SerialNumber", "testinfo", dutId);

		QDateTime startTime = MetricsData::instance()->getStartRecipeTime();
		QDateTime endTime = QDateTime::currentDateTime();
		int dec = startTime.secsTo(endTime);
		QString currentStr = startTime.toString("yyyy-MM-dd");
		QString timeStr = startTime.toString("hh:mm:ss");

		MetricsDataBase::getInstance()->updateOneFieldValue("Date", "testinfo", currentStr);
		MetricsDataBase::getInstance()->updateOneFieldValue("Time", "testinfo", timeStr);
		MetricsDataBase::getInstance()->updateOneFieldValue("TestTotalTime", "testinfo", QString::number(dec) + "s");
		MetricsDataBase::getInstance()->updateOneFieldValue(
			"DutAngle", "testinfo", QString::number(MetricsData::instance()->getDutAngle(), 'f', 3));
		//MetricsDataBase::getInstance()->updateOneFieldValue(
		//    "Diopter", "testinfo", QString::number(MLColorimeterMode::Instance()->getCurrentDiopters(), 'f', 3));

		QString eyeboxQueue = MetricsData::instance()->getEyeboxQueue();
		if (eyeboxQueue.isEmpty())
			return Result(false, "save recipe metrics to csv fail : eyeboxList is empty!");

		QMap<QString, QString> testResult = MetricsDataBase::getInstance()->getIQTestResult(eyeboxQueue);
		QMap<QString, QString>::iterator itor = testResult.begin();
		while (itor != testResult.end())
		{
			QString key = itor.key();
			QString value = itor.value();
			MetricsDataBase::getInstance()->updateOneFieldValue(key, "testinfo", value);
			itor++;
		}

		QStringList testInfo = MetricsDataBase::getInstance()->getTestInfoValues();
		csvFile.write(testInfo.join(",").toUtf8());
		csvFile.write(",");
		//QStringList wpcInfoList = MetricsDataBase::getInstance()->getWPCInfoValues();
		//csvFile.write(wpcInfoList.join(",").toUtf8());
		//csvFile.write(",");

		// TODO:: change the  QList<QString>  to  QString
		QStringList ValuesList = MetricsDataBase::getInstance()->getAllMetricsToCsv("value", eyeboxQueue);
		// QStringList ValuesList1 = MetricsDataBase::getInstance()->getAllMetricsToCsv("value",
		// GenCSVHeader::getMetricsEyeboxIds()) ;
		csvFile.write(ValuesList.join(",").toUtf8());
		csvFile.write("\n");
		csvFile.close();

		return Result(true, (testResult["TestResult"] == "PASS" ? "PASS" : "FAIL:" + testResult["TestFailItem"]).toStdString());
	}
}

void IQTModel::deleteDataBase()
{
	QString eyeboxQueue = MetricsData::instance()->getEyeboxQueue();
	QStringList eyeboxlist = eyeboxQueue.split(",");

	for (size_t i = 0; i < eyeboxlist.size(); i++)
	{
		MetricsDataBase::getInstance()->delIQMetricDatas(eyeboxlist[i].toInt());
	}

	MetricsDataBase::getInstance()->delTestInfoDatas();
}

void IQTModel::updateTestTimeinfo(QString date, QString time)
{
	QDateTime enddatetime(QDateTime::currentDateTime());
	enddatetime.setTimeSpec(Qt::LocalTime);
	uint endseconds = enddatetime.toTime_t();

	QDateTime startdateTime = QDateTime::fromString(date + ":" + time, "yyyyMMdd:hh:mm:ss");
	startdateTime.setTimeSpec(Qt::LocalTime);
	uint startseconds = startdateTime.toTime_t();

	uint totalseconds = endseconds - startseconds;
	MetricsDataBase::getInstance()->updateOneFieldValue("TestTotalTime", "testinfo",
		QString::number(totalseconds) + "s");
}

bool IQTModel::transposeCsv(QString PathName)
{
	QFile file(PathName);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream in(&file);
		QStringList lines;
		while (!in.atEnd())
		{
			QString line = in.readLine();
			lines.append(line);
		}

		int rowCount = lines.size();
		int columnCount = lines.isEmpty() ? 0 : lines.first().count(',');

		QStringList transposedLines;
		for (int c = 0; c < columnCount + 1; ++c)
		{
			QString transposedLine;
			for (int r = 0; r < rowCount; ++r)
			{
				QString line = lines.at(r);
				QStringList values = line.split(',');
				if (values.size() != columnCount + 1)
				{
					return false;
				}
				transposedLine.append(values.at(c));
				if (r < rowCount - 1)
					transposedLine.append(",");
			}
			transposedLines.append(transposedLine);
		}
		int last = PathName.lastIndexOf('\\');
		QString lastPart = PathName.mid(last + 1);
		QString path = PathName.mid(0, last + 1);
		QString newPath = path + "T_" + lastPart;

		QFile outputFile(newPath);
		if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QTextStream out(&outputFile);
			out << transposedLines.join("\n");
			outputFile.close();
		}

		file.close();
		return true;
	}
}

void IQTModel::calulateAllMetrics()
{
	QString qimgPath = MetricsData::instance()->getMTFImgsDir(); // +processDataDir;
	std::string cImgPath = qimgPath.toStdString();
	for each (MetricDescription * var in *metricVec)
	{
		curTargetMetric = *var;
		QString metricName = QString::fromStdString(curTargetMetric.name);
		std::vector<std::string> colors = curTargetMetric.colors;
		std::vector<std::string> imgNames = curTargetMetric.images;
		std::vector<std::string> eyeboxIds = curTargetMetric.eyeboxId;
		QString currentId;
		if (colors.empty())
		{
			params.clear();
			bool paramsReady = true;
			for each (std::string imgname in imgNames)
			{

				std::string imgFileName = cImgPath + imgname + ".tif";
				if (!std::filesystem::exists(imgFileName))
				{
					LoggingWrapper::instance()->error("error filename:" + QString::fromStdString(imgFileName) + "is not exist");
					paramsReady = false;
					break;
				}
				else
				{
					cv::Mat img = cv::imread(imgFileName, -1);
					params.push_back(img);
				}
			}
			if (!paramsReady)
			{
				continue;
			}
			MetricCalculator metricFunc = metricCalculators.value(metricName);
			if (metricFunc == nullptr)
			{
				LoggingWrapper::instance()->error("can not find metric function");
				continue;
			}
			QString result = (this->*metricFunc)("", metricName, params, currentId);
		}
		else
		{
			for each (std::string eyeboxId in eyeboxIds)
			{
				currentId = QString::fromStdString(eyeboxId);
				if (currentId.toStdString() == eyeboxId)

					for each (std::string color in colors)
					{
						params.clear();
						bool paramsReady = true;
						for each (std::string imgname in imgNames)
						{

							std::string imgFileName = cImgPath + imgname + ".tif";
							if (imgFileName.find("$color$") != std::string::npos)
							{
								imgFileName = imgFileName.replace(imgFileName.find("$color$"), 7, color);
							}
							if (imgFileName.find("$eyeboxId$") != std::string::npos)
							{
								imgFileName = imgFileName.replace(imgFileName.find("$eyeboxId$"), 10, eyeboxId);
								IC(imgFileName);
							}
							if (!std::filesystem::exists(imgFileName))
							{
								LoggingWrapper::instance()->error("error filename:" + QString::fromStdString(imgFileName) + "is not exist");

								paramsReady = false;
								break;
							}
							else
							{
								cv::Mat img = cv::imread(imgFileName, 0);
								params.push_back(img);
							}
						}
						if (!paramsReady)
							continue;
						MetricCalculator metricFunc = metricCalculators.value(metricName);
						if (metricFunc == nullptr)
						{
							LoggingWrapper::instance()->error("can not find metric function");
							continue;
						}
						(this->*metricFunc)(QString::fromStdString(color), metricName, params, currentId);
						if (curTargetMetric.releate.size() > 0)
						{
							for each (std::string var in curTargetMetric.releate)
							{
								MetricCalculator metricFunc = metricCalculators.value(QString::fromStdString(var));
								if (metricFunc == nullptr)
								{
									LoggingWrapper::instance()->error("can not find metric function");
									return;
								}
								(this->*metricFunc)(QString::fromStdString(color), QString::fromStdString(var),
									params, currentId);
							}
						}
					}
			}
		}
	}
}

Result IQTModel::calulateOneMetricsByRecipe(QString metricName, QString eyeboxIDs, QString colorT)
{
	Result re;
	MetricDescription curTargetMetric;
	for each (MetricDescription * var in *metricVec)
	{
		if (var->name == metricName.toStdString())
		{
			curTargetMetric = *var;
			break;
		}
	}
	if (curTargetMetric.name.empty())
	{
		LoggingWrapper::instance()->error("metrics name is empty");
		return Result(false, "metrics name is empty");
	}
	QString qimgPath = MetricsData::instance()->getMTFImgsDir();
	std::string cImgPath = qimgPath.toStdString();
	IC(cImgPath);
	std::vector<std::string> colors = curTargetMetric.colors;
	std::vector<std::string> imgNames = curTargetMetric.images;
	std::vector<std::string> eyeboxIds;
	if (eyeboxIDs == "")
	{
		QString eyeboxList = MetricsData::instance()->getEyeboxQueue();
		QStringList ebs = eyeboxList.split(",");
		std::unordered_set<std::string> targetSet(
			curTargetMetric.eyeboxId.begin(), curTargetMetric.eyeboxId.end());


		for (int i = 0; i < ebs.size(); i++)
		{
			std::string id = ebs[i].toStdString();
			if (targetSet.count(id) > 0)
			{
				eyeboxIds.push_back(id);
			}
		}
	}
	else
	{
		QStringList ebs = eyeboxIDs.split(",");
		for (size_t i = 0; i < ebs.length(); i++)
		{
			eyeboxIds.push_back(ebs[i].toStdString());
		}
	}

	QString currentId;
	std::vector<cv::Mat> params;
	if (colors.empty())
	{
		//params.clear();
		bool paramsReady = true;
		for each (std::string imgname in imgNames)
		{
			imgname = MetricsData::instance()->getImageNDFilter(QString::fromStdString(imgname)).toStdString();
			std::string imgFileName = cImgPath + imgname + ".tif";
			IC(imgFileName);
			if (!std::filesystem::exists(imgFileName))
			{
				LoggingWrapper::instance()->error("error filename:" + QString::fromStdString(imgFileName) + "is not exist");
				paramsReady = false;
				//  re.success = false;
				// re.errorMsg = "error fiename:" + (imgFileName);
				break;
			}
			else
			{
				cv::Mat img = cv::imread(imgFileName, -1);
				params.push_back(img);
			}
		}
		if (!paramsReady)
		{
			return Result();
		}
		MetricCalculator metricFunc = metricCalculators.value(metricName);
		if (metricFunc == nullptr)
		{
			LoggingWrapper::instance()->error("can not find metric function");
			return Result(false, "can not find metric function");
		}
		IC(metricFunc, metricName.toStdString());
		QString result = (this->*metricFunc)("", metricName, params, currentId);
		if (curTargetMetric.releate.size() > 0)
		{
			for each (std::string var in curTargetMetric.releate)
			{
				MetricCalculator metricFunc2 = metricCalculators.value(QString::fromStdString(var));
				if (metricFunc2 == nullptr)
				{
					LoggingWrapper::instance()->error("can not find metric function");
					return Result(false, "can not find metric function");
				}
				(this->*metricFunc2)("", QString::fromStdString(var), params, currentId);
			}
		}
	}
	else
	{
		IC();
		for each (std::string eyeboxId in eyeboxIds)
		{
			// currentId = "1";//TODO Value needs to be passed
			// if (currentId.toStdString() == eyeboxId)
			currentId = QString::fromStdString(eyeboxId);
			{
				for each (std::string color in colors)
				{
					bool paramsReady = true;
					params.clear();
					for each (std::string imgname in imgNames)
					{
						imgname = MetricsData::instance()->getImageNDFilter(QString::fromStdString(imgname)).toStdString();
						std::string imgFileName = cImgPath + imgname + ".tif";
						IC(imgFileName);
						if (imgFileName.find("$color$") != std::string::npos)
						{
							imgFileName = imgFileName.replace(imgFileName.find("$color$"), 7, color);
							IC(imgFileName);
						}
						if (imgFileName.find("$eyeboxId$") != std::string::npos)
						{
							imgFileName = imgFileName.replace(imgFileName.find("$eyeboxId$"), 10, eyeboxId);
							IC(imgFileName);
						}
						if (!std::filesystem::exists(imgFileName))
						{
							LoggingWrapper::instance()->error("error filename:" + QString::fromStdString(imgFileName) + "is not exist");
							// re.success = false;
							//  re.errorMsg = "erroe fiename:" +(imgFileName);
							paramsReady = false;
							break;
						}
						else
						{
							cv::Mat img = cv::imread(imgFileName, -1);
							params.push_back(img);
						}
					}
					if (!paramsReady)
						continue;
					MetricCalculator metricFunc = metricCalculators.value(metricName);
					if (metricFunc == nullptr)
					{
						LoggingWrapper::instance()->error("can not find metric function");
						// re.success = false;
						// re.errorMsg = "can not find metric function";
						continue;
						// return Result(false, "can not find metric function");
					}
					IC();
					IC(metricFunc, metricName.toStdString());
					QString result = (this->*metricFunc)(QString::fromStdString(color), metricName, params, currentId);
					if (curTargetMetric.releate.size() > 0)
					{
						for each (std::string var in curTargetMetric.releate)
						{

							MetricDescription* relDes = nullptr;
							for each (MetricDescription * des in *metricVec)
							{
								if (des->name == var)
								{
									relDes = des;
									break;
								}
							}
							if (relDes != nullptr)
							{
								for (int i = 0; i < relDes->colors.size(); i++)
								{
									if (color == relDes->colors.at(i))
									{
										MetricCalculator metricFunc2 =
											metricCalculators.value(QString::fromStdString(var));
										if (metricFunc2 == nullptr)
										{
											LoggingWrapper::instance()->error("can not find metric function");
											return Result(false, "can not find metric function");
										}
										IC(color, relDes->name);
										(this->*metricFunc2)(QString::fromStdString(color), QString::fromStdString(var),
											params, currentId);
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return Result();
}

cv::Mat IQTModel::getGraySacleImage(bool saveRawData, QString name, QString binnstr)
{
	//GrayscaleImageCapture capture;
	cv::Mat img/* = capture.getUndistortImage(binnstr.toStdString())*/;
	QString imgPaths = MetricsData::instance()->getMTFImgsDir();
	if (saveRawData)
	{
		QString fileName = imgPaths + name;

		if (!img.empty())
		{
			QString chromaY = fileName + ".tif";
			cv::imwrite(chromaY.toStdString(), img);
		}
	}
	return img;
}
void IQTModel::myRotateAntiClockWise90(Mat& src)
{
	if (src.empty())
	{
		return;
	}
	transpose(src, src);
	flip(src, src, 1);
}
float IQTModel::getExposureTime(QString filePath, QString name)
{
	float time = -1;
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return time;
	QTextStream stream(&file);
	while (!stream.atEnd())
	{
		QString line = stream.readLine();
		// QStringList row = line.split(',', Qt::SkipEmptyParts);
		QStringList list = line.split(";");
		if (list.size() == 4)
		{
			QStringList namePair = list.at(0).split(":");
			QString reticleName = namePair[1].trimmed();
			if (reticleName == name)
			{
				QStringList exposureTime = list.at(3).split(":");
				QString ext = exposureTime[1].trimmed();
				time = ext.toFloat() / 1000.0;
				break;
			}
		}
	}
	file.close();
	return time;
}
MLIQMetrics::ROIParaNew IQTModel::getLuminanceROIandRoMiInfo()
{
	MLIQMetrics::ROIParaNew para;
	//MLColorimeterMode::Instance()->getRotationMirror(para.rotationAngle, para.mirror);

	QString lumiROI = MetricsData::instance()->getLuminanceInfo();
	int index = lumiROI.indexOf("/");
	QStringList list = lumiROI.split("/");
	for (int i = 0; i < list.size(); i++)
	{
		QStringList paramPair = list.at(i).split("=");
		if (paramPair.size() == 2)
		{
			QString key = paramPair[0].trimmed();
			QString val = paramPair[1].trimmed();
			if (key == "FOVOffsetX")
			{
				para.cenX = val.toFloat();
			}
			else if (key == "FOVOffsetY")
			{
				para.cenY = val.toFloat();
			}
			else if (key == "ROIWidthX")
			{
				para.width = val.toFloat();
			}
			else if (key == "ROIHeightY")
			{
				para.height = val.toFloat();
			}
			else if (key == "Step")
			{
				para.step = val.toFloat();
			}
		}
	}

	return para;
}
void IQTModel::writeMatTOCSV(std::string filename, cv::Mat m)
{
	std::ofstream myfile;
	myfile.open(filename.c_str());
	myfile << cv::format(m, cv::Formatter::FMT_CSV) << std::endl;
	myfile.close();
}
void IQTModel::writeMatTOCSV(std::string filename, vector<cv::Mat> mVec, vector<string> infoVec)
{
	if (mVec.size() != infoVec.size())
		return;
	std::ofstream myfile;
	myfile.open(filename.c_str());
	for (int i = 0; i < infoVec.size(); i++)
	{
		if (infoVec[i].size() > 0)
			myfile << infoVec[i] << endl;
		myfile << cv::format(mVec[i], cv::Formatter::FMT_CSV) << std::endl;
	}
	myfile.close();
}
void IQTModel::changeAlogParam(QString param, QString val)
{
	// if (param == "brightness")
	//{
	//    int brightnessVal = val.toInt();
	//    if (brightnessVal > 0)
	//    {
	//        chroma.changeWhitepointBrightnesslevel(brightnessVal);
	//    }
	//}
}
void IQTModel::setSaveSlbData(bool isSave)
{
	m_saveSlbData = isSave;
}

void IQTModel::wakeupThread(bool isSuccess)
{
	QMutexLocker locker(&mutex);
	m_CalResult = isSuccess;
	condition.wakeOne();
}

void IQTModel::initPythonSDKCall()
{

	qRegisterMetaType<string>("string");
	//connect(this, &IQTModel::sigCalculateDistortion, PyController::getInstance(), &PyController::calculateDistortion,
	//        Qt::ConnectionType::QueuedConnection);
	//connect(this, &IQTModel::sigCalculateFlare, PyController::getInstance(), &PyController::calculateFlare,
	//        Qt::ConnectionType::QueuedConnection);
	//connect(this, &IQTModel::sigCalculateGhost, PyController::getInstance(), &PyController::calculateGhost,
	//        Qt::ConnectionType::QueuedConnection);
	//connect(this, &IQTModel::sigCalculatePupilSwim, PyController::getInstance(), &PyController::calculatePupilSwim,
	//        Qt::ConnectionType::QueuedConnection);
	//connect(this, &IQTModel::sigCalculateStrayLight, PyController::getInstance(), &PyController::calculateStrayLight,
	//        Qt::ConnectionType::QueuedConnection);
	connect(this, &IQTModel::sigCalculateColorUniformity, PyController::getInstance(),
		&PyController::calculateColorUniformity, Qt::ConnectionType::QueuedConnection);
	bool r = connect(this, &IQTModel::sigInitpython, PyController::getInstance(), &PyController::InitPyController);
	m_thread = new QThread(this);
	m_thread->setObjectName("PythonSDKCall");
	PyController::getInstance()->moveToThread(m_thread);
	//  connect(m_thread, &QThread::finished, this, &IQTModel::waitingThread);
	connect(m_thread, &QThread::finished, PyController::getInstance(), &QObject::deleteLater);
	connect(PyController::getInstance(), &PyController::sigCalculateFinish, this, &IQTModel::wakeupThread,
		Qt::ConnectionType::DirectConnection);
	m_thread->start();

	emit sigInitpython();
}

void IQTModel::saveCRToConfigFile(cv::Mat cr, string color, string id)
{
	string dir = "./config/slbCR/" + color + "_" + id +
		"_slbCR.csv";
	//PrintLog_::Log(LogType_::INFO_,
	//               "Save contrast SLB mode data[" + dir +
	//                   "]: ratio of the average ROI of the chessboard grid between the contrast pos and neg images");
	writeMatTOCSV(dir, cr);
}

void IQTModel::saveROICRToConfigFile(vector<double> cr, string color, string id)
{
	cv::Mat m = cv::Mat(cr).clone();
	string dir = "./config/slbCR/" + color + "_" + id +
		"_slbROICR.csv";
	//PrintLog_::Log(LogType_::INFO_, "Save contrast SLB mode roi data[" + dir +
	//                                    "]: The ratio of the average checkerboard pattern of the pos and neg regions "
	//                                    "in the middle ROI of the image");
	writeMatTOCSV(dir, m);
}

MLIQMetrics::FOVTYPE IQTModel::getFOVType()
{
	MLUtils::TestState state = MetricsData::instance()->GetTestState();
	bool IsLarge = state.size == MLUtils::Size::Large;
	FOVTYPE type;
	if (IsLarge)
		type = BIGFOV;
	else
		type = SMALLFOV;
	return type;
}
