#ifndef DLLFILE
#define DLLFILE
#endif // !DLLFILE

#pragma once
//#include"stdafx.h"
#include "MTF.h"
#include"SlantedCross.h"
using namespace SlantedEdge;
using namespace SlantedCross;


MTF::MTF() {}

MTF::~MTF() {
	if (freq != NULL)
		free(freq);
	if (mtf != NULL)
		free(mtf);
	if (esf_lsf_x != NULL)
		free(esf_lsf_x);

	if (esf != NULL)
		free(esf);
	if (lsf != NULL)
		free(lsf);

	//if(freq_need != NULL)
	//	free(freq_need);

	//if (mtf_need != NULL)
	//	free(mtf_need);
}
int MTF::my_mtf(Mat roi,int mtfType)
{
	try
	{
		int direction;
		if(mtfType==0)
		 direction = calculate_mtf_slantededge(roi, &freq, &mtf, &esf_lsf_x, &esf, &lsf, &bin_len, &esf_len, &slope);//由斜边计算MTF,ESF,LSF
		else if(mtfType==1)
	     direction = calculate_mtf_slantedcross(roi, &freq, &mtf, &esf_lsf_x, &esf, &lsf, &bin_len, &esf_len, &slope);//由斜边计算MTF,ESF,LSF
		return direction;
	}
	catch (const std::exception& e)
	{
		e.what();
	}

}

//void MTF::set_my_freq(int len, int interval) 
//{
//	this->len = len;
//	this->interval = interval;
//	freq_need = (double *)malloc(len * sizeof(double));
//	for (int i = 0; i < len; i++) {
//		freq_need[i] = (i + 1) * interval;
//	}
//}
//
//
//void MTF::set_my_mtf()
//{
//	if (freq_need == NULL) {
//		printf("Wrong: Haven't set frequency!! ");
//	}
//	else
//	{
//		//for (int i = 0; i < bin_len; i++) {
//		//	cout << freq[i] << ":" << mtf[i] << endl;
//		//}
//		//for (int j = 0; j < len; j++) {
//		//	cout << freq_need[j] << endl;
//		//}
//		mtf_need = (double *)malloc(len * sizeof(double));
//		//MTF插值
//		SplineSpace::Spline sp(freq, mtf, bin_len, SplineSpace::GivenSecondOrder, leftBound, RightBound);
//		sp.MultiPointInterp(freq_need, len, mtf_need);
//
//	}
//
//}
double MTF::get_my_mtf(double x)
{
	if ((freq == NULL) || (mtf == NULL)) return 0;
	SplineSpace::Spline sp(freq, mtf, bin_len, SplineSpace::GivenSecondOrder, leftBound, RightBound);
	double mtf_need;
	sp.SinglePointInterp(x, mtf_need);
//	cout << mtf_need << endl;
	double a = 4 * pixel / 4;
//	cout << sin(M_PI * a * x) / (M_PI * a * x) << endl;
	//mtf_need = mtf_need / sin(M_PI * a * x) * (M_PI * a * x);
	return mtf_need;

}

/*******************************************************/

double* MTF::get_freq()
{
	return freq;
}
double* MTF::get_mtf()
{
	return mtf;
}
double* MTF::get_esf_lsf_x()
{
	return esf_lsf_x;
}
double* MTF::get_esf()
{
	return esf;
}
double* MTF::get_lsf()
{
	return lsf;
}
//double* MTF::get_freq_need()
//{
//	return freq_need;
//}
//double* MTF::get_mtf_need()
//{
//	return mtf_need;
//}
int MTF::get_bin_len()
{
	return bin_len;
}
double MTF::get_esf_len()
{
	return esf_len;
}
double MTF::get_angle()
{
	return fabs(atan(slope)) * 180 / M_PI;
}

