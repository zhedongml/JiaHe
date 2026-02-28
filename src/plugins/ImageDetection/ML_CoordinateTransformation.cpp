#include "ML_CoordinateTransformation.h"

MLImageDetection::ML_CoordinateTransformation::ML_CoordinateTransformation()
{
}

MLImageDetection::ML_CoordinateTransformation::~ML_CoordinateTransformation()
{
}

void MLImageDetection::ML_CoordinateTransformation::getFeatureACSLoc(FeatureLoc fiducialLoc, PlaneInfo plane, FeatureLoc &eyeboxLoc)
{
    cv::Mat R = calculateRotationMatrix(plane);
    cv::Point3f fiducialPlane = calcuatePlaneLoc(R, fiducialLoc.WGLoc);
    cv::Point3f eyeboxPlane = calcuatePlaneLoc(R, eyeboxLoc.WGLoc);
    cv::Point3f delt = eyeboxPlane - fiducialPlane;
    eyeboxLoc.ACSLoc = fiducialLoc.ACSLoc + delt;
    return ;
}

void MLImageDetection::ML_CoordinateTransformation::getFeatureACSLoc(FeatureLoc fiducialLoc, PlaneInfo plane, vector<FeatureLoc> &eyeboxLocVec)
{
    cv::Mat R = calculateRotationMatrix(plane);
    cv::Point3f fiducialPlane = calcuatePlaneLoc(R, fiducialLoc.WGLoc);
    for (int i = 0; i < eyeboxLocVec.size(); i++)
    {
        cv::Point3f eyeboxPlane = calcuatePlaneLoc(R, eyeboxLocVec[i].WGLoc);
        cv::Point3f delt = eyeboxPlane - fiducialPlane;
        if (plane.isAdd)
        {
            eyeboxLocVec[i].ACSLoc = fiducialLoc.ACSLoc + delt;
        }
        else
        {
            eyeboxLocVec[i].ACSLoc = fiducialLoc.ACSLoc - delt;
        }
    }
    return ;
}

cv::Mat MLImageDetection::ML_CoordinateTransformation::calculateRotationMatrix(PlaneInfo plane)
{
    double thetax = plane.thetaX*CV_PI/180;
    double thetay = plane.thetaY * CV_PI / 180;
    double thetaz = plane.thetaZ * CV_PI / 180;
    cv::Mat Rx = (cv::Mat_<float>(3, 3) << 1,0,0,
        0,cos(thetax),-sin(thetax),
        0,sin(thetax),cos(thetax));
    cv::Mat Ry = (cv::Mat_<float>(3, 3) << cos(thetay), 0 ,sin(thetay),
    0, 1, 0,
    -sin(thetay), 0, cos(thetay));
    cv::Mat Rz = (cv::Mat_<float>(3, 3) << cos(thetaz), -sin(thetaz), 0,
    sin(thetaz), cos(thetaz), 0,
    0, 0, 1);
   // cv::Mat R = Rz * Ry * Rx;
    cv::Mat R = Rx * Ry * Rz;

    return R;
}

cv::Point3f MLImageDetection::ML_CoordinateTransformation::calcuatePlaneLoc(cv::Mat R, cv::Point3f wgLoc)
{
    cv::Mat wgLocMat = (cv::Mat_<float>(3, 1) << wgLoc.x, wgLoc.y, wgLoc.z);
    cv::Mat locMat=R* wgLocMat;
    cv::Point3f loc;
    loc.x = locMat.at<float>(0, 0);
    loc.y = locMat.at<float>(1, 0);
    loc.z = locMat.at<float>(2, 0);
    return loc;
}
