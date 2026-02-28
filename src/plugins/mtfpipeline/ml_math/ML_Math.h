#pragma once
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\types_c.h>
#include <vector>
using namespace std;
#include "coreplugin/service/ml.h"

#ifndef ML_MATH_H
#define ML_MATH_H


#ifdef MLMATH_BEGIN
#undef  MLMATH_BEGIN
#endif
#define MLMATH_BEGIN namespace MyMath {

#ifdef MLMATH_END
#undef MLMATH_END
#endif

#define MLMATH_END }


MLMATH_BEGIN

/***************
First use ，set camera pixel size
****************/
extern "C" void __declspec(dllexport)  setPixel(double pixel);

extern "C" bool  __declspec(dllexport) checkSlantROI(cv::Mat roi, unsigned char& direction);

extern "C" bool __declspec(dllexport) checkCrossROI(cv::Mat img, unsigned char &dir);

/*****************
NOTIC:  x 必须小到大
*****************/
extern "C" double  __declspec(dllexport) getSmoothingSpline(double*x, double*y, int bin_len, double need_x);

/***********
AA Profiler 
************/
extern "C" void  __declspec(dllexport) getCTF(unsigned char* buffer, int height, int width, int type, OUT double& frequency,OUT double& contrast);


extern "C" double __declspec(dllexport) get_point_angle(cv::Point2f pointO, cv::Point2f pointA);

extern "C" void __declspec(dllexport) Smooth(double *value, int half_window, int len);

//花樱计算特殊十字线
 vector<cv::Point2f> __declspec(dllexport) circleCenter(cv::Mat img);

 //cross 相同偏移量的4个roi点
 vector<cv::Point> __declspec(dllexport) getCrossAutoROI(cv::Mat img, int offset);

 //cross 相同偏移量的4个roi点
void __declspec(dllexport) getCrossAutoROI(cv::Mat img, int offset, vector<cv::Point>&);

 double __declspec(dllexport) getInterp(double* x, double *y, int len, double need);



MLMATH_END

#endif // !ML_MATH_H

//#include"MathFactory.h"
//#include "MTFPlus.h"