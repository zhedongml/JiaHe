#include "pch.h"
#include "ML_Efficiency.h"
#include "CrossCenter.h"
#include"MLSolidDetection.h"
#include"LogPlus.h"
using namespace MLImageDetection;
using namespace MLIQMetrics;
using namespace cv;
//IQMetricUtl *IQMetricUtl::self = nullptr;

MLEfficiency *MLEfficiency::effSelf = nullptr;

MLIQMetrics::MLEfficiency::MLEfficiency()
{
}

MLIQMetrics::MLEfficiency::~MLEfficiency()
{
}
MLEfficiency *MLIQMetrics::MLEfficiency::instance()
{
    if (effSelf == nullptr)
    {
        effSelf = new MLEfficiency();
    }
    return effSelf;
}
void MLIQMetrics::MLEfficiency::setFOVType(FOVTYPE type)
{
    m_fovType = type;
}
LumiEfficencyRe MLIQMetrics::MLEfficiency::GetLuminanceEfficiency(cv::Mat img, string color, float angle, int eyeLoc)
{
    LumiEfficencyRe re;
    double efficiency = -1;
    string str = "ALG Error-----[getLuminanceEfficiency]-----";
    string eyeType = eyeLoc == 0 ? "left" : "right";
    string slbpath = "./config/slbImgs/" + eyeType + "/ND3_" + color + "_solid_5_Y.tif";
    cv::Mat slb = cv::imread(slbpath, -1);
    if (img.data != NULL && slb.data != NULL)
    {
       // m_img_draw.release();
        if (slb.size() != img.size())
            cv::resize(slb, slb, img.size());
        cv::Rect rectDUT, rectSLB;
        img = getSolidImgRotated(img,rectDUT);
        slb = getSolidImgRotated(slb, rectSLB);
        double ratio = IQMetricsParameters::LuminaceActive;
        int binNum = IQMetricUtl::instance()->getBinNum(img.size());
        rectDUT = updateRectByRatio(rectDUT, ratio);
        rectSLB = updateRectByRatio(rectSLB, ratio);
        cv::Mat efficiencyMat = img / slb;
        re.efficiencyMat = efficiencyMat;
        //cv::Rect rectDUT = getLumiEfficiencyROI(img, angle);
       // cv::Rect rectSLB = getLumiEfficiencyROI(slb, angle);

        if (rectDUT.x != 0 && rectDUT.y != 0)
        {
            cv::Mat dutROI = img(rectDUT).clone();
            cv::Mat slbROI = slb(rectSLB).clone();
            cv::Mat img_draw = convertToUint8(img);
            cv::Mat slb_draw = convertToUint8(slb);
            img_draw = convertTo3Channels(img_draw);
            slb_draw = convertTo3Channels(slb_draw);

            cv::rectangle(img_draw, rectDUT, Scalar(255, 0, 255), 16/binNum);
            cv::rectangle(slb_draw, rectSLB, Scalar(255, 0, 255), 16 / binNum);
            //m_img_draw = img_draw.clone();
            re.slb_draw = slb_draw.clone();
            cv::Scalar dutMean = cv::mean(dutROI);
            cv::Scalar slbMean = cv::mean(slbROI);
            re.efficicncy = dutMean(0) / slbMean(0) * 100;
            re.imgdraw = img_draw;
           // m_img_draw = img_draw.clone();
        }
    }
    else
    {
        re.flag = false;
        string sMsg = "The ROI is out of the boundary!";
        string errorMessage = str + sMsg;
        // LOG4CPLUS_INFO(LogPlus::getInstance()->logger, errorMessage.c_str());
        re.errMsg = sMsg;
    }

    return re;
}

