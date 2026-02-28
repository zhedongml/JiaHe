#include "ml_chromaPara.h"
#include <color-util/XYZ_to_Lab.hpp>
#include <fstream>			  
#include <algorithm>
#include <fstream>
#include <iostream>
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include "polygonIntersection.h"
#include "ml_chromaPara.h"
#include "PrjCommon\metricsdata.h"
#include<qdir.h>

#define DEBUG 0
using namespace algorithm;
using namespace std;
algorithm::MLChromaPara::MLChromaPara()
{
}

algorithm::MLChromaPara::~MLChromaPara()
{
}
void algorithm::MLChromaPara::changeRollOffFOVLength(double val)
{
    HydrusParameters::RolloffLength = val;
}
ALGResult algorithm::MLChromaPara::getSLBFilePath(string &filePath, string color)
{
    QString path = "./config/slbLuminanceImgs/";
    QDir dir(path);
    QStringList filename;
    filename << "*.tif"
             << "*.tiff";
    QStringList results = dir.entryList(filename, QDir::Files | QDir::Readable, QDir::Name);

    for (QString name : results)
    {
        if (name.toUpper().startsWith(QString::fromStdString(color).toUpper()))
        {
            filePath = (path + name).toStdString();
            return ALGResult();
        }
    }
    return ALGResult(
        false, QString(path + " dir is not exist SLB %1 image.").arg(QString::fromStdString(color)).toStdString());
}
cv::Rect algorithm::MLChromaPara::getLumiEfficiencyROI(cv::Mat img,float angle)
{
    cv::Rect rect = Rect(0, 0, -1, -1);
    if (img.data != NULL)
    {
        Newpara para = hydrusConfig.GetNewPara(img);
        cv::Mat roi = GetROIMat(img, para.rect);
        cv::Mat roi1 = convertToUint8(roi);
        cv::Mat roi_draw = convertToUint8(roi);
        double bin = para.binNum;
        double focallength = HydrusParameters::FocalLength;
        int dis = focallength * tan(angle/2.0 * CV_PI / 180) / para.pixel_size*2.0;
        cv::Point2f center;
        cv::Rect rec = Rect(0, 0, -1, -1);
        //rec = findborder(roi1);
        double ratio = HydrusParameters::LuminaceActive; 
        getSolidBorder(roi1, rec);
        dis = (rec.width + rec.height) * ratio/2.0;
        if (rec.x != 0 && rec.y != 0)
        {
            center.x = rec.x + rec.width / 2;
            center.y = rec.y + rec.height / 2;
            int top = cv::max((int)center.y - dis / 2, 0);
            int bottom = cv::min((int)center.y + dis / 2, roi.rows);
            int left = cv::max(0, (int)center.x - dis / 2);
            int right = cv::min(roi.cols, (int)center.x + dis / 2);
            cv::Rect rectangle = Rect(left, top, right - left, bottom - top);
            cv::rectangle(roi_draw, rectangle, Scalar(255, 0, 255), 1);
            rect = rectangle;
            rect.x = rectangle.x + para.rect.x;
            rect.y = rectangle.y + para.rect.y;
        }
        else
        {
            cv::Point cen = HydrusParameters::W_ND0_opticalcenter / bin;
            int top = cen.y - dis / 2;
            int bottom = cen.y + dis / 2;
            int left = cen.x - dis / 2;
            int right = cen.x + dis / 2;
            rect = Rect(left, top, right - left, bottom - top);
        }
    }
    return rect;
}
void algorithm::MLChromaPara::changeWhitepointBrightnesslevel(double val)
{
    HydrusParameters::brightnesslevel = val;
}
void algorithm::MLChromaPara::SetRotationDeg(float degree)
{
    m_rotationAngle = degree;
}
void algorithm::MLChromaPara::SetFovInform(float centerX, float centerY, std::vector<double> rotateH, std::vector<double> rotateV)
{
    m_center.x = centerX;
    m_center.y = centerY;
    m_Hline = rotateH;
    m_Vline = rotateV;
}

void algorithm::MLChromaPara::SetXYZImage(cv::Mat X, cv::Mat Y, cv::Mat Z)
{
    m_X = X;
    m_Y = Y;
    m_Z = Z;
    hydrusConfig.GetNewPara(X);
    m_para = hydrusConfig.m_para;
}

WhitePointRe algorithm::MLChromaPara::GetCIELABCoordinates()
{
    WhitePointRe re;
    re = whitePoint(m_X, m_Y, m_Z);
    return re;
}

double algorithm::MLChromaPara::GetColorDistance()
{
    if (m_whitePointRe.dCavg != 0)
    {
        return m_whitePointRe.dCavg;
    }
    else {
        whitePoint(m_X, m_Y, m_Z);
        return m_whitePointRe.dCavg;
    }
}

double algorithm::MLChromaPara::GetColorDistanceAvg()
{
    if (m_whitePointRe.dCmean != 0)
    {
        return m_whitePointRe.dCmean;
    }
    else
    {
        whitePoint(m_X, m_Y, m_Z);
        return m_whitePointRe.dCmean;
    }
}

double algorithm::MLChromaPara::GetColorDistanceMax()
{
    if (m_whitePointRe.dCmax != 0)
    {
        return m_whitePointRe.dCmax;
    }
    else
    {
        whitePoint(m_X, m_Y, m_Z);
        return m_whitePointRe.dCmax;
    }
}

void algorithm::MLChromaPara::SetWRGBHImage(Mat img_WRGB, Mat img_black)
{
    m_img_WRGB = img_WRGB;
    m_img_black = img_black;
    hydrusConfig.GetNewPara(img_WRGB);
    m_para = hydrusConfig.m_para;
}

SeqCRRe algorithm::MLChromaPara::GetSequentialContrast()
{
    SeqCRRe result;
    result = SequentialContrast(m_img_WRGB, m_img_black);
    return result;
}

double algorithm::MLChromaPara::GetSequentialContrastAverage()
{
    if (m_Secr.seqCR != 0)
    {
        return m_Secr.seqCR;
    }
    else {
        SequentialContrast(m_img_WRGB, m_img_black);
        return m_Secr.seqCR;
    }
}

cv::Mat algorithm::MLChromaPara::GetColorROIMat()
{
    if (m_Secr.ColorROIMat.data != NULL)
    {
        return m_Secr.ColorROIMat;
    }
    else
    {
        SequentialContrast(m_img_WRGB, m_img_black);
        return m_Secr.ColorROIMat;
    }
}

cv::Mat algorithm::MLChromaPara::GetBlackROIMat()
{
    if (m_Secr.BlackROIMat.data != NULL)
    {
        return m_Secr.BlackROIMat;
    }
    else
    {
        SequentialContrast(m_img_WRGB, m_img_black);
        return m_Secr.BlackROIMat;
    }
}

void algorithm::MLChromaPara::SetcolorIMG(cv::Mat img_color)
{
    m_img_color = img_color;
    hydrusConfig.GetNewPara(img_color);
    m_para = hydrusConfig.m_para;
}

LuminanceRe algorithm::MLChromaPara::GetLuminanceUniformity()
{
    LuminanceRe result;
    result = LuminanceUniformity(m_img_color);
    return result;
}

