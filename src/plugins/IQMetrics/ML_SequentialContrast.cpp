#include "pch.h"
#include "ML_SequentialContrast.h"
#include "ML_SequentialContrast.h"
#include "ML_Efficiency.h"
#include"ml_rectangleDetection.h"
#include "pch.h"
#include<armadillo>
#include"LogPlus.h"
using namespace cv;
using namespace MLIQMetrics;
using namespace MLImageDetection;
MLIQMetrics::MLSequentialContrast::MLSequentialContrast()
{
}

MLIQMetrics::MLSequentialContrast::~MLSequentialContrast()
{
}
SeqCRRe MLIQMetrics::MLSequentialContrast::getSequentialContrast(Mat img_WRGB, Mat img_black)
{
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "SequentialContrast calculation start");
    string info = "------SequentialContrast------";
    SeqCRRe re;
    if (img_WRGB.empty() || img_black.empty())
    {
        re.flag = false;
        re.errMsg = info + "The input image is NULL!";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, re.errMsg.c_str());
        return re;
    }

    int binNum = IQMetricUtl::instance()->getBinNum(img_WRGB.size());
    if (binNum <= 0)
    {
        re.flag = false;
        re.errMsg = info + "the image size is not right, please check the input image";
        return re;
    }
    cv::Mat imgc8 = convertToUint8(img_WRGB);
    cv::Mat imgb8 = convertToUint8(img_black);
    cv::Mat imgdrawColor = convertTo3Channels(imgc8);
    cv::Mat imgdrawBlack = convertTo3Channels(imgb8);
    cv::Rect rect;
    cv::RotatedRect rectR = MLEfficiency::instance()->getSolidBorder(imgc8, rect);
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "SolidBorder detection successfully");

    cv::Point2f center((float)(img_WRGB.cols / 2.0), (float)(img_WRGB.rows / 2.0));
    cv::Rect rectRAfterRotation = rotateRect(rectR, center, rectR.angle);
    updateRotateImg(imgdrawBlack, rectR.angle);
    updateRotateImg(imgdrawColor, rectR.angle);
    updateRotateImg(img_WRGB, rectR.angle);
    updateRotateImg(img_black, rectR.angle);
    rectRAfterRotation = updateRotateRect(rectR, center);

    float ratio = IQMetricsParameters::LuminaceActive;
    cv::Rect ROIrect = updateRectByRatio(rectRAfterRotation, ratio);
    cv::Mat roiMatBlack = img_black(ROIrect).clone();
    cv::Mat roiMatColor = img_WRGB(ROIrect).clone();
    cv::Mat roiMatSeq;
    cv::divide(roiMatColor, roiMatBlack, roiMatSeq, 1);
    roiMatSeq.setTo(3999.99, roiMatSeq >= 4000);
    double seqCR;
    Scalar blackM = cv::mean(roiMatBlack);
    Scalar colorM = cv::mean(roiMatColor);
    if (blackM(0) <= 0)
        seqCR = 3999.9;
    else
        seqCR = colorM(0) / blackM(0);
    re.Roicr = roiMatSeq;
    re.seqCR = seqCR;
    drawRectOnImage(imgdrawColor, ROIrect);
    drawRectOnImage(imgdrawBlack, ROIrect);
    string strC = "Luinance:" + numToString(colorM(0));
    string strB = "BlackLuinance:" + numToString(blackM(0));
    putTextOnImage(imgdrawColor, strC, center,24/binNum);
    putTextOnImage(imgdrawColor, strB, center+cv::Point2f(0,300/binNum), 24 / binNum);
    putTextOnImage(imgdrawColor, "seqCR:"+numToString(seqCR), center + cv::Point2f(0, 900 / binNum), 24 / binNum);

    re.imgdrawColor = imgdrawColor;
    re.imgdrawBlack = imgdrawBlack;
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "SequentialContrast calculation successfully");

    return re;
}

