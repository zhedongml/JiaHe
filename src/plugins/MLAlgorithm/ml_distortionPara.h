#pragma once
#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<iostream>
#include "ml_image_public.h"
#include "ml_hydrus_2config.h"
#include "ml_gridDetect.h"
using namespace std;
using namespace cv;
namespace algorithm {
	struct  DistortionRe
	{
        double comp = -1;
		double disAvg = -1;
        std::vector<double> disCorner;
		cv::Mat xPos;
		cv::Mat yPos;
		cv::Mat disMat;
		cv::Point2f fovCenter = cv::Point2f(3947.92, 3045.94);
		double newFocalLength = -1;
	};

    class ALGORITHM_API MLDistortionPara:public MLimagePublic
    {
	public:
		MLDistortionPara();
		~MLDistortionPara();
	public:
		void SetGridImage(cv::Mat img_grid);
        void SetFovAngle(double fovH, double fovV);
        void SetRotationDeg(float degree);
		DistortionRe GetGridDistortion();
		double GetDistortionAverage();
		cv::Mat GetCornerPositionsX();
		cv::Mat GetCornerPositionsY();
        std::vector<double> GetDistortionCorner();
		double GetNewfocalLength();
		cv::Mat getImgDraw();		
	private:
		DistortionRe GridDistortion(cv::Mat img, int flag = 2, cv::Rect rect = cv::Rect(0, 0, -1, -1));
		cv::Rect getGridRect(cv::Mat roi);
        double getCornerDistortionVal(cv::Point2f corner, cv::Point2f center);
        cv::Point2f getCenter(vector<Point2f> corns, cv::Mat indexMap);

	private:
		cv::Mat m_img_grid;
		MLHydrusConfig hydrusConfig;
		Newpara m_para;
		cv::Mat m_img_draw;
		DistortionRe m_distortionRe;
        double m_fovH = -1;
        double m_fovV = -1;
        double m_rotationAngle=0.0;
    };
}

