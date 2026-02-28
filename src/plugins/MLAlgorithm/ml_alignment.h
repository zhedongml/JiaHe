#ifndef MLALGORITHM_MLALIGNMENT_H_
#define MLALGORITHM_MLALIGNMENT_H_

#include <Eigen/Dense>
#include <Eigen/Core>
#include <opencv2/core/eigen.hpp>
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include "ml_machine_vision.h"

#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif

namespace algorithm {

	std::vector<cv::Point3f> operator-(std::vector<cv::Point3f>& points, cv::Point3f& center)
	{
		std::vector<cv::Point3f>result = points;

		for (int i = 0; i < points.size(); i++)
		{
			result[i] = points[i] - center;
		}
		
		return result;
	}
	class ALGORITHM_API MLAlignment
	{
	public:
		MLAlignment();
		~MLAlignment();
		MLAlignment(std::vector<cv::Point3f> reference_points, cv::Point3f controll_point, cv::Mat align_to_move);

		void GetAlignmentInfo(std::vector<cv::Point3f> test, Eigen::Vector3d& euler_angle, Eigen::Vector3d& displacement);
	private:
		std::vector<cv::Point3f> reference_points_;
		Eigen::Matrix3d reference_coordinate_;
		Eigen::Vector3d reference_eulerAngle_;
		cv::Point3f controll_point_;
		cv::Mat align_to_move_;
		cv::Vec3f reference_vector_;

		Eigen::Matrix3d CalculateCoordinate(std::vector<cv::Point3f>& points);
		Eigen::Vector3d CalculateUnitVector(cv::Point3f& point1, cv::Point3f& point2);
		Eigen::Vector3d CalculateNormalVector(std::vector<cv::Point3f>& points);
		Eigen::Vector3d CvPoint2Vector3d(cv::Point3d point);
		Eigen::Vector3d ToEulerAngle(Eigen::Matrix3d rotate_matrix);
	};
}


#endif // !MLALGORITHM_MLMACHINEVISION_H_

