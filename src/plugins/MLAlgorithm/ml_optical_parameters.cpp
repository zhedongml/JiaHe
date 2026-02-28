#include "ml_optical_parameters.h"
#include <color-util/XYZ_to_Lab.hpp>
#include "libcbdetect/boards_from_corners.h"
#include "libcbdetect/config.h"
#include "libcbdetect/find_corners.h"
#include "libcbdetect/plot_boards.h"
#include "libcbdetect/plot_corners.h"
#include <dmtx.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <opencv2/imgproc/types_c.h>
#include<iostream>
#include "ml_cross_center.h"
#include "ml_image_public.h"
//#include "measuremodel.h"
using namespace std;
using namespace algorithm;

MLOpticalParametersDUT::MLOpticalParametersDUT(){

}
MLOpticalParametersDUT::~MLOpticalParametersDUT(){

}
double algorithm::MLOpticalParametersDUT::PupilEfficiency(cv::Mat dut_clear, cv::Mat slb_clear, cv::Rect roiRect)
{
	double eff = 0;
	if (dut_clear.data!=NULL & slb_clear.data!=NULL)
	{
		cv::Mat retical_roi = slbMetrics.GetROIMat(dut_clear, roiRect);
		cv::Mat clear_roi = slbMetrics.GetROIMat(slb_clear, roiRect);
		cv::Scalar meanR, meanC;
		meanR = cv::mean(retical_roi);
		meanC = cv::mean(clear_roi);
		double lumiR = meanR(0);
		double lumiC = meanC(0);
		if (lumiR != 0)
		{
			eff = lumiR / lumiC;
		}
	}
	return eff;
	
}

double algorithm::MLOpticalParametersDUT::EyeboxThroughput(vector<double> pupil_efficiency, double Seyebox, double leyepupil)
{
	double result = 0.0;

	double Seyepupil = 0.0;
	Seyepupil = CV_PI * (leyepupil / 2) * (leyepupil / 2);
	if (pupil_efficiency.size() >= 5)
	{
		double sum= pupil_efficiency[0]+ pupil_efficiency[1] + pupil_efficiency[2] + pupil_efficiency[3] + pupil_efficiency[4];
		if(Seyepupil!=0)
		result = sum / 5.0 * Seyebox / Seyepupil;
	}
	
	return result;
}

double algorithm::MLOpticalParametersDUT::CenterContrastDUTBigChessboard(cv::Mat imgP, cv::Mat imgN, cv::Mat slbPos,  cv::Mat slbNeg, roiAreaData re, cv::Rect roiRect)
{
	double cr = 0;

	if (imgP.data != NULL & imgN.data != NULL&slbPos.data != NULL & slbNeg.data != NULL)
	{	

		cv::Mat posROI_gray = slbMetrics.GetROIMat(imgP, roiRect);
		cv::Mat negROI_gray = slbMetrics.GetROIMat(imgN, roiRect);

		vector<cv::Point2f>pointsP, pointsN;
		vector<cv::Rect>roiVecP, roiVecN;
		roiVecP = re.roiVecPC;
		roiVecN = re.roiVecNC;
		//string filepathP = "D:\\iqt7\\src\\app\\config\\IQT7CR_ROI\\P_centerROI_big.txt";
	//	string filepahtN = "D:\\iqt7\\src\\app\\config\\IQT7CR_ROI\\N_centerROI_big.txt";
		//string filepathP = "./config_iqt/IQT7CR_ROI/P_centerROI_big.txt";
		//string filepahtN = "./config_iqt/IQT7CR_ROI/N_centerROI_big.txt";
		//readROItxtFile(filepathP, roiVecP);
		//readROItxtFile(filepahtN, roiVecN);

		//calculate the SLB CR 
		cv::Mat slbPosROI= slbMetrics.GetROIMat(slbPos, roiRect);
		cv::Mat slbNegROI = slbMetrics.GetROIMat(slbNeg, roiRect);
		vector<double> posValueSlb = slbMetrics.CalculateROIValue(slbPosROI, roiVecP);
		vector<double> negValueSlb = slbMetrics.CalculateROIValue(slbNegROI, roiVecN);
		vector<double> centerCR_big = slbMetrics.CalculateCR(posValueSlb, negValueSlb);

		//slbMetrics.CenterContrastSLBBigChessboard(slbPos, slbNeg,roiRect);
		//vector<double> centerCR_big;
		//centerCR_big = slbMetrics.GetCenterCRBig();

		vector<double>posValueDut, negValueDut;
		posValueDut = slbMetrics.CalculateROIValue(posROI_gray, roiVecP);
		negValueDut = slbMetrics.CalculateROIValue(negROI_gray, roiVecN);
		vector<double>crVec = slbMetrics.CalculateCR(posValueDut, negValueDut);

		//draw need to change
		cv::Mat img_drawP = convertToUint8(posROI_gray);
		cv::Mat img_drawN = convertToUint8(negROI_gray);
		for (int i = 0; i < roiVecP.size(); i++)
		{
			cv::rectangle(img_drawP, roiVecP[i], cv::Scalar(255, 0, 255), 1);
			cv::rectangle(img_drawN, roiVecN[i], cv::Scalar(255, 0, 255), 1);
		}

		double crMean = 0;
		// calculate the valid data
		double sum = 0;
		int n = 0;
		for (int i = 0; i < crVec.size(); i++)
		{
			if (crVec[i] > 1e-9 & crVec[i] < 200)
			{
				sum = sum + crVec[i];
				n = n + 1;
			}
		}
		crMean = sum / n;
		vector<double> crDut;
		for (int i = 0; i < crVec.size(); i++)
		{
			double temp = 0;
			if (crMean != 0 & centerCR_big[i] != 0 & (crVec[i] > 1e-9 & crVec[i] < 200))
			{
				temp = 1.0 / (1.0 / crMean - 1.0 / centerCR_big[i]);
				if (temp > 0)
				{
					crDut.push_back(temp);
				}
			}
		}			
		cr = accumulate(crDut.begin(), crDut.end(), 0.0) / crDut.size();
	}

	return cr;
}

double algorithm::MLOpticalParametersDUT::HarmonicMeanContrastDUTBigChessboard(cv::Mat imgP, cv::Mat imgN, cv::Mat slbPos, cv::Mat slbNeg, roiAreaData re, cv::Rect roiRect)
{
	double crHValue = 0;
	if (imgP.data!=NULL & imgN.data != NULL& slbPos.data != NULL & slbNeg.data != NULL)
	{
		cv::Mat posROI_gray = slbMetrics.GetROIMat(imgP, roiRect);
		cv::Mat negROI_gray = slbMetrics.GetROIMat(imgN, roiRect);

		vector<cv::Point2f>pointsP, pointsN;
		vector<cv::Rect>roiVecP, roiVecN;
		roiVecP = re.roiVecP;
		roiVecN = re.roiVecN;
		//string filepathP = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\P_allROI_big.txt"; 
		//string filepahtN = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\N_allROI_big.txt";
		//string filepathP = "./config_iqt/IQT7CR_ROI/P_allROI_big.txt";//
		//string filepahtN ="./config_iqt/IQT7CR_ROI/N_allROI_big.txt";
		//readROItxtFile(filepathP, roiVecP);
		//readROItxtFile(filepahtN, roiVecN);
	
		//calculate the SLB CR 
		cv::Mat slbPosROI = slbMetrics.GetROIMat(slbPos, roiRect);
		cv::Mat slbNegROI = slbMetrics.GetROIMat(slbNeg, roiRect);
		vector<double> posValueSlb = slbMetrics.CalculateROIValue(slbPosROI, roiVecP);
		vector<double> negValueSlb = slbMetrics.CalculateROIValue(slbNegROI, roiVecN);
		vector<double> allCR_big = slbMetrics.CalculateCR(posValueSlb, negValueSlb);
		//slbMetrics.ArithemeticMeanContrastSLBBigChessboard(slbPos, slbNeg);
		//vector<double> allCR_big;
		//allCR_big = slbMetrics.GetAllCRbig();

		vector<double>posValueDut, negValueDut;
		posValueDut = slbMetrics.CalculateROIValue(posROI_gray, roiVecP);
		negValueDut = slbMetrics.CalculateROIValue(negROI_gray, roiVecN);
		vector<double>crVec=slbMetrics.CalculateCR(posValueDut, negValueDut);


		double  maxValue;    // 最大值，最小值
		cv::minMaxLoc(posROI_gray, NULL, &maxValue);
		double maxValueP = maxValue;
		cv::minMaxLoc(negROI_gray, NULL, &maxValue);
		double maxValueN = maxValue;
		maxValue = std::max(maxValueP,maxValueN);
		vector<double>crVecCali = CalculateCRCalibration(crVec, allCR_big, posValueDut, negValueDut, maxValue);
		vector<double>crInverseVec = slbMetrics.CalculatInverse(crVecCali);

		//draw
		cv::Mat img_drawP = convertToUint8(posROI_gray);
		cv::Mat img_drawN = convertToUint8(negROI_gray);
		for (int i = 0; i < roiVecP.size(); i++)
		{
			cv::rectangle(img_drawP, roiVecP[i], cv::Scalar(255, 0, 255), 1);
			cv::rectangle(img_drawN, roiVecN[i], cv::Scalar(255, 0, 255), 1);
		}

		for (int i = 0; i < crVec.size(); i++)
		{
			double temp = 0;
			double max = std::max(posValueDut[i], negValueDut[i]);
			if ( allCR_big[i] != 0&max> maxValue *0.1&(crVec[i]>0&crVec[i]<200))
			{
				cv::rectangle(img_drawP, roiVecP[i], cv::Scalar(255, 0, 255), 1);
				cv::rectangle(img_drawN, roiVecN[i], cv::Scalar(255, 0, 255), 1);
			}
		}	

		if (crInverseVec.size() > 0)
		{
			crHValue = crInverseVec.size() / accumulate(crInverseVec.begin(), crInverseVec.end(), 0.0);
		}
	}
	return crHValue;

}

double algorithm::MLOpticalParametersDUT::ArithmeticMeanContrastDUTBigChessboard(cv::Mat imgP, cv::Mat imgN, cv::Mat slbPos, cv::Mat slbNeg, roiAreaData re, cv::Rect roiRect)
{
	double crValue = 0;
	if (imgP.data != NULL & imgN.data != NULL & slbPos.data != NULL & slbNeg.data != NULL)
	{
		cv::Mat posROI_gray = slbMetrics.GetROIMat(imgP, roiRect);
		cv::Mat negROI_gray = slbMetrics.GetROIMat(imgN, roiRect);
		vector<cv::Point2f>pointsP, pointsN;
		vector<cv::Rect>roiVecP, roiVecN;
		roiVecP = re.roiVecP;
		roiVecN = re.roiVecN;
		//string filepathP = "D:\\iqt7\\src\\app\\config\\IQT7CR_ROI\\P_allROI_big.txt"; 
		//string filepahtN = "D:\\iqt7\\src\\app\\config\\IQT7CR_ROI\\N_allROI_big.txt";
		//string filepathP ="./config/IQT7CR_ROI/P_allROI_big.txt";//
		//string filepahtN ="./config/IQT7CR_ROI/N_allROI_big.txt";
		//readROItxtFile(filepathP, roiVecP);
		//readROItxtFile(filepahtN, roiVecN);
	
		//calculate the SLB CR 
		cv::Mat slbPosROI= slbMetrics.GetROIMat(slbPos, roiRect);
		cv::Mat slbNegROI = slbMetrics.GetROIMat(slbNeg, roiRect);
		vector<double> posValueSlb = slbMetrics.CalculateROIValue(slbPosROI, roiVecP);
		vector<double> negValueSlb = slbMetrics.CalculateROIValue(slbNegROI, roiVecN);
		vector<double> allCR_big = slbMetrics.CalculateCR(posValueSlb, negValueSlb);
        // dut
		vector<double>posValueDut, negValueDut;
		posValueDut = slbMetrics.CalculateROIValue(posROI_gray, roiVecP);
		negValueDut = slbMetrics.CalculateROIValue(negROI_gray, roiVecN);
		vector<double>crVec=slbMetrics.CalculateCR(posValueDut, negValueDut);

		double minValue, maxValue;    // 最大值，最小值
		cv::Point  minIdx, maxIdx;    // 最小值坐标，最大值坐标     
		cv::minMaxLoc(posROI_gray, NULL, &maxValue);
		double maxValueP = maxValue;
		cv::minMaxLoc(negROI_gray, NULL, &maxValue);
		double maxValueN = maxValue;
		 maxValue = std::max(maxValueP, maxValueN);

		vector<double>crVecCali = CalculateCRCalibration(crVec, allCR_big, posValueDut, negValueDut, maxValue);

		//draw
		cv::Mat img_drawP = convertToUint8(posROI_gray);
		cv::Mat img_drawN = convertToUint8(negROI_gray);
		for (int i = 0; i < crVec.size(); i++)
		{			
			double max = std::max(posValueDut[i], negValueDut[i]);
			double temp = 0;
			if ( allCR_big[i] != 0&max> maxValue *0.1&(crVec[i]>1e-9&crVec[i]<200))
			{
				cv::rectangle(img_drawP, roiVecP[i], cv::Scalar(255, 0, 255), 1);
				cv::rectangle(img_drawN, roiVecN[i], cv::Scalar(255, 0, 255), 1);
			}

		}
		if (crVecCali.size() > 0)
		{
			crValue = accumulate(crVecCali.begin(), crVecCali.end(), 0.0) / crVecCali.size();
		}
	}
	return crValue;
}

