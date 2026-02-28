#include "ml_image_public.h"
#include <Windows.h>
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\imgproc\types_c.h>
#include <opencv2\opencv.hpp>
#include <stdlib.h>
#include <string>
#include <vector>
using namespace cv;
using namespace std;
void algorithm::MLimagePublic::NaiveRemoveNoise(cv::Mat &srcImage, double pArea)
{
    // pArea = double(srcImage.rows) / 2048.0 * pArea;
    int i, j;
    int color = 1;
    int nRows = srcImage.rows;
    int nCols = srcImage.cols;

    cv::Mat img, img_edge, labels, centroids, img_color, stats;
    int nccomps = connectedComponentsWithStats(srcImage, labels, stats, centroids);
    for (i = 0; i < nRows; ++i)
    {
        for (j = 0; j < nCols; ++j)
        {
            if (srcImage.at<uchar>(i, j) == 255)
            {
                int label = labels.at<int>(i, j);
                if (stats.at<int>(label, cv::CC_STAT_AREA) < pArea)
                {
                    srcImage.at<uchar>(i, j) = 0;
                }
            }
        }
    }
}
void algorithm::MLimagePublic::ContoursRemoveNoise(cv::Mat &srcImage, double pArea)
{
    int i, j;
    int color = 1;
    int nRows = srcImage.rows;
    int nCols = srcImage.cols;

    for (i = 0; i < nRows; ++i)
        for (j = 0; j < nCols; ++j)
        {
            if (!srcImage.at<uchar>(i, j))
            {
                // FloodFill each point in connect area using different color
                floodFill(srcImage, Point(j, i), Scalar(color));
                color++;
            }
        }

    int ColorCount[255] = {0};

    for (i = 0; i < nRows; ++i)
    {
        for (j = 0; j < nCols; ++j)
        {
            // caculate the area of each area

            if (srcImage.at<uchar>(i, j) != 255)
                ColorCount[srcImage.at<uchar>(i, j)]++;
        }
    }

    // get rid of noise point
    for (i = 0; i < nRows; ++i)
    {
        for (j = 0; j < nCols; ++j)
        {
            double temp = ColorCount[srcImage.at<uchar>(i, j)];
            if (srcImage.at<uchar>(i, j) != 255)
            {
                if ((ColorCount[srcImage.at<uchar>(i, j)]) <= pArea)
                    srcImage.at<uchar>(i, j) = 255;
                else
                    srcImage.at<uchar>(i, j) = 0;
            }
        }
    }
}
void algorithm::MLimagePublic::Clear_MicroConnected_Areas(cv::Mat src, cv::Mat &dst, double min_area)
{
    // 备份复制
    dst = src.clone();
    std::vector<std::vector<cv::Point>> contours; // 创建轮廓容器
    std::vector<cv::Vec4i> hierarchy;

    // 寻找轮廓的函数
    // 第四个参数CV_RETR_EXTERNAL，表示寻找最外围轮廓
    // 第五个参数CV_CHAIN_APPROX_NONE，表示保存物体边界上所有连续的轮廓点到contours向量内
    cv::findContours(src, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());

    if (!contours.empty() && !hierarchy.empty())
    {
        std::vector<std::vector<cv::Point>>::const_iterator itc = contours.begin();
        // 遍历所有轮廓
        while (itc != contours.end())
        {
            // 定位当前轮廓所在位置
            cv::Rect rect = cv::boundingRect(cv::Mat(*itc));
            // contourArea函数计算连通区面积
            double area = contourArea(*itc);
            // 若面积小于设置的阈值
            if (area < min_area)
            {
                // 遍历轮廓所在位置所有像素点
                for (int i = rect.y; i < rect.y + rect.height; i++)
                {
                    uchar *output_data = dst.ptr<uchar>(i);
                    for (int j = rect.x; j < rect.x + rect.width; j++)
                    {
                        // 将连通区的值置0
                        if (output_data[j] == 255)
                        {
                            output_data[j] = 0;
                        }
                    }
                }
            }
            itc++;
        }
    }
}
bool algorithm::MLimagePublic::GetAllMinLoc(Mat image, Mat templ, double sim, Scalar mask,
                                            vector<cv::Point> *all_min_loc, int num)
{
    bool flag = true;
    if (image.data != NULL & templ.data != NULL)
    {
        if (image.channels() == 3)
        {
            cvtColor(image, image, cv::COLOR_BGR2GRAY);
        }
        if (templ.channels() == 3)
        {
            cvtColor(templ, templ, cv::COLOR_BGR2GRAY);
        }
        int i = 0;
        int src_Width = image.cols, src_Height = image.rows;
        int templ_Width = templ.cols, templ_Height = templ.rows;
        double min_value, max_value;
        Point min_Loc, max_Loc;
        Mat img_result;
        Mat img_clon = image.clone();
        bool matchFlag = true;
        if (templ_Width > src_Width || templ_Height > src_Height)
        {
            printf("The template size is larger , please select the correct template\n");
            matchFlag = false;
            return false;
        }
        if (matchFlag)
        {
            for (int i = 0; i < num; i++)
            {
                matchTemplate(img_clon, templ, img_result, TM_CCOEFF_NORMED);
                minMaxLoc(img_result, &min_value, &max_value, &min_Loc, &max_Loc);
                // cout << max_value << endl;
                if (max_value > sim)
                {
                    all_min_loc->push_back(max_Loc);
                    rectangle(img_clon, Rect(max_Loc.x, max_Loc.y, templ_Width, templ_Height), mask, -1);
                }
                else
                    break;
            }
        }
    }

    return flag;
}

void algorithm::MLimagePublic::readROItxtFile(string filepath, vector<cv::Rect> &roiArea)
{

    std::ifstream accfile;
    accfile.open(filepath);
    if (!accfile.is_open())
    {
        cerr << "open acc file failed" << endl;
        return;
    }
    while (!accfile.eof())
    {
        double stratx = 0, straty = 0, width = 0, height = 0;
        accfile >> stratx >> straty >> width >> height;
        if (width > 0 & height > 0)
        {
            cv::Rect temp = cv::Rect(stratx, straty, width, height);
            roiArea.push_back(temp);
        }
    }
    if (accfile.is_open())
    {
        accfile.close();
    }
}

void algorithm::MLimagePublic::readPointstxtFile(string filepath, vector<cv::Point2f> &points)
{

    std::ifstream accfile;
    accfile.open(filepath);
    if (!accfile.is_open())
    {
        cerr << "open acc file failed" << endl;
        return;
    }
    while (!accfile.eof())
    {
        double x = 0, y = 0;
        accfile >> x >> y;
        cv::Point2f temp = cv::Point2f(x, y);
        points.push_back(temp);
    }
    if (accfile.is_open())
    {
        accfile.close();
    }
}