LumiEfficencyRe algorithm::MLChromaPara::GetLuminanceEfficiency(cv::Mat img, string color, float angle)
{
        LumiEfficencyRe re;   
        double efficiency = -1;
        string str = "ALG Error-----[getLuminanceEfficiency]-----";
        string slbpath = "./config/slbLuminanceImgs/" + color + "_solid_1_YY.tif";
        ALGResult result = getSLBFilePath(slbpath, color);
        if (!result.success)
        {
            re.flag = result;
            return re;
        }
        cv::Mat slb = cv::imread(slbpath, -1);
        if (img.data!=NULL&&slb.data!=NULL)
        {
            m_img_draw.release();
            if (slb.size() != img.size())
                cv::resize(slb, slb, img.size());
             double rotationAngle = HydrusParameters::roationAngle;
            img = getRotationImg(img, rotationAngle);
             if (m_rotationAngle == 0)
                img = getSolidImgRotated(img);
             else
            img = getRotationImg(img, m_rotationAngle);
             // if the rotated  slb image
            slb = getSolidImgRotated(slb);
           // slb = getRotationImg(slb, rotationAngle);
            cv::Mat efficiencyMat = img / slb;
            re.efficiencyMat = efficiencyMat;
            cv::Rect rectDUT = getLumiEfficiencyROI(img,angle);
            cv::Rect rectSLB = getLumiEfficiencyROI(slb, angle);

            if (rectDUT.x != 0 && rectDUT.y != 0)
            {
                cv::Mat dutROI = img(rectDUT).clone();
                cv::Mat slbROI = slb(rectSLB).clone();
                cv::Mat img_draw = convertToUint8(img);
                cv::Mat slb_draw = convertToUint8(slb);
                cv::rectangle(img_draw, rectDUT, Scalar(255, 0, 255), 2);
                cv::rectangle(slb_draw, rectSLB, Scalar(255, 0, 255), 2);
                m_img_draw = img_draw.clone();
                re.slb_draw = slb_draw.clone();
                cv::Scalar dutMean = cv::mean(dutROI);
                cv::Scalar slbMean = cv::mean(slbROI);
                re.efficicncy = dutMean(0) / slbMean(0)*100;
                m_img_draw = img_draw.clone();
            }        
        }
        else
        {
            re.flag.success = false;
            string sMsg = "The ROI is out of the boundary!";
            string errorMessage = str + sMsg;
           // LOG4CPLUS_INFO(LogPlus::getInstance()->logger, errorMessage.c_str());
            re.flag.errorMsg = sMsg;        
        }
    
    return re;
}

std::vector<double> algorithm::MLChromaPara::GetLuminanceMinMaxPer()
{
    if (m_luminance.MinMaxPer.size() > 0) {
        return m_luminance.MinMaxPer;
    }
    else {
        LuminanceUniformity(m_img_color);
        return m_luminance.MinMaxPer;
    }
}

std::vector<double> algorithm::MLChromaPara::GetLuminanceMinMaxAvg()
{
    if (m_luminance.MinMaxAvg.size() > 0) {
        return m_luminance.MinMaxAvg;
    }
    else
    {
        LuminanceUniformity(m_img_color);
        return m_luminance.MinMaxAvg;
    }
}

std::vector<double> algorithm::MLChromaPara::GetLuminanceRolloff()
{
    std::vector<double> re;
   // re = LuminanceRolloff(m_img_color);
    re = LuminanceRolloffRotation(m_img_color);
    return re;
}


void algorithm::MLChromaPara::SetcolorMat(std::vector<cv::Mat> R, std::vector<cv::Mat> G, std::vector<cv::Mat> B)
{
    if (R.size() == 2 && G.size() == 2 && B.size() == 2)
    {
        m_rgbxy.m_xy_R.clear();
        m_rgbxy.m_xy_G.clear();
        m_rgbxy.m_xy_B.clear();
        hydrusConfig.GetNewPara(R[0]);
        m_para = hydrusConfig.m_para;
        cv::Rect rectangle = m_para.rect;//GetNewPara(R[0]).rect;
        if (rectangle.x != 0 || rectangle.y != 0)
        {
            for (int i = 0; i < R.size(); i++)
            {
                m_rgbxy.m_xy_R.push_back(R[i](rectangle));
                m_rgbxy.m_xy_G.push_back(G[i](rectangle));
                m_rgbxy.m_xy_B.push_back(B[i](rectangle));
            }
        }
        else
        {
            m_rgbxy.m_xy_R = R;
            m_rgbxy.m_xy_G = G;
            m_rgbxy.m_xy_B = B;
        }
    }

}

void algorithm::MLChromaPara::SetChromaMat(cv::Mat Colorx, cv::Mat Colory)
{
    m_Colorx = Colorx;
    m_Colory = Colory;
    hydrusConfig.GetNewPara(Colorx);
    m_para = hydrusConfig.m_para;
}

double algorithm::MLChromaPara::GetColorGamut()
{
    double re;
    re = ColorGamut(m_rgbxy);
    return re;
}

double algorithm::MLChromaPara::ColorGamut(RGBXYZ m_rgbXYZ)
{
    double areaRetio = -1;
    if (m_rgbxy.m_xy_R.size() == 2 & m_rgbxy.m_xy_G.size() == 2 & m_rgbxy.m_xy_B.size() == 2)
    {
        //double rotationAngle = HydrusParameters::roationAngle;
        //m_rgbxy.m_xy_R[0] = getRotationImg(m_rgbxy.m_xy_R[0], rotationAngle);
        Point2f r, g, b;
        double rx, ry, gx, gy, bx, by;
        cv::Rect rec = m_para.rect;
        double bin = m_para.binNum;
        cv::Point2f center;
        if (m_center.x != 0 && m_center.y != 0)
        {
            center.x = m_center.x / bin - rec.x;
            center.y = m_center.y / bin - rec.y;
        }
        else
        {
            m_center = HydrusParameters::W_ND0_opticalcenter;
            center.x = m_center.x / bin - rec.x;
            center.y = m_center.y / bin - rec.y;
        }
        r = getcoordinate(m_rgbxy.m_xy_R, center, bin, rec);
        g = getcoordinate(m_rgbxy.m_xy_G, center, bin, rec);
        b = getcoordinate(m_rgbxy.m_xy_B, center, bin, rec);
        //rx = r.x;
        //ry = r.y;
        //gx = g.x;
        //gy = g.y;
        //bx = b.x;
        //by = b.y;
        rx = 0.6400;
        ry = 0.3300;
        gx = 0.3000;
        gy = 0.6000;
        bx = 0.1500;
        by = 0.0600;
        cv::Point2f p1[300], p2[300];
        p1[0] = r;
        p1[1] = g;
        p1[2] = b;
        p2[0] = { 0.6400, 0.3300 };
        p2[1] = { 0.3000, 0.6000 };
        p2[2] = { 0.1500, 0.0600 };
        float Area = 0;
        MLpolygonIntersection intersection;
        Area = intersection.SPIA(p1, p2, 3, 3);
        //qCritical() << "rgbCoordinate: " << rx << "," << ry <<"," << gx<< "," << gy<< "," << bx<< "," << by;
        double area = 0; //S=(1/2)*(x1y2+x2y3+x3y1-x1y3-x2y1-x3y2)
        area = 0.5 * abs(rx * gy + gx * by + bx * ry - rx * by - gx * ry - bx * gy);
        areaRetio = 100 * Area / area;
    }
    return areaRetio;
}

