#pragma once
#ifdef MLALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif

#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include<vector>
#include<string>
#include<iostream>
using namespace std;
namespace MLImageDetection
{
	struct RigidTrans2D
	{
		double rotationAngle=0;//radian
		cv::Point2f offset;
		bool flag = true;
		std::string errMsg = "";
	};
	struct RigidTrans3D
	{
		cv::Mat matR;
		Eigen::Matrix3d R;
		Eigen::Vector3d eulerAngle;
		Eigen::Vector3d displacement;
		bool flag = true;
		std::string errMsg = "";
	};
	
	class ALGORITHM_API MLCoordinateTransformation
	{
	public:
		MLCoordinateTransformation();  
		~MLCoordinateTransformation();
	public:
		RigidTrans2D getRigidTrans2D(std::vector<cv::Point2f>srcPts, std::vector<cv::Point2f>dstPts);
		RigidTrans3D getRigidTrans3D(std::vector<cv::Point3f> srcPoints, std::vector<cv::Point3f> dstPoints);
		vector<cv::Point2f>getPointsAfterTrans2D(vector<cv::Point2f>srcPts, RigidTrans2D trans);
	    cv::Point2f getPointAfterTrans2D(cv::Point2f srcPts, RigidTrans2D trans);	
	private:
	};
}

