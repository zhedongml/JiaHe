/*
 *      Author: alexanderb
 */

#include "harris.h"
#include <qdatetime.h>
#include <QDEBUG>
#define M_PI  3.1415926
Harris::Harris() {

}
Harris::Harris(Mat img, float k, int filterRange, bool gauss) {
    // (1) Convert to greyscalescale image
    Mat greyscaleImg = convertRgbToGrayscale(img);
}
std::vector<Mat> Harris::create_Haar_wavelet_xy(float sigma) {
    std::vector<Mat> result;
    int M = 4 * sigma;
    if (M % 2 == 0) {
        M += 2;
    }
    else {
        M += 1;
    }
    Mat filter_x(M, M, CV_32F, 1);
    filter_x(Range::all(), Range(0, M / 2)) = -1;
    Mat filter_y(M, M, CV_32F, 1);
    filter_y(Range(M / 2, M), Range::all()) = -1;
    result.push_back(filter_x);
    result.push_back(filter_y);
    return result;
}
std::vector<Mat> Harris::compute_derivatives(Mat img, Mat filter_x, Mat filter_y) {
    std::vector<Mat> result;
    Mat Ix, Iy;
    Mat flipx, flipy;
    flip(filter_x, flipx, -1);
    flip(filter_y, flipy, -1);
    filter2D(img, Ix, CV_64F, flipx, Point(-1, -1), 0.0, BORDER_CONSTANT);
    filter2D(img, Iy, CV_64F, flipy, Point(-1, -1), 0.0, BORDER_CONSTANT);
    result.push_back(Ix);
    result.push_back(Iy);
    return result;
}
std::vector<cv::Point2d> Harris::detect_corners(Mat img, int nms_kernel_size, float sigma, float k, float thresh, float max_no_corners) {
    std::vector<cv::Point2d> corners;
    // Harris Corner response
    Mat corner_response = create_corner_response_matrix(img, sigma, k, thresh);

    // Non Maximal Suppression
    Mat nms_corner_resp = local_non_maximal_suppression(corner_response, nms_kernel_size);

    // Retain "max_no_corners"
    corners = get_max_corners(nms_corner_resp, max_no_corners);
    return corners;
}

Mat Harris::create_corner_response_matrix(Mat img, float sigma, float k, float thresh) {
    std::vector<Mat> matVecs = create_Haar_wavelet_xy(sigma);
    std::vector<Mat> derVecs = compute_derivatives(img, matVecs[0], matVecs[1]);
    int win_size = int(5 * sigma);
    if (win_size % 2 == 0)
        win_size += 1;
    else
        win_size += 2;
    Mat win = Mat::ones(win_size, win_size, CV_64F);
    Mat IxPow, IyPow, IxMulIy;
    Mat Ix = derVecs[0];
    Mat Iy = derVecs[1];
    cv::pow(Ix, 2, IxPow);
    cv::pow(Iy, 2, IyPow);
    cv::multiply(Ix, Iy, IxMulIy);
    Mat Ix_2_win, Iy_2_win, Ix_Iy_win;
    filter2D(IxPow, Ix_2_win, CV_64F, win, Point(-1, -1), 0.0, BORDER_CONSTANT);
    filter2D(IyPow, Iy_2_win, CV_64F, win, Point(-1, -1), 0.0, BORDER_CONSTANT);
    filter2D(IxMulIy, Ix_Iy_win, CV_64F, win, Point(-1, -1), 0.0, BORDER_CONSTANT);
    normalize(Ix_2_win, Ix_2_win, 1.0, 0.0, cv::NORM_INF);
    normalize(Iy_2_win, Iy_2_win, 1.0, 0.0, cv::NORM_INF);
    normalize(Ix_Iy_win, Ix_Iy_win, 1.0, 0.0, cv::NORM_INF);

    Mat det_corner, trace_corner, corner_resp;
    Mat temp1, temp2, temp3;
    cv::multiply(Ix_2_win, Iy_2_win, temp1);
    cv::pow(Ix_Iy_win, 2, temp2);
    det_corner = temp1 - temp2;
    trace_corner = Ix_2_win + Iy_2_win;
    cv::pow(trace_corner, 2, temp3);
    corner_resp = det_corner - k * temp3;
    float avg = 0, sum = 0;
    int nozeroNum = 0;
    for (int i = 0; i < corner_resp.rows; i++)
        for (int j = 0; j < corner_resp.cols; j++) {
            double val = corner_resp.at<double>(i, j);
            if (val > 0) {
                nozeroNum++;
                sum += val;
            }
        }
    if (nozeroNum > 0)
        avg = sum / nozeroNum;
    float threshold = thresh * avg;
    for (int i = 0; i < corner_resp.rows; i++)
        for (int j = 0; j < corner_resp.cols; j++) {
            float val = corner_resp.at<double>(i, j);
            if (val < threshold) {
                corner_resp.at<double>(i, j) = 0;
            }
        }
    return corner_resp;
}
Mat Harris::local_non_maximal_suppression(Mat corner_resp, int kernel_size) {

    cv::Mat mask;
    cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(25, 25));
    cv::dilate(corner_resp, mask, element1);
    cv::compare(corner_resp, mask, mask, cv::CMP_GE);
    cv::Mat non_plateau_mask;
    cv::erode(corner_resp, non_plateau_mask, element1);
    cv::compare(corner_resp, non_plateau_mask, non_plateau_mask, cv::CMP_GT);
    cv::bitwise_and(mask, non_plateau_mask, mask);
    return mask;
    if (kernel_size % 2 == 0)
        kernel_size = kernel_size + 1;
    int pad = kernel_size / 2;
    copyMakeBorder(corner_resp, corner_resp, pad, pad, pad, pad, BORDER_CONSTANT, 0);
    Mat nms_corner_resp = Mat::zeros(corner_resp.size(), CV_64F);
    int h = corner_resp.rows;
    int w = corner_resp.cols;
    double max_val = 0.0, min_val = 0.0;
    cv::Point minp, maxp;
    for (int i = pad; i < h - pad; i++) {
        for (int j = pad; j < w - pad; j++) {
            Mat temp = corner_resp(Range(i - pad, i + pad + 1), Range(j - pad, j + pad + 1));
            minMaxLoc(temp, &min_val, &max_val, &minp, &maxp);
            if (corner_resp.at< double >(i, j) == max_val)
                nms_corner_resp.at<double>(i, j) = max_val;
        }
    }
    nms_corner_resp = nms_corner_resp(Range(pad, h - pad), Range(pad, w - pad));
    return nms_corner_resp;
}
std::vector<cv::Point2d> Harris::get_max_corners(Mat corner_resp, int max_no_corners) {
    std::vector<cv::Point2d> nonZeroCoordinates;
    findNonZero(corner_resp, nonZeroCoordinates);
    int width = corner_resp.cols;
    int height = corner_resp.rows;
    for (int i = 0; i < nonZeroCoordinates.size(); i++) {
        cv::Point2d pt0 = nonZeroCoordinates.at(i);
        if (pt0.x < 25 || pt0.y < 25 || width - pt0.x < 25 || height - pt0.y < 25) {
            nonZeroCoordinates.erase(nonZeroCoordinates.begin() + i);
            i--;
        }
    }
    return nonZeroCoordinates;
}


