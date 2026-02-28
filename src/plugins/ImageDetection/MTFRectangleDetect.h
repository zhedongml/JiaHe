#pragma once
#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif
#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
#include"ml_image_public.h"
namespace MLImageDetection {
	struct MTFRectRe
	{
		std::vector<cv::Rect>rectVec; //每个方格外部取的大ROI集合，图片上依次从左到右
		vector<cv::Mat>roiVec; //九个包含方格的ROI图像，图片上依次从左到右
		map<string, cv::Rect>rectMap; //对rectVec依次设置标识
		cv::Mat img_draw;
		bool flag = false;
		std::string errMsg = "";
	};
	class ALGORITHM_API MTFRectangleDetect:public MLimagePublic
	{
	public:
		MTFRectangleDetect();
		~MTFRectangleDetect();
	public:
		MTFRectRe getMtfRectangleRect(const cv::Mat img);
		cv::Mat preProcess(cv::Mat img);
		vector<cv::Rect>findRectangleByContour(cv::Mat img,cv::Mat &imgdraw,vector<double>&xv, vector<double>&yv);
		vector<cv::Rect>findRectangleByTemplate(cv::Mat img, cv::Mat& imgdraw, vector<double>&xv, vector<double>& yv);
		vector<cv::Point2f> matchTemplateMaxLocsMTF(cv::Mat img, cv::Mat templ, double score = 0.6, int bin = 1);
	private:
		void updateImgdraw(cv::Mat& imgdraw, vector<cv::Rect>rectVec, vector<cv::Point2f>ptsVec);
		void getSortData(vector<double>xv, vector<double>yv, vector<cv::Rect>rectVec,vector<cv::Rect>&rectV, vector<cv::Point2f>&ptsV);
		void updateRectsAndPoints(cv::Mat xmat,cv::Mat ymat,vector<cv::Rect>&rectVec,vector<cv::Point2f>&ptsVec);
		void updateRectMap(vector<cv::Rect>rectVec, map<string, cv::Rect>&rectMap);
	private:
		string m_filePath = "./config/ALGConfig/RectangleTempl.tif";
		cv::Rect m_rect = cv::Rect(0,0,-1,-1);
		int m_ROILen = 1000;
		cv::Mat m_dotsAImg;
		vector<double>xdis = {-696,0,696,-696,0,696,-696,0,696};
		vector<double>ydis = {-346,-346,-346,0,0,0,346,346,346};
	};
}

