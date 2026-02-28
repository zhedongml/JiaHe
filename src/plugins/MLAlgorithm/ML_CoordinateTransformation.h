#pragma once
#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include<vector>
#include"ml_image_public.h"
namespace algorithm
{
	struct FeatureLoc
	{
		cv::Point3f ACSLoc;
		cv::Point3f WGLoc;
		bool flag = true;
		string message = "";
	};
	struct PlaneInfo
	{
		double thetaX=0;
		double thetaY = 0;
		double thetaZ = 0;

        bool isAdd = true;
	};
	class ALGORITHM_API ML_CoordinateTransformation
	{
	public:
		ML_CoordinateTransformation();
		~ML_CoordinateTransformation();
	public:
		void getFeatureACSLoc(FeatureLoc fiducialLoc, PlaneInfo plane, FeatureLoc &eyeboxLoc);
	    void getFeatureACSLoc(FeatureLoc fiducialLoc, PlaneInfo plane, vector<FeatureLoc> &eyeboxLoc);
		cv::Mat calculateRotationMatrix(PlaneInfo plane);
		cv::Point3f calcuatePlaneLoc(cv::Mat R, cv::Point3f wgLoc);				
	};

}