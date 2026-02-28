#pragma once
#include "CrossCenter.h"

using namespace std;

float STDVector(vector<float> resultSet)
{
	float sum = std::accumulate(std::begin(resultSet), std::end(resultSet), 0.0);
	float mean = sum / resultSet.size(); //均值

	float accum = 0.0;
	std::for_each(std::begin(resultSet), std::end(resultSet), [&](const double d) {
		accum += (d - mean)*(d - mean);
	});

	float stdev = sqrt(accum / (resultSet.size() - 1)); //方差
	return stdev;
}

CrossCenter::CrossCenter()
{
}

CrossCenter::~CrossCenter()
{
}

CrossCenter::CrossCenter(Mat img, int off, int size)
{
	this->img = img;
	this->size = size;
	this->off = off;
	//this->roiLen = roiLen;
	

}
void CrossCenter::thinningIteration(Mat& img, int iter)
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
	uchar *pAbove;
	uchar *pCurr;
	uchar *pBelow;
	uchar *nw, *no, *ne;    
	uchar *we, *me, *ea;
	uchar *sw, *so, *se;    

	uchar *pDst;

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

			if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)//腐蚀四个条件
				pDst[x] = 1;
		}
	}

	img &= ~marker;
}


void CrossCenter::thinning(const cv::Mat& src, cv::Mat& dst)
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
	} while (cv::countNonZero(diff) >0);//迭代终止条件

	dst *= 255;
}

double CrossCenter::locate_mid(Mat m, int mlen)
{

	double* img_data = (double *)malloc(mlen * sizeof(double));
	double* diff_data = (double *)malloc(mlen * sizeof(double));
	//img_data_free = img_data;

	for (int i = 0; i < mlen; i++)
	{
		double* data = (double*)m.data;
		img_data[i] = data[i];
	}
	diff_data[0] = 0;
	diff_data[mlen - 1] = 0;
	for (int i = 1; i < mlen-1; i++)
	{
		diff_data[i] = img_data[i - 1] - img_data[i + 1];
	}

	double max_index = max_element(diff_data, diff_data + mlen) - diff_data;
	double min_index = min_element(diff_data, diff_data + mlen) - diff_data;

	free(img_data);
	free(diff_data);
	return (max_index+ min_index)/2;
}

vector<double> line_fit(Mat abs_grad, int flag)
{
	vector<double> point_x, point_y;
	for (int j = 0; j < abs_grad.rows; j++)
	{
		uchar* ptr = abs_grad.ptr<uchar>(j);
		for (int i = 0; i < abs_grad.cols; i++)
		{
			if (ptr[i] > 0)
			{
				point_x.push_back(double(i));
				point_y.push_back(double(j));
			}

		}
	}
	Mat X,X0,Y;
	switch (flag)
	{
	case 0:
		Y = Mat(point_y);
		X0 = Mat(point_x);
		break;
	case 1:
		Y = Mat(point_x);
		X0 = Mat(point_y);
		break;
	}
	Mat X1 = Mat::ones(X0.size(), X0.type());
	hconcat(X0, X1, X);

	Mat X_inv = (X.t()*X).inv();
	Mat co = X_inv*X.t()*Y;
	vector<double> coe = (vector<double>)(co.reshape(1,1));
	return coe;
}


vector<double> line_fit_orthogonal(Mat abs_grad)
{
	vector<double> point_x, point_y;
	for (int j = 0; j < abs_grad.rows; j++)
	{
		uchar* ptr = abs_grad.ptr<uchar>(j);
		for (int i = 0; i < abs_grad.cols; i++)
		{
			if (ptr[i] > 0)
			{
				point_x.push_back(double(i));
				point_y.push_back(double(j));
			}

		}
	}

	Mat Y = Mat(point_y);
	Mat X = Mat(point_x);
	double n = point_x.size();
	double A = n*X.dot(Y) - sum(X)[0] * sum(Y)[0];
	double B = pow(sum(Y)[0], 2) - n*Y.dot(Y) - pow(sum(X)[0], 2) + n*X.dot(X);
	double C = sum(X)[0] * sum(Y)[0] - n*X.dot(Y);
	
	vector<double> coe(2);
	coe[0] = (-B + pow((B * 4 - 4 * A*C), 0.5)) / 2 / A;
	coe[1] = (sum(Y)[0] - coe[0] * sum(X)[0]) / n;

	return coe;
}