double algorithm::MLOpticalParametersDUT::CenterContrastDUTSmallChessboard(cv::Mat imgP, cv::Mat imgN, cv::Mat slbPos, cv::Mat slbNeg, roiAreaData re, cv::Rect roiRect)
{
	double cr = 0;
	if (imgP.data != NULL & imgN.data != NULL & slbPos.data != NULL & slbNeg.data != NULL)
	{
		cv::Mat posROI_gray = slbMetrics.GetROIMat(imgP, roiRect);
		cv::Mat negROI_gray = slbMetrics.GetROIMat(imgN, roiRect);
		vector<cv::Point2f>pointsP, pointsN;
		vector<cv::Rect>roiVecP, roiVecN;
		roiVecP = re.roiVecPC;
		roiVecN = re.roiVecNC;

		//string filepathP = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\P_centerROI_small.txt";
	//	string filepahtN = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\N_centerROI_small.txt";
		//string filepathP ="./config_iqt/IQT7CR_ROI/P_centerROI_small.txt";// 
		//string filepahtN = "./config_iqt/IQT7CR_ROI/N_centerROI_small.txt";

		//readROItxtFile(filepathP, roiVecP);
		//readROItxtFile(filepahtN, roiVecN);

		//calculate the SLB CR 
		//slbMetrics.CenterContrastSLBSmallChessboard(slbPos,slbNeg);
		//vector<double> centerCR_small;
		//centerCR_small = slbMetrics.GetCenterCRSmall();

		cv::Mat slbPosROI = slbMetrics.GetROIMat(slbPos, roiRect);
		cv::Mat slbNegROI = slbMetrics.GetROIMat(slbNeg, roiRect);
		vector<double> posValueSlb = slbMetrics.CalculateROIValue(slbPosROI, roiVecP);
		vector<double> negValueSlb = slbMetrics.CalculateROIValue(slbNegROI, roiVecN);
		vector<double> centerCR_small = slbMetrics.CalculateCR(posValueSlb, negValueSlb);

		vector<double>posValueDut, negValueDut;
		posValueDut = slbMetrics.CalculateROIValue(posROI_gray, roiVecP);
		negValueDut = slbMetrics.CalculateROIValue(negROI_gray, roiVecN);
		vector<double>crVec= slbMetrics.CalculateCR(posValueDut, negValueDut);

		//draw
		cv::Mat img_drawP = convertToUint8(posROI_gray);
		cv::Mat img_drawN = convertToUint8(negROI_gray);
		for (int i = 0; i < roiVecP.size(); i++)
		{
			cv::rectangle(img_drawP, roiVecP[i], cv::Scalar(0, 0, 255), 1);
			cv::rectangle(img_drawN, roiVecN[i], cv::Scalar(0, 0, 255), 1);
		}
		double crMean = 0;
		double sum = 0;
		int n = 0;
		for (int i = 0; i < crVec.size(); i++)
		{
			if (crVec[i] > 1e-9 & crVec[i] < 200)
			{
				sum = sum + crVec[i];
				n = n + 1;
			}
		}
		crMean = sum / n;

		vector<double> crDut;
		for (int i = 0; i < crVec.size(); i++)
		{
			double temp = 0;
			if (crMean != 0 & centerCR_small[i]!= 0 & (crVec[i] > 1e-9 & crVec[i] < 200))
			{
				 temp = 1.0 / (1.0 / crMean - 1.0 / centerCR_small[i]);
				 if(temp>0){ crDut.push_back(temp); }
				
			}		
		}
		cr = accumulate(crDut.begin(), crDut.end(), 0.0) / crDut.size();

	}

	return cr;
}

double algorithm::MLOpticalParametersDUT::HarmonicMeanContrastDUTSmallChessboard(cv::Mat imgP, cv::Mat imgN, cv::Mat slbPos, cv::Mat slbNeg, roiAreaData re, cv::Rect roiRect)
{
	double crHValue = 0;
	if (imgP.data!=NULL & imgN.data != NULL & slbPos.data != NULL & slbNeg.data != NULL)
	{
		cv::Mat posROI_gray = slbMetrics.GetROIMat(imgP, roiRect);
		cv::Mat negROI_gray = slbMetrics.GetROIMat(imgN, roiRect);
		vector<cv::Point2f>pointsP, pointsN;
		vector<cv::Rect>roiVecP, roiVecN;
		roiVecP = re.roiVecP;
		roiVecN = re.roiVecN;

		cv::Mat slbPosROI = slbMetrics.GetROIMat(slbPos, roiRect);
		cv::Mat slbNegROI = slbMetrics.GetROIMat(slbNeg, roiRect);
		vector<double> posValueSlb = slbMetrics.CalculateROIValue(slbPosROI, roiVecP);
		vector<double> negValueSlb = slbMetrics.CalculateROIValue(slbNegROI, roiVecN);
		vector<double> allCR_small = slbMetrics.CalculateCR(posValueSlb, negValueSlb);

		vector<double>posValueDut, negValueDut;
		posValueDut = slbMetrics.CalculateROIValue(posROI_gray, roiVecP);
		negValueDut = slbMetrics.CalculateROIValue(negROI_gray, roiVecN);
		vector<double>crVec = slbMetrics.CalculateCR(posValueDut, negValueDut);

		double  maxValue;    // 最大值，最小值
		cv::minMaxLoc(posROI_gray, NULL, &maxValue);
		double maxValueP = maxValue;
		cv::minMaxLoc(negROI_gray, NULL, &maxValue);
		double maxValueN = maxValue;
		 maxValue = std::max(maxValueP, maxValueN);
		vector<double>crVecCali = CalculateCRCalibration(crVec, allCR_small, posValueDut, negValueDut, maxValue);
		vector<double>crInverseVec = slbMetrics.CalculatInverse(crVecCali);

		//draw
		cv::Mat img_drawP = convertToUint8(posROI_gray);
		cv::Mat img_drawN = convertToUint8(negROI_gray);

		for (int i = 0; i < crVec.size(); i++)
		{
			double max = std::max(posValueDut[i], negValueDut[i]);
			double temp = 0;
			if (  allCR_small[i] != 0&max>maxValueP*0.1&(crVec[i]>1e-9&crVec[i]<200))
			{				
				cv::rectangle(img_drawP, roiVecP[i], cv::Scalar(255, 0, 255), 1);
				cv::rectangle(img_drawN, roiVecN[i], cv::Scalar(255, 0, 255), 1);				
			}
		}
		if (crInverseVec.size() > 0)
		{
			crHValue = crInverseVec.size() / accumulate(crInverseVec.begin(), crInverseVec.end(), 0.0);
		}
	}
	return crHValue;
}
double algorithm::MLOpticalParametersDUT::ArithmeticMeanContrastDUTSmallChessboard(cv::Mat imgP, cv::Mat imgN, cv::Mat slbPos, cv::Mat slbNeg, roiAreaData re, cv::Rect roiRect)
{
	double crValue = 0;
	if (imgP.data != NULL & imgN.data != NULL & slbPos.data != NULL & slbNeg.data != NULL)
	{
		cv::Mat posROI_gray = slbMetrics.GetROIMat(imgP, roiRect);
		cv::Mat negROI_gray = slbMetrics.GetROIMat(imgN, roiRect);
		vector<cv::Point2f>pointsP, pointsN;
		vector<cv::Rect>roiVecP, roiVecN;
		roiVecP = re.roiVecP;
		roiVecN = re.roiVecN;
	//	string filepathP = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\P_allROI_small.txt";
	//	string filepahtN = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\N_allROI_small.txt";
		//string filepathP ="./config_iqt/IQT7CR_ROI/P_allROI_small.txt"; //
		//string filepahtN = "./config_iqt/IQT7CR_ROI/N_allROI_small.txt";//

		//readROItxtFile(filepathP, roiVecP);
		//readROItxtFile(filepahtN, roiVecN);
	
		//calculate the SLB CR 
		//slbMetrics.ArithemeticMeanContrastSLBSmallChessboard(slbPos, slbNeg);
		//vector<double> allCR_small;
		//allCR_small = slbMetrics.GetAllCRSmall();
		cv::Mat slbPosROI = slbMetrics.GetROIMat(slbPos, roiRect);
		cv::Mat slbNegROI = slbMetrics.GetROIMat(slbNeg, roiRect);
		vector<double> posValueSlb = slbMetrics.CalculateROIValue(slbPosROI, roiVecP);
		vector<double> negValueSlb = slbMetrics.CalculateROIValue(slbNegROI, roiVecN);
		vector<double> allCR_small = slbMetrics.CalculateCR(posValueSlb, negValueSlb);

		vector<double>posValueDut, negValueDut;
		posValueDut = slbMetrics.CalculateROIValue(posROI_gray, roiVecP);
		negValueDut = slbMetrics.CalculateROIValue(negROI_gray, roiVecN);
		vector<double>crVec=slbMetrics.CalculateCR(posValueDut, negValueDut);

		double  maxValue;    // 最大值，最小值
		cv::minMaxLoc(posROI_gray, NULL, &maxValue);
		double maxValueP = maxValue;
		cv::minMaxLoc(negROI_gray, NULL, &maxValue);
		double maxValueN = maxValue;
		 maxValue = std::max(maxValueP, maxValueN);
		vector<double>crVecCali = CalculateCRCalibration(crVec, allCR_small, posValueDut, negValueDut, maxValue);
		if (crVecCali.size() > 0)
		{
			crValue = accumulate(crVecCali.begin(), crVecCali.end(), 0.0) / crVecCali.size();
		}


		//draw
		cv::Mat img_drawP = convertToUint8(posROI_gray);
		cv::Mat img_drawN = convertToUint8(negROI_gray);

		for (int i = 0; i < crVec.size(); i++)
		{
			double max = std::max(posValueDut[i], negValueDut[i]);
			if (allCR_small[i] != 0&max> maxValue *0.1&(crVec[i]>1e-9&crVec[i]<200))
			{			
					cv::rectangle(img_drawP, roiVecP[i], cv::Scalar(255, 0, 255), 1);
					cv::rectangle(img_drawN, roiVecN[i], cv::Scalar(255, 0, 255), 1);			
			}
		}
														
	}
	return crValue;
}

double algorithm::MLOpticalParametersDUT::AngleUniformity(cv::Mat img, cv::Rect roiRect)
{
	double result = 0;
	if (img.data != NULL)
	{
		cv::Mat img_gray;
		if (roiRect.x != 0 & roiRect.y != 0)
		{
			img = img(roiRect);
		}
		if (img.channels() == 3)//turn gray
			cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
		else
			img_gray = img.clone();
		int row = img_gray.rows;
		int col = img_gray.cols;
		cv::Mat imgReshape = img_gray.reshape(1, row * col);
		cv::Mat imgReshape_sort;
		cv::sort(imgReshape, imgReshape_sort, CV_SORT_EVERY_ROW + CV_SORT_ASCENDING);
		int n95List = int(0.95 * row * col);
		int n5List = int(0.05 * row * col);
		double n95 = 0;
		double n5 = 1;
		if (imgReshape_sort.type() == 5)
		{
			n95 = imgReshape_sort.at<float>(n95List);
			n5 = imgReshape_sort.at<float>(n5List);
		}
		else
		{
			n95 = imgReshape_sort.at<uchar>(n95List);
			n5 = imgReshape_sort.at<uchar>(n5List);
		}
		if (n95 != 0)
		{
			result = n95 / n5;
		}
	}

	return result;
}

double algorithm::MLOpticalParametersDUT::SpatialUniformity(vector<double> pupil_efficiency)
{
	double result = 0.0;
	double maxValue = 0, minValue = 1.0;
	 maxValue = *max_element(pupil_efficiency.begin(), pupil_efficiency.end());
	 minValue = *min_element(pupil_efficiency.begin(), pupil_efficiency.end());
	if (minValue != 0.0)
		result = maxValue / minValue;
	return result;
}



bool sort_user(const vector<double>x, vector<double>& result, vector<int>& index)
{
	bool flag = true;
	vector<double>x1(x);
	sort(x1.begin(), x1.end());
	result.swap(x1);
	double temp;
	int num = x.size();
	//index(num, 0);

	for (int i = 0; i != x.size(); i++)
	{
		index.push_back(i);
	}
	sort(index.begin(), index.end(),
		[&](const int& a, const int& b)
		{
			return (x[a] < x[b]);
		}
	);

	return flag;
}


algorithm::MLOpticalParametersSystem::MLOpticalParametersSystem()
{
}

algorithm::MLOpticalParametersSystem::~MLOpticalParametersSystem()
{
}

void algorithm::MLOpticalParametersSystem::SetXYZImage(cv::Mat X, cv::Mat Y, cv::Mat Z)
{
	m_X = X;
	m_Y = Y;
	m_Z = Z;
}

void algorithm::MLOpticalParametersSystem::SetYImage(cv::Mat Y)
{
	m_Y = Y;
}

void algorithm::MLOpticalParametersSystem::SetChessboardImage(cv::Mat imgP, cv::Mat imgN, cv::Rect roiRect, int row , int col)
{
	m_imgP = imgP;
	m_imgN = imgN;
	m_roiRect = roiRect;
	m_row = row;
	m_col = col;
}

double algorithm::MLOpticalParametersSystem::GetUniformity()
{
	double result = 0.0;
	result = Uniformity(m_Y);
	return result;
}

double algorithm::MLOpticalParametersSystem::GetLuminance()
{
	double result = 0.0;
	result = Luminance(m_Y);
	return result;
}

double algorithm::MLOpticalParametersSystem::GetDeltE()
{
	double result = 0.0;
	result = DeltaE(m_X, m_Y,m_Z);
	return result;
}

double algorithm::MLOpticalParametersSystem::GetDeltC()
{
	double result = 0.0;
	result = DeltaE(m_X, m_Y, m_Z);
	return result;
}

double algorithm::MLOpticalParametersSystem::GetbigChessboardCR()
{
	double cr = 0.0;
	cr = bigChessboardCR(m_imgP,m_imgN);
	return cr;
}

double algorithm::MLOpticalParametersSystem::GetsmallChessboardCR()
{
	double cr = 0.0;
	cr = smallChessboardCR(m_imgP, m_imgN);
	return cr;
}

algorithm::MLOpticalParametersSLB::MLOpticalParametersSLB()
{

}

algorithm::MLOpticalParametersSLB::~MLOpticalParametersSLB()
{
}

void algorithm::MLOpticalParametersSLB::SetImageGhost(cv::Mat img_ghost)
{
	m_img_ghost = img_ghost;
}

void algorithm::MLOpticalParametersSLB::SetImageClear(cv::Mat img_clear)
{
	m_img_clear = img_clear;
}

//double algorithm::MLOpticalParametersDUT::GetPupilEfficency()
//{
//	double result = 0;
//	//result = PupilEfficiency(m_img_clear, m_roiRect);
//	return result;
//}


void algorithm::MLOpticalParametersSLB::SetChessboardImage(cv::Mat imgP, cv::Mat imgN, cv::Rect roiRect)
{
	m_imgP = imgP;
	m_imgN = imgN;
	m_roiRect = roiRect;
}

void algorithm::MLOpticalParametersSLB::SetFovPara(double magnification, double pixel_size, double FocalLength)
{
	m_magnification = magnification;
	m_pixel_size = pixel_size;
	m_FocalLength = FocalLength;
}

cv::Point2f algorithm::MLOpticalParametersSLB::GetCenterCoordinate()
{
	cv::Point2f center =CenterCoordinate(m_img_ghost);
	return center;
}

cv::Point2f algorithm::MLOpticalParametersSLB::GetFovAngle()
{
	cv::Point2f angle = FovAngle(m_img_clear,m_magnification,m_pixel_size,m_FocalLength);
	return angle;
}