ChromaRe algorithm::MLChromaPara::ChromaMat(cv::Mat ColorX, cv::Mat ColorY)
{
    if (ColorX.data != NULL && ColorY.data != NULL)
    {
        //ColorX = getRotationImg(ColorX, );
        cv::Mat xMat, yMat;
        cv::Rect rec = m_para.rect;//GetNewPara(ColorX).rect;
        xMat = GetROIMat(ColorX, rec);
        yMat = GetROIMat(ColorY, rec);
        double bin = m_para.binNum;
        cv::Mat Area90Matx, Area90Maty;
        cv::Mat xROIMat = getROIsMeanMat(xMat, Area90Matx, bin, rec);
        m_Chromax = m_img_draw.clone();
        cv::Mat yROIMat = getROIsMeanMat(yMat, Area90Maty, bin, rec);
        m_Chromay = m_img_draw.clone();
        m_Chroma.Chromax = xROIMat;
        m_Chroma.Chromay = yROIMat;
    }
    return m_Chroma;

}

cv::Mat algorithm::MLChromaPara::GetChromax()
{
    ChromaMat(m_Colorx, m_Colory);
    return m_Chroma.Chromax;
}

cv::Mat algorithm::MLChromaPara::GetChromay()
{
    if (m_Chroma.Chromay.data != NULL)
    {
        return m_Chroma.Chromay;
    }
    else
    {
        ChromaMat(m_Colorx, m_Colory);
        return m_Chroma.Chromay;
    }
}

cv::Mat algorithm::MLChromaPara::getImgDraw()
{
    return m_img_draw;
}

cv::Mat algorithm::MLChromaPara::getColorDraw()
{
    return m_img_color;
}

cv::Mat algorithm::MLChromaPara::getBlackDraw()
{
    return m_img_black;
}

cv::Mat algorithm::MLChromaPara::getChromaxDraw()
{
    return m_Chromax;
}

cv::Mat algorithm::MLChromaPara::getChromayDraw()
{
    return m_Chromay;
}

WhitePointRe algorithm::MLChromaPara::whitePoint(cv::Mat X, cv::Mat Y, cv::Mat Z)
{
    if (X.data != NULL & Y.data != NULL & Z.data != NULL & X.size() == Y.size() & X.size() == Z.size())
    {
        double rotationAngle = HydrusParameters::roationAngle;
        X = getRotationImg(X, rotationAngle);
        Y = getRotationImg(Y, rotationAngle);
        Z = getRotationImg(Z, rotationAngle);
        X = getRotationImg(X, m_rotationAngle);
        Y = getRotationImg(Y, m_rotationAngle);
        Z = getRotationImg(Z, m_rotationAngle);


        cv::Rect rectangle = m_para.rect;//GetNewPara(X).rect;
        if (rectangle.x != 0 || rectangle.y != 0)
        {
            X = X(rectangle);
            Y = Y(rectangle);
            Z = Z(rectangle);
        }
        double m_FocalLength = HydrusParameters::FocalLength;
        // double m_pixel_size = KunLunParameters::pixel_size;
        double m_pixel_size = m_para.pixel_size;
        int roidegrees = HydrusParameters::degrees;
        int dis = m_FocalLength * tan(roidegrees * CV_PI / 180) / m_pixel_size;
        cv::Point2f center;
        if (DEBUG == 1)
        {
            if (m_center.x > 1e-6 || m_center.y > 1e-6)
            {
                center = m_center / m_para.binNum - Point2f(rectangle.x, rectangle.y);
            }
            else
            {
                center = HydrusParameters::W_ND0_opticalcenter / m_para.binNum - Point2f(rectangle.x, rectangle.y);
            }
        }
        else
        {
            cv::Rect rec = findborder(X);
            center = cv::Point2f(rec.x + rec.width / 2, rec.y + rec.height / 2);
        }
        //cv::Point2f center((float)(X.cols / 2), (float)(X.rows / 2));
        //cv::Mat affine_matrix = getRotationMatrix2D(center, m_rotationDeg, 1.0);
        //warpAffine(X, X, affine_matrix, X.size());
        //warpAffine(Y, Y, affine_matrix, X.size());
        //warpAffine(Z, Z, affine_matrix, X.size());
        cv::Mat X_draw = convertToUint8(X);
        cv::Mat Y_draw = convertToUint8(Y);
        cv::Mat Z_draw = convertToUint8(Z);

        int top = cv::max((int)center.y - dis / 2, 0);
        int bottom = cv::min((int)center.y + dis / 2, X.rows);
        int left = cv::max(0, (int)center.x - dis / 2);
        int right = cv::min(X.cols, (int)center.x + dis / 2);
        Rect rect = Rect(left, top, right - left, bottom - top);
        cv::rectangle(X_draw, rect, Scalar(255, 0, 255), 2);
        cv::rectangle(Y_draw, rect, Scalar(255, 0, 255), 2);
        cv::rectangle(Z_draw, rect, Scalar(255, 0, 255), 2);
        m_img_draw = X_draw.clone();

        cv::Mat xROI = GetROIMat(X, rect);
        cv::Mat yROI = GetROIMat(Y, rect);
        cv::Mat zROI = GetROIMat(Z, rect);

        int top1 = cv::max((int)center.y - 50, 0);
        int bottom1 = cv::min((int)center.y + 50, X.rows);
        int left1 = cv::max(0, (int)center.x - 50);
        int right1 = cv::min(X.cols, (int)center.x + 50);
        Rect rec = Rect(left1, top1, right1 - left1, bottom1 - top1);
        cv::Mat YcenterROI = Y(rec);
        cv::Scalar mean0 = cv::mean(YcenterROI);
        cv::rectangle(Y_draw, rec, Scalar(255, 0, 255), 2);

        cv::Mat xDown, yDown, zDown;
        int pyDownNum = HydrusParameters::downSampling; // m_whitePointRe.pyDownNum;//24;
        cv::resize(xROI, xDown, cv::Size(pyDownNum, pyDownNum));
        cv::resize(yROI, yDown, cv::Size(pyDownNum, pyDownNum));
        cv::resize(zROI, zDown, cv::Size(pyDownNum, pyDownNum));

        cv::Mat LMat(xDown.size(), CV_32FC1);
        cv::Mat aMat(xDown.size(), CV_32FC1);
        cv::Mat bMat(xDown.size(), CV_32FC1);
        int brightnesslevel = HydrusParameters::brightnesslevel;

        cv::Mat XMat = 100*xDown / mean0(0);
        cv::Mat YMat = 100*yDown / mean0(0);
        cv::Mat ZMat = 100*zDown / mean0(0);

        for (int i = 0; i < xDown.rows; i++)
        {
            for (int j = 0; j < xDown.cols; j++)
            {
                double x, y, z;
                if (XMat.type() == CV_8U)
                {
                    x = XMat.at<uchar>(i, j);
                    y = YMat.at<uchar>(i, j);
                    z = ZMat.at<uchar>(i, j);
                }
                else if (XMat.type() == CV_16U)
                {
                    x = XMat.at<short>(i, j);
                    y = YMat.at<short>(i, j);
                    z = ZMat.at<short>(i, j);
                }
                else if (XMat.type() == CV_32F)
                {
                    x = XMat.at<float>(i, j);
                    y = YMat.at<float>(i, j);
                    z = ZMat.at<float>(i, j);
                }
                const colorutil::RGB xyz_color(x, y, z);
                const colorutil::Lab lab_color = colorutil::convert_XYZ_to_Lab(xyz_color);
                double l = lab_color(0);
                double a = lab_color(1);
                double b = lab_color(2);
                LMat.at<float>(i, j) = l;
                aMat.at<float>(i, j) = a;
                bMat.at<float>(i, j) = b;
            }
        }
        cv::Scalar aMean, bMean, LMean;
        aMean = cv::mean(aMat);
        bMean = cv::mean(bMat);
        LMean = cv::mean(LMat);
        double dCavg = sqrt(aMean(0) * aMean(0) + bMean(0) * bMean(0));
        cv::Mat dCmn(aMat.rows, aMat.cols, CV_32FC1);
        for (int i = 0; i < aMat.rows; i++)
        {
            for (int j = 0; j < aMat.cols; j++)
            {
                double atmp = aMat.at<float>(i, j) - aMean(0);
                double btmp = bMat.at<float>(i, j) - bMean(0);
                float dc = sqrt(atmp * atmp + btmp * btmp);
                dCmn.at<float>(i, j) = dc;
            }
        }
        cv::Scalar mean;
        mean = cv::mean(dCmn);
        double dCmean = mean(0);
        double minVal, maxVal;
        cv::minMaxIdx(dCmn, &minVal, &maxVal);
        double dCmax = maxVal;
        double dCmin = minVal;
        m_whitePointRe.dCmax = dCmax;
        m_whitePointRe.dCmin = dCmin;
        m_whitePointRe.dCavg = dCavg;
        m_whitePointRe.dCmean = dCmean;
        m_whitePointRe.aMean = aMean(0);
        m_whitePointRe.bMean = bMean(0);
        m_whitePointRe.LMean = LMean(0);
        m_whitePointRe.color_uniformity = dCmax / dCmean;
        m_whitePointRe.dCmn = dCmn;
        m_whitePointRe.lMat = LMat;
        m_whitePointRe.aMat = aMat;
        m_whitePointRe.bMat = bMat;
    }
    return m_whitePointRe;
}