void CrossCenter::sepLines(vector<Vec4i>& lines, vector<Vec4i>& line1, vector<float>& ks1, vector<Vec4i>& line2, vector<float>& ks2)
{
	
	

	float k1 = float((lines[0][3] - lines[0][1])) / float((lines[0][2] - lines[0][0]));
	float err = 20;
	double angle1, angle2;


	line1.push_back(lines[0]);
	ks1.push_back(k1);
	//k1 = -INFINITY;
	angle1 = (atan(k1));
	m_angle1.push_back(angle1);
	for (int i = 1; i < lines.size(); i++)
	{
		float k2 = float((lines[i][3] - lines[i][1])) / float((lines[i][2] - lines[i][0]));
		angle2 = (atan(k2));
		//if ((k1 == 0 && (k2 == INFINITY || k2 == -INFINITY || k2>tan(90 - err) || k2<tan(90 + err)))||((k1 == INFINITY|| k1 == -INFINITY) && (k2 == 0 || (k2>tan(-err) && k2<tan(err))))||(k1*k2 <= -0.3 && k1*k2 >= -1.3))
		if (fabs(angle1 - angle2)<err / 180 * CV_PI || fabs(angle1 - angle2)>(180 - err / 2) / 180 * CV_PI)
		{
			ks1.push_back(k2);
			line1.push_back(lines[i]);
			m_angle1.push_back(angle2);
		}
		else
		{

			ks2.push_back(k2);
			line2.push_back(lines[i]);
			m_angle2.push_back(angle2);
		}


	}




}

int CrossCenter::calLine(vector<Vec4i>& lines, vector<float>& ks, float& a, float& b, float& c)
{
	int width;
	bool is_inf;
	float ksum = 0, asum = 0, bsum = 0, csum = 0;
	//float kk, bb;
	vector<float>::iterator ret1, ret2;


	ret1 = std::find(ks.begin(), ks.end(), INFINITY);
	ret2 = std::find(ks.begin(), ks.end(), -INFINITY);
	if (ret1 != ks.end() || ret2 != ks.end())
	{
		ksum = INFINITY;
		//is_inf = true;
	}
	else
	{
		ksum = std::accumulate(ks.begin(), ks.end(), 0.0);
		//is_inf = false;
	}

	ksum = ksum / lines.size();

	vector<float> A, B, C;
	if (abs(ksum) >1)
	{
		a = 1;
		float bb, cc;
		for (int i = 0; i < lines.size(); i++)
		{
			bb = -float((lines[i][2] - lines[i][0])) / float((lines[i][3] - lines[i][1]));
			B.push_back(bb);
			bsum += bb;
			//csum += cc;
		}
		b = bsum / lines.size();
		//b = (*max_element(B.begin(), B.end()) + *min_element(B.begin(), B.end())) / 2;
		for (int i = 0; i < lines.size(); i++)
		{
			cc = -(float(lines[i][0] + float(b*lines[i][1])) + float(lines[i][2] + float(b*lines[i][3]))) / 2;
			C.push_back(cc);
			//csum += cc;
		}



		c = (*max_element(C.begin(), C.end()) + *min_element(C.begin(), C.end())) / 2;

		//c = -838.6;
		//c = csum / lines.size();
	}
	else
	{
		b = 1;
		float aa, cc;
		for (int i = 0; i < lines.size(); i++)
		{
			aa = -float((lines[i][3] - lines[i][1])) / float((lines[i][2] - lines[i][0]));
			A.push_back(aa);
			asum += aa;
			//csum += cc;
		}
		a = asum / lines.size();
		//a = (*max_element(A.begin(), A.end()) + *min_element(A.begin(), A.end())) / 2;
		for (int i = 0; i < lines.size(); i++)
		{
			cc = -(float(lines[i][1] + float(a*lines[i][0])) + float(lines[i][3] + float(a*lines[i][2]))) / 2;
			C.push_back(cc);
			//csum += cc;
		}

		c = (*max_element(C.begin(), C.end()) + *min_element(C.begin(), C.end())) / 2;


		//c = csum / lines.size();
	}

	width = ceil(*max_element(C.begin(), C.end()) - *min_element(C.begin(), C.end()));
	return width;

}

