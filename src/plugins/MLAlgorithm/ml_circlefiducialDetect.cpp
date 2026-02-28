#include "ml_circlefiducialDetect.h"
#include <opencv2/imgproc/types_c.h>
#include <opencv2\opencv.hpp>
#include <QDebug>
using namespace algorithm;

algorithm::MLCircleFiducialDetect::MLCircleFiducialDetect()
{
}
algorithm::MLCircleFiducialDetect::~MLCircleFiducialDetect()
{
}
std::vector<cv::Point2f> algorithm::MLCircleFiducialDetect::CircleFiducialDetect(cv::Mat img)
{
    vector<cv::Point2f> center;
    vector<cv::Vec3f> location;
  
    location = CircleFiducialDetectMatch(img);
    //if (location.size()<1)
    //{
    //    location = CircleFiducialDetectHough(img); 
    //}
    //else if (location.size()<1)
    //{
    //    location = CircleFiducialDetectFit(img);
    //}
    
    for (int i = 0; i < location.size();i++)
    {
        center.emplace_back(location[i][0], location[i][1]);
    }
    return center;
}
std::vector<cv::Vec3f> algorithm::MLCircleFiducialDetect::CircleFiducialRecognition(cv::Mat img)
{
    vector<cv::Vec3f> location;

    location = CircleFiducialDetectFit(img);
    if (location.size() < 1)
    {
        location = CircleFiducialDetectHough(img);
    }
    else if (location.size() < 1)
    {
        location = CircleFiducialDetectMatch(img);
    }
    return location;
}
vector<cv::Vec3f> algorithm::MLCircleFiducialDetect::CircleFiducialDetectFit(Mat img)
{
    vector<cv::Vec3f> newcircles;
    vector<cv::Vec3f> location;
    if(img.data!=NULL){
        Mat img_gray;
        if (img.channels() == 3) // Convert to grayscale
            cvtColor(img, img_gray, CV_BGR2GRAY);
        else
            img_gray = img.clone();
        cv::Mat img_draw =img.clone() ;//convertToUint8(img);
       // img_gray = convertToUint8(img_gray);
        vector<cv::Point2f> center;
        Mat img_th;
        Point minp, maxp;
        double minv = 0.0, maxv = 0.0;
        minMaxLoc(img_gray, &minv, &maxv, &minp, &maxp);
        GaussianBlur(img_gray, img_gray, Size(3, 3), 0, 0);
        equalizeHist(img_gray, img_gray);
        // C++
   /*     Ptr<CLAHE> clahe = createCLAHE();
        clahe->apply(img_gray, img_gray);*/


        //equalizeHist(img_gray, img_gray);
        Mat element1 = getStructuringElement(MORPH_RECT, Size(5, 5));
         erode(img_gray, img_gray, element1);// Swell, let the black lumps separate
        // cv::morphologyEx(img_gray, img_gray, MORPH_CLOSE, element1);

     //   cv::morphologyEx(img_gray, img_gray, MORPH_GRADIENT, element1);
        threshold(img_gray, img_th, 0, 255, CV_THRESH_OTSU);
        dilate(img_th, img_th, element1); // Swell, let the black lumps separate
        // find contours
        vector<vector<Point>> contours;
        vector<Vec4i> hierachy;
        findContours(img_th, contours, hierachy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(-1, -1));
        // findContours(canny_out, contours, hierachy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
        Point2f pc;
        float r = 0;
        vector<vector<Point>> fitpts;
        for (int i = 0; i < contours.size(); i++)
        {
          //  drawContours(img_draw, contours, i, Scalar(rand() % 255, rand() % 255, rand() % 255), 1, 8);
            // cout << contourArea(contours_new[i], false) << endl;
            // float r;
            double area = contourArea(contours[i], false);
            cv::minEnclosingCircle(contours[i], pc, r);
            if (r > 30 && r < 100)
            {
                drawContours(img_draw, contours, i, Scalar(0, 0, 255), 3, 8);
                cv::circle(img_draw, pc, r, Scalar(0, 0, 255), 1);
                cv::circle(img_draw, pc, 2, Scalar(0, 0, 255), -1);
                m_img_draw = img_draw.clone();
                // cout << pc << r << endl;
                center.push_back(pc);
                location.push_back(cv::Vec3f(pc.x, pc.y, r));
                fitpts.push_back(contours[i]);
            }
        }

        vector<cv::Point> pts;
        if (fitpts.size() >= 1)
            pts = fitpts[0];
        for (int i = 1; i < fitpts.size(); i++)
        {
            for (int j = 0; j < fitpts[i].size(); j++)
            {
                pts.push_back(fitpts[i][j]);
            }
        }
        cv::Vec3f loc;
        if (pts.size() > 0)
        {
            cv::Point2f c0;
            double r0;
            circleLeastFit(pts, c0, r0);
            loc = cv::Vec3f(c0.x, c0.y, r0);
            cv::circle(img_draw, c0, r0, Scalar(255, 0, 0), 1);
            cv::circle(img_draw, c0, 5, Scalar(255, 0, 0), -1);
        }
        newcircles.push_back(loc);
        m_img_draw = img_draw.clone();

    }
    else{
        qCritical() << "input image is empty !!";
    }
    return newcircles;
}
std::vector<cv::Vec3f> closeTocenterfilter(cv::Mat img, vector<cv::Vec3f> circles)
{
    vector<cv::Vec3f> location;
    vector<cv::Point2f> center;
    cv::Mat img_draw = img.clone();
    int centerX = img.cols / 2;
    int centerY = img.rows / 2;

    int minR = centerX * centerX + centerY * centerY;
    int index = 0;
    for (int i = 0; i < circles.size(); i++)
    {
        int deltaX = circles[i][0] - centerX;
        int deltaY = circles[i][1] - centerY;

        int currR = deltaX * deltaX + deltaY * deltaY;
        if (minR > currR)
        {
            minR = currR;
            index = i;
        }
    }
    float r = circles[index][2];
    center.emplace_back(circles[index][0], circles[index][1]);
    circle(img_draw, center[0], r, cv::Scalar(0, 0, 255), 1);
    location.push_back((center[0].x, center[0].y, r));
    return location;
}
void algorithm::MLCircleFiducialDetect::circleSort(vector<cv::Vec3f> circles,vector<cv::Vec3f>&newcircles)
{
    if (circles.size() <= 1)
        newcircles = circles;
    else
    {

        std::sort(circles.begin(), circles.end(),
                  [](cv::Vec3f &a, cv::Vec3f &b) { return a[1] > b[1]; }); // change sort by y,from bottom to top
        // std::sort(circles.begin(), circles.begin() + 1, [](cv::Vec3f& a, cv::Vec3f& b) { return a[0] < b[0];
        // }); std::sort(circles.begin() + 2, circles.end(), [](cv::Vec3f& a, cv::Vec3f& b) { return a[0] <
        // b[0]; });
        for (size_t i = 0; i < circles.size(); i++)
        {
            cv::Point2f center(circles[i][0], circles[i][1]);
            int radius = cvRound(circles[i][2]);
            newcircles.push_back(Vec3f(center.x, center.y, radius));
        }
    }
}

