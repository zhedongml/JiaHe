#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include<numeric> // accumulate定义的头文件
#include "ml_deadpointDetect.h"
#include<string.h>
using namespace algorithm;

algorithm::MLDeadPointDetect::MLDeadPointDetect()
{
}

algorithm::MLDeadPointDetect::~MLDeadPointDetect()
{
}

DeadPointRe algorithm::MLDeadPointDetect::DeadPoints(cv::Mat img,double a)
{    
        DeadPointRe m_Deadpoint;
        if (img.data != NULL)
        {
            cv::Mat img_gray;
            if (img.channels() == 3)
            {
                cvtColor(img, img_gray, CV_BGR2GRAY);
            }
            else
                img_gray = img.clone();
         //   double m_FocalLength = KunLunParameters::FocalLength;
           // double m_pixel_size = m_para.pixel_size;
           // double a = m_FocalLength * tan(0.02 * CV_PI / 180) / m_pixel_size;
            cv::Mat srcbinary;
            cv::Scalar mean;
            mean = cv::mean(img_gray);
            if (mean(0) > 10)
            {
                threshold(img_gray, srcbinary, mean(0) * 0.95, 255, THRESH_BINARY);
            }
            else
            {
                threshold(img_gray, srcbinary, mean(0) * 0.5, 255, THRESH_BINARY);
            }
            threshold(img_gray, srcbinary, 0, 255, THRESH_OTSU);

            Mat srcbinary8, img_draw;
            if (srcbinary.depth() == CV_8U)
            {
                srcbinary8 = srcbinary.clone();
                img_draw = img.clone();
            }
            else
            {
                srcbinary8 = convertToUint8(srcbinary);
                img_draw = convertToUint8(img);
            }
            std::vector<std::vector<cv::Point>> contours;
            std::vector<cv::Vec4i> hierarchy;
            cv::findContours(srcbinary8, contours, hierarchy, CV_RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
            for (int i = 0; i < contours.size(); i++)
            {
                double area = contourArea(contours[i], false);
                // cout << area << endl;
                if (area > (a * a))
                {
                    drawContours(img_draw, contours, i, Scalar(255, 0, 0), 1, 8, hierarchy);
                    Point2f pc;
                    float r;
                    cv::minEnclosingCircle(contours[i], pc, r);
                    cv::circle(img_draw, pc, r, Scalar(0, 0, 255), 1);
                    cv::circle(img_draw, pc, 2, Scalar(255, 0, 0), -1);
                    // cout << pc << r << endl;
                    m_Deadpoint.locALL.push_back(pc);
                    m_Deadpoint.allAreaNum++;
                }
            }
            // for (int i = 0; i < m_Deadpoint.allAreaNum; ++i)
            //{
            //    cv::Point2f num = m_Deadpoint.locALL[i];
            //    cv::putText(img_draw, std::to_string(i), num, cv::FONT_HERSHEY_SIMPLEX, 0.45, Scalar(255, 230,
            //    0), 1.8);
            //}
            // m_Deadpoint.MarkerImg = img_draw;
        }
        return m_Deadpoint;
    
}

DeadPointRe algorithm::MLDeadPointDetect::DeadPointsDetectHist(cv::Mat img)
{
    
        cv::Mat img_draw = img.clone();
    //   img_draw = convert2ColorImage(img_draw);
        DeadPointRe m_Deadpoint;
        if (img.data != NULL)
        {
            cv::Mat img_new;
            img_new = img.clone();
            cv::Mat img_gray;
            if (img_new.channels() == 3)
                cvtColor(img_new, img_gray, cv::COLOR_BGR2GRAY);
            else
                img_gray = img_new.clone();

            cv::Mat meanMat, stdMat;
            cv::meanStdDev(img_gray, meanMat, stdMat);
            img_gray.convertTo(img_gray, CV_32FC1);
            double mean; // = meanMat.at<float>(0);
            double std;  // = stdMat.at<float>(0);
            mean = double(meanMat.at<double>(0));
            std = double(stdMat.at<double>(0));
            int row = img_gray.rows;
            int col = img_gray.cols;
            cout << mean + 10 * std << endl;
            for (int i = 0; i < row; i++)
            {
                for (int j = 0; j < col; j++)
                {
                    float temp;                       
                    temp = float(img_gray.at<float>(i, j));                   
                    // cout << mean - 3 * std <<","<< mean + 3 * std << endl;
                   
                    if ((temp < mean - 10 * std || temp > mean + 10 * std) & (temp < mean * 0.05 || temp > mean * 0.5))
                    {
                        m_Deadpoint.allAreaNum++;
                        m_Deadpoint.locALL.push_back(cv::Point(j, i));
                        cv::circle(img_draw, Point(j, i), 1, cv::Scalar(255, 0, 255), 1);
                    }
                }
            }
        }
        string name = "E:\\project\\hydrus2Imgs\\hotpixel\\hotpixelRE.tif";
        imwrite(name, img_draw);
        return m_Deadpoint;   
}

DeadPointRe algorithm::MLDeadPointDetect::DeadPointsDetectThreshold(cv::Mat img)
{
    
        DeadPointRe re;
        cv::Mat img_draw = img.clone();
        if (img.data != NULL)
        {
            cv::Mat img_gray;
            if (img.channels() == 3)
                cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
            else
                img_gray = img.clone();
            cv::Mat meanMat, stdMat;
            cv::meanStdDev(img_gray, meanMat, stdMat);
            img_gray.convertTo(img_gray, CV_32FC1);
            double mean; // = meanMat.at<float>(0);
            double std;  // = stdMat.at<float>(0);
            mean = double(meanMat.at<double>(0));
            std = double(stdMat.at<double>(0));

            vector<cv::Point> loc;
            vector<double> value;
            for (int i = 0; i < img.rows; i++)
            {
                for (int j = 0; j < img.cols; j++)
                {
                    float tmp;                                  
                     tmp = float(img_gray.at<float>(i, j));
                                    
                    if ((tmp > mean + std || tmp < mean - std) & (tmp < mean * 0.05 || tmp > mean * 0.5))
                    {
                        loc.push_back(cv::Point(j, i));
                        value.push_back(tmp);
                    }
                }
            }
            double max = *max_element(value.begin(), value.end());
            double min = *min_element(value.begin(), value.end());
            double thresh = max - min;
            vector<double> valNew;
            for (int i = 0; i < value.size() - 1; i++)
            {
                double tmp = abs(value[i + 1] - value[i]);
                valNew.push_back(tmp);
            }
            double maxValue = *max_element(valNew.begin(), valNew.end());
            for (int i = 0; i < img_gray.rows; i++)
            {
                for (int j = 0; j < img_gray.cols; j++)
                {

                    // float tmp = img_gray.at<float>(i, j);
                    float tmp;
                                
                    tmp = float(img_gray.at<float>(i, j));
                    int maxB = int((mean + std + thresh) * 10 + 5) / 10;
                    int minB = int((mean - std - thresh) * 10 + 5) / 10;
                    if ((tmp > maxB || tmp < minB) & (tmp < mean * 0.05 || tmp > mean * 0.5))
                    {
                        re.locALL.push_back(cv::Point(j, i));
                        re.allAreaNum++;
                        cv::circle(img_draw, Point(j, i), 1, cv::Scalar(0, 0, 255), 1);
                    }
                }
            }
        }

        return re;
    
}

DeadPointRe algorithm::MLDeadPointDetect::DeadPointsDetectPinto(cv::Mat img)
{   
        DeadPointRe re;
        cv::Mat img_draw = img.clone();
        if (img.data != NULL)
        {
            cv::Mat imgGray;
            if (img.channels() == 3)
                cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);
            else
                imgGray = img.clone();
            cv::Scalar mean0;
            mean0 = cv::mean(imgGray);
            cv::Mat imgExp = ExpandMat(imgGray, 1);

            imgExp.convertTo(imgExp, CV_32FC1);
            int row = imgExp.rows;
            int col = imgExp.cols;
            for (int i = 0; i < row - 3; i++)
            {
                for (int j = 0; j < col - 3; j++)
                {
                    cv::Mat tmp = imgExp(cv::Range(i, i + 3), cv::Range(j, j + 3));
                    double midVal;                          
                     midVal = tmp.at<float>(1, 1);                                
                    cv::Mat meanMat, stdMat;
                    cv::meanStdDev(tmp, meanMat, stdMat);
                    double mean = meanMat.at<double>(0);
                    double std = stdMat.at<double>(0);
                    int flagN = 0;
                    int flagP = 0;
                    vector<double> subVec;
                    int i0, j0;
                    for (i0 = 0; i0 < 3; i0++)
                    {
                        for (j0 = 0; j0 < 3; j0++)
                        {
                            double sub = 0;                                      
                            sub = tmp.at<float>(i0, j0) - midVal;                           
                            if (i0 == 1 & j0 == 1)
                            {
                                continue;
                            }
                            else
                            {
                                subVec.push_back(abs(sub));
                            }
                            if (sub < 0)
                            {
                                flagN++;
                            }
                            else if (sub > 0)
                            {
                                flagP++;
                            }
                        }
                    }
                    if (flagP == 8 || flagN == 8)
                    {
                        double min = *min_element(subVec.begin(), subVec.end());
                        bool isB = false;
                        if (i - 1 == 0 || i - 1 == row || j - 1 == 0 || j - 1 == col)
                        {
                            isB = true;
                        }
                        float temp;                            
                         temp = float(tmp.at<float>(i, j));                       
                        // thresh value 30 is from the matlab, this can deduce the point
                        if ((min > 30) /*&!isB*/ /*min<mean - 1 * std || min>mean + 1 * std*/ &
                            (temp < mean0(0) * 0.05 || temp > mean0(0) * 0.5))
                        {
                            re.allAreaNum++;
                            re.locALL.push_back(Point(j, i));
                            cv::circle(img_draw, Point(j, i), 1, cv::Scalar(0, 0, 255), 1);
                        }
                    }
                }
            }
        }
        return re;
    
}

