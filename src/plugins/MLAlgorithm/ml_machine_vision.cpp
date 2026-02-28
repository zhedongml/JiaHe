#include "libcbdetect/boards_from_corners.h"
#include "libcbdetect/config.h"
#include "libcbdetect/find_corners.h"
#include "libcbdetect/plot_boards.h"
#include "libcbdetect/plot_corners.h"
#include <dmtx.h>
#include "ml_circle_detect.h"
#include <Eigen/Dense>
#include <Eigen/Core>
#include "ml_machine_vision.h"
#include <opencv2/imgproc/types_c.h>
using namespace algorithm;
using namespace cv;

MLMachineVision::MLMachineVision()
{
}
MLMachineVision::~MLMachineVision()
{
}
MLMachineVision::MLMachineVision(cv::Mat instrinsic_matrix, cv::Mat distortion_coefficient, cv::Size img_size, double pixel_size, double magnification) :
    MLDistortionCorrection(instrinsic_matrix, distortion_coefficient, img_size), pixel_size_(pixel_size), magnification_(magnification)
{
    m_center_.x = img_size.width / 2;
    m_center_.y = img_size.height / 2;
}
void NaiveRemoveNoise1(cv::Mat& srcImage, double  pArea)
{
    pArea = double(srcImage.rows) / 2048.0 * pArea;
    int i, j;
    int color = 1;
    int nRows = srcImage.rows;
    int nCols = srcImage.cols;

    cv::Mat img, img_edge, labels, centroids, img_color, stats;
    int nccomps = connectedComponentsWithStats(srcImage, labels, stats, centroids);


    for (i = 0; i < nRows; ++i)
    {
        for (j = 0; j < nCols; ++j)
        {
            if (srcImage.at<uchar>(i, j) == 255)
            {
                int label = labels.at<int>(i, j);
                if (stats.at<int>(label, cv::CC_STAT_AREA) < pArea)
                {
                    srcImage.at<uchar>(i, j) = 0;
                }
            }

        }
    }

}
std::vector<cv::Vec3f> MLMachineVision::GetMachineVisionPoints(cv::Mat img, int point_number)
{
    std::vector<cv::Point2f> circle_point;
    cv::Mat img_gray;
    if (img.channels() == 3)//turn gray
        cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    else
        img_gray = img.clone();

    cv::GaussianBlur(img_gray, img_gray, cv::Size(9, 9), 2, 2);
    //cv::medianBlur(img_gray, img_gray, 9);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(10, 10));
    //cv::morphologyEx(img_gray, img_gray, cv::MORPH_ERODE, element);
    //cv::morphologyEx(img_gray, img_gray, cv::MORPH_GRADIENT, element);


    cv::Mat mat_mean, mat_stddev;
    cv::meanStdDev(img_gray, mat_mean, mat_stddev);
    double thresh_level = mat_mean.at<double>(0, 0);

    cv::Mat img_threshold;
    cv::threshold(img_gray, img_threshold, 0, 255, CV_THRESH_OTSU);
    //cv::threshold(img_gray, img_threshold, thresh_level, 255, CV_THRESH_TOZERO);
    NaiveRemoveNoise1(img_threshold, 2000);
    //cv::Canny(img_gray, img_threshold, 20, 60, 3);//canny edge detection
    //NaiveRemoveNoise1(img_threshold, 500);
    std::vector<cv::Vec3f> circles;
    if (point_number == 4)
        cv::HoughCircles(img_gray, circles, cv::HOUGH_GRADIENT, 1.5, 100, 200, 100, 0, 0); //full
    else if (point_number == 3)
        cv::HoughCircles(img_threshold, circles, cv::HOUGH_GRADIENT, 1.5, 100, 60, 20, 130, 150);

    //draw circles
        // 
    std::vector<cv::Point2f> points;
    for (size_t i = 0; i < circles.size(); i++)
    {
        cv::Point2f center(circles[i][0], circles[i][1]);
        int radius = cvRound(circles[i][2]);
        points.push_back(center);
        circle(img, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
        circle(img, center, radius, cv::Scalar(155, 50, 255), 3, 8, 0);
    }

    //cv::Vec3f big_circle = FitCircle(points);
    //cv::Point center2(cvRound(big_circle[0]), cvRound(big_circle[1]));
    //int radius2 = cvRound(big_circle[2]);
    //circle(img_gray, center2, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
    //circle(img_gray, center2, radius2, cv::Scalar(0, 50, 255), 3, 8, 0);

    if (circles.size() == point_number)
    {
        std::sort(circles.begin(), circles.end(), [](cv::Vec3f& a, cv::Vec3f& b) { return a[0] < b[0]; });// change sort by x,from top to bottom
        //std::sort(circles.begin(), circles.begin() + 1, [](cv::Vec3f& a, cv::Vec3f& b) { return a[0] < b[0]; });
        //std::sort(circles.begin() + 2, circles.end(), [](cv::Vec3f& a, cv::Vec3f& b) { return a[0] < b[0]; });

        return circles;
    }
    else
        return circles;
}
std::vector<cv::Vec3f> MLMachineVision::GetMachineVisionPoints_test(cv::Mat img, cv::Mat img1, int point_number)
{
    std::vector<cv::Point2f> circle_point;
    cv::Mat img_gray;
    if (img.channels() == 3)//turn gray 
        cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    else
        img_gray = img.clone();

    cv::Mat img_gray1;
    if (img1.channels() == 3)//turn gray
        cvtColor(img1, img_gray1, cv::COLOR_BGR2GRAY);
    else
        img_gray1 = img1.clone();

    //cv::absdiff(img_gray1, img_gray, img_gray);


    cv::GaussianBlur(img_gray, img_gray, cv::Size(9, 9), 2, 2);
    //cv::medianBlur(img_gray, img_gray, 9);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(10, 10));
    cv::morphologyEx(img_gray, img_gray, cv::MORPH_DILATE, element);
    cv::morphologyEx(img_gray, img_gray, cv::MORPH_GRADIENT, element);
    //cv::morphologyEx(img_gray, img_gray, cv::MORPH_CLOSE, element);

    cv::Mat mat_mean, mat_stddev;
    cv::meanStdDev(img_gray, mat_mean, mat_stddev);
    double thresh_level = mat_mean.at<double>(0, 0);

    cv::Mat img_threshold;
    //cv::threshold(img_gray, img_threshold, 0, 255, CV_THRESH_OTSU);
    cv::threshold(img_gray, img_threshold, thresh_level, 255, CV_THRESH_BINARY);
    //	cv::morphologyEx(img_threshold, img_threshold, cv::MORPH_GRADIENT, element);
    NaiveRemoveNoise1(img_threshold, 1000);

    cv::Canny(img_gray, img_threshold, 20, 60, 3);//canny edge detection
    NaiveRemoveNoise1(img_threshold, 500);


    std::vector<cv::Vec3f> circles;
    if (point_number == 4)
        cv::HoughCircles(img_gray, circles, cv::HOUGH_GRADIENT, 1.5, 100, 200, 100, 0, 0); //full
    else if (point_number == 3)
        cv::HoughCircles(img_threshold, circles, cv::HOUGH_GRADIENT, 1.5, 100, 60, 20, 130, 150);

    //draw circles
        // 
    std::vector<cv::Point2f> points;
    for (size_t i = 0; i < circles.size(); i++)
    {
        cv::Point2f center(circles[i][0], circles[i][1]);
        int radius = cvRound(circles[i][2]);
        points.push_back(center);
        circle(img, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
        circle(img, center, radius, cv::Scalar(155, 50, 255), 3, 8, 0);
    }

    //cv::Vec3f big_circle = FitCircle(points);
    //cv::Point center2(cvRound(big_circle[0]), cvRound(big_circle[1]));
    //int radius2 = cvRound(big_circle[2]);
    //circle(img_gray, center2, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
    //circle(img_gray, center2, radius2, cv::Scalar(0, 50, 255), 3, 8, 0);

    if (circles.size() == point_number)
    {
        std::sort(circles.begin(), circles.end(), [](cv::Vec3f& a, cv::Vec3f& b) { return a[0] < b[0]; });// change sort by x,from top to bottom
        //std::sort(circles.begin(), circles.begin() + 1, [](cv::Vec3f& a, cv::Vec3f& b) { return a[0] < b[0]; });
        //std::sort(circles.begin() + 2, circles.end(), [](cv::Vec3f& a, cv::Vec3f& b) { return a[0] < b[0]; });

        return circles;
    }
    else
        return circles;
}

std::vector<cv::Vec3f> MLMachineVision::GetMachineVisionPoints_DPA(cv::Mat img, int point_number)
{
    std::vector<cv::Point2f> circle_point;
    cv::Mat img_gray;
    if (img.channels() == 3)//turn gray
        cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    else
        img_gray = img.clone();

    cv::GaussianBlur(img_gray, img_gray, cv::Size(9, 9), 2, 2);
    //cv::medianBlur(img_gray, img_gray, 9);

    cv::Mat mat_mean, mat_stddev;
    cv::meanStdDev(img_gray, mat_mean, mat_stddev);
    double thresh_level = mat_mean.at<double>(0, 0);

    cv::Mat img_threshold;
    cv::threshold(img_gray, img_threshold, 0, 255, CV_THRESH_OTSU);
    //cv::threshold(img_gray, img_threshold, thresh_level, 255, CV_THRESH_BINARY);

    //NaiveRemoveNoise1(img_threshold, 500);

    cv::Canny(img_threshold, img_threshold, 50, 150, 3);//canny edge detection
    NaiveRemoveNoise1(img_threshold, 500);


    std::vector<cv::Vec3f> circles;
    if (point_number == 4)
        cv::HoughCircles(img_gray, circles, cv::HOUGH_GRADIENT, 1.5, 100, 200, 100, 0, 0); //full
    else if (point_number == 3)
        cv::HoughCircles(img_threshold, circles, cv::HOUGH_GRADIENT, 1.5, 100, 150, 50, 100, 200);

    //draw circles
        // 
    std::vector<cv::Point2f> points;
    for (size_t i = 0; i < circles.size(); i++)
    {
        cv::Point2f center(circles[i][0], circles[i][1]);
        int radius = cvRound(circles[i][2]);
        points.push_back(center);
        circle(img, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
        circle(img, center, radius, cv::Scalar(155, 50, 255), 3, 8, 0);
    }

    //cv::Vec3f big_circle = FitCircle(points);
    //cv::Point center2(cvRound(big_circle[0]), cvRound(big_circle[1]));
    //int radius2 = cvRound(big_circle[2]);
    //circle(img_gray, center2, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
    //circle(img_gray, center2, radius2, cv::Scalar(0, 50, 255), 3, 8, 0);

    if (circles.size() == point_number)
    {
        std::sort(circles.begin(), circles.end(), [](cv::Vec3f& a, cv::Vec3f& b) { return a[0] < b[0]; });// change sort by x,from top to bottom
        //std::sort(circles.begin(), circles.begin() + 1, [](cv::Vec3f& a, cv::Vec3f& b) { return a[0] < b[0]; });
        //std::sort(circles.begin() + 2, circles.end(), [](cv::Vec3f& a, cv::Vec3f& b) { return a[0] < b[0]; });

        return circles;
    }
    else
        return circles;
}

std::vector<cv::Vec3f> MLMachineVision::GetDisplayFids_mock(cv::Mat img, int point_number)
{
    std::vector<cv::Vec3f> fids;
    cv::Vec3f a{ 1,2,3 };
    fids.push_back(a);
    return fids;
}

std::vector<cv::Point2f> MLMachineVision::GetGlobaPoints(std::vector<cv::Vec3f>& camera_points, cv::Point2f initial_point)
{

    std::vector<cv::Point2f> global_points;
    cv::Point2f temp;
    cv::Point2f temp1;
    for (int i = 0; i < camera_points.size(); i++)
    {
        //global = (img-center)*pixel size/magnification+initial

        cv::Point2f cameraOffset = cv::Point2f(camera_points[i][0], camera_points[i][1]) - m_center_;

        // Convert image coords to global coords
        double x = cameraOffset.x;
        double y = cameraOffset.y;
        cameraOffset.x = x;
        cameraOffset.y = -y;
        //cameraOffset.y = x;

        temp1 = cameraOffset * pixel_size_ / magnification_;
        temp = temp1 + initial_point;
        global_points.push_back(temp);
    }
    return global_points;
}

std::vector<cv::Point2f> MLMachineVision::GetGlobaPoints(std::vector<cv::Point2f>& camera_points, cv::Point2f initial_point)
{

    std::vector<cv::Point2f> global_points;
    cv::Point2f temp;
    cv::Point2f temp1;
    for (int i = 0; i < camera_points.size(); i++)
    {
        //global = (img-center)*pixel size/magnification+initial

        cv::Point2f cameraOffset = cv::Point2f(camera_points[i].x, camera_points[i].y) - m_center_;

        // Convert image coords to global coords
        double x = cameraOffset.x;
        double y = cameraOffset.y;
        cameraOffset.x = x;
        cameraOffset.y = -y;
        //cameraOffset.y = x;

        temp1 = cameraOffset * pixel_size_ / magnification_;
        temp = temp1 + initial_point;
        global_points.push_back(temp);
    }
    return global_points;
}

cv::Vec3f MLMachineVision::FitCircle(std::vector<cv::Point2f>& points)
{
    std::vector<float> xv, yv, xyv;
    cv::Mat coef;
    cv::Vec3f circle;
    if (points.size() >= 3)
    {
        cv::Mat one = cv::Mat::ones(points.size(), 1, CV_32F);
        for (int i = 0; i < points.size(); i++)
        {
            xv.push_back(points[i].x);
            yv.push_back(points[i].y);
            xyv.push_back(-(pow(points[i].x, 2) + pow(points[i].y, 2)));
        }
        // ax+by+c = -x^2-y^2
        cv::Mat x = cv::Mat(xv, true);
        cv::Mat y = cv::Mat(yv, true);
        cv::Mat xy = cv::Mat(xyv, true);
        cv::Mat T;
        hconcat(x, y, T);
        hconcat(T, one, T);
        coef = (T.t() * T).inv() * T.t() * xy;
        circle[0] = -coef.at<float>(0, 0) / 2;
        circle[1] = -coef.at<float>(1, 0) / 2;
        circle[2] = sqrt(pow(coef.at<float>(0, 0), 2) + pow(coef.at<float>(1, 0), 2) - 4 * coef.at<float>(2, 0)) / 2;
    }
    return circle;
}

cv::Point2f MLMachineVision::GetMiddle(std::vector<cv::Point2f>& points)
{

    return (points[0] + points[2]) / 2;
}
std::vector<cv::Point2d> MLMachineVision::detect_display_fiducial(cv::Mat img, cv::Rect* roisPtr, int roiNums) {
    std::vector<cv::Point2d> result;
    int errCode = 0;
    cbdetect::Corner corners;
    cbdetect::Params params;
    params.corner_type = cbdetect::SaddlePoint;
    params.show_debug_image = false;
    params.find_corners_reiszed = false;
    if (roisPtr == nullptr)
    {
        try
        {
            cbdetect::find_corners(img, corners, params);
            result = corners.p;
        }
        catch (...)
        {
            return result;
        }
    }
    else
    {
        for (int i = 0; i < roiNums; i++)
        {
            cv::Rect rect = *roisPtr;
            cv::Mat roiimg = img(rect);
            try
            {
                if (roiimg.data)
                {
                    cbdetect::find_corners(roiimg, corners, params);
                    for (int j = 0; j < corners.p.size(); j++)
                    {
                        cv::Point2f pt;
                        pt.x = corners.p.at(j).x + rect.x;
                        pt.y = corners.p.at(j).y + rect.y;
                        result.push_back(pt);
                    }
                }
            }
            catch (...)
            {
                return result;
            }
            roisPtr++;
        }
    }
    return result;
}
std::string MLMachineVision::decodeMatrixRegion(cv::Mat img, int startx, int starty, int width, int height) {
    cv::Mat roiimg;
    std::string result = "";
    cv::Mat dst8bit;

    dst8bit = img.clone();
    if (img.depth() == CV_16U)
    {
        cv::Mat outtmp;// = Mat(roi.cols, roi.rows, CV_8UC1);

        double minv = 0.0, maxv = 0.0;
        double* minp = &minv;
        double* maxp = &maxv;
        minMaxIdx(dst8bit, minp, maxp);

        if (maxv > 4095.1)
        {
            cv::Mat tmp = dst8bit / 65535 * 255;

            tmp.convertTo(img, CV_8UC1);
        }
        else
        {
            cv::Mat tmp = dst8bit / 4095.0 * 255;

            tmp.convertTo(img, CV_8UC1);
        }

    }
    if (width == -1 || height == -1)
    {
        roiimg = img;
    }
    else
    {
        cv::Rect rect(startx, starty, width, height);
        roiimg = img(rect).clone();
    }
    DmtxImage* decodeimg;
    DmtxDecode* dec;
    DmtxRegion* reg;
    DmtxMessage* msg;
    DmtxPackOrder type = DmtxPack8bppK;
    switch (roiimg.type())
    {
    case CV_8UC1:
        type = DmtxPack8bppK;
        break;
    case CV_8UC3:
        type = DmtxPack24bppBGR;
        break;
    }

    decodeimg = dmtxImageCreate(roiimg.data, roiimg.cols, roiimg.rows, type);

    dec = dmtxDecodeCreate(decodeimg, 1);
    if (dec == NULL)
    {
        return "";
    }

    reg = dmtxRegionFindNext(dec, NULL);
    if (reg != NULL)
    {
        msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);
        if (msg != NULL)
        {
            result = ((char*)msg->output);
            dmtxMessageDestroy(&msg);
        }
        dmtxRegionDestroy(&reg);
    }
    dmtxDecodeDestroy(&dec);
    dmtxImageDestroy(&decodeimg);
    return result;
}
std::vector<cv::Vec3f> MLMachineVision::getMdpaPoints(cv::Mat img, int startx, int starty, int width, int height, bool isRight, int radius) {
    Ml_Circle_detect circleDetect;
    return circleDetect.detectCircle(img, startx, starty, width, height, isRight, radius);
}

// AA
vector<double>  MLMachineVision::Displacement(cv::Mat DPA_img, cv::Rect roiDPA, cv::Mat WG_img, cv::Rect roiWG)
{
    vector<cv::Vec3f> DPA_location, WG_location;
    int point_number = 3;
    cv::Point2f initial_point = { 0,0 };


    // add the roi regien
    int lenx, leny;
    lenx = DPA_img.rows;
    leny = DPA_img.cols;

    cv::Mat DPA_roi = DPA_img(roiDPA); //DPA_img(cv::Rect(2100, 1200, 1900, 1700));
    cv::Mat WG_roi = WG_img(roiWG);//WG_img(cv::Rect(2100, 1200, 1900, 1700));
    cv::Vec3f start_DPA(roiDPA.x, roiDPA.y, 0);
    cv::Vec3f start_WG(roiWG.x, roiWG.y, 0);

    DPA_location = MLMachineVision::GetMachineVisionPoints_DPA(DPA_roi, point_number);
    WG_location = MLMachineVision::GetMachineVisionPoints(WG_roi, point_number);


    // add the ROI start loaction
    for (int i = 0; i < DPA_location.size(); i++)
    {
        DPA_location[i] = DPA_location[i] + start_DPA;
    }
    for (int i = 0; i < WG_location.size(); i++)
    {
        WG_location[i] = WG_location[i] + start_WG;
    }

    vector<double> dis;
    vector<cv::Point2f>points_DPA, points_WG;
    points_DPA = MLMachineVision::GetGlobaPoints(DPA_location, initial_point);
    points_WG = MLMachineVision::GetGlobaPoints(WG_location, initial_point);

    // add the WG size == 1
    //if (points_WG.size() == 1)
    //{
    //	cv::Point2f temp = points_WG[0];
    //	cv::Point2f top, bottom;
    //	top.x = temp.x + 5.8 * 1000;
    //	top.y = temp.y + 2.1 * 1000;
    //	bottom.x = temp.x + 0.3 * 1000;
    //	bottom.y = temp.y + 3.15 * 1000;

    //	points_WG.clear();
    //	points_WG.push_back(top);
    //	points_WG.push_back(temp);
    //	points_WG.push_back(bottom);
    //}

    if (points_DPA.size() == 3 && points_WG.size() == 3)
    {

        cv::Vec3f circle_DPA = MLMachineVision::FitCircle(points_DPA);
        cv::Vec3f circle_WG = MLMachineVision::FitCircle(points_WG);

        // the  center gravity of the triangle
        //double deltx = double(circle_DPA[0]) - double(circle_WG[0]);
        //double deltz = double(circle_DPA[1]) - double(circle_WG[1]);

        // the middle point of D1 and D2
        cv::Point2f middle_dpa = (points_DPA[1] - points_DPA[0]) / 2.0 + points_DPA[0];
        cv::Point2f middle_wg = (points_WG[1] - points_WG[0]) / 2.0 + points_WG[0];
        double deltx = middle_dpa.x - middle_wg.x; //(circle_DPA[0]) - (circle_WG[0]);
        double deltz = middle_dpa.y - middle_wg.y;//double(circle_DPA[1]) - double(circle_WG[1]);

        dis.push_back(deltx);
        dis.push_back(deltz);


        // Angle to be corrected
        Eigen::Vector2f a, b;
        vector<double>theta;
        for (int i = 0; i < point_number; i++)
        {
            a(0) = points_DPA[i].x - circle_DPA[0];
            a(1) = points_DPA[i].y - circle_DPA[1];
            b(0) = points_WG[i].x - circle_WG[0];
            b(1) = points_WG[i].y - circle_WG[1];
            double costheta1 = a.dot(b) / (a.norm() * b.norm()); // Angle cos value
            double theta1 = acos(costheta1) * 180 / M_PI;     //degree
            theta.push_back(theta1);

        }
        double mean_theta = 0;
        double sumTheata = accumulate(begin(theta), end(theta), 0.0);
        mean_theta = *min_element(theta.begin(), theta.end());//sumTheata / theta.size();
        dis.push_back(mean_theta);
    }

    return dis;

}
map<string, vector<double>> MLMachineVision::ThreePointDistance(cv::Mat DPA_img, cv::Rect roiDPA, cv::Mat WG_img, cv::Rect roiWG, vector<vector<cv::Vec3f>>& Location)
{
    int point_number = 3;
    vector<cv::Vec3f> DPA_location, WG_location;

    // add the roi regien
    int lenx, leny;
    lenx = DPA_img.rows;
    leny = DPA_img.cols;

    cv::Mat DPA_roi = DPA_img(roiDPA);//DPA_img(cv::Rect(2100, 1200, 1900, 1700));
    cv::Mat WG_roi = WG_img(roiWG);//WG_img(cv::Rect(2100, 1200, 1900, 1700));
    cv::Vec3f start_DPA(roiDPA.x, roiDPA.y, 0);
    cv::Vec3f start_WG(roiWG.x, roiWG.y, 0);

    DPA_location = MLMachineVision::GetMachineVisionPoints_DPA(DPA_roi, point_number);
    WG_location = MLMachineVision::GetMachineVisionPoints(WG_roi, point_number);

    for (int i = 0; i < DPA_location.size(); i++)
    {
        DPA_location[i] = DPA_location[i] + start_DPA;
    }
    for (int i = 0; i < WG_location.size(); i++)
    {
        WG_location[i] = WG_location[i] + start_WG;
    }

    Location.push_back(DPA_location);
    Location.push_back(WG_location);

    vector<cv::Point2f>points_DPA, points_WG;
    cv::Point2f initial_point = { 0,0 };
    points_DPA = MLMachineVision::GetGlobaPoints(DPA_location, initial_point);
    points_WG = MLMachineVision::GetGlobaPoints(WG_location, initial_point);

    // add the WG size == 1
    /*	if (points_WG.size() == 1)
        {
            cv::Point2f temp = points_WG[0];
            cv::Point2f top, bottom;
            top.x = temp.x + 5.8 * 1000;
            top.y = temp.y + 2.1 * 1000;
            bottom.x = temp.x + 0.3 * 1000;
            bottom.y = temp.y + 3.15 * 1000;


            points_WG.clear();
            points_WG.push_back(top);
            points_WG.push_back(temp);
            points_WG.push_back(bottom);
        }*/

        // sort by the x, from the top to the bottom
        //std::sort(points_DPA.begin(), points_DPA.end(), [](cv::Vec2f& a, cv::Vec2f& b) { return a[0] < b[0]; });
        //std::sort(points_WG.begin(), points_WG.end(), [](cv::Vec2f& a, cv::Vec2f& b) { return a[0] < b[0]; });
    map < string, vector<double>> xzdy;

    if (points_DPA.size() == 3 && points_WG.size() == 3)
    {
        vector<double> d1, d2, d3;
        d1.push_back((double(points_DPA[0].x) - double(points_WG[0].x))); d1.push_back((double(points_DPA[0].y) - double(points_WG[0].y)));
        d2.push_back((double(points_DPA[1].x) - double(points_WG[1].x))); d2.push_back((double(points_DPA[1].y) - double(points_WG[1].y)));
        d3.push_back((double(points_DPA[2].x) - double(points_WG[2].x))); d3.push_back((double(points_DPA[2].y) - double(points_WG[2].y)));

        xzdy["B"] = d1;
        xzdy["R"] = d2;
        xzdy["G"] = d3;
    }

    return xzdy;

}

//eyebox scanning
int findElement(vector<double>a, double b)
{
    int j = -1;
    for (int i = 0; i < a.size(); i++)
    {
        if (abs(a[i] - b) < 1e-6)
            j = i;

    }
    return j;
}
//IQT
map<string, vector<double>>MLMachineVision::FiducialPointstLocation(cv::Mat fiducial_img, vector<cv::Rect>RoiFiducial, int RoiFlag, vector<cv::Vec3f>& fiducial_location)
{
    map<string, vector<double>>fiducial;
    //	vector<cv::Vec3f>fiducial_location;

    RoiFlag = RoiFlag + 1;
    if (RoiFlag == 1 || RoiFlag == 3)// fiducial is the circle 
    {
        cv::Mat RoiFiducial_img = fiducial_img(RoiFiducial[RoiFlag - 1]);
        cv::Point2f f1 = MLMachineVision::CircleFiducialDetect(RoiFiducial_img);
        cv::Vec3f start_fiducial(RoiFiducial[RoiFlag - 1].x, RoiFiducial[RoiFlag - 1].y, 0);
        cv::Vec3f f = cv::Vec3f(f1.x, f1.y, 0);
        fiducial_location.push_back(f + start_fiducial);
    }
    else if (RoiFlag == 2 || RoiFlag == 0)// fiducal is the line intersection
    {
        double test = 1;
        cv::Mat RoiFiducial_img = fiducial_img(RoiFiducial[RoiFlag - 1]);
        cv::Point2f f1 = MLMachineVision::LineFiducialDetect(RoiFiducial_img);

        cv::Vec3f start_fiducial(RoiFiducial[RoiFlag - 1].x, RoiFiducial[RoiFlag - 1].y, 0);

        cv::Vec3f f = cv::Vec3f(f1.x, f1.y, 0);
        fiducial_location.push_back(f + start_fiducial);
    }
    vector<cv::Point2f>fiducial_global;
    cv::Point2f initial_point = { 0,0 };

    if (fiducial_location.size() >= 1)
    {
        double sum = 0;
        sum = fiducial_location[0][0] + fiducial_location[0][1];

        if (sum != 0)
        {
            fiducial_global = MLMachineVision::GetGlobaPoints(fiducial_location, initial_point);
            vector<double>temp1;
            temp1.push_back(-fiducial_global[0].x);
            temp1.push_back(fiducial_global[0].y);
            fiducial["Fiducai1"] = temp1;
        }
    }
    return fiducial;
}

map<string, std::vector<double>> algorithm::MLMachineVision::FiducialPointstLocation(vector<cv::Point2f>fiducial)
{
    map<string, vector<double>>fiducial_location;

    vector<cv::Point2f>fiducial_global;
    cv::Point2f initial_point = { 0,0 };
    fiducial_global = MLMachineVision::GetGlobaPoints(fiducial, initial_point);

    if (fiducial.size() >= 1)
    {
        double sum = 0;
        sum = fiducial[0].x + fiducial[0].y;
        if (sum != 0)
        {
            vector<double>temp1;
            temp1.push_back(-fiducial_global[0].x);
            temp1.push_back(fiducial_global[0].y);
            fiducial_location["Fiducai1"] = temp1;
        }
    }
    return fiducial_location;
}
void GetAllMinLoc(Mat image, Mat templ, double sim, Scalar mask, vector<Point>* all_min_loc)
{
    bool test = true;
    int i = 0;
    int src_Width = image.cols, src_Height = image.rows;
    int templ_Width = templ.cols, templ_Height = templ.rows;
    double min_value, max_value;
    Point min_Loc, max_Loc;
    Mat img_result;
    Mat img_clon = image.clone();
    if (templ_Width > src_Width || templ_Height > src_Height)printf("The template size is larger than the original image, please choose the correct template\n");
    for (int i = 0; i < 1; i++)
    {
        matchTemplate(img_clon, templ, img_result, TM_CCOEFF_NORMED);
        minMaxLoc(img_result, &min_value, &max_value, &min_Loc, &max_Loc);
        //cout << max_value << endl;
        if (max_value > sim)// The test results are within the expected range
        {
            all_min_loc->push_back(max_Loc);
            rectangle(img_clon, Rect(max_Loc.x, max_Loc.y, templ_Width, templ_Height), mask, -1);// Mask the first region detected
        }
        else break;
    }

}
vector<Point2f> circleCenter(Mat img, float& r, bool isHoughCircles)
{
    Mat img1 = img.clone();
    Mat img_gray;
    if (img.channels() == 3)// Convert to grayscale
        cvtColor(img, img_gray, CV_BGR2GRAY);
    else
        img_gray = img.clone();
    vector<cv::Point2f>center;
    if (isHoughCircles)
    {
        cv::Mat img_draw = img.clone();
        cv::Mat cal_img = img.clone();
        GaussianBlur(cal_img, cal_img, Size(5, 5), 1, 1);
        vector<Vec3f>circles;
        HoughCircles(cal_img, circles, HOUGH_GRADIENT, 2, 200, 70, 30, 90, 120);

        if (circles.size() == 1)
        {
            r = circles[0][2];
            center.emplace_back(circles[0][0], circles[0][1]);
            circle(img_draw, center[0], r, cv::Scalar(0, 0, 255), 1);

        }
        else if (circles.size() > 1)
        {
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

            r = circles[index][2];
            center.emplace_back(circles[index][0], circles[index][1]);
            circle(img_draw, center[0], r, cv::Scalar(0, 0, 255), 1);
        }
    }
    else
    {
        cv::Mat img_draw = img.clone();
        Mat img_th;
        Point minp, maxp;
        double minv = 0.0, maxv = 0.0;
        minMaxLoc(img_gray, &minv, &maxv, &minp, &maxp);

        GaussianBlur(img_gray, img_gray, Size(3, 3), 0, 0);
        equalizeHist(img_gray, img_gray);
        Mat element1 = getStructuringElement(MORPH_RECT, Size(20, 20));
        //dilate(img_gray, img_gray, element1);// Swell, let the black lumps separate
        //cv::morphologyEx(img_gray, img_gray, MORPH_CLOSE, element1);
        cv::morphologyEx(img_gray, img_gray, MORPH_GRADIENT, element1);
        threshold(img_gray, img_th, 0, 255, CV_THRESH_OTSU);
        dilate(img_th, img_th, element1);//Swell, let the black lumps separate

        //threshold(img_gray, img_th, (minv + maxv) / 2, 255, CV_THRESH_BINARY);
        //erode(img_th, img_th, element1);// Swell, let the black lumps separate
        //cv::morphologyEx(img_th, img_th, MORPH_OPEN, element1);
        Mat img_blur;
        //GaussianBlur(img_gray, img_blur, Size(9, 9), 4, 4);
        // Use the canny algorithm to find the contour, so the result of findcontours will be better
        Mat canny_out;
        Canny(img_th, canny_out, 20, 60, 3, false);
        // find contours
        vector<vector<Point>> contours;
        vector<Vec4i> hierachy;
        findContours(img_th, contours, hierachy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(-1, -1));
        //findContours(canny_out, contours, hierachy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
        vector<vector<Point>> contours_new;
        vector<Vec4i> hierachy_new;
        for (int i = 0; i < contours.size(); i++)
        {
            if (/*hierachy[i][2] != -1 && hierachy[i][3] != -1 && */contourArea(contours[i], false) > 8000)
            {
                contours_new.push_back(contours[i]);
                hierachy_new.push_back(hierachy[i]);
            }
            //drawContours(img_draw, contours, i, Scalar(rand() % 255, rand() % 255, rand() % 255), 3, 8);
            //cout << contourArea(contours[i], false) << endl;
        }
        //cout << contours_new.size() << endl;
        for (int i = 0; i < contours_new.size(); i++)
        {
            drawContours(img_draw, contours_new, i, Scalar(rand() % 255, rand() % 255, rand() % 255), 1, 8);
            //cout << contourArea(contours_new[i], false) << endl;
            Point2f pc;
            //float r;
            cv::minEnclosingCircle(contours_new[i], pc, r);
            if (r > 80 & r < 130)
            {
                //drawContours(img, contours_new, i, Scalar(rand() % 255, rand() % 255, rand() % 255), 3, 8);
                cv::circle(img_draw, pc, r, Scalar(0, 0, 255), 1);
                cv::circle(img_draw, pc, 2, Scalar(0, 0, 255), 1);
                //cout << pc << r << endl;
                center.push_back(pc);
            }
        }
    }
    return center;

}

cv::Point2f MLMachineVision::CircleFiducialDetect(cv::Mat RoiFiducial_img)
{
    cv::Point2f fiducial_loaction;

    std::vector<cv::Point2f> circle_point;
    cv::Mat img_gray;
    if (RoiFiducial_img.channels() == 3)//turn gray
        cvtColor(RoiFiducial_img, img_gray, cv::COLOR_BGR2GRAY);
    else
        img_gray = RoiFiducial_img.clone();

    // the old code
    /*
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::GaussianBlur(img_gray, img_gray, cv::Size(9, 9), 2, 2);
    cv::Mat img_threshold;
    cv::threshold(img_gray, img_threshold, 0, 255, CV_THRESH_OTSU);
    //ContoursRemoveNoise(img_threshold, 20);
    cv::Canny(img_threshold, img_threshold, 100, 150, 3);//canny edge detection
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(img_threshold, circles, cv::HOUGH_GRADIENT, 1.5, 100, 200, 50, 50, 100);

    // draw circles

    std::vector<cv::Point2f> points;
    for (size_t i = 0; i < circles.size(); i++)
    {
        cv::Point2f center(circles[i][0], circles[i][1]);
        int radius = cvRound(circles[i][2]);
        points.push_back(center);
        circle(RoiFiducial_img, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
        circle(RoiFiducial_img, center, radius, cv::Scalar(155, 50, 255), 3, 8, 0);
    }
    */

    // has ROI, donot need template
    /*
    cv::Mat temp = cv::imread("E:\\project\\IQT image\\fiducial\\temp.tif");
    cv::Mat temp_gray;
    if (temp.channels() == 3)
        cvtColor(temp, temp_gray, cv::COLOR_BGR2GRAY);
    else
        temp_gray = temp.clone();
    vector<cv::Point> P;// Store the coordinates of all detected objects
    GetAllMinLoc(img_gray, temp_gray, 0.5, cv::Scalar(0, 0, 0), &P);
    // Circle the target to be detected according to all the acquired coordinate data
    cv::Point loc = P[0];
    //rectangle(roi_draw, Rect(loc.x, loc.y, temp.cols, temp.rows), Scalar(0, 0, 255), 2, 8);
    cv::Mat temp1 = img_gray(cv::Rect(loc.x, loc.y, temp.cols, temp.rows));

    */
    float r = 0;
    vector<cv::Point2f> center;
    center = circleCenter(img_gray, r, false);
    if (center.size() == 1)
    {
        fiducial_loaction.x = center[0].x;
        fiducial_loaction.y = center[0].y;
    }
    // ADD THE CROSS CENTER DETECT
       /*
       if (circles.size() >= 1)
       {
               int radius = cvRound(circles[0][2]);
               cv::Rect roi = cv::Rect(circles[0][0] - radius + 25, circles[0][1] - radius + 25, radius * 2 - 40, radius * 2 - 40);
               cv::Mat roi_img = RoiFiducial_img(roi);
               cv::Point2f linePoint = { 0,0 };
               linePoint = crosscenterDectect(roi_img);
               fiducial_loaction.x = linePoint.x;
               fiducial_loaction.y = linePoint.y;
       }

       */
    return fiducial_loaction;

}

cv::Point2f MLMachineVision::LineFiducialDetect(cv::Mat RoiFiducial_img)
{
    cv::Point2f fiducial_loaction;

    cv::Mat img_gray;
    if (RoiFiducial_img.channels() == 3)//turn gray
        cvtColor(RoiFiducial_img, img_gray, cv::COLOR_BGR2GRAY);
    else
        img_gray = RoiFiducial_img.clone();

    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(10, 10));
    //cv::morphologyEx(img_gray, img_gray, cv::MORPH_OPEN, element);
    cv::morphologyEx(img_gray, img_gray, cv::MORPH_GRADIENT, element);
    //	cv::GaussianBlur(img_gray, img_gray, cv::Size(9, 9), 2, 2);
//	cv::medianBlur(img_gray, img_gray, 3);
    //cv::morphologyEx(img_gray, img_gray, cv::MORPH_CLOSE, element);

    double thresh_level = 0;
    cv::Mat thresh_roi = img_gray(cv::Rect(0, 0, 100, 100));
    cv::Mat mat_mean, mat_stddev;
    cv::meanStdDev(img_gray, mat_mean, mat_stddev);
    thresh_level = mat_mean.at<double>(0, 0);

    cv::Mat img_threshold;
    cv::threshold(img_gray, img_threshold, thresh_level, 255, CV_THRESH_BINARY);

    //cv::Canny(img_threshold, img_threshold, 50, 150, 3);//canny edge detection

    //ContoursRemoveNoise(img_threshold, 500);
    NaiveRemoveNoise1(img_threshold, 1000);// 

    cv::Canny(img_threshold, img_threshold, 50, 150, 3);//canny edge detection
    NaiveRemoveNoise1(img_threshold, 1000);// 
    cv::Mat m;
    int img_rows_ = img_gray.rows;
    int img_cols_ = img_gray.cols;
    img_threshold.convertTo(m, CV_64FC1);
    cv::Mat m_row(1, img_cols_, CV_64FC1, cv::Scalar(0));
    cv::Mat m_col(img_rows_, 1, CV_64FC1, cv::Scalar(0));
    cv::reduce(m, m_row, 0, cv::REDUCE_SUM);
    cv::reduce(m, m_col, 1, cv::REDUCE_SUM);

    int numR = countNonZero(m_row);
    int numC = countNonZero(m_col);
    int l = (numR < numC ? numR : numC) / 6;

    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(img_threshold, lines, 1, CV_PI / 180.0, l, l, l);
    //cv::HoughLinesP(img_threshold, lines, 1, CV_PI / 180.0, 20, 20, 20);


    for (int i = 0; i < lines.size(); i++)
    {
        line(RoiFiducial_img, cv::Point(lines[i][0], lines[i][1]), cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0, 0, 255));
    }

    algorithm::MLCrossCenter my_cross;
    std::vector<cv::Vec4i> lines1, lines2;
    std::vector<float> ks1, ks2;
    float a1, b1, c1, a2, b2, c2;
    double line_width;
    cv::Point2f center_cross;// = { 0,0 };

    if (!lines.empty())
    {
        my_cross.SeparateLines(lines, lines1, ks1, lines2, ks2);
        if (!lines1.empty() & !lines2.empty())
        {
            int width1, width2;
            width1 = my_cross.CalculateLine(lines1, ks1, a1, b1, c1);
            width2 = my_cross.CalculateLine(lines2, ks2, a2, b2, c2);
            line_width = std::max(width1, width2);
            center_cross = my_cross.CalculateLinesCross(a1, b1, c1, a2, b2, c2);
        }
    }

    circle(RoiFiducial_img, center_cross, 2, cv::Scalar(155, 50, 255));
    //VIEW CIRCLE
    //circle(RoiFiducial_img, center_cross, 1, cv::Scalar(0, 255, 0), -1, 8, 0);
    //fiducial_loaction.x = center_cross.x;
    //fiducial_loaction.y = center_cross.y;
    fiducial_loaction = center_cross;

    return fiducial_loaction;
}

