#include "ml_rectangleDetection.h"
#include"LogPlus.h"
MLImageDetection::RectangleDetection::RectangleDetection()
{
}

MLImageDetection::RectangleDetection::~RectangleDetection()
{
}

cv::Rect MLImageDetection::RectangleDetection::getSquareRect(cv::Mat img, int buffer, float height)
{

	cv::Mat img8 = convertToUint8(img);
	cv::Mat imgdraw = convertTo3Channels(img8);
	cv::Mat imgf;
	img.convertTo(imgf, CV_32FC1);
	CV_Assert(imgf.type() == CV_32F || imgf.type() == CV_64F);
	int H = imgf.rows, W = imgf.cols;
	// 取中间 ±5 行，按列平均 → crossX (长度 W)
	int r0 = H / 2 - 10, r1 = H / 2 + 10;
	r0 = std::max(0, r0); r1 = std::min(H, r1);
	cv::Mat bandRows = imgf.rowRange(r0, r1);
	cv::Mat crossX_mat;
	cv::reduce(bandRows, crossX_mat, 0, cv::REDUCE_AVG);
	std::vector<float> crossX(W);
	for (int j = 0; j < W; ++j)
		crossX[j] = static_cast<float>(crossX_mat.at<float>(0, j));
	int c0 = W / 2 - 10, c1 = W / 2 + 10;
	c0 = std::max(0, c0); c1 = std::min(W, c1);
	cv::Mat bandCols = imgf.colRange(c0, c1);
	cv::Mat crossY_mat;
	cv::reduce(bandCols, crossY_mat, 1, cv::REDUCE_AVG);
	std::vector<float> crossY(H);
	for (int i = 0; i < H; ++i)
		crossY[i] = static_cast<float>(crossY_mat.at<float>(i, 0));
	auto computeGrad2 = [&](const std::vector<float>& v) {
		int n = (int)v.size();
		std::vector<float> grad(n, 0.f);
		for (int i = 1; i < n - 1; ++i)
			grad[i] = (v[i + 1] - v[i - 1]) * 0.5f;
		grad[0] = v[1] - v[0];
		grad[n - 1] = v[n - 1] - v[n - 2];
		float mx = *std::max_element(grad.begin(), grad.end(),
			[](float a, float b) {return std::abs(a) < std::abs(b); });
		mx = std::abs(mx) > 0 ? std::abs(mx) : 1.f;
		for (auto& g : grad) {
			g = (g / mx) * (g / mx);
		}
		float m2 = *std::max_element(grad.begin(), grad.end());
		m2 = m2 > 0 ? m2 : 1.f;
		for (auto& g : grad) g /= m2;
		return grad;
	};

	auto gradX = computeGrad2(crossX);
	auto gradY = computeGrad2(crossY);

	// 检测峰值
	int distX = W / 8;
	int distY = H / 8;
	auto peaksX = findPeaks(gradX, height, distX);
	auto peaksY = findPeaks(gradY, height, distY);
	if (peaksX.size() < 2 || peaksY.size() < 2)
		return cv::Rect();

	// 应用 buffer
	peaksX[0] = std::min(peaksX[0] + buffer, W - 1);
	peaksX[1] = std::max(peaksX[1] - buffer, 0);
	peaksY[0] = std::min(peaksY[0] + buffer, H - 1);
	peaksY[1] = std::max(peaksY[1] - buffer, 0);

	cv::Rect rect;
	rect.x = peaksX[0];
	rect.y = peaksY[0];
	rect.width = peaksX[1] - peaksX[0];
	rect.height = peaksY[1] - peaksY[0];

	rectangle(imgdraw, rect, Scalar(0, 0, 255), 1);
	circle(imgdraw, cv::Point2f(peaksX[0], peaksY[0]), 5, Scalar(0, 0, 255), -1);
	circle(imgdraw, cv::Point2f(peaksX[1], peaksY[1]), 5, Scalar(0, 0, 255), -1);
	return rect;


}