SeqCRGuSuRe MLIQMetrics::MLSequentialContrast::getSequentialContrastGuSu(cv::Mat img_WRGB, cv::Mat img_black)
{
    string info = "------SequentialContrast------";
    SeqCRGuSuRe re;
    if (img_WRGB.empty() || img_black.empty())
    {
        re.flag = false;
        re.errMsg = info + "The input image is NULL!";
        return re;
    }

    int binNum = IQMetricUtl::instance()->getBinNum(img_WRGB.size());
    cv::Mat imgc8 = convertToUint8(img_WRGB);
   // cv::Mat imgb8 = convertToUint8(img_black);
    cv::Mat imgdrawColor = convertTo3Channels(imgc8);
   // cv::Mat imgdrawBlack = convertTo3Channels(imgb8);

    int resizeNum = 4 / binNum;
    cv::Mat imgRe;
    cv::resize(imgc8, imgRe, imgc8.size() / resizeNum);
    RectangleDetection rd;
    cv::RotatedRect rectR = rd.getRectangleBorder(imgRe);
    cv::Rect rectRAfterRotation;
    cv::Point2f center((float)(imgRe.cols / 2), (float)(imgRe.rows / 2));
    rectRAfterRotation = updateRotateRect(rectR, center);
    rectRAfterRotation = rd.getSolidExactRect(imgRe, rectRAfterRotation);
    updateRectByRatio1(rectRAfterRotation, resizeNum);
       
    updateRotateImg(imgdrawColor, rectR.angle);
    updateRotateImg(img_WRGB, rectR.angle);
    updateRotateImg(img_black, rectR.angle);

    double zoneAR = IQMetricsParameters::zoneARadius;
    double pixel = IQMetricsParameters::pixel_size * binNum;
    double focallength = IQMetricsParameters::FocalLength;
    int radius = tan(zoneAR / 180 * CV_PI) * focallength / pixel;
    cv::Mat maskA = getCircleMask(rectRAfterRotation.size(), radius);
    cv::Mat maskB;
    cv::bitwise_not(maskA, maskB);
    cv::Mat roiMatBlack = img_black(rectRAfterRotation);
    cv::Mat roiMatColor = img_WRGB(rectRAfterRotation);
    cv::Mat seqMat;
    roiMatColor.convertTo(roiMatColor, CV_32FC1);
    roiMatBlack.convertTo(roiMatBlack, CV_32FC1);
    cv::divide(roiMatColor, roiMatBlack+1e-6, seqMat, 1);
    //roiMatSeq.setTo(3999.99, roiMatSeq >= 4000);

    double meanA=cv::mean(seqMat,maskA)(0);
    double meanB = cv::mean(seqMat, maskB)(0);
    double p5_A = getPercentile(seqMat,maskA,5);
    double p5_B = getPercentile(seqMat, maskB, 5);
    cv::Point2f cen = getRectCenter(rectRAfterRotation);
    re.meanZoneA = meanA;
    re.meanZoneB = meanB;
    re.p5ZoneA = p5_A;
    re.p5ZoneB = p5_B;
   // re.seqMat = seqMat;
       
    drawRectOnImage(imgdrawColor, rectRAfterRotation, 16 / binNum);
    circle(imgdrawColor, cen, radius, Scalar(0, 0, 255), 16 / binNum);
    string strC = "mean_zoneA:" + numToString(meanA);
    string strB = "p5_zoneA:" + numToString(p5_A);
    putTextOnImage(imgdrawColor, strC, cen, 24 /binNum);
    putTextOnImage(imgdrawColor, strB, cen + cv::Point2f(0, 300 / binNum), 24 / binNum);
   
    string strC1 = "mean_zoneB:" + numToString(meanB);
    string strB1 = "p5_zoneB:" + numToString(p5_B);
    putTextOnImage(imgdrawColor, strC1, rectRAfterRotation.tl() + cv::Point(0, 300 / binNum), 24 / binNum);
    putTextOnImage(imgdrawColor, strB1, rectRAfterRotation.tl() + cv::Point(0, 600 / binNum), 24 / binNum);
    re.imgdraw = imgdrawColor;
    return re;
}

double MLIQMetrics::MLSequentialContrast::getPercentile(cv::Mat seqMat, cv::Mat mask, double p)
{
    cv::Mat seqmatMask = seqMat + 1e-6;
    seqmatMask.setTo(0, mask == 0);
    seqmatMask.convertTo(seqmatMask, CV_64FC1);
    arma::mat data(reinterpret_cast<double*>(seqmatMask.data), seqmatMask.cols, seqmatMask.rows);
    arma::vec datavec = arma::vectorise(data);
    arma::uvec index = arma::find(data > 0);
    arma::vec dataMask = data(index);
 //   cout << dataMask << endl;

    double p0 = percentile(dataMask, p);
    return p0;
}
