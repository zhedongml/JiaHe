#include "ml_measure.h"
#include "LogPlus.h"
#include "libcbdetect/boards_from_corners.h"
#include "libcbdetect/config.h"
#include "libcbdetect/find_corners.h"
#include "libcbdetect/plot_boards.h"
#include "libcbdetect/plot_corners.h"
#include <color-util/HSL_to_RGB.hpp>
#include <color-util/Lab_to_XYZ.hpp>
#include <color-util/RGB_to_HSL.hpp>
#include <color-util/RGB_to_XYZ.hpp>
#include <color-util/XYZ_to_Lab.hpp>
#include <color-util/XYZ_to_RGB.hpp>
algorithm::MLMeasureRobot::MLMeasureRobot()
{
}

algorithm::MLMeasureRobot::~MLMeasureRobot()
{
}

algorithm::MLMeasureRobot::MLMeasureRobot(cv::Point2f f1_wafer, cv::Point3f f1_robot)
{
    m_fiducialWaferLocation.push_back(f1_wafer);
    m_f_robot.push_back(f1_robot);
}

void algorithm::MLMeasureRobot::addReference(cv::Point2f f2_wafer, cv::Point3f f2_robot)
{
    m_fiducialWaferLocation.push_back(f2_wafer);
    m_f_robot.push_back(f2_robot);
}

void algorithm::MLMeasureRobot::SetFiucialPxlLocation(vector<cv::Point2f> fiducialPxlLocation)
{
    for (int i = 0; i < fiducialPxlLocation.size(); i++)
    {
        std::string message = "Robot pixel location:" + std::to_string(fiducialPxlLocation[i].x) + "," +
                              std::to_string(fiducialPxlLocation[i].y);
        LOG4CPLUS_DEBUG(LogPlus::getInstance()->logger, message.c_str());
        cv::Point2f fiducial_global = CalculateGlobalLocation(m_center_, fiducialPxlLocation[i], cv::Point2f(0, 0));
        cv::Point3f fiduRobot = CalculateFiducialRobotLocaion(fiducial_global, m_f_robot[i]);

        message =
            "Robot location for fiducial to center:" + std::to_string(fiduRobot.x) + "," + std::to_string(fiduRobot.y);
        LOG4CPLUS_DEBUG(LogPlus::getInstance()->logger, message.c_str());

        m_fiducialRobotLocation.push_back(fiduRobot);
    }
}

vector<cv::Point3f> algorithm::MLMeasureRobot::MeasureRobotLocation(vector<cv::Point3f> fiducial_robot)
{
    if (m_rotateOffset.radian > 360)
    {
        m_rotateOffset = calculateRoataOffset(RotateOffsetCalData(m_fiducialWaferLocation, m_fiducialRobotLocation));
        std::string message = "Robot clock angle :" + std::to_string(m_rotateOffset.radian);
        LOG4CPLUS_DEBUG(LogPlus::getInstance()->logger, message.c_str());
    }
    cv::Point2f offset = m_rotateOffset.offset;
    double radian = m_rotateOffset.radian;

    vector<cv::Point3f> fiducialRobotLocation;
    for (int i = 0; i < fiducial_robot.size(); i++)
    {
        // cv::Point3f temp;
        cv::Point3f temp;
        temp.x = fiducial_robot[i].x * cos(radian) + fiducial_robot[i].y * sin(radian) + offset.x;
        temp.y = fiducial_robot[i].y * cos(radian) - fiducial_robot[i].x * sin(radian) + offset.y;
        temp.z = 0;
        fiducialRobotLocation.push_back(temp);
    }
    return fiducialRobotLocation;
}

void algorithm::MLMeasureRobot::ClearLocationParameter()
{
    m_fiducialRobotLocation.clear();
    m_fiducialWaferLocation.clear();
    // m_img_fiducial.clear();
}

cv::Point3f algorithm::MLMeasureRobot::CalculateFiducialRobotLocaion(cv::Point2f gloalPoints, cv::Point3f f1_rbot)
{
    // Convert image coords to global coords
    cv::Point2f cameraOffset = cv::Point2f(gloalPoints.y, gloalPoints.x);

    cv::Point3f fiducialMoto;
    fiducialMoto.x = f1_rbot.x - cameraOffset.x;
    fiducialMoto.y = f1_rbot.y - cameraOffset.y;
    fiducialMoto.z = f1_rbot.z;
    return fiducialMoto;
}