SeqCRRe algorithm::MLChromaPara::SequentialContrast(Mat img_WRGB, Mat img_black)
{
    double sc = 0;
    vector<double> crs;
    SeqCRRe re;
    if (img_WRGB.data != NULL && img_black.data != NULL)
    {
        m_img_color.release();
        m_img_black.release();
        double rotationAngle = HydrusParameters::roationAngle;
        img_WRGB = getRotationImg(img_WRGB, rotationAngle);
        img_WRGB = getRotationImg(img_WRGB, m_rotationAngle);
        img_black = getRotationImg(img_black, rotationAngle);
        img_black = getRotationImg(img_black, m_rotationAngle);

        cv::Rect rect = m_para.rect;//GetNewPara(img_WRGB).rect;
        double bin = m_para.binNum;
        cv::Mat colorROI = GetROIMat(img_WRGB, rect);
        cv::Mat blackROI = GetROIMat(img_black, rect);

        cv::Point2f center;
        if (m_center.x > 1e-6 || m_center.y > 1e-6){
            center = m_center / bin - Point2f(rect.x, rect.y);
        }
        else{
            center = HydrusParameters::W_ND0_opticalcenter / bin - Point2f(rect.x, rect.y);
        }
        //Point2f center((float)(colorROI.cols / 2), (float)(colorROI.rows / 2));
        //Mat affine_matrix = getRotationMatrix2D(center, m_rotationDeg, 1.0);
        //warpAffine(colorROI, colorROI, affine_matrix, colorROI.size());
        //warpAffine(blackROI, blackROI, affine_matrix, colorROI.size());
        cv::Mat colorROI8 = convertToUint8(colorROI);
        cv::Mat blackROI8 = convertToUint8(blackROI);

        // cv::Mat blackROIsMat = getROIsMeanMat(blackROI, Area90MatBlack, bin, rect);
        // m_black_draw = m_img_draw.clone();
        cv::Mat Area90MatColor, Area90MatBlack, Area90MatColor32, Area90MatBlack32;
         m_img_color = m_img_draw;
        m_img_black = convertToUint8(blackROI8);
        cv::Mat colorROIsMat = getROIsMeanMat(colorROI, Area90MatColor, bin, rect);
   
        if (!m_Area90Rect.empty())
        {
            Area90MatBlack = blackROI(m_Area90Rect);
            Area90MatColor = colorROI(m_Area90Rect);
        }
        vector<cv::Mat> Rois;
        cv::Mat blackROIsMat = cv::Mat::zeros(Size(3, 3), CV_32FC1);
        for (int i = 0; i < m_RoisRect.size(); i++)
        {
            Rois.push_back(blackROI(m_RoisRect[i]));
            //cv::rectangle(blackROI8, m_RoisRect[i], cv::Scalar(255, 0, 255), 2);
        }
       // cv::rectangle(blackROI8, m_RoisRect[i], cv::Scalar(255, 0, 255), 2);
       // m_img_black = blackROI8.clone();
        // m_black_draw = m_img_draw.clone();
        vector<double> XROI;
        if (Rois.size() == 9)
        {
            for (int i = 0; i < Rois.size(); i++)
            {
                cv::Scalar rmean;
                rmean = cv::mean(Rois[i]);
                XROI.push_back(rmean(0));
            }
            int n = 0;
            if (XROI.size() == 9)
            {
                for (int i = 0; i < blackROIsMat.rows; i++)
                {
                    for (int j = 0; j < blackROIsMat.cols; j++)
                    {
                        blackROIsMat.at<float>(i, j) = XROI[n];
                        n++;
                    }
                }
            }
        }
        cv::Mat seqROICR = cv::Mat::zeros(colorROIsMat.size(), CV_32FC1);
        cv::Mat seq90Mat = cv::Mat::zeros(Area90MatColor.size(), CV_32FC1);
        
        //test
       // MetricsData md;
        //if (MetricsData::instance()->getIQSLB()==false)
        //{
        //    colorROIsMat = colorROIsMat - 120;
        //    blackROIsMat = blackROIsMat - 120;
        //}
        double seqCR;
        if (colorROIsMat.data != NULL && blackROIsMat.data != NULL &&
            Area90MatColor.data != NULL && Area90MatBlack.data != NULL)
        {
   

            cv::divide(colorROIsMat, blackROIsMat, seqROICR, 1);
            // add the 4000
            seqROICR.setTo(3999.99, seqROICR >= 4000);

            cv::Scalar mean;
            mean = cv::mean(seqROICR);
            sc = mean(0);
            cv::Mat Area90MatColor32 = cv::Mat::zeros(Area90MatColor.size(), CV_32FC1);
            cv::Mat Area90MatBlack32 = cv::Mat::zeros(Area90MatBlack.size(), CV_32FC1);
            Area90MatColor.convertTo(Area90MatColor32, CV_32FC1);
            Area90MatBlack.convertTo(Area90MatBlack32, CV_32FC1);
            cv::divide(Area90MatColor32, Area90MatBlack32, seq90Mat, 1);
           
            Scalar blackM = cv::mean(Area90MatBlack);
            Scalar colorM = cv::mean(Area90MatColor);
            
            if (blackM(0) <= 0)
                seqCR = 3999.9;
            else
                seqCR = colorM(0) / blackM(0);

        }
        re.Roicr = seqROICR;
      //  re.seqCR = sc;
        re.seqCR = seqCR;

        re.seq90Mat = seq90Mat;
        re.ColorROIMat = colorROIsMat;
        re.BlackROIMat = blackROIsMat;
        m_Secr = re;
    }
    return m_Secr;
}

