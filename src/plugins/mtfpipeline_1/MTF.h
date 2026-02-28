#pragma once
#ifndef MTF_H
#define MTF_H
#include "SlantedEdge.h"

#ifdef DLLFILE
class  __declspec(dllexport) MTF
#else
class __declspec(dllimport) MTF
#endif // DLLFILE

{
private:
	double* freq = NULL;
	double* mtf = NULL;
	double* esf_lsf_x = NULL;
	double* esf = NULL;
	double* lsf = NULL;
	double esf_len = 0;
	//double *freq_need = NULL;
	//double *mtf_need = NULL;
	//int len, interval;
	int bin_len = 0;
	//SplineSpace::Spline *sp = NULL;
	double leftBound = 0, RightBound = 0;
	double slope;//Ð±±ßÐ±ÂÊ	


public:
	MTF();
	~MTF();

	int my_mtf(Mat roi,int mtfType=0);
	//void set_my_freq(int len, int interval);
	//void set_my_mtf();
	double get_my_mtf(double x);

	double* get_freq();
	double* get_mtf();
	double* get_esf_lsf_x();
	double* get_esf();
	double* get_lsf();
	//double* get_freq_need();
	//double* get_mtf_need();
	int get_bin_len();
	double get_esf_len();
	double get_angle();

};


#endif // !MTF_H