double algorithm::MLMeasureRobot::getClockAngle()
{
    if (abs(m_rotateOffset.radian) >= 360.0)
    {
        m_rotateOffset = calculateRoataOffset(RotateOffsetCalData(m_fiducialWaferLocation, m_fiducialRobotLocation));
    }
    return m_rotateOffset.radian;
}

void algorithm::MLMeasureRobot::addFiducial1Image(cv::Mat img_f1)
{
    m_img_fiducial.push_back(img_f1);

    m_center_.x = img_f1.rows / 2;
    m_center_.y = img_f1.cols / 2;
}

vector<cv::Point2f> algorithm::MLMeasureRobot::findFiducial()
{
    vector<cv::Point2f> fiducialPxlLocation;
    for (int i = 0; i < m_img_fiducial.size(); i++)
    {
        // if (i == 0)
        //	ClearLocationParameter();
        cv::Mat img = m_img_fiducial[i];
        cv::Point3f fiducialMoto;
        cv::Point3f fiduRobot;
        cv::Point2f fiduPxl = CalculatePxlLocation(img);
        fiducialPxlLocation.push_back(fiduPxl);
        if (fiduPxl.x != 0 & fiduPxl.y != 0)
        {
            cv::Point2f fiduGlo = CalculateGlobalLocation(m_center_, fiduPxl, cv::Point2f(0, 0));
            fiduRobot = CalculateFiducialRobotLocaion(fiduGlo, m_f_robot[i]);
            m_fiducialRobotLocation.push_back(fiduRobot);
        }
        // else
        //{
        //	fiduRobot = cv::Point3f(0, 0, 0);
        //	m_fiducialRobotLocation.push_back(fiduRobot);
        //}
    }
    return fiducialPxlLocation;
}

void algorithm::MLMeasureRobot::setCenter(cv::Point2f center_)
{
    m_center_ = center_;
}

cv::Point2f algorithm::MLMeasureRobot::getCenter()
{
    return m_center_;
}

algorithm::MLMeasureMV::MLMeasureMV()
{
}

algorithm::MLMeasureMV::~MLMeasureMV()
{
}

algorithm::MLMeasureMV::MLMeasureMV(cv::Point2f f1_wg, cv::Point3f f1_motor)
{
    // f1_wg.y = -f1_wg.y;

    m_fiducialWGLocation.push_back(f1_wg);
    m_f_motor.push_back(f1_motor);
}

void algorithm::MLMeasureMV::addReference(cv::Point2f f2_wg, cv::Point3f f2_motor)
{
    // f2_wg.y = -f2_wg.y;

    m_fiducialWGLocation.push_back(f2_wg);
    m_f_motor.push_back(f2_motor);
}

vector<cv::Point3f> algorithm::MLMeasureMV::MeasureEyeBoxMotorLocation(vector<cv::Point3f> eyeboxWGLocation)
{
    // for (int i = 0; i < eyeboxWGLocation.size(); ++i) {
    //	eyeboxWGLocation[i].y = -eyeboxWGLocation[i].y;
    //}

    if (m_rotateOffset.radian > 360)
    {
        m_rotateOffset = calculateRoataOffset(RotateOffsetCalData(m_fiducialWGLocation, m_fiducialMotorLocation));

        std::string message = "Motion3D clock angle :" + std::to_string(m_rotateOffset.radian);
        LOG4CPLUS_DEBUG(LogPlus::getInstance()->logger, message.c_str());
    }
    cv::Point2f offset = m_rotateOffset.offset;
    double radian = m_rotateOffset.radian;

    vector<cv::Point3f> EyeboxMotorLocation;
    for (int i = 0; i < eyeboxWGLocation.size(); i++)
    {
        cv::Point3f temp;
        temp.x = eyeboxWGLocation[i].x * cos(radian) + eyeboxWGLocation[i].y * sin(radian) + offset.x;
        temp.y = eyeboxWGLocation[i].y * cos(radian) - eyeboxWGLocation[i].x * sin(radian) + offset.y;
        temp.z = 0;
        EyeboxMotorLocation.push_back(temp);
    }
    return EyeboxMotorLocation;
}

void algorithm::MLMeasureMV::ClearLocationParameterMV()
{
    m_fiducialPxlLocation.clear();
    m_fiducialMotorLocation.clear();
    m_fiducialWGLocation.clear();
    // m_img_fiducial.clear();
}

void algorithm::MLMeasureMV::setImageCenter(cv::Mat image)
{
    m_center_.x = image.cols / 2;
    m_center_.y = image.rows / 2;
}