LumiEfficencyRe MLIQMetrics::MLEfficiency::GetLuminanceEfficiency(cv::Mat img, string color)
{
    LumiEfficencyRe re;
    double efficiency = -1;
    string str = "ALG Error-----[getLuminanceEfficiency]-----";
    string slbpath = "./config/slbImgs/ND3_" + color + "_solid_5_Y.tif";
    cv::Mat slb = cv::imread(slbpath, -1);
    if (img.data != NULL && slb.data != NULL)
    {
        // m_img_draw.release();
        if (slb.size() != img.size())
            cv::resize(slb, slb, img.size());
        cv::Rect rectDUT, rectSLB;
        img = getSolidImgRotated(img, rectDUT);
        slb = getSolidImgRotated(slb, rectSLB);
        double ratio = IQMetricsParameters::LuminaceActive;
        int binNum = IQMetricUtl::instance()->getBinNum(img.size());
        rectDUT = updateRectByRatio(rectDUT, ratio);
        rectSLB = updateRectByRatio(rectSLB, ratio);
       // cv::Mat efficiencyMat = img / slb;
       // re.efficiencyMat = efficiencyMat;
        //cv::Rect rectDUT = getLumiEfficiencyROI(img, angle);
       // cv::Rect rectSLB = getLumiEfficiencyROI(slb, angle);

        if (rectDUT.x != 0 && rectDUT.y != 0)
        {
            cv::Mat dutROI = img(rectDUT).clone();
            cv::Mat slbROI = slb(rectSLB).clone();
            cv::Mat img_draw = convertToUint8(img);
            cv::Mat slb_draw = convertToUint8(slb);
            img_draw = convertTo3Channels(img_draw);
            slb_draw = convertTo3Channels(slb_draw);

            cv::rectangle(img_draw, rectDUT, Scalar(255, 0, 255), 16 / binNum);
            cv::rectangle(slb_draw, rectSLB, Scalar(255, 0, 255), 16 / binNum);
            
            if (dutROI.size() != slbROI.size())
                cv::resize(slbROI, slbROI, dutROI.size());
            dutROI.convertTo(dutROI,CV_32F);
            slbROI.convertTo(slbROI, CV_32F);

            cv::Mat effmat = dutROI / slbROI * 100;
            re.efficiencyMat = effmat;
            double p5 = percentile(effmat, 5);
            double p50 = percentile(effmat, 50);
            double p95 = percentile(effmat, 95);
            re.p5 = p5;
            re.p50 = p50;
            re.p95 = p95;
            //m_img_draw = img_draw.clone();
            cv::Scalar dutMean = cv::mean(dutROI);
            cv::Scalar slbMean = cv::mean(slbROI);
           // re.efficicncy = dutMean(0) / slbMean(0) * 100;
            re.efficicncy = mean(effmat)(0);
            putTextOnImage(slb_draw, to_string(slbMean(0)), rectSLB.tl(), 20 / binNum);
            putTextOnImage(img_draw, to_string(dutMean(0)), rectDUT.tl(), 20 / binNum);
            putTextOnImage(img_draw, to_string(re.efficicncy), rectDUT.tl()+cv::Point(0,400/binNum), 20 / binNum);
            re.imgdraw = img_draw;
            re.slb_draw = slb_draw.clone();

        }
    }
    else
    {
        re.flag = false;
        string sMsg = "The ROI is out of the boundary!";
        string errorMessage = str + sMsg;
        // LOG4CPLUS_INFO(LogPlus::getInstance()->logger, errorMessage.c_str());
        re.errMsg = sMsg;
    }

    return re;
}