void algorithm::MLimagePublic::writePointsFile(string fileName, vector<cv::Point2f> points)
{

    ofstream datafile;
    datafile.open(fileName, ofstream::app);
    ofstream file(fileName, ios::out);
    for (int i = 0; i < points.size(); i++)
    {
        datafile << points[i].x << ',' << points[i].y << endl;
    }
    datafile.close();
}

void algorithm::MLimagePublic::writeROIFile(string fileName, vector<cv::Rect> roiVec)
{
    if (roiVec.size() > 0)
    {
        ofstream datafile1;
        datafile1.open(fileName, ofstream::app);
        ofstream file1(fileName, ios::out);
        for (int i = 0; i < roiVec.size(); i++)
        {
            if (roiVec[i].width > 0 & roiVec[i].height > 0)
                datafile1 << roiVec[i].x << ' ' << roiVec[i].y << ' ' << roiVec[i].width << ' ' << roiVec[i].height
                          << endl;
        }
        datafile1.close();
    }
}
cv::Mat algorithm::MLimagePublic::readCSVToMat(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        // qDebug << "Could not open file: " + filename;// << endl;
        return Mat();
    }
    string line;
    vector<vector<double>> data;
    while (getline(file, line))
    {
        stringstream ss(line);
        string value;
        vector<double> row;
        while (getline(ss, value, ','))
        {
            row.push_back(stod(value)); // 将字符串转换为 double
        }
        if (row.size() > 0)
            data.push_back(row);
    }
    file.close();
    // 确定 Mat 的大小
    int rows = data.size();
    int cols = (rows > 0) ? data[0].size() : 0;

    // 创建 Mat 并填充数据
    Mat mat(rows, cols, CV_32FC1); // 使用 CV_64F 表示 double 类型
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            mat.at<float>(i, j) = data[i][j];
        }
    }
    return mat;
}

cv::Mat algorithm::MLimagePublic::convertToUint8(const cv::Mat img)
{
    if (img.depth() == CV_8U)
        return img;
    cv::Mat tmp;
    cv::Mat dst8 = cv::Mat::zeros(img.size(), CV_8U);
    cv::Mat img1 = img.clone();
    img1.setTo(0, img1 < 0);
    cv::normalize(img1, tmp, 0, 255, NORM_MINMAX);
    cv::convertScaleAbs(tmp, dst8);
    return dst8;
}

cv::Mat algorithm::MLimagePublic::convert12bitTo8bit(cv::Mat img)
{
    cv::Mat img_gray;
    if (img.channels() == 3)
        cvtColor(img, img_gray, CV_BGR2GRAY);
    else
        img_gray = img.clone();
    Mat dst8bit;
    dst8bit = img_gray.clone();
    if (img_gray.depth() == CV_16U)
    {
        Mat outtmp; // = Mat(roi.cols, roi.rows, CV_8UC1);
        double minv = 0.0, maxv = 0.0;
        double *minp = &minv;
        double *maxp = &maxv;
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
    return img_gray;
}

cv::Mat algorithm::MLimagePublic::convert2ColorImage(cv::Mat img)
{
    if (img.channels() == 1)
    {
        vector<cv::Mat> imgm;
        imgm.push_back(img);
        imgm.push_back(img);
        imgm.push_back(img);
        cv::merge(imgm, img);
    }
    return img;
}

cv::Mat algorithm::MLimagePublic::convertTo3Channels(const cv::Mat binImg)
{
    // binImg.setTo(0, binImg < 0);
    cv::Mat img3;
    if (binImg.channels() == 1)
    {
        cv::Mat tmp = binImg.clone();
        // tmp.setTo(0, tmp < 0);
        cvtColor(tmp, img3, CV_GRAY2BGR);
    }
    else
        img3 = binImg.clone();
    return img3;
}

cv::Mat algorithm::MLimagePublic::convertToGrayImage(const cv::Mat img)
{
    cv::Mat img8;

    if (img.channels() == 1)
        img8 = img.clone();
    else
        cv::cvtColor(img, img8, COLOR_BGR2GRAY);
    return img8;
}

cv::Mat algorithm::MLimagePublic::getRectROIImg(cv::Mat img, cv::Rect rect)
{
    if (img.empty())
        return img;
    if (rect.area() <= 0 || rect.x < 0 || rect.y < 0 || rect.width <= 0 || rect.height <= 0)
        return img;
    cv::Rect rectAnd = rect & cv::Rect(0, 0, img.cols, img.rows);
    return img(rectAnd).clone();
}

cv::RotatedRect algorithm::MLimagePublic::getMaxAreaContourRect(cv::Mat imgth, cv::Rect &rect)
{
    NaiveRemoveNoise(imgth, 1000);
    ContoursRemoveNoise(imgth, 1000);
    cv::RotatedRect rectR;
    vector<vector<Point>> contours;
    vector<Vec4i> hierachy;
    findContours(imgth, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<cv::Rect> rectVec;
    vector<cv::RotatedRect> rectRVec;
    vector<double> areaVec;
    for (int i = 0; i < contours.size(); i++)
    {
        cv::Rect rect0 = cv::boundingRect(contours[i]);
        double area = cv::contourArea(contours[i]);
        cv::RotatedRect rectR0 = cv::minAreaRect(contours[i]);
        rectVec.push_back(rect0);
        rectRVec.push_back(rectR0);
        areaVec.push_back(area);
        // areaVec.push_back(rect0.area());
    }
    if (areaVec.size() < 1)
        return RotatedRect();
    int index = max_element(areaVec.begin(), areaVec.end()) - areaVec.begin();
    if (index < 0)
        return RotatedRect();
    rectR = rectRVec[index];
    rect = rectVec[index];
    return rectR;
}

cv::RotatedRect algorithm::MLimagePublic::getClosestContourRect(cv::Mat imgth, cv::Point2f c0, cv::Rect &rect)
{
    NaiveRemoveNoise(imgth, 1000);
    ContoursRemoveNoise(imgth, 1000);
    cv::RotatedRect rectR;
    vector<vector<Point>> contours;
    vector<Vec4i> hierachy;
    findContours(imgth, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<cv::Rect> rectVec;
    vector<cv::RotatedRect> rectRVec;
    vector<double> areaVec;
    vector<double> disVec;
    double sumx = 0;
    double sumy = 0;
    for (int i = 0; i < contours.size(); i++)
    {
        cv::Rect rect0 = cv::boundingRect(contours[i]);
        double area = cv::contourArea(contours[i]);
        cv::RotatedRect rectR0 = cv::minAreaRect(contours[i]);
        // double dis0 = Getdistance(rectR0.center, c0);
        if (rect0.area() > 2e3)
        {
            sumx = sumx + rectR0.center.x;
            sumy = sumy + rectR0.center.y;
            rectVec.push_back(rect0);
            rectRVec.push_back(rectR0);
            areaVec.push_back(area);
            // disVec.push_back(dis0);
            // areaVec.push_back(rect0.area());
        }
    }

    if (areaVec.size() < 1)
        return RotatedRect();

    //c0 = Point2f(sumx / areaVec.size(), sumy / areaVec.size());
    for (int i = 0; i < rectRVec.size(); i++)
    {
        double dis0 = Getdistance(rectRVec[i].center, c0);
        disVec.push_back(dis0);
    }

    int index = min_element(disVec.begin(), disVec.end()) - disVec.begin();
    if (index < 0)
        return RotatedRect();
    rectR = rectRVec[index];
    rect = rectVec[index];
    return rectR;
}

cv::Mat algorithm::MLimagePublic::GetROIMat(cv::Mat img, Rect rec)
{
    cv::Mat img_gray;
    cv::Mat roi;
    if (img.data != NULL)
    {
        if (img.channels() == 3)
        {
            cvtColor(img, img_gray, CV_BGR2GRAY);
        }
        else
            img_gray = img.clone();
        if (rec.x >= 0 & rec.y >= 0 & rec.x + rec.width <= img.cols & rec.y + rec.height <= img.rows & rec.width > 0 &
            rec.height > 0)
        {
            roi = img_gray(rec).clone();
        }
        else
        {
            roi = img_gray.clone();
        }
    }
    return roi;
}

double algorithm::MLimagePublic::Getdistance(cv::Point p1, cv::Point p2)
{
    double d = 0;
    d = sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
    return d;
}

double algorithm::MLimagePublic::corrlateMat90(cv::Mat A0)
{

    double re = 0;
    int len = min(A0.rows, A0.cols);
    cv::Mat A = A0(cv::Rect(0, 0, len, len));
    cv::Mat B;
    rotate(A, B, ROTATE_90_CLOCKWISE);
    int rows = A.rows;
    int cols = A.cols;
    cv::Scalar meanA, meanB;
    meanA = cv::mean(A);
    meanB = cv::mean(B);
    double sumAB = 0;
    double sumA = 0;
    double sumB = 0;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            sumAB = sumAB + (A.at<uchar>(i, j) - meanA(0)) * (B.at<uchar>(i, j) - meanB(0));
            sumA = sumA + pow((A.at<uchar>(i, j) - meanA(0)), 2);
            sumB = sumB + pow((B.at<uchar>(i, j) - meanA(0)), 2);
        }
    }

    // cout << sumAB << ',' << sqrt((sumA * sumB)) << endl;
    double corr = sumAB / sqrt((sumA * sumB));

    return corr;
}

double algorithm::MLimagePublic::corrlateMat180(cv::Mat A)
{
    double re = 0;
    cv::Mat B;
    rotate(A, B, ROTATE_180);
    int rows = A.rows;
    int cols = B.cols;
    cv::Scalar meanA, meanB;
    meanA = cv::mean(A);
    meanB = cv::mean(B);
    double sumAB = 0;
    double sumA = 0;
    double sumB = 0;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            sumAB = sumAB + (A.at<uchar>(i, j) - meanA(0)) * (B.at<uchar>(i, j) - meanB(0));
            sumA = sumA + pow((A.at<uchar>(i, j) - meanA(0)), 2);
            sumB = sumB + pow((B.at<uchar>(i, j) - meanA(0)), 2);
        }
    }
    double corr = sumAB / sqrt((sumA * sumB));
    return corr;
}

