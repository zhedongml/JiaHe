#pragma once
//#include "pch.h"
#include "CrossCenter.h"
//#include <MLAlgorithm/ml_optpara_kunlun.h>
//#include"qlog.h"
using namespace std;
using namespace cv;
using namespace MLImageDetection;
double locate_mid(Mat m, int mlen)
{

	double* img_data = (double*)malloc(mlen * sizeof(double));
	double* diff_data = (double*)malloc(mlen * sizeof(double));
	//img_data_free = img_data;

	for (int i = 0; i < mlen; i++)
	{
		double* data = (double*)m.data;
		img_data[i] = data[i];
	}
	diff_data[0] = 0;
	diff_data[mlen - 1] = 0;
	for (int i = 1; i < mlen - 1; i++)
	{
		diff_data[i] = img_data[i - 1] - img_data[i + 1];
		//cout << diff_data[i] << endl;
	}
	//	cout << "diffdata=" << temp << endl;
	double max_index = max_element(diff_data, diff_data + mlen) - diff_data;
	double min_index = min_element(diff_data, diff_data + mlen) - diff_data;
	free(img_data);
	free(diff_data);
	return (max_index + min_index) / 2;
}
void thinningIteration(Mat& img, int iter)
{
	CV_Assert(img.channels() == 1);
	CV_Assert(img.depth() != sizeof(uchar));
	CV_Assert(img.rows > 3 && img.cols > 3);

	Mat marker = Mat::zeros(img.size(), CV_8UC1);

	int nRows = img.rows;
	int nCols = img.cols;

	if (img.isContinuous()) {
		nCols *= nRows;
		nRows = 1;
	}

	int x, y;
	uchar* pAbove;
	uchar* pCurr;
	uchar* pBelow;
	uchar* nw, * no, * ne;
	uchar* we, * me, * ea;
	uchar* sw, * so, * se;

	uchar* pDst;

	pAbove = NULL;
	pCurr = img.ptr<uchar>(0);
	pBelow = img.ptr<uchar>(1);

	for (y = 1; y < img.rows - 1; ++y) {
		pAbove = pCurr;
		pCurr = pBelow;
		pBelow = img.ptr<uchar>(y + 1);

		pDst = marker.ptr<uchar>(y);

		no = &(pAbove[0]);
		ne = &(pAbove[1]);
		me = &(pCurr[0]);
		ea = &(pCurr[1]);
		so = &(pBelow[0]);
		se = &(pBelow[1]);

		for (x = 1; x < img.cols - 1; ++x) {
			// shift col pointers left by one (scan left to right)
			nw = no;
			no = ne;
			ne = &(pAbove[x + 1]);
			we = me;
			me = ea;
			ea = &(pCurr[x + 1]);
			sw = so;
			so = se;
			se = &(pBelow[x + 1]);

			int A = (*no == 0 && *ne == 1) + (*ne == 0 && *ea == 1) +
				(*ea == 0 && *se == 1) + (*se == 0 && *so == 1) +
				(*so == 0 && *sw == 1) + (*sw == 0 && *we == 1) +
				(*we == 0 && *nw == 1) + (*nw == 0 && *no == 1);
			int B = *no + *ne + *ea + *se + *so + *sw + *we + *nw;
			int m1 = iter == 0 ? (*no * *ea * *so) : (*no * *ea * *we);
			int m2 = iter == 0 ? (*ea * *so * *we) : (*no * *so * *we);

			if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
				pDst[x] = 1;
		}
	}

	img &= ~marker;
}

void thinning(const cv::Mat& src, cv::Mat& dst)
{
	dst = src.clone();
	dst /= 255;

	cv::Mat prev = cv::Mat::zeros(dst.size(), CV_8UC1);
	cv::Mat diff;

	do {
		thinningIteration(dst, 0);
		thinningIteration(dst, 1);
		absdiff(dst, prev, diff);
		dst.copyTo(prev);
		//static int ii = 0;
		//cout << ++ii << std::endl;
	} while (cv::countNonZero(diff) > 0);

	dst *= 255;
}
cv::Point2f CrossCenter::find_center(cv::Mat img)
{
	Mat img_gray;
	if (img.channels() == 3)
		cvtColor(img, img_gray, CV_BGR2GRAY);
	else
		img_gray =img;
	cv::Mat img_draw = img.clone();
	Mat element1 = getStructuringElement(MORPH_RECT, Size(15, 15));
	Mat element2 = getStructuringElement(MORPH_RECT, Size(15, 15));
	//Mat element = getStructuringElement(MORPH_RECT, Size(m_MorphSizeWidth, m_MorphSizeHeight));
	int iRow = img.rows;
	int iCol = img.cols;
	double minv = 0.0, maxv = 0.0;
	Point minp, maxp;
	cv::minMaxLoc(img_gray, &minv, &maxv, &minp, &maxp);
	morphologyEx(img_gray, img_gray, MORPH_CLOSE, element1);
	//threshold(img_gray, img_gray,(minv+maxv)/2, 255, CV_THRESH_BINARY);
	threshold(img_gray, img_gray, 0, 255, CV_THRESH_OTSU);
	thinning(img_gray, img_gray);

	vector<Vec4i> lines;
	vector<Vec4i> lines1, lines2;
	vector<float> ks1, ks2;
	Mat m;
	img_gray.convertTo(m, CV_64FC1);
	Mat m_row(1, iCol, CV_64FC1, Scalar(0));
	Mat m_col(iRow, 1, CV_64FC1, Scalar(0));
	reduce(m, m_row, 0, CV_REDUCE_SUM);
	reduce(m, m_col, 1, CV_REDUCE_SUM);

	double x = locate_mid(m_row, iCol);
	double y = locate_mid(m_col, iRow);
	m_center = Point2f(x, y);
	circle(img_draw, m_center, 5, cv::Scalar(0, 0, 255), -1);

	cv::Mat img_threshold, img_dilate, img_erode;
	img_threshold = img_gray.clone();
	dilate(img_threshold, img_dilate, element1);
	erode(img_dilate, img_erode, element2);

	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	/// Gradient X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel(img_gray, grad_x, CV_64F, 1, 0, 3, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);

	/// Gradient Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel(img_gray, grad_y, CV_64F, 0, 1, 3, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);


	//thinning(img_gray, img_gray);
	vector<double> coe_x, coe_y;
	//coe1 = line_fit(abs_grad_x);
	//coe2 = line_fit(abs_grad_y);
    coe_x = line_fit(abs_grad_x, 1);
    coe_y = line_fit(abs_grad_y, 0);

	Point2f cen;
	cen.x = (coe_x[0] * coe_y[1] + coe_x[1]) / (1 - coe_x[0] * coe_y[0]);
	cen.y = coe_y[0] * cen.x + coe_y[1];

	circle(img_draw, cen, 2, Scalar(255, 0, 0), -1);
	circle(img_draw, m_center, 1, Scalar(0, 255, 0), -1);

	//Mat draw_img(img_gray.size(), CV_8UC3);
	//for (int i = 0; i < lines.size(); i++)
	//{
	//	Vec4i l = lines[i];
	//	Scalar color = Scalar(rand() % 255, rand() % 255, rand() % 255);
	//	line(img_draw, Point(l[0], l[1]), Point(l[2], l[3]), color, 1);

	//}
	return m_center;
}