double algorithm::MLOpticalParametersSLB::GetmeanY()
{
	double mean = meanY(m_img_clear);
	return mean;
}

double algorithm::MLOpticalParametersSLB::GetUniformity()
{
	double mean = Uniformity(m_img_clear);
	return mean;
}


cv::Mat algorithm::MLOpticalParametersSLB::GetImageClear()
{
	return m_img_clear;
}

double algorithm::MLOpticalParametersSystem::Uniformity(cv::Mat img)
{
	double result = 0;
	if (img.data!=NULL)
	{
		//cv::transpose(img, img);
		//cv::Mat img1 = img(cv::Rect(125, 1, 1441, 1095));
		cv::Mat img_gray;
		if (img.channels() == 3)//turn gray
			cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
		else
			img_gray = img.clone();
		int row = img_gray.rows;
		int col = img_gray.cols;
		cv::Mat imgReshape = img_gray.reshape(1, row*col);
		cv::Mat imgReshape_sort;
		cv::sort(imgReshape, imgReshape_sort, CV_SORT_EVERY_ROW + CV_SORT_ASCENDING);
		int n95List = int(0.95 * row * col);
		int n5List = int(0.05 * row * col);
		double n95 = 0;
		double n5 = 1;

		if (imgReshape_sort.type() == 5)
		{
			n95 = imgReshape_sort.at<float>(n95List);
			n5 = imgReshape_sort.at<float>(n5List);
		}
		else
		{
			n95 = imgReshape_sort.at<uchar>(n95List);
			n5 = imgReshape_sort.at<uchar>(n5List);
		}
		if (n95 != 0)
		{
			result = n95 / n5;
		}       
	}
	return result;
}

double algorithm::MLOpticalParametersSystem::Luminance(cv::Mat I)
{
	double result = 0;;
	cv::Mat img_gray;
	if (I.data)
	{
		if (I.channels() == 3)
			cvtColor(I, img_gray, cv::COLOR_BGR2GRAY);
		else
			img_gray = I.clone();

		cv::Scalar mean;
		mean = cv::mean(img_gray);
		result = mean(0);
	}

	return result;
}

double algorithm::MLOpticalParametersSystem::DeltaE(cv::Mat X, cv::Mat Y, cv::Mat Z)
{
	cv::Mat deltaE;
	cv::Mat x_gray, y_gray, z_gray;

	if (X.channels() == 3)
		cvtColor(X, x_gray, cv::COLOR_BGR2GRAY);
	else
		x_gray = X.clone();

	if (Y.channels() == 3)
		cvtColor(Y, y_gray, cv::COLOR_BGR2GRAY);
	else
		y_gray = Y.clone();

	if (Z.channels() == 3)
		cvtColor(Z, z_gray, cv::COLOR_BGR2GRAY);
	else
		z_gray = Z.clone();

	int rows = X.rows;
	int cols = X.cols;
	/*
	if (x_gray.size() == y_gray.size() & y_gray.size() == z_gray.size())
	{
		int rows = x_gray.rows;
		int cols = x_gray.cols;
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				double x = x_gray.at<float>(i, j);
				double y = y_gray.at<float>(i, j);
				double z = z_gray.at<float>(i, j);
				const colorutil::RGB xyz_color(x, y, z);
				const colorutil::Lab lab_color = colorutil::convert_XYZ_to_Lab(xyz_color);
				double l = lab_color(0);
				double a = lab_color(1);
				double b = lab_color(2);
				deltaE.at<float>(i, j) = sqrt((l - 100) * (l - 100) + a * a + b * b);
			}
		}
	}
	*/
	cv::Scalar meanX;
	meanX = cv::mean(x_gray);
	cv::Scalar meanY;
	meanY = cv::mean(y_gray);
	cv::Scalar meanZ;
	meanZ = cv::mean(z_gray);
	const colorutil::RGB xyz_color(meanX(0), meanY(0), meanZ(0));
	const colorutil::Lab lab_color = colorutil::convert_XYZ_to_Lab(xyz_color);
	double l = lab_color(0);
	double a = lab_color(1);
	double b = lab_color(2);
	double result = sqrt((l - 100) * (l - 100) + a * a + b * b);
	//cv::Scalar mean;
	//mean = cv::mean(deltaE);
	//double result = mean(0);

	return result;
}

double algorithm::MLOpticalParametersSystem::DeltaC(cv::Mat X, cv::Mat Y, cv::Mat Z)
{
	cv::Mat deltaC;
	cv::Mat x_gray, y_gray, z_gray;

	if (X.channels() == 3)
		cvtColor(X, x_gray, cv::COLOR_BGR2GRAY);
	else
		x_gray = X.clone();

	if (Y.channels() == 3)
		cvtColor(Y, y_gray, cv::COLOR_BGR2GRAY);
	else
		y_gray = Y.clone();

	if (Z.channels() == 3)
		cvtColor(Z, z_gray, cv::COLOR_BGR2GRAY);
	else
		z_gray = Z.clone();

	int rows = X.rows;
	int cols = X.cols;
	/*
	if (x_gray.size() == y_gray.size() & y_gray.size() == z_gray.size())
	{
		int rows = x_gray.rows;
		int cols = x_gray.cols;
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				double x = x_gray.at<float>(i, j);
				double y = y_gray.at<float>(i, j);
				double z = z_gray.at<float>(i, j);
				const colorutil::RGB xyz_color(x, y, z);
				const colorutil::Lab lab_color = colorutil::convert_XYZ_to_Lab(xyz_color);
				double l = lab_color(0);
				double a = lab_color(1);
				double b = lab_color(2);
				deltaC.at<float>(i, j) = sqrt(a * a + b * b);
			}
		}
	}
	cv::Scalar mean;
	mean = cv::mean(deltaC);
	double result = mean(0);
	*/
	cv::Scalar meanX;
	meanX = cv::mean(x_gray);
	cv::Scalar meanY;
	meanY = cv::mean(y_gray);
	cv::Scalar meanZ;
	meanZ = cv::mean(z_gray);
	const colorutil::RGB xyz_color(meanX(0), meanY(0), meanZ(0));
	const colorutil::Lab lab_color = colorutil::convert_XYZ_to_Lab(xyz_color);
	double l = lab_color(0);
	double a = lab_color(1);
	double b = lab_color(2);
	double result = sqrt(a * a + b * b);
	return result;
}

double algorithm::MLOpticalParametersSystem::bigChessboardCR(cv::Mat imgP, cv::Mat imgN, int row, int col, cv::Rect roiRect)
{
	double cr = 0;
	cv::Mat temp = cv::imread("./config/systemTemplate/bigchesstemplate.tif");// cv::imread("D:\\iqt7\\src\\app\\config\\systemTemplate\\bigchesstemplate.tif");
	cv::Mat temp_gray;
	if (temp.channels() == 3)
		cvtColor(temp, temp_gray, cv::COLOR_BGR2GRAY);
	else
		temp_gray = temp.clone();
	cv::Mat posRoi, negRoi,posRoi_gray,negRoi_gray;
	if (roiRect.x == 0 & roiRect.y == 0)
	{
		posRoi = imgP.clone();
		negRoi = imgN.clone();
	}
	else
	{
		posRoi = imgP(roiRect);
		negRoi = imgN(roiRect);
	}
	if (posRoi.channels() == 3)
		cvtColor(posRoi, posRoi_gray, cv::COLOR_BGR2GRAY);
	else
		posRoi_gray = posRoi.clone();

	if (negRoi.channels() == 3)
		cvtColor(negRoi, negRoi_gray, cv::COLOR_BGR2GRAY);
	else
		negRoi_gray = negRoi.clone();
	vector<double>posValue, negValue;
	vector<cv::Rect> crROI = GetCRrect();
	cv::Mat img_draw = posRoi.clone();
	 if (crROI.size() == 9)
	 {
		 for (int i = 0; i < crROI.size(); i++)
		 {
			 cv::Mat roiP = posRoi_gray(crROI[i]);
			 cv::Scalar meanP;
			 meanP = cv::mean(roiP);
			 posValue.push_back(meanP(0));

			 cv::Mat roiN = negRoi_gray(crROI[i]);
			 cv::Scalar  meanN;
			 meanN = cv::mean(roiN);
			 negValue.push_back(meanN(0));
			cv::rectangle(img_draw, crROI[i], cv::Scalar(0, 255, 0), 1);
		 }
	 }
	 else
	 {
		 bool posBool = getCRArea(posRoi_gray, temp_gray, row, col, 20, 9);
		 crROI = GetCRrect();
		 for (int i = 0; i < crROI.size(); i++)
		 {
			 cv::Mat roiP = posRoi_gray(crROI[i]);
			 cv::Scalar meanP;
			 meanP = cv::mean(roiP);
			 posValue.push_back(meanP(0));

			 cv::Mat roiN = negRoi_gray(crROI[i]);
			 cv::Scalar  meanN;
			 meanN = cv::mean(roiN);
			 negValue.push_back(meanN(0));
			 cv::rectangle(img_draw, crROI[i], cv::Scalar(0, 255, 0), 1);

		 }		 
	 }

	//bool posBool = getCRArea(posRoi_gray, temp_gray, row, col,20,9, posValue);
	//bool negBool = getCRArea(negRoi_gray, temp_gray, row, col,20,9, negValue);
	if (posValue.size()==negValue.size()&posValue.size()>0)
	{
		vector<double>crVec;
		for (int i = 0; i < posValue.size(); i++)
		{
			if (posValue[i] > negValue[i])
				crVec.push_back(posValue[i]/negValue[i]);
			else
				crVec.push_back(negValue[i]/posValue[i]);
		}
		cr = accumulate(crVec.begin(), crVec.end(), 0.0) / crVec.size();
	}
	return cr;
}

double algorithm::MLOpticalParametersSystem::smallChessboardCR(cv::Mat imgP, cv::Mat imgN, int row, int col, cv::Rect roiRect)
{
	double cr = 0;
	cv::Mat temp = cv::imread("./config/systemTemplate/smallchesstemplate.tif");//cv::imread("D:\\iqt7\\src\\app\\config\\systemTemplate\\smallchesstemplate.tif");
	cv::Mat temp_gray;
	if (temp.channels() == 3)
		cvtColor(temp, temp_gray, cv::COLOR_BGR2GRAY);
	else
		temp_gray = temp.clone();
	cv::Mat posRoi, negRoi, posRoi_gray, negRoi_gray;

	if (imgP.rows > 2000 & imgP.cols > 2000)
		roiRect = cv::Rect(imgP.cols /2 - 200, imgP.rows /2 - 200, 400, 400);
	else
		roiRect = cv::Rect(imgP.cols /2 - 50, imgP.rows /2 - 50, 100, 100);

//	cv::Rect(3288 - 200, 2192 - 200, 400, 400);

	if (roiRect.x == 0 & roiRect.y == 0)
	{
		posRoi = imgP.clone();
		negRoi = imgN.clone();
	}
	else
	{
		posRoi = imgP(roiRect);
		negRoi = imgN(roiRect);
	}
	if (posRoi.channels() == 3)
		cvtColor(posRoi, posRoi_gray, cv::COLOR_BGR2GRAY);
	else
		posRoi_gray = posRoi.clone();

	if (negRoi.channels() == 3)
		cvtColor(negRoi, negRoi_gray, cv::COLOR_BGR2GRAY);
	else
		negRoi_gray = negRoi.clone();
	cv::Mat img_draw = negRoi.clone();
	vector<double>posValue, negValue;
	vector<cv::Rect>crROI = GetCRrect();
	if (crROI.size() == 16)
	{
		for (int i = 0; i < crROI.size(); i++)
		{
			cv::Mat roiP = posRoi_gray(crROI[i]);
			cv::Scalar meanP;
			meanP = cv::mean(roiP);
			posValue.push_back(meanP(0));

			cv::Mat roiN = negRoi_gray(crROI[i]);
			cv::Scalar  meanN;
			meanN = cv::mean(roiN);
			negValue.push_back(meanN(0));
			cv::rectangle(img_draw, crROI[i], cv::Scalar(0, 255, 0), 1);

		}

	}
	else
	{
		bool posBool = getCRArea(posRoi_gray, temp_gray, row, col, 10, 30);
		crROI = GetCRrect();
		for (int i = 0; i < crROI.size(); i++)
		{
			cv::Mat roiP = posRoi_gray(crROI[i]);
			cv::Scalar meanP;
			meanP = cv::mean(roiP);
			posValue.push_back(meanP(0));

			cv::Mat roiN = negRoi_gray(crROI[i]);
			cv::Scalar  meanN;
			meanN = cv::mean(roiN);
			negValue.push_back(meanN(0));
			cv::rectangle(img_draw, crROI[i], cv::Scalar(0, 255, 0), 1);

		}
	}
	//bool posBool = getCRArea(posRoi_gray, temp_gray, row, col, 10, 30, posValue);
	//bool negBool = getCRArea(negRoi_gray, temp_gray, row, col, 10, 30, negValue);
	if (posValue.size() == negValue.size()& negValue.size()>0)
	{
		vector<double>crVec;
		for (int i = 0; i < posValue.size(); i++)
		{
			if (posValue[i] > negValue[i])
				crVec.push_back(posValue[i] / negValue[i]);
			else
				crVec.push_back(negValue[i] / posValue[i]);
		}
		cr = accumulate(crVec.begin(), crVec.end(), 0.0) / crVec.size();
	}
	return cr;
}
bool algorithm::MLOpticalParametersSystem::getCRArea(cv::Mat img_gray, cv::Mat temp_gray, int row, int col,int size, int num)
{
	// TODO: turn the data to image
	//cv::Mat img_gray1 = img_gray.clone();
	double minv = 0.0, maxv = 0.0;
	double* minp = &minv;
	double* maxp = &maxv;
	cv::minMaxIdx(img_gray, minp, maxp);

	cv::Mat tmp;
	cv::Mat dst8 = cv::Mat::zeros(img_gray.size(), CV_8U);
	normalize(img_gray, tmp, 0, 255, NORM_MINMAX);
	convertScaleAbs(tmp, dst8);
	cv::Mat img_draw = dst8.clone();//img.clone();

	cv::Mat element1 = getStructuringElement(cv::MORPH_RECT, cv::Size(size, size));
	cv::morphologyEx(dst8, dst8, cv::MORPH_OPEN, element1);
	cv::morphologyEx(dst8, dst8, cv::MORPH_CLOSE, element1);
	cv::morphologyEx(dst8, dst8, cv::MORPH_GRADIENT, element1);
	vector<cv::Point> P;//存储所有检测目标的坐标
	algorithm::MLimagePublic my_public;
	cv::Point2f center_cross;
	if (img_gray.data != NULL & temp_gray.data != NULL)
	{
		my_public.GetAllMinLoc(dst8, temp_gray, 0.5, cv::Scalar(0, 0, 0), &P, num);
		center_cross = crossHairDetect(temp_gray);
	}
	if (P.size() > 0)
	{
		vector<double>x, y;
		for (int i = 0; i < P.size(); i++)
		{
			cv::Point loc = P[i];
			x.push_back(loc.x + center_cross.x);
			y.push_back(loc.y + center_cross.y);
		}


		//cv::Mat img_draw = img_gray.clone();
		for (int i = 0; i < x.size(); i++)
		{
			circle(img_draw, cv::Point2f(x[i], y[i]), 10, cv::Scalar(0, 0, 255), -1);
		}
		// jisuan 
		double sum_x = std::accumulate(std::begin(x), std::end(x), 0.0);
		double mean_x = sum_x / x.size(); //均值
		double sum_y = std::accumulate(std::begin(y), std::end(y), 0.0);
		double mean_y = sum_y / y.size(); //均值
		//cout << mean_x << " " << mean_y << endl;
		vector<double>dis;
		for (int i = 0; i < x.size(); i++)
		{
			double temp = sqrt(pow(x[i] - mean_x, 2) + pow(y[i] - mean_y, 2));
			dis.push_back(temp);
		}

		vector<double>dis_sort;
		vector<int> list;
		bool flag = sort_user(dis, dis_sort, list);
		cv::Point2f c0;
		if (list[0] < x.size() & list[0] < y.size())
		{
			 c0 = cv::Point2f(x[list[0]], y[list[0]]);
		}
		vector<double>dis1;
		for (int i = 0; i < x.size(); i++)
		{
			double temp = sqrt(pow(x[i] - c0.x, 2) + pow(y[i] - c0.y, 2));
			dis1.push_back(temp);
		}
		vector<int>list1;
		vector<double>dis1_sort;
		bool flag1 = sort_user(dis1, dis1_sort, list1);

		vector<double>x_use, y_use;
		for (int i = 1; i < 5; i++)
		{
			x_use.push_back(x[list1[i]]);
			y_use.push_back(y[list1[i]]);
		}
		double len = (dis1_sort[1] + dis1_sort[2] + dis1_sort[3] + dis1_sort[4]) / 4.0;
		//cout << len << endl;

		vector<int>list_x, list_y;
		vector<double>x_sort, y_sort;
		bool flag_x = sort_user(x_use, x_sort, list_x);
		bool flag_y = sort_user(y_use, y_sort, list_y);

		double kh = (c0.y - y_use[list_x[0]]) / (c0.x - x_use[list_x[0]]);
		double kv = (c0.y - y_use[list_y[0]]) / (c0.x - x_use[list_y[0]]);
		double anglev = atan(kv);
		double angleh = atan(kh);
		//cout << kh << " " << kv << endl;
		//cout << angleh << " " << anglev << endl;
		int flag_s = 1;
		if (kv < 0)
			flag_s = -1;
		else
			flag_s = 1;

		c0.x = (-col / 2) * len * cos(angleh) + (c0.x + flag_s * (-row / 2) * len * cos(anglev));
		c0.y = (-row / 2) * len * (sin(anglev)) * flag_s + (c0.y + (-col / 2) * len * sin(angleh));

		//c1(1) = fix((-col / 2)) * r * cos(angleh) + (c0(1) + flag * fix((-row / 2)) * r * cos(anglev));
		//c1(2) = fix((-row / 2)) * r * (sin(anglev)) * flag + (c0(2) - fix((-col / 2)) * r * sin(angleh));
		// 
		//vector<double>ob;
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				double  sx = (j - 1 + 1) * len * cos(angleh) + (c0.x + flag_s * (i - 1 + 1) * len * cos(anglev));
				double  sy = (i - 1 + 1) * len * abs(sin(anglev)) + (c0.y + (j - 1 + 1) * len * sin(angleh));

				//sx = (j - 1) * r * cos(angleh) + (c1(1) + flag * (i - 1) * r * cos(anglev));
			//	sy = (i - 1) * r * abs(sin(anglev)) + (c1(2) + (j - 1) * r * sin(angleh));


				cv::Point2f start, end;
				start.x = sx + len / 4.0;
				start.y = sy + len / 4.0;
				end.x = sx + len / 4.0 + len / 2.0;
				end.y = sy + len / 4.0 + len / 2.0;
				//cv::Mat roi = img_gray(cv::Rect(start, end));
				cv::rectangle(img_draw, start, end, cv::Scalar(0, 255, 0), 1);
				m_CRrect.push_back(cv::Rect(start, end));
				//cv::Mat  mean, stddev;
				//cv::meanStdDev(roi, mean, stddev);
			//	ob.push_back(mean.at<double>(0, 0));
			}
		}
		if (m_CRrect.size() > 0)
			return true;
		else
			return false;
	}
	else
	return false;
	}