double algorithm::MLimagePublic::getMagnitude(cv::Mat img)
{
    cv::Mat Gx, Gy;
    cv::Sobel(img, Gx, CV_32F, 0, 1);
    cv::Sobel(img, Gy, CV_32F, 1, 0);
    double normGx = norm(Gx);
    double normGy = norm(Gy);
    double mag = sqrt(pow(normGx, 2) + pow(normGy, 2));
    return mag;
}

vector<cv::Point> algorithm::MLimagePublic::drawline(double k, double b)
{
    vector<cv::Point> point;
    cv::Point point1, point2;
    point1.x = 0;
    point1.y = b;
    point2.x = 5000;
    point2.y = k * 5000 + b;
    point.push_back(point1);
    point.push_back(point2);
    return point;
}

vector<cv::Point> algorithm::MLimagePublic::drawline(vector<double> line)
{
    vector<cv::Point> point;
    if (line.size() == 3)
    {
        double a = line[0];
        double b = line[1];
        double c = line[2];
        if (abs(b) < 1e-6)
        {
            b = 1e5;
        }
        double k = -a / b;
        double b0 = -c / b;
        cv::Point point1, point2;
        point1.x = 0;
        point1.y = b0;
        point2.x = 5000;
        point2.y = k * 5000 + b0;
        point.push_back(point1);
        point.push_back(point2);
    }
    else
    {
        point.push_back(cv::Point(0, 0));
        point.push_back(cv::Point(0, 0));
    }

    return point;
}

double algorithm::MLimagePublic::median(vector<double> valVec)
{
    nth_element(valVec.begin(), valVec.begin() + 1, valVec.end());
    double midVal = valVec[valVec.size() / 2];
    return midVal;
}

double algorithm::MLimagePublic::median(double a1, double a2, double a3)
{
    vector<double> a;
    a.push_back(a1);
    a.push_back(a2);
    a.push_back(a3);
    nth_element(a.begin(), a.begin() + 1, a.end());
    double midVal = a[a.size() / 2];
    return midVal;
}
vector<double> algorithm::MLimagePublic::LineInf(cv::Point2f p1, cv::Point2f p2)
{

    vector<double> line;
    double k = (p2.y - p1.y) / (p2.x - p1.x);
    if (k > 1e6)
    {
        k = 1e5;
    }
    else if (k < -1e6)
    {
        k = -1e5;
    }
    double b = p1.y - k * p1.x;
    line.push_back(k);
    line.push_back(b);
    return line;
}

cv::Point2f algorithm::MLimagePublic::getintersection(double k1, double b1, double k2, double b2)
{
    cv::Point2f point;
    double x = 0, y = 0;
    x = (b2 - b1) / (k1 - k2);
    y = k1 * x + b1;
    point.x = x;
    point.y = y;
    return point;
}