cv::Point3f algorithm::MLMeasureMV::CalculateFiducialMotorLocaion(cv::Point2f gloalPoints, cv::Point3f f1_motor,
                                                                  QString xCoord, QString yCoord)
{
    // Convert image coords to global coords
    gloalPoints = relMoveCoordMVToAcs(gloalPoints, xCoord, yCoord);
    cv::Point2f cameraOffset = cv::Point2f(gloalPoints.x, gloalPoints.y);
    // cv::Point2f cameraOffset = cv::Point2f(-gloalPoints.y, -gloalPoints.x);

    cv::Point3f fiducialMotor;
    fiducialMotor.x = f1_motor.x + cameraOffset.x;
    fiducialMotor.y = f1_motor.y + cameraOffset.y;
    fiducialMotor.z = f1_motor.z;
    return fiducialMotor;
}

cv::Point2f algorithm::MLMeasureMV::relMoveCoordMVToAcs(cv::Point2f coord, QString acsX, QString acsY)
{
    cv::Point2f newPos = coord;
    //Ä¬ÈÏhydrus3MV¹ØÏµ
    // cv::Point2f cameraOffset = cv::Point2f(-gloalPoints.y, -gloalPoints.x);
    if (acsX.contains("X"))
    {
        if (acsX.contains("-"))
        {
            newPos.x = -coord.x;
        }
        else
        {
            newPos.x = coord.x;
        }
    }
    else if (acsX.contains("Y"))
    {
        if (acsX.contains("-"))
        {
            newPos.x = -coord.y;
        }
        else
        {
            newPos.x = coord.y;
        }
    }
    else
    {
        newPos.x = -coord.y;
    }

    if (acsY.contains("X"))
    {
        if (acsY.contains("-"))
        {
            newPos.y = -coord.x;
        }
        else
        {
            newPos.y = coord.x;
        }
    }
    else if (acsY.contains("Y"))
    {
        if (acsY.contains("-"))
        {
            newPos.y = -coord.y;
        }
        else
        {
            newPos.y = coord.y;
        }
    }
    else
    {
        newPos.y = -coord.x;
    }

    return newPos;
}

void algorithm::MLMeasureMV::addFiducialImage(cv::Mat img_f2)
{
    m_img_fiducial.push_back(img_f2);
    // m_img_f2 = img_f2;;
    m_center_.x = img_f2.rows / 2;
    m_center_.y = img_f2.cols / 2;
}

vector<cv::Point2f> algorithm::MLMeasureMV::findFiducial()
{

    for (int i = 0; i < m_img_fiducial.size(); i++)
    {
        // if (i == 0)
        // ClearLocationParameterMV();
        cv::Mat img = m_img_fiducial[i];
        cv::Point3f fiducialMoto;
        cv::Point3f fiduMoto;
        cv::Point2f fiduPxl = CalculatePxlLocation(img);
        m_fiducialPxlLocation.push_back(fiduPxl);
        if ((fiduPxl.x != 0) & (fiduPxl.y != 0))
        {
            cv::Point2f fiduGlo = CalculateGlobalLocation(m_center_, fiduPxl, cv::Point2f(0, 0));
            fiduMoto = CalculateFiducialMotorLocaion(fiduGlo, m_f_motor[i]);
            m_fiducialMotorLocation.push_back(fiduMoto);
        }
        // else
        //{
        //	fiduMoto = cv::Point3f(0, 0, 0);
        //	m_fiducialMotorLocation.push_back(fiduMoto);
        //}
    }
    return m_fiducialPxlLocation;
}

void algorithm::MLMeasureMV::SetFiucialPxlLocation(vector<cv::Point2f> fiducialPxlLocation, QString xCoord,
                                                   QString yCoord)
{
    for (int i = 0; i < fiducialPxlLocation.size(); i++)
    {
        // if (i == 0)
        // ClearLocationParameterMV();
        m_fiducialPxlLocation.push_back(fiducialPxlLocation[i]);

        std::string message = "Motoion3D pixel location:" + std::to_string(fiducialPxlLocation[i].x) + "," +
                              std::to_string(fiducialPxlLocation[i].y);
        LOG4CPLUS_DEBUG(LogPlus::getInstance()->logger, message.c_str());

        cv::Point2f fiducial_global = CalculateGlobalLocation(m_center_, fiducialPxlLocation[i], cv::Point2f(0, 0));
        cv::Point3f fiduMoto = CalculateFiducialMotorLocaion(fiducial_global, m_f_motor[i], xCoord, yCoord);

        message = "Motoion3D location for fiducial to center:" + std::to_string(fiduMoto.x) + "," +
                  std::to_string(fiduMoto.y);
        LOG4CPLUS_DEBUG(LogPlus::getInstance()->logger, message.c_str());

        m_fiducialMotorLocation.push_back(fiduMoto);
    }
}

