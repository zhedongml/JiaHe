#include "ml_inputgratingDetect.h"
#include "LogPlus.h"
#include "ml_cross_center.h"
#include "ml_matchTemplate.h"
using namespace cv;
using namespace algorithm;
algorithm::MLInputgratingDetect::MLInputgratingDetect()
{
}

algorithm::MLInputgratingDetect::~MLInputgratingDetect()
{
}

ALGResult algorithm::MLInputgratingDetect::getInputAndProjectPixelDistance(cv::Mat projectImg, cv::Mat inputImg,
                                                                           cv::Point2f &dis)
{
    ALGResult re;
    // cv::Point2f dis;
    if (projectImg.data != NULL && inputImg.data != NULL)
    {
        cv::Point2f cPro = getProjectImgCoordinate(projectImg);
        cv::Point2f cInput = getInputImgCoordinate(inputImg);
        if (cPro.x <= 0 || cPro.y <= 0)
        {
            re.success = false;
            re.errorMsg = "The projector image detection faill";
            return re;
        }
        if (cInput.x <= 0 || cInput.y <= 0)
        {
            re.success = false;
            re.errorMsg = "The inputgrating image detection faill";
            return re;
        }
        dis = cPro - cInput;
    }
    else
    {
        re.success = false;
        re.errorMsg = "The image is NULL";
    }
    return re;
}

cv::Point2f algorithm::MLInputgratingDetect::getInputImgCoordinate(cv::Mat inputImg)
{
    cv::Point2f c0;
    vector<cv::Point> c = circleDetectHough(inputImg);
    cv::Point2f mark = cv::Point2f(850, 1329);
    if (c.size() > 1)
    {
        vector<double> disVec;
        for (int i = 0; i < c.size(); i++)
        {
            double dis = Getdistance(c[i], mark);
            disVec.push_back(dis);
        }
        int minIndex = min_element(disVec.begin(), disVec.end()) - disVec.begin();
        c0 = c[minIndex];
    }
    else if (c.size() == 1)
        c0 = c[0];

    cv::Mat img_draw = convertTo3Channels(inputImg);
    circle(img_draw, c0, 5, Scalar(0, 0, 255), -1);
    m_img_draw = img_draw.clone();
    return c0;
}

InputRe algorithm::MLInputgratingDetect::getInputImgCoordinateRectangle(cv::Mat img)
{
    string info = "-----getInputImgCoordinateRectangle------";
    InputRe re;
    if (img.empty())
    {
        re.flag = false;
        re.errMsg = info + "Input is NULL";
        return re;
    }
    cv::Mat gray = convertToGrayImage(img);
    cv::Mat imgdraw = convertTo3Channels(img);

    cv::Mat kernel = cv::getStructuringElement(MORPH_RECT, Size(5, 5));
    cv::morphologyEx(gray, gray, MORPH_GRADIENT, kernel);
    cv::Mat imgth;
    cv::threshold(gray, imgth, 0, 255, THRESH_OTSU);
    vector<vector<Point>> contours;
    vector<Vec4i> hierachy;
    findContours(imgth, contours, hierachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(-1, -1));
    //cv::RotatedRect rectR;
    for (int i=0;i<contours.size();i++)
    {
        double area = cv::contourArea(contours[i]);
        cv::RotatedRect rectR = cv::minAreaRect(contours[i]);
        cv::Size s = rectR.size;
        int w = min(s.width, s.height);
        int h = max(s.width, s.height);
        if (area>10e4&&area<40e4&&w>150&w<250&&h>800&h<900)
        {
            cv::drawContours(imgdraw, contours, i, Scalar(0, 0, 255), 1);
            drawPointOnImage(imgdraw, rectR.center);
            re.center = rectR.center;
            re.imgdraw = imgdraw;
            break;
          // cout << area << rectR.size << endl;
        }
    
    }
    
    return re;
}

