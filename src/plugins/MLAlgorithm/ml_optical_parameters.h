#ifndef MLALGORITHM_MLOPTICALPARAMETERS_H_
#define MLALGORITHM_MLOPTICALPARAMETERS_H_
#include "ml_distortion_correction.h"
#include "ml_image_public.h"
#include <numeric>
#include<vector>
#include <map>
#include<fstream>
#include<iostream>
using namespace std;
namespace algorithm 
{
	class ALGORITHM_API MLOpticalParametersSystem
	{
	public:
		MLOpticalParametersSystem();
		~MLOpticalParametersSystem();
	public:
		void SetXYZImage(cv::Mat X, cv::Mat Y, cv::Mat Z);
		void SetYImage(cv::Mat Y);
		void SetChessboardImage(cv::Mat imgP, cv::Mat imgN, cv::Rect roiRect = cv::Rect(0, 0, -1, -1),int row=2,int col=2);
		double GetUniformity();
		double GetLuminance();
		double GetDeltE();
		double GetDeltC();
		double GetbigChessboardCR();
		double GetsmallChessboardCR();			
		private:
			cv::Mat m_Y, m_X, m_Z, m_imgP, m_imgN;
			cv::Rect m_roiRect = cv::Rect(0, 0, -1, -1);
			int m_row = 2, m_col = 2;
	public:
		double Uniformity(cv::Mat Y);
		double Luminance(cv::Mat Y);
		double DeltaE(cv::Mat X, cv::Mat Y, cv::Mat Z);
		double DeltaC(cv::Mat X, cv::Mat Y, cv::Mat Z);
		double bigChessboardCR(cv::Mat img_positive, cv::Mat imgN, int row=3, int col=3, cv::Rect roiRect = cv::Rect(0, 0, -1, -1));
		double smallChessboardCR(cv::Mat img_positive, cv::Mat imgN, int row=4, int col=4, cv::Rect roiRect = cv::Rect(0, 0, -1,-1));
	private:
		bool getCRArea(cv::Mat img_gray, cv::Mat temp_gray, int row, int col, int size,int num);
		cv::Point2f crossHairDetect(cv::Mat img);
	private:
		vector<cv::Rect> GetCRrect();
		vector<cv::Rect> m_CRrect;
	};

	struct roiAreaData {
		vector<cv::Rect>roiVecP;
		vector<cv::Rect>roiVecN;
		vector<cv::Rect>roiVecPC;
		vector<cv::Rect>roiVecNC;
	};
	class ALGORITHM_API MLOpticalParametersSLB : public MLimagePublic
	{
	public:
		MLOpticalParametersSLB();
		~MLOpticalParametersSLB();		
	public:
		void SetImageGhost(cv::Mat img_ghost);
		void SetImageClear(cv::Mat img_clear);
		void SetChessboardImage(cv::Mat imgP, cv::Mat imgN, cv::Rect roiRect = cv::Rect(0, 0, -1, -1));
		void SetFovPara(double magnification, double pixel_size, double FocalLength);		
		cv::Point2f GetCenterCoordinate();
		cv::Point2f GetFovAngle();
		double GetmeanY();
		double GetUniformity();
		//double GetCenterContrastSLB();
		//double GetHarmonicMeanContrastSLB();
		//double GetArithemeticMeanContrastSLB();
		cv::Mat GetImageClear();
	public:
		
		roiAreaData SetCRROIArea(cv::Mat imgP,cv::Mat imgN, String str,bool isSmall,cv::Rect rect1 = cv::Rect(0, 0, -1, -1));
		void ReoderROIVec(vector<cv::Rect>roiVecP, vector<cv::Rect>roiVecN, vector<cv::Rect>& roiVecPNew, vector<cv::Rect>& roiVecNNew);
		cv::Point2f CenterCoordinate(cv::Mat img);
		cv::Point2f FovAngle(cv::Mat img, double magnification, double pixel_size, double FocalLength, cv::Rect roiRect = cv::Rect(125, 1, 1440, 1095));
		double meanY(cv::Mat img, cv::Rect roiRect=cv::Rect(125, 1, 1440, 1095));
	    double Uniformity(cv::Mat img, cv::Rect roiRect=cv::Rect(125, 1, 1440, 1095));	

