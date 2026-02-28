#ifndef DLLFILE
#define DLLFILE
#endif // !DLLFILE
#pragma once
#include <opencv2\imgproc\types_c.h>
#include "contrast_profile.h"
ImageProfiler::ImageProfiler(){}

ImageProfiler::~ImageProfiler() {}

ImageProfiler::ImageProfiler(Mat colormap, int size)
{
	jet = colormap;
	n = size;
}

double ImageProfiler::contrast(Mat roi)
{
	double max, min;
	cv::Point min_loc, max_loc;
	cv::minMaxLoc(roi, &min, &max, &min_loc, &max_loc);

	return (max - min) / (max + min);
}

Mat ImageProfiler::profile(Mat &img)
{

	if (img.channels() == 3)//转灰度图
		cvtColor(img, img, CV_BGR2GRAY);

	int row = img.rows / n;
	int col = img.cols / n;

	Mat profile(row, col, CV_64FC1,0.0);

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			profile.at<double>(i, j) = contrast(img(Range(i*n, (i + 1)*n), Range(j*n, (j + 1)*n)).clone());
		}
	}
	return profile;
}


Mat ImageProfiler::contrast_profile(Mat &img)
{
	
	int row = img.rows;
	int col = img.cols;

	Mat my_profile(row,col,CV_8UC3);

	int num_jet = jet.rows;
	double max, min;
	cv::Point min_loc, max_loc;
	cv::minMaxLoc(img, &min, &max, &min_loc, &max_loc);

	double len = max - min;

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			int dis = (img.at<double>(i, j) - min) / len*(num_jet-1);
			my_profile.at<Vec3b>(i, j) = jet.at<Vec3b>(dis, 0);
		}
	}

	return my_profile;
}


Mat ImageProfiler::get_profile(Mat img)
{
	Mat pro = profile(img);

	Mat my_pro = contrast_profile(pro);

	return my_pro;
}


int mainaa()
{
	Mat	img = imread("1.bmp");
	Mat jet = imread("my_jet.jpg");
	int num = 8;
	//初始化
	ImageProfiler my(jet, num);

	//每幅图进行计算
	Mat pro = my.get_profile(img);

//	imwrite("1.jpg", pro);

	//char filename_r[256], filename_w[256];
	//for (int i = 1; i <= 40; i++) {
	//	sprintf_s(filename_r, 256, "E:\\CODE\\contrast_profile\\contrast_profile\\1\\Iteration_%d.tif", i);
	//	sprintf_s(filename_w, 256, "E:\\CODE\\contrast_profile\\contrast_profile\\1\\%d.bmp", i);
	//	Mat	img = imread(filename_r);
	//	Mat pro = my.get_profile(img);
	//	imwrite(filename_w, pro);
	//}
	//
	return 0;
}