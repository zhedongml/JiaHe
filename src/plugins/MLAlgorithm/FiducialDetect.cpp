#include "FiducialDetect.h"
#include <opencv2/imgproc/types_c.h>
using namespace algorithm;

algorithm::FiducialDetect::FiducialDetect()
{
}
algorithm::FiducialDetect::~FiducialDetect()
{
}
std::vector<cv::Point2f> algorithm::FiducialDetect::FiducialCoordinate(cv::Mat img, int flag)
{
    std::vector<cv::Point2f> center;
    if (flag == 1)
    {
        MLCircleFiducialDetect circledetect;
        center = circledetect.CircleFiducialDetect(img);
        m_img_draw = circledetect.getImgDraw();
    }
    else if (flag == 2)
    {
        MLLinesFiducialDetect linesdetect;
        center = linesdetect.LineFiducialDetect(img);
        m_img_draw = linesdetect.getImgDraw();
    }
    else if (flag == 3)
    {
        MLBMWFiducialDetect BMWdetect;
        center = BMWdetect.BMWFiducialDetect(img);
        m_img_draw = BMWdetect.getImgDraw();
    }
    else if (flag == 4)
    {
        MLRectangleFiducialDetect rectangledetect;
        center = rectangledetect.RectangleFiducialDetect(img);
        m_img_draw = rectangledetect.getImgDraw();
    }
    else if (flag==5)//for the PICO fiducial
    {

        ALGResult re= FiducialContour(img, center);
        if (!re.success)
        {
            cv::Mat templ;
            templ = cv::imread("./config/templateImg/picoTemplate.tif", 0);
            if (!templ.data)
                templ = cv::imread("../../app/config/templateImg/picoTemplate.tif", 0);
            MatchTemplate mt;
            re = mt.getMatchTemplateResult(img, templ, 80, center);
            m_img_draw = mt.getImgDraw();
        }  

    }
    return center;
}
ALGResult algorithm::FiducialDetect::FiducialCoordinate(cv::Mat img, FiducialType ftype,
                                                        std::vector<cv::Point2f> &center)
{
  //  std::vector<cv::Point2f> center;
    ALGResult re;
    switch (ftype)
    {
        case CIRCLE: {

            cv::Mat templ;
            if (DEBUG == 1)
            {
                templ = cv::imread("E:\\project\\hydrus2\\mtfiq\\src\\app\\config\\templateImg\\circletemplate1.tiff");
            }
            else
            {
                templ = cv::imread("./config/templateImg/circletemplate1.tiff", 0);
            }
            MatchTemplate mt;
            re = mt.getMatchTemplateResult(img, templ, 90, center);
            for (int i = 0; i < center.size(); i++)
            {
                center[i] = center[i] + cv::Point2f(260, 135);
            }
            break;
        }
        case CROSSHAIR: {

            cv::Mat templ;
            if (DEBUG == 1)
            {
                templ =
                    cv::imread("E:\\project\\hydrus2\\mtfiq\\src\\app\\config\\templateImg\\crosshairTemplate.tiff");
            }
            else
            {
                templ = cv::imread("./config/templateImg/crosshairTemplate.tiff", 0);
            }
            MatchTemplate mt;
            re = mt.getMatchTemplateResult(img, templ, 90, center);
            for (int i = 0; i < center.size(); i++)
            {
                center[i] = center[i] + cv::Point2f(62, 61);
            }
            break;
        }
        case BMW: {
            // TODO:test
            cv::Mat templ;
            if (DEBUG == 1)
            {
                templ =
                    cv::imread("E:\\project\\hydrus2\\mtfiq\\src\\app\\config\\templateImg\\crosshairTemplate.tiff");
            }
            else
            {
                templ = cv::imread("./config/templateImg/crosshairTemplate.tiff", 0);
            }
            MatchTemplate mt;
            re = mt.getMatchTemplateResult(img, templ, 90, center);
            for (int i = 0; i < center.size(); i++)
            {
                center[i] = center[i];
            }
            break;
        }
        default:
            break;     
    }
    m_img_draw = img.clone();
    for (int i=0;i<center.size();i++)
    {
        circle(m_img_draw, center[i], 10, Scalar(0, 0, 255), -1);
    }
    return re;
}
ALGResult algorithm::FiducialDetect::FiducialContour(cv::Mat img, vector<cv::Point2f> &loc)
{
    ALGResult re;
    re.success = false;
    if (img.data)
    {
        cv::Mat img_draw;
        cv::Mat gray;
        if (img.channels() == 3)
        {
            cvtColor(img, gray, COLOR_BGR2GRAY);
            img_draw = img.clone();
        }
        else
        {
            gray = img.clone();
            cvtColor(img, img_draw, COLOR_GRAY2BGR);
        }

        cv::GaussianBlur(gray, gray, Size(3, 3), 0, 0);
      //  cv::equalizeHist(gray, gray);
        cv::Mat element = cv::getStructuringElement(MORPH_RECT, Size(5, 5));
        cv::Mat element1 = cv::getStructuringElement(MORPH_RECT, Size(10, 10));

       // cv::morphologyEx(gray, gray, MORPH_DILATE, element);

        cv::morphologyEx(gray, gray, MORPH_GRADIENT, element1);
        cv::Mat imgth;
        cv::threshold(gray, imgth, 0, 255, THRESH_TRIANGLE);
        NaiveRemoveNoise(imgth, 1000);
        //Clear_MicroConnected_Areas(imgth, imgth, 1000);
        ContoursRemoveNoise(imgth, 1000);      
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(imgth, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
     //   drawContours(img_draw, contours, -1, Scalar(0, 0, 255), 2);

        for (int i = 0; i < contours.size();i++)
        {
            cv::RotatedRect rectR = cv::minAreaRect(contours[i]);
            cv::Size s = rectR.size;
            cv::Rect rect = boundingRect(contours[i]);
            double area = rectR.size.area(); // cv::contourArea(contours[i]);
         //   cv::Size s = rectR.size;
            double ratio = double(min(s.width, s.height)) / double(max(s.width, s.height));
            cv::drawContours(img_draw, contours,i, cv::Scalar(0, 0, 255), 5);
            double A = rect.area();
            //cv::contourArea(contours[i]);
            double d; //= cv::arcLength(contours[i],true);
            Point2f c0;
            float r;
            cv::minEnclosingCircle(contours[i], c0,r);
            d = 2 * CV_PI * r;
            double roundness = (4 * CV_PI * A) / (d * d);

            if (area>2.5e4&area<5e4&&ratio>0.75&ratio<1.2&&roundness>0.6)
            {
               // cout << area << "," << roundness << endl;
                drawContours(img_draw, contours, i, Scalar(0, 255, 0), 5);
                cv::Point2f center;
                cv::RotatedRect reRo = cv::minAreaRect(contours[i]);
                center = reRo.center;
                //std::cout << center << endl;
                cv::circle(img_draw, center, 5, Scalar(0, 0, 255), -1);
                loc.push_back(center);
                re.success = true;
                
          /*      float r;
                cv::minEnclosingCircle(contours[i], center, r);
                cout << center << endl;
                cv::circle(img_draw, center, 1, Scalar(0, 255, 0), -1);*/
        
            }
        
        }
        m_img_draw = img_draw.clone();
    }
    else
    {
        re.success = false;
        re.errorMsg = "The Input image is NULL";
   
    }

    return re;
}
cv::Mat algorithm::FiducialDetect::getImgDraw()
{
    return m_img_draw;
}