cv::Mat algorithm::MLChromaPara::getROIsMeanMat(cv::Mat img, cv::Mat& Area90Mat, double bin, cv::Rect rec)
{
    cv::Mat ROIsMat = cv::Mat::zeros(Size(3, 3), CV_32FC1);
    if (img.data != NULL)
    {
        cv::Rect Area90Rcet;
        cv::Mat img8 = convertToUint8(img);
        vector<cv::Rect> RoisRect;
        vector<cv::Mat> Rois;
        bool debugflag = true;
        if (debugflag == false)
        {
            RoisRect = Findrois(img8, Area90Rcet, bin, rec);
        }
        else if (debugflag == true)
        {
            RoisRect = FindroisBorder(img8, Area90Rcet, bin);
        }

        string dir, filepath, filepath1;
        if (DEBUG == 1)
        {
            dir = "E:\\project\\hydrus2\\mtfiq\\src\\app\\config\\ROIInfo\\";
        }
        else if (DEBUG == 0)
        {
            string dir = "./config/ROIInfo/";
        }
        filepath = dir + "seqROI.txt";
        filepath1 = dir + "Area90Rcet.txt";

        vector<Rect> roiVec;
        if (Area90Rcet.area() > 0)
        {
            roiVec.push_back(Area90Rcet);
        }
        writeROIFile(filepath1, roiVec);
        writeROIFile(filepath, RoisRect);

        if (Area90Rcet.area() < 1)
        {
            readROItxtFile(filepath1, roiVec);
            readROItxtFile(filepath, RoisRect);
            Area90Rcet = roiVec[0];
        }
        Area90Mat = img(Area90Rcet);
        m_img_draw = img8.clone();
        cv::rectangle(m_img_draw, Area90Rcet, Scalar(255, 0, 255), 1);
        if (m_img_black.data!=NULL)
        cv::rectangle(m_img_black, Area90Rcet, Scalar(255, 0, 255), 1);

        for (int i = 0; i < RoisRect.size(); i++)
        {
            Rois.push_back(img(RoisRect[i]));
            //cv::rectangle(m_img_draw, RoisRect[i], cv::Scalar(255, 0, 255), 2);
        }
        m_RoisRect = RoisRect;
        m_Area90Rect = Area90Rcet;
        vector<double> XROI;
        if (Rois.size() == 9)
        {
            for (int i = 0; i < Rois.size(); i++)
            {
                cv::Scalar rmean;
                rmean = cv::mean(Rois[i]);
                XROI.push_back(rmean(0));
            }
            int n = 0;
            if (XROI.size() == 9)
            {
                for (int i = 0; i < ROIsMat.rows; i++)
                {
                    for (int j = 0; j < ROIsMat.cols; j++)
                    {
                        ROIsMat.at<float>(i, j) = XROI[n];
                        n++;
                    }
                }
            }
        }
    }
    return ROIsMat;
}

std::vector<cv::Rect> algorithm::MLChromaPara::Findrois(Mat img, cv::Rect& Area90Rect, double bin, cv::Rect rect)
{

    vector<cv::Rect> roisRect;
    if (img.data != NULL)
    {
        // cv::Rect rec = findborder(img, rect);
        Mat img_draw, img1;
        if (img.depth() == CV_8U)
        {
            img1 = img.clone();
            img_draw = img.clone();
        }
        else
        {
            img1 = convertToUint8(img);
            img_draw = convertToUint8(img);
        }
        double m_FocalLength = HydrusParameters::FocalLength;
        double m_pixel_size = HydrusParameters::pixel_size;
        double pixel_size = m_pixel_size * bin;

        int dis = m_FocalLength * tan(14.1 * 0.9 * CV_PI / 180) / pixel_size;
        cv::Point2f center;
        double k1 = 0, k2 = 0;
        if (m_center.x != 0 && m_center.y != 0 && m_Vline.size() == 3 && m_Hline.size() == 3)
        {
            center.x = m_center.x / bin - rect.x;
            center.y = m_center.y / bin - rect.y;
            k2 = -m_Vline[0] / m_Vline[1];
            k1 = -m_Hline[0] / m_Hline[1];
        }
        else
        {
            m_center = HydrusParameters::W_ND0_opticalcenter;
            center.x = m_center.x / bin - rect.x;
            center.y = m_center.y / bin - rect.y;
            k2 = 1e6;
            k1 = 0;
        }
        circle(img_draw, center, 3, cv::Scalar(0, 255, 0), -1);
        double bh, bv, b1, b2, b3, b4;
        bh = center.y - k1 * center.x;
        bv = center.y - k2 * center.x;

        b1 = center.y - dis / 2.0 - k1 * center.x;
        b2 = center.y + dis / 2.0 - k1 * center.x;
        b3 = center.y - k2 * (center.x + dis / 2.0);
        b4 = center.y - k2 * (center.x - dis / 2.0);

        //vector<cv::Point> point1, point2, point3, point4;
        //point1 = drawline(k1, b1);
        //point2 = drawline(k1, b2);
        //point3 = drawline(k2, b3);
        //point4 = drawline(k2, b4);
        //cv::line(img_draw, point1[0], point1[1], cv::Scalar(0, 255, 0), 1, 8, 0);
        //cv::line(img_draw, point2[0], point2[1], cv::Scalar(0, 255, 0), 1, 8, 0);
        //cv::line(img_draw, point3[0], point3[1], cv::Scalar(0, 255, 0), 1, 8, 0);
        //cv::line(img_draw, point4[0], point4[1], cv::Scalar(0, 255, 0), 1, 8, 0);
        std::vector<cv::Point2f> intersection;
        cv::Point2f intersection0, intersection1, intersection2, intersection3, intersection4, intersection5,
            intersection6, intersection7, intersection8;
        intersection2 = getintersection(k1, b1, k2, b3);
        intersection0 = getintersection(k1, b1, k2, b4);
        intersection8 = getintersection(k1, b2, k2, b3);
        intersection6 = getintersection(k1, b2, k2, b4);
        intersection1 = (intersection0 + intersection2) / 2.0;
        intersection3 = (intersection0 + intersection6) / 2.0;
        intersection4 = (intersection0 + intersection2 + intersection6 + intersection8) / 4.0;
        intersection5 = (intersection8 + intersection2) / 2.0;
        intersection7 = (intersection6 + intersection8) / 2.0;
        intersection.push_back(intersection0);
        intersection.push_back(intersection1);
        intersection.push_back(intersection2);
        intersection.push_back(intersection3);
        intersection.push_back(intersection4);
        intersection.push_back(intersection5);
        intersection.push_back(intersection6);
        intersection.push_back(intersection7);
        intersection.push_back(intersection8);

        double width = Getdistance(intersection0, intersection2);
        double height = Getdistance(intersection0, intersection6);
        int top = cv::max(int(center.y - height / 2.0), 0);
        int bottom = cv::min(int(center.y + height / 2.0), img.rows);
        int left = cv::max(0, int(center.x - width / 2.0));
        int right = cv::min(img.cols, int(center.x + width / 2.0));
        Rect rect90 = Rect(left, top, right - left, bottom - top);
        cv::rectangle(img_draw, rect90, Scalar(0, 0, 255), 1);
        Area90Rect = rect90;

        m_img_draw = convertToUint8(img);
        for (int i = 0; i < intersection.size(); i++)
        {
            double p = m_FocalLength * tan(CV_PI / 180) / pixel_size;
            Point2f temp = Point2f(intersection[i].x - p / 2, intersection[i].y - p / 2);
            int top = cv::max(int(temp.y), 0);
            int bottom = cv::min(int(temp.y + p), img.rows);
            int left = cv::max(0, int(temp.x));
            int right = cv::min(img.cols, int(temp.x + p));
            Rect e_rect = Rect(left, top, right - left, bottom - top);
            cv::rectangle(img_draw, e_rect, cv::Scalar(0, 0, 255), 2);
            Mat eroi = img(Rect(e_rect));
            roisRect.push_back(e_rect);
            m_img_draw = img_draw.clone();
        }
    }
    return roisRect;

}