Point2f CrossCenter::calLinesCross(float a1, float b1, float c1, float a2, float b2, float c2)
{
	Point2f cross;


	cross.x = -(c1*b2 - c2*b1) / (a1*b2 - a2*b1);
	cross.y = -(c1*a2 - c2*a1) / (b1*a2 - b2*a1);



	return cross;
}


void CrossCenter::find_center()
{
	Mat img_gray;
	if (this->img.channels() == 3)//转灰度图
		cvtColor(this->img, img_gray, CV_BGR2GRAY);
	else
		img_gray = this->img;
	Mat element1 = getStructuringElement(MORPH_RECT, Size(size, size));
	Mat element2 = getStructuringElement(MORPH_RECT, Size(25, 25));
	//Mat element = getStructuringElement(MORPH_RECT, Size(m_MorphSizeWidth, m_MorphSizeHeight));
	int iRow = img.rows;
	int iCol = img.cols;
	double minv = 0.0, maxv = 0.0;

	Point minp, maxp;
	minMaxLoc(img_gray, &minv, &maxv, &minp, &maxp);
	

	morphologyEx(img_gray, img_gray, MORPH_CLOSE, element1);
	threshold(img_gray, img_gray,(minv+maxv)/2, 255, CV_THRESH_BINARY);
	

	
	vector<Vec4i> lines;
	vector<Vec4i> lines1, lines2;
	vector<float> ks1, ks2;


	//HoughLinesP(img_gray, lines, 1, CV_PI / 180, 80, 150, 10);
	//if (!lines.empty())
	//{
	//	sepLines(lines, lines1, ks1, lines2, ks2);
	//	float a1, b1, c1, a2, b2, c2;

	//	if (!lines1.empty() && !lines2.empty())
	//	{
	//		calLine(lines1, ks1, a1, b1, c1);
	//		calLine(lines2, ks2, a2, b2, c2);

	//		center = calLinesCross(a1, b1, c1, a2, b2, c2);
	//		
	//	}

	//}
	//else
	//{
		Mat m;
		img_gray.convertTo(m, CV_64FC1);
		Mat m_row(1, iCol, CV_64FC1, Scalar(0));
		Mat m_col(iRow, 1, CV_64FC1, Scalar(0));


		reduce(m, m_row, 0, CV_REDUCE_SUM);
		reduce(m, m_col, 1, CV_REDUCE_SUM);

		double x = locate_mid(m_row, iCol);

		double y = locate_mid(m_col, iRow);
		center = Point2f(x, y);

//	}

	



	//circle(img, center, 3, Scalar(0, 0, 255), -1);

	//dilate(img_threshold, img_dilate, element1);//膨胀
	//
	//erode(img_dilate, img_erode, element2);

/*
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	/// Gradient X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel(img_gray, grad_x, CV_64F, 1, 0, 3, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);

	/// Gradient Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel(img_gray, grad_y, CV_64F, 0, 1, 3 ,1, 0, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);


	//thinning(img_gray, img_gray);
	vector<double> coe_x, coe_y;
	//coe1 = line_fit(abs_grad_x);
	//coe2 = line_fit(abs_grad_y);
	coe_x = line_fit(abs_grad_x,1);
	coe_y = line_fit(abs_grad_y,0);

	Point2f cen;
	cen.x = (coe_x[0] * coe_y[1] + coe_x[1]) / (1 - coe_x[0] * coe_y[0]);
	cen.y = coe_y[0] * cen.x + coe_y[1];
*/

//	dilate(img_threshold, img_dilate, element1);//膨胀
	//Canny(img_gray, img_gray, 150, 100, 3);
	//thinning(img_erode, img_erode);



	//circle(img, cen, 2, Scalar(0, 0, 255), -1);
	//circle(img, center, 1, Scalar(0, 255, 0), -1);

	//Mat draw_img(img_gray.size(), CV_8UC3);
	//for (int i = 0; i < lines.size(); i++)
	//{
	//	Vec4i l = lines[i];
	//	Scalar color = Scalar(rand() % 255, rand() % 255, rand() % 255);
	//	line(draw_img, Point(l[0], l[1]), Point(l[2], l[3]), color, 1);

	//}

}

