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
//#include <MLImageDetection>
#include <algorithm>

#include <functional>
#include "ml_image_public.h"
using namespace cv;
using namespace std;
using namespace MLImageDetection;

class ALGORITHM_API MultiCrosshairCenter:public MLimagePublic
{
	struct pointSet
	{
		int Numx1 = 0;
		double xSum1 = 0;
		int Numx2 = 0;
		double xSum2 = 0;
		int Numy1 = 0;
		double ySum1 = 0;
		int Numy2 = 0;
		double ySum2 = 0;
	};
public: 
	MultiCrosshairCenter();
	~MultiCrosshairCenter();
	vector<cv::Point2f>getMultiCrosshairCenter(cv::Mat img,int flag=1);

private:
	vector<cv::Point2f>multiCrosshairHoughLine(cv::Mat img);


};

