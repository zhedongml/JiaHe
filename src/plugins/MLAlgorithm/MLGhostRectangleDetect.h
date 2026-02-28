#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
#include <armadillo>
#include"ml_image_public.h"
namespace algorithm {
	struct GhostRectRe
	{
		std::vector<cv::Rect>rectVec;
		std::vector<cv::Rect>ghostRectVec;
		map<string, cv::Rect>rectMap;
		cv::Mat imgdraw;
		bool flag = false;
		std::string errMsg = "";
	};
	class ALGORITHM_API GhostRectangleDetect :public MLimagePublic
	{
	public:
		GhostRectangleDetect();
		~GhostRectangleDetect();
	public:
		GhostRectRe getGhostRectangleRect(const cv::Mat img,cv::Rect rect);
		vector<cv::Rect>findRectangleByContour(cv::Mat img, cv::Mat& imgdraw, cv::Rect rect);
		vector<cv::Rect>findRectangleByTemplate(cv::Mat img, cv::Mat& imgdraw, cv::Rect rectBoun);
	private:
		cv::Mat preProcess(cv::Mat img);
		vector<cv::Point> matchTemplateMaxLocs(cv::Mat img, cv::Mat templ, double score = 0.7, int bin = 1);
		void getSortData(vector<cv::Rect>rectVec, vector<cv::Rect>& rectVecSort);
	private:
		string m_filePath = "\\JsonFile\\AlgJson\\templateImg\\ghostTemplate.tif";
	};
}