cv::Point2f algorithm::MLOpticalParametersSystem::crossHairDetect(cv::Mat img)
{
	cv::Mat roi = img.clone();// img(Rect(loc.x, loc.y, temp.cols, temp.rows));
	cv::Mat RoiFiducial_img = roi.clone();
	cv::Mat img_draw = img.clone();
	cv::Point2d fiducial_loaction;
	cv::Mat img_gray;
	if (RoiFiducial_img.channels() == 3)//turn gray
		cvtColor(RoiFiducial_img, img_gray, cv::COLOR_BGR2GRAY);
	else
		img_gray = RoiFiducial_img.clone();
	// cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(10, 10));
	// cv::morphologyEx(img_gray, img_gray, cv::MORPH_OPEN, element);
	// cv::morphologyEx(img_gray, img_gray, MORPH_CLOSE, element);
	// cv::morphologyEx(img_gray, img_gray, cv::MORPH_GRADIENT, element);
	 //	cv::GaussianBlur(img_gray, img_gray, cv::Size(9, 9), 2, 2);
 //	cv::medianBlur(img_gray, img_gray, 3);
	 //cv::morphologyEx(img_gray, img_gray, cv::MORPH_CLOSE, element);

	double thresh_level = 0;
	//cv::Mat thresh_roi = img_gray(cv::Rect(0, 0, 100, 100));
	cv::Mat mat_mean, mat_stddev;
	cv::meanStdDev(img_gray, mat_mean, mat_stddev);
	thresh_level = mat_mean.at<double>(0, 0);

	cv::Mat img_threshold;
	cv::threshold(img_gray, img_threshold, thresh_level, 255, CV_THRESH_BINARY);
	//cv::threshold(img_gray, img_threshold, 0, 255, CV_THRESH_OTSU);
	//cv::Canny(img_threshold, img_threshold, 50, 150, 3);//canny边缘检测
	//ContoursRemoveNoise(img_threshold, 500);
	//NaiveRemoveNoise1(img_threshold, 1000);// 
	cv::Canny(img_threshold, img_threshold, 50, 150, 3);//canny边缘检测
	algorithm::MLimagePublic my_public;
	my_public.NaiveRemoveNoise(img_threshold, 500);// 
	cv::Mat m;
	int img_rows_ = img_gray.rows;
	int img_cols_ = img_gray.cols;
	img_threshold.convertTo(m, CV_64FC1);
	cv::Mat m_row(1, img_cols_, CV_64FC1, cv::Scalar(0));
	cv::Mat m_col(img_rows_, 1, CV_64FC1, cv::Scalar(0));
	cv::reduce(m, m_row, 0, cv::REDUCE_SUM);
	cv::reduce(m, m_col, 1, cv::REDUCE_SUM);

	int numR = countNonZero(m_row);
	int numC = countNonZero(m_col);
	int l = (numR < numC ? numR : numC) / 6;

	std::vector<cv::Vec4i> lines;
	cv::HoughLinesP(img_threshold, lines, 1, CV_PI / 180.0, l, l, l);
	//cv::HoughLinesP(img_threshold, lines, 1, CV_PI / 180.0, 20, 20, 20);

	for (int i = 0; i < lines.size(); i++)
	{
		line(img_draw, cv::Point(lines[i][0], lines[i][1]), cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0, 0, 255));
	}

	algorithm::MLCrossCenter my_cross;
	std::vector<cv::Vec4i> lines1, lines2;
	std::vector<float> ks1, ks2;
	float a1, b1, c1, a2, b2, c2;
	double line_width;
	cv::Point2f center_cross;
	if (!lines.empty())
	{
		my_cross.SeparateLines(lines, lines1, ks1, lines2, ks2);
		if (!lines1.empty() & !lines2.empty())
		{
			int width1, width2;
			width1 = my_cross.CalculateLine(lines1, ks1, a1, b1, c1);
			width2 = my_cross.CalculateLine(lines2, ks2, a2, b2, c2);
			line_width = std::max(width1, width2);
			center_cross = my_cross.CalculateLinesCross(a1, b1, c1, a2, b2, c2);
		}
	}

	//circle(RoiFiducial_img, center_cross, 2, cv::Scalar(155, 50, 255));

	return center_cross;
}

vector<cv::Rect> algorithm::MLOpticalParametersSystem::GetCRrect()
{
	return m_CRrect;
}

roiAreaData algorithm::MLOpticalParametersSLB::SetCRROIArea(cv::Mat imgP, cv::Mat imgN, string str, bool isSmall,cv::Rect rect1)
{
	roiAreaData re;
	if (imgP.data != NULL & imgN.data != NULL)
	{
		cv::Mat posROI_gray = GetROIMat(imgP, rect1);
		cv::Mat negROI_gray = GetROIMat(imgN, rect1);
		vector<cv::Rect>roiVecP, roiVecN;
		roiVecP = detectChessboard(posROI_gray, re.roiVecPC,isSmall);
		roiVecN = detectChessboard(negROI_gray, re.roiVecNC, isSmall);
		if (roiVecP.size() == roiVecN.size())
		{
			re.roiVecP = roiVecP;
			re.roiVecN = roiVecN;
		}
		else
		{
			ReoderROIVec(roiVecP,roiVecN,re.roiVecP,re.roiVecN);

		}
		if (roiVecP.size() > 0)
		{
			bool flagDebug = false;
			string color = str;
			string type;
			if (isSmall)
			{
				type = "small.txt";
			}
			else
			{
				type = "big.txt";
			}
			string strPub, strP, strN, strPC, strNC;
			if (flagDebug)
			{
				strPub = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\";
				strP = "P_allROI_";
				strN = "N_allROI_";
				strPC = "P_centerROI_";
				strNC = "N_centerROI_";
			}
			else
			{
				strPub = "./config_iqt/IQT7CR_ROI/";
				strP = "P_allROI__";
				strN = "N_allROI_";
				strPC = "P_centerROI_";
				strNC = "N_centerROI_";

			}

			string filepathP = strPub + color + strP + type;
			string filepathN = strPub + color + strN + type;
			writeROIFile(filepathP, re.roiVecP);
			writeROIFile(filepathN, re.roiVecN);
			string filepathPC = strPub + color + strPC + type;
			string filepathNC = strPub + color + strNC + type;
			writeROIFile(filepathPC, re.roiVecPC);
			writeROIFile(filepathNC, re.roiVecNC);
		}
		else
		{
			MLOpticalParametersDUT dut;
			roiAreaData re=dut.GetCRROIArea(str,isSmall);
		}
        /*
		if (isSmall)
		{
			if (str == "R")
			{
				string filepathP = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\RP_allROI_small.txt";
				string filepahtN = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\RN_allROI_small.txt";
				//string filepathP = "./config_iqt/IQT7CR_ROI/RP_allROI_small.txt";
				//string filepahtN = "./config_iqt/IQT7CR_ROI/RN_allROI_small.txt";
				writeROIFile(filepathP, re.roiVecP);
				writeROIFile(filepahtN, re.roiVecN);
				string filepathPC = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\RP_centerROI_small.txt";
				string filepahtNC = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\RN_centerROI_small.txt";
				//string filepathPC = "./config_iqt/IQT7CR_ROI/RP_centerROI_small.txt";
				//string filepahtNC = "./config_iqt/IQT7CR_ROI/RN_centerROI_small.txt";
				writeROIFile(filepathPC, re.roiVecPC);
				writeROIFile(filepahtNC, re.roiVecNC);
			}
			else if (str == "G")
			{
				string filepathP = "./config_iqt/IQT7CR_ROI/GP_allROI_small.txt";
				string filepahtN = "./config_iqt/IQT7CR_ROI/GN_allROI_small.txt";
				writeROIFile(filepathP, re.roiVecP);
				writeROIFile(filepahtN, re.roiVecN);
				string filepathPC = "./config_iqt/IQT7CR_ROI/GP_centerROI_small.txt";
				string filepahtNC = "./config_iqt/IQT7CR_ROI/GN_centerROI_small.txt";
				writeROIFile(filepathP, re.roiVecPC);
				writeROIFile(filepahtN, re.roiVecNC);
			}
			else if (str == "B")
			{
				string filepathP = "./config_iqt/IQT7CR_ROI/BP_allROI_small.txt";
				string filepahtN = "./config_iqt/IQT7CR_ROI/BN_allROI_small.txt";
				writeROIFile(filepathP, re.roiVecP);
				writeROIFile(filepahtN, re.roiVecN);
				string filepathPC = "./config_iqt/IQT7CR_ROI/BP_centerROI_small.txt";
				string filepahtNC = "./config_iqt/IQT7CR_ROI/BN_centerROI_small.txt";
				writeROIFile(filepathP, re.roiVecPC);
				writeROIFile(filepahtN, re.roiVecNC);

			}
		}
		else
		{
			if (str == "R")
			{
				string filepathP = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\RP_allROI_big.txt";
				string filepahtN = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\RN_allROI_big.txt";
				//string filepathP = "./config_iqt/IQT7CR_ROI/RP_allROI_big.txt";
				//string filepahtN = "./config_iqt/IQT7CR_ROI/RN_allROI_big.txt";
				writeROIFile(filepathP, re.roiVecP);
				writeROIFile(filepahtN, re.roiVecN);
				string filepathPC = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\RP_centerROI_big.txt";
				string filepahtNC = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\RN_centerROI_big.txt";
				//string filepathPC = "./config_iqt/IQT7CR_ROI/RP_centerROI_big.txt";
				//string filepahtNC = "./config_iqt/IQT7CR_ROI/RN_centerROI_big.txt";
				writeROIFile(filepathPC, re.roiVecPC);
				writeROIFile(filepahtNC, re.roiVecNC);
			}
			else if (str == "G")
			{
				string filepathP = "./config_iqt/IQT7CR_ROI/GP_allROI_big.txt";
				string filepahtN = "./config_iqt/IQT7CR_ROI/GN_allROI_small.txt";
				writeROIFile(filepathP, re.roiVecP);
				writeROIFile(filepahtN, re.roiVecN);
				string filepathPC = "./config_iqt/IQT7CR_ROI/GP_centerROI_big.txt";
				string filepahtNC = "./config_iqt/IQT7CR_ROI/GN_centerROI_big.txt";
				writeROIFile(filepathP, re.roiVecPC);
				writeROIFile(filepahtN, re.roiVecNC);
			}
			else if (str == "B")
			{
				string filepathP = "./config_iqt/IQT7CR_ROI/BP_allROI_big.txt";
				string filepahtN = "./config_iqt/IQT7CR_ROI/BN_allROI_big.txt";
				writeROIFile(filepathP, re.roiVecP);
				writeROIFile(filepahtN, re.roiVecN);
				string filepathPC = "./config_iqt/IQT7CR_ROI/BP_centerROI_big.txt";
				string filepahtNC = "./config_iqt/IQT7CR_ROI/BN_centerROI_big.txt";
				writeROIFile(filepathP, re.roiVecPC);
				writeROIFile(filepahtN, re.roiVecNC);

			}
		}
		*/
	}
	return re;

}
int findNearstList(vector<cv::Rect>roiVecN, cv::Rect temp)
{
	int list = -1;
	vector<double>dis;
	for (int i = 0; i < roiVecN.size(); i++)
	{
		double deltx = roiVecN[i].x - temp.x;
		double delty = roiVecN[i].y - temp.y;
		double tmpDis = sqrt(deltx*deltx+delty*delty);
		dis.push_back(tmpDis);
	}	
	double minValue = *min_element(dis.begin(), dis.end());
    int minPosition = min_element(dis.begin(), dis.end()) - dis.begin();
	if (minValue < 10)
	{
		list = minPosition;
	}

	return list;
}
void algorithm::MLOpticalParametersSLB::ReoderROIVec(vector<cv::Rect>roiVecP, vector<cv::Rect>roiVecN, vector<cv::Rect>& roiVecPNew, vector<cv::Rect>& roiVecNNew)
{
	if (roiVecN.size() != roiVecP.size())
	{
		int len = min(roiVecN.size(), roiVecP.size());
		for (int i = 0; i < len; i++)
		{
			cv::Rect temp = roiVecP[i];
			int list = findNearstList(roiVecN, temp);
			if (list >= 0&list<len)
			{
				roiVecPNew.push_back(roiVecP[list]);
				roiVecNNew.push_back(roiVecN[list]);
			}

		}

	}
	else
	{
		roiVecPNew = roiVecP;
		roiVecN = roiVecN;
	}
	
}


