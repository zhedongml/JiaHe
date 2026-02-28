#include "pch.h"
#include "ml_CRA.h"
#include <json.hpp>
#include "LogPlus.h"
#include <fstream>
#include"ml_multiCrossHairDetection.h"
#include"IQMetricUtl.h"
using namespace cv;
using Json = nlohmann::json;
using namespace MLIQMetrics;
using namespace MLImageDetection;
 MLCRA::MLCRA()
{
}

 MLCRA::~MLCRA()
{
}

CRARE  MLCRA::getCRA(cv::Mat img)
{
    CRARE re;
    string str = "[------CRA-----]";
    m_img_draw = img.clone();
    if (img.data != NULL)
    {
        m_pixel = IQMetricsParameters::pixel_size;
        m_focallength = IQMetricsParameters::FocalLength;
       /* m_binNum = sqrt((6004 * 7920) / img.total());*/

        cv::Mat img8 = convertToUint8(img);
        cv::Mat img_draw = convertTo3Channels(img8);
        MultiCrossHairDetection mltd;
        CenterCrossHairRe cenRe = mltd.getCenterCrossHairCenter(img8);
        if (!cenRe.flag)
        {
            re.flag = cenRe.flag;
            re.errMsg = cenRe.errMsg;
            return re;
        }
        cv::Mat roi = img8(cenRe.rectCenter).clone();
        CrossCenter cc;
        cv::Point2f center = cc.find_centerGaussian(roi);
        if (abs(center.x) < 1e-6 || abs(center.y) < 1e-6)
        {
            center = cc.find_centerLINES(roi);
        }
        if (abs(center.x) > 1e-6 || abs(center.y) > 1e-6)
        {
            center = center + Point2f(cenRe.rectCenter.tl());
            double x = center.x - m_basePoint.x / m_binNum;
            double y = center.y - m_basePoint.y / m_binNum;
            m_H = x;
            m_V = y;
            Point2f angle = cc.getRotationAngle();
            m_RotationAngle=angle.y;
            m_angleX = atan2(m_H * m_pixel *m_binNum, m_focallength) * 180.0 / CV_PI;
            m_angleY = atan2(m_V * m_pixel*m_binNum, m_focallength) * 180.0 / CV_PI;
            m_distance = sqrt(x * x + y * y);
            m_angle = atan2(m_distance * m_pixel * m_binNum, m_focallength) * 180.0 / CV_PI;
            circle(img_draw, center, 1, Scalar(0, 0, 255), -1);
          //  circle(img_draw, m_basePoint / m_binNum, 5, Scalar(255, 0, 0), -1);
          //  cout << center << endl;
            m_img_draw = img_draw.clone();
            re.angle = m_angle;
            re.angleX = m_angleX;
            re.angleY = m_angleY;
            m_img_draw = img_draw.clone();

            // update the dut offset 
            cv::Point2f offset = center * m_binNum - m_basePoint; 
        }
        else
        {
            //打印log
            string sMsg = "Crosshair Center detect fail!-----Error";
            string errorMessage = str + sMsg;
           // LOG4CPLUS_INFO(LogPlus::getInstance()->logger, errorMessage.c_str());
          //  throw invalid_argument(errorMessage);
            re.flag = false;
            re.errMsg = errorMessage;
        }
    }
    else
    {
        // 打印log
        string sMsg = "Input image is NULL!-----Error";
        string errorMessage = str + sMsg;
       // LOG4CPLUS_INFO(LogPlus::getInstance()->logger, errorMessage.c_str());
        //throw invalid_argument(errorMessage);
        re.flag = false;
        re.errMsg = errorMessage;
    }
    return re; 
}

