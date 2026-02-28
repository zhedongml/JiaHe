#include "ml_two_cross.h"
//#include "../mlx/LoggingWrapper.h"

void NaiveRemoveNoise(cv::Mat& srcImage, double pNum);
void NaiveRemoveShortLine(cv::Mat& srcImage, cv::Mat& tmp, double pNum);


using namespace algorithm;
using namespace cv;
using namespace std;
MLTwoCross::MLTwoCross()
{
}

MLTwoCross::~MLTwoCross()
{
}

double MLTwoCross::GetDistance(Point a, Point b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

Point MLTwoCross::GetCenter(const vector<Point> contour)
{
	Point sum = Point(0,0);
	for (int i = 0; i < contour.size(); i++)
	{
		sum += contour[i];
	}
	return Point(sum.x / contour.size(), sum.y / contour.size());
}

int MLTwoCross::GetLen(const vector<Point> contour)
{
	int sumLen = 0;
	for (int i = 0; i < contour.size(); i++)
	{
		int end;
		int begin = i;
		if (begin == contour.size() - 1)
			end = 0;
		else
			end = i + 1;
		sumLen += GetDistance(contour[begin], contour[end]);
	}
	return sumLen / 4;

}

MLRect MLTwoCross::CheckRect(const vector<vector<Point>> contours, vector<Vec4i> hierarchy)
{
	MLRect temp;
	vector<Point> approx;
	int threshold = 100;
	double perimeter;
	for (int i = 0; i < contours.size(); i++)
	{
		perimeter = arcLength(Mat(contours[i]), true);
		approxPolyDP(Mat(contours[i]), approx, sqrt(perimeter) / 2, true);

		if (isContourConvex(Mat(approx)) && fabs(contourArea(Mat(approx))) > threshold * threshold)
		{

			temp.contour = approx;
			temp.center = GetCenter(approx);
			temp.len = GetLen(approx);
			break;

		}
	}
	
	return temp;



}

cv::Point2f MLTwoCross::GetTwoCrossCenter(cv::Mat img)
{
	string message;
    cv::Mat img_gray;
    if (img.channels() == 3)//×ª»Ò¶ÈÍ¼
        cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    else
        img_gray = img.clone();
	Mat dst8bit;
	dst8bit = img_gray.clone();
	if (img_gray.depth() == CV_16U)
	{
		Mat outtmp;// = Mat(roi.cols, roi.rows, CV_8UC1);

		double minv = 0.0, maxv = 0.0;
		double* minp = &minv;
		double* maxp = &maxv;
		minMaxIdx(dst8bit, minp, maxp);

		if (maxv > 4095.1)
		{
			cv::Mat tmp = dst8bit / 65535 * 255;

			tmp.convertTo(img_gray, CV_8UC1);
		}
		else
		{
			cv::Mat tmp = dst8bit / 4095.0 * 255;

			tmp.convertTo(img_gray, CV_8UC1);
		}

	}
    img_rows_ = img.rows;
    img_cols_ = img.cols;
	Mat img_process = img_gray.clone();
	Mat img_threshold;
	cv::Mat img_draw;
	cvtColor(img_gray, img_draw, cv::COLOR_GRAY2BGR);
	int s = 10;
	cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(s, s));
	cv::morphologyEx(img_process, img_process, cv::MORPH_DILATE, element1);
	cv::medianBlur(img_process, img_process, 3);
    threshold(img_process, img_threshold, 0, 255, cv::THRESH_OTSU);
	cv::medianBlur(img_threshold, img_threshold, 3);
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(img_threshold, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

	MLRect my_rect = CheckRect(contours, hierarchy);
	//cv::rectangle(img_draw, my_rect.contour.at(0), my_rect.contour.at(2), cv::Scalar(0, 0,255));
	message = "Find rectangle, center: (" + std::to_string(my_rect.center.x) + "," + std::to_string(my_rect.center.y) + "), length: " + std::to_string(my_rect.len);
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
	
	//for (int i = 0; i < my_rect.contour.size(); i++)
	//{
	//	cv::circle(img, my_rect.contour[i], 2, cv::Scalar(255, 255, 0), -1);
	//}

	if(my_rect.center == Point(0, 0))
		return cv::Point2f(0, 0);


	Mat labels, centroids, img_color, stats;
	int nccomps = cv::connectedComponentsWithStats(img_threshold, labels, stats, centroids);

	message = "Connected area number: " + std::to_string(nccomps);
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
	
	double min_distance = INFINITY;
	int min_index = 0;
	int num_big = 0;
	for (int i = 0; i < nccomps; i++)
	{
		message = "Connected area No.  " + std::to_string(i) + ":";
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
		message = "Size: (" + std::to_string(stats.at<int>(i, 0)) + "," + std::to_string(stats.at<int>(i, 1)) + "," + std::to_string(stats.at<int>(i, 2)) + "," + std::to_string(stats.at<int>(i, 3)) + "," + std::to_string(stats.at<int>(i, 4)) + ")";
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
		double d = pow(centroids.at<double>(i, 0) - my_rect.center.x, 2) + pow(centroids.at<double>(i, 1) - my_rect.center.y, 2);
		if (d < min_distance && stats.at<int>(i, 4)>10000 && stats.at<int>(i, 4) < img_rows_ * img_cols_ / 4)
		{
			num_big++;
			min_distance = d;
			min_index = i;
		}
	}
	message = "Min distance: " + std::to_string(min_distance);
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());

	my_rect.center = Point(centroids.at<double>(min_index, 0), centroids.at<double>(min_index, 1));
	message = "Rectangle center update: (" + std::to_string(my_rect.center.x) + "," + std::to_string(my_rect.center.y) + ")";
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());

	Rect region_image(0, 0, img_cols_, img_rows_);
	//minus rectangle
	Mat mask, img_minus;
	double radius = my_rect.len;
	//Rect region(my_rect.center.x - radius, my_rect.center.y - radius, 2 * radius, 2 * radius);
	mask = Mat::zeros(img_threshold.size(), CV_8UC1);
	cv::circle(mask, my_rect.center, radius, cv::Scalar(255, 255, 255), -1, 8, 0);
	img_minus = mask & img_gray;
	Mat img_two_cross = img_gray - img_minus;
	cv::morphologyEx(img_two_cross, img_two_cross, cv::MORPH_DILATE, element1);
	cv::medianBlur(img_two_cross, img_two_cross, 3);
	threshold(img_two_cross, img_two_cross, 0, 255, cv::THRESH_OTSU);
	Mat labels2, centroids2, img_color2, stats2;
	int nccomps2 = cv::connectedComponentsWithStats(img_two_cross, labels2, stats2, centroids2);
	//choose roi according to center of rectangle
	double second_min_distance = INFINITY;
	int second_min_index = 0;
	int second_min_pixval = 0;
	for (int i = 0; i < nccomps2; i++)
	{
		double d = pow(centroids2.at<double>(i, 0) - my_rect.center.x, 2) + pow(centroids2.at<double>(i, 1) - my_rect.center.y, 2);
		cv::Point2f ptf(centroids2.at<double>(i, 0), centroids2.at<double>(i, 1));
		
		if (d < second_min_distance && stats2.at<int>(i, 4)>2000 && stats2.at<int>(i, 4) < img_rows_ * img_cols_ / 4)
		{
			cv::circle(img_draw, ptf, 3, cv::Scalar(0, 0, 255), -1, 8, 0);
			second_min_distance = d;
			second_min_index = i;
			int centx = centroids2.at<double>(i, 0);
			int centy = centroids2.at<double>(i, 1);
			second_min_pixval=labels2.at<int>(centy, centx);
		}
	}

	message = "Second min distance: " + std::to_string(second_min_distance);
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
	cv::Mat lableclone = Mat::zeros(labels2.size(), CV_8UC1);
	for(int i=0;i< labels2.rows;i++)
		for (int j = 0; j < labels2.cols; j++) {
			int pixval = labels2.at<int>(i, j);
			if (pixval == second_min_index) {
				lableclone.at<char>(i, j) = 255;
			}
		}

	Point2f center = GetCrossCenter(lableclone);

	if (center == Point2f(0, 0))
	{
		return cv::Point2f(0, 0);
	}
	return center; 
}