		double CenterContrastSLBBigChessboard(cv::Mat imgP, cv::Mat imgN, roiAreaData re, cv::Rect rect1 = cv::Rect(0, 0, -1, -1));//(422, 148, 800, 800)
		double HarmonicMeanContrastSLBBigChessboard(cv::Mat imgP, cv::Mat imgN, roiAreaData re, cv::Rect roiRect = cv::Rect(0, 0, -1, -1));//(125, 1, 1440, 1095)
		double ArithemeticMeanContrastSLBBigChessboard(cv::Mat imgP, cv::Mat imgN, roiAreaData re, cv::Rect roiRect = cv::Rect(0, 0, -1, -1));//cv::Rect(125, 1, 1440, 1095)		
	
		double CenterContrastSLBSmallChessboard(cv::Mat imgP, cv::Mat imgN, roiAreaData re, cv::Rect rect1 = cv::Rect(0, 0, -1, -1));//cv::Rect(762, 488, 120, 120)
		double HarmonicMeanContrastSLBSmallChessboard(cv::Mat imgP, cv::Mat imgN, roiAreaData re, cv::Rect roiRect = cv::Rect(0, 0, -1, -1));//cv::Rect(125, 1, 1440, 1095))
		double ArithemeticMeanContrastSLBSmallChessboard(cv::Mat imgP, cv::Mat imgN, roiAreaData re, cv::Rect roiRect = cv::Rect(0, 0, -1, -1));//cv::Rect(125, 1, 1440, 1095))
	public:
		vector<cv::Rect> detectChessboard(cv::Mat img, vector<cv::Rect>& posC,bool isSmall);
		
		bool  findChessboardLibdetect(cv::Mat img, vector<cv::Point2f>&points);
		bool  GetROIArea(vector<cv::Point2f>points, vector<cv::Rect>&roiVect);
		vector<double> CalculateROIValue(cv::Mat img, vector<cv::Rect>roiVec);
		vector<double> CalculateCR(vector < double>posValue, vector<double>negValue);
		vector<double> CalculatInverse(vector<double>crVec);
		cv::Mat GetROIMat(cv::Mat img, cv::Rect roiRect = cv::Rect(0, 0, -1, -1));

	public:

	
		vector<double> GetCenterCRSmall();
		vector<double> GetAllCRSmall();
		vector<double> GetCenterCRBig();
		vector<double> GetAllCRbig();

		vector<cv::Rect>GetCenterBigChessboardArea();
		vector<cv::Rect>GetCenterSmallChessboardArea();
		vector<cv::Rect>GetBigChessboardArea();
		vector<cv::Rect>GetSmallChessboardArea();
		vector<cv::Rect>m_bigChessboard;
		vector<cv::Rect>m_smallChessboard;
		vector<cv::Rect>m_centerBigChessboard;
		vector<cv::Rect>m_centerSmallChessboard;

		double GetCenterBigChessboardCR();
		double GetCenterSmallChessboardCR();
		double GetBigChessboardACR();
		double GetSmallChessboardACR();
		double GetBigChessboardHCR();
		double GetSmallChessboardHCR();

		double m_centerBigChessboardCR;
		double m_centerSmallChessboardCR;
		double m_bigChessboardACR;
		double m_smallChessboardACR;
		double m_bigChessboardHCR;
		double m_smallChessboardHCR;
		vector<double>m_centerCRVec_small;
		vector<double>m_allCRVec_small;
		vector<double>m_centerCRVec_big;
		vector<double>m_allCRVec_big;		
	private:
		cv::Mat m_img_ghost;
		cv::Mat m_img_clear;
		cv::Mat m_imgP;
		cv::Mat m_imgN;
		cv::Rect m_roiRect;
		double m_magnification=1;
		double m_pixel_size=5.5;
		double m_FocalLength=13.3;
	};
	class ALGORITHM_API MLOpticalParametersDUT : public MLimagePublic
	{
	public:
		MLOpticalParametersDUT();
		~MLOpticalParametersDUT();
	public:
		MLOpticalParametersSLB slbMetrics;
		roiAreaData GetCRROIArea(String str,bool isSmallChecker);