bool algorithm::MLCircleFiducialDetect::circleFiducailTest(cv::Mat src, Rect rect, int circleRadius, cv::Point2f &center)
{
    //clock_t t1 = clock();

    Mat grayImage;
    if (src.channels() == 4)
    {
        cvtColor(src, grayImage, COLOR_BGRA2GRAY);
    }
    else if (src.channels() == 3)
        cvtColor(src, grayImage, COLOR_BGR2GRAY);
    else
        grayImage = src.clone();
    grayImage = grayImage(rect);

    Mat blurImage;
    GaussianBlur(grayImage, blurImage, Size(9, 9), 2, 2);

    Mat edgesImage;
    Canny(blurImage, edgesImage, 1, 10);

    vector<Vec3f> circles;
    int minDist = 30;
    int param1 = 10;
    int param2 = 25;
    int minRadius = circleRadius - 10;
    int maxRadius = circleRadius + 10;
    HoughCircles(blurImage, circles, HOUGH_GRADIENT, 1, minDist, param1, param2, minRadius, maxRadius);

    if (circles.size() == 0)
    {
        param1 = 5;
        param2 = 25;
        HoughCircles(blurImage, circles, HOUGH_GRADIENT, 1, minDist, param1, param2, minRadius, maxRadius);
    }

    //clock_t t2 = clock();

    cv::cvtColor(src, m_img_draw, cv::COLOR_GRAY2RGB);
    for (size_t i = 0; i < circles.size(); i++)
    {
        Point centerTmp(cvRound(circles[i][0]), cvRound(circles[i][1]));
        center = centerTmp + Point(rect.x, rect.y);
        circle(m_img_draw, center, 2, Scalar(0, 0, 255), -1, 8, 0);

        int radius = cvRound(circles[i][2]);
        circle(m_img_draw, center, radius, Scalar(0, 0, 255), 3, 8, 0);
    }
    // clock_t t3 = clock();
    // double timeForHoughCircle = (double)(t2 - t1) / CLOCKS_PER_SEC * 1000;
    // double timeForDrawCircle = (double)(t3 - t2) / CLOCKS_PER_SEC * 1000;
    // cout << "circles size: " << circles.size() << ". " << "HoughCircles:" << timeForHoughCircle << "ms, DrawCircle:"
    // << timeForDrawCircle << "ms" << endl;

    if (circles.size() != 1)
    {
        cout << "Calculate circle radius error, circle size is " << circles.size() << endl;
        return false;
    }
    return true;
}