cv::Point2f MLMachineVision::crosscenterDectect(cv::Mat circleRoi)
{
    cv::Point2f fiducial_loaction;

    cv::Mat img_gray;
    if (circleRoi.channels() == 3)//turn gray
        cvtColor(circleRoi, img_gray, cv::COLOR_BGR2GRAY);
    else
        img_gray = circleRoi.clone();

    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(20, 20));
    //	cv::morphologyEx(img_gray, img_gray, cv::MORPH_CLOSE, element);
    //cv::morphologyEx(img_gray, img_gray, cv::MORPH_OPEN, element);
    //cv::morphologyEx(img_gray, img_gray, cv::MORPH_DILATE, element);
    //cv::GaussianBlur(img_gray, img_gray, cv::Size(9, 9), 2, 2);
    cv::medianBlur(img_gray, img_gray, 3);
    cv::Mat img_threshold;
    cv::threshold(img_gray, img_threshold, 0, 255, CV_THRESH_OTSU);

    cv::Canny(img_threshold, img_threshold, 100, 150, 3);//canny edge detection

    //ContoursRemoveNoise(img_threshold, 500);
    //NaiveRemoveNoise1(img_threshold, 600);

    std::vector<cv::Vec4i> lines;
    //cv::HoughLinesP(img_threshold, lines, 1, CV_PI / 180.0, 100, 100, 100);
    cv::HoughLinesP(img_threshold, lines, 1, CV_PI / 180.0, 20, 20, 20);


    for (int i = 0; i < lines.size(); i++)
    {
        line(circleRoi, cv::Point(lines[i][0], lines[i][1]), cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0, 0, 255));
    }
    algorithm::MLCrossCenter my_cross;
    std::vector<cv::Vec4i> lines1, lines2;
    std::vector<float> ks1, ks2;
    float a1, b1, c1, a2, b2, c2;
    double line_width;
    cv::Point2f center_cross;// = { 0,0 };

    if (!lines.empty())
    {
        my_cross.SeparateLines(lines, lines1, ks1, lines2, ks2);
        if (!lines1.empty() & !lines2.empty())
        {
            int width1, width2;
            width1 = my_cross.CalculateLine(lines1, ks1, a1, b1, c1);
            width2 = my_cross.CalculateLine(lines2, ks2, a2, b2, c2);
            line_width = std::max(width1, width2);
            center_cross = my_cross.CalculateLinesCross(a1, b1, c1, a2, b2, c2);
        }

    }

    //VIEW CIRCLE
    //circle(circleRoi, center_cross, 1, cv::Scalar(0, 255, 0), -1, 8, 0);
    //fiducial_loaction.x = center_cross.x;
    //fiducial_loaction.y = center_cross.y;
    fiducial_loaction = center_cross;

    return fiducial_loaction;
}

