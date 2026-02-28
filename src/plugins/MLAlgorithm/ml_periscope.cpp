#include "ml_periscope.h"

using namespace algorithm;

MLPeriscope::MLPeriscope()
{
}

MLPeriscope::~MLPeriscope()
{
}

float MLPeriscope::DisConverttoAngle(float x)
{
    return atan(x * pixel_size_ / focal_length_) / CV_PI * 180;
}

float MLPeriscope::GetTwoPosDistance(cv::Point2f p1, cv::Point2f p2)
{
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.x - p2.x, 2));
}

void MLPeriscope::SplitImage(int divide)
{
    img_divide_ = divide;
    left_img_ = img_.colRange(0, divide).clone();
    right_img_ = img_.colRange(divide, width_).clone();
}


void MLPeriscope::Combine(cv::Point2f &right)
{
    //left = left;
    right = right + cv::Point2f(img_divide_, 0);
}

void MLPeriscope::LocatePeriscope(cv::Mat img, int crossNumber)
{
    img_ = img.clone();
    width_ = img.cols;
    height_ = img.rows;

    switch (type_)
    {
        case algorithm::LEFT:
        {
            if (crossNumber == 1)
            {
                location_.right = cv::Point2f(0, 0);
                location_.left = left_cross_.GetCrossCenter(img);
            }
            else
            {
                location_.right = cv::Point2f(0, 0);
                location_.left = left_cross_.GetTwoCrossCenter(img);
            }

            break;
        }
        case algorithm::RIGHT:
        {
            if (crossNumber == 1)
            {
                location_.left = cv::Point2f(0, 0);
                location_.right = right_cross_.GetCrossCenter(img);
            }
            else
            {
                location_.left = cv::Point2f(0, 0);
                location_.right = right_cross_.GetTwoCrossCenter(img);
            }
            
            break;
        }
        case algorithm::BOTH:
        {
            SplitImage(width_ / 2);
            location_.left = left_cross_.GetCrossCenter(left_img_);
            location_.right = right_cross_.GetCrossCenter(right_img_);
            Combine(location_.right);
            break;
        }
    }


}

PeriscopeInfo MLPeriscope::GetPeriscopeInfo()
{

    return location_;
}

std::vector<cv::Point2f> MLPeriscope::GetPeriscopeROIs(int off_set)
{
    std::vector<cv::Point2f> left_rois, right_rois;
    switch (type_)
    {
    case algorithm::LEFT:
    {
        left_rois = left_cross_.GetTwoCrossROICenter(off_set);
        break;
    }
    case algorithm::RIGHT:
    {
        right_rois = right_cross_.GetTwoCrossROICenter(off_set);

        break;
    }
    case algorithm::BOTH:
    {
        left_rois = left_cross_.GetROICenter(off_set);
        right_rois = right_cross_.GetROICenter(off_set);
        for (int i = 0; i < right_rois.size(); i++)
        {
            Combine(right_rois[i]);
        }
        break;
    }
    }
    

    left_rois.insert(left_rois.end(), right_rois.begin(), right_rois.end());
    return left_rois;
}

void MLPeriscope::CalculatePeriscopeError()
{

}

PeriscopeError MLPeriscope::GetPeriscopeError()
{
    if (type_ != RIGHT)
    {
        periscope_error_.delta_left = location_.left - reference_.left;
        periscope_error_.angle_leftXY = cv::Point2f(DisConverttoAngle(periscope_error_.delta_left.x), DisConverttoAngle(periscope_error_.delta_left.y));
        periscope_error_.angle_left = DisConverttoAngle(GetTwoPosDistance(location_.left, reference_.left));
    }
    if (type_ != LEFT)
    {
        periscope_error_.delta_right = location_.right - reference_.right;
        periscope_error_.angle_rightXY = cv::Point2f(DisConverttoAngle(periscope_error_.delta_right.x), DisConverttoAngle(periscope_error_.delta_right.y));
        periscope_error_.angle_right = DisConverttoAngle(GetTwoPosDistance(location_.right, reference_.right));
    }
    if (type_ == BOTH)
        periscope_error_.angle_left_to_right = DisConverttoAngle(GetTwoPosDistance(location_.left, location_.right));

    return periscope_error_;
}