#pragma region AcsToRobot Fun
algorithm::MLMeasureAcsToRobot::MLMeasureAcsToRobot()
{
}
algorithm::MLMeasureAcsToRobot::~MLMeasureAcsToRobot()
{
}
algorithm::MLMeasureAcsToRobot::MLMeasureAcsToRobot(
    /*cv::Point2f f1_ACS, cv::Point3f f1_robot, */ CalibrationAcsRobot OrigData, cv::Point3f alignmentMotionPos)
{
    m_CalibrationData = OrigData;
    m_alignmentMotionPos = alignmentMotionPos;
}
void algorithm::MLMeasureAcsToRobot::addReference(cv::Point2f f2_ACS, cv::Point3f f2_robot)
{
    m_fiducialACSLocation.push_back(f2_ACS);
    m_f_robotRefAcs.push_back(f2_robot);
}

vector<cv::Point3f> algorithm::MLMeasureAcsToRobot::SetFiucialPxlLocation(vector<cv::Point2f> fiducialPxlLocation,
                                                                          vector<cv::Point3f> fiducialRotbot)
{
    vector<cv::Point3f> fiducialRobotLocation;
    for (int i = 0; i < fiducialPxlLocation.size(); i++)
    {
        cv::Point2f fiducial_global = CalculateGlobalLocation(m_center_, fiducialPxlLocation[i], cv::Point2f(0, 0));
        cv::Point3f fiduRobot = CalculateAcsRobotLocaion(fiducial_global, fiducialRotbot[i]);
        fiducialRobotLocation.push_back(fiduRobot);
    }
    return fiducialRobotLocation;
}

vector<cv::Point3f> algorithm::MLMeasureAcsToRobot::OffsetViewCenterShift(vector<cv::Point2f> fiducialPxlLocation,
                                                                          vector<cv::Point3f> fiducialRotbot)
{
    return SetFiucialPxlLocation(fiducialPxlLocation, fiducialRotbot);
}

void algorithm::MLMeasureAcsToRobot::addFiducialImage(cv::Mat img_f3)
{
    m_img_fiducial.push_back(img_f3);
    m_center_.x = img_f3.rows / 2;
    m_center_.y = img_f3.cols / 2;
}

vector<cv::Point2f> algorithm::MLMeasureAcsToRobot::findFiducial()
{
    for (int i = 0; i < m_img_fiducial.size(); i++)
    {
        cv::Mat img = m_img_fiducial[i];
        cv::Point3f fiducialMoto;
        cv::Point3f fiduRobot;
        cv::Point2f fiduPxl = CalculatePxlLocation(img);
        m_fiducialPxlLocation.push_back(fiduPxl);
        if (fiduPxl.x != 0 & fiduPxl.y != 0)
        {
            cv::Point2f fiduGlo = CalculateGlobalLocation(m_center_, fiduPxl, cv::Point2f(0, 0));
            fiduRobot = CalculateAcsRobotLocaion(fiduGlo, m_f_robotRefAcs[i]);
            m_fiducialRobotLocation.push_back(fiduRobot);
        }
    }
    return m_fiducialPxlLocation;
}
/// <summary>
/// cal the coord of Robot form Acs
/// </summary>
/// <param name="gloalPoints">offset of view by camera</param>
/// <param name="f1_rbot">Mrak robot coor</param>
/// <returns>robot coor</returns>
cv::Point3f algorithm::MLMeasureAcsToRobot::CalculateAcsRobotLocaion(cv::Point2f globalPoints, cv::Point3f f1_rbot)
{
    // Convert image coords to global coords
    cv::Point2f cameraOffset = cv::Point2f(globalPoints.y, globalPoints.x);

    cv::Point3f fiducialMoto;
    cv::Point3f temp;
    temp.x = f1_rbot.x - cameraOffset.x;
    temp.y = f1_rbot.y - cameraOffset.y;
    temp.z = f1_rbot.z;
    fiducialMoto = temp;
    return fiducialMoto;
}

void algorithm::MLMeasureAcsToRobot::ClearLocationParameter()
{
    m_fiducialPxlLocation.clear();
    m_fiducialPxlLocation.clear();
    m_fiducialRobotLocation.clear();
    m_fiducialACSLocation.clear();
}

