#ifndef MLALGORITHM_MLMEASURE_H_
#define MLALGORITHM_MLMEASURE_H_

#include "ml_distortion_correction.h"
#include "ml_machine_vision.h"
#include <map>
#include <numeric>
#include <qstring.h>
#include <vector>
using namespace std;

#define PIXEL_SIZE 1.38
#define MAGNIFICATION 0.2

struct RotateOffset
{
    cv::Point2f offset;
    double radian = 361; // angle = radian * 180 / M_PI;
};
struct CalibrationAcsRobot
{
    float ACSX1 = 0;
    float ACSY1 = 0;
    float RobotX1 = 0;
    float RobotY1 = 0;
    float ACSX2 = 0;
    float ACSY2 = 0;
    float RobotX2 = 0;
    float RobotY2 = 0;
    float OffsetCImageX1 = 0;
    float OffsetCImageY1 = 0;
    float OffsetCImageX2 = 0;
    float OffsetCImageY2 = 0;
};

struct RotateOffsetCalData
{
    vector<cv::Point2f> srcPoints;
    vector<cv::Point2f> destPoints;

    RotateOffsetCalData()
    {
    }

    RotateOffsetCalData(vector<cv::Point2f> srcPoints, vector<cv::Point2f> destPoints)
    {
        this->srcPoints = srcPoints;
        this->destPoints = destPoints;
    }

    RotateOffsetCalData(vector<cv::Point2f> srcPoints, vector<cv::Point3f> destPoints)
    {
        this->srcPoints = srcPoints;
        for (int i = 0; i < destPoints.size(); ++i)
        {
            this->destPoints.push_back(cv::Point2f(destPoints[i].x, destPoints[i].y));
        }
    }

    RotateOffsetCalData(vector<cv::Point3f> srcPoints, vector<cv::Point2f> destPoints)
    {
        this->destPoints = destPoints;
        for (int i = 0; i < srcPoints.size(); ++i)
        {
            this->srcPoints.push_back(cv::Point2f(srcPoints[i].x, srcPoints[i].y));
        }
    }
};

namespace algorithm
{
class ALGORITHM_API MLMeasureRobot
{
  public:
    MLMeasureRobot();
    ~MLMeasureRobot();
    // IQT7
    // meachine arm
    MLMeasureRobot(cv::Point2f f1_wafer, cv::Point3f f1_robot);
    void addReference(cv::Point2f f2_wafer, cv::Point3f f2_robot);
    void addFiducial1Image(cv::Mat img_f1);
    vector<cv::Point2f> findFiducial();
    void SetFiucialPxlLocation(vector<cv::Point2f> fiducialPxlLocation);
    vector<cv::Point3f> MeasureRobotLocation(vector<cv::Point3f> fiducial_wafer);
    void ClearLocationParameter();
    double getClockAngle();

    // void SetFiducial2Image(cv::Mat img_f2);
    void setCenter(cv::Point2f center_);
    cv::Point2f getCenter();

  private:
    cv::Point3f CalculateFiducialRobotLocaion(cv::Point2f gloalPoints, cv::Point3f f1_rbot);

  private:
    // IQT para
    RotateOffset m_rotateOffset;
    cv::Point2f m_center_ = cv::Point2f(1232.0, 1028.0);
    vector<cv::Mat> m_img_fiducial;

    vector<cv::Point3f> m_f_robot;
    vector<cv::Point3f> m_fiducialRobotLocation;
    vector<cv::Point2f> m_fiducialWaferLocation;
};

class ALGORITHM_API MLMeasureMV
{
  public:
    MLMeasureMV();
    ~MLMeasureMV();
    MLMeasureMV(cv::Point2f f1_wg, cv::Point3f f1_motor);
    void addReference(cv::Point2f f2_wg, cv::Point3f f2_motor);
    void addFiducialImage(cv::Mat img_f2);
    vector<cv::Point2f> findFiducial();
    void SetFiucialPxlLocation(vector<cv::Point2f> fiducialPxlLocation, QString xCoord = "-y", QString yCoord = "-x");
    vector<cv::Point3f> MeasureEyeBoxMotorLocation(vector<cv::Point3f> eyeboxWGLocation);
    void ClearLocationParameterMV();
    void setImageCenter(cv::Mat image);

  private:
    cv::Point3f CalculateFiducialMotorLocaion(cv::Point2f gloalPoints, cv::Point3f f1_motor, QString xCoord = "-y",
                                              QString yCoord = "-x");
    cv::Point2f relMoveCoordMVToAcs(cv::Point2f coord, QString acsX, QString acsY);

  public:
    // void SetFiducialImage(cv::Mat img_f1);
    // IQT NEW
  private:
    // IQT para
    int count = 1;
    RotateOffset m_rotateOffset;
    cv::Point2f m_center_ = cv::Point2f(1232.0, 1028.0);
    vector<cv::Mat> m_img_fiducial;

    vector<cv::Point3f> m_f_motor;
    vector<cv::Point2f> m_fiducialPxlLocation;
    vector<cv::Point3f> m_fiducialMotorLocation;
    vector<cv::Point2f> m_fiducialWGLocation;
};
class ALGORITHM_API MLMeasureAcsToRobot
{
  public:
    MLMeasureAcsToRobot();
    ~MLMeasureAcsToRobot();

    MLMeasureAcsToRobot(/*cv::Point2f f1_Acs, cv::Point3f f1_robot, */ CalibrationAcsRobot OrigData,
                        cv::Point3f alignmentMotionPos);
    void addReference(cv::Point2f f2_Acs, cv::Point3f f2_robot);
    void addFiducialImage(cv::Mat img_f3);
    vector<cv::Point2f> findFiducial();
    vector<cv::Point3f> SetFiucialPxlLocation(vector<cv::Point2f> fiducialPxlLocation,
                                              vector<cv::Point3f> fiducialRobot);
    cv::Point3f MapObjectsAcsFromRobot(cv::Point3f RobotLocation);
    cv::Point3f robotOffsetToAcs(cv::Point3f offsetToinput);
    cv::Point2f imgOffsetToRobot(cv::Point2f robotPos, cv::Point2f imageOffset);
    void ClearLocationParameter();

  private:
    cv::Point3f CalculateAcsRobotLocaion(cv::Point2f gloalPoints, cv::Point3f f1_motor);
    vector<cv::Point3f> OffsetViewCenterShift(vector<cv::Point2f> fiducialPxlLocation,
                                              vector<cv::Point3f> fiducialRobot);
    cv::Point2f imgToRobotOffset(cv::Point2f imageOffset);
    RotateOffset calRoataOffset();

  private:
    RotateOffset m_rotateOffset;
    cv::Point2f m_center_ = cv::Point2f(1232.0, 1028.0);
    vector<cv::Mat> m_img_fiducial;
    vector<cv::Point3f> m_f_robotRefAcs;
    vector<cv::Point2f> m_fiducialPxlLocation;
    vector<cv::Point3f> m_fiducialRobotLocation;
    vector<cv::Point2f> m_fiducialACSLocation;

    CalibrationAcsRobot m_CalibrationData;
    cv::Point3f m_alignmentMotionPos;
};

RotateOffset calculateRoataOffset(RotateOffsetCalData rotateOffsetCalData);

cv::Point2f CalculatePxlLocation(cv::Mat img, cv::Rect roiRect = cv::Rect(0, 0, -1, -1), int typeflag = 2);
cv::Point2f CalculateGlobalLocation(cv::Point2f center, cv::Point2f fiducialPxl, cv::Point2f initial_point);
} // namespace algorithm

#endif // !MLALGORITHM_MLMEASURE_H_