LuminanceRe algorithm::MLChromaPara::LuminanceUniformity(Mat img)
{
    LuminanceRe re;
    if (img.data != NULL)
    {
        m_img_draw.release();
        if (m_rotationAngle == 0)
            img = getSolidImgRotated(img);
        double rotationAngle = HydrusParameters::roationAngle;
        img = getRotationImg(img, rotationAngle);
        img = getRotationImg(img, m_rotationAngle);

        cv::Rect rect = m_para.rect;//GetNewPara(img).rect;
        cv:Mat roi = GetROIMat(img, rect);
        cv::Mat img_draw = roi.clone();
        double bin = m_para.binNum;
        cv::Mat Area90Mat;

        cv::Point2f center;
        if (m_center.x > 1e-6 || m_center.y > 1e-6){
            center = m_center / bin - Point2f(rect.x, rect.y);
        }
        else{
            center = HydrusParameters::W_ND0_opticalcenter / bin - Point2f(rect.x, rect.y);
        }
        //Point2f center((float)(roi.cols / 2), (float)(roi.rows / 2));
        //Mat affine_matrix = getRotationMatrix2D(center, m_rotationDeg, 1.0);
        //warpAffine(roi, roi, affine_matrix, roi.size());
        cv::Mat roi8 = convertToUint8(roi);

        cv::Mat ROIsMat = getROIsMeanMat(roi, Area90Mat, bin, rect);
        if (ROIsMat.data != NULL && Area90Mat.data != NULL) //& AreaFullMat.data !=NULL)
        {
            cv::Scalar mean, roisMean,std;
           // mean = cv::mean(Area90Mat);
            cv::meanStdDev(Area90Mat, mean, std);
            double minVal = 0, maxVal = 0, avg = 0, uniformity = 0;
            cv::minMaxIdx(ROIsMat, &minVal, &maxVal);
            roisMean = cv::mean(ROIsMat);
            uniformity = 100 * roisMean(0) / mean(0);
            double MinRoiPer = 0, MaxRoiPer = 0;
            MinRoiPer = 100 * minVal / mean(0);
            MaxRoiPer = 100 * maxVal / mean(0);
            re.MinMaxAvg.clear();
            re.MinMaxPer.clear();
            re.MinMaxAvg.push_back(minVal);
            re.MinMaxAvg.push_back(maxVal);
            re.MinMaxAvg.push_back(mean(0));
            re.MinMaxPer.push_back(MinRoiPer);
            re.MinMaxPer.push_back(MaxRoiPer);
            re.uniformity = uniformity;
            re.rectMean = mean(0);
            re.rectCov = std(0) / mean(0);
            m_luminance = re;
        }
    }
    return m_luminance;
}

cv::Point2f algorithm::MLChromaPara::getcoordinate(std::vector<cv::Mat> A, cv::Point2f center, double bin, cv::Rect rect)
{
    Point2f a = { 0, 0 };
    if (A.size() == 2)
    {
        double left = cv::max(0, int(center.x - 50 / bin));
        double right = cv::min(A[0].cols, int(center.x + 50 / bin));
        double top = cv::max(0, int(center.y - 50 / bin));
        double bottom = cv::min(A[0].rows, int(center.y + 50 / bin));
        cv::Rect rect = Rect(left, top, right - left, bottom - top);
        cv::Mat A0_draw, A1_draw;
        A0_draw = A[0].clone();
        A1_draw = A[1].clone();
        cv::rectangle(A0_draw, rect, Scalar(0, 0, 255), 2);
        cv::rectangle(A1_draw, rect, Scalar(255, 0, 0), 2);
        cv::Mat imgx = A[0](rect);
        cv::Mat imgy = A[1](rect);
        cv::Scalar meanx, meany;
        meanx = cv::mean(imgx);
        meany = cv::mean(imgy);
        a.x = meanx(0);
        a.y = meany(0);
    }
    return a;
}