		void SetImageClear(cv::Mat img_clear);
		void SetImageChessboard(cv::Mat imgP,cv::Mat imgN,cv::Rect roiRect=cv::Rect(0,0,-1,-1));
		//double GetPupilEfficency();
		double GetEyeboxThroughput();
		//double GetCenterContrastDUT();
		//double GetHarmonicMeanContrastDUT();
	//	double GetArithmeticMeanContrastDUT();
		double GetAngleUniformity();
		double GetSpacialUniformity();
	public:
		double PupilEfficiency(cv::Mat clear_dut, cv::Mat clear_slb, cv::Rect roiRect = cv::Rect(125, 1, 1440, 1095));
		double EyeboxThroughput(vector<double>pupil_efficiency, double Seyebox, double leyepupil = 3);
		double AngleUniformity(cv::Mat img, cv::Rect roiRect = cv::Rect(125, 1, 1440, 1095));
		double SpatialUniformity(vector<double>pupil_efficiency);
	
		double CenterContrastDUTBigChessboard(cv::Mat imgP, cv::Mat imgN, cv::Mat slbPos, cv::Mat slbNeg, roiAreaData re, cv::Rect roiRect = cv::Rect(0, 0, -1, -1));//cv::Rect(422, 148, 800, 800)
		double HarmonicMeanContrastDUTBigChessboard(cv::Mat imgP, cv::Mat imgN, cv::Mat slbPos, cv::Mat slbNeg, roiAreaData re, cv::Rect roiRect = cv::Rect(0, 0, -1, -1));//(125, 1, 1440, 1095)
		double ArithmeticMeanContrastDUTBigChessboard(cv::Mat imgP, cv::Mat imgN, cv::Mat slbPos, cv::Mat slbNeg, roiAreaData re, cv::Rect roiRect = cv::Rect(0, 0, -1, -1));//	(125, 1, 1440, 1095)	
	
		double CenterContrastDUTSmallChessboard(cv::Mat imgP, cv::Mat imgN, cv::Mat slbPos, cv::Mat slbNeg, roiAreaData re, cv::Rect = cv::Rect(0, 0, -1, -1));//cv::Rect(498, 772, 100, 100))
		double HarmonicMeanContrastDUTSmallChessboard(cv::Mat imgP, cv::Mat imgN, cv::Mat slbPos, cv::Mat slbNeg, roiAreaData re, cv::Rect roiRect = cv::Rect(0, 0, -1, -1));// cv::Rect(498, 772, 100, 100)
		double ArithmeticMeanContrastDUTSmallChessboard(cv::Mat imgP, cv::Mat imgN, cv::Mat slbPos, cv::Mat slbNeg, roiAreaData re, cv::Rect roiRect = cv::Rect(0, 0, -1, -1));//cv::Rect(498, 772, 100, 100)
		double CalculateInvaidCRMean(vector<double>crVec);
		vector<double> CalculateCRCalibration(vector<double>crVec,vector<double> allSlb,vector<double>posValueDut,vector<double>negValueDut, double maxValue);

	private:
	cv::Mat	m_img_clear;
	cv::Mat m_img_dut;
	cv::Mat m_imgP;
	cv::Mat m_imgN;
	cv::Rect m_roiRect= cv::Rect(125, 1, 1440, 1095);
	vector<double>m_PupilEefficencyVec;
	double m_Seyebox;
	double m_leyepupil = 3;
	double m_CRslb;	
	};
}

#endif // !MLALGORITHM_MLOPTICALPARAMETERS_H_

