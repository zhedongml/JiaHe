#include "MLGhostRectangleDetect.h"
#include"LogPlus.h"
using namespace algorithm;
using namespace cv;
algorithm::GhostRectangleDetect::GhostRectangleDetect()
{
}

algorithm::GhostRectangleDetect::~GhostRectangleDetect()
{
}

GhostRectRe algorithm::GhostRectangleDetect::getGhostRectangleRect(const cv::Mat img, cv::Rect rect)
{
	string info = "----getGhostRectangleRect-----";
	GhostRectRe re;
	if (img.data == NULL)
	{
		re.flag = false;
		re.errMsg = info + "Input image is null!!!";
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}
	cv::Mat img8 = convertToUint8(img);
	cv::Mat imgdraw = convertTo3Channels(img8);
	cv::Mat grayPre = preProcess(img8);
	vector<Rect>rectVec=findRectangleByContour(grayPre,imgdraw,rect);
	if (rectVec.size() != 5)
	{
		rectVec = findRectangleByTemplate(grayPre, imgdraw, rect);
	}
	if (rectVec.size() != 5)
	{
		re.flag = false;
		re.errMsg = "The ghost rectangle detection fail!";
		return re;
	}
	vector<cv::Rect>rectVesSort;
	re.rectVec = rectVec;
	re.ghostRectVec = rectVec;
	re.imgdraw = imgdraw;
	return re;
}

vector<cv::Rect> algorithm::GhostRectangleDetect::findRectangleByContour(cv::Mat img, cv::Mat& imgdraw,cv::Rect rectBoun)
{
	Mat kernel = getStructuringElement(MORPH_RECT, Size(30, 30), Point(-1, -1));
	morphologyEx(img, img, MORPH_DILATE, kernel, Point(-1, -1));
	vector<cv::Rect> rectVec;
	cv::Mat imgth;
	cv::threshold(img, imgth, 0, 255, THRESH_OTSU);
	vector<vector<Point>> contours;
	vector<Vec4i> hierachy;
	findContours(imgth, contours, hierachy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); i++)
	{
		cv::Rect rect = cv::boundingRect(contours[i]);
		double lenRatio = double(max(rect.width, rect.height) / min(rect.width, rect.height));
		double area = cv::contourArea(contours[i]);
		double areaRatio = area / rect.area();
		if (lenRatio > 0.8 && lenRatio < 1.2 && areaRatio>0.8&&area>1e4&&area<9e4/*&&containRect(rectBoun,rect)*/)
		{
			cv::rectangle(imgdraw, rect, cv::Scalar(0, 0, 255), 1);
			rectVec.push_back(rect);
			cv::Point2f center = getRectCenter(rect);
		}
	}
	return rectVec;

	
}

vector<cv::Rect> algorithm::GhostRectangleDetect::findRectangleByTemplate(cv::Mat img, cv::Mat& imgdraw, cv::Rect rectBoun)
{
	
		vector<cv::Rect> rectVec;
		string dir = get_exe_dir() + m_filePath;
		cv::Mat templ = cv::imread(dir, 0);
		if (templ.data == NULL)
			return vector<cv::Rect>();
		int binNum = 1;
		vector<cv::Point> pts = matchTemplateMaxLocs(img, templ, 0.6, binNum);
		int len = 280;
		for (int i = 0; i < pts.size(); i++)
		{
			cv::Rect rect(pts[i].x - len / 2, pts[i].y - len / 2, len, len);
			//if (containRect(rectBoun, rect))
			{
				rectVec.push_back(rect);
				rectangle(imgdraw, rect, Scalar(0, 0, 255), 2);
				circle(imgdraw, pts[i], 1, Scalar(0, 255, 0), -1);
			}
		}
		return rectVec;
}

cv::Mat algorithm::GhostRectangleDetect::preProcess(cv::Mat img)
{
    cv::Mat gray = convertToGrayImage(img);
	cv::medianBlur(gray, gray, 3);
	//  cv::GaussianBlur(gray, gray, Size(3, 3), 1, 0);
	Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
	//  clahe->apply(gray, gray);
	return gray;
}

vector<cv::Point> algorithm::GhostRectangleDetect::matchTemplateMaxLocs(cv::Mat img, cv::Mat templ, double score, int bin)
{
	
		vector<cv::Point> pts;
		cv::Mat img_draw = convertTo3Channels(img);
		cv::Mat img_result;
		matchTemplate(img, templ, img_result, TM_CCOEFF_NORMED);
		cv::Mat imgre;
		threshold(img_result, imgre, score, 255, THRESH_BINARY);
		imgre.convertTo(imgre, CV_8UC1);
		Mat kernel = getStructuringElement(MORPH_RECT, Size(30, 30), Point(-1, -1));
		morphologyEx(imgre, imgre, MORPH_DILATE, kernel, Point(-1, -1));

		vector<vector<Point>> contours;
		vector<Vec4i> hierachy;
		findContours(imgre, contours, hierachy, RETR_TREE, CHAIN_APPROX_SIMPLE);
		vector<Vec4i> hierachy_new;
		for (int i = 0; i < contours.size(); i++)
		{
			double area = contourArea(contours[i], false);
			if (area > 1e2 / pow(bin, 2) & area < 1e3 / pow(bin, 2))
			{
				// cout << area << endl;
				drawContours(img_draw, contours, i, Scalar(rand() * 255, rand() * 255, rand() * 255), 3, 8);
				Rect rect = boundingRect(contours[i]);
				rectangle(img_draw, rect, Scalar(255, 0, 255), 2);
				cv::Mat roi1 = img_result(rect);
				double minval, maxval;
				Point minloc, maxloc;
				cv::minMaxLoc(roi1, &minval, &maxval, &minloc, &maxloc);
				cv::Point2f loc = cv::Point2f(maxloc + cv::Point(rect.x, rect.y));
				cv::Rect rect1 = cv::Rect(loc.x, loc.y, templ.rows, templ.cols);
				rectangle(img_draw, rect1, Scalar(0, 0, 255), 2);
				cv::Mat roi = img(rect1).clone();
				Scalar m, std;
				cv::meanStdDev(roi, m, std);
				if (std(0) > 2)
				{
					cv::Point2f loc1 = loc + cv::Point2f(templ.size() / 2);
					pts.push_back(loc1);
					circle(img_draw, loc1, 5, Scalar(255, 0, 255), -1);
				}
			}
		}
		return pts;

}

void algorithm::GhostRectangleDetect::getSortData(vector<cv::Rect> rectVec, vector<cv::Rect>& rectVecSort)
{
	arma::vec yvec(rectVec.size());
	arma::vec xvec(rectVec.size());
	for (int i = 0; i < rectVec.size(); i++)
	{
		xvec[i] = rectVec[i].x;
		yvec[i] = rectVec[i].y;
	}
	arma::vec xsort(xvec.size());
	arma::vec ysort(yvec.size());
	xsort = arma::sort(xvec);
	ysort = arma::sort(yvec);
}
