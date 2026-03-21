#include "pch.h"
#include "ML_distortionCompensation.h"
#include <Eigen/Dense>
using namespace cv;
using namespace MLIQMetrics;
MLIQMetrics::MLDistortionCompensation::MLDistortionCompensation()
{
}

MLIQMetrics::MLDistortionCompensation::~MLDistortionCompensation()
{
}

Eigen::Matrix3d getTheoreticalKeystoneMatrix(double phi_x_deg, double phi_y_deg, bool phi_x_first = true) {
    double phi_x = phi_x_deg * CV_PI / 180.0;
    double phi_y = phi_y_deg * CV_PI / 180.0;

    Eigen::Matrix3d Mx, My;
    Mx << std::cos(phi_x), 0, -std::sin(phi_x),
        0, 1, 0,
        std::sin(phi_x), 0, std::cos(phi_x);

    My << 1, 0, 0,
        0, std::cos(phi_y), std::sin(phi_y),
        0, -std::sin(phi_y), std::cos(phi_y);

    Eigen::Matrix3d H = phi_x_first ? (My * Mx).inverse() : (Mx * My).inverse();
    return H;
}
void getKeystoneForwardMap(
    const Eigen::Matrix3d& H,
    int width, int height,
    cv::Point2d ref_center,
    double z_norm,
    cv::Mat& map_x,
    cv::Mat& map_y
) {
    map_x.create(height, width, CV_32F);
    map_y.create(height, width, CV_32F);

    for (int y = 0; y < height; ++y) {
        double y0 = y - ref_center.y;
        for (int x = 0; x < width; ++x) {
            double x0 = x - ref_center.x;

            Eigen::Vector3d pt(x0, y0, z_norm);
            Eigen::Vector3d warped = H * pt;
            map_x.at<float>(y, x) = static_cast<float>(ref_center.x + z_norm * warped(0) / warped(2));
            map_y.at<float>(y, x) = static_cast<float>(ref_center.y + z_norm * warped(1) / warped(2));
        }
    }
}
bool MLIQMetrics::MLDistortionCompensation::getDistortionCompensationMap(cv::Size size,cv::Mat &map_x, cv::Mat& map_y, double phi_x, double phi_y)
{
    double pixel = IQMetricsParameters::pixel_size;
    double focal = IQMetricsParameters::FocalLength;
    IQMetricUtl IQMetricUtltmp;
    int binNum = IQMetricUtltmp.getBinNum(size);
   double dpp = atan(pixel/focal)*180.0/CV_PI*binNum;
   double z_norm = 1.0 / std::tan(dpp * CV_PI / 180.0);
   cv::Point2d ref_center(size.width / 2.0, size.height / 2.0);
   Eigen::Matrix3d H = getTheoreticalKeystoneMatrix(phi_x, phi_y,false);
    getKeystoneForwardMap(H, size.width, size.height, ref_center, z_norm, map_x, map_y);
    return true;
}



DistortionCompensationRe MLIQMetrics::MLDistortionCompensation::getDistortionCompensationMapImg(cv::Mat img, cv::Mat map_x, cv::Mat map_y)
{
    DistortionCompensationRe re;
    if (map_x.empty() || map_y.empty())
    {
        re.flag = false;
        re.errMsg = "input distortion compensation map is null";
        return re;
    }
    if (img.size() != map_x.size())
    {
        re.flag = false;
        re.errMsg = "the map size is not equal to the input image";
        return re;
    }
    if(map_y.size() != map_x.size())
    {
        re.flag = false;
        re.errMsg = "the map_x size is not equal to the map_y size";
        return re;
    }
    cv::Mat img_out;
    cv::remap(img, img_out, map_x, map_y, cv::INTER_LINEAR, cv::BORDER_CONSTANT, 0.0);
    re.corrtedImg = img_out;
    return re;
}
