#include "pch.h"
#include "ML_FOV.h"
#include"CrossCenter.h"
#include"ml_rectangleDetection.h"
#include"LogPlus.h"
using namespace cv;
using namespace MLIQMetrics;
using namespace MLImageDetection;
MLIQMetrics::MLFOV::MLFOV()
{
}

MLIQMetrics::MLFOV::~MLFOV()
{
}

void MLIQMetrics::MLFOV::setIsSLB(bool flag)
{
    m_isSLB = flag;
}

FovRe MLIQMetrics::MLFOV::getFOVSolid(cv::Mat imgRaw)   //   -----this-----
{
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Solid FOV  calculation start");
    string info = "------ getFOVSolid------";
    FovRe re;
    if (imgRaw.data != NULL)
    {
        cv::Mat img = IQMetricUtl::instance()->getRotationAndFlipImg(imgRaw, m_isSLB);
        img = getRotationImg(img, m_rotationAngle);
        int binNum = IQMetricUtl::instance()->getBinNum(img.size());
        cv::Rect ROIRect = IQMetricsParameters::ROIRect;
        updateRectByRatio1(ROIRect, 1.0 / binNum);
        cv::Mat roi = getRectROIImg(img, ROIRect);
        cv::Mat img8 = convertToUint8(roi);
        cv::Mat imgdraw = convertTo3Channels(img8);
        // need add 4 to config
        int resizeNum = 4 / binNum;
        cv::Mat imgRe;
        cv::resize(img8, imgRe, img8.size() / resizeNum);
        RectangleDetection rd;
        cv::RotatedRect rectR = rd.getRectangleBorder(imgRe);
        //if (rectR.size.area() < 1e6 || rectR.size.area() > 2.25e6)
        if (rectR.size.area() < 1e6 || rectR.size.area() > 5.85e6)
        {
            rd.readSolidInfoFromCSV(rectR);
        }
        else
        {
            rd.writeSolidInfoToCSV(rectR);
        }
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "SolidBorder detection successfully");
        cv::Rect rectAf;
        cv::Point2f center((float)(imgRe.cols / 2), (float)(imgRe.rows / 2));
        rectAf = updateRotateRect(rectR, center); // 外接水平矩形
        rectAf = rd.getSolidExactRect(imgRe, rectAf);  // 精确矩形rect
        updateRectByRatio1(rectAf, resizeNum);

        cv::Point2f P[4];
        //rectR.points(P);
        P[0] = Point2f(rectAf.tl());
        P[1] = Point2f(rectAf.tl()) + Point2f(0, rectAf.height);
        P[2] = Point2f(rectAf.tl()) + Point2f(rectAf.width, rectAf.height);
        P[3] = Point2f(rectAf.tl()) + Point2f(rectAf.width, 0);

        cv::Point2f cen = rectR.center;
        cv::Point2f h1 = (P[0] + P[1]) / 2.0;
        cv::Point2f h2 = (P[2] + P[3]) / 2.0;
        cv::Point2f v1 = (P[0] + P[3]) / 2.0;
        cv::Point2f v2 = (P[2] + P[1]) / 2.0;

        if (false)
        {
            cv::Mat rowMat = img8(cv::Range(v1.y, v2.y), cv::Range(0, img8.cols));
            cv::Mat colMat = img8(cv::Range(0, img8.rows), cv::Range(h1.x, h2.x));
            cv::reduce(rowMat, rowMat, 0, REDUCE_AVG);
            cv::reduce(colMat, colMat, 1, REDUCE_AVG);
            if (findEdgePts1(rowMat, 0, h1, h2))
            {
                h1.y = cen.y;
                h2.y = cen.y;
            }
            if (findEdgePts1(colMat, 1, v1, v2))
            {
                v1.x = cen.x;
                v2.x = cen.x;
            }
        }
        re.EdgePoint.push_back(h1);
        re.EdgePoint.push_back(h2);
        re.EdgePoint.push_back(v1);
        re.EdgePoint.push_back(v2);
        cen = getPointsCenter(re.EdgePoint); // 求中心点

        // draw the data
        updateImgdraw(imgdraw, h1, binNum);
        updateImgdraw(imgdraw, h2, binNum);
        updateImgdraw(imgdraw, v1, binNum);
        updateImgdraw(imgdraw, v2, binNum);
        updateImgdraw(imgdraw, cen, binNum);

        double FovH = calculateFOV(h1, cen, binNum); // 中心到左边界的半水平视场角
        double FovH1 = calculateFOV(h2, cen, binNum);
        double FovV = calculateFOV(v1, cen, binNum);
        double FovV1 = calculateFOV(v2, cen, binNum);
        re.FovH = FovH + FovH1;
        re.FovV = FovV + FovV1;
        re.FovDiag = sqrt(re.FovH * re.FovH + re.FovV * re.FovV);
        putTextOnImage(imgdraw, "FOVH:" + to_string(re.FovH), cen + cv::Point2f(0, 400 / binNum), 24 / binNum);
        putTextOnImage(imgdraw, "FOVV:" + to_string(re.FovV), cen + cv::Point2f(0, 800 / binNum), 24 / binNum);
        re.imgdraw = imgdraw;
    }
    else
    {
        re.flag = false;
        re.errMsg = info + "input is null";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
        return re;
    }
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Solid FOV  calculation successfully");
    return re;
}

void MLIQMetrics::MLFOV::setRotaionxAngle(double angle)
{
    m_rotationAngle = angle;
}

double MLIQMetrics::MLFOV::calculateFOV(cv::Point2f h1, cv::Point2f cen,int binNum)
{
    double dis = Getdistance(h1, cen);
    double FocalLength = IQMetricsParameters::FocalLength;
    double pixel_size = IQMetricsParameters::pixel_size * binNum;
    double FovH = atan(pixel_size * dis / FocalLength) * 180.0 / CV_PI;
    return FovH;
}

