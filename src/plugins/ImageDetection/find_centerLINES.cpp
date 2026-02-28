#pragma once
//#include "pch.h"
#include "CrossCenter.h"
//#include"ImageCommonProcessing.h"
//#include <MLAlgorithm/ml_optpara_kunlun.h>
//#include"qlog.h"
using namespace std;
using namespace cv;
using namespace MLImageDetection;

void CrossCenter::sepLines(vector<Vec4i>& lines, vector<Vec4i>& line1, vector<float>& ks1, vector<Vec4i>& line2, vector<float>& ks2)
{
	float k1;
    int i = 0;
    for (i=0;i<lines.size();i++)
    {
        k1= float((lines[i][3] - lines[i][1])) / float((lines[i][2] - lines[i][0]));
        if (k1!=INFINITY&&k1!=-INFINITY)
        {
           break;
		}
        else
        {
            k1 = 1e3;
            break;
        }

    }
    float err = CrossCenterParameter::anglediff;
	double angle1, angle2;
    //cout << k1 << endl;
	line1.push_back(lines[0]);
	ks1.push_back(k1);
	//k1 = -INFINITY;
	angle1 = (atan(k1));
	m_angle1.push_back(angle1);
	for (i = i+1; i < lines.size(); i++)
	{
		float k2 = float((lines[i][3] - lines[i][1])) / float((lines[i][2] - lines[i][0]));
		angle2 = (atan(k2));
        if (k2 == INFINITY || k2 == -INFINITY)
        {
            //continue;
            k2 = 1e3;
		}
		//if ((k1 == 0 && (k2 == INFINITY || k2 == -INFINITY || k2>tan(90 - err) || k2<tan(90 + err)))||((k1 == INFINITY|| k1 == -INFINITY) && (k2 == 0 || (k2>tan(-err) && k2<tan(err))))||(k1*k2 <= -0.3 && k1*k2 >= -1.3))
		if ((fabs(angle1 - angle2) < err / 180 * CV_PI || fabs(angle1 - angle2) > (180 - err / 2) / 180 * CV_PI))
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
		/*	vector<float>::iterator it;
			for (it = ks1.begin(); it != ks1.end(); )
			{
				if (*it== -INFINITY|| *it == INFINITY)
				{
					it = ks1.erase(it);
				}

			}*/


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
	if (abs(ksum) > 1)
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
			cc = -(float(lines[i][0] + float(b * lines[i][1])) + float(lines[i][2] + float(b * lines[i][3]))) / 2;
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
			cc = -(float(lines[i][1] + float(a * lines[i][0])) + float(lines[i][3] + float(a * lines[i][2]))) / 2;
			C.push_back(cc);
			//csum += cc;
		}

		c = (*max_element(C.begin(), C.end()) + *min_element(C.begin(), C.end())) / 2;


		//c = csum / lines.size();
	}

	width = ceil(*max_element(C.begin(), C.end()) - *min_element(C.begin(), C.end()));
	return width;

}

int calLineVertical(vector<Vec4i>& lines, vector<float>& ks, float& a, float& b, float& c)
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
		vector<float> ksP, ksN;
		for (int i = 0; i < ks.size(); i++)
		{
			if (ks[i] > 0)
			{
				ksP.push_back(ks[i]);
			}
			else
			{
				ksN.push_back(ks[i]);
			}

		}
		if (ksP.size() > ksN.size())
		{
			ksum = std::accumulate(ksP.begin(), ksP.end(), 0.0) / ksP.size();
		}
		else
		{
			ksum = std::accumulate(ksN.begin(), ksN.end(), 0.0) / ksN.size();
		}

		//ksum = std::accumulate(ks.begin(), ks.end(), 0.0);
		//is_inf = false;
	}

	//ksum = ksum / lines.size();

	vector<float> A, B, C;
	if (abs(ksum) > 1)
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
			cc = -(float(lines[i][0] + float(b * lines[i][1])) + float(lines[i][2] + float(b * lines[i][3]))) / 2;
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
			cc = -(float(lines[i][1] + float(a * lines[i][0])) + float(lines[i][3] + float(a * lines[i][2]))) / 2;
			C.push_back(cc);
			//csum += cc;
		}

		c = (*max_element(C.begin(), C.end()) + *min_element(C.begin(), C.end())) / 2;


		//c = csum / lines.size();
	}

	width = ceil(*max_element(C.begin(), C.end()) - *min_element(C.begin(), C.end()));
	return width;
}