//-----------------------------------------------------------------------------------------------
Mat Harris::convertRgbToGrayscale(Mat& img) {
    Mat greyscaleImg(img.rows, img.cols, CV_32F);

    for (int c = 0; c < img.cols; c++) {
        for (int r = 0; r < img.rows; r++) {
            greyscaleImg.at<float>(r, c) =
                0.2126 * img.at<cv::Vec3b>(r, c)[0] +
                0.7152 * img.at<cv::Vec3b>(r, c)[1] +
                0.0722 * img.at<cv::Vec3b>(r, c)[2];
        }
    }

    return greyscaleImg;
}


//-----------------------------------------------------------------------------------------------
Mat Harris::computeIntegralImg(Mat& img) {
    Mat integralMat(img.rows, img.cols, CV_32F);

    integralMat.at<float>(0, 0) = img.at<float>(0, 0);

    for (int i = 1; i < img.cols; i++) {
        integralMat.at<float>(0, i) =
            integralMat.at<float>(0, i - 1)
            + img.at<float>(0, i);
    }

    for (int j = 1; j < img.rows; j++) {
        integralMat.at<float>(j, 0) =
            integralMat.at<float>(j - 1, 0)
            + img.at<float>(j, 0);
    }

    for (int i = 1; i < img.cols; i++) {
        for (int j = 1; j < img.rows; j++) {
            integralMat.at<float>(j, i) =
                img.at<float>(j, i)
                + integralMat.at<float>(j - 1, i)
                + integralMat.at<float>(j, i - 1)
                - integralMat.at<float>(j - 1, i - 1);
        }
    }

    return integralMat;
}

//-----------------------------------------------------------------------------------------------
Mat Harris::meanFilter(Mat& intImg, int range) {
    Mat medianFilteredMat(intImg.rows - range * 2, intImg.cols - range * 2, CV_32F);

    for (int r = range; r < intImg.rows - range; r++) {
        for (int c = range; c < intImg.cols - range; c++) {
            medianFilteredMat.at<float>(r - range, c - range) =
                intImg.at<float>(r + range, c + range)
                + intImg.at<float>(r - range, c - range)
                - intImg.at<float>(r + range, c - range)
                - intImg.at<float>(r - range, c + range);
        }
    }

    return medianFilteredMat;
}

Mat Harris::gaussFilter(Mat& img, int range) {
    // Helper Mats for better time complexity
    Mat gaussHelperV(img.rows - range * 2, img.cols - range * 2, CV_32F);
    for (int r = range; r < img.rows - range; r++) {
        for (int c = range; c < img.cols - range; c++) {
            float res = 0;

            for (int x = -range; x <= range; x++) {
                float m = 1 / sqrt(2 * M_PI) * exp(-0.5 * x * x);

                res += m * img.at<float>(r - range, c - range);
            }

            gaussHelperV.at<float>(r - range, c - range) = res;
        }
    }

    Mat gauss(img.rows - range * 2, img.cols - range * 2, CV_32F);
    for (int r = range; r < img.rows - range; r++) {
        for (int c = range; c < img.cols - range; c++) {
            float res = 0;

            for (int x = -range; x <= range; x++) {
                float m = 1 / sqrt(2 * M_PI) * exp(-0.5 * x * x);

                res += m * gaussHelperV.at<float>(r - range, c - range);
            }

            gauss.at<float>(r - range, c - range) = res;
        }
    }

    return gauss;
}

