#pragma once

#ifndef CROSSEDGE_H
#define CROSSEDGE_H

#include <opencv2\opencv.hpp>
#include "Spline.h"

using namespace cv;
using namespace std;

#include <string.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include<numeric>
#ifdef SLANDEDCROSS_BEGIN
#undef  SLANDEDCROSS_BEGIN
#endif
#define SLANDEDCROSS_BEGIN namespace SlantedCross{

#ifdef SLANDEDCROSS_END 
#undef SLANDEDCROSS_END
#endif

#define SLANDEDCROSS_END }


SLANDEDCROSS_BEGIN



#ifndef M_PI
#define M_PI  3.14159265358979323846
#endif



#define VERTICAL 1//垂直方向
#define HORIZONTAL 0//水平方向
#define ALPHA 4.0

static double sqrarg;
//static double pixel = 3.1e-3;
static double pixelc = 0.004;
static int binning = 1;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0: sqrarg*sqrarg)

void SetCrossPixel(double pxl);
double getCrossPixel();
void SetBinning(int binning);
int getBinning();
int calculate_mtf_slantedcross(Mat& roi, double** Freq, double** Mtf, double** Esf_Lsf_x, double** Esf, double** Lsf, int* bin_len, double* esf_len, double* slope);
//预处理
double quick_esf(Mat img);

bool check_roi(Mat& roi, unsigned char& direction);
bool roi_normilized(Mat &roi, double* farea, int& size_x, int& size_y, int& len);

//超采样计算MTF
int mtfProc(double **, double **, double **, double **, double **, int*, double *, double *,
	unsigned short, int *, double *, int *, int *, double*, double*);
void cut_lsf(double *lsf_temp, double **lsf, int &lsf_len, int& bin_len, double max);

void smooth(double *, int , int );
void gamma_correct(double *value, double gamma, int len);

double calculate_esf(double *esf, double *esf_lsf_x, int len, double esf1, double esf2);
void calculate_esf(unsigned int len, double *AveTmp, double *AveEdge);
unsigned short ftwos(long, double, double *, long, double, double *);
void apply_hamming_window(unsigned short, unsigned int, unsigned short,
	double *, long *);
void locate_max_PSF(unsigned int, double *, long *);
void calculate_derivative_old(unsigned int, double *, double *, double *, int);
void calculate_derivative_new(unsigned int, double *, double *, double *, int);

unsigned short bin_to_regular_xgrid(unsigned short, double *, double *,
	double *, long *,
	unsigned short, unsigned short);
unsigned short fit(unsigned long, double *, double *, double *, double *,
	double *, double *, double *);
unsigned short locate_centroids(double *, double *, double *,
	unsigned short, unsigned short, double *, bool);
unsigned short check_slope(double, unsigned short *, int *, double, int);

void polyfit(const double* x, const double* y, size_t length, int poly_n, vector<double>& factor);
void gauss_solve(int n, std::vector<double>& A, std::vector<double>& x, std::vector<double>& b);
void gauss_solve(int n, double* A, double* x, double* b);
double getY(const double x, const vector<double> factor);
void reduce_pixelization(double* esf, int len);
SLANDEDCROSS_END

#endif