cv::Point2f algorithm::MLOpticalParametersSLB::CenterCoordinate(cv::Mat img)
{
	//cv::transpose(img, img);

	cv::Point2f center;
	if (img.data)
	{	
		cv::Mat img_gray;
		if (img.channels() == 3)
			cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
		else
			img_gray = img.clone();
		//cv::Mat temp_gray = cv::imread("./config_iqt/IQT Template/centerCoordinateTemplate.tif",0);
		////cv::Mat temp_gray = cv::imread("D:\\iqt7\\src\\app\\config_iqt\\IQT Template\\centerCoordinateTemplate.tif", 0);
	
		//cv::Mat tmp;
		//cv::Mat dst8 = cv::Mat::zeros(img_gray.size(), CV_8U);
		//normalize(img_gray, tmp, 0, 255, NORM_MINMAX);
		//convertScaleAbs(tmp, dst8);
		//cv::Mat img_draw = dst8.clone();//img.clone();

		cv::Mat img_draw = convertToUint8(img_gray);

		//vector<cv::Point> P;//存储所有检测目标的坐标
		//algorithm::MLimagePublic my_public;
		//my_public.GetAllMinLoc(img_draw, temp_gray, 0.5, cv::Scalar(0, 0, 0), &P, 1);
		//cv::Point loc = P[0];
		//cv::Mat roi = img_draw(cv::Rect(loc.x, loc.y, temp_gray.cols, temp_gray.rows));

		int row = img.rows;
		int col = img.cols;
		double len = row / 10.0;
		cv::Rect rect = cv::Rect(col / 2 - len / 2,row / 2.0 - len / 2.0,  len, len);
		cv::Mat roi = img_draw(rect);

		cv::Mat roi_th;
		cv::Mat roi_draw = roi.clone();
		cv::Mat element1 = getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
		cv::morphologyEx(roi, roi, cv::MORPH_OPEN, element1);
		threshold(roi, roi_th, 0, 255, CV_THRESH_OTSU);
		//寻找轮廓
		vector<vector<cv::Point>> contours;
		vector<cv::Vec4i> hierachy;
		findContours(roi_th, contours, hierachy, cv::RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(-1, -1));
		vector<vector<cv::Point>> contours_new;
		vector<cv::Vec4i> hierachy_new;
		for (int i = 0; i < contours.size(); i++)
		{
			if (/*hierachy[i][2] != -1 && hierachy[i][3] != -1 && */contourArea(contours[i], false) > 50)
			{
				contours_new.push_back(contours[i]);
				hierachy_new.push_back(hierachy[i]);
			}
		//	drawContours(roi_th, contours, i, Scalar(rand() % 255, rand() % 255, rand() % 255), 3, 8);
		}
		cv::Point2f pc;
		for (int i = 0; i < contours_new.size(); i++)
		{
			drawContours(roi_draw, contours_new, i, Scalar(rand() % 255, rand() % 255, rand() % 255), 1, 8);
			float r;
			cv::minEnclosingCircle(contours_new[i], pc, r);
			if (r > 4 & r < 10)
			{
				//drawContours(img, contours_new, i, Scalar(rand() % 255, rand() % 255, rand() % 255), 3, 8);
				cv::circle(roi_draw, pc, r, cv::Scalar(0, 0, 255), 1);
				cv::circle(roi_draw, pc, 2, cv::Scalar(0, 0, 255), 1);
				center = cv::Point2f(pc.x + rect.x, pc.y + rect.y);
				//cout << pc << r << endl;
			}
		}
		 cv::circle(img_draw, center, 2, cv::Scalar(0, 0, 255), 1);

	}
	return center ;
}

cv::Point2f algorithm::MLOpticalParametersSLB::FovAngle(cv::Mat img, double magnification, double pixel_size, double FocalLength, cv::Rect rect1)
{
	cv::Point2f fovAngle = { 0,0 };
	if (img.data)
	{
		cv::Mat img_gray = GetROIMat(img, rect1);
		int rows = img_gray.rows;
		int cols = img_gray.cols;
		double horizontalAngle = atan(cols / 2.0 * pixel_size / magnification / FocalLength) * 180 / CV_PI;
		double verticalAngle = horizontalAngle;
			//atan(rows / 2.0 * pixel_size / magnification / FocalLength) * 180 / CV_PI;

		fovAngle.x = verticalAngle;
		fovAngle.y = horizontalAngle;
	}
	return fovAngle;
}

double algorithm::MLOpticalParametersSLB::meanY(cv::Mat img, cv::Rect roiRect)
{

	double result = 0;
	
	if (img.data!=NULL)
	{
		cv::Mat img_gray;
		cv::Mat roi;
	/*	if (img.channels() == 3)
			cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
		else
			img_gray = img.clone();
		roi = img_gray(roiRect);*/
		roi = GetROIMat(img,roiRect);

		cv::Scalar mean;
		mean = cv::mean(roi);
		result = mean(0);
	}
	return result;
}

double algorithm::MLOpticalParametersSLB::Uniformity(cv::Mat img, cv::Rect roiRect)
{
	double result = 0;
	if (img.data != NULL)
	{
		cv::Mat img_gray;
		//if (roiRect.x != 0 & roiRect.y != 0)
		//{
		//	img = img(roiRect);
		//}
		//if (img.channels() == 3)//turn gray
		//	cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
		//else
		//	img_gray = img.clone();
		img_gray = GetROIMat(img,roiRect);
		cv::Mat img2 = img_gray.clone();

		int row = img_gray.rows;
		int col = img_gray.cols;
		cv::Mat imgReshape = img2.reshape(1, row * col);
		cv::Mat imgReshape_sort;
		cv::sort(imgReshape, imgReshape_sort, CV_SORT_EVERY_ROW + CV_SORT_ASCENDING);
		int n95List = int(0.95 * row * col);
		int n5List = int(0.05 * row * col);
		double n95 = 0;
		double n5 = 1;
		if (imgReshape_sort.type() == 5)
		{
			n95 = imgReshape_sort.at<float>(n95List);
			n5 = imgReshape_sort.at<float>(n5List);
		}
		else
		{
			n95 = imgReshape_sort.at<uchar>(n95List);
			n5 = imgReshape_sort.at<uchar>(n5List);
		}
		if (n95 != 0)
		{
			result = n95 / n5;
		}
	}
	return result;
}

double algorithm::MLOpticalParametersSLB::CenterContrastSLBBigChessboard(cv::Mat imgP, cv::Mat imgN, roiAreaData re, cv::Rect rect1)
{
	double crValue = 0;
	if (imgP.data != NULL & imgN.data != NULL)
	{
		cv::Mat posROI_gray = GetROIMat(imgP, rect1);
		cv::Mat negROI_gray = GetROIMat(imgN, rect1);
		vector<cv::Point2f>pointsP, pointsN;
		vector<cv::Rect>roiVecP, roiVecN;
		roiVecP = re.roiVecPC;
		roiVecN = re.roiVecNC;
		//string filepathP = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\P_centerROI_big.txt";
		//string filepahtN = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\N_centerROI_big.txt";
		//string filepathP = "./config_iqt/IQT7CR_ROI/P_centerROI_big.txt";
		//string filepahtN = "./config_iqt/IQT7CR_ROI/N_centerROI_big.txt";

		//readROItxtFile(filepathP, roiVecP);
		//readROItxtFile(filepahtN, roiVecN);
		//draw
		cv::Mat img_drawP = convertToUint8(posROI_gray);
		cv::Mat img_drawN = convertToUint8(negROI_gray);
	
		vector<double>posValue,negValue;
		posValue=CalculateROIValue(posROI_gray,roiVecP);
		negValue=CalculateROIValue(negROI_gray, roiVecN);
		vector<double>crVec= CalculateCR(posValue, negValue);
		// draw
			for (int i = 0; i < roiVecP.size(); i++)
			{
				//draw
				cv::rectangle(img_drawP, roiVecP[i], cv::Scalar(255, 0, 255), 1);
				cv::rectangle(img_drawN, roiVecN[i], cv::Scalar(255, 0, 255), 1);
			}
		
		if (crVec.size() > 0)
		{
			crValue = accumulate(crVec.begin(), crVec.end(), 0.0) / crVec.size();
		}
	}
	return crValue;
}