cv::RotatedRect MLImageDetection::RectangleDetection::getRectangleBorder(cv::Mat img)
{
	RotatedRect re;
	string infoHeader = "---MLImageDetection log---RectangleDetectionALG---";
	if (img.data != NULL)
	{
		cv::Mat gray;
		cv::Mat img8 = convertToUint8(img);
		cv::Mat img_draw = convertTo3Channels(img8);
		cv::GaussianBlur(img, gray, Size(5, 5), 3, 3);
		Mat kernel = getStructuringElement(MORPH_RECT, Size(10, 10), Point(-1, -1));
		morphologyEx(gray, gray, MORPH_GRADIENT, kernel, Point(-1, -1));
		cv::Mat imgth;
		cv::threshold(gray, imgth, 0, 255, THRESH_TRIANGLE);
		vector<vector<Point>> contours;
		vector<Vec4i> hierachy;
		findContours(imgth, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		vector<cv::Point2f> centers;
		//  drawContours(img_draw, contours, -1, Scalar(0, 0, 255), 10);
		for (int i = 0; i < contours.size(); i++)
		{
			double area = cv::contourArea(contours[i]);
			cv::Rect rect0 = cv::boundingRect(contours[i]);
			cv::RotatedRect rectR = cv::minAreaRect(contours[i]);
			Point2f P[4];
			rectR.points(P);
			double w = Getdistance(P[0], P[1]);
			double h = Getdistance(P[1], P[2]);
			double ratio = min(w, h) / max(w, h);
			if (rect0.area() > 10e5 /*&& ratio>0.6 && ratio<1.2*/)
			{
				re = rectR;
				cv::line(img_draw, P[0], P[1], Scalar(0, 0, 255), 1);
				cv::line(img_draw, P[1], P[2], Scalar(0, 0, 255), 1);
				cv::line(img_draw, P[2], P[3], Scalar(0, 0, 255), 1);
				cv::line(img_draw, P[0], P[3], Scalar(0, 0, 255), 1);

				if (abs(rectR.angle) > 45)
				{
					double angle1 = atan((P[1].y - P[0].y) / (P[1].x - P[0].x + 1e-6)) * 180 / CV_PI;
					double angle2 = atan((P[1].y - P[2].y) / (P[1].x - P[2].x + 1e-6)) * 180 / CV_PI;
					if (abs(angle1) > abs(angle2))
						re.angle = angle2;
					else
						re.angle = angle1;
					std::swap(re.size.width, re.size.height);
				}
				m_img_draw = img_draw.clone();
				break;
			}
		}

	}
	else
	{
		string message = infoHeader + "The input image is NULL";
		LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
	}
	return re;
}

cv::Rect MLImageDetection::RectangleDetection::getSolidExactRect(cv::Mat img8, cv::Rect rect)
{
	cv::Rect rect0;
	cv::Mat imgdraw = convertTo3Channels(img8);
	cv::Point h1 = rect.tl() + Point(0, rect.height / 2);// 找四条边中点 左边中点
	cv::Point h2 = rect.tl() + Point(rect.width, rect.height / 2);
	cv::Point v1 = rect.tl() + Point(rect.width / 2, 0);
	cv::Point v2 = rect.tl() + Point(rect.width / 2, rect.height);

	circle(imgdraw, h1, 5, Scalar(0, 0, 255), -1);
	circle(imgdraw, h2, 5, Scalar(0, 0, 255), -1);
	circle(imgdraw, v1, 5, Scalar(0, 255, 0), -1);
	circle(imgdraw, v2, 5, Scalar(0, 255, 0), -1);

	cv::Mat hmat1 = img8(cv::Range(h1.y - 5, h1.y + 5), cv::Range(h1.x - 100, h1.x + 100)); //在裁剪高度10宽度200的矩形条带
	cv::Mat hmat2 = img8(cv::Range(h2.y - 5, h2.y + 5), cv::Range(h2.x - 100, h2.x + 100));
	cv::Mat vmat1 = img8(cv::Range(v1.y - 100, v1.y + 100), cv::Range(v1.x - 5, v1.x + 5));
	cv::Mat vmat2 = img8(cv::Range(v2.y - 100, v2.y + 100), cv::Range(v1.x - 5, v1.x + 5));
	cv::rotate(vmat1, vmat1, ROTATE_90_COUNTERCLOCKWISE); // 旋转竖向条带
	cv::rotate(vmat2, vmat2, ROTATE_90_COUNTERCLOCKWISE);
	cv::medianBlur(hmat1, hmat1, 3);
	cv::medianBlur(hmat2, hmat2, 3);
	cv::medianBlur(vmat1, vmat1, 3);
	cv::medianBlur(vmat2, vmat2, 3);

	cv::reduce(hmat1, hmat1, 0, REDUCE_AVG);// 二维压成一维 变成一行，按列求平均
	cv::reduce(hmat2, hmat2, 0, REDUCE_AVG);
	cv::reduce(vmat1, vmat1, 0, REDUCE_AVG);
	cv::reduce(vmat2, vmat2, 0, REDUCE_AVG);

	int hindex1 = findEdgePt(hmat1);  // 根据梯度找具体边界
	int hindex2 = findEdgePt(hmat2);
	int vindex1 = findEdgePt(vmat1);
	int vindex2 = findEdgePt(vmat2);
	cv::Point2f h1Upate(h1.x + hindex1 - 100, h1.y); // 条带索引是以100为中心点的，故都要-100，<100为左/上，..为右/下
	cv::Point2f h2Upate(h2.x + hindex2 - 100, h2.y);
	cv::Point2f v1Upate(v1.x, v1.y + vindex1 - 100);
	cv::Point2f v2Upate(v2.x, v2.y + vindex2 - 100);
	circle(imgdraw, h1Upate, 5, Scalar(255, 0, 255), -1);
	circle(imgdraw, h2Upate, 5, Scalar(255, 0, 255), -1);
	circle(imgdraw, v1Upate, 5, Scalar(255, 0, 255), -1);
	circle(imgdraw, v2Upate, 5, Scalar(255, 0, 255), -1);
	rect0.x = h1Upate.x;
	rect0.y = v1Upate.y;
	rect0.width = h2Upate.x - h1Upate.x;
	rect0.height = v2Upate.y - v1Upate.y;
	cv::rectangle(imgdraw, rect0, Scalar(0, 0, 255), 10);
	return rect0;
}

cv::Mat MLImageDetection::RectangleDetection::getImgdraw()
{
	return m_img_draw;
}

void MLImageDetection::RectangleDetection::writeSolidInfoToCSV(cv::RotatedRect rectR)
{
	string path1 = "./config/AlgConfig/slbInfo/solidInfo.csv";
	cv::Mat solidmat(cv::Size(5, 1), CV_32FC1);
	solidmat.at<float>(0, 0) = rectR.center.x;
	solidmat.at<float>(0, 1) = rectR.center.y;
	solidmat.at<float>(0, 2) = rectR.size.width;
	solidmat.at<float>(0, 3) = rectR.size.height;
	solidmat.at<float>(0, 4) = rectR.angle;
	writeMatTOCSV(path1, solidmat);
}

void MLImageDetection::RectangleDetection::readSolidInfoFromCSV(cv::RotatedRect& rectR)
{
	string path1 = "./config/AlgConfig/slbInfo/solidInfo.csv";
	cv::Mat solidmat;// (cv::Size(1, 5), CV_32FC1);
	solidmat = readCSVToMat(path1);
	rectR.center.x = solidmat.at<float>(0, 0);
	rectR.center.y = solidmat.at<float>(0, 1);
	rectR.size.width = solidmat.at<float>(0, 2);
	rectR.size.height = solidmat.at<float>(0, 3);
	rectR.angle = solidmat.at<float>(0, 4);
}