std::vector<cv::Point2f> MLTwoCross::GetTwoCrossROICenter(int off)
{
	roi_center_.clear();
	vector<cv::Point2f> roi_centervec = GetROICenter(off);
	for (int i = 0; i < roi_centervec.size(); i++)
	{
		cv::Point pt = roi_centervec[i] + center_offset_;
		roi_center_.push_back(pt);
	}
    return roi_center_;
}


void NaiveRemoveNoise(cv::Mat& srcImage, double  pArea )
{
	pArea = double(srcImage.rows) / 2048.0 * pArea;
	int i, j;
	int color = 1;
	int nRows = srcImage.rows;
	int nCols = srcImage.cols;

	Mat img, img_edge, labels, centroids, img_color, stats;
	int nccomps = connectedComponentsWithStats(srcImage, labels, stats, centroids);


	for (i = 0; i < nRows; ++i) {
		for (j = 0; j < nCols; ++j) {
			if (srcImage.at<uchar>(i, j) == 255)
			{
				int label = labels.at<int>(i, j);
				if (stats.at<int>(label, CC_STAT_AREA) < pArea)
				{
					srcImage.at<uchar>(i, j) = 0;
				}
			}
			
		}
	}

}

void NaiveRemoveShortLine(cv::Mat& srcImage, cv::Mat& tmp, double pArea)
{
	pArea = double(srcImage.rows) / 2048.0 * pArea;
	int i, j;
	int color = 1;
	int nRows = srcImage.rows;
	int nCols = srcImage.cols;

	Mat img, img_edge, labels, centroids, img_color, stats;
	int nccomps = connectedComponentsWithStats(tmp, labels, stats, centroids);

	if (nccomps<3)
	{
		return;
	}
	Rect r; double area = INFINITY;
	for (int i = 0; i < nccomps; i++)
	{
		if (stats.at<int>(i, CC_STAT_AREA) < area)
		{
			area = stats.at<int>(i, CC_STAT_AREA);
			r = Rect(stats.at<int>(i, 0), stats.at<int>(i, 1), stats.at<int>(i, 2), stats.at<int>(i, 3));
		}
	}
	srcImage(r).setTo(0);


	/*for (i = 0; i < nRows; ++i) {
		for (j = 0; j < nCols; ++j) {
			if (tmp.at<uchar>(i, j) == 255)
			{
				int label = labels.at<int>(i, j);
				if (stats.at<int>(label, CC_STAT_AREA) < pArea)
				{
					srcImage.at<uchar>(i, j) = 0;
				}
			}

		}
	}*/
}