cv::Point2f algorithm::MLInputgratingDetect::getProjectImgCoordinate(cv::Mat img)
{
    cv::Point2f c0;
    
        if (!img.data)
            return c0;
        cv::Mat gray;
        cv::Mat img_draw;
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
        cv::GaussianBlur(gray, gray, Size(5, 5), 0, 0);
        cv::Mat element = cv::getStructuringElement(MORPH_RECT, Size(20, 20));
        cv::morphologyEx(gray, gray, MORPH_DILATE, element);
        cv::Mat element1 = cv::getStructuringElement(MORPH_RECT, Size(20, 20));
        cv::morphologyEx(gray, gray, MORPH_GRADIENT, element);
        cv::Mat imgth;
        cv::threshold(gray, imgth, 0, 255, THRESH_TRIANGLE);
        NaiveRemoveNoise(imgth, 1000);
        ContoursRemoveNoise(imgth, 1000);
        cv::Mat can;
        cv::Canny(imgth,can,30,90);
        // hough test
        vector<double> rVec;
        vector<cv::Point2f> cenVec;
        vector<cv::Vec3f> circles;
 /*       HoughCircles(can, circles, HOUGH_GRADIENT, 1.5, 200, 30, 90, 200, 300);
        for (int i = 0; i < circles.size(); i++)
        {
            double r = circles[i][2];
            if (r > 200 & r < 300)
            {
                cv::Point2f cen = Point(circles[i][0], circles[i][1]);
                circle(img_draw, cen, r, cv::Scalar(0, 0, 255), 1);
                circle(img_draw, cen, 5, cv::Scalar(0, 0, 255), -1);
                rVec.push_back(r);
                cenVec.push_back(cen);
            }
        }*/
        // use the contour
        vector<vector<Point>> contours;
        vector<Vec4i> hierachy;
        findContours(imgth, contours, hierachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(-1, -1));
        Point2f pc;
        float r = 0;
        cv::Point2f loc = cv::Point2f(837, 1701);
        int yLoc = 1400;
        vector<double> disVec;
        for (int i = 0; i < contours.size(); i++)
        {
            cv::minEnclosingCircle(contours[i], pc, r);
            double area = cv::contourArea(contours[i]);
            cv::Rect rect = cv::boundingRect(contours[i]);
            double ratio = double(max(rect.height, rect.width)) / double(min(rect.height, rect.width));
            if (r > 200 && r < 300&area>1e3&ratio>0.8&ratio<1.3)
            {
                cv::drawContours(img_draw,contours, i, Scalar(255, 0, 0), 1);
                cv::circle(img_draw, pc, r, Scalar(0, 255, 0), 1);
                cv::circle(img_draw, pc, 5, Scalar(0, 255, 0), -1);
                rVec.push_back(r);
                cenVec.push_back(pc);
               // disVec.push_back(Getdistance(loc, pc));
                disVec.push_back(abs(pc.y-yLoc));

            }
        }
        cv::Rect rectAll = cv::Rect(0, 0, gray.cols, gray.rows);
        if (rVec.size() == 1)
        {
            c0 = cenVec[0];
        }
        else if (rVec.size() > 1)
        {
            int maxIndx = 0;
            maxIndx = max_element(disVec.begin(), disVec.end()) - disVec.begin();
            c0 = cenVec[maxIndx];
        }   
        cv::circle(img_draw, c0, 5, Scalar(0, 0, 255), -1);
    return c0;
}
    
cv::Mat algorithm::MLInputgratingDetect::getImgDraw()
{
    return m_img_draw;
}

vector<cv::Point> algorithm::MLInputgratingDetect::circleDetectHough(cv::Mat img)
{
    vector<cv::Point> loc;
    cv::Mat img_draw = img.clone();
    cv::Mat img_gray;
    if (img.channels() == 3)
        cvtColor(img, img_gray, COLOR_BGR2GRAY);
    else
        img_gray = img.clone();

    cv::Mat imgBlur;
    cv::GaussianBlur(img_gray, imgBlur, cv::Size(3, 3), 0, 0);
    vector<cv::Vec3f> circles;
    HoughCircles(imgBlur, circles, HOUGH_GRADIENT, 1.5, 200, 30, 90, 230, 300);
    for (int i = 0; i < circles.size(); i++)
    {
        double r = circles[i][2];
        if (r > 120 & r < 300)
        {
            cv::Point2f cen = Point(circles[i][0], circles[i][1]);
            circle(img_draw, cen, r, cv::Scalar(0, 0, 255), 1);
            circle(img_draw, cen, 5, cv::Scalar(0, 0, 255), -1);
            loc.push_back(cen);
        }
    }
    m_img_draw = img_draw.clone();
    return loc;
}

