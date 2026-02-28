#include "ml_ImageQuality.h"
#include "LogPlus.h"
#include <log4cplus/fileappender.h>
using namespace algorithm;
using namespace cv;
 //m_rect = cv::Rect(-1,-1,0,0);

algorithm::MLImageQuality::MLImageQuality()
{
}

algorithm::MLImageQuality::~MLImageQuality()
{
}

double algorithm::MLImageQuality::getImgQualtiy(cv::Mat &image, IQmethod method)
{
    double re = -1;
    string str = "-----[getImgQualtiy]-----";
    if (image.data != NULL)
    {
       // if (m_rect.width <= 0)
         cv::Rect rect = getCircleROI(image);
        cv::Mat roi;
         if (rect.width <= 0||rect.height<=0)
             return 0;
         else
             roi = image(rect).clone();
        switch (method)
        {
        case 1: {
            re = brenner(roi);
            break;
        }
        case 2: {
            re = tenengard(roi);
            break;
        }
        case 3: {
            re = laplacian(roi);
            break;
        }
        case 4: {
            re = standarddeviation(roi);
            break;
        }
        default:
            break;
        }
    }
    else
    {
        string sMsg = "Image is NULL!-----ALG Error";
        string errorMessage = str + sMsg;
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, errorMessage.c_str());
        //  throw invalid_argument(errorMessage);
       // re.result.success = false;
      //  re.result.errorMsg = errorMessage;   
    }
    return re;
}

double algorithm::MLImageQuality::brenner(cv::Mat &image)
{

    //	assert(image.empty());
    cv::Mat gray_img;
    if (image.channels() == 3)
    {
        cv::cvtColor(image, gray_img, COLOR_BGR2GRAY);
    }
    double result = .0f;
    for (int i = 0; i < gray_img.rows; ++i)
    {
        uchar *data = gray_img.ptr<uchar>(i);
        for (int j = 0; j < gray_img.cols - 2; ++j)
        {
            result += pow(data[j + 2] - data[j], 2);
        }
    }

    return result / gray_img.total();
}

double algorithm::MLImageQuality::tenengard(cv::Mat &image)
{
    // assert(image.empty());
    cv::Mat gray_img, sobel_x, sobel_y, G;
    if (image.channels() == 3)
    {
        cv::cvtColor(image, gray_img, COLOR_BGR2GRAY);
    }
    else
        gray_img = image.clone();

    //分别计算x/y方向梯度
    cv::Sobel(gray_img, sobel_x, CV_32FC1, 1, 0);
    cv::Sobel(gray_img, sobel_y, CV_32FC1, 0, 1);
    cv::multiply(sobel_x, sobel_x, sobel_x);
    cv::multiply(sobel_y, sobel_y, sobel_y);
    cv::Mat sqrt_mat = sobel_x + sobel_y;
    cv::sqrt(sqrt_mat, G);
    return cv::mean(G)[0];
}

double algorithm::MLImageQuality::laplacian(cv::Mat &image)
{

    // assert(image.empty());

    cv::Mat gray_img, lap_image;
    if (image.channels() == 3)
    {
        cv::cvtColor(image, gray_img, COLOR_BGR2GRAY);
    }
    else
        gray_img = image.clone();

    cv::Laplacian(gray_img, lap_image, CV_32FC1);
    lap_image = cv::abs(lap_image);

    return cv::mean(lap_image)[0];
}

double algorithm::MLImageQuality::standarddeviation(cv::Mat &image)
{

    cv::Mat gray_img, lap_image;
    if (image.channels() == 3)
    {
        cv::cvtColor(image, gray_img, COLOR_BGR2GRAY);
    }
    else
        gray_img = image.clone();
    Scalar m0, std0;
    cv::meanStdDev(gray_img, m0, std0);
    return std0[0];
}

cv::Rect algorithm::MLImageQuality::getCircleROI(cv::Mat img)
{
    cv::Rect rect;
    if (!img.data)
        return rect;
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
    vector<vector<Point>> contours;
    vector<Vec4i> hierachy;
    findContours(imgth, contours, hierachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(-1, -1));
    Point2f pc;
    float r = 0;
    cv::Point2f loc = cv::Point2f(837, 1701);
    int yLoc = 1400;
    vector<double> disVec;
    vector<double> rVec;
    vector<cv::Point2f> cenVec;
    for (int i = 0; i < contours.size(); i++)
    {
        cv::minEnclosingCircle(contours[i], pc, r);
        if (r > 200 && r < 300)
        {
            cv::circle(img_draw, pc, r, Scalar(0, 255, 0), 1);
            cv::circle(img_draw, pc, 5, Scalar(0, 255, 0), -1);
            rVec.push_back(r);
            cenVec.push_back(pc);
            disVec.push_back(abs(pc.y - yLoc));
        }
    }
    cv::Rect rectAll = cv::Rect(0, 0, gray.cols, gray.rows);
    if (rVec.size()==1)
    {
        int len = 20;
        rect.x = cenVec[0].x - rVec[0]-len;
        rect.y = cenVec[0].y - rVec[0]-len;
        rect.width = 2 * rVec[0] + 2 * len;
        rect.height = 2 * rVec[0] + 2 * len;
        rect = rect & rectAll;
    }
    else if (rVec.size()>1)
    {
        int len = 20;
        int maxIndx = 0;
        maxIndx = max_element(disVec.begin(), disVec.end()) - disVec.begin();
        rect.x = cenVec[maxIndx].x - rVec[maxIndx]-len;
        rect.y = cenVec[maxIndx].y - rVec[maxIndx]-len;
        rect.width = 2 * rVec[maxIndx] + 2 * len;
        rect.height = 2 * rVec[maxIndx] + 2 * len;
        rect = rect & rectAll;
    }
    rectangle(img_draw, rect, Scalar(255, 0, 255), 1);
    return rect;
}
