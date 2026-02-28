#pragma once
#include "opencv2/opencv.hpp"
using namespace cv;

struct PipeLineOutPut {
	double* Esf_Lsf_x = nullptr;
	double* Esf = nullptr;
	double* Lsf = nullptr;
	//double* esf_len = nullptr;// 数组长度

	double* Freq= nullptr; //mtf  axis x
	double* Mtf = nullptr;// mtf axis y == double* sfr,
	int bin_len; //mtf数组长度
	double slope;
	unsigned char direction = -1;
	long center;
	int errNum = 0;
	double* freq_interp = nullptr;
	double* mtf_interp = nullptr;
	int len_interp = 0;
};

struct  PipeLineInput {
	Mat roi;
	double* farea;
	double* farea_old;
	double slope;
	struct DataInfo {
		int size_x; int size_y; int length;
	} dataInfo;

	struct  Centroid {
		double* temp;  //x
		double* shifts;  //y
		double offset; //最后一行中心点
	} centroids;
	struct PolynomialFit {
		double R2;
		std::vector<double> result;
	} linearFit;
	struct Oversample {
		//Polynomial Fit vector result=>
		double* edgex;
		double* signal;
		double* aveEdge; //esf
		double* aveEdgeCopy;
	} overSample;
	struct  Derivation {
		double* aveTmp;   //(esf);
		double* aveEdge; // (lsf);
		double* centroid;
	};
	double* sfr; //mtf
};