map<string, vector<double> > MLMachineVision::DPALocation(cv::Mat DPA_img, cv::Rect RoiDPA)
{
    map<string, vector<double>>DPALocation;

    cv::Mat RoiDPA_img = DPA_img(RoiDPA);

    int point_number = 3;
    vector<cv::Vec3f> DPA_location, WG_location;

    DPA_location = MLMachineVision::GetMachineVisionPoints_DPA(RoiDPA_img, point_number);

    cv::Vec3f start_DPA(RoiDPA.x, RoiDPA.y, 0);

    for (int i = 0; i < DPA_location.size(); i++)
    {
        DPA_location[i] = DPA_location[i] + start_DPA;
    }


    cv::Point2f initial_point(0, 0);

    vector < cv::Point2f> DPA_global = MLMachineVision::GetGlobaPoints(DPA_location, initial_point);
    //std::sort(DPA_global.begin(), DPA_global.end(), [](cv::Vec2f& a, cv::Vec2f& b) { return a[0] < b[0]; });


    if (DPA_global.size() == 3)
    {
        vector<double> d1, d2, d3;
        d1.push_back(-DPA_global[0].x); d1.push_back(DPA_global[0].y);
        d2.push_back(-DPA_global[1].x); d2.push_back(DPA_global[1].y);
        d3.push_back(-DPA_global[2].x); d3.push_back(DPA_global[2].y);

        DPALocation["B"] = d1;
        DPALocation["R"] = d2;
        DPALocation["G"] = d3;
    }
    return DPALocation;
}
vector<cv::Point3f > MLMachineVision::EyeboxFiducialOffset(map<string, vector<double> >f1, map<string, vector<double> >f2,
    cv::Point3f m1, cv::Point3f m2,
    cv::Point2f RalativeLocation, vector<cv::Point3f >eyeboxoffset)
{
    vector<cv::Point3f > offset;
    double theta0 = atan(RalativeLocation.y / RalativeLocation.x);
    //f1-f2
    cv::Point2f f1Location, f2Location;

    if (!f1.empty() & !f2.empty() & !eyeboxoffset.empty())
    {
        vector<double>temp1;
        temp1 = f1["Fiducai1"];
        f1Location.x = temp1[0];
        f1Location.y = temp1[1];
        vector<double>temp2;
        temp2 = f2["Fiducai1"];
        f2Location.x = temp2[0];
        f2Location.y = temp2[1];
        cv::Point2f f1f2Location;
        f1f2Location.x = m2.x + f2Location.x - (m1.x + f1Location.x);
        f1f2Location.y = (m2.y + f2Location.y) - (m1.y + f1Location.y);
        if (f1f2Location.y >= 0)
        {
            double thetaf1f2 = abs(atan(f1f2Location.y / f1f2Location.x));

            for (int i = 0; i < eyeboxoffset.size(); i++)
            {
                double R = sqrt(eyeboxoffset[i].x * eyeboxoffset[i].x + eyeboxoffset[i].y * eyeboxoffset[i].y);
                double theta = abs(atan(eyeboxoffset[i].y / eyeboxoffset[i].x));
                double thetafix = theta - theta0;
                double deltx = R * cos(thetafix);
                double delty = R * sin(thetafix);
                double thetaEyebox = thetafix - thetaf1f2;
                cv::Point3f delta;
                delta.x = R * cos(thetaEyebox);
                delta.y = -R * sin(thetaEyebox);
                delta.z = m1.z + eyeboxoffset[i].z;
                offset.push_back(delta);
            }


        }
        else
        {
            double thetaf1f2 = abs(atan(abs(f1f2Location.y / f1f2Location.y)));
            for (int i = 0; i < eyeboxoffset.size(); i++)
            {
                double R = sqrt(eyeboxoffset[i].x * eyeboxoffset[i].x + eyeboxoffset[i].y * eyeboxoffset[i].y);
                double theta = abs(atan(eyeboxoffset[i].y / eyeboxoffset[i].y));
                double thetafix = theta - theta0;
                double deltx = R * cos(thetafix);
                double delty = R * sin(thetafix);
                double thetaEyebox = thetafix + thetaf1f2;
                cv::Point3f delta;
                delta.x = R * cos(thetaEyebox);
                delta.y = R * sin(thetaEyebox);
                delta.z = m1.z + eyeboxoffset[i].z;
                offset.push_back(delta);
            }

        }

    }

    return offset;
}
/*
vector<cv::Point3f> MLMachineVision::EyeboxFiducialOffset(map<string, vector<double>> f1,map<string, vector<double>> f2,
    cv::Point3f m1, cv::Point3f m2,
    cv::Point2f f1Org, cv::Point2f f2Org,
    vector<cv::Point3f> eyeboxoffset)
{
    vector<cv::Point3f>EyeboxMotoLocation;
    vector<cv::Point2f> FiducialMot, FiducialOrg;
    if (!f1.empty() & !f2.empty())
    {
        FiducialOrg.push_back(f1Org);
        FiducialOrg.push_back(f2Org);
        vector<double> temp = f1["Fiducial1"];
        vector<double> temp1 = f1["Fiducial1"];
        if (temp.size() == 2 & temp.size() == 2)
        {
            FiducialMot.push_back(cv::Point2f(temp[0]+m1.x, temp[1]+m1.y));
            FiducialMot.push_back(cv::Point2f(temp1[0]+m2.x, temp1[1]+m2.y));
            EyeboxMotoLocation= MLMachineVision::EyeboxMotoLocation(FiducialMot,FiducialOrg,eyeboxoffset);
        }
        else
            return EyeboxMotoLocation;

    }
    else
        return EyeboxMotoLocation;

    return EyeboxMotoLocation;


    //return vector<cv::Point3f>();
}
*/
std::vector<cv::Point2f> algorithm::MLMachineVision::FiducialMotorLocation(vector<cv::Point2f>FiducalGlobal,
    vector<cv::Point2f>MotorLocation)
{
    vector<cv::Point2f> FiducialOrg;
    if (FiducalGlobal.empty() & !MotorLocation.empty() & FiducalGlobal.size() == MotorLocation.size())
    {
        for (int i = 0; i < FiducalGlobal.size(); i++)
        {
            double x = FiducalGlobal[i].x + MotorLocation[i].x;
            double y = FiducalGlobal[i].y + MotorLocation[i].y;
            FiducialOrg.push_back(cv::Point2f(x, y));
        }

    }
    return FiducialOrg;// std::vector<cv::Point2f>();
}
//! Compute cosine of the angle between two vectors.
double computeCosineOfAngleBetweenVectors1(const Eigen::VectorXd& vector0,
    const Eigen::VectorXd& vector1)
{
    assert(vector0.size() == vector1.size());

    // Get the cosine of the angle by dotting the normalized vectors.
    double dotProductOfNormalizedVectors = vector0.normalized().dot(vector1.normalized());

    // Explicitly define the extreme cases, which can give problems with the acos function.
    if (dotProductOfNormalizedVectors >= 1.0)
    {
        return 1.0;
    }

    else if (dotProductOfNormalizedVectors <= -1.0)
    {
        return -1.0;
    }
    // Determine the actual angle.
    else
    {
        return dotProductOfNormalizedVectors;
    }
}
cv::Point3f algorithm::MLMachineVision::FiducialMotoLocation(cv::Mat img, cv::Rect roiRect, int typeflag, cv::Point3f motoLocation)
{
    cv::Point3f fiducialMoto;
    cv::Point3f fiduMoto;
    cv::Point2f fiduPxl = CalculatePxlLocation(img, roiRect, typeflag);
    if (fiduPxl.x != 0 & fiduPxl.y != 0)
    {
        cv::Point2f fiduGlo = CalculateGlobalLocation(fiduPxl, cv::Point2f(0, 0));
        fiduMoto = CalculateFiducialMotoLocaion(fiduGlo, motoLocation);
        //m_FducialMotoLocation.push_back(fiduMoto);
    }
    else
    {
        fiduMoto = cv::Point3f(0, 0, 0);
        m_FducialMotoLocation.push_back(fiduMoto);

    }

    return fiduMoto;
}
cv::Point3f algorithm::MLMachineVision::FiducialMotoLicationMan(cv::Point2f fiducialPxl, cv::Point3f motoLocation)
{
    cv::Point2f fiducial_global = CalculateGlobalLocation(fiducialPxl, cv::Point2f(0, 0));
    cv::Point3f fiduMoto = CalculateFiducialMotoLocaion(fiducial_global, motoLocation);
    //m_FducialMotoLocation.push_back(fiduMoto);
    return fiduMoto;
}
vector<cv::Point3f> algorithm::MLMachineVision::Measure(vector<cv::Point3f> EyeboxOffset)
{
    vector<cv::Point3f> EyeboxMotoLocation;
    for (int i = 0; i < EyeboxOffset.size(); i++)
    {
        //cv::Point3f temp;
        cv::Point3f temp;
        temp.x = (EyeboxOffset[i].x * cos(m_theta) + EyeboxOffset[i].y * sin(m_theta)) + m_displacement.x;
        temp.y = (EyeboxOffset[i].y * cos(m_theta) - EyeboxOffset[i].y * sin(m_theta)) + m_displacement.y;
        temp.z = 0;
        EyeboxMotoLocation.push_back(temp);
    }

    return 	EyeboxMotoLocation;
}
cv::Point2f algorithm::MLMachineVision::CalculatePxlLocation(cv::Mat img, cv::Rect roiRect, int RoiFlag)
{
    cv::Point2f fiducialPxl;
    cv::Mat img_gray;
    if (img.channels() == 3)
    {
        cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    }
    else
        img_gray = img.clone();
    cv::Mat roi = img_gray(roiRect);
    cv::Point2f pxlroi;
    if (RoiFlag == 1 || RoiFlag == 3)
    {
        pxlroi = MLMachineVision::CircleFiducialDetect(roi);
    }
    else if (RoiFlag == 2 || RoiFlag == 0)
    {
        pxlroi = MLMachineVision::LineFiducialDetect(roi);
    }
    if (pxlroi.x != 0 & pxlroi.y != 0)
    {
        cv::Point2f temp;
        temp.x = pxlroi.x + roiRect.x;
        temp.y = pxlroi.y + roiRect.y;
        fiducialPxl = temp;
    }
    m_pxllocation.push_back(fiducialPxl);
    return fiducialPxl;
}
cv::Point2f algorithm::MLMachineVision::CalculateGlobalLocation(cv::Point2f fiducialPxl, cv::Point2f initial_point)
{

    cv::Point2f global_point;
    cv::Point2f temp;
    cv::Point2f temp1;

    //global = (img-center)*pixel size/magnification+initial
    //for (int i = 0; i < m_pxllocation.size(); i++)
    //{
    cv::Point2f cameraOffset = cv::Point2f(fiducialPxl.x, fiducialPxl.y) - m_center_;

    // Convert image coords to global coords
    double x = cameraOffset.x;
    double y = cameraOffset.y;
    cameraOffset.x = x;
    cameraOffset.y = -y;
    //cameraOffset.y = x;

    temp1 = cameraOffset * pixel_size_ / magnification_ / 1000.0;
    temp = temp1 + initial_point;
    global_point = temp;
    //}
    return global_point;
}
cv::Point3f algorithm::MLMachineVision::CalculateFiducialMotoLocaion(cv::Point2f global_points, cv::Point3f m1)
{
    cv::Point3f fiducialMoto;
    //vector<cv::Point2f> global_points = GetGlobaPoints(m_pxllocation, initial_point);
    //for (int i = 0; i < global_points.size(); i++)
    //{
    cv::Point3f temp;
    temp.x = m1.x + global_points.x;
    temp.y = m1.y + global_points.y;
    temp.z = m1.z;
    fiducialMoto = temp;
    m_FducialMotoLocation.push_back(fiducialMoto);
    //}
    //m_FiducialOrgLocation.push_back(m1);
    return fiducialMoto;
}