DeadPointRe algorithm::MLDeadPointDetect::DeadPointsDetectMedian(cv::Mat img)
{   
        DeadPointRe re;
        double th1 = 40, th2 = 30;
        cv::Mat img_draw = img.clone();
        if (img.data != NULL)
        {
            cv::Mat imgGray;
            if (img.channels() == 3)
                cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);
            else
                imgGray = img.clone();
            cv::Scalar mean0;
            mean0 = cv::mean(imgGray);
            cv::Mat imgExp = ExpandMat(img, 1);
            imgExp.convertTo(imgExp, CV_32FC1);
            int row = imgExp.rows;
            int col = imgExp.cols;
            for (int i = 0; i < row - 3; i++)
            {
                for (int j = 0; j < col - 3; j++)
                {
                    cv::Mat tmp = imgExp(cv::Range(i, i + 3), cv::Range(j, j + 3));
                    double Pc = 0;

                    
                        Pc = tmp.at<float>(1, 1);
                    

                    bool isB = false;
                    if (i - 1 < 0 || i - 1 == row || j - 1 < 0 || j - 1 == col)
                    {
                        isB = true;
                    }
                    if (Pc < 15)
                    {
                        cv::Mat sub = cv::Mat::zeros(cv::Size(3, 3), CV_8UC1);
                        sub = tmp - Pc;
                        sub = sub - th1;
                        int numB = cv::countNonZero(sub);
                        if (numB == 8 /*&!isB*/)
                        {
                            re.allAreaNum++;
                            re.locALL.push_back(Point(j, i));
                            cv::circle(img_draw, Point(j, i), 1, cv::Scalar(0, 0, 255), 1);
                        }
                    } //
                    else if (Pc > 236)
                    {
                        cv::Mat sub1;
                        sub1 = Pc - tmp;
                        sub1 = sub1 - th2;
                        int numS = cv::countNonZero(sub1);
                        if (numS == 8 /*&!isB*/)
                        {
                            re.allAreaNum++;
                            re.locALL.push_back(Point(j, i));
                            cv::circle(img_draw, Point(j, i), 1, cv::Scalar(0, 0, 255), 1);
                        }
                    }
                    else
                    {
                        double P1 = 0, P2 = 0, P3 = 0, P4 = 0, P5 = 0, P6 = 0, P7 = 0, P8 = 0;                       
                            P1 = tmp.at<float>(0, 0);
                            P2 = tmp.at<float>(0, 1);
                            P3 = tmp.at<float>(0, 2);
                            P4 = tmp.at<float>(1, 0);
                            P5 = tmp.at<float>(1, 2);
                            P6 = tmp.at<float>(2, 0);
                            P7 = tmp.at<float>(2, 1);
                            P8 = tmp.at<float>(2, 2);
                        
                        double Dh1 = abs(P1 + P3 - 2 * P2);
                        double Dh2 = abs(P4 + P5 - 2 * Pc);
                        double Dh3 = abs(P6 + P8 - 2 * P7);
                        double Dv1 = abs(P1 + P6 - 2 * P4);
                        double Dv2 = abs(P2 + P7 - 2 * Pc);
                        double Dv3 = abs(P3 + P8 - 2 * P5);
                        double D45_1 = abs(2 * (P4 - P2));
                        double D45_2 = abs(P3 + P6 - 2 * Pc);
                        double D45_3 = abs(P7 - P4);
                        double D135_1 = abs(2 * (P2 - P5));
                        double D135_2 = abs(P1 + P8 - 2 * Pc);
                        double D135_3 = abs(2 * (P7 - P4));
                        double Dh_median = median(Dh1, Dh2, Dh3);
                        double Dv_median = median(Dv1, Dv2, Dv3);
                        double D45_median = median(D45_1, D45_2, D45_3);
                        double D135_median = median(D135_1, D135_2, D135_3);
                        vector<double> medVec;
                        medVec.push_back(Dh_median);
                        medVec.push_back(Dv_median);
                        medVec.push_back(D45_median);
                        medVec.push_back(D135_median);
                        int minIndex = min_element(medVec.begin(), medVec.end()) - medVec.begin();
                        switch (minIndex)
                        {
                        case 1: {
                            if (Dh2 > 4 * (Dh1 + Dh3))
                            {
                                re.allAreaNum++;
                                re.locALL.push_back(Point(j, i));
                                cv::circle(img_draw, Point(j, i), 1, cv::Scalar(0, 0, 255), 1);
                            }
                            break;
                        }
                        case 2: {

                            if (Dv2 > 4 * (Dv1 + Dv3))
                            {
                                re.allAreaNum++;
                                re.locALL.push_back(Point(j, i));
                                cv::circle(img_draw, Point(j, i), 1, cv::Scalar(0, 0, 255), 1);
                            }
                            break;
                        }
                        case 3: {

                            double D135_sum = abs(D135_1 - D135_2) + abs(D135_1 - D135_3) + abs(D135_1 - D135_3);
                            if (D135_sum < 100)
                            { //坏点
                                if (D45_2 > 3 * (D45_1 + D45_3) && D135_2 > 3 * (D135_1 + D135_3))
                                {
                                    re.allAreaNum++;
                                    re.locALL.push_back(Point(j, i));
                                    cv::circle(img_draw, Point(j, i), 1, cv::Scalar(0, 0, 255), 1);
                                }
                            }
                            else if (D135_sum >= 100)
                            {
                                if (D45_2 > 3 * (D45_1 + D45_3))
                                {
                                    re.allAreaNum++;
                                    re.locALL.push_back(Point(j, i));
                                    cv::circle(img_draw, Point(j, i), 1, cv::Scalar(0, 0, 255), 1);
                                }
                            }
                            break;
                        }
                        case 4: {
                            double D45_sum = abs(D45_1 - D45_2) + abs(D45_1 - D45_3) + abs(D45_1 - D45_3);
                            if (D45_sum < 100)
                            {
                                re.allAreaNum++;
                                re.locALL.push_back(Point(j, i));
                                cv::circle(img_draw, Point(j, i), 1, cv::Scalar(0, 0, 255), 1);
                            }
                            else if (D45_sum >= 100)
                            {
                                if (D135_2 > 3 * (D135_1 + D135_3))
                                {
                                    re.allAreaNum++;
                                    re.locALL.push_back(Point(j, i));
                                    cv::circle(img_draw, Point(j, i), 1, cv::Scalar(0, 0, 255), 1);
                                }
                            }
                            break;
                        }
                        default:
                            break;
                        }
                    }
                }
            }
        }
        return re;
    
}