CRARE MLCRA::getCRA(cv::Mat img, int roationAngle, MirrorALG mirror)
{
    CRARE re;
    string str = "[------CRA-----]";
    m_img_draw = img.clone();

    if (img.data != NULL)
    {
        m_pixel = IQMetricsParameters::pixel_size;
        m_focallength = IQMetricsParameters::FocalLength;
       /* int binNum = sqrt((6004 * 7920) / img.total());
        m_binNum = binNum;   */  
        cv::Mat img8 = convertToUint8(img);
        cv::Mat img_draw = convertTo3Channels(img8);
        MultiCrossHairDetection mltd;
        CenterCrossHairRe cenRe = mltd.getCenterCrossHairCenter(img8);
        if (!cenRe.flag)
        {
            re.flag = cenRe.flag;
            re.errMsg = cenRe.errMsg;
            return re;
        }
        cv::Mat roi = img8(cenRe.rectCenter).clone();
        CrossCenter cc;
        cv::Point2f center = cc.find_centerGaussian(roi);
        if (abs(center.x) < 1e-6 || abs(center.y) < 1e-6)
        {
            center = cc.find_centerLINES(roi);
        }

        // MLLuminance lu;
        cv::Point2f basePoint;// = lu.updateOpticalCenter(m_basePoint / binNum, img.size(), roationAngle, mirror);
        if (abs(center.x) > 1e-6 || abs(center.y) > 1e-6)
        {
            center = center + cv::Point2f(cenRe.rectCenter.tl());
            double x = center.x - basePoint.x;
            double y = center.y - basePoint.y;
            m_H = x;
            m_V = y;
            Point2f angle = cc.getRotationAngle();
            m_RotationAngle = angle.y;
            m_angleX = atan2(m_H * m_pixel * m_binNum, m_focallength) * 180.0 / CV_PI;
            m_angleY = atan2(m_V * m_pixel * m_binNum, m_focallength) * 180.0 / CV_PI;
            m_distance = sqrt(x * x + y * y);
            m_angle = atan2(m_distance * m_pixel * m_binNum, m_focallength) * 180.0 / CV_PI;
            circle(img_draw, center, 5, Scalar(0, 0, 255), -1);
            circle(img_draw, basePoint, 5, Scalar(255, 0, 0), -1);
            m_img_draw = img_draw.clone();
            re.angle = m_angle;
            re.angleX = m_angleX;
            re.angleY = m_angleY;
            m_img_draw = img_draw.clone();
            // update the dut offset
            cv::Point2f offset = center * m_binNum - basePoint* m_binNum;
        }
        else
        {
            //打印log
            string sMsg = "Crosshair Center detect fail!-----Error";
            string errorMessage = str + sMsg;
           // LOG4CPLUS_INFO(LogPlus::getInstance()->logger, errorMessage.c_str());
            //  throw invalid_argument(errorMessage);
            re.flag = false;
            re.errMsg = errorMessage;
        }
    }
    else
    {
        // 打印log
        string sMsg = "Input image is NULL!-----Error";
        string errorMessage = str + sMsg;
       // LOG4CPLUS_INFO(LogPlus::getInstance()->logger, errorMessage.c_str());
        // throw invalid_argument(errorMessage);
        re.flag = false;
        re.errMsg = errorMessage;
    }
    return re; 
}

CRARE MLIQMetrics::MLCRA::getMultiCrossCRA(cv::Mat img, int roationAngle, int eyeLoc, MirrorALG mirror)
{
    CRARE re;
    string str = "[------CRA-----]";
    m_img_draw = img.clone();

    if (img.data != NULL)
    {
        m_pixel = IQMetricsParameters::pixel_size;
        m_focallength = IQMetricsParameters::FocalLength;
        int binNum = IQMetricUtl::instance()->getBinNum(img.size());
        cv::Mat img8 = convertToUint8(img);
        cv::Mat img_draw = convertTo3Channels(img8);
        MultiCrossHairDetection mltd;
       // CenterCrossHairRe cenRe = mltd.getCenterCrossHairCenterByContour(img8,binNum);
        MultiCrossHairRe crossLocRe = mltd.getMuliCrossHairCenterByDistance(img8, eyeLoc, binNum);
        
        
        if (!crossLocRe.flag)
        {
            re.flag = crossLocRe.flag;
            re.errMsg = crossLocRe.errMsg;
            return re;
        }
       // cv::Mat roi = img8(cenRe.rectCenter).clone();
        cv::Mat roi = img8(crossLocRe.rectMap["7"]).clone();
        CrossCenter cc;
        cv::Point2f center = cc.find_centerLINES(roi);
        cv::Point2f basePoint; // = lu.updateOpticalCenter(m_basePoint / binNum, img.size(), roationAngle, mirror);
        if (abs(center.x) > 1e-6 || abs(center.y) > 1e-6)
        {
            center = center + cv::Point2f(crossLocRe.rectMap["7"].tl());
            double x = center.x - basePoint.x;
            double y = center.y - basePoint.y;
            m_H = x;
            m_V = y;
            Point2f angle = cc.getRotationAngle();
            m_RotationAngle = angle.y;
            m_angleX = atan2(m_H * m_pixel * m_binNum, m_focallength) * 180.0 / CV_PI;
            m_angleY = atan2(m_V * m_pixel * m_binNum, m_focallength) * 180.0 / CV_PI;
            m_distance = sqrt(x * x + y * y);
            m_angle = atan2(m_distance * m_pixel * m_binNum, m_focallength) * 180.0 / CV_PI;
            circle(img_draw, center, 5, Scalar(0, 0, 255), -1);
            circle(img_draw, basePoint, 5, Scalar(255, 0, 0), -1);
            m_img_draw = img_draw.clone();
            re.angle = m_angle;
            re.angleX = m_angleX;
            re.angleY = m_angleY;
            m_img_draw = img_draw.clone();
            // update the dut offset
            cv::Point2f offset = center * m_binNum - basePoint * m_binNum;
        }
        else
        {
            //打印log
            string sMsg = "Crosshair Center detect fail!-----Error";
            string errorMessage = str + sMsg;
            // LOG4CPLUS_INFO(LogPlus::getInstance()->logger, errorMessage.c_str());
            //  throw invalid_argument(errorMessage);
            re.flag = false;
            re.errMsg = errorMessage;
        }
    }
    else
    {
        // 打印log
        string sMsg = "Input image is NULL!-----Error";
        string errorMessage = str + sMsg;
        // LOG4CPLUS_INFO(LogPlus::getInstance()->logger, errorMessage.c_str());
        // throw invalid_argument(errorMessage);
        re.flag = false;
        re.errMsg = errorMessage;
    }
    return re; 
}