/// <summary>
/// Call by external function
/// </summary>
/// <param name="vRobotLocation"></param>
/// <param name="therta"></param>
/// <param name="offset"></param>
/// <returns></returns>
cv::Point3f algorithm::MLMeasureAcsToRobot::MapObjectsAcsFromRobot(cv::Point3f RobotLocation)
{
    cv::Point3f temp;
    return temp;
}

cv::Point3f algorithm::MLMeasureAcsToRobot::robotOffsetToAcs(cv::Point3f offsetToinput)
{
    offsetToinput.y = -offsetToinput.y;
    m_rotateOffset = calRoataOffset();
    double radian = m_rotateOffset.radian;
    radian = 0;
    cv::Point3f delta;
    delta.x = offsetToinput.x * cos(radian) + offsetToinput.y * sin(radian);
    delta.y = offsetToinput.y * cos(radian) - offsetToinput.x * sin(radian);

    cv::Point3f ret;
    ret.x = m_alignmentMotionPos.x + delta.x;
    ret.y = m_alignmentMotionPos.y + delta.y;
    return ret;
}

cv::Point2f algorithm::MLMeasureAcsToRobot::imgOffsetToRobot(cv::Point2f robotPos, cv::Point2f imageOffset)
{
    cv::Point2f robotOffset = imgToRobotOffset(imageOffset);
    robotPos.x += robotOffset.x;
    robotPos.y += robotOffset.y;
    return robotPos;
}

cv::Point2f algorithm::MLMeasureAcsToRobot::imgToRobotOffset(cv::Point2f imageOffset)
{
    if (imageOffset.x == 0 && imageOffset.y == 0)
    {
        return cv::Point2f(0, 0);
    }

    imageOffset = cv::Point2f(imageOffset.y, -imageOffset.x);

    m_rotateOffset = this->calRoataOffset();
    double radian = -m_rotateOffset.radian;

    cv::Point2f delta;
    delta.x = imageOffset.x * cos(radian) + imageOffset.y * sin(radian);
    delta.y = imageOffset.y * cos(radian) - imageOffset.x * sin(radian);
    return delta;
}

RotateOffset algorithm::MLMeasureAcsToRobot::calRoataOffset()
{
    if (m_rotateOffset.radian > 360)
    {
        vector<cv::Point3f> fiducialRobotLocation;
        vector<cv::Point2f> fiducialACSLocation;
        cv::Point2f aP1 = cv::Point2f(m_CalibrationData.ACSX1, m_CalibrationData.ACSY1);
        cv::Point2f aP2 = cv::Point2f(m_CalibrationData.ACSX2, m_CalibrationData.ACSY2);
        cv::Point3f RP1 = cv::Point3f(m_CalibrationData.RobotX1, m_CalibrationData.RobotY1, 0);
        cv::Point3f RP2 = cv::Point3f(m_CalibrationData.RobotX2, m_CalibrationData.RobotY2, 0);
        fiducialACSLocation.push_back(aP1);
        fiducialACSLocation.push_back(aP2);
        fiducialRobotLocation.push_back(RP1);
        fiducialRobotLocation.push_back(RP2);

        for (int i = 0; i < fiducialRobotLocation.size(); ++i)
        {
            fiducialRobotLocation[i].y = -fiducialRobotLocation[i].y;
        }
        m_rotateOffset = calculateRoataOffset(RotateOffsetCalData(fiducialRobotLocation, fiducialACSLocation));
    }
    return m_rotateOffset;
}
double computeSineOfAngleBetweenVectors(const Eigen::VectorXd &vector0, const Eigen::VectorXd &vector1)
{
    assert(vector0.size() == vector1.size());
    // Get the cosine of the angle by dotting the normalized vectors.
    Eigen::VectorXd vector00 = vector0.normalized();
    Eigen::VectorXd vector11 = vector1.normalized();
    double crossProductOfNormalizedVectors = vector00(0) * vector11(1) - vector00(1) * vector11(0);
    // Explicitly define the extreme cases, which can give problems with the acos function.
    if (crossProductOfNormalizedVectors >= 1.0)
    {
        return 1.0;
    }

    else if (crossProductOfNormalizedVectors <= -1.0)
    {
        return -1.0;
    }
    // Determine the actual angle.
    else
    {
        return crossProductOfNormalizedVectors;
    }
}
double computeCosineOfAngleBetweenVectors(const Eigen::VectorXd &vector0, const Eigen::VectorXd &vector1)
{
    assert(vector0.size() == vector1.size());

    // Get the cosine of the angle by dotting the normalized vectors.
    double dotProductOfNormalizedVectors = vector0.normalized().dot(vector1.normalized());

    // Explicitly define the extreme cases, which can give problems with the acos function.
    if (dotProductOfNormalizedVectors >= 1.0)
    {
        return 1.0;
    }

    else if (dotProductOfNormalizedVectors <= -1.0)
    {
        return -1.0;
    }
    // Determine the actual angle.
    else
    {
        return dotProductOfNormalizedVectors;
    }
}

