#include "MTFRectangleDetect.h"
#include"LogPlus.h"
using namespace MLImageDetection;
using namespace cv;
MLImageDetection::MTFRectangleDetect::MTFRectangleDetect()
{
}

MLImageDetection::MTFRectangleDetect::~MTFRectangleDetect()
{
}


MTFRectRe MLImageDetection::MTFRectangleDetect::getMtfRectangleRect(const cv::Mat img)
{
	string info = "----getMtfRectangleRect-----";
	MTFRectRe re;
	if (img.data == NULL)
	{
		re.flag = false;
		re.errMsg = info + "Input image is null!!!";
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, re.errMsg.c_str());
		return re;
	}

	//cv::Mat img1=GetROIMat(img,m_rect);
	cv::Mat img8 = convertToUint8(img);
	cv::Mat imgdraw = convertTo3Channels(img8);
	cv::Mat grayPre = preProcess(img8);
	vector<double>xV, yV;
	vector<cv::Rect> rectVec;

	if (rectVec.size() != 9)
	{
		rectVec.clear();
		xV.clear();
		yV.clear();
		rectVec = findRectangleByTemplate(grayPre, imgdraw, xV, yV);
	}
	if (rectVec.size() != 9)
	{
		rectVec.clear();
		xV.clear();
		yV.clear(); 
		 rectVec = findRectangleByContour(grayPre, imgdraw, xV, yV);
	}

	if (rectVec.size() != 9)
	{
		re.flag = false;
		string message = info + "MTF Rectangle detection fail.";
		re.errMsg = message;
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
		return re;
	}
	vector<cv::Rect>rectVesSort;
	vector<cv::Point2f>ptsSort;
	map<string, cv::Rect>rectMap;
	
	//ptsSort为排序后的方格中心点坐标(从左到右)，rectVesSort为方格排序后外部取的大roi集合
	getSortData(xV,yV,rectVec,rectVesSort,ptsSort);
	

	//prepare for the flip and rotation 
	cv::Mat xmat(cv::Size(3,3),CV_32FC1,Scalar(0));
	cv::Mat ymat(cv::Size(3, 3), CV_32FC1,Scalar(0));

 	for (int i = 0; i < ptsSort.size(); i++)
	{
		int i0 = i /3;
		int j0 = i % 3;
		xmat.at<float>(i0, j0) = ptsSort[i].x;
		ymat.at<float>(i0, j0) = ptsSort[i].y;
	}


	updateRectsAndPoints(xmat,ymat, rectVesSort,ptsSort);

	//给每个方格设置标识
	updateRectMap(rectVesSort, rectMap);
	updateImgdraw(imgdraw, rectVesSort, ptsSort);

	vector<cv::Mat> roiVec;
	for (int i = 0; i < rectVesSort.size(); i++)
	{
		roiVec.push_back(img(rectVesSort[i]));
	}

	re.flag = true;
	re.rectVec = rectVesSort;
	re.roiVec = roiVec;
	re.rectMap = rectMap;
	re.img_draw = imgdraw;
	string message = info + "MTF Rectangle detection sucessfully.";
	LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
	return re;
}


cv::Mat MLImageDetection::MTFRectangleDetect::preProcess(cv::Mat img)
{
	cv::Mat gray = convertToGrayImage(img);
	//cv::medianBlur(gray, gray, 3);
	cv::GaussianBlur(gray, gray, Size(3, 3), 1, 0);
	//Ptr<CLAHE> clahe = createCLAHE(2.0, Size(10, 10));
	//clahe->apply(gray, gray);
	//Mat kernel = getStructuringElement(MORPH_RECT, Size(10, 10), Point(-1, -1));
	//morphologyEx(gray, gray, MORPH_DILATE, kernel, Point(-1, -1));

	return gray;
}

vector<cv::Rect> MLImageDetection::MTFRectangleDetect::findRectangleByContour(cv::Mat img, cv::Mat& imgdraw,vector<double>&xv, vector<double>& yv)
{
	vector<cv::Rect> rectVec;
	cv::Mat imgth;
	cv::threshold(img, imgth, 0, 255, THRESH_OTSU);
	vector<vector<Point>> contours;
	vector<Vec4i> hierachy;
	findContours(imgth, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); i++)
	{
		cv::Rect rect = cv::boundingRect(contours[i]);
		double lenRatio = double(max(rect.width, rect.height) / min(rect.width, rect.height));
		double area = cv::contourArea(contours[i]);
		double areaRatio = area / rect.area();
		if (lenRatio > 0.7 && lenRatio < 1.2 && areaRatio>0.7)
		{
			cv::rectangle(imgdraw, rect, cv::Scalar(0, 0, 255), 1);
			rectVec.push_back(rect);
			cv::Point2f center = getRectCenter(rect);
			xv.push_back(center.x);
			yv.push_back(center.y);
		}
	}

	return rectVec;
}

