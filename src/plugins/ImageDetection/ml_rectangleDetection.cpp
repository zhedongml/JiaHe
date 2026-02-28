#include "ml_rectangleDetection.h"
#include"LogPlus.h"
MLImageDetection::RectangleDetection::RectangleDetection()
{
}

MLImageDetection::RectangleDetection::~RectangleDetection()
{
}

void MLImageDetection::RectangleDetection::setBinNum(int num)
{
    m_binNum = num;
}

cv::RotatedRect MLImageDetection::RectangleDetection::getRectangleBorder(cv::Mat img)
{
    RotatedRect re;
    string infoHeader = "---MLImageDetection log---RectangleDetectionALG---";
    if (img.data!=NULL)
    {
        cv::Mat gray;
        cv::Mat img8 = convertToUint8(img);
        cv::Mat img_draw = convertTo3Channels(img8);
        cv::GaussianBlur(img, gray, Size(5, 5), 3, 3);
        Mat kernel = getStructuringElement(MORPH_RECT, Size(10, 10), Point(-1, -1));
        morphologyEx(gray, gray, MORPH_GRADIENT, kernel, Point(-1, -1));
        cv::Mat imgth;
        cv::threshold(gray, imgth, 0, 255, THRESH_TRIANGLE);
        vector<vector<Point>> contours;
        vector<Vec4i> hierachy;
        findContours(imgth, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        vector<cv::Point2f> centers;
      //  drawContours(img_draw, contours, -1, Scalar(0, 0, 255), 10);
        for (int i = 0; i < contours.size(); i++)
        {
            double area = cv::contourArea(contours[i]);
            cv::Rect rect0 = cv::boundingRect(contours[i]);
            cv::RotatedRect rectR = cv::minAreaRect(contours[i]);
            Point2f P[4];
            rectR.points(P);
            double w = Getdistance(P[0], P[1]);
            double h = Getdistance(P[1], P[2]);
            double ratio = min(w, h) / max(w, h);
            if (rect0.area()>5e4 /*&& ratio>0.6 && ratio<1.2*/)
            {
                re = rectR;
                cv::line(img_draw,P[0],P[1],Scalar(0,0,255),1);
                cv::line(img_draw, P[1], P[2], Scalar(0, 0, 255), 1);
                cv::line(img_draw, P[2], P[3], Scalar(0, 0, 255), 1);
                cv::line(img_draw, P[0], P[3], Scalar(0, 0, 255), 1);
                           
                m_img_draw = img_draw.clone();
                break;
            }          
        }
       
    }
    else
    {
        string message = infoHeader +"The input image is NULL";
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
    }
    return re;
}

cv::Rect MLImageDetection::RectangleDetection::getSolidExactRect(cv::Mat img8, cv::Rect rect)
{
    cv::Rect rect0;
    cv::Mat imgdraw = convertTo3Channels(img8);
    cv::Point h1 = rect.tl() + Point(0, rect.height / 2);
    cv::Point h2 = rect.tl() + Point(rect.width, rect.height / 2);
    cv::Point v1 = rect.tl() + Point(rect.width/2, 0);
    cv::Point v2 = rect.tl() + Point(rect.width/2, rect.height);
   
    circle(imgdraw, h1, 5, Scalar(0, 0, 255), -1);
    circle(imgdraw, h2, 5, Scalar(0, 0, 255), -1);
    circle(imgdraw, v1, 5, Scalar(0, 255, 0), -1);
    circle(imgdraw, v2, 5, Scalar(0, 255, 0), -1);
    int len = m_len / m_binNum;
    int start_h1 = h1.x-max(0, h1.x - len);
    int start_h2 = h2.x - max(0, h2.x - len);
    int start_v1 =v1.y- max(0, v1.y - len);
    int start_v2 = v2.y-max(0, v2.y - len);

    cv::Mat hmat1 = img8(cv::Range(h1.y-5,h1.y+5),cv::Range(max(0,h1.x-len),min(h1.x+ len,img8.cols)));
    cv::Mat hmat2 = img8(cv::Range(h2.y - 5, h2.y + 5), cv::Range(max(0,h2.x - len),min(h2.x + len,img8.cols)));
    cv::Mat vmat1 = img8(cv::Range(max(0,v1.y - len), min(v1.y + len,img8.rows)), cv::Range(v1.x - 5, v1.x + 5));
    cv::Mat vmat2 = img8(cv::Range(max(0,v2.y - len), min(v2.y + len,img8.rows)), cv::Range(v1.x - 5, v1.x + 5));
    cv::rotate(vmat1, vmat1, ROTATE_90_COUNTERCLOCKWISE);
    cv::rotate(vmat2, vmat2, ROTATE_90_COUNTERCLOCKWISE);
    cv::medianBlur(hmat1, hmat1, 3);
    cv::medianBlur(hmat2, hmat2, 3);
    cv::medianBlur(vmat1, vmat1, 3);
    cv::medianBlur(vmat2, vmat2, 3);

    cv::reduce(hmat1, hmat1, 0, REDUCE_AVG);
    cv::reduce(hmat2, hmat2, 0, REDUCE_AVG);
    cv::reduce(vmat1, vmat1, 0, REDUCE_AVG);
    cv::reduce(vmat2, vmat2, 0, REDUCE_AVG);

    int hindex1=findEdgePt(hmat1);
    int hindex2 = findEdgePt(hmat2);
    int vindex1 = findEdgePt(vmat1);
    int vindex2 = findEdgePt(vmat2);
    if (start_h1 == 0)
        hindex1 = 0;
    if (start_h2 == 0)
        hindex2 = 0;
    if (start_v1 == 0)
        vindex1 = 0;
    if (start_v2 == 0)
        vindex2 = 0;

    // bug£¬ change the len by real
    cv::Point2f h1Upate(h1.x+hindex1 - start_h1,h1.y);
    cv::Point2f h2Upate(h2.x+hindex2 - start_h2, h2.y);
    cv::Point2f v1Upate(v1.x,v1.y+vindex1- start_v1);
    cv::Point2f v2Upate(v2.x, v2.y+vindex2 - start_v2);
    circle(imgdraw, h1Upate, 5, Scalar(255, 0, 255), -1);
    circle(imgdraw, h2Upate, 5, Scalar(255, 0, 255), -1);
    circle(imgdraw, v1Upate, 5, Scalar(255, 0, 255), -1);
    circle(imgdraw, v2Upate, 5, Scalar(255, 0, 255), -1);
    rect0.x = h1Upate.x;
    rect0.y = v1Upate.y;
    rect0.width = h2Upate.x - h1Upate.x;
    rect0.height = v2Upate.y - v1Upate.y;
    cv::rectangle(imgdraw, rect0, Scalar(0, 0, 255), 10);
    return rect0;
}

cv::Mat MLImageDetection::RectangleDetection::getImgdraw()
{
    return m_img_draw;
}
