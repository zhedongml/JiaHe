#pragma once
#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif

#include <opencv2\opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>
#include<numeric>
#include <string>
#include <algorithm>
#include <functional>
#include "ml_image_public.h"
#include"lsd.h"
//using namespace cv;
using namespace std;
namespace MLImageDetection {
	class CrossCenterParameter
	{
	public:
		static int roilength;
		static int iterationInterval;
		static int searchlength;
		static int asfilterROI;
		static int dataFilterVer;
		static int dataFilterHor;
		static int pointdistance;
		static float anglediff;
	};
	struct  PointInfo
	{
		cv::Point2f pt;
		int angle;
		double mag;
		double theta;
	};
	struct PointSet
	{
		vector<cv::Point2f>pts;
		int angle1;
		vector<cv::Point2f>ptsOther;
		int angle2;
		bool flag = false;
	};
	struct Peak {
		int index;
		double value;
		int width; // 峰值宽度
		double prominence; // 峰值突出度
	};
	class ALGORITHM_API CrossCenter : public MLimagePublic
	{
	public:
		CrossCenter();
		~CrossCenter();
		CrossCenter(cv::Mat img, int off);
		static bool isInitFromJson;
		void loadJsonConfig(const char* path);
		void SetbinNum(int bin);

		cv::Point2f get_crossCenter(int flag = 2);
		cv::Point2f get_crossCenterMTF();// flag=5
		cv::Point2f get_crossCenterDIFF();//centroid method flag=3
		cv::Point2f get_crossCenterLINES();//houghline flag=1
		cv::Point2f get_crossCenterMATCH(cv::Mat templa); // flag=4
		cv::Point2f get_crossCenterGaussian();//flag=2	
		cv::Point2f get_crossCenteGradAndGaussian();// flag=6


		// get the information of the crosshair
		vector<cv::Point> get_roi_center();
		vector<cv::Point> get_newroi_center();
		int get_line_width();
		cv::Mat getImgDraw();
		cv::Point2f getRotationAngle();
		vector<double> GetVerticalLine();
		vector<double> GetHorizontalLine();
		double GetHorizontalLineTheta();
		double GetVerticalLineTheta();
		// the different crosshair detect algorithm
		cv::Point2f find_centerMTF(cv::Mat img); // template/grad+gaussian
		cv::Point2f find_centerMTF_manual(cv::Mat img);
		cv::Point2f find_centerGrad(cv::Mat img);//grad
		cv::Point2f find_centerTemplate(cv::Mat img, cv::Mat tmpl);
		cv::Point2f find_center(cv::Mat img);//centroid
		cv::Point2f find_centerLINES(cv::Mat img);//houghLines
		cv::Point2f find_centerGaussian(cv::Mat img, bool fitflag = true);//gaussian
		cv::Point2f find_centerGaussian1(cv::Mat img); //

		cv::Point2f find_centerGaussianEn(cv::Mat img);//gaussian

		cv::Point2f find_centerGradAndGaussian(cv::Mat img);// grad+gaussian
		cv::Point2f find_centerHoughLineAndWidth(cv::Mat img, int i, int signFlag);// houghline-width/2
		cv::Point2f find_centerLSD(cv::Mat img);

		int calLine(vector<cv::Vec4i>& lines, vector<float>& ks, float& a, float& b, float& c);

		double getHorizontalLineAngle();
	private:
		int m_binNum = 1;
		vector<float> m_angle1, m_angle2;
		double m_thetaH = 0;
		double m_thetaV = 0;
		cv::Point2f m_center = cv::Point2f(0, 0);
		cv::Point2f center_offset = cv::Point2f(0, 0);
		//Mat img;
		cv::Mat m_img_draw;
		cv::Mat m_img;
		int off;
		int m_line_width = 0;
		vector<cv::Point> roi_center;
		vector<cv::Point> new_roi_center;
		vector<cv::Point> find_roi_center(cv::Point2f center, float a1, float b1, float c1, float a2, float b2, float c2);
		void sepLines(vector<cv::Vec4i>& lines, vector<cv::Vec4i>& line1, vector<float>& ks1, vector<cv::Vec4i>& line2,
			vector<float>& ks2);
		cv::Vec4f verticalLineFit(cv::Mat img, bool fitflag);
		PointSet verticalScan(cv::Mat img);
		PointSet horizontalScan(cv::Mat img);
		float findGaussianFitMaxValue(vector<cv::Point2f>pts);
		cv::Vec4f horizontalLineFit(cv::Mat img, bool fitflag);

		cv::Vec4f verticalLineFit(cv::Mat img);
		cv::Vec4f horizontalLineFit(cv::Mat img);



		bool getROIRect(cv::Mat img, cv::Mat rowMat, int i0, int flag, int len, cv::Rect& rect, double& maxgray);
		vector<double>linespace(double start, double end, double step);
		vector<cv::Point2f> pointFinterByAngle(vector<cv::Point2f>pts, double thresh = 5);
		vector<cv::Point2f> pointClusterByAngle(vector<cv::Point2f>pts, double thresh = 5);
		PointSet pointFilterAngle(vector<cv::Point2f>pts, double thresh = 5);
		vector<int> findPeaks(cv::Mat data, std::vector<Peak>& peaks, double minHeight, int minWidth, double minProminence);
		vector<int>findPeaksByContour(cv::Mat rowMat);
		vector<int>findPeakByMaxLoc(cv::Mat rowMat);
		vector<int>findPeaks1(cv::Mat rowMat, double minpeakdistance, double minpeakheight);

	private:
		double m_a1;
		double m_b1;
		double m_c1;
		double m_a2;
		double m_b2;
		double m_c2;

		double m_hAngle = 0;
		double m_vAngle = 0;
	};
}