double algorithm::MLMachineVision::CalculateRotateAngle(cv::Point2f& displacement)
{
    double angle = 0.0;

    if (m_FiducialOrgLocation.size() == 2 & m_FducialMotoLocation.size() == 2)
    {
        Eigen::Vector2d MotoCoordinate(m_FducialMotoLocation[1].x - m_FducialMotoLocation[0].x, m_FducialMotoLocation[1].y - m_FducialMotoLocation[0].y);
        Eigen::Vector2d FiducialOrgCoordinate(m_FiducialOrgLocation[1].x - m_FiducialOrgLocation[0].x, m_FiducialOrgLocation[1].y - m_FiducialOrgLocation[0].y);
        double RotateAngle = computeCosineOfAngleBetweenVectors1(MotoCoordinate, FiducialOrgCoordinate);
        if (abs(RotateAngle) != 1)
        {
            double deltaYMot = abs(m_FducialMotoLocation[1].y - m_FducialMotoLocation[0].y);
            double deltaYOrg = abs(m_FiducialOrgLocation[1].y - m_FiducialOrgLocation[0].y);
            // check the rotate angle neg or positive
            if (deltaYMot - deltaYOrg < 0)
                angle = -acos(RotateAngle);
            //RotateAngle = -RotateAngle;
            //Eigen::Matrix2f rotateMatrix(cos(RotateAngle), sin(RotateAngle), -sin(RotateAngle), cos(RotateAngle));
            cv::Point2f displacement;
            cv::Point2f FiducialOrgRotate;
            FiducialOrgRotate.x = m_FiducialOrgLocation[0].x * cos(RotateAngle) + m_FiducialOrgLocation[0].y * sin(RotateAngle);
            FiducialOrgRotate.y = m_FiducialOrgLocation[0].y * cos(RotateAngle) - m_FiducialOrgLocation[0].x * sin(RotateAngle);
            displacement.x = m_FducialMotoLocation[0].x - FiducialOrgRotate.x;
            displacement.y = m_FducialMotoLocation[0].y - FiducialOrgRotate.y;
            //angle = RotateAngle;
            m_theta = angle * 180 / M_PI;
            m_displacement = displacement;
        }
    }
    else if (m_FiducialOrgLocation.size() == 1 & m_FducialMotoLocation.size() == 1)
    {
        m_theta = 0;
        displacement.x = m_FducialMotoLocation[0].x - m_FiducialOrgLocation[0].x;
        displacement.y = m_FducialMotoLocation[0].y - m_FiducialOrgLocation[0].y;
        m_displacement = displacement;
    }
    return angle;
}