vector<cv::Point> algorithm::MLInputgratingDetect::circleDetectContour(cv::Mat img)
{
    vector<cv::Point> loc;
    cv::Mat img_draw = img.clone();
    cv::Mat img_gray;
    if (img.channels() == 3)
        cvtColor(img, img_gray, COLOR_BGR2GRAY);
    else
        img_gray = img.clone();
    cv::Mat imgBlur;
    cv::GaussianBlur(img_gray, imgBlur, cv::Size(5, 5), 0, 0);
    cv::Mat element = cv::getStructuringElement(MORPH_RECT, Size(5, 5));
    cv::morphologyEx(imgBlur, imgBlur, MORPH_GRADIENT, element);
    //  cv::Mat mask = cv::Mat::zeros(imgBlur.size(), CV_8UC1);
    //  circle(mask, cv::Point(1156, 928),202, Scalar(255),-1);

    // cv::Mat dst = mask + imgBlur;
    // imgBlur.setTo(0, mask);

    cv::Mat imgBinarry;
    cv::threshold(imgBlur, imgBinarry, 0, 255, THRESH_OTSU);
    NaiveRemoveNoise(imgBinarry, 1000);
    ContoursRemoveNoise(imgBinarry, 1000);
    cv::Mat imgBinarry1;
    // cv::bitwise_not(imgBinarry, imgBinarry1);
    // NaiveRemoveNoise(imgBinarry, 150);
    //  Clear_MicroConnected_Areas(imgBinarry1, imgBinarry1, 500);
    // NaiveRemoveNoise(imgBinarry1, 500);
    // cv::bitwise_not(imgBinarry1, imgBinarry);

    // find contours
    vector<vector<Point>> contours;
    vector<Vec4i> hierachy;
    findContours(imgBinarry, contours, hierachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(-1, -1));
    Point2f pc;
    float r = 0;
    vector<vector<Point>> fitpts;
    for (int i = 0; i < contours.size(); i++)
    {
        // drawContours(img_draw, contours, i, Scalar(255, 0, 255), 1, 8);
        cv::minEnclosingCircle(contours[i], pc, r);
        if (r > 200 && r < 300)
        {
            // drawContours(img_draw, contours, i, Scalar(255, 0, 255), 1, 8);
            cv::circle(img_draw, pc, r, Scalar(0, 0, 255), 1);
            cv::circle(img_draw, pc, 5, Scalar(0, 0, 255), -1);
            loc.push_back(pc);
        }
    }
    m_img_draw = img_draw.clone();

    return loc;
}

vector<cv::Point> algorithm::MLInputgratingDetect::circleMatchTemplate(cv::Mat img)
{
    vector<cv::Point> pt;
    MatchTemplate mt;
    cv::Mat templ;
    if (DEBUG == 1)
    {
        templ = cv::imread("E:\\project\\gen2\\mlx\\mlx\\ALGImgs\\circletemplate.tif");
    }
    else
    {
        templ = cv::imread("./ALGImgs/circletemplate.tif");
    }
    if (img.data != NULL & templ.data != NULL)
    {
        vector<cv::Point2f> loc;
         mt.getMatchTemplateResult(img, templ, 40, loc);
        if (loc.size() > 0)
            pt.push_back(loc[0] + Point2f(225, 230));
        cv::Mat img_draw = img.clone();
        if (pt.size() > 0)
            circle(img_draw, pt[0], 5, Scalar(255, 0, 255), -1);
        m_img_draw = img_draw.clone();
        string message = "Circle inputgrating detect successfully";
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
    }
    else
    {
        string message = "img or template image  is NULL";
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
    }
    return pt;
}