void algorithm::MLimagePublic::writeCSV(string filename, Mat m)
{
    ofstream myfile;
    myfile.open(filename.c_str());
    myfile << cv::format(m, cv::Formatter::FMT_CSV) << std::endl;
    myfile.close();
}

cv::Point2f algorithm::MLimagePublic::get2LineIntersectionPoint(cv::Point2f lineOnePt1, cv::Point2f lineOnePt2,
                                                                cv::Point2f lineTwoPt1, cv::Point2f lineTwoPt2)
{
    double x;
    double y;
    cv::Point2f result(0, 0);
    double k = (lineOnePt1.y - lineOnePt2.y) / (lineOnePt1.x - lineOnePt2.x);
    if (k == INFINITY)
    {
        k = 1e5;
    }
    else if (k == -INFINITY)
    {
        k = -1e5;
    }
    double b = lineOnePt1.y - k * lineOnePt1.x;

    double k1 = (lineTwoPt1.y - lineTwoPt2.y) / (lineTwoPt1.x - lineTwoPt2.x);
    if (k1 == INFINITY)
    {
        k1 = 1e5;
    }
    else if (k1 == -INFINITY)
    {
        k1 = -1e5;
    }
    double b1 = lineTwoPt1.y - k1 * lineTwoPt1.x;

    x = (b1 - b) / (k - k1);
    y = k * x + b;
    result.x = x;
    result.y = y;

    return result;
}

bool algorithm::MLimagePublic::sort_user(const vector<double> x, vector<double> &result, vector<int> &index)
{
    bool flag = true;
    vector<double> x1(x);
    sort(x1.begin(), x1.end(), greater<double>());
    result.swap(x1);
    double temp;
    int num = x.size();
    // index(num, 0);

    for (int i = 0; i != x.size(); i++)
    {
        index.push_back(i);
    }
    sort(index.begin(), index.end(), [&](const int &a, const int &b) { return (x[a] > x[b]); });

    return flag;
}

cv::Mat algorithm::MLimagePublic::linspace(double begin, double finish, int number)
{
    double interval = (finish - begin) / (number - 1); //
    Mat f(1, number, CV_64FC1);
    for (int i = 0; i < f.rows; i++)
    {
        for (int j = 0; j < f.cols; j++)
        {
            f.at<double>(i, j) = begin + j * interval;
            // cout << "("<<i+1<<","<<j+1<<")"<<f.at<double>(i, j) << endl;
        }
    }
    return f;
}
double algorithm::MLimagePublic::percentile(cv::Mat img, double p)
{
    cv::Mat rowMat = img.reshape(0, 1);
    rowMat.convertTo(rowMat, CV_32FC1);
    cv::Mat rowMatSort;
    cv::sort(rowMat, rowMatSort, SORT_ASCENDING);
    double position = 1 + (rowMatSort.total() - 1) * p / 100.0 - 1; // -1 the index different with python
    double posV = rowMatSort.at<float>(0, int(position));
    double posVNext = rowMatSort.at<float>(0, int(position) + 1);
    double y = posV + (posVNext - posV) * (position - int(position));
    return y;
}

cv::Mat algorithm::MLimagePublic::getRotationImg(cv::Mat img, double angle)
{
    cv::Mat rotatedImg;
    cv::Point2f center((float)(img.cols / 2), (float)(img.rows / 2));
    cv::Mat affine_matrix = getRotationMatrix2D(center, angle, 1.0);
    warpAffine(img, rotatedImg, affine_matrix, img.size());
    return rotatedImg;
}