void algorithm::MLMachineVision::setCenter(cv::Point2f center_)
{
    m_center_ = center_;
}

cv::Point2f algorithm::MLMachineVision::getCenter()
{
    return m_center_;
}

vector<cv::Point2f> algorithm::MLMachineVision::GetPxlLocation()
{
    return m_pxllocation;
}

vector<cv::Point3f> algorithm::MLMachineVision::GetFiducialMotoLocaion()
{
    return m_FducialMotoLocation;
}

vector<cv::Point3f> algorithm::MLMachineVision::GetFiducialOrgLocaion()
{
    return m_FiducialOrgLocation;
}

void algorithm::MLMachineVision::SetFiducialOrgLocation(cv::Point3f fiducialOrg)
{
    m_FiducialOrgLocation.push_back(fiducialOrg);
}

double algorithm::MLMachineVision::GetRotateAngle()
{
    return m_theta;
}

cv::Point2f algorithm::MLMachineVision::GetDisplacement()
{
    return m_displacement;
}

void algorithm::MLMachineVision::ClearLocation()
{
    m_pxllocation.clear();
    m_FiducialOrgLocation.clear();
    m_FducialMotoLocation.clear();
}

algorithm::MachineVisionImageRecognition::MachineVisionImageRecognition()
{
}

algorithm::MachineVisionImageRecognition::~MachineVisionImageRecognition()
{
}
void GetAllMinLoc(Mat image, Mat templ, double sim, Scalar mask, vector<Point>* all_min_loc, int num)
{
    bool test = true;
    int i = 0;
    int src_Width = image.cols, src_Height = image.rows;
    int templ_Width = templ.cols, templ_Height = templ.rows;
    double min_value, max_value;
    Point min_Loc, max_Loc;
    Mat img_result;
    Mat img_clon = image.clone();
    if (templ_Width > src_Width || templ_Height > src_Height)printf("The template size is larger than the original image, please choose the correct template\n");
    for (int i = 0; i < num; i++)
    {
        matchTemplate(img_clon, templ, img_result, TM_CCOEFF_NORMED);
        minMaxLoc(img_result, &min_value, &max_value, &min_Loc, &max_Loc);
        cout << max_value << endl;
        if (max_value > sim)// The test results are within the expected range
        {
            all_min_loc->push_back(max_Loc);
            rectangle(img_clon, Rect(max_Loc.x, max_Loc.y, templ_Width, templ_Height), mask, -1);// Mask the first region detected
        }
        else break;
    }

}