double algorithm::MLOpticalParametersSLB::HarmonicMeanContrastSLBBigChessboard(cv::Mat imgP, cv::Mat imgN, roiAreaData re, cv::Rect roiRect)
{
	double crHValue = 0;
	if (imgP.data != NULL & imgN.data != NULL)
	{
		cv::Mat posROI_gray = GetROIMat(imgP, roiRect);
		cv::Mat negROI_gray = GetROIMat(imgN, roiRect);

		vector<cv::Point2f>pointsP, pointsN;
		vector<cv::Rect>roiVecP, roiVecN;
		roiVecP = re.roiVecP;
		roiVecN = re.roiVecN;
		//string filepathP = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\P_allROI_big.txt"; 
		//string filepahtN = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\N_allROI_big.txt"; 
		//string filepathP = "./config_iqt/IQT7CR_ROI/P_allROI_big.txt";
		//string filepahtN ="./config_iqt/IQT7CR_ROI/N_allROI_big.txt";		
		//readROItxtFile(filepathP, roiVecP);
		//readROItxtFile(filepahtN, roiVecN);

		vector<double>posValue, negValue;
		posValue = CalculateROIValue(posROI_gray, roiVecP);
		negValue = CalculateROIValue(negROI_gray, roiVecN);
		vector<double>crInverseVec,crVec;
		 crVec = CalculateCR(posValue, negValue);
		 crInverseVec = CalculatInverse(crVec);

		 if (crInverseVec.size() > 0)
		 {
			 crHValue = crInverseVec.size() / accumulate(crInverseVec.begin(), crInverseVec.end(), 0.0);
		 }

		 // draw
		 cv::Mat img_drawP = convertToUint8(posROI_gray);
		 cv::Mat img_drawN = convertToUint8(negROI_gray);
		for (int i = 0; i < roiVecP.size(); i++)
		{
			cv::rectangle(img_drawP, roiVecP[i], cv::Scalar(255, 0, 255), 1);
			cv::rectangle(img_drawN, roiVecN[i], cv::Scalar(255, 0, 255), 1);
		}


	}
	return crHValue;
}
double algorithm::MLOpticalParametersSLB::ArithemeticMeanContrastSLBBigChessboard(cv::Mat imgP, cv::Mat imgN, roiAreaData re, cv::Rect roiRect)
{
	double crValue = 0;
	if (imgP.data != NULL & imgN.data != NULL)
	{
		cv::Mat posROI_gray = GetROIMat(imgP, roiRect);
		cv::Mat negROI_gray = GetROIMat(imgN, roiRect);
		vector<cv::Point2f>pointsP, pointsN;
		vector<cv::Rect>roiVecP, roiVecN;
		roiVecP = re.roiVecP;
		roiVecN = re.roiVecN;
		//string filepathP = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\P_allROI_big.txt";
		//string filepahtN = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\N_allROI_big.txt";
		//string filepathP = "./config_iqt/IQT7CR_ROI/P_allROI_big.txt";
		//string filepahtN = "./config_iqt/IQT7CR_ROI/N_allROI_big.txt";
		//readROItxtFile(filepathP, roiVecP);
		//readROItxtFile(filepahtN, roiVecN);

		vector<double>posValue, negValue;
		posValue = CalculateROIValue(posROI_gray, roiVecP);
		negValue = CalculateROIValue(negROI_gray, roiVecN);
		vector<double>crVec=CalculateCR(posValue,negValue);
		//draw
		cv::Mat img_drawP = convertToUint8(posROI_gray);
		cv::Mat img_drawN = convertToUint8(negROI_gray);
		for (int i = 0; i < roiVecP.size(); i++)
		{
			cv::rectangle(img_drawP, roiVecP[i], cv::Scalar(255, 0, 255), 1);
			cv::rectangle(img_drawN, roiVecN[i], cv::Scalar(255, 0, 255), 1);
		}
			if (crVec.size() > 0)
			{
				crValue = accumulate(crVec.begin(), crVec.end(), 0.0) / crVec.size();
			}
		
		return crValue;
	}
}
double algorithm::MLOpticalParametersSLB::CenterContrastSLBSmallChessboard(cv::Mat imgP, cv::Mat imgN, roiAreaData re, cv::Rect rect1)
{
	double crValue = 0;
	if ((imgP.data != NULL) & (imgN.data != NULL))
	{
		cv::Mat posROI_gray = GetROIMat(imgP, rect1);
		cv::Mat negROI_gray = GetROIMat(imgN, rect1);
		vector<cv::Point2f>pointsP,pointsN;
		vector<cv::Rect>roiVecP,roiVecN;
		roiVecP = re.roiVecPC;
		roiVecN = re.roiVecNC;

	/*	string filepathP = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\P_centerROI_small.txt";
		string filepahtN = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\N_centerROI_small.txt";*/
	/*	string filepathP = "./config_iqt/IQT7CR_ROI/P_centerROI_small.txt";
		string filepahtN = "./config_iqt/IQT7CR_ROI/N_centerROI_small.txt";*/
		//readROItxtFile(filepathP, roiVecP);
		//readROItxtFile(filepahtN, roiVecN);
		vector<double>posValue, negValue;
		posValue = CalculateROIValue(posROI_gray, roiVecP);
		negValue = CalculateROIValue(negROI_gray, roiVecN);
		vector<double>crVec=CalculateCR(posValue, negValue);

		//draw
		cv::Mat img_drawP = convertToUint8(posROI_gray);
		cv::Mat img_drawN = convertToUint8(negROI_gray);
		for (int i = 0; i < roiVecP.size(); i++)
		{
			cv::rectangle(img_drawP, roiVecP[i], cv::Scalar(255, 0, 255), 1);
			cv::rectangle(img_drawN, roiVecN[i], cv::Scalar(255, 0, 255), 1);
		}
		if (crVec.size() > 0)
		{
			crValue = accumulate(crVec.begin(), crVec.end(), 0.0) / crVec.size();
		}
	}
	return crValue;
}
double algorithm::MLOpticalParametersSLB::HarmonicMeanContrastSLBSmallChessboard(cv::Mat imgP, cv::Mat imgN, roiAreaData re, cv::Rect roiRect)
{
	double crHValue = 0;
	if (imgP.data != NULL & imgN.data != NULL)
	{
		cv::Mat posROI_gray = GetROIMat(imgP, roiRect);
		cv::Mat negROI_gray = GetROIMat(imgN, roiRect);	
		vector<cv::Point2f>pointsP, pointsN;
		vector<cv::Rect>roiVecP, roiVecN;
		roiVecP = re.roiVecP;
		roiVecN = re.roiVecN;
		//string filepathP = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\P_allROI_small.txt";
		//string filepahtN = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\N_allROI_small.txt";
		//string filepathP = "./config_iqt/IQT7CR_ROI/P_allROI_small.txt";
		//string filepahtN ="./config_iqt/IQT7CR_ROI/N_allROI_small.txt";
		//readROItxtFile(filepathP, roiVecP);
		//readROItxtFile(filepahtN, roiVecN);


		vector<double>posValue, negValue;
		posValue = CalculateROIValue(posROI_gray, roiVecP);
		negValue = CalculateROIValue(negROI_gray, roiVecN);
		vector<double>crVec, crInverseVec;
		crVec = CalculateCR(posValue, negValue);
		crInverseVec = CalculatInverse(crVec);
		//draw
		cv::Mat img_drawP = convertToUint8(posROI_gray);
		cv::Mat img_drawN = convertToUint8(negROI_gray);
		for (int i = 0; i < roiVecP.size(); i++)
		{
			cv::rectangle(img_drawP, roiVecP[i], cv::Scalar(255, 0, 255), 1);
			cv::rectangle(img_drawN, roiVecN[i], cv::Scalar(255, 0, 255), 1);
		}

		if (crInverseVec.size() > 0)
		{
			crHValue = crInverseVec.size() / accumulate(crInverseVec.begin(), crInverseVec.end(), 0.0);
		}
	}
	return crHValue;
}
double algorithm::MLOpticalParametersSLB::ArithemeticMeanContrastSLBSmallChessboard(cv::Mat imgP, cv::Mat imgN, roiAreaData re, cv::Rect roiRect)
{
	double crValue = 0;
	if (imgP.data != NULL & imgN.data != NULL)
	{
		cv::Mat posROI_gray = GetROIMat(imgP, roiRect);
		cv::Mat negROI_gray = GetROIMat(imgN, roiRect);
		vector<cv::Point2f>pointsP, pointsN;
		vector<cv::Rect>roiVecP, roiVecN;
		roiVecP = re.roiVecP;
		roiVecN = re.roiVecN;
		//string filepathP = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\P_allROI_small.txt";
		//string filepahtN = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\N_allROI_small.txt";
		//string filepathP = "./config_iqt/IQT7CR_ROI/P_allROI_small.txt";
		//string filepahtN = "./config_iqt/IQT7CR_ROI/N_allROI_small.txt";

		//readROItxtFile(filepathP, roiVecP);
		//readROItxtFile(filepahtN, roiVecN);

		vector<double>posValue, negValue;
		posValue = CalculateROIValue(posROI_gray, roiVecP);
		negValue = CalculateROIValue(negROI_gray, roiVecN);
		vector<double>crVec= CalculateCR(posValue, negValue);

		//draw
		cv::Mat img_drawP = convertToUint8(posROI_gray);
		cv::Mat img_drawN = convertToUint8(negROI_gray);
			for (int i = 0; i < roiVecP.size(); i++)
			{
				cv::rectangle(img_drawP, roiVecP[i], cv::Scalar(255, 0, 255), 1);
				cv::rectangle(img_drawN, roiVecN[i], cv::Scalar(255, 0, 255), 1);
			}
			if (crVec.size() > 0)
			{
				crValue = accumulate(crVec.begin(), crVec.end(), 0.0) / crVec.size();
			}
		
		m_smallChessboardACR = crValue;
	}
	return crValue;
}
bool algorithm::MLOpticalParametersSLB::findChessboardLibdetect(cv::Mat img, vector<cv::Point2f>& points)
{
	if (img.data!=NULL)
	{
		cv::Mat tmp;
		cv::Mat dst8 = cv::Mat::zeros(img.size(), CV_8U);
		normalize(img, tmp, 0, 255, NORM_MINMAX);
		convertScaleAbs(tmp, dst8);
		cv::Mat img_draw = dst8.clone();//img.clone();
		// detect image has been changed to the 8uint
		cv::Mat img_gray;
		if (img_draw.channels() == 3)
			cvtColor(img_draw, img_gray, cv::COLOR_BGR2GRAY);
		else
			img_gray = img_draw.clone();

		int errCode = 0;
		cbdetect::Corner corners;
		cbdetect::Params params;
		params.corner_type = cbdetect::SaddlePoint;
		params.show_debug_image = false;
		params.find_corners_reiszed = false;
		vector<double>x, y;

		//ofstream datafile;
		//datafile.open("jiaodian.txt",ofstream::app);
		//ofstream file("jiaodian.txt", ios::out);

		if (img_gray.data)
		{
			cbdetect::find_corners(img_gray, corners, params);
			for (int j = 0; j < corners.p.size(); j++)
			{
				cv::Point2d pt;
				pt.x = corners.p.at(j).x;// +rect.x;
				pt.y = corners.p.at(j).y;// +rect.y;
				points.push_back(pt);
				circle(img_draw, pt, 3, cv::Scalar(0, 0, 255), -1);
				//cout << pt << endl;
				//datafile << pt.x << ',' << pt.y << endl;
			}
			//datafile.close();
			if (corners.p.size() > 1)
				return true;
			else
				return false;
			//return true;
		}
		else
			return false;
	}
	else
		return false;
}
bool algorithm::MLOpticalParametersSLB::GetROIArea(vector<cv::Point2f> points, vector<cv::Rect>& roiVect)
{
	if (points.size() > 0)
	{
		vector<double>x, y;
		for (int i = 0; i < points.size(); i++)
		{
			x.push_back(points[i].x);
			y.push_back(points[i].y);
		}
		// jisuan 
		double sum_x = std::accumulate(std::begin(x), std::end(x), 0.0);
		double mean_x = sum_x / x.size(); //均值
		double sum_y = std::accumulate(std::begin(y), std::end(y), 0.0);
		double mean_y = sum_y / y.size(); //均值
		//cout << mean_x << " " << mean_y << endl;
		vector<double>dis;
		for (int i = 0; i < x.size(); i++)
		{
			double temp = sqrt(pow(x[i] - mean_x, 2) + pow(y[i] - mean_y, 2));
			dis.push_back(temp);
		}

		vector<double>dis_sort;
		vector<int> list;
		bool flag = sort_user(dis, dis_sort, list);

		cv::Point2f c0 = cv::Point2f(x[list[0]], y[list[0]]);
		vector<double>dis1;
		for (int i = 0; i < x.size(); i++)
		{
			double temp = sqrt(pow(x[i] - c0.x, 2) + pow(y[i] - c0.y, 2));
			dis1.push_back(temp);
		}
		vector<int>list1;
		vector<double>dis1_sort;
		bool flag1 = sort_user(dis1, dis1_sort, list1);

		vector<double>x_use, y_use;
		for (int i = 1; i < 5; i++)
		{
			x_use.push_back(x[list1[i]]);
			y_use.push_back(y[list1[i]]);
		}
		double len = (dis1_sort[1] + dis1_sort[2] + dis1_sort[3] + dis1_sort[4]) / 4.0;
		//	cout << len << endl;

		vector<int>list_x, list_y;
		vector<double>x_sort, y_sort;
		bool flag_x = sort_user(x_use, x_sort, list_x);
		bool flag_y = sort_user(y_use, y_sort, list_y);

		double kh = (c0.y - y_use[list_x[0]]) / (c0.x - x_use[list_x[0]]);
		double kv = (c0.y - y_use[list_y[0]]) / (c0.x - x_use[list_y[0]]);
		double anglev = atan(kv);
		double angleh = atan(kh);
		//cout << kh << " " << kv << endl;
		//cout << angleh << " " << anglev << endl;
		int flag_s = 1;
		if (kv < 0)
			flag_s = -1;
		else
			flag_s = 1;

		int row1 = 4;// = 2 / 2;
		int col1 = 4;// = 2 / 2;
		//circle(img_draw, c0, 5, Scalar(0, 255, 0), -1);


		/*ofstream datafile;
		datafile.open("rectangle.txt", ofstream::app);
		ofstream file("rectangle.txt", ios::out);*/
		c0.x = (-col1 / 2) * len * cos(angleh) + (c0.x + flag_s * (-row1 / 2) * len * cos(anglev));
		c0.y = (-row1 / 2) * len * (sin(anglev)) * flag_s + (c0.y + (-col1 / 2) * len * sin(angleh));
		vector<double>ob;
		for (int i = 0; i < row1; i++)
		{
			for (int j = 0; j < col1; j++)
			{
				double  sx = (j - 1 + 1) * len * cos(angleh) + (c0.x + flag_s * (i - 1 + 1) * len * cos(anglev));
				double  sy = (i - 1 + 1) * len * abs(sin(anglev)) + (c0.y + (j - 1 + 1) * len * sin(angleh));
				//sx = (j - 1) * r * cos(angleh) + (c1(1) + flag * (i - 1) * r * cos(anglev));
				//sy = (i - 1) * r * abs(sin(anglev)) + (c1(2) + (j - 1) * r * sin(angleh));

				cv::Point2f start, end;
				start.x = sx + len*6 / 20.0;//len / 4.0 * cos(angleh)+len/4.0 * cos(anglev)*flag_s;
				start.y = sy + len*6 / 20.0;////len / 4.0 * (sin(anglev)) * flag_s + len/4.0 * sin(angleh);
				end.x = sx + 0.75 * len;//len / 4.0 * cos(angleh) + len / 4.0 * cos(anglev) * flag_s;
				end.y = sy + 0.75 * len;// len * 0.75 * (sin(anglev)) * flag_s + len * 0.75 * sin(angleh);
				//datafile << start.x << ',' << start.y << endl;
				cv::Rect temp = cv::Rect(start.x, start.y, len / 2, len / 2);
				roiVect.push_back(temp);
				//cv::Mat roi = img_gray1(cv::Rect(start, end));
				//cv::rectangle(img_draw, start, end, cv::Scalar(0, 0, 255), 5);
				//cv::Mat  mean, stddev;
				//cv::meanStdDev(roi, mean, stddev);
				//ob.push_back(mean.at<double>(0, 0));
			}
		}
		//datafile.close();


		return true;
	}
	else
	return false;
}

vector<double> algorithm::MLOpticalParametersSLB::CalculateROIValue(cv::Mat img, vector<cv::Rect> roiVec)
{
	vector<double>crVec;
	if (img.data != NULL)
	{
		for (int i = 0; i < roiVec.size(); i++)
		{
			double cr = 1e-9;
			bool is_rect = (0 <= roiVec[i].x && 0 < roiVec[i].width && roiVec[i].x + roiVec[i].width < img.cols &&
				0 <= roiVec[i].y && 0 < roiVec[i].height && roiVec[i].y + roiVec[i].height < img.rows);
			if (is_rect)
			{
				cv::Mat posROI = img(roiVec[i]);
				if (posROI.data != NULL)
				{
					cv::Scalar mean = cv::mean(posROI);
					cr = mean(0);
				}
				else
				{
					cout << roiVec[i] << endl;
				}
				if (cr < 0) { cr = 1e-9; }
			}
			crVec.push_back(cr);
		}
	}
	return crVec;
}

vector<double> algorithm::MLOpticalParametersSLB::CalculateCR(vector<double> posValue, vector<double> negValue)
{
	vector<double>crVec;
	if (posValue.size() == negValue.size())
	{
		for (int i = 0; i < posValue.size(); i++)
		{
			if (posValue[i] >= negValue[i] & negValue[i] != 0& negValue[i]>0)
			{
				crVec.push_back(posValue[i] / negValue[i]);
			}
			else if (negValue[i] >= posValue[i] & posValue[i] != 0& posValue[i]>0)
			{
				crVec.push_back(negValue[i] / posValue[i]);
			}
			else
			{
				crVec.push_back(1e-9);
			}
		}
		
	}
	return crVec;
}

vector<double> algorithm::MLOpticalParametersSLB::CalculatInverse(vector<double> crVec)
{
	vector<double>crInverse;
	for (int i = 0; i < crVec.size(); i++)
	{
		if(crVec[i]!=0)
		{
			crInverse.push_back(1.0 / crVec[i]);
		}
		//else { crInverse.push_back(1e-9); }

	}
	return crInverse;
}