bool CrossCenter::find_centerLINES()
{
	Mat img_gray;
	if (this->img.channels() == 3)//转灰度图
		cvtColor(this->img, img_gray, CV_BGR2GRAY);
	else
		img_gray = this->img.clone();
	Mat element1 = getStructuringElement(MORPH_RECT, Size(size, size));
	Mat element2 = getStructuringElement(MORPH_RECT, Size(5, 5));
	//Mat element = getStructuringElement(MORPH_RECT, Size(m_MorphSizeWidth, m_MorphSizeHeight));
	int iRow = img.rows;
	int iCol = img.cols;

	cout << img_gray.type() << endl;
	img_gray.convertTo(img_gray, CV_8UC1);

	Mat img_draw = img_gray.clone();
	
	morphologyEx(img_gray, img_gray, MORPH_CLOSE, element1);
	double minv = 0.0, maxv = 0.0;
	Point minp, maxp;
	minMaxLoc(img_gray, &minv, &maxv, &minp, &maxp);

	//threshold(img_gray, img_gray, (minv + maxv) / 2, 255, CV_THRESH_BINARY);
	threshold(img_gray, img_gray, 0, 255, CV_THRESH_OTSU);
	
	morphologyEx(img_gray, img_gray, MORPH_CLOSE, element1);
	//adaptiveThreshold(img_gray, img_gray, 255, ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY,3,1);
	vector<Vec4i> lines;
	vector<Vec4i> lines1, lines2;
	vector<float> ks1, ks2;
	//img_gray = 255 - img_gray;
	Mat m;
	img_gray.convertTo(m, CV_64FC1);
	Mat m_row(1, iCol, CV_64FC1, Scalar(0));
	Mat m_col(iRow, 1, CV_64FC1, Scalar(0));


	reduce(m, m_row, 0, CV_REDUCE_SUM);
	reduce(m, m_col, 1, CV_REDUCE_SUM);

	int numR = countNonZero(m_row);
	int numC = countNonZero(m_col);

	int l = (numR < numC ? numR : numC)/12;



	Canny(img_gray, img_gray, 125, 300);
	HoughLinesP(img_gray, lines, 1, CV_PI / 180, l, l, l);


	for (int i = 0; i < lines.size(); i++)
	{
		line(img_draw, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(255, 255, 255));
	}



	if (!lines.empty())
	{
		sepLines(lines, lines1, ks1, lines2, ks2);
		float a1, b1, c1, a2, b2, c2;

		if (!lines1.empty() && !lines2.empty())
		{
			int width1, width2;
			width1 = calLine(lines1, ks1, a1, b1, c1);
			width2 = calLine(lines2, ks2, a2, b2, c2);
			line_width = max(width1, width2);
			center = calLinesCross(a1, b1, c1, a2, b2, c2);

			roi_center = find_roi_center(center+ center_offset, a1, b1, c1, a2, b2, c2);


			return true;
		}
		else
			return false;

	}
	else
		return false;
}