cv::Point2f algorithm::MachineVisionImageRecognition::projectorInputgrationgOffset(cv::Mat img_input, cv::Mat img_projector, cv::Rect roiRect)
{
    cv::Point2f offset = cv::Point2f(0, 0);

    // get the ROI region
    cv::Mat inputRoi, prjRoi;
    if (roiRect.x == 0 & roiRect.y == 0)
        inputRoi = img_input.clone();
    else
        inputRoi = img_input(roiRect);

    prjRoi = img_projector(cv::Rect(1600, 1200, 800, 800));

    // change to gray image
    cv::Mat inputROI_gray, prjROI_gray;
    if (inputRoi.channels() == 3)
        cvtColor(inputRoi, inputROI_gray, cv::COLOR_BGR2GRAY);
    else
        inputROI_gray = inputRoi.clone();

    if (prjRoi.channels() == 3)
        cvtColor(prjRoi, prjROI_gray, cv::COLOR_BGR2GRAY);
    else
        prjROI_gray = prjRoi.clone();

    // get the pixel coordinate
    vector<cv::Point2f> inputPxlCoordinate, prjPxlCoordinate;
    //TODO::
    //inputPxlCoordinate = inputgratingDetec(inputROI_gray);
    cv::Mat temp = cv::imread("./config/IQT Template/inputTemp.tif");
    cv::Mat temp_gray;
    cvtColor(temp, temp_gray, cv::COLOR_BGR2GRAY);
    vector<Point> P;// Store the coordinates of all detected objects
    GetAllMinLoc(inputROI_gray, temp_gray, 0.3, Scalar(0, 0, 0), &P, 1);
    inputPxlCoordinate.push_back(cv::Point2f(P[0].x + 707, P[0].y + 635));
    prjPxlCoordinate = circleDetect(prjROI_gray);
    prjPxlCoordinate[0].x = prjPxlCoordinate[0].x + 1600;
    prjPxlCoordinate[0].y = prjPxlCoordinate[0].y + 1200;

    // get the global coordinate
    vector<cv::Point2f> inputGlobalCoordinate, prjGlobalCoordinate;
    inputGlobalCoordinate = getGlobalpoint(inputPxlCoordinate);
    prjGlobalCoordinate = getGlobalpoint(prjPxlCoordinate);
    if (inputGlobalCoordinate.size() > 0 & prjGlobalCoordinate.size() > 0)
    {
        offset.x = prjGlobalCoordinate[0].x - inputGlobalCoordinate[0].x;
        offset.y = prjGlobalCoordinate[0].y - inputGlobalCoordinate[0].y;
    }
    if (sqrt(offset.x * offset.x + offset.y * offset.y) * 1000 < 50)
    {
        offset.x = 0;
        offset.y = 0;
    }
    return offset;
}