std::vector<double> algorithm::MLChromaPara::LuminanceRolloff(cv::Mat img)
{
    std::vector<double> re;
    double H_Rolloff = 0;
    double V_Rolloff = 0;
    if (img.data != NULL)
    {
        double rotationAngle = HydrusParameters::roationAngle + m_rotationAngle;
       // img = getRotationImg(img, rotationAngle);
        double k1 = 0, k2 = 1e6, b1 = 0, b2 = 0;
        //if (m_Vline.size() == 3 && m_Hline.size() == 3)
        //{
        //    k2 = -m_Vline[0] / m_Vline[1];
        //    k1 = -m_Hline[0] / m_Hline[1];
        //}
        k1 = tan(rotationAngle * CV_PI / 180);
        k2 = tan((rotationAngle - 90) * CV_PI / 180);
        if (abs(k1) > abs(k2))
        {
            //double temp = k1;
           // k1 = k2;
           // k2 = k1;
            swap(k1, k2);    
        }

        cv::Rect rect = m_para.rect;//GetNewPara(img).rect;
        cv::Mat roi = GetROIMat(img, rect);
        cv::Mat img_draw = roi.clone();
        Mat img1 = convertToUint8(img);
        img_draw = convertToUint8(roi);
        double m_FocalLength = HydrusParameters::FocalLength;
        double m_pixel_size = m_para.pixel_size;
        double RolloffLength = HydrusParameters::RolloffLength;

        double dis = m_FocalLength * tan(RolloffLength * 2 * CV_PI / 180) / m_pixel_size;
        double degree = atan(k1) * 180.0 / CV_PI;
        int hydis = dis * cos(degree * CV_PI / 180.0);
        Point2f center;
        //m_center = Point2f{4500, 3710};
        if (m_center.x != 0 && m_center.y != 0)
        {
            center.x = m_center.x / m_para.binNum - rect.x;
            center.y = m_center.y / m_para.binNum - rect.y;
            b1 = center.y - k1 * center.x;
            b2 = center.y - k2 * center.x;
            cv::circle(img_draw, center, 2, Scalar(255, 0, 255), -1);
            double RolloffWidth = HydrusParameters::RolloffWidth;
            int htop = cv::max(int(center.y - RolloffWidth / 2.0), 0);
            int hbottom = cv::min(int(center.y + RolloffWidth / 2.0), roi.rows);
            int hleft = cv::max((int)center.x - hydis / 2, 0);
            int hright = cv::min((int)center.x + hydis / 2, roi.cols);
            vector<double> Hmeans;
            for (int i = hleft; i < hright; i++)
            {
                double hsum = 0;
                for (int j = htop; j < hbottom; j++)
                {
                    int x = (j - b2) / k2;
                    double b = j - k1 * x;
                    int y = k1 * i + b;
                    int y0 = 4000 * k1 + b;
                     //cv::line(img_draw, Point(0, b), Point(4000, y0), Scalar(255, 0, 255), 1, 8, 0);
                    cv::circle(img_draw, Point(i, y), 2, Scalar(255, 0, 255), -1);
                     //cout << Point(i, y) << endl;
                    double temp;
                    if (roi.depth() == CV_16U)
                    {
                        temp = float(roi.at<short>(y, i));
                    }
                    else if (roi.depth() == CV_32F)
                    {
                        temp = float(roi.at<float>(y, i));
                    }
                    else
                    {
                        temp = float(roi.at<uchar>(y, i));
                    }
                    hsum += temp;
                }
                double Hmean = hsum / 10;
                //cout << "Hmean" << Hmean<<endl;
                Hmeans.push_back(Hmean);
            }

            int vleft = cv::max(int(center.x - RolloffWidth / 2.0), 0);
            int vright = cv::min(int(center.x + RolloffWidth / 2.0), roi.cols);
            int vtop = cv::max((int)center.y - hydis / 2, 0);
            int vbottom = cv::min((int)center.y + hydis / 2, roi.rows);
            vector<double> Vmeans;
            for (int i = vtop; i < vbottom; i++)
            {
                double vsum = 0;
                for (int j = vleft; j < vright; j++)
                {
                    int y = k1 * j + b1;
                    double b = y - k2 * j;
                    int x = (i - b) / k2;
                    int x0 = (4000 - b) / k2;
                    // cv::line(img_draw, Point(-b / k2, 0), Point(x0, 4000), Scalar(255, 0, 255), 1, 8, 0);
                    cv::circle(img_draw, Point(x, i), 2, Scalar(255, 0, 255), -1);
                    // cout << Point(x, i) << endl;
                    double temp;
                    if (roi.depth() == CV_16U)
                    {
                        temp = float(roi.at<short>(i, x));
                    }
                    else if (roi.depth() == CV_32F)
                    {
                        temp = float(roi.at<float>(i, x));
                    }
                    else
                    {
                        temp = float(roi.at<uchar>(i, x));
                    }
                    vsum += temp;
                }
                double Vmean = vsum / 10;
                //cout << "Vmean" << Vmean<<endl;
                Vmeans.push_back(Vmean);
            }
            if (Hmeans.size() != 0 && Vmeans.size() != 0)
            {
                double top = Hmeans[0];
                double bottom = Hmeans[Hmeans.size() - 1];
                double h_center = Hmeans[Hmeans.size() / 2.0 - 1];

                double left = Vmeans[0];
                double right = Vmeans[Vmeans.size() - 1];
                double v_center = Vmeans[Vmeans.size() / 2.0 - 1];
                double cen = (v_center + h_center) / 2.0;
                H_Rolloff = cv::min(100 * top / cen, 100 * bottom / cen);
                V_Rolloff = cv::min(100 * left / cen, 100 * right / cen);
                m_img_draw = img_draw.clone();
            }
        }
    }
    re.push_back(H_Rolloff);
    re.push_back(V_Rolloff);
    return re;
}

std::vector<double> algorithm::MLChromaPara::LuminanceRolloffRotation(cv::Mat img)
{
    std::vector<double> re;
    double H_Rolloff = 0;
    double V_Rolloff = 0;
    if (img.data != NULL)
    {
        m_img_draw.release();
        double rotationAngle = HydrusParameters::roationAngle + m_rotationAngle;
        img = getRotationImg(img, rotationAngle);
        cv::Rect rect = m_para.rect; // GetNewPara(img).rect;
        cv::Mat roi = GetROIMat(img, rect);
        cv::Mat img_draw = roi.clone();
        Mat img1 = convertToUint8(img);
        img_draw = convertToUint8(roi);
        double m_FocalLength = HydrusParameters::FocalLength;
        double m_pixel_size = m_para.pixel_size;
        double RolloffLength = HydrusParameters::RolloffLength;
        double dis = m_FocalLength * tan(RolloffLength * CV_PI / 180) / m_pixel_size*2;
        Point2f center;
        // m_center = Point2f{4500, 3710};
        cv::Rect rectBound;
        cv::RotatedRect rectR = getSolidBorder(img_draw, rectBound);
        center = rectR.center;
        if (center.x != 0 && center.y != 0)
        {
            cv::circle(img_draw, center, 2, Scalar(255, 0, 255), -1);
            double RolloffWidth = HydrusParameters::RolloffWidth;        
            cv::Rect rectH,rectV;
            rectV.x = (round(center.x) - RolloffWidth / 2);
            rectV.y = (round(center.y) - round(dis / 2.0));
            rectV.width = RolloffWidth;
            rectV.height = dis;
            rectH.x = round(center.x) - round(dis / 2.0);
            rectH.y = (round(center.y) - RolloffWidth / 2.0);
            rectH.width = dis;
            rectH.height = RolloffWidth;
            cv::rectangle(img_draw, rectH, Scalar(255, 0, 255), -1);
            cv::rectangle(img_draw, rectV, Scalar(255, 0, 255), -1);
            cv::Mat matH = roi(rectH).clone();
            cv::Mat matV = roi(rectV).clone();
            cv::Mat hM, vM;
            cv::reduce(matH, hM, 0, REDUCE_AVG);
            cv::reduce(matV, vM, 1, REDUCE_AVG);                   
            double minVal = min(hM.at<float>(0, 0), hM.at<float>(0,hM.total()-1));
            double cenVal = hM.at<float>(0,(hM.total() / 2));
            H_Rolloff = minVal / cenVal*100;
            minVal = min(vM.at<float>(0, 0), vM.at<float>(vM.total() - 1, 0));           
            cenVal = vM.at<float>((vM.total() / 2),0);
            V_Rolloff = minVal / cenVal * 100;
            m_img_draw = img_draw.clone();
        }
    }
    re.push_back(H_Rolloff);
    re.push_back(V_Rolloff);
    return re;
}