bool algorithm::MLCircleFiducialDetect::circleRadius(string fileName, float &radius, Rect &rect)
{
    clock_t t1 = clock();

    Mat grayImage, src;
    src = imread(fileName, IMREAD_UNCHANGED);
    if (src.empty())
    {
        return false;
    }

    if (src.channels() == 4)
    {
        cvtColor(src, grayImage, COLOR_BGRA2GRAY);
    }
    else if (src.channels() == 3)
        cvtColor(src, grayImage, COLOR_BGR2GRAY);
    else
        grayImage = src.clone();

    Mat blurImage;
    GaussianBlur(grayImage, blurImage, Size(9, 9), 2, 2); 
    // medianBlur(grayImage, blurImage, 7); 
    // blur(grayImage, blurImage, Size(7, 7));

    Mat edgesImage;
    Canny(blurImage, edgesImage, 1, 10);

    vector<Vec3f> circles;
    int minDist = 30;   
    int param1 = 10;    
    int param2 = 40;    
    int minRadius = 10; 
    int maxRadius = 120;

    int min = src.rows > src.cols ? src.cols : src.rows;
    if (min / 2 < maxRadius)
    {
        maxRadius = min / 2;
    }

    HoughCircles(blurImage, circles, HOUGH_GRADIENT, 1, minDist, param1, param2, minRadius, maxRadius);
    clock_t t2 = clock();
    for (size_t i = 0; i < circles.size(); i++)
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        radius = cvRound(circles[i][2]);
        circle(src, center, 3, Scalar(0, 0, 255), -1, 8, 0);    
    }
    //clock_t t3 = clock();
    //double timeForHoughCircle = (double)(t2 - t1) / CLOCKS_PER_SEC * 1000;
    //double timeForDrawCircle = (double)(t3 - t2) / CLOCKS_PER_SEC * 1000;
    //cout << "CircleRadius:" << timeForHoughCircle << "ms, DrawCircle:" << timeForDrawCircle << "ms" << endl;

    if (circles.size() != 1)
    {
        cout << "Calculate circle radius error, circle size is " << circles.size() << endl;
        return false;
    }

    rect.x = cvRound(circles[0][0]);
    rect.y = cvRound(circles[0][1]);
    rect.width = radius * 2;
    rect.height = radius * 2;
    return true;
}
vector<cv::Vec3f> algorithm::MLCircleFiducialDetect::CircleFiducialDetectHough(Mat img)
{
    vector<cv::Vec3f> newcircles;
    vector<cv::Vec3f> location;
    if (img.data != NULL)
    {
        Mat img_gray;
        if (img.channels() == 3) // Convert to grayscale
            cvtColor(img, img_gray, CV_BGR2GRAY);
        else
            img_gray = img.clone();
        vector<cv::Point2f> center;
        float r = 0;
        cv::Mat img_draw = convertToUint8(img);
        cv::Mat img8 = convertToUint8(img_gray);
        cv::Mat cal_img;
        GaussianBlur(img8, cal_img, Size(5, 5), 1, 1);

        vector<cv::Vec3f> circles;
        //HoughCircles(cal_img, circles, HOUGH_GRADIENT, 2, 200, 70, 30, 90, 120);
        HoughCircles(cal_img, circles, HOUGH_GRADIENT, 2, 200, 70, 30, 30, 45);

        for (int i = 0; i < circles.size();i++)
        {
            r = circles[i][2];
            cv::Point2f cen= Point2f(circles[i][0], circles[i][1]);
            circle(img_draw, cen, r, cv::Scalar(0, 0, 255), 1);
            circle(img_draw, cen, 5, cv::Scalar(0, 0, 255), -1);
            if (r>30&r<60)
            location.push_back(circles[0]);
        }
        
        std::vector<cv::Point2f> cen;
        circleSort(location, newcircles);
        for (size_t i = 0; i < newcircles.size(); i++)
        {
            cv::Point2f center(newcircles[i][0], newcircles[i][1]);
            int radius = cvRound(newcircles[i][2]);
            cen.push_back(center);
            circle(img_draw, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
            circle(img_draw, center, radius, cv::Scalar(255, 50, 255), 3, 8, 0);
        }
        
        m_img_draw = img_draw.clone();
    }
    else{
        qCritical() << "input image is empty !!";
    }
    return newcircles;
}
std::vector<cv::Vec3f> algorithm::MLCircleFiducialDetect::CircleFiducialDetectMatch(cv::Mat img)
{
    std::vector<cv::Vec3f> circles;
    cv::Mat temp;
    if (DEBUG==1)
    {
     temp = cv::imread("E:\\project\\hydrus2\\mtfiq\\src\\app\\config\\templateImg\\circletemplate1.tiff"); 
    }
    else
    {
        temp = cv::imread("./config/templateImg/circletemplate1.tiff"); 
    }
    if (img.data != NULL && temp.data != NULL)
    {
        std::vector<cv::Point2f> points;
        cv::Mat img_draw = img.clone();
        vector<Point> P;
        GetAllMinLoc(img, temp, sim, Scalar(0, 0, 0), &P, 1);
        for (int k = 0; k < P.size(); k++)
        {
            cv::Point loc = P[k];
            cv::Mat roi = img(cv::Rect(loc.x, loc.y, temp.cols, temp.rows));         
            vector<cv::Vec3f> corner;
            corner=CircleFiducialDetectHough(roi);
           // cout << corner[0][0] << "," << corner[0][0] << endl;
         //   if (corner.size()<1)
                corner=CircleFiducialDetectFit(roi);
          //  cout << corner[0][0] << "," << corner[0][0] << endl;

            
            if (!corner.empty())
            {
                cv::Point2f c = cv::Point2f(corner[0][0] + loc.x, (corner[0][1] + loc.y));
                circle(img_draw, c, corner[0][2], cv::Scalar(0, 0, 255), 4);
                circle(img_draw, c, 5, cv::Scalar(0, 255, 0), -1, 8, 0);
                circles.push_back(cv::Vec3f(c.x, c.y, corner[0][2]));
                points.push_back(c);
            }
        }
      
        vector<cv::Vec3f> newcircles;
        std::vector<cv::Point2f> cen;
        circleSort(circles, newcircles);
        for (size_t i = 0; i < newcircles.size(); i++)
        {
            cv::Point2f center(newcircles[i][0], newcircles[i][1]);
            int radius = cvRound(newcircles[i][2]);
            cen.push_back(center);
            circle(img_draw, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
            circle(img_draw, center, radius, cv::Scalar(0, 50, 255), 3, 8, 0);
        }       
            m_img_draw = img_draw.clone();
    }
    else{
        qCritical() << "input image is empty !!";
    }
    return circles;
}
cv::Mat algorithm::MLCircleFiducialDetect::getImgDraw()
{
    return m_img_draw;
}

bool algorithm::MLCircleFiducialDetect::circleFiducailTest(cv::Mat src, Rect rect, cv::Point2f &center)
{
    if (src.data == NULL)
    {
        return false;
    }

    float radius;
    Rect rectTmp;
    bool ret = circleRadius("./config/templateImg/circletemplate1.tiff", radius, rectTmp);
    if (!ret)
    {
        return ret;    
    }

    ret = circleFiducailTest(src, rect, radius, center);
    return ret;
}

