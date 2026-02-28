#pragma once
#include "opencv2/opencv.hpp"
#include <QMap>
#include "piMotionActor/calibrationconfig.h"

using namespace cv;
using namespace std;

struct CaptureFiducialBase{
    // fiducial relative coordinates in dut config file
    std::vector<cv::Point3f> fiducials;

    // input grating relative coordinates in dut config file
    cv::Point3f inputGrating;

    // motion3DX=dutY, motion3DY=-dutX; DUT move station, DUT config coordinate system
    QMap<QString, QString> motion3DAndDutCoordRelate;

    // MV camera coordinates relative to DUT input grating
    cv::Point3f mvRelativeDutIG;

    // DUT coordinate when input grating alignment projector
    cv::Point3f alignIGCoord;

    MoveOutInfo moveOutInfo;

    DUT_EYE_TYPE eyeType;
};

class CaptureFiducialImage
{
  public:
    CaptureFiducialImage();
    ~CaptureFiducialImage();

    Result calMvCaptureFiducialCoords2D(vector<cv::Point3f>& fiducials, const QString& sn, bool isCal = true);
    Result calMvCaptureFiducialCoords2D(vector<cv::Point3f>& fiducials, const DutConfig& dutConfig);

    Result calMvCaptureFiducialCoords(vector<cv::Point3f> &fiducials, const QString &sn, bool isCal = true);
    Result calMvCaptureFiducialCoords(vector<cv::Point3f> &fiducials, const DutConfig &dutConfig);

    Result getInitMoveCoordinate(const QString &sn, cv::Point3f &coord);

  private:
    Result mvCoords(const CaptureFiducialBase &baseInfo, vector<cv::Point3f> &fiducials, bool calAngle = true);
    Result mvCoords(const CaptureFiducialBase &baseInfo, vector<cv::Point3f> &fiducials, const cv::Point2f &dutMoveOutOffset);
    Result getBaseInfo(CaptureFiducialBase &baseInfo, const DutConfig &dutConfig);

    Result calMvByDutAngle(DUT_EYE_TYPE eyeType, vector<cv::Point3f> &fiducials, const CaptureFiducialBase &baseInfo);
    vector<cv::Point3f> calMvByDutAngle(const vector<cv::Point3f> &fiducials, cv::Point2f dutAngle,
                                        cv::Point2f dutRotationAxisLength, const CaptureFiducialBase &baseInfo);

    vector<cv::Point3f> calMvByDutAngle(const vector<cv::Point3f> &fiducials, cv::Point2f dutAngle,
                                        DutRotationAxisInfo dutRotationAxisInfo, const CaptureFiducialBase &baseInfo);
   
    vector<cv::Point3f> calMvByDutAngle3D(const vector<cv::Point3f> &fiducials, cv::Point2f dutAngle,
                                        cv::Point3f dutInputGratingAxisPostion, const CaptureFiducialBase &baseInfo);

    cv::Point3f calMvByDutAngleSingle(const cv::Point3f &coord, cv::Point2f dutAngle,
                                      cv::Point2f dutRotationAxisLength);

    cv::Point3f calMvByDutAngleSingle(const cv::Point3f &coord, cv::Point2f dutAngle, DutRotationAxisInfo dutRotationAxisInfo);
};