cv::Mat algorithm::MLOpticalParametersSLB::GetROIMat(cv::Mat img, Rect rec)
{
	cv::Mat img_gray;
	cv::Mat roi;
	if (img.data != NULL)
	{
		if (img.channels() == 3)
		{
			cvtColor(img, img_gray, CV_BGR2GRAY);
		}
		else
			img_gray = img.clone();
		if (rec.x == 0 | rec.y == 0)
		{
			roi = img_gray.clone();
		}
		else {
			roi = img_gray(rec);
		}
	}
	return roi;		
}

vector<double> algorithm::MLOpticalParametersSLB::GetCenterCRSmall()
{
	return m_centerCRVec_small;
}
vector<double> algorithm::MLOpticalParametersSLB::GetCenterCRBig()
{
	return m_centerCRVec_big;
}
vector<cv::Rect> algorithm::MLOpticalParametersSLB::GetCenterBigChessboardArea()
{
	return m_centerBigChessboard;
}
vector<double> algorithm::MLOpticalParametersSLB::GetAllCRSmall()
{
	return m_allCRVec_small;
}
vector<double> algorithm::MLOpticalParametersSLB::GetAllCRbig()
{
	return m_allCRVec_big;
}

int findNegtiveNum(vector<int>list)
{
	int len = 0;
	for (int i = 0; i < list.size(); i++)
	{
		if (list[i] < 0) { len = len + 1; }
	}
	return len;
}
int findMapNegNum(cv::Mat list)
{
	int len = 0;
	for (int i = 0; i < list.rows; i++)
	{
		for (int j = 0; j < list.cols; j++)
		{
			int temp = list.at<short>(i, j);
			if (temp < 0)
			{
				len = len + 1;
			}
		}
	}
	return len;
}
bool searchColIndex(cv::Mat indexMap, int& start, int& end)
{
	bool flag = true;
	int row = indexMap.rows;
	int col = indexMap.cols;
	start = 0;
	end = 0;
	int i = 0;
	for (i = 0; i < col / 2; i++)
	{
		cv::Mat temp = indexMap.colRange(i, i + 1);
		int len = 0;
		len = findMapNegNum(temp);
		if (len > 10/*col * 0.03*/)
		{
			continue;
		}
		else
		{
			break;
		}

	}
	start = i;
	for (i = col; i > col / 2; i--)
	{
		cv::Mat temp = indexMap.colRange(i - 1, i);
		int len = findMapNegNum(temp);
		if (len > 10/*col * 0.03*/)
		{
			continue;
		}
		else
		{
			break;
		}

	}
	end = i;
	return flag;
}
bool searchRowIndex(cv::Mat indexMap, int& start, int& end)
{
	bool flag = true;
	int row = indexMap.rows;
	int col = indexMap.cols;
	start = 0;
	end = 0;
	int i = 0;
	for (i = 0; i < row / 2; i++)
	{
		cv::Mat temp = indexMap.rowRange(i, i + 1);
		int len = 0;
		len = findMapNegNum(temp);
		if (len > 10/*row * 0.03*/)
		{
			continue;
		}
		else
		{
			break;
		}

	}
	start = i;
	for (i = row; i > row / 2; i--)
	{
		cv::Mat temp = indexMap.rowRange(i - 1, i);
		int len = findMapNegNum(temp);
		if (len > 10/*row * 0.03*/)
		{
			continue;
		}
		else
		{
			break;
		}

	}
	end = i;
	return flag;

}
cv::Mat updateIndexMap(cv::Mat indexMap, vector<cv::Point2d>cornersPoint, vector<cv::Point2d>& cornersPointNew)
{
	int row = indexMap.rows;
	int col = indexMap.cols;
	cornersPointNew = cornersPoint;
	vector<cv::Point>negPos;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			int temp = indexMap.at<short>(i, j);
			if (temp < 0)
			{
				negPos.push_back(cv::Point(i, j));
			}
		}
	}
	vector<cv::Point> failUpdatePos = negPos;
	while (failUpdatePos.size() > 0)
	{
		negPos.clear();
		negPos = failUpdatePos;
		failUpdatePos.clear();
		for (int i = 0; i < negPos.size(); i++)
		{
			int x = negPos[i].x;
			int y = negPos[i].y;
			int left = -1, right = -1, top = -1, bottom = -1;
			if (x - 1 >= 0) { left = indexMap.at<short>(x - 1, y); }
			if (x + 1 < row) { right = indexMap.at<short>(x + 1, y); }
			if (y - 1 >= 0) { top = indexMap.at<short>(x, y - 1); }
			if (y + 1 < col) { bottom = indexMap.at<short>(x, y + 1); }
			int flag = 0;
		//	cout << x << ',' << y << endl;
		//	cout << left << ',' << right << ',' << top << ',' << bottom << endl;
			int size = cornersPointNew.size();
			if (left >= 0 & left < size & right >= 0 & right < size)
			{
				flag = 1;
			}
			else if (top >= 0 & top < size & bottom >= 0 & bottom < size) { flag = 2; }
			else if (top >= 0 & top < size & right >= 0 & right < size) { flag = 3; }
			else if (top >= 0 & top < size & left >= 0 & left < size) { flag = 4; }
			else if (bottom >= 0 & bottom < size & right >= 0 & right < size) { flag = 5; }
			else if (bottom >= 0 & bottom < size & left >= 0 & left < size) { flag = 6; }
			else { failUpdatePos.push_back(negPos[i]); }
			//cout << flag << endl;
			switch (flag)
			{
			case 1: {
				cv::Point2d pt;
				pt.x = (cornersPointNew[left].x + cornersPointNew[right].x) / 2.0;
				pt.y = (cornersPointNew[left].y + cornersPointNew[right].y) / 2.0;
				cornersPointNew.push_back(pt);
				indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
				break; }
			case 2: {
				cv::Point2d pt;
				pt.x = (cornersPointNew[top].x + cornersPointNew[bottom].x) / 2.0;
				pt.y = (cornersPointNew[top].y + cornersPointNew[bottom].y) / 2.0;
				cornersPointNew.push_back(pt);
				indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
				break; }
			case 3: {
				cv::Point2d pt;
				pt.x = cornersPointNew[top].x;
				pt.y = cornersPointNew[right].y;
				cornersPointNew.push_back(pt);
				indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
				break;
			}
			case 4: {
				cv::Point2d pt;
				pt.x = cornersPointNew[top].x;
				pt.y = cornersPointNew[left].y;
				cornersPointNew.push_back(pt);
				indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
				break; }
			case 5: {
				cv::Point2d pt;
				pt.x = cornersPointNew[bottom].x;
				pt.y = cornersPointNew[right].y;
				cornersPointNew.push_back(pt);
				indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
				break; }
			case 6: {
				cv::Point2d pt;
				pt.x = cornersPointNew[bottom].x;
				pt.y = cornersPointNew[left].y;
				cornersPointNew.push_back(pt);
				indexMap.at<short>(x, y) = cornersPointNew.size() - 1;
				break; }
			}
		}
	}

	return indexMap;
}
bool getXYcoordinate(cv::Mat indexMap, vector<cv::Point2d>cornersPoint, cv::Mat& x, cv::Mat& y)
{
	bool flag = true;
	int row = indexMap.rows;
	int col = indexMap.cols;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			int list = indexMap.at<short>(i, j);
			if (list > cornersPoint.size()) {
				cout << list << endl;
			}
			else
			{
				x.at<float>(i, j) = cornersPoint[list].x;
				y.at<float>(i, j) = cornersPoint[list].y;
			}
		}
	}
	return flag;
}
bool getCenter(cv::Mat ip, cv::Mat& p, cv::Mat& px1, cv::Mat& px2, cv::Mat& px3, cv::Mat& px4)
{
	bool flag = true;
	int row = ip.rows;
	int col = ip.cols;
	px1 = ip(cv::Range(0, row - 1), cv::Range(0, col - 1));
	px2 = ip(cv::Range(1, row), cv::Range(0, col - 1));
	px3 = ip(cv::Range(0, row - 1), cv::Range(1, col));
	px4 = ip(cv::Range(1, row), cv::Range(1, col));
	cv::Mat temp1, temp2;
	cv::add(px1, px2, temp1);
	cv::add(px3, px4, temp2);
	cv::add(temp1, temp2, p);
	p = p / 4.0;

	//Px1 = iP(1:end - 1, 1 : end - 1);
	//Px2 = iP(2:end, 1 : end - 1);
	//Px3 = iP(1:end - 1, 2 : end);
	//Px4 = iP(2:end, 2 : end);
	//p = (Px1 + Px2 + Px3 + Px4) / 4;
	return flag;
}
cv::Mat getArea(cv::Mat x1, cv::Mat x2, cv::Mat x3, cv::Mat x4, cv::Mat y1, cv::Mat y2, cv::Mat y3, cv::Mat y4)
{
	/*   s1 = 1 / 2 * sqrt(((x2 - x1). ^ 2 + (y2 - y1). ^ 2).*((x3 - x1). ^ 2 + (y3 - y1). ^ 2) - ((x2 - x1).*(x3 - x1) + (y2 - y1).*(y3 - y1)). ^ 2);
	   s2 = 1 / 2 * sqrt(((x2 - x4). ^ 2 + (y2 - y4). ^ 2).*((x3 - x4). ^ 2 + (y3 - y4). ^ 2) - ((x2 - x4).*(x3 - x4) + (y2 - y4).*(y3 - y4)). ^ 2);
	   s = s1 + s2;*/
	cv::Mat s1, s2, s;
	cv::Mat temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9, temp10;
	cv::subtract(x2, x1, temp1);
	cv::subtract(y2, y1, temp2);
	cv::subtract(x3, x1, temp3);
	cv::subtract(y3, y1, temp4);
	cv::multiply(temp1, temp1, temp5);
	cv::multiply(temp2, temp2, temp6);
	cv::multiply(temp3, temp3, temp7);
	cv::multiply(temp4, temp4, temp8);
	cv::multiply(temp1, temp3, temp9);
	cv::multiply(temp2, temp4, temp10);

	cv::add(temp5, temp6, temp6);
	cv::add(temp7, temp8, temp7);
	cv::multiply(temp6, temp7, temp6);

	cv::add(temp9, temp10, temp9);
	cv::multiply(temp9, temp9, temp9);

	cv::subtract(temp6, temp9, temp6);

	cv::sqrt(temp6, s1);
	s1 = s1 * 0.5;

	//  s2 = 1 / 2 * sqrt(((x2 - x4). ^ 2 + (y2 - y4). ^ 2).*((x3 - x4). ^ 2 + (y3 - y4). ^ 2) 
	  //- ((x2 - x4).*(x3 - x4) + (y2 - y4).*(y3 - y4)). ^ 2);

	cv::subtract(x2, x4, temp1);
	cv::subtract(y2, y4, temp2);
	cv::subtract(x3, x4, temp3);
	cv::subtract(y3, y4, temp4);
	cv::multiply(temp1, temp1, temp5);
	cv::multiply(temp2, temp2, temp6);
	cv::multiply(temp3, temp3, temp7);
	cv::multiply(temp4, temp4, temp8);
	cv::multiply(temp1, temp3, temp9);
	cv::multiply(temp2, temp4, temp10);
	cv::add(temp5, temp6, temp5);
	cv::add(temp7, temp8, temp7);
	cv::multiply(temp5, temp7, temp5);

	cv::add(temp9, temp10, temp9);
	cv::multiply(temp9, temp9, temp9);
	cv::subtract(temp5, temp9, temp5);
	cv::sqrt(temp5, s2);
	s2 = s2 * 0.5;
	cv::add(s1, s2, s);
	return s;
}