std::vector<cv::Rect> algorithm::MLChromaPara::FindroisBorder(Mat img, cv::Rect &Area90Rect, double bin)
{
    vector<cv::Rect> roisRect;
    if (img.data != NULL)
    {

        cv::Rect rectAll(0,0,img.cols,img.rows);
        cv::Mat img_gray, img_gray1, img_draw;
        if (img.depth() == CV_8U)
        {
            img_gray1 = img.clone();
            img_draw = img.clone();
        }
        else
        {
            MLimagePublic mlp;
            img_gray1 = mlp.convertToUint8(img);
            img_draw = mlp.convertToUint8(img);
        }
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::Mat img_process, srcbinary;

        threshold(img_gray1, srcbinary, 0, 255, THRESH_OTSU);

        cv::findContours(srcbinary, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
        vector<double> area_sort;
        vector<int> list;
        cv::Mat roi;
        RotatedRect rec;
        cv::Rect rect;
        for (int i = 0; i < contours.size(); ++i)
        {
            double area = contourArea(contours[i], false);
            // cout << area << endl;
            double powbin = 2; // pow(m_para.multiple, 2);           
            if (area > 1e4&area<50e5/(m_para.binNum*m_para.binNum))
            {
                rect = cv::boundingRect(cv::Mat(contours[i]));
                cv::rectangle(img_draw, rect, Scalar(255, 0, 255), 1);
            }
        }
        double m_FocalLength = HydrusParameters::FocalLength;
        double m_pixel_size = HydrusParameters::pixel_size;
        double pixel_size = m_pixel_size * bin;
        double p = m_FocalLength * tan(CV_PI / 180) / pixel_size;
        
        if (rect.width > 0 & rect.height > 0)
        {

            vector<cv::Point2f> pt;
            double w = rect.width;
            double h = rect.height;
            cv::Point2f start = cv::Point2f(rect.x, rect.y);
            cv::Point2f center = start + cv::Point2f(w / 2.0, h / 2.0);
            cv::Rect rect90;
            // rect90.x = start.x + w * 0.1;
            // rect90.y = start.y + h * 0.1;
            // rect90.width = w * 0.8;
            // rect90.height = h * 0.8;
            //TODO:72%
         //   w = w * 0.8;
          //  h = h * 0.8;
            float ratio = HydrusParameters::LuminaceActive;
            cv::Point2f ul = center - Point2f(ratio * w / 2, ratio * h / 2);
            rect90 = Rect(ul.x, ul.y, ratio * w, ratio * h);
            cv::rectangle(img_draw, rect90, Scalar(0, 0, 255), 1);
            cv::Point2f startNew = cv::Point2f(rect90.x, rect90.y);
            Area90Rect = (rect90)&rectAll;
            for (int j = 0; j < 3; j++)
            {
                for (int i = 0; i < 3; i++)
                {
                    double dx = i / 2.0 * w * ratio;
                    double dy = j / 2.0 * h * ratio;
                    cv::Point tmp = startNew + cv::Point2f(dx, dy);
                    pt.push_back(tmp);
                    circle(img_draw, tmp, 5, cv::Scalar(0, 0, 255), -1);
                    cv::Rect rectTmp = cv::Rect(tmp.x - p / 2.0, tmp.y - p / 2.0, p, p)&rectAll;
                    cv::rectangle(img_draw, rectTmp, Scalar(0, 0, 255), 1);
                    cv::Mat matTmp = img(rectTmp);
                    roisRect.push_back(rectTmp);
                }
            }
        }
    }

    return roisRect;
}
cv::Rect algorithm::MLChromaPara::findborder(cv::Mat img, cv::Rect rect)
{
    cv::Rect rect_new = Rect(0, 0, -1, -1);
    // cv::Mat img_draw = img.clone();
    if (img.data != NULL)
    {
        cv::Mat img_gray, img_gray1, img_draw;
        img_gray = GetROIMat(img, rect);
        if (img_gray.depth() == CV_8U)
        {
            img_gray1 = img_gray.clone();
            img_draw = img.clone();
        }
        else
        {
            img_gray1 = convertToUint8(img_gray);
            img_draw = convertToUint8(img);
        }
        std::vector<std::vector<cv::Point>> contours, contoursNew;
        std::vector<cv::Vec4i> hierarchy;
        cv::Mat img_process, srcbinary;
        cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        // cv::GaussianBlur(img_gray1, img_gray1, cv::Size(9, 9), 2, 2);
        // cv::morphologyEx(img_gray1, img_gray1, cv::MORPH_CLOSE, element1);
        cv::morphologyEx(img_gray1, img_process, cv::MORPH_GRADIENT, element1);
        threshold(img_process, srcbinary, 0, 255, THRESH_TRIANGLE);
        NaiveRemoveNoise(srcbinary, 500);
        // cv::Mat img_canny;
        // cv::Canny(srcbinary, img_canny, 50, 150);
        cv::findContours(srcbinary, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
        vector<double> area_sort;
        vector<int> list;
        cv::Mat roi;
        vector<double> areaVec;
        for (int i = 0; i < contours.size(); ++i)
        {
            double area = contourArea(contours[i], false);
            // cout << area << endl;
            double powbin = pow(m_para.binNum, 2);
            if (area > 4e6 / powbin) //&& area < 4.2e6 / powbin)
            // if (area > 4e6 / powbin && area < 6e6 / powbin)
            {
                rect_new = cv::boundingRect(cv::Mat(contours[i]));
                cv::rectangle(img_draw, rect_new, Scalar(255, 0, 255), 1);
                contoursNew.push_back(contours[i]);
                areaVec.push_back(area);
                // roi = img_gray1(rect_new);
            }
        }
        if (contoursNew.size() > 0)
        {
            int maxPos = max_element(areaVec.begin(), areaVec.end()) - areaVec.begin();
            rect_new = cv::boundingRect(cv::Mat(contoursNew[maxPos]));
            roi = img_draw(rect_new);
        }
    }
    return rect_new;
}

 cv::RotatedRect algorithm::MLChromaPara::getSolidBorder(cv::Mat img, cv::Rect &rect)
{
     cv::RotatedRect rectR;
    if (img.data != NULL)
    {
        cv::Mat img_gray, img_draw;
       
        if (img.depth() != CV_8U)
        {
            img = convertToUint8(img);
        }
        img_draw = img.clone();
        std::vector<std::vector<cv::Point>> contours, contoursNew;
        std::vector<cv::Vec4i> hierarchy;
        cv::Mat img_process, srcbinary;
        cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
       cv::morphologyEx(img, img_process, cv::MORPH_GRADIENT, element1);
        threshold(img, srcbinary, 0, 255, THRESH_OTSU);
        cv::findContours(srcbinary, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
        vector<double> area_sort;
        vector<int> list;
        cv::Mat roi;
        vector<double> areaVec;
        for (int i = 0; i < contours.size(); ++i)
        {
            double area = contourArea(contours[i], false);
            double powbin = pow(m_para.binNum, 2);
            rect = cv::boundingRect(contours[i]);
            double maxLen = max(rect.width, rect.height);
            double minLen = min(rect.width, rect.height);
            double ratio = minLen / maxLen;
            if (area > 2e5 && ratio>0.8 && ratio<1.2)
            {
                rectR = cv::minAreaRect(contours[i]);
               // center = rectR.center;
                cv::rectangle(img_draw, rect, Scalar(255, 0, 255), 1);
                break;
            }
        }
    }
    return rectR;
}

cv::Mat algorithm::MLChromaPara::getSolidImgRotated(cv::Mat img)
{
    cv::Mat imgR;
    if (img.data!=NULL)
    {
        cv::Rect rect;
        cv::RotatedRect rectR = getSolidBorder(img,rect);
        imgR = getRotationImg(img, rectR.angle); 
    }
    return imgR;
}