LumiEfficencyRe MLIQMetrics::MLEfficiency::getLuminanceEfficiency(cv::Mat img, string color)
{
    LumiEfficencyRe effRe;
    if (img.empty())
    {
        effRe.flag = false;
        effRe.errMsg = "Input image is NULL";
        return effRe;
    }
    int binNum = IQMetricUtl::instance()->getBinNum(img.size());
    cv::Rect rectROI = IQMetricsParameters::ROIRect;
    updateRectByRatio1(rectROI, 1.0 / binNum);
    cv::Mat imgROI = GetROIMat(img, rectROI);
    Mat img8 = convertToUint8(imgROI);
    cv::Mat img_draw = convertTo3Channels(img8);
    MLSolidDetection solid;
    solid.setBinNum(binNum);
    solid.setFOVType(m_fovType);
    SolidDetectionRe solidRe = solid.getSolidLocation(img8);
    updateRotateImg(img, solidRe.rotationAngle);
    updateRotateImg(img_draw, solidRe.rotationAngle);
    updateRotateImg(img8, solidRe.rotationAngle);
    cv::Rect rectR = solid.getSolidExactRect(img8, solidRe.rectAf);
    double ratio = IQMetricsParameters::RolloffAreaRatio;
    cv::Rect rectRatio = updateRectByRatio(rectR, sqrt(ratio));
    rectangle(img_draw, rectRatio, Scalar(0, 255, 0), 4);
    cv::Mat lumiROI = imgROI(rectRatio).clone();
    double lumiSLB = readSLBLuminance(color);
    double eff = cv::mean(lumiROI)(0) / lumiSLB * 100;
    effRe.efficicncy = eff;
    effRe.imgdraw = img_draw;
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "efficiency calculation successfully");
    return effRe;
}

 cv::Mat MLIQMetrics::MLEfficiency::getSolidImgRotated(cv::Mat img, cv::Rect &rectAfR)
{
     cv::Mat imgR;
     if (img.data != NULL)
     {
         cv::Rect rect;
         cv::Mat img8 = convertToUint8(img);
         cv::RotatedRect rectR = getSolidBorder(img8, rect);
         cv::Point2f center((float)(img.cols / 2), (float)(img.rows / 2));

         updateRotateImg(img, rectR.angle);
         rectAfR = updateRotateRect(rectR, center);
     }
     return img;
 }

 cv::RotatedRect MLIQMetrics::MLEfficiency::getSolidBorder(cv::Mat img, cv::Rect &rect)
{
    cv::RotatedRect rectR;
    if (img.data != NULL)
    {
       cv::Mat gray = convertToGrayImage(img);
       cv::Mat imgdraw = convertTo3Channels(img);
        cv::Mat preImg = preProcess(gray);
        cv::Mat srcbinary;     
        threshold(preImg, srcbinary, 0, 255, THRESH_OTSU);
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(srcbinary, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
        vector<double> area_sort;
        vector<int> list;
        cv::Mat roi;
        vector<double> areaVec;
        for (int i = 0; i < contours.size(); ++i)
        {
            double area = contourArea(contours[i], false);
            int binNum = IQMetricUtl::instance()->getBinNum(img.size());
            double powbin = pow(binNum, 2);
            rect = cv::boundingRect(contours[i]);
            double maxLen = max(rect.width, rect.height);
            double minLen = min(rect.width, rect.height);
            double ratio = minLen / maxLen;
            if (rect.area() > 2e5 /*&& ratio > 0.8 && ratio < 1.2*/)
            {
                rectR = cv::minAreaRect(contours[i]);
                // center = rectR.center;
                cv::rectangle(imgdraw, rect, Scalar(255, 0, 255), 10);
                break;
            }
        }
    }


    return rectR;
}

 cv::Rect MLIQMetrics::MLEfficiency::getLumiEfficiencyROI(cv::Mat img, float angle)
{
    cv::Rect rect = Rect(0, 0, -1, -1);
    if (img.data != NULL)
    {
        //Newpara para = hydrusConfig.GetNewPara(img);
        cv::Rect ROIRect = IQMetricsParameters::ROIRect;
        int binNum = IQMetricUtl::instance()->getBinNum(img.size());
        cv::Point2f opticalCenter = IQMetricsParameters::opticalCenter;
        cv::Mat roi = GetROIMat(img, ROIRect).clone();
        cv::Mat roi1 = convertToUint8(roi);
        cv::Mat roi_draw = convertToUint8(roi);
        double focallength = IQMetricsParameters::FocalLength;
        double pixel_size = IQMetricsParameters::pixel_size;
        int dis = focallength * tan(angle / 2.0 * CV_PI / 180) / pixel_size * 2.0;
        cv::Point2f center;
        cv::Rect rec = Rect(0, 0, -1, -1);
        // rec = findborder(roi1);
        double ratio = IQMetricsParameters::LuminaceActive;
        getSolidBorder(roi1, rec);
        dis = (rec.width + rec.height) * ratio / 2.0;
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
            rect.x = rectangle.x + ROIRect.x;
            rect.y = rectangle.y + ROIRect.y;
        }
        else
        {
            cv::Point cen = opticalCenter / binNum;
            int top = cen.y - dis / 2;
            int bottom = cen.y + dis / 2;
            int left = cen.x - dis / 2;
            int right = cen.x + dis / 2;
            rect = Rect(left, top, right - left, bottom - top);
        }
    }
    return rect;
}

cv::Mat MLIQMetrics::MLEfficiency::preProcess(cv::Mat gray)
{
    cv::medianBlur(gray, gray, 3);
    //  cv::GaussianBlur(gray, gray, Size(3, 3), 1, 0);
    Ptr<CLAHE> clahe = createCLAHE(2.0, Size(10, 10));
    clahe->apply(gray, gray);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(10, 10), Point(-1, -1));
     //morphologyEx(gray, gray, MORPH_GRADIENT, kernel, Point(-1, -1));
    return gray;
}

double MLIQMetrics::MLEfficiency::readSLBLuminance(string color)
{
    string fovstr = IQMetricUtl::instance()->fovTypeToString(m_fovType);
    string filepath = "./config/ALGConfig/" + fovstr+ "_slbLumi_" + color + ".csv";
    cv::Mat lumimat=readCSVToMat(filepath);
    return lumimat.at<float>(0,0);
}

// algorithm::ALGResult MLIQMetrics::MLEfficiency::getSLBFilePath(string &filePath, string color)
//{
//    QString path = "./config/slbLuminanceImgs/";
//    QDir dir(path);
//    QStringList filename;
//    filename << "*.tif"
//             << "*.tiff";
//    QStringList results = dir.entryList(filename, QDir::Files | QDir::Readable, QDir::Name);
//    for (QString name : results)
//    {
//        if (name.toUpper().startsWith(QString::fromStdString(color).toUpper()))
//        {
//            filePath = (path + name).toStdString();
//            return ALGResult();
//        }
//    }
//    return ALGResult(
//        false, QString(path + " dir is not exist SLB %1 image.").arg(QString::fromStdString(color)).toStdString());
//
//    return algorithm::ALGResult();
//}