vector<cv::Rect> MLImageDetection::MTFRectangleDetect::findRectangleByTemplate(cv::Mat img, cv::Mat& imgdraw, vector<double>&xv, vector<double>& yv)
{
	vector<cv::Rect> rectVec;
	string dir = get_exe_dir() + m_filePath;
	cv::Mat templ = cv::imread(dir, 0);
	if (templ.data == NULL)
		return vector<cv::Rect>();
	int binNum = 1;
	vector<cv::Point2f> pts = matchTemplateMaxLocsMTF(img, templ, 0.6,binNum);
	int len = 550;
	for (int i = 0; i < pts.size(); i++)
	{
		cv::Rect rect(pts[i].x - len / 2, pts[i].y - len / 2,len,len);
		rectVec.push_back(rect);
		xv.push_back(pts[i].x);
		yv.push_back(pts[i].y);
		//rectangle(imgdraw, rect, Scalar(0, 0, 255), 2);
		circle(imgdraw, pts[i],1, Scalar(0, 255, 0),-1);
	}
	
	return rectVec;
}

vector<cv::Point2f> MLImageDetection::MTFRectangleDetect::matchTemplateMaxLocsMTF(cv::Mat img, cv::Mat templ, double score, int bin)
{
	vector<cv::Point2f> pts;
	//vector<cv::Point2f> pts;
	cv::Mat img_draw = convertTo3Channels(img);
	cv::Mat img_result;
	matchTemplate(img, templ, img_result, TM_CCOEFF_NORMED);
	cv::Mat imgre;
	threshold(img_result, imgre, score, 255, THRESH_BINARY);
	imgre.convertTo(imgre, CV_8UC1);
	vector<vector<Point>> contours;
	vector<Vec4i> hierachy;
	findContours(imgre, contours, hierachy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	vector<Vec4i> hierachy_new;
	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i], false);
		if (area > 1e3 / pow(bin, 2) & area < 1e5 / pow(bin, 2))
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

void MLImageDetection::MTFRectangleDetect::updateImgdraw(cv::Mat& imgdraw, vector<cv::Rect> rectVec, vector<cv::Point2f> ptsVec)
{
	string str[9] = {"TL","TM","TR","ML","CEN","MR","BL","BM","BR"};
	for (int i = 0; i < rectVec.size(); i++)
	{
		//rectangle(imgdraw, rectVec[i], Scalar(0, 0, 255));
		circle(imgdraw, ptsVec[i], 5, Scalar(0, 0, 255), -1);
		string text =to_string(i)+"_" + str[i];
		cv::putText(imgdraw, text, rectVec[i].br(), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 1);
	}
}
void MLImageDetection::MTFRectangleDetect::getSortData(vector<double> xV, vector<double> yV, vector<cv::Rect>rectVec,vector<cv::Rect>& rectVesSort, vector<cv::Point2f>& ptsSort)
{
	arma::vec xvec(xV); //原始xV集合
	arma::vec yvec(yV); //原始yV集合
	arma::vec xsort(xvec.size());
	arma::vec ysort(yvec.size());
	xsort = arma::sort(xvec); //存放xV排序后的集合
	ysort = arma::sort(yvec); //存放yV排序后的集合
	for (int i = 0; i < xvec.size(); i = i + 3)
	{
		for (int j = 0; j < xvec.size(); j = j + 3)
		{
			double x = xsort[j];
			double y = ysort[i];
			arma::vec subx = xvec - x;
			arma::vec suby = yvec - y;
			arma::vec dis = arma::sqrt(subx % subx + suby % suby);
			arma::uvec list = find(dis == arma::min(dis));
			ptsSort.push_back(cv::Point2f(xvec[list[0]], yvec[list[0]]));
			rectVesSort.push_back(updateRectByRatio(rectVec[list[0]], 1.5));
		}
	}
}

void MLImageDetection::MTFRectangleDetect::updateRectsAndPoints(cv::Mat xmat, cv::Mat ymat, vector<cv::Rect>& rectVec, vector<cv::Point2f>& ptsVec)
{
	ptsVec.clear();
	rectVec.clear();
	int len = m_ROILen;
	for (int i = 0; i < xmat.rows; i++)
	{
		for (int j = 0; j < xmat.cols; j++)
		{
			double x0 = xmat.at<float>(i, j);
			double y0 = ymat.at<float>(i, j);
			ptsVec.push_back(cv::Point2f(x0,y0));
			cv::Rect rect0(x0 - len / 2, y0 - len / 2, len, len);
			rectVec.push_back(rect0);
		}
	}
}

void MLImageDetection::MTFRectangleDetect::updateRectMap(vector<cv::Rect> rectVec, map<string, cv::Rect> &rectMap)
{
	if (rectVec.size() == 9)
	{
		rectMap.insert(std::make_pair("TL", rectVec[0]));
		rectMap.insert(std::make_pair("TM", rectVec[1]));
		rectMap.insert(std::make_pair("TR", rectVec[2]));
		rectMap.insert(std::make_pair("ML", rectVec[3]));
		rectMap.insert(std::make_pair("Center", rectVec[4]));
		rectMap.insert(std::make_pair("MR", rectVec[5]));
		rectMap.insert(std::make_pair("BL", rectVec[6]));
		rectMap.insert(std::make_pair("BM", rectVec[7]));
		rectMap.insert(std::make_pair("BR", rectVec[8]));
	}
}





