#include "ml_deadPara.h"
using namespace algorithm;

algorithm::MLDeadPara::MLDeadPara()
{
}

algorithm::MLDeadPara::~MLDeadPara()
{
}

void algorithm::MLDeadPara::SetDeadImg(Mat img)
{
    m_imgDead = img;
    hydrusConfig.GetNewPara(img);
    m_para = hydrusConfig.m_para;

}

DeadPointRe algorithm::MLDeadPara::GetDeadPoints()
{
    DeadPointRe result;
    MLDeadPointDetect deadec;
    if (HydrusParameters::deadPointsAlgorithm == 1)
    {
        result = deadec.DeadPointsDetectHist(m_imgDead);
    }
    else if (HydrusParameters::deadPointsAlgorithm == 2)
    {
        result = deadec.DeadPointsDetectThreshold(m_imgDead);
    }
    else if (HydrusParameters::deadPointsAlgorithm == 3)
    {
        result = deadec.DeadPointsDetectPinto(m_imgDead);
    }
    else if (HydrusParameters::deadPointsAlgorithm == 4)
    {
        result = deadec.DeadPointsDetectMedian(m_imgDead);
    }
    return result;
}

DeadPointRe algorithm::MLDeadPara::GetDeadPointsInf()
{
    DeadPointRe result;
    result = DeadPointsInf(m_imgDead);
    return result;
}

cv::Mat algorithm::MLDeadPara::getImgDraw()
{
    return m_img_draw;
}

DeadPointRe algorithm::MLDeadPara::DeadPointsInf(cv::Mat img, cv::Rect rect)
{
    DeadPointRe re;
    if (img.data != NULL)
    {
        cv::Mat roi, srcbinary1, srcbinary2;
        //GetNewPara(img);
        double m_FocalLength = HydrusParameters::FocalLength;
        double m_pixel_size = m_para.pixel_size;
        double bin = m_para.binNum;
        double dis = m_FocalLength * tan(14 * CV_PI / 180) / m_pixel_size;
        double a = m_FocalLength * tan(0.02 * CV_PI / 180) / m_pixel_size;
        cv::Point2f center;
        center.x = rect.x + rect.width / 2.0;
        center.y = rect.y + rect.height / 2.0;
        //TODO:
        //if (m_center.x != 0 && m_center.y != 0)
        //{
        //    center.x = m_center.x / bin - rect.x;
        //    center.y = m_center.y / bin - rect.y;
        //}
        //else
        //{
        //    GetBoresight();
        //    center.x = m_center.x / bin - rect.x;
        //    center.y = m_center.y / bin - rect.y;
        //}
        cv::Mat img_draw = img.clone();
        // circle(img_draw, center, 3, cv::Scalar(0, 255, 0), -1);
        int top = cv::max(int(center.y - dis / 2), 0);
        int bottom = cv::min(int(center.y + dis / 2), img.rows);
        int left = cv::max(0, int(center.x - dis / 2));
        int right = cv::min(img.cols, int(center.x + dis / 2));
        Rect rect = Rect(left, top, right - left, bottom - top);
        roi = img(rect);
        int row = roi.rows;
        int col = roi.cols;
        int left70 = int(row * 0.15);
        int right70 = int(row * 0.85);
        int top70 = int(col * 0.15);
        int bottom70 = int(col * 0.85);
        cv::Mat imgPart = roi(cv::Range(left70, right70), cv::Range(top70, bottom70));
        cv::Mat imgAll = roi.clone();
        MLDeadPointDetect deadDec;
        DeadPointRe part = deadDec.DeadPoints(imgPart, a);
        DeadPointRe all = deadDec.DeadPoints(imgAll, a);
        re.partAreaNum = part.allAreaNum;
        re.allAreaNum = all.allAreaNum;
        // re.locALL = all.locALL;
        // re.locPart = all.locPart;
        for (int i = 0; i < all.locALL.size(); i++)
        {
            cv::Point2f center;
            center.x = all.locALL[i].x + rect.x;
            center.y = all.locALL[i].y + rect.y;
            cv::circle(img_draw, center, 2, Scalar(0, 255, 0), -1);
            cv::putText(img_draw, std::to_string(i), center, cv::FONT_HERSHEY_SIMPLEX, 0.45, Scalar(0, 230, 0), 1.8);
            re.locALL.push_back(center);
        }
        for (int m = 0; m < part.locPart.size(); m++)
        {
            cv::Point2f center;
            center.x = all.locPart[m].x + rect.x + left70;
            center.y = all.locPart[m].y + rect.y + top70;
            re.locPart.push_back(center);
        }
        re.MarkerImg = img_draw;
        m_img_draw = img_draw.clone();
        // re.MarkerImg = all.MarkerImg;
    }
    return re;

}
