#pragma once

#include <opencv2\opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <stdlib.h>
#include<numeric>
#include <string>
#include <algorithm>
#include <functional>

using namespace cv;
using namespace std;
class CrossCenter
{
public:
	CrossCenter();
	~CrossCenter();
	CrossCenter(Mat img, int off, int size = 17);
	cv::Point2f get_crossCenterDIFF();
	cv::Point2f get_crossCenterLINES();
	cv::Point2f get_crossCenterMATCH(Mat templa);
	vector<Point> get_roi_center();
	int get_line_width();
private:
	vector<float> m_angle1, m_angle2;
	cv::Point2f center;
	cv::Point2f center_offset = cv::Point2f(0, 0);
	Mat img;
	int off;
	//int roiLen;
	int size;
	int line_width;
	vector<Point> roi_center;

	vector<Point> find_roi_center(cv::Point2f center, float a1, float b1, float c1, float a2, float b2, float c2);

	void thinningIteration(Mat& img, int iter);



	void thinning(const cv::Mat& src, cv::Mat& dst);

	double locate_mid(Mat m, int mlen);

	cv::Point2f calLinesCross(float a1, float b1, float c1, float a2, float b2, float c2);
	int calLine(vector<Vec4i>& lines, vector<float>& ks, float& a, float& b, float& c);
	void sepLines(vector<Vec4i>& lines, vector<Vec4i>& line1, vector<float>& ks1, vector<Vec4i>& line2, vector<float>& ks2);

	void find_center();
	bool find_centerLINES();
	void find_centerMTACH(Mat templa);
};