vector<cv::Rect> algorithm::MLOpticalParametersSLB::detectChessboard(cv::Mat img, vector<cv::Rect>&posC, bool isSmall)
{
	
		cv::Mat img_gray;
		if (img.channels() == 3)
			cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
		else
			img_gray = img.clone();
		cv::Mat img_draw = img.clone();
		cv::Mat roi = img_gray;// (cv::Rect(2900, 1700, 800, 800));
		std::vector<cv::Point2d> result;
		int errCode = 0;
		cbdetect::Corner corners;
		cbdetect::Params params;
		params.corner_type = cbdetect::SaddlePoint;
		params.show_debug_image = false;
		params.find_corners_reiszed = false;
		vector<cbdetect::Board> chessboard;
		cv::GaussianBlur(roi, roi, cv::Size(3, 3), 0, 0);
		//equalizeHist(roi, roi);
		Mat srcbinary;
		threshold(roi, srcbinary, 0, 255, THRESH_OTSU);
		//adaptiveThreshold(roi, srcbinary, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 17, 0);
		Mat kernel = getStructuringElement(MORPH_RECT, Size(10, 10), Point(-1, -1));
		//morphologyEx(srcbinary, srcbinary, MORPH_OPEN, kernel, Point(-1, -1));
		//morphologyEx(srcbinary, srcbinary, MORPH_CLOSE, kernel, Point(-1, -1));
		cv::Mat roiimg = srcbinary;
		vector<double>x, y;
		std::vector<cv::Point2d> cornersPoint;
		vector<cv::Rect>pos;
		if (roiimg.data)
		{
			cbdetect::find_corners(roiimg, corners, params);
			// cbdetect::plot_corners(img_draw, corners);
			cbdetect::boards_from_corners(roiimg, corners, chessboard, params);
			//   cbdetect::plot_boards(img_draw, corners, chessboard, params);
			cornersPoint = corners.p;

			for (int j = 0; j < corners.p.size(); j++)
			{
				cv::Point2d pt;
				pt.x = corners.p.at(j).x;// +rect.x;
				pt.y = corners.p.at(j).y;// +rect.y;
		/*		result.push_back(pt);
				cornersPoint.push_back(pt);
				x.push_back(pt.x);
				y.push_back(pt.y);*/
				circle(img_draw, pt, 2, cv::Scalar(0, 0, 255), -1);
				// cout << pt << endl;
			}
			if (corners.p.size() > 0 & chessboard.size() > 0)
			{
				cbdetect::Board board;
				board = chessboard[0];
				vector<std::vector<int>>index = board.idx;
				int len = index.size();
				int len1 = index[0].size();
				cv::Mat indexMap = cv::Mat::zeros(len, len1, CV_16S);
				// change the vector to the mat
				for (int i = 0; i < len; i++)
				{
					for (int j = 0; j < len1; j++)
					{
						int a = index[i][j];
						indexMap.at<short>(i, j) = a;
					}
					//cout << endl;
				}
				if (isSmall)
				{
					indexMap = indexMap(cv::Range(2, indexMap.rows - 2), cv::Range(2, indexMap.cols - 2));
				}
				else
				{
					indexMap = indexMap(cv::Range(1, indexMap.rows - 1), cv::Range(1, indexMap.cols - 1));
				}

				int left = 0, right = 0;
				int top = 0, bottom = 0;
				//searchColIndex(indexMap, left, right);
				//indexMap = indexMap(cv::Range(0, indexMap.rows), cv::Range(left, right));
				//searchRowIndex(indexMap, top, bottom);

				vector<cv::Point2d>cornersPointNew;
				cv::Mat indexMapUpdate;
				indexMapUpdate = updateIndexMap(indexMap, cornersPoint, cornersPointNew);

				searchColIndex(indexMapUpdate, left, right);
				indexMapUpdate = indexMapUpdate(cv::Range(0, indexMap.rows), cv::Range(left, right));
				searchRowIndex(indexMapUpdate, top, bottom);
				cv::Mat indexMapNew = indexMap(Range(top, bottom), cv::Range(0, indexMap.cols));
				// cv::Mat indexMapUpdate;
			   //  vector<cv::Point2d>cornersPointNew;
				indexMapUpdate = updateIndexMap(indexMapNew, cornersPointNew, cornersPointNew);
				int row = indexMapUpdate.rows;
				int col = indexMapUpdate.cols;
				cv::Mat xPoints = cv::Mat::zeros(row, col, CV_32F);
				cv::Mat yPoints = cv::Mat::zeros(row, col, CV_32F);
				getXYcoordinate(indexMapUpdate, cornersPointNew, xPoints, yPoints);
				//draw  test the function result
				for (int i = 0; i < xPoints.rows; i++)
				{
					for (int j = 0; j < xPoints.cols; j++)
					{
						float x = xPoints.at<float>(i, j);
						float y = yPoints.at<float>(i, j);
						cv::circle(img_draw, Point2f(x, y), 2, cv::Scalar(0, 255, 0), -1);
					}
				}
				cv::Mat xcenter, ycenter;
				cv::Mat px1, px2, px3, px4, py1, py2, py3, py4;
				getCenter(xPoints, xcenter, px1, px2, px3, px4);
				getCenter(yPoints, ycenter, py1, py2, py3, py4);
				for (int i = 0; i < xcenter.rows; i++)
				{
					for (int j = 0; j < xcenter.cols; j++)
					{
						float x = xcenter.at<float>(i, j);
						float y = ycenter.at<float>(i, j);
						cv::circle(img_draw, Point2f(x, y), 1, cv::Scalar(0, 255, 0), -1);
					}
				}
				cv::Mat boardArea = getArea(px1, px2, px3, px4, py1, py2, py3, py4);
				cv::Mat boardRadius;
				double percent = 0.5;
				cv::sqrt(boardArea * percent / M_PI, boardRadius);

				for (int i = 0; i < xcenter.rows; i++)
				{
					for (int j = 0; j < xcenter.cols; j++)
					{
						float x = xcenter.at<float>(i, j) - boardRadius.at<float>(i, j) / 2;
						float y = ycenter.at<float>(i, j) - boardRadius.at<float>(i, j) / 2;
						cv::Point2f start = cv::Point2f(x, y);
						cv::Point2f end = cv::Point2f(x + boardRadius.at<float>(i, j), y + boardRadius.at<float>(i, j));
						//cv::Rect temp = cv::Rect(x, y, boardRadius.at<float>(i, j), boardRadius.at<float>(i, j));
						cv::Rect temp = cv::Rect(start, end);
						cv::rectangle(img_draw, temp, cv::Scalar(0, 0, 255), 1);
						pos.push_back(temp);
					}
				}

				int centerListx = int(xcenter.rows / 2.0);
				int centerListy = int(xcenter.cols / 2.0);
				top = max(centerListx - 1, 0);
				bottom = min(centerListx + 1, xcenter.rows);
				left = max(centerListy - 1, 0);
				right = min(centerListy + 1, xcenter.cols);
				for (int i = top; i < bottom; i++)
				{
					for (int j = left; j < right; j++)
					{
						float x = xcenter.at<float>(i, j) - boardRadius.at<float>(i, j) / 2;
						float y = ycenter.at<float>(i, j) - boardRadius.at<float>(i, j) / 2;
						cv::Point2f start = cv::Point2f(x, y);
						cv::Point2f end = cv::Point2f(x + boardRadius.at<float>(i, j), y + boardRadius.at<float>(i, j));
						//cv::Rect temp = cv::Rect(x, y, boardRadius.at<float>(i, j), boardRadius.at<float>(i, j));
						cv::Rect temp = cv::Rect(start, end);
						cv::rectangle(img_draw, temp, cv::Scalar(255, 0, 0), 1);
						posC.push_back(temp);
					}
				}
			}
		}

	return pos;
}
vector<cv::Rect> algorithm::MLOpticalParametersSLB::GetCenterSmallChessboardArea()
{
	return m_centerSmallChessboard;
}
vector<cv::Rect> algorithm::MLOpticalParametersSLB::GetBigChessboardArea()
{
	return m_bigChessboard;
}
vector<cv::Rect> algorithm::MLOpticalParametersSLB::GetSmallChessboardArea()
{
	return m_smallChessboard;
}
double algorithm::MLOpticalParametersSLB::GetCenterBigChessboardCR()
{
	return m_centerBigChessboardCR;
}
double algorithm::MLOpticalParametersSLB::GetCenterSmallChessboardCR()
{
	return m_centerSmallChessboardCR;
}
double algorithm::MLOpticalParametersSLB::GetBigChessboardACR()
{
	return m_bigChessboardACR;
}
double algorithm::MLOpticalParametersSLB::GetSmallChessboardACR()
{
	return m_smallChessboardACR;
}
double algorithm::MLOpticalParametersSLB::GetBigChessboardHCR()
{
	return m_bigChessboardHCR;
}
double algorithm::MLOpticalParametersSLB::GetSmallChessboardHCR()
{
	return m_smallChessboardHCR;
}
roiAreaData  algorithm::MLOpticalParametersDUT::GetCRROIArea(string str, bool isSmallChecker)
{
	bool flagDebug = false;
	roiAreaData re;
	string color = str;
	string type;
	if(isSmallChecker)
	{
		type = "small.txt";
	}
	else
	{
		type = "big.txt";
	}
	string strPub, strP, strN, strPC, strNC;
	if (flagDebug)
	{
		 strPub = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\";
		 strP = "P_allROI_";
		 strN = "N_allROI_";
		 strPC = "P_centerROI_";
		 strNC = "N_centerROI_";
	}
	else
	{
		 strPub = "./config_iqt/IQT7CR_ROI/";
		 strP = "P_allROI__";
		 strN = "N_allROI_";
		 strPC = "P_centerROI_";
		 strNC = "N_centerROI_";

	}

	string filepathP = strPub + color + strP + type;
	string filepathN = strPub + color + strN + type;
	readROItxtFile(filepathP, re.roiVecP);
	readROItxtFile(filepathN, re.roiVecN);
	string filepathPC = strPub + color + strPC + type;
	string filepathNC = strPub + color + strNC + type;
	readROItxtFile(filepathPC, re.roiVecPC);
	readROItxtFile(filepathNC, re.roiVecNC);

	/*
	if (isSmallChecker)
	{
		if (str == "R")
		{

			string filepathP = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\RP_allROI_small.txt";
			string filepahtN = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\RN_allROI_small.txt";
			//string filepathP = "./config_iqt/IQT7CR_ROI/RP_allROI_small.txt";
			//string filepahtN = "./config_iqt/IQT7CR_ROI/RN_allROI_small.txt";
			readROItxtFile(filepathP,re.roiVecP);
			readROItxtFile(filepahtN, re.roiVecN);
			string filepathPC = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\RP_centerROI_small.txt";
			string filepahtNC = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\RN_centerROI_small.txt";
			//string filepathPC = "./config_iqt/IQT7CR_ROI/RP_centerROI_small.txt";
			//string filepahtNC = "./config_iqt/IQT7CR_ROI/RN_centerROI_small.txt";
			readROItxtFile(filepathPC, re.roiVecPC);
			readROItxtFile(filepahtNC, re.roiVecNC);
		}
		else if (str == "G")
		{
			string filepathP = "./config_iqt/IQT7CR_ROI/GP_allROI_small.txt";
			string filepahtN = "./config_iqt/IQT7CR_ROI/GN_allROI_small.txt";
			readROItxtFile(filepathP, re.roiVecP);
			readROItxtFile(filepahtN, re.roiVecN);
			string filepathPC = "./config_iqt/IQT7CR_ROI/GP_centerROI_small.txt";
			string filepahtNC = "./config_iqt/IQT7CR_ROI/GN_centerROI_small.txt";
			readROItxtFile(filepathP, re.roiVecPC);
			readROItxtFile(filepahtN, re.roiVecNC);
		}
		else if (str == "B")
		{
			string filepathP = "./config_iqt/IQT7CR_ROI/BP_allROI_small.txt";
			string filepahtN = "./config_iqt/IQT7CR_ROI/BN_allROI_small.txt";
			readROItxtFile(filepathP, re.roiVecP);
			readROItxtFile(filepahtN, re.roiVecN);
			string filepathPC = "./config_iqt/IQT7CR_ROI/BP_centerROI_small.txt";
			string filepahtNC = "./config_iqt/IQT7CR_ROI/BN_centerROI_small.txt";
			readROItxtFile(filepathP, re.roiVecPC);
			readROItxtFile(filepahtN, re.roiVecNC);
		}
	
	}
	else
	{
		if (str == "R")
		{
			string filepathP = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\RP_allROI_big.txt";
			string filepahtN = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\RN_allROI_big.txt";
			//string filepathP = "./config_iqt/IQT7CR_ROI/RP_allROI_big.txt";
			//string filepahtN = "./config_iqt/IQT7CR_ROI/RN_allROI_big.txt";
			readROItxtFile(filepathP, re.roiVecP);
			readROItxtFile(filepahtN, re.roiVecN);

			string filepathPC = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\RP_centerROI_big.txt";
			string filepahtNC = "D:\\iqt7\\src\\app\\config_iqt\\IQT7CR_ROI\\RN_centerROI_big.txt";
			//string filepathPC = "./config_iqt/IQT7CR_ROI/RP_centerROI_big.txt";
			//string filepahtNC = "./config_iqt/IQT7CR_ROI/RN_centerROI_big.txt";
			readROItxtFile(filepathPC, re.roiVecPC);
			readROItxtFile(filepahtNC, re.roiVecNC);
		}
		else if (str == "G")
		{
			string filepathP = "./config_iqt/IQT7CR_ROI/GP_allROI_big.txt";
			string filepahtN = "./config_iqt/IQT7CR_ROI/GN_allROI_big.txt";
			readROItxtFile(filepathP, re.roiVecP);
			readROItxtFile(filepahtN, re.roiVecN);
			string filepathPC = "./config_iqt/IQT7CR_ROI/GP_centerROI_big.txt";
			string filepahtNC = "./config_iqt/IQT7CR_ROI/GN_centerROI_big.txt";
			readROItxtFile(filepathP, re.roiVecPC);
			readROItxtFile(filepahtN, re.roiVecNC);
		}
		else if (str == "B")
		{
			string filepathP = "./config_iqt/IQT7CR_ROI/BP_allROI_big.txt";
			string filepahtN = "./config_iqt/IQT7CR_ROI/BN_allROI_big.txt";
			readROItxtFile(filepathP, re.roiVecP);
			readROItxtFile(filepahtN, re.roiVecN);
			string filepathPC = "./config_iqt/IQT7CR_ROI/BP_centerROI_big.txt";
			string filepahtNC = "./config_iqt/IQT7CR_ROI/BN_centerROI_big.txt";
			readROItxtFile(filepathP, re.roiVecPC);
			readROItxtFile(filepahtN, re.roiVecNC);
		}
		
		
	}
	*/

	
	return re;
}


void algorithm::MLOpticalParametersDUT::SetImageClear(cv::Mat img_clear)
{
	m_img_clear = img_clear;
}



void algorithm::MLOpticalParametersDUT::SetImageChessboard(cv::Mat imgP, cv::Mat imgN, cv::Rect roiRect)
{
	m_imgP = imgP;
	m_imgN = imgN;
	m_roiRect = roiRect;
}
double algorithm::MLOpticalParametersDUT::GetEyeboxThroughput()
{
	double res = 0;
	res = EyeboxThroughput(m_PupilEefficencyVec, m_Seyebox, m_leyepupil);
	return res;
}
double algorithm::MLOpticalParametersDUT::GetAngleUniformity()
{
	double res = 0;
	res = AngleUniformity(m_img_clear,m_roiRect);
	return res;
}

double algorithm::MLOpticalParametersDUT::GetSpacialUniformity()
{
	double res = 0;
	 res= SpatialUniformity(m_PupilEefficencyVec);
	return res;
}

double algorithm::MLOpticalParametersDUT::CalculateInvaidCRMean(vector<double>crVec)
{
	double crMean = 0;
	// calculate the valid data
	double sum = 0;
	int n = 0;
	for (int i = 0; i < crVec.size(); i++)
	{
		if (crVec[i] > 1e-9 & crVec[i] < 200)
		{
			sum = sum + crVec[i];
			n = n + 1;
		}
	}
	crMean = sum / n;

	return crMean;
}

vector<double> algorithm::MLOpticalParametersDUT::CalculateCRCalibration(vector<double> crVec,vector<double>crSlb, vector<double> posValueDut, vector<double> negValueDut, double maxValue)
{
	vector<double>crVecCali;
	double sum = 0, n = 0;
	if (crVec.size() == posValueDut.size() & posValueDut.size() == negValueDut.size())
	{
		for (int i = 0; i < crVec.size(); i++)
		{
			double max = std::max(posValueDut[i], negValueDut[i]);
			if (max > maxValue * 0.1 & crVec[i] > 1e-9 & crVec[i] < 200)
			{
				sum = sum + crVec[i];
				n = n + 1;
			}
		}

		double crMean = 0;
		if (n > 0)
		{
			crMean=sum / n;
		}
		for (int i = 0; i < crVec.size(); i++)
		{
			double max = std::max(posValueDut[i], negValueDut[i]);
			if (max > maxValue*0.1 & crVec[i] > 1e-9 & crVec[i] < 200)
			{
				double temp = 1.0 / (1.0 / crMean - 1.0 / crSlb[i]);
				if (temp > 0)
				{
					crVecCali.push_back(temp);
				}
			}
		}
	}

	return crVecCali;
}