NineCRARE  MLCRA::getNineCRA(cv::Mat img, int roationAngle, MirrorALG mirror)
{
    NineCRARE re;
    string str = "[------CRA-----]";
    m_img_draw = img.clone();
    if (img.data != NULL)
    {
        m_pixel = IQMetricsParameters::pixel_size;
        m_focallength = IQMetricsParameters::FocalLength;
        /*int binNum = sqrt((6004 * 7920) / img.total());
        m_binNum = binNum;*/
        cv::Mat img8 = convertToUint8(img);
        cv::Mat img_draw = convertTo3Channels(img8);
        MultiCrossHairDetection mltd;
        mltd.setBinNum(m_binNum);
        MultiCrossHairRe crossRE= mltd.getMuliCrossHairCenter(img8, 8/m_binNum,false);
        if (!crossRE.flag)
        {
            re.flag = false;
            re.errorMsg = crossRE.errMsg;
            return re;
        }
        //MLLuminance lu;
        cv::Point2f basePoint = IQMetricsParameters::opticalCenter; // = lu.updateOpticalCenter(m_basePoint / binNum,
                                                               // img.size(), roationAngle, mirror);
        m_img_draw = crossRE.img_draw;
        circle(m_img_draw, basePoint, 5, Scalar( 0, 255, 0), -1);
        map<string, CRARE> craMap;
        map<string, cv::Rect>::iterator it;      
        for (it = crossRE.rectMap.begin();it!=crossRE.rectMap.end();it++)
        {
            string str = it->first;
            cv::Rect rect = it->second;
            CRARE re=calculateCRA(img8, rect, basePoint);
            craMap.insert(std::make_pair(str, re)); 
        }
        re.mapCRA.clear();
        re.mapCRA = craMap;
    }
    else
    {
        // 打印log
        string sMsg = "Input image is NULL!-----Error";
        string errorMessage = str + sMsg;
        //LOG4CPLUS_INFO(LogPlus::getInstance()->logger, errorMessage.c_str());
        // throw invalid_argument(errorMessage);
        re.flag = false;
        re.errorMsg = errorMessage;
    }
    return re;
}

CRARE  MLCRA::calculateCRA(cv::Mat img, cv::Rect rect, cv::Point2f opticalCenter)
{
    CRARE re;
    cv::Mat roi = img(rect).clone();
    cv::Point2f center;
    CrossCenter cc;
    center = cc.find_centerGaussian(roi);
    if (center.x<1e6||center.y<1e-6)
    {
        center = cc.find_centerLINES(roi);
    }
    if (center.x>1e-6&&center.y>1e-6)
    {
        center = center + cv::Point2f(rect.tl());
        double x = center.x - opticalCenter.x;
        double y = center.y - opticalCenter.y;
        //Point2f angle = cc.getRotationAngle();
       // m_RotationAngle = angle.y;
       double  angleX = atan2(x * m_pixel * m_binNum, m_focallength) * 180.0 / CV_PI;
       double angleY = atan2(y * m_pixel * m_binNum, m_focallength) * 180.0 / CV_PI;
       double distance = sqrt(x * x + y * y);
       double  angle = atan2(distance * m_pixel * m_binNum, m_focallength) * 180.0 / CV_PI;
       re.angle = angle;
       re.angleX = angleX;
       re.angleY = angleY;
       re.center = center;
       re.flag = true;
    }
    else
    {
        //打印log
        string sMsg = "Crosshair Center detect fail!-----Error";
        string errorMessage = sMsg;
         //LOG4CPLUS_INFO(LogPlus::getInstance()->logger, errorMessage.c_str());
        //  throw invalid_argument(errorMessage);
        re.flag = false;
        re.errMsg = errorMessage;
    }
    return re;
}

cv::Mat  MLCRA::getImgDraw()
{
    return m_img_draw;
}
double  MLCRA::getCrossHaireRotationAngle()
{
    return m_RotationAngle;
}

double  MLCRA::getAngleX()
{
    return m_angleX;
}

double  MLCRA::getAngleY()
{
    return m_angleY;
}

double  MLCRA::getAngle()
{
    return m_angle;
}

double  MLCRA::getXdistance()
{
    return m_H;
}

double  MLCRA::getYdistance()
{
    return m_V;
}

double  MLCRA::getdistance()
{
    return m_distance;
}






