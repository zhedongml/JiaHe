#pragma once

#ifdef ALGORITHM_EXPORTS
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
#include"ml_image_public.h"
#include"FiducialDetect.h"
using namespace std;
#define PIXEL_SIZE 4
#define MAGNICICATION 0.5
namespace MLImageDetection
{
struct FiducialBase
{
    vector<cv::Point3f> fiudcialWafer;
    vector<cv::Point3f> fiducialACS;
    bool manualFlag = false;
};

struct FiducialManual : public FiducialBase
{
   vector<cv::Point2f> fiducialPixel;
};

struct FiducialAuto : public FiducialBase
{
    vector<cv::Mat> fiducialImg;
};

struct EyeboxACS
{
    vector<cv::Point3f> eyebox;
    vector<cv::Mat> img_draw;
    cv::Point3d eulerAngle;
    ALGResult result;
};
struct RigidTrans3D
{
    cv::Mat matR;
    Eigen::Matrix3d R;
    Eigen::Vector3d eulerAngle;
    Eigen::Vector3d displacement;
    ALGResult result;
};
class ALGORITHM_API MLRigidTrans3D:public MLimagePublic
{
  public:
    MLRigidTrans3D();
   ~ MLRigidTrans3D();

  public:
   EyeboxACS getEyboxLocation(FiducialBase *f, vector<cv::Point3f> eyebox);
   EyeboxACS getEyeboxLocationManual(FiducialManual *f, vector<cv::Point3f> eyebox);
   EyeboxACS getEyeboxLocationAuto(FiducialAuto *f, vector<cv::Point3f> eyebox);
   
  public:
   RigidTrans3D getRigidTrans3D(std::vector<cv::Point3f> srcPoints, std::vector<cv::Point3f> dstPoints);
   vector<cv::Point3f> getTranslationLoc(RigidTrans3D trans,vector<cv::Point3f>src);

  private:
   cv::Point3f getFiducialACSLoc(cv::Point2f fiducialPixel,cv::Point3f fiducialACS);
   cv::Point3f centroid(std::vector<cv::Point3f> pt);
   Eigen::Vector3d rotationMatrixToEulerAngles(Eigen::Matrix3d &R);

 private:
   vector<cv::Mat> m_img_draw;
   cv::Size m_size;


};
} // namespace algorithm