bool MLIQMetrics::MLFOV::findEdgePts(cv::Mat data, int flag, cv::Point2f &p1, cv::Point2f &p2)
{
    if (flag == 1)
        cv::rotate(data, data, ROTATE_90_COUNTERCLOCKWISE);
    cv::Mat gx, gy;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 1));
    cv::morphologyEx(data, gx, cv::MORPH_GRADIENT, element);
    double maxV;
    cv::minMaxLoc(gx, NULL, &maxV);
    vector<int> xVec = findPeaks(gx, maxV / 3.0, 0, 0);
    if (xVec.size() != 2)
        return false;

    if (flag == 0)
    {
        p1.x = xVec[0];
        p2.x = xVec[1];
    }
    else if (flag == 1)
    {
        p1.y = xVec[0];
        p2.y = xVec[1];
    }
    return true;
}

bool MLIQMetrics::MLFOV::findEdgePts1(cv::Mat data, int flag, cv::Point2f& p1, cv::Point2f& p2)
{
    
    
        if (flag == 1)
            cv::rotate(data, data, ROTATE_90_COUNTERCLOCKWISE);
        cv::Mat gx, gy;
        cv::Mat roi1 = data(cv::Range(0, 1), cv::Range((p1.x) - m_len, p1.x + m_len));
        cv::Mat roi2 = data(cv::Range(0, 1), cv::Range((p2.x) - m_len, p2.x + m_len));
        cv::Mat gx1, gx2;
        cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 1));
        cv::morphologyEx(data, gx, cv::MORPH_GRADIENT, element);
        cv::morphologyEx(roi1, gx1, cv::MORPH_GRADIENT, element);
        cv::morphologyEx(roi2, gx2, cv::MORPH_GRADIENT, element);
        cv::medianBlur(gx1, gx1, 5);
        cv::medianBlur(gx2, gx2, 5);

        double maxV, maxV1, maxV2;
        cv::minMaxLoc(gx, NULL, &maxV);
        cv::minMaxLoc(gx1, NULL, &maxV1);
        cv::minMaxLoc(gx2, NULL, &maxV2);

        vector<int> xVec = findPeaks(gx, maxV / 3.0, 0, 0);
        vector<int> xVec1 = findPeaks(gx1, maxV1 / 3.0, 0, 0);
        vector<int> xVec2 = findPeaks(gx2, maxV2 / 3.0, 0, 0);
        if (xVec1.size() < 1 || xVec2.size() < 1)
            return false;
        //if (xVec.size() != 2)
        //    return false;

        if (flag == 0)
        {
            p1.x = xVec1[0] + (p1.x) - m_len;
            p2.x = xVec2[0] + (p2.x) - m_len;
        }
        else if (flag == 1)
        {
            p1.x = xVec1[0] + (p1.x) - m_len;
            p2.x = xVec2[0] + (p2.x) - m_len;
        }
        return true;
    

}

vector<int> MLIQMetrics::MLFOV::findPeaks(cv::Mat data, double minHeight, int minWidth, double minProminence)
{
    vector<int> xVec;
    // peaks.clear(); // 清空峰值数组
    data.convertTo(data, CV_64FC1);
    int thresh = 1;
    for (int i = thresh; i < data.cols - thresh; ++i)
    {
        // 判断是否为峰值，即比相邻的两个点大，并且大于设定的最小高度
        // for (int i0 = i - thresh;  i0 < i + thresh; i0++)
        //{
        //        if (data.at<double>(0, i) > data.at<double>(0, i0));
        //        break;
        //}
        if (data.at<double>(0, i) > minHeight &&
            (data.at<double>(0, i) > data.at<double>(0, i - 1) && data.at<double>(0, i) >= data.at<double>(0, i + 1)))
        {
            // 寻找峰值的宽度，即峰值到其两侧的极小值点的距离
            int left = i - thresh;
            while (left >= 0 && (data.at<double>(0, left) <= data.at<double>(0, left + thresh)))
            {
                --left;
            }

            int right = i + thresh;
            while (right < data.cols && data.at<double>(0, right) <= data.at<double>(0, right - thresh))
            {
                ++right;
            }

            // 计算峰值的宽度和突出度
            int width = right - left - 2;
            double prominence =
                data.at<double>(0, i) - std::max(data.at<double>(0, left + 1), data.at<double>(0, right - 1));

            // 判断峰值的宽度和突出度是否满足设定的条件
            if (width >= minWidth && prominence > minProminence)
            {
                if (xVec.size() > 0)
                {
                    int sub = i - xVec[xVec.size() - 1];
                    if (sub > 100)
                    {
                        // peaks.push_back({i, data.at<double>(0, i), width, prominence});
                        xVec.push_back(i);
                    }
                }
                else
                {
                    // peaks.push_back({i, data.at<double>(0, i), width, prominence});
                    xVec.push_back(i);
                }
            }
        }
    }
    return xVec;
}

void MLIQMetrics::MLFOV::updateImgdraw(cv::Mat &imgdraw, cv::Point2f h1, int binNum)
{
    circle(imgdraw, h1, 24 / binNum, Scalar(0, 0, 255), -1);
    string xstr = to_string(h1.x);
    string ystr = to_string(h1.y);
    string text = xstr.substr(0, xstr.size() - 4) + "," + ystr.substr(0, ystr.size() - 4);
    cv::putText(imgdraw, text, h1-Point2f(1000 / binNum, 0), FONT_HERSHEY_PLAIN, 8/binNum, Scalar(255, 0, 255), 8/binNum);
}
