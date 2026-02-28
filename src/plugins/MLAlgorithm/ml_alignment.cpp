#include "ml_alignment.h"

using namespace algorithm;
//using namespace cv;
//using namespace Eigen;

MLAlignment::MLAlignment()
{
}

MLAlignment::~MLAlignment()
{
}

MLAlignment::MLAlignment(std::vector<cv::Point3f> reference_points, cv::Point3f controll_point, cv::Mat align_to_move) :
    reference_points_(reference_points - controll_point), controll_point_(controll_point), align_to_move_(align_to_move)
{
    reference_coordinate_ = CalculateCoordinate(reference_points_);
    
}

Eigen::Matrix3d MLAlignment::CalculateCoordinate(std::vector<cv::Point3f>& points)
{
    Eigen::Vector3d x, y, z;
    Eigen::Matrix3d coordinate;

    //unit vector - x
    coordinate.col(0) = CalculateUnitVector(points[1], points[0]);
    //norm vector - y
    coordinate.col(1) = CalculateNormalVector(points);
    //last vector - z
    coordinate.col(2) = coordinate.col(0).cross(coordinate.col(1));

    return coordinate;

}

Eigen::Vector3d MLAlignment::CalculateUnitVector(cv::Point3f& point1, cv::Point3f& point2)
{
    Eigen::Vector3d unit_vector(point2.x - point1.x, point2.y - point1.y, point2.z - point1.z);
    unit_vector = unit_vector / unit_vector.squaredNorm();
    return unit_vector;
}

Eigen::Vector3d MLAlignment::CalculateNormalVector(std::vector<cv::Point3f>& points)
{
    //z = Ax+By+C
    cv::Mat x, y, z, one, T, coef;
    one = cv::Mat::ones(points.size(), 1, CV_32F);
    std::vector<float> xv, yv, zv;
    for (int i = 0; i < points.size(); i++)
    {
        xv.push_back(points[i].x);
        yv.push_back(points[i].y);
        zv.push_back(points[i].z);
    }
    x = cv::Mat(xv, true);
    y = cv::Mat(yv, true);
    z = cv::Mat(zv, true);
    //x = cv::Mat(cv::Vec4f(points[0].x, points[1].x, points[2].x, points[3].x));
    //y = cv::Mat(cv::Vec4f(points[0].y, points[1].y, points[2].y, points[3].y));
    //z = cv::Mat(cv::Vec4f(points[0].z, points[1].z, points[2].z, points[3].z));
    hconcat(x, y, T);
    hconcat(T, one, T);


    coef = (T.t() * T).inv() * T.t() * z;
    cv::Vec3f normal_vector = cv::Vec3f(coef.at<float>(0, 0), coef.at<float>(1, 0), -1);
    normal_vector = normal_vector / cv::norm(normal_vector);
    Eigen::Vector3d norm_tensor;
    norm_tensor << normal_vector(0), normal_vector(1), normal_vector(2);
    return norm_tensor;

}

Eigen::Vector3d MLAlignment::CvPoint2Vector3d(cv::Point3d point)
{
    Eigen::Vector3d tensor(point.x, point.y, point.z);
    return tensor;

}

void MLAlignment::GetAlignmentInfo(std::vector<cv::Point3f> test, Eigen::Vector3d& euler_angle, Eigen::Vector3d& displacement)
{
    test = test - controll_point_;
    Eigen::Matrix3d test_coordinate = CalculateCoordinate(test);
    //std::cout << reference_coordinate_ << std::endl;
    //std::cout << test_coordinate<<std::endl;
    Eigen::Matrix3d rotate_matrix = reference_coordinate_ * test_coordinate.inverse();
    //std::cout << rotate_matrix << std::endl;
    //std::cout << rotate_matrix(3);

    //euler_angle = rotate_matrix.eulerAngles(2, 1, 0);
    euler_angle = ToEulerAngle(rotate_matrix);
    //displace = Reference-R*t
    Eigen::Vector3d d(0,0,0);
    for (int i = 0; i < test.size(); i++)
    {
        //Eigen::Vector3d tmp = CvPoint2Vector3d(reference_points_[i]) - rotate_matrix * CvPoint2Vector3d(test[i]);
        //std::cout << tmp << std::endl;
        d += CvPoint2Vector3d(reference_points_[i]) - rotate_matrix * CvPoint2Vector3d(test[i]);
    }
    displacement = d / test.size();
    //std::cout << displacement << std::endl;
}

Eigen::Vector3d MLAlignment::ToEulerAngle(Eigen::Matrix3d rotate_matrix)
{
    
    double theta_x = atan2(rotate_matrix(5), rotate_matrix(8));
    double theta_y = atan2(-rotate_matrix(2), rotate_matrix(5)/sin(theta_x));
    double theta_z = atan2(rotate_matrix(1), rotate_matrix(0));
    Eigen::Vector3d angle(theta_x, theta_y, theta_z);
    return angle;
}