void algorithm::MLDeadPointDetect::DeanPointsAvg(cv::Mat img)
{    
        cv::Mat img_draw = img.clone();
        cv::Mat img_gray;
        if (img.channels() == 3)
            cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
        else
            img_gray = img.clone();
        int row = img_gray.rows;
        int col = img_gray.cols;
        int numH = 0, numD = 0;
        for (int i = 0; i < col - 5; i = i + 5)
        {
            for (int j = 0; j < row - 5; j = j + 5)
            {
                cv::Mat tmp = img_gray(cv::Range(i, i + 5), cv::Range(j, j + 5));
                cv::Rect rect = cv::Rect(i, j, 5, 5);
                // use the mat
                /*           cv::Mat tmp1 = cv::Mat::zeros(cv::Size(1, tmp.rows * tmp.cols), tmp.type());
                           cv::Mat tmp2 = tmp.clone();
                           tmp1=tmp2.reshape(1, tmp.rows * tmp.cols);
                           cv::Mat dst = cv::Mat::zeros(cv::Size(1,tmp.rows * tmp.cols),tmp.type());
                           cv::sort(tmp1, dst, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
                           double Gh = dst.at<uchar>(dst.rows - 2);
                           double Gl = dst.at<uchar>(1);
                           double diff = Gh - Gl;
                           cv::Scalar meanS = cv::mean(tmp);
                           double mean = meanS(0);*/

                vector<double> tmpVec;
                for (int i0 = 0; i0 < 5; i0++)
                {
                    for (int j0 = 0; j0 < 5; j0++)
                    {
                        tmpVec.push_back(tmp.at<uchar>(i0, j0));
                    }
                }

                std::sort(tmpVec.begin(), tmpVec.end()); // shengxu
                double Gmid = tmpVec[13];
                double Gh = tmpVec[tmpVec.size() - 2];
                double Gl = tmpVec[1];
                double diff = Gh - Gl;
                double sum = std::accumulate(std::begin(tmpVec), std::end(tmpVec), 0.0) - (Gmid + Gh + Gl);
                double mean = sum / (tmpVec.size() - 3.0); //均值

                for (int i0 = 0; i0 < 5; i0++)
                {
                    for (int j0 = 0; j0 < 5; j0++)
                    {
                        double value = tmp.at<uchar>(i0, j0);
                        double minB = int(((mean - diff) * 10 + 5) / 10);
                        double maxB = int(((mean + diff) * 10 + 5) / 10);
                        if (minB < 0)
                        {
                            minB = 0;
                        }
                        //   if (maxB > 255) { maxB = 255; }
                        if (value < minB)
                        {
                            numD++;
                            cv::circle(img_draw, cv::Point(j0 + j, i0 + i), 1, cv::Scalar(0, 0, 255));
                        }
                        else if (value > maxB)
                        {
                            numH++;
                            cv::circle(img_draw, cv::Point(j0 + j, i0 + i), 1, cv::Scalar(0, 255, 0));
                            cout << cv::Point(j0 + j, i0 + i) << endl;
                        }
                    }
                }
            }
        }
    
}

