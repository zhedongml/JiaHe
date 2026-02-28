#include "find_multiCrosshair.h"

MultiCrosshairCenter::MultiCrosshairCenter()
{
}

MultiCrosshairCenter::~MultiCrosshairCenter()
{
}

vector<cv::Point2f> MultiCrosshairCenter::getMultiCrosshairCenter(cv::Mat img, int flag)
{
	vector<cv::Point2f>centers;
	if (img.data)
	{
		if (flag == 1)
		{
			centers = multiCrosshairHoughLine(img);
		}
	}
	return centers;
}

vector<cv::Point2f> MultiCrosshairCenter::multiCrosshairHoughLine(cv::Mat img)
{	
		vector<cv::Point2f>center;
		if (img.data != NULL)
		{
			cv::Mat img_draw = img.clone();
			cv::Mat img_gray;
			if (img.channels() == 3)
				cvtColor(img, img_gray, COLOR_BGR2GRAY);
			else
				img_gray = img.clone();
			Mat element1 = getStructuringElement(MORPH_RECT, Size(10, 10));
			int iRow = img.rows;
			int iCol = img.cols;
			cv::GaussianBlur(img_gray, img_gray, cv::Size(5, 5), 0, 0);
			morphologyEx(img_gray, img_gray, MORPH_CLOSE, element1);
			double minv = 0.0, maxv = 0.0;
			Point minp, maxp;
			cv::minMaxLoc(img_gray, &minv, &maxv, &minp, &maxp);
			vector<Vec4i> lines;
			vector<Vec4i> lines1, lines2;
			vector<float> ks1, ks2;
			Mat m;
			img_gray.convertTo(m, CV_64FC1);
			Mat m_row(1, iCol, CV_64FC1, Scalar(0));
			Mat m_col(iRow, 1, CV_64FC1, Scalar(0));
			reduce(m, m_row, 0, CV_REDUCE_SUM);
			reduce(m, m_col, 1, CV_REDUCE_SUM);
			int numR = countNonZero(m_row);
			int numC = countNonZero(m_col);
			int l = (numR < numC ? numR : numC) / 18;
			Canny(img_gray, img_gray, 20, 60);
			HoughLinesP(img_gray, lines, 1, CV_PI / 180, l, l, l);
			vector<float>b1, b2;
			cv::Mat xImg = cv::Mat::zeros(img_gray.size(), CV_8UC1);
			cv::Mat yImg = cv::Mat::zeros(img_gray.size(), CV_8UC1);
			for (int i = 0; i < lines.size(); i++)
			{
				cv::Point point0;
				point0.x = lines[i][0];
				point0.y = lines[i][1];
				float k = float((lines[i][3] - lines[i][1])) / float((lines[i][2] - lines[i][0]));
				double b = point0.y - k * point0.x;
				if (abs(k) < 1)
				{
					ks1.push_back(k);
					b1.push_back(b);
					lines1.push_back(lines[i]);
					line(img_draw, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 1);
					line(xImg, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(255, 255, 255), 1);
				}
				else if (abs(k) > 5)
				{
					k = float((lines[i][2] - lines[i][0])) / float((lines[i][3] - lines[i][1]));
					b = -(k * point0.y - point0.x);
					ks2.push_back(k);
					b2.push_back(b);
					lines2.push_back(lines[i]);
					line(img_draw, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0, 255, 0), 1);
					line(yImg, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(255, 255, 255), 1);
				}
			}
			int row = xImg.rows;
			int col = xImg.cols;
			vector<cv::Point2f>corners;
			for (int i = 0; i < row; i++)
			{
				for (int j = 0; j < col; j++)
				{
					int x = xImg.at < uchar>(i, j);
					int y = yImg.at<uchar>(i, j);
					if (x == 255 & y == 255)
					{
						corners.push_back(cv::Point2f(j, i));
						circle(img_draw, cv::Point2f(j, i), 1, cv::Scalar(255, 0, 0), -1);
					}
				}
			}
			vector<cv::Point2f>corNew;
			corNew.push_back(corners[0]);
			vector<pointSet>corNewP;
			pointSet a;
			a.Numx1 = 1;
			a.xSum1 = a.xSum1 + corNew[0].x;
			a.Numy1 = 1;
			a.ySum1 = a.ySum1 + corNew[0].y;
			corNewP.push_back(a);
			for (int i = 1; i < corners.size(); i++)
			{
				for (int j = 0; j < corNew.size(); j++)
				{
					double dis = Getdistance(corners[i], corNew[j]);
					int flag = 0;
					if (dis < 50)
					{
						double x = (corners[i].x + corNew[j].x) / 2;
						double y = (corners[i].y + corNew[j].y) / 2;

						if (abs(corNew[j].x - corners[i].x) < 5)
						{
							corNewP[j].Numx1++;
							corNewP[j].xSum1 = corNewP[j].xSum1 + corners[i].x;
						}
						else if (abs(corNew[j].x - corners[i].x) >= 5)
						{
							corNewP[j].Numx2++;
							corNewP[j].xSum2 = corNewP[j].xSum2 + corners[i].x;
						}
						if (abs(corNew[j].y - corners[i].y) < 5)
						{
							corNewP[j].Numy1++;
							corNewP[j].ySum1 = corNewP[j].ySum1 + corners[i].y;
						}
						else if (abs(corNew[j].y - corners[i].y) >= 5)
						{
							corNewP[j].Numy2++;
							corNewP[j].ySum2 = corNewP[j].ySum2 + corners[i].y;
						}
						flag = 1;
					}
					if (flag == 0 & j == corNew.size() - 1)
					{
						corNew.push_back(corners[i]);
						pointSet tmp;
						//tmp.Numx1++;
					   //tmp.xSum1 = tmp.xSum1 + corners[i].x;
						//tmp.Numy1++;
					   // tmp.ySum1 = tmp.ySum1 + corners[i].y;
						corNewP.push_back(tmp);
					}
				}
			}
			for (int i = 0; i < corNew.size(); i++)
			{
				double x = (corNewP[i].xSum1 / corNewP[i].Numx1 + corNewP[i].xSum2 / corNewP[i].Numx2) / 2.0;
				double y = (corNewP[i].ySum1 / corNewP[i].Numy1 + corNewP[i].ySum2 / corNewP[i].Numy2) / 2.0;
				center.push_back(cv::Point2f(x, y));
				circle(img_draw, cv::Point2f(x, y), 2, cv::Scalar(255, 0, 255), -1);
			}
		}
		return center;
	
}