cv::Point2f CrossCenter::find_centerLINES(cv::Mat img)
{
    try
    {

        Mat img_gray1;
        if (img.channels() == 3)
            cvtColor(img, img_gray1, CV_BGR2GRAY);
        else
            img_gray1 =img.clone();
        cv::Mat img_gray;
        // still
        //img_gray = img_gray1.clone();
        // convert to 8bit
        if (img_gray1.depth() == CV_8U)
        {
            img_gray = img_gray1.clone();
            m_img_draw = img.clone();
        }
        else
        {
            img_gray = convertToUint8(img_gray1);
            m_img_draw = convertToUint8(img);
        }
		m_img_draw = convertTo3Channels(m_img_draw);
        Mat element1 = getStructuringElement(MORPH_RECT, Size(80, 80));
        Mat element2 = getStructuringElement(MORPH_RECT, Size(3, 3));
        // Mat element = getStructuringElement(MORPH_RECT, Size(m_MorphSizeWidth, m_MorphSizeHeight));
        int iRow = img.rows;
        int iCol = img.cols;
        Mat img_draw = img.clone();
        cv::medianBlur(img_gray, img_gray, 3);
        //morphologyEx(img_gray, img_gray, MORPH_CLOSE, element2);
     //   morphologyEx(img_gray, img_gray, MORPH_GRADIENT, element2);
        cv::Mat imgth;
        threshold(img_gray, img_gray, 0, 255, THRESH_OTSU);
		//threshold(img_gray, img_gray, 0, 255, THRESH_TRIANGLE);
	
        double minv = 0.0, maxv = 0.0;
        Point minp, maxp;
        cv::minMaxLoc(img_gray, &minv, &maxv, &minp, &maxp);

      //threshold(img_gray, img_gray, (minv + maxv) / 2, 255, CV_THRESH_BINARY);
      //   threshold(img_gray, img_gray, 0, 255, CV_THRESH_OTSU);
      // NaiveRemoveNoise(img_gray, 1000);
      //	cv::Canny(img_gray, img_gray, 30, 60);
      // morphologyEx(img_gray, img_gray, MORPH_CLOSE, element1);
      // adaptiveThreshold(img_gray, img_gray, 255, ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY,3,1);
        vector<Vec4i> lines;
        vector<Vec4i> lines1, lines2;
        vector<float> ks1, ks2;
        // img_gray = 255 - img_gray;
        Mat m;
        img_gray.convertTo(m, CV_64FC1);
        Mat m_row(1, iCol, CV_64FC1, Scalar(0));
        Mat m_col(iRow, 1, CV_64FC1, Scalar(0));

        reduce(m, m_row, 0, CV_REDUCE_SUM);
        reduce(m, m_col, 1, CV_REDUCE_SUM);

        int numR = countNonZero(m_row);
        int numC = countNonZero(m_col);
        //TODO: l is 6
        int l = (numR < numC ? numR : numC) / 9;
		// l = 3;
        Canny(img_gray, img_gray, 30, 90);
        HoughLinesP(img_gray, lines, 1, CV_PI / 180, l, l, l);
        for (int i = 0; i < lines.size(); i++)
        {
            line(m_img_draw, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(255, 0, 0));
        }

        if (!lines.empty())
        {
            sepLines(lines, lines1, ks1, lines2, ks2);
            float a1, b1, c1, a2, b2, c2;

            if (!lines1.empty() && !lines2.empty())
            {
                double mean1, mean2;
                mean1 = std::accumulate(ks1.begin(), ks1.end(), 0.0) / ks1.size();
                mean2 = std::accumulate(ks2.begin(), ks2.end(), 0.0) / ks2.size();
                int width1, width2;
               // width1 = calLine(lines1, ks1, a1, b1, c1);
                //width2 = calLine(lines2, ks2, a2, b2, c2);

                if (abs(mean1) > 0.2)
                {
                    width1 = calLineVertical(lines1, ks1, a1, b1, c1);
                }
                else
                {
                    width1 = calLine(lines1, ks1, a1, b1, c1);
                }
                if (abs(mean2) > 0.2)
                {
                    width2 = calLineVertical(lines2, ks2, a2, b2, c2);
                }
                else
                {
                    width2 = calLine(lines2, ks2, a2, b2, c2);
                }
                m_line_width = max(width1, width2);
				m_center = calLinesCross(a1, b1, c1, a2, b2, c2);
                if (abs(m_center.x > 1e-6) & abs(m_center.y) > 1e-6)
                {
                    double flag1 = abs(atan(-a1 / (b1+1e-6)));
                    double flag2 = abs(atan(-a2 / (b2+1e-6)));
                    if (flag1 > flag2)
                    {
                        m_a1 = a1;
                        m_b1 = b1;
                        m_c1 = c1;
                        m_a2 = a2;
                        m_b2 = b2;
                        m_c2 = c2;


                    }
                    else
                    {
                        m_a1 = a2;
                        m_b1 = b2;
                        m_c1 = c2;
                        m_a2 = a1;
                        m_b2 = b1;
                        m_c2 = c1;
                    }
                    m_thetaH = atan(-m_a2 / (m_b2 + 1e-6));
                    m_thetaV = atan(-m_a1 / (m_b1 + 1e-6));
					cv::Point p1, p2;
					p1.x=0;
					p1.y = -(m_a1 * p1.x + m_c1) / m_b1;
					p2.x = 1000;
					p2.y = -(m_a1 * p2.x + m_c1) / m_b1;
					cv::line(m_img_draw, p1, p2, Scalar(0, 0, 255), 1);

					p1.x = 0;
					p1.y = -(m_a2 * p1.x + m_c2) / m_b2;
					p2.x = 1000;
					p2.y = -(m_a2 * p2.x + m_c2) / m_b2;
					cv::line(m_img_draw, p1, p2, Scalar(0, 0, 255), 1);

					double ang1 = atan2(-m_a1, m_b1);
					double ang2= atan2(-m_a2, m_b2);
                    if (m_angle1.size() > 1 & m_angle2.size() > 1)
                    {
                        double angle1 = std::accumulate(m_angle1.begin(), m_angle1.end(), 0.0) / m_angle1.size();
                        double angle2 = std::accumulate(m_angle2.begin(), m_angle2.end(), 0.0) / m_angle2.size();
                        m_angle1.clear();
                        m_angle1.push_back(angle1);
                        m_angle2.clear();
                        m_angle2.push_back(angle2);
                    }
                    circle(m_img_draw, m_center, 2, cv::Scalar(255, 0, 0), -1);
                    roi_center = find_roi_center(m_center + center_offset, a1, b1, c1, a2, b2, c2);
                    for (int i = 0; i < roi_center.size(); i++)
                    {
                        circle(m_img_draw, roi_center[i], 5, cv::Scalar(255, 0, 0), -1);
                    }
                    return m_center;

                }
                else
                {
					m_center.x = 0; m_center.y = 0;
                    return m_center;
                }
            }
            else
                return m_center;
        }
        else
            return m_center;
    }
    catch (const std::exception& w)
    {
        //		QLog::GetInstance().qlog("CrossCenter find_centerLINES:%s/n", w.what());

        if (!this->m_img.empty())
        {
            imwrite("1.jpg", this->m_img);
        }
    }
}