vector<cv::Point2f> algorithm::MachineVisionImageRecognition::circleDetect(cv::Mat img)
{
    vector<cv::Point2f> centers;
    cv::Mat img1 = img.clone(); //img(cv::Rect(1600, 1200, 800, 800));
    cv::Mat img_draw = img1.clone();
    cv::Mat img_gray;
    if (img.channels() == 3)
        cvtColor(img1, img_gray, cv::COLOR_BGR2GRAY);
    else
        img_gray = img1.clone();
    cv::Mat img_th;
    Mat element1 = getStructuringElement(MORPH_RECT, Size(5, 5));
    //cv::morphologyEx(img_gray, img_gray, MORPH_CLOSE, element1);
    threshold(img_gray, img_th, 0, 255, CV_THRESH_OTSU);
    cv::Mat img_canny;
    Canny(img_th, img_canny, 50, 150, 3);
    // find contour
    vector<vector<Point>> contours;
    vector<Vec4i> hierachy;
    findContours(img_th, contours, hierachy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(-1, -1));
    vector<vector<Point>> contours_new;
    vector<Vec4i> hierachy_new;
    for (int i = 0; i < contours.size(); i++)
    {
        if (/*hierachy[i][2] != -1 && hierachy[i][3] != -1 && */contourArea(contours[i], false) > 2000)
        {
            Point2f pc;
            float r;
            cv::minEnclosingCircle(contours[i], pc, r);
            if (r > 10 & r < 250)
            {
                //	drawContours(img_draw, contours, i, Scalar(rand() % 255, rand() % 255, rand() % 255), 3, 8);
                cv::circle(img_draw, pc, r, Scalar(0, 0, 255), 1);
                cv::circle(img_draw, pc, 5, Scalar(0, 0, 255), -1);
                //cout << pc << r << endl;
                centers.push_back(pc);
            }
        }


    }

    return centers;// vector<cv::Point2f>();
}

vector<cv::Point2f> algorithm::MachineVisionImageRecognition::inputgratingDetec(cv::Mat img)
{


    return vector<cv::Point2f>();
}

vector<cv::Point2f> algorithm::MachineVisionImageRecognition::getGlobalpoint(vector<cv::Point2f> pxlCoordinate)
{
    vector<cv::Point2f> global_point;
    cv::Point2f temp;
    cv::Point2f temp1;

    //global = (img-center)*pixel size/magnification+initial
    for (int i = 0; i < pxlCoordinate.size(); i++)
    {
        cv::Point2f cameraOffset = cv::Point2f(pxlCoordinate[i].x, pxlCoordinate[i].y) - m_center_;

        // Convert image coords to global coords
        double x = cameraOffset.x;
        double y = cameraOffset.y;
        cameraOffset.x = x;
        cameraOffset.y = y;
        //cameraOffset.y = x;
        temp1 = cameraOffset * pixel_size_ / magnification_ / 1000.0;
        temp = temp1;
        global_point.push_back(temp);
    }
    return global_point;

}
