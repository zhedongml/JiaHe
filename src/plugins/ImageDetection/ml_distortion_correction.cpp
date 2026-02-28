#include "ml_distortion_correction.h"

using namespace MLImageDetection;

MLDistortionCorrection::MLDistortionCorrection()
{
}

MLDistortionCorrection::~MLDistortionCorrection()
{
}

void MLDistortionCorrection::setDistortionMat(cv::Mat instrinsic_matrix, cv::Mat distortion_coefficient) {
    instrinsic_matrix_ = instrinsic_matrix;
    distortion_coefficient_ = distortion_coefficient;
}
void MLDistortionCorrection::setImageSize(cv::Size img_size) {
    if (img_size_.height != img_size.height || img_size_.width != img_size.width) {
        img_size_ = img_size;
        initUndistortRectifyMap(instrinsic_matrix_, distortion_coefficient_, cv::Mat(), cv::Mat(), img_size_, CV_16SC2, map1_, map2_);
    }
}
MLDistortionCorrection::MLDistortionCorrection(cv::Mat instrinsic_matrix, cv::Mat distortion_coefficient, cv::Size img_size) :
    instrinsic_matrix_(instrinsic_matrix), distortion_coefficient_(distortion_coefficient) ,img_size_(img_size)
{
    
    initUndistortRectifyMap(instrinsic_matrix_, distortion_coefficient_, cv::Mat(), cv::Mat(), img_size_, CV_16SC2, map1_, map2_);
}


cv::Mat MLDistortionCorrection::GetUndistortedImage(cv::Mat img)
{
    cv::Mat undistorted_img;
    remap(img, undistorted_img, map1_, map2_, cv::INTER_LINEAR);
    return undistorted_img;
}