vector<Point> CrossCenter::find_roi_center(Point2f center, float a1, float b1, float c1, float a2, float b2, float c2)
{
	vector<Point> rcenter;
	float k1 = fabs(a1 / b1);
	float k2 = fabs(a2 / b2);


	float kh = k1 < k2 ? a1 / b1 : a2 / b2;
	float kv = k1 > k2 ? a1 / b1 : a2 / b2;


	float anglev = atan(kv);
	float angleh = atan(kh);

	if (anglev > 0)
	{
		rcenter.push_back(Point(center.x + cos(anglev)*off, center.y - sin(anglev)*off));
		rcenter.push_back(Point(center.x - cos(anglev)*off, center.y + sin(anglev)*off));
	}
	else
	{
		rcenter.push_back(Point(center.x - cos(anglev)*off, center.y + sin(anglev)*off));
		rcenter.push_back(Point(center.x + cos(anglev)*off, center.y - sin(anglev)*off));
	}

	rcenter.push_back(Point(center.x - cos(angleh)*off, center.y + sin(angleh)*off));
	rcenter.push_back(Point(center.x + cos(angleh)*off, center.y - sin(angleh)*off));




	return rcenter;
}




void CrossCenter::find_centerMTACH(Mat templa)
{
	Mat img_gray;
	if (this->img.channels() == 3)//转灰度图
		cvtColor(this->img, img_gray, CV_BGR2GRAY);
	if (templa.channels() == 3)//转灰度图
		cvtColor(templa, templa, CV_BGR2GRAY);
	Mat result;
	int result_cols = img_gray.cols - templa.cols + 1;
	int result_rows = img_gray.rows - templa.rows + 1;




	result.create(result_rows, result_cols, CV_32FC1);


	//int t1 = clock();
	//    enum { TM_SQDIFF=0, TM_SQDIFF_NORMED=1, TM_CCORR=2, TM_CCORR_NORMED=3, TM_CCOEFF=4, TM_CCOEFF_NORMED=5 };
	matchTemplate(img_gray, templa, result, TM_CCOEFF_NORMED);   //最好匹配为1,值越小匹配越差

	//int t2 = clock();

	//cout << t2 - t1 << endl;
	double minVal = -1;
	double maxVal;
	Point minLoc;
	Point maxLoc;
	Point matchLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	//取大值(视匹配方法而定)
	//    matchLoc = minLoc;
	matchLoc = maxLoc;


	center = Point2f(matchLoc.x + templa.cols / 2, matchLoc.y + templa.rows / 2);

}





Point2f CrossCenter::get_crossCenterDIFF()
{
	find_center();
	return center;
}


Point2f CrossCenter::get_crossCenterLINES()
{
	if (find_centerLINES())
	{
		float std1, std2;
		std1 = STDVector(m_angle1);
		std2 = STDVector(m_angle2);

		if ((std1>0.01 || std2>0.01)&& line_width>0)
		{
			m_angle1.clear();
			m_angle2.clear();
			int roi_size = 10 * line_width;
			center_offset = center - Point2f(roi_size / 2, roi_size / 2);
			center_offset.x = center_offset.x < 0 ? 0 : center_offset.x;
			center_offset.y = center_offset.y < 0 ? 0 : center_offset.y;
			Rect ROI = Rect(center_offset, Size(roi_size, roi_size));
			Rect full_roi = Rect(0, 0, img.cols, img.rows);
			this->img = this->img(ROI & full_roi);
			find_centerLINES();
		}

		return center + center_offset;
	}
	else
		return Point2f(0, 0);
}


Point2f CrossCenter::get_crossCenterMATCH(Mat templa)
{
	find_centerMTACH(templa);
	return center;
}


vector<Point> CrossCenter::get_roi_center()
{
	return roi_center;
}

int CrossCenter::get_line_width()
{
	return line_width;
}