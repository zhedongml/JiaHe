#include "ml_circle_detect.h"
vector<int> Ml_Circle_detect::getAllRadius(int minR, int maxR, int step) {
    vector<int> radius;
    int count = (maxR - minR) / step;
    for (int i = 0; i < count; i++) {
        radius.push_back(minR + i * step);
    }
    return radius;
}
cv::Mat Ml_Circle_detect::max_min_value_filter(cv::Mat image, int ksize, int mode) {

    cv::Mat img = image.clone();
    int rows = img.rows;
    int cols = img.cols;
    int padding = ksize / 2;
    for (int i = padding; i < rows - padding; i++)
        for (int j = padding; j < cols - padding; j++) {
            cv::Rect rec(j - padding, i - padding, ksize, ksize);
            cv::Mat roi_img = image(rec);
            double max_val = 0.0, min_val = 0.0;
            cv::Point minp, maxp;
            minMaxLoc(roi_img, &min_val, &max_val, &minp, &maxp);
            if (mode == 1) {
                img.at<char>(i, j) = max_val;
            }
            else if (mode == 2)
            {
                img.at<char>(i, j) = min_val;
            }
        }
    return  img;
}
cv::Mat Ml_Circle_detect::makeAnnulusKernel(int outer_radius, int annulus_width) {
    int arrayDim = outer_radius * 2 + 1;
    cv::Mat Kernel(arrayDim, arrayDim, CV_8UC1, cv::Scalar(0));
    cv::Mat grid1(arrayDim, arrayDim, CV_16SC1, cv::Scalar(0));
    cv::Mat grid2(arrayDim, arrayDim, CV_16SC1, cv::Scalar(0));
    for (int i = 0; i < arrayDim; i++)
        for (int j = 0; j < arrayDim; j++) {
            grid1.at<short>(i, j) = i - outer_radius;
            grid2.at<short>(j, i) = i - outer_radius;
        }
    int innerR = pow(outer_radius - annulus_width, 2);
    int outerR = pow(outer_radius, 2);
    for (int i = 0; i < arrayDim; i++)
        for (int j = 0; j < arrayDim; j++) {
            int r = pow(grid1.at<short>(i, j), 2) + pow(grid2.at<short>(i, j), 2);
            if (r <= outerR && r >= innerR)
                Kernel.at<char>(i, j) = 255.0f;
        }
    return Kernel;
}
CirclePos Ml_Circle_detect::getTopCircle(cv::Mat image,int cradius) {
    double maxima = 0;
    CirclePos pos;
    vector<int> radius = getAllRadius(cradius, cradius+5, 5);
    double max_val = 0.0, min_val = 0.0;
    cv::Point minp, maxp;
    cv::Mat doubleImg;
    image.convertTo(doubleImg, CV_32FC1);
    for (int i = 0; i < radius.size(); i++) {
        int rad = radius.at(i);
        cv::Mat ker = makeAnnulusKernel(rad, _ANNULUS_WIDTH);


        cv::Mat conv_img;// = Convlution(image, ker);
        cv::filter2D(doubleImg, conv_img, -1, ker);
        minMaxLoc(conv_img, &min_val, &max_val, &minp, &maxp);

        if (max_val > maxima) {
            maxima = max_val;
            pos.x = maxp.x;
            pos.y = maxp.y;
            pos.r = rad;
        }
    }
    return pos;
}
vector<cv::Vec3f> Ml_Circle_detect::detectCircle(cv::Mat img, int startx, int starty, int width, int height, bool isRight, int radius) {
    vector<cv::Vec3f> result;
    cv::Rect roiRect(startx, starty, width, height);
    cv::Mat roiImg = img(roiRect);
    if (roiImg.data) {
        cv::Mat filterimg = max_min_value_filter(roiImg, 6);
        if(!isRight)
            cv::Canny(filterimg, filterimg, 50, 255);
        try {
            cv::Mat imgr = img.clone();
            CirclePos pos = getTopCircle(filterimg,radius);
            cv::Vec3f c1(pos.x + startx, pos.y + starty,radius);
            result.push_back(c1);
            
            if (isDebug) {
                cv::Point cc(c1[0], c1[1]);
               // Point cc(pos.x + startx, pos.y + starty);
                cvtColor(imgr, imgr, cv::COLOR_GRAY2BGR);
                circle(imgr, cc, pos.r, cv::Scalar(0, 0, 255), 2, 8, 0);
            }
         
            cv::Rect rec(pos.x - pos.r, pos.y - pos.r, 2 * pos.r, 2 * pos.r);
            cv::Mat roi_img = filterimg(rec);
            roi_img = cv::Scalar(0, 0, 0);
            CirclePos pos2 = getTopCircle(filterimg,radius);
            cv::Vec3f c2(pos2.x+startx, pos2.y+starty, radius);
            result.push_back(c2);
            if (isDebug) {
                //Point cc2(pos2.x + startx, pos2.y + starty);
                cv::Point cc2(c2[0], c2[1]);
                circle(imgr, cc2, pos2.r, cv::Scalar(0, 0, 255), 2, 8, 0);
            }
            cv::Rect rec2(pos2.x - pos2.r, pos2.y - pos2.r, 2 * pos2.r, 2 * pos2.r);
            cv::Mat roi_img2 = filterimg(rec2);
            roi_img2 = cv::Scalar(0, 0, 0);
            CirclePos pos3 = getTopCircle(filterimg,radius);
            cv::Vec3f c3(pos3.x + startx, pos3.y + starty,radius);
            result.push_back(c3);
            if (isDebug) {
                //Point cc3(pos3.x + startx, pos3.y + starty);
                cv::Point cc3(c3[0], c3[1]);
                circle(imgr, cc3, pos3.r, cv::Scalar(0, 0, 255), 2, 8, 0);
            }
            if (result.size() == 3)
            {
                std::sort(result.begin(), result.end(), [](cv::Vec3f& a, cv::Vec3f& b) { return a[1] < b[1]; });
                std::sort(result.begin(), result.begin() + 1, [](cv::Vec3f& a, cv::Vec3f& b) { return a[0] < b[0]; });
                std::sort(result.begin() + 2, result.end(), [](cv::Vec3f& a, cv::Vec3f& b) { return a[0] < b[0]; });
            }
        }
        catch (...) {
            return result;
        }
    }
    return result;
}