vector<double> algorithm::MLimagePublic::line_fit(Mat abs_grad, int flag)
{
    vector<double> point_x, point_y;
    for (int j = 0; j < abs_grad.rows; j++)
    {
        uchar *ptr = abs_grad.ptr<uchar>(j);
        for (int i = 0; i < abs_grad.cols; i++)
        {
            if (ptr[i] > 0)
            {
                point_x.push_back(double(i));
                point_y.push_back(double(j));
            }
        }
    }
    Mat X, X0, Y;
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

    Mat X_inv = (X.t() * X).inv();
    Mat co = X_inv * X.t() * Y;
    vector<double> coe = (vector<double>)(co.reshape(1, 1));
    return coe;
}
vector<double> algorithm::MLimagePublic::line_fit_orthogonal(Mat abs_grad)
{
    vector<double> point_x, point_y;
    for (int j = 0; j < abs_grad.rows; j++)
    {
        uchar *ptr = abs_grad.ptr<uchar>(j);
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
    double A = n * X.dot(Y) - sum(X)[0] * sum(Y)[0];
    double B = pow(sum(Y)[0], 2) - n * Y.dot(Y) - pow(sum(X)[0], 2) + n * X.dot(X);
    double C = sum(X)[0] * sum(Y)[0] - n * X.dot(Y);

    vector<double> coe(2);
    coe[0] = (-B + pow((B * 4 - 4 * A * C), 0.5)) / 2 / A;
    coe[1] = (sum(Y)[0] - coe[0] * sum(X)[0]) / n;

    return coe;
}

cv::Point2f algorithm::MLimagePublic::calLinesCross(float a1, float b1, float c1, float a2, float b2, float c2)
{

    Point2f cross;
    cross.x = -(c1 * b2 - c2 * b1) / (a1 * b2 - a2 * b1);
    cross.y = -(c1 * a2 - c2 * a1) / (b1 * a2 - b2 * a1);
    return cross;
}

cv::Point2f algorithm::MLimagePublic::calLinesIntersection(cv::Vec4f line1, cv::Vec4f line2)
{
    // TODO:: 判断是否为0
    double k1 = 0, k2 = 0, b1 = 0, b2 = 0;
    k1 = line1[1] / line1[0];
    b1 = line1[3] - k1 * line1[2];
    k2 = line2[1] / line2[0];
    b2 = line2[3] - k2 * line2[2];
    cv::Point2f point;
    double x = 0, y = 0;
    x = (b2 - b1) / (k1 - k2);
    y = k1 * x + b1;
    point.x = x;
    point.y = y;
    return point;
}

vector<double> algorithm::MLimagePublic::unique(vector<double> xVec, int thresh)
{
    int xNum = 1;
    vector<double> xUniq;
    if (xVec.size() > 0)
    {
        xUniq.push_back(xVec[0]);
        for (int i = 1; i < xVec.size(); i++)
        {
            vector<double> diff;
            for (int j = 0; j < xUniq.size(); j++)
            {
                double temp = abs(xVec[i] - xUniq[j]);
                diff.push_back(temp);
            }
            double mindiff = *min_element(diff.begin(), diff.end());
            if (mindiff > thresh)
            {
                xNum++;
                xUniq.push_back(xVec[i]);
            }
        }
        sort(xUniq.begin(), xUniq.end());
    }
    return xUniq;
}

int algorithm::MLimagePublic::findIndex(vector<cv::Point2f> pts, cv::Point2f pt0, double thresh)
{
    int index = -1;
    vector<double> dis;
    for (int i = 0; i < pts.size(); i++)
    {
        double temp = Getdistance(pts[i], pt0);
        dis.push_back(temp);
    }
    double minVal = *min_element(dis.begin(), dis.end());
    if (minVal < thresh)
    {
        index = min_element(dis.begin(), dis.end()) - dis.begin();
    }
    return index;
}

int algorithm::MLimagePublic::findNeighborNum(vector<double> xVec, double x0, double thresh)
{
    int num = 0;
    for (int i = 0; i < xVec.size(); i++)
    {
        if (abs(xVec[i] - x0) < thresh)
            num++;
    }
    return num;
}

int algorithm::MLimagePublic::findNeighborNum(vector<double> xVec, double x0, vector<int> &inVec, double thresh)
{
    int num = 0;
    for (int i = 0; i < xVec.size(); i++)
    {
        if (abs(xVec[i] - x0) < thresh)
        {
            num++;
            inVec.push_back(i);
        }
    }
    return num;
}

vector<cv::Point2f> algorithm::MLimagePublic::pointsClusters(vector<cv::Point2f> pts, double thresh)
{
    vector<double> dis1;
    double k[5000] = {0};
    cv::Mat kMat = cv::Mat::ones(cv::Size(1, 5000), CV_16UC1);
    kMat = -kMat;
    //   cv::Mat dis = cv::Mat::zeros(cv::Size(100, 100), CV_32FC1);
    cv::Mat index = cv::Mat::ones(cv::Size(5000, 5000), CV_16UC1);
    cv::Mat xValue = cv::Mat::ones(cv::Size(5000, 5000), CV_32FC1);
    cv::Mat yValue = cv::Mat::ones(cv::Size(5000, 5000), CV_32FC1);
    index = -index;
    xValue = -xValue;
    yValue = -yValue;
    if (pts.size() > 0 & pts.size() < 6000)
    {
        //  double tmp = Getdistance(pts[0], cv::Point2f(0, 0));
        // dis.at<float>(0, k[0]) = tmp;
        index.at<short>(0, 0) = 0;
        xValue.at<float>(0, 0) = pts[0].x;
        yValue.at<float>(0, 0) = pts[0].y;
        k[0]++;
        kMat.at<short>(0, 0) = k[0];
        for (int i = 1; i < pts.size(); i++)
        {

            // double tmp = Getdistance(pts[i],pts[i-1]);
            int flag = 0;
            int j = 0;
            for (j = 0; j < kMat.rows; j++)
            {
                if (kMat.at<short>(j, 0) > 0)
                {
                    // double d = abs(tmp - dis.at<float>(j, 0));
                    int in = index.at<short>(j, 0);
                    double dx = abs(pts[i].x - pts[index.at<short>(j, 0)].x);
                    double dy = abs(pts[i].y - pts[index.at<short>(j, 0)].y);
                    if (dx < thresh & dy < thresh)
                    {
                        // dis.at<float>(j, k[j]) = tmp;
                        index.at<short>(j, k[j]) = i;
                        xValue.at<float>(j, k[j]) = pts[i].x;
                        yValue.at<float>(j, k[j]) = pts[i].y;
                        k[j]++;
                        kMat.at<short>(j, 0) = k[j];
                        flag = 1;
                    }
                }
                else
                {
                    break;
                }
            }
            if (flag == 0)
            {
                // dis.at<float>(j, k[j]) = tmp;
                index.at<short>(j, k[j]) = i;
                xValue.at<float>(j, k[j]) = pts[i].x;
                yValue.at<float>(j, k[j]) = pts[i].y;
                k[j]++;
                kMat.at<short>(j, 0) = k[j];
            }
        }
    }

    vector<cv::Point2f> ptsNew;
    for (int i = 0; i < xValue.rows; i++)
    {
        if (xValue.at<float>(i, 0) > -1)
        {
            double sumx = 0, sumy = 0;
            for (int j = 0; j < xValue.cols; j++)
            {
                if (xValue.at<float>(i, j) > -1)
                {
                    sumx = sumx + xValue.at<float>(i, j);
                    sumy = sumy + yValue.at<float>(i, j);
                }
                else
                {
                    double x = sumx / k[i];
                    double y = sumy / k[i];
                    ptsNew.push_back(cv::Point2f(x, y));
                    break;
                }
            }
        }
        else

            break;
    }
    return ptsNew;
}

vector<double> algorithm::MLimagePublic::dataClusters(vector<double> data, vector<int> &vecList, double thresh)
{
    vector<double> dataNew;

    vector<double> dataUniq = unique(data, thresh);
    int num = dataUniq.size();
    vector<int> numVec;
    for (int i = 0; i < dataUniq.size(); i++)
    {
        int n0 = findNeighborNum(data, dataUniq[i], thresh);
        numVec.push_back(n0);
    }
    int maxNum = max_element(numVec.begin(), numVec.end()) - numVec.begin();
    double maxVal = dataUniq[maxNum];
    for (int i = 0; i < data.size(); i++)
    {
        if (abs(data[i] - maxVal) < thresh)
        {
            dataNew.push_back(data[i]);
            vecList.push_back(i);
        }
    }
    return dataNew;
}

vector<double> algorithm::MLimagePublic::dataIndexMap(vector<double> data, vector<Point> P, cv::Mat &indexMap,
                                                      double thresh)
{
    vector<double> dataUnique;
    dataUnique = unique(data, thresh);
    sort(dataUnique.begin(), dataUnique.end(), greater<double>());
    indexMap = cv::Mat::ones(cv::Size(3, dataUnique.size()), CV_8UC1);
    cv::multiply(indexMap, -1, indexMap);
    for (int i = 0; i < dataUnique.size(); i++)
    {
        vector<int> inVec;
        findNeighborNum(data, dataUnique[i], inVec, thresh);
        vector<double> xVec;
        for (int j = 0; j < inVec.size(); j++)
        {
            xVec.push_back(P[inVec[j]].x);
            // indexMap.at<uchar>(i, j) = inVec[j];
        }
        vector<int> inVecNew = sortIndex(xVec);
        for (int j = 0; j < inVecNew.size(); j++)
        {
            indexMap.at<uchar>(i, j) = inVec[inVecNew[j]];
        }
    }
    return dataUnique;
}

cv::Mat algorithm::MLimagePublic::generatePointsIndexMap1(vector<cv::Point2f> ptsNew, bool extendFlag, double thresh)
{
    vector<double> xVec, yVec;
    for (int i = 0; i < ptsNew.size(); i++)
    {
        // circle(img_draw1, ptsNew[i], 10, Scalar(0, 0, 255), -1);
        xVec.push_back(ptsNew[i].x);
        yVec.push_back(ptsNew[i].y);
        //  cout << ptsNew[i] << endl;
    }
    // x dian fenlei
    vector<double> xUniq = unique(xVec, thresh); //棋盘格分割阈值
    int xNum = xUniq.size();
    // y dian fenlei
    vector<double> yUniq = unique(yVec, thresh); // 棋盘格分割阈值
    int yNum = yUniq.size();
    cv::Mat indexMap1;
    int num = 0;
    if (extendFlag == true)
    {
        indexMap1 = cv::Mat::ones(cv::Size(yNum + 2, xNum + 2), CV_16S);
        num = 1;
    }
    else
    {
        indexMap1 = cv::Mat::ones(cv::Size(yNum, xNum), CV_16S);
    }
    cv::multiply(indexMap1, -1, indexMap1);
    // TODO: not reasonable
    // if (xNum != yNum)
    //    return indexMap1;
    for (int i = 0; i < xNum; i++)
    {
        for (int j = 0; j < yNum; j++)
        {
            cv::Point2f pt0 = cv::Point2f(xUniq[i], yUniq[j]);
            int index = -1;
            index = findIndex(ptsNew, pt0, thresh); // 50 棋盘格点直接距离阈值
            indexMap1.at<short>(j + num, i + num) = index;
        }
    }
    return indexMap1;
}

cv::Mat algorithm::MLimagePublic::generatePointsIndexMap(vector<cv::Point2f> ptsNew, bool extendFlag, double thresh)
{
    if (ptsNew.size() < 1)
        return Mat();
    vector<double> xVec, yVec;
    for (int i = 0; i < ptsNew.size(); i++)
    {
        // circle(img_draw1, ptsNew[i], 10, Scalar(0, 0, 255), -1);
        xVec.push_back(ptsNew[i].x);
        yVec.push_back(ptsNew[i].y);
        //  cout << ptsNew[i] << endl;
    }
    double minX = *min_element(xVec.begin(), xVec.end());
    double maxX = *max_element(xVec.begin(), xVec.end());
    double minY = *min_element(yVec.begin(), yVec.end());
    double maxY = *max_element(yVec.begin(), yVec.end());
    double xm = (minX + maxX) / 2.0;
    double ym = (minY + maxY) / 2.0;
    vector<double> xVec1, yVec1;
    for (int i = 0; i < ptsNew.size(); i++)
    {
        double suby = ptsNew[i].y - ym;
        double subx = ptsNew[i].x - xm;
        if (suby < thresh)
            xVec1.push_back(ptsNew[i].x);
        if (subx < thresh)
            yVec1.push_back(ptsNew[i].y);
    }
    // x dian fenlei
    vector<double> xUniq = unique(xVec1, thresh); //棋盘格分割阈值
    int xNum = xUniq.size();
    // y dian fenlei
    vector<double> yUniq = unique(yVec1, thresh); // 棋盘格分割阈值
    int yNum = yUniq.size();
    double xW = 0;
    for (int i = 0; i < xUniq.size() - 1; i++)
    {
        xW = xW + xUniq[i + 1] - xUniq[i];
    }
    xW = xW / (xUniq.size() - 1);
    int num = 0;
    cv::Size s;
    if (extendFlag == true)
    {
        s = cv::Size(xNum + 2, yNum + 2);
        num = 1;
    }
    else
    {
        s = cv::Size(xNum, yNum);
    }
    cv::Mat indexMap1(s, CV_16S, Scalar(-1));

    for (int i = 0; i < yNum; i++)
    {
        for (int j = 0; j < xNum; j++)
        {
            cv::Point2f pt0 = cv::Point2f(xUniq[j], yUniq[i]);
            int index = -1;
            index = findIndex(ptsNew, pt0, thresh); // 50 棋盘格点直接距离阈值
            indexMap1.at<short>(i + num, j + num) = index;
        }
    }

    return indexMap1;
}

// convexhull
// return the cos value of the polar angle
float calPolarAngle(const Point &base, const Point &another)
{

    float xDistance = another.x - base.x;
    float yDistance = another.y - base.y;
    float distance = std::sqrtf(xDistance * xDistance + yDistance * yDistance);
    return xDistance / distance;
}

struct YLess
{
    bool operator()(const Point &lhs, const Point &rhs)
    {
        return lhs.y < rhs.y;
    }
};

struct PolarAngleLess
{
    PolarAngleLess(const Point &p) : base(p)
    {
    }

    bool operator()(const Point &lhs, const Point &rhs)
    {
        return calPolarAngle(base, lhs) > calPolarAngle(base, rhs);
    }
    Point base;
};

bool ccwTurn(const cv::Point &a, const cv::Point &b, const cv::Point &c)
{
    Point ab(b.x - a.x, b.y - a.y);
    Point bc(c.x - b.x, c.y - b.y);
    return ab.x * bc.y - ab.y * bc.x > 0;
}
std::vector<cv::Point> algorithm::MLimagePublic::convecHull(std::vector<cv::Point> &points)
{

    if (points.size() <= 3)
        return points;

    std::vector<Point> backPoints(points);
    // for (int i = 0; i < backPoints.size(); i++)
    //{
    //    cout << backPoints[i] << endl;
    //}
    auto yMinItr = std::min_element(backPoints.begin(), backPoints.end(), YLess());
    // put the yMinPoint to the front
    std::iter_swap(yMinItr, backPoints.begin());
    // sort the left n-1 points based polar angle
    std::sort(backPoints.begin() + 1, backPoints.end(), PolarAngleLess(backPoints.front()));
    // cout << "after sort" << endl;
    // for (int i = 0; i < backPoints.size(); i++)
    //{
    //    cout << backPoints[i] << endl;
    //}
    for (auto it = backPoints.begin(); it != backPoints.end() - 2; ++it)
    {
        while (!ccwTurn(*it, *(it + 1), *(it + 2)))
        {
            backPoints.erase(it + 1);
            if (it == backPoints.begin())
                it = it;
            else if (it + 1 == backPoints.end() || it == backPoints.end())
                break;
            else
                it = it - 1;
        }
    }
    return backPoints;
}

double algorithm::MLimagePublic::getDistancePL(Point a, Point b, Point c)
{
    //点到直线距离(利用外积平行四边形)
    Point ab(b.x - a.x, b.y - a.y);
    Point ac(c.x - a.x, c.y - a.y);
    return ab.cross(ac) / norm(ab); // cross函数求两个向量的外积模,abs函数求向量模
                                    //点到直线距离(利用外积平行四边形)
}

vector<int> algorithm::MLimagePublic::sortIndex(vector<double> xVec)
{
    vector<int> index;
    vector<double> x;
    x = xVec;
    sort(x.begin(), x.end());
    for (int i = 0; i < x.size(); i++)
    {
        int in = findNeartVal(xVec, x[i]);
        index.push_back(in);
    }
    return index;
}

int algorithm::MLimagePublic::findNeartVal(vector<double> xVec, double x0)
{
    int index = -1;
    vector<double> dis;
    for (int i = 0; i < xVec.size(); i++)
    {
        dis.push_back(abs(xVec[i] - x0));
    }
    index = min_element(dis.begin(), dis.end()) - dis.begin();
    return index;
}

void algorithm::MLimagePublic::circleLeastFit(const std::vector<cv::Point> points, cv::Point2f &center, double &radius)
{
    radius = 0.0f;
    double sum_x = 0.0f, sum_y = 0.0f;
    double sum_x2 = 0.0f, sum_y2 = 0.0f;
    double sum_x3 = 0.0f, sum_y3 = 0.0f;
    double sum_xy = 0.0f, sum_x1y2 = 0.0f, sum_x2y1 = 0.0f;
    int N = points.size();
    for (int i = 0; i < N; i++)
    {
        double x = points[i].x;
        double y = points[i].y;
        double x2 = x * x;
        double y2 = y * y;
        sum_x += x;
        sum_y += y;
        sum_x2 += x2;
        sum_y2 += y2;
        sum_x3 += x2 * x;
        sum_y3 += y2 * y;
        sum_xy += x * y;
        sum_x1y2 += x * y2;
        sum_x2y1 += x2 * y;
    }
    double C, D, E, G, H;
    double a, b, c;
    C = N * sum_x2 - sum_x * sum_x;
    D = N * sum_xy - sum_x * sum_y;
    E = N * sum_x3 + N * sum_x1y2 - (sum_x2 + sum_y2) * sum_x;
    G = N * sum_y2 - sum_y * sum_y;
    H = N * sum_x2y1 + N * sum_y3 - (sum_x2 + sum_y2) * sum_y;
    a = (H * D - E * G) / (C * G - D * D);
    b = (H * C - E * D) / (D * D - G * C);
    c = -(a * sum_x + b * sum_y + sum_x2 + sum_y2) / N;
    center.x = a / (-2);
    center.y = b / (-2);
    radius = sqrt(a * a + b * b - 4 * c) / 2;
}

string algorithm::MLimagePublic::get_current_dir()
{
    char buff[FILENAME_MAX];
    _getcwd(buff, FILENAME_MAX);
    string current_working_dir(buff);
    return current_working_dir;
}

string algorithm::MLimagePublic::createDir()
{
    string currPath = get_current_dir();
    string folderName = "\\ALGRE";
    string fullPath = currPath + folderName;
    if (_access(fullPath.c_str(), 0) == -1) //如果文件夹不存在
    {
        _mkdir(fullPath.c_str());
    }
    return fullPath;
}
string algorithm::MLimagePublic::get_exe_dir()
{
    char szFilePath[MAX_PATH + 1] = {0};
    GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
    (strrchr(szFilePath, '\\'))[0] = 0;
    string str_path = szFilePath;
    return str_path;
}
string algorithm::MLimagePublic::currentTimetoStr()
{
    char tmp[64];
    time_t t = time(NULL);
    tm *_tm = localtime(&t);
    int year = _tm->tm_year + 1900;
    int month = _tm->tm_mon + 1;
    int date = _tm->tm_mday;
    int hh = _tm->tm_hour;
    int mm = _tm->tm_min;
    int ss = _tm->tm_sec;
    sprintf(tmp, "%04d-%02d-%02d-%02d-%02d-%02d", year, month, date, hh, mm, ss);
    return std::string(tmp);
}

cv::Point2f algorithm::MLimagePublic::getOpticalCenterAfterRotation(cv::Point2f center, cv::Size s, int rota)
{
    Point2f centerR;
    if (rota == 0)
        return center;
    cv::Point2f c0 = Point2f(s.width / 2.0, s.height / 2.0);
    cv::Point2f dis = center - c0;
    if (rota == 180)
    {
        centerR = c0 - dis;
    }
    else if (rota == 90)
    {
        centerR.x = c0.y - dis.y;
        centerR.y = c0.x + dis.x;
    }
    else if (rota == 270)
    {
        centerR.x = c0.y + dis.y;
        centerR.y = c0.x - dis.x;
    }
    return centerR;
}

cv::Point2f algorithm::MLimagePublic::getOpticalCenterAfterMirror(cv::Point2f center, cv::Size s, MirrorALG m)
{
    Point2f centerM;
    cv::Point2f c0 = Point2f(s.width / 2.0, s.height / 2.0);
    cv::Point2f dis = center - c0;
    if (m == NO_OP)
    {
        return center;
    }
    else if (m == LEFT_RIGHT)
    {
        centerM.x = c0.x - dis.x;
        centerM.y = center.y;
        return centerM;
    }
    else if (m == UP_DOWN)
    {
        centerM.x = center.x;
        centerM.y = c0.y - dis.y;
        return centerM;
    }
}

cv::Point2f algorithm::MLimagePublic::updateOpticalCenter(cv::Point2f base, cv::Size size, int rotationAngle,
                                                          MirrorALG mirror)
{
    cv::Size s;
    // if (rotationAngle == 90 || rotationAngle == 270)
    //    s = Size(size.height, size.width);
    // else
    //    s = size;
    int maxl = max(size.height, size.width);
    int minl = min(size.height, size.width);
    s = cv::Size(maxl, minl);
    cv::Point2f optPt;
    optPt = getOpticalCenterAfterRotation(base, s, rotationAngle);
    if (rotationAngle == 90 || rotationAngle == 270)
        s = cv::Size(minl, maxl);
    optPt = getOpticalCenterAfterMirror(optPt, s, mirror);
    return optPt;
}

bool algorithm::MLimagePublic::Is_File_Exist(const std::string &file_path)
{
    std::ifstream file(file_path.c_str());
    return file.good();
}
bool algorithm::MLimagePublic::containRect(cv::Rect a, cv::Rect b)
{
    if (a.contains(b.tl()) && a.contains(b.br()))
        return true;
    else
        return false;
    cv::Point p1, p2, p3, p4;
    p1 = cv::Point(b.x, b.y);
    p2 = cv::Point(b.x + b.width, b.y);
    p3 = cv::Point(b.x, b.y + b.height);
    p4 = cv::Point(b.x + b.width, b.y + b.height);
    if (a.contains(p1) && a.contains(p2) && a.contains(p3) && a.contains(p4))
    {
        return true;
    }
    else
        return false;
}

cv::Point2f algorithm::MLimagePublic::getRectCenter(cv::Rect rect)
{
    cv::Point cpt;
    cpt.x = rect.x + cvRound(rect.width / 2.0);
    cpt.y = rect.y + cvRound(rect.height / 2.0);
    return cpt;
}

cv::Rect algorithm::MLimagePublic::updateRectByRatio(cv::Rect rect, double ratio)
{
    cv::Rect rectNew;
    rectNew.x = rect.x + (rect.width - rect.width * ratio) / 2;
    rectNew.y = rect.y + (rect.height - rect.height * ratio) / 2;
    rectNew.width = rect.width * ratio;
    rectNew.height = rect.height * ratio;
    return rectNew;
}
cv::Rect algorithm::MLimagePublic::rotateRect(cv::RotatedRect rectR, cv::Point2f center, double angle)
{
    cv::Rect rect;
    cv::Point2f pt[4];
    rectR.points(pt);
    cv::Mat m = cv::getRotationMatrix2D(center, angle, 1.0);
    vector<cv::Point2f> pts;
    for (int i = 0; i < 4; i++)
    {
        pts.push_back(pt[i]);
    }
    vector<cv::Point2f> ptsR;
    cv::transform(pts, ptsR, m);
    rect = cv::boundingRect(ptsR);
    return rect;
}
vector<cv::Point2f> algorithm::MLimagePublic::rotatePoints(vector<cv::Point2f> pts, cv::Point2f center, double angle)
{
    cv::Mat m = cv::getRotationMatrix2D(center, angle, 1.0);
    vector<cv::Point2f> ptsR;
    cv::transform(pts, ptsR, m);
    return ptsR;
}

void algorithm::MLimagePublic::updateRotateImg(cv::Mat &img, double angle)
{

    if (abs(angle) < 10)
        img = getRotationImg(img, angle);
    else if (abs(angle - 90) < 10)
        img = getRotationImg(img, angle - 90);
    else if (abs(angle + 90) < 10)
        img = getRotationImg(img, angle + 90);
    else if (angle < 45)
        img = getRotationImg(img, angle + 90);
    else if (angle > 45)
        img = getRotationImg(img, angle + 180);
    // if (abs(angle) < 5)
    //{
    //    img = getRotationImg(img, 90-angle);
    //    return;
    //}
    // if (angle < 40 || abs(angle-90)<2)
    //{
    //    img = getRotationImg(img, angle);
    //    return;
    //}
    // else if (angle > 40 && angle < 90)
    //{
    //    img = getRotationImg(img, angle-90);
    //    return;
    //}
}
cv::Rect algorithm::MLimagePublic::updateRotateRect(cv::RotatedRect rectR, cv::Point2f center)
{
    cv::Rect rect;
    double angle = rectR.angle;
    if (abs(angle) < 10  )
        rect = rotateRect(rectR, center, angle);
    else if (abs(angle - 90) < 10)
        rect = rotateRect(rectR, center, angle - 90);
    else if ( abs(angle + 90) < 10)
        rect = rotateRect(rectR, center, angle + 90);
    else if (angle < 45)
        rect = rotateRect(rectR, center, angle + 90);
    else if (angle > 45)
        rect = rotateRect(rectR, center, angle + 180);
    return rect;

    // if (abs(rectR.angle) < 2)
    //{
    //    rect = rotateRect(rectR, center, 90-rectR.angle);
    //    return rect;
    //}
    // if (rectR.angle < 40 || abs(rectR.angle - 90) < 2)
    //{
    //    rect = rotateRect(rectR, center, rectR.angle);
    //    return rect ;
    //}
    // else if (rectR.angle > 40 && rectR.angle < 90)
    //{
    //    rect = rotateRect(rectR, center, rectR.angle-90);
    //    return rect;
    //}
}
string algorithm::MLimagePublic::numToString(double value)
{
    if (value < 1e4)
    {
        string str = to_string(value);
        return str.substr(0, str.size() - 5);
    }
    float numM = value;
    if (value > 1e4)
        numM = value / 1000;
    float s = ((float)((int)((numM + 0.005) * 100))) / 100;
    string str = to_string(s);
    if (value > 1e4)
        str = str.substr(0, str.size() - 7) + "e3";
    else
        str = str.substr(0, str.size() - 7);
    return str;
}

void algorithm::MLimagePublic::drawPointOnImage(cv::Mat &imgdraw, cv::Point2f pt, int r)
{
    cv::circle(imgdraw, pt, r, Scalar(0, 0, 255), -1);
}

void algorithm::MLimagePublic::drawPointsOnImage(cv::Mat &imgdraw, std::vector<cv::Point2f> pts, int r,
                                                 cv::Scalar color)
{
    for (int i = 0; i < pts.size(); i++)
    {
        cv::circle(imgdraw, pts[i], r, color, -1);
    }
}

void algorithm::MLimagePublic::drawRectOnImage(cv::Mat &imgdraw, cv::Rect rect, int width)
{
    cv::rectangle(imgdraw, rect, Scalar(0, 0, 255), width);
}

void algorithm::MLimagePublic::drawRectsOnImage(cv::Mat &imgdraw, std::vector<cv::Rect> rectVec, int width)
{
    for (int i = 0; i < rectVec.size(); i++)
    {
        cv::rectangle(imgdraw, rectVec[i], Scalar(0, 0, 255), width);
    }
}

void algorithm::MLimagePublic::drawCircleOnImage(cv::Mat &imgdraw, cv::Point2f pt, int r)
{
    cv::circle(imgdraw, pt, r, Scalar(0, 0, 255), 1);
}

void algorithm::MLimagePublic::putTextOnImage(cv::Mat &imgdraw, std::string str, cv::Point2f pt, int width)
{
    putText(imgdraw, str, pt, FONT_HERSHEY_PLAIN, width, Scalar(0, 255, 0), width);
}

void algorithm::MLimagePublic::drawLineOnImage(cv::Mat &imgdraw, cv::Point2f pt1, cv::Point2f pt2, int width)
{
    cv::line(imgdraw, pt1, pt2, Scalar(0, 0, 255), width);
}