cv::Mat algorithm::MLDeadPointDetect::ExpandMat(cv::Mat img, int r)
{   
        // just about the gray image
        cv::Mat imgGray;
        if (img.channels() == 3)
        {
            cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);
        }
        else
        {
            imgGray = img.clone();
        }
        int row = imgGray.rows;
        int col = imgGray.cols;
        cv::Mat imgExp = cv::Mat::zeros(cv::Size(col + 2 * r, row + 2 * r), imgGray.type());
        imgGray.copyTo(imgExp(cv::Range(r, r + row), cv::Range(r, r + col)));
        imgGray(cv::Range(0, r), cv::Range(0, col)).copyTo(imgExp(cv::Range(0, r), cv::Range(r, col + r)));
        imgGray(cv::Range(row - r, row), cv::Range(0, col))
            .copyTo(imgExp(cv::Range(row + r, row + 2 * r), cv::Range(r, col + r)));
        imgGray(cv::Range(0, row), cv::Range(0, r)).copyTo(imgExp(cv::Range(r, row + r), cv::Range(0, r)));
        imgGray(cv::Range(0, row), cv::Range(row - r, row))
            .copyTo(imgExp(cv::Range(r, row + r), cv::Range(row + r, row + 2 * r)));
        return imgExp;
    
}