RotateOffset algorithm::calculateRoataOffset(RotateOffsetCalData rotateOffsetCalData)
{
    RotateOffset rotateOffset;
    vector<cv::Point2f> srcPoints = rotateOffsetCalData.srcPoints;
    vector<cv::Point2f> destPoints = rotateOffsetCalData.destPoints;

    if (srcPoints.size() == 2 & destPoints.size() == 2)
    {
        Eigen::Vector2d srcPoint(srcPoints[0].x - srcPoints[1].x, srcPoints[0].y - srcPoints[1].y);
        Eigen::Vector2d destPoint(destPoints[0].x - destPoints[1].x, destPoints[0].y - destPoints[1].y);
        double cosine = computeCosineOfAngleBetweenVectors(srcPoint, destPoint);
        double crossProduct = computeSineOfAngleBetweenVectors(srcPoint, destPoint);
        if (abs(cosine) != 1)
        {
            // check the rotate angle neg or positive
            if (crossProduct >= 0)
            {
                rotateOffset.radian = -acos(cosine);
            }
            else
            {
                rotateOffset.radian = acos(cosine);
            }
            cv::Point2f srPointcRotate;
            srPointcRotate.x = srcPoints[0].x * cos(rotateOffset.radian) + srcPoints[0].y * sin(rotateOffset.radian);
            srPointcRotate.y = srcPoints[0].y * cos(rotateOffset.radian) - srcPoints[0].x * sin(rotateOffset.radian);
            rotateOffset.offset.x = destPoints[0].x - srPointcRotate.x;
            rotateOffset.offset.y = destPoints[0].y - srPointcRotate.y;
        }
        else if (srcPoints.size() == 1 & destPoints.size() == 1)
        {
            rotateOffset.radian = 0;
            rotateOffset.offset.x = destPoints[0].x - srcPoints[0].x;
            rotateOffset.offset.y = destPoints[0].y - srcPoints[0].y;
        }
        return rotateOffset;
    }
}

/// <summary>
/// offset from image center
/// </summary>
/// <param name="img"></param>
/// <param name="roiRect"></param>
/// <param name="typeflag"></param>
/// <returns></returns>
cv::Point2f algorithm::CalculatePxlLocation(cv::Mat img, cv::Rect roiRect, int typeflag)
{
    cv::Point2f fiducialPxl;
    cv::Mat img_gray;
    if (img.channels() == 3)
    {
        cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    }
    else
        img_gray.clone();
    // cv::Mat roi = img_gray(roiRect);
    cv::Mat roi;
    if (roiRect.x == 0 & roiRect.y == 0)
        roi = img_gray;
    else
        roi = img_gray(roiRect);
    cv::Point2f pxlroi;
    if (typeflag == 1 || typeflag == 3)
    {
        pxlroi = cv::Point2f(0, 0); // MLMachineVision::CircleFiducialDetect(roi);
    }
    else if (typeflag == 2 || typeflag == 0)
    {
        pxlroi = cv::Point2f(0, 0); // MLMachineVision::LineFiducialDetect(roi);
    }
    if (pxlroi.x != 0 & pxlroi.y != 0)
    {
        cv::Point2f temp;
        temp.x = pxlroi.x + roiRect.x;
        temp.y = pxlroi.y + roiRect.y;
        fiducialPxl = temp;
    }
    // m_pxllocation.push_back(fiducialPxl);
    return fiducialPxl;
}

cv::Point2f algorithm::CalculateGlobalLocation(cv::Point2f center, cv::Point2f fiducialPxl, cv::Point2f initial_point)
{
    cv::Point2f global_point;
    cv::Point2f cameraOffset = center - cv::Point2f(fiducialPxl.x, fiducialPxl.y);

    global_point = cameraOffset * PIXEL_SIZE / MAGNIFICATION / 1000.0;
    global_point = global_point + initial_point;
    return global_point;
}

#pragma endregion
