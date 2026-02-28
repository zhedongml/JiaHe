#include "ml_linefiducialDetect.h"
#include <opencv2/imgproc/types_c.h>
#include <QtDebug>
using namespace algorithm;

algorithm::MLLinesFiducialDetect::MLLinesFiducialDetect()
{
}
algorithm::MLLinesFiducialDetect::~MLLinesFiducialDetect()
{
}
std::vector<cv::Point2f> algorithm::MLLinesFiducialDetect::LineFiducialDetect(cv::Mat img)
{
    std::vector<cv::Point2f> location;
    cv::Point2f center;
    CrossCenter cen;
    center = cen.get_crossCenter(flag);
    location.push_back(center);
    m_img_draw = cen.getImgDraw();
    return location;
}
cv::Mat algorithm::MLLinesFiducialDetect::getImgDraw()
{
    return m_img_draw;
}