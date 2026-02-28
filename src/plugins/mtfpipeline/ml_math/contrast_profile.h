#ifndef PROFILER_H
#define PROFILER_H
#pragma once
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>

using namespace cv;
using namespace std;

class ImageProfiler 
{
public:
	ImageProfiler();
	ImageProfiler(Mat colormap, int size);
	~ImageProfiler();
	Mat get_profile(Mat img);

private:

	Mat jet;
	int n;
	
	double contrast(Mat roi);


	Mat profile(Mat &img);
	Mat contrast_profile(Mat &img);


};


#endif // !PROFILER_H