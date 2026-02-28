#include "MAalignmentLogic.h"
#include <Core/modemanager.h>

#include <QWidget>
#include <integratingSphere\integratingspheretcpmodel.h>
#include "CameraModel.h"

#include "piMotionActor\ConfigMAInfo.h"
#include "piMotionActor\configItem.h"
#include <LogPlus.h>
#include <MAalignment\MAalignmentLogic.h>
#include <PLCControl\PLCController.h>
#include <QtConcurrent>
#include <QtWidgets\qfiledialog.h>
#include <Core/icore.h>
#include <PrjCommon/metricsdata.h>
#include <Core/modemanager.h>
#include "Core/loggingwrapper.h"
#include "PrjCommon/metricsdata.h"
#include "piMotionActor/MoveLimit.h"
#include "piMotionActor/DutProjectorCalibrate.h"
#include "integratingSphere/ReticleMode2D.h"
#include "opencv2/opencv.hpp"
#include "FiducialRecognize.h"

using namespace Core;

MAalignmentLogic *MAalignmentLogic::m_instance = nullptr;

MAalignmentLogic::MAalignmentLogic(QObject *parent) : QObject(parent)
{
}

MAalignmentLogic::~MAalignmentLogic()
{
}

MAalignmentLogic *MAalignmentLogic::instance(QObject *parent)
{
    if (m_instance == nullptr)
    {
        m_instance = new MAalignmentLogic(parent);
    }
    return m_instance;
}

Result MAalignmentLogic::calibrate(const AAlignConfig &alignConfig)
{
    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
    LoggingWrapper::instance()->debug(
        QString("Input grating alignment start, DUT SN %1, Eye type %2.").arg(dutConfig.sn).arg(dutConfig.eyeType == 0 ? "left" : "right"));
    MoveLimit::instance()->setInputAlignState(false);

    Result ret = isConnected();
    if (!ret.success)
    {
        LoggingWrapper::instance()->error(
            QString("Input grating alignment Z failed, %1.").arg(QString::fromStdString(ret.errorMsg)));
        return ret;
    }

    ret = MoveLimit::moveBeforeInputAA();
    if (!ret.success)
    {
        LoggingWrapper::instance()->error(
            QString("Input grating alignment Z failed, %1.").arg(QString::fromStdString(ret.errorMsg)));
        return ret;
    }

    // DUT Z alignment 
    float dutZ;
    ret = m_findDistance.getBestDistance(dutZ, DutDistanceInfo(alignConfig.inView, alignConfig.distanceLimit, alignConfig.stepZ));
    if (!ret.success)
    {
        LoggingWrapper::instance()->error(
            QString("Input grating alignment Z failed, %1.").arg(QString::fromStdString(ret.errorMsg)));
        return ret;
    }
    LoggingWrapper::instance()->info(QString("Input grating alignment Z end, Z = %1.").arg(dutZ));

    ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    ret = MoveLimit::instance()->setMotion3DMoveAlignSpeed(true);
    if (!ret.success)
    {
        return ret;
    }

    // DUT X,Y alignment
    ret = alignXY(alignConfig);
    if (!ret.success)
    {
        ret = MoveLimit::instance()->setMotion3DMoveAlignSpeed(false);
        return ret;
    }

    ret = MoveLimit::instance()->setMotion3DMoveAlignSpeed(false);
    if (!ret.success)
    {
        return ret;
    }

    saveResultImage(true);
    ret = AlignUtils::controlLight(false, false);
    if (!ret.success)
    {
        return ret;
    }

    Sleep(2000);

    {
        cv::Point3f dutCoordCurrent;
        ret = MoveLimit::getCoordinate(withDUT, dutCoordCurrent);
        if (ret.success)
        {
            LoggingWrapper::instance()->info(QString("Input AA alignment result, DUT coordinate is X=%1, Y=%2, Z=%3.")
                                                 .arg(dutCoordCurrent.x)
                                                 .arg(dutCoordCurrent.y)
                                                 .arg(dutCoordCurrent.z));
        }
    }

    ret =  MoveLimit::instance()->setInputAlignState(ret.success);
    if (!ret.success)
    {
        LoggingWrapper::instance()->error(
            QString("Input grating alignment failed, %1.").arg(QString::fromStdString(ret.errorMsg)));
    }
    else
    {
        LoggingWrapper::instance()->debug(QString("Input grating alignment success, DUT SN %1, Eye type %2.")
                                              .arg(dutConfig.sn)
                                              .arg(dutConfig.eyeType == 0 ? "left" : "right"));
    }

    ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    return ret;
}

Result MAalignmentLogic::calibrateZ(float &dutZ, const AAlignConfig &alignConfig)
{
    // DUT Z alignment
    Result ret = m_findDistance.getBestDistance(dutZ, DutDistanceInfo(alignConfig.inView, alignConfig.distanceLimit, alignConfig.stepZ));
    if (!ret.success)
    {
        LoggingWrapper::instance()->info(
            QString("Input grating alignment Z failed, %1.").arg(QString::fromStdString(ret.errorMsg)));
        return ret;
    }
    LoggingWrapper::instance()->info(QString("Input grating alignment Z end, Z = %1.").arg(dutZ));

    return Result();
}

void MAalignmentLogic::continueRun(bool isContinue, InputProjectorPixel pixels)
{
    if(isContinue){
        m_pixels = pixels;
    }

    if (!isContinue)
    {
        Motion3DModel::getInstance(withCamera)->Motion3DMoveStop();
        Motion3DModel::getInstance(withDUT)->Motion3DMoveStop();

        AlignUtils::controlLight(false, false);
    }

    m_waitPause.continueRun(isContinue);
    emit notifyStopSignal(!isContinue);
    m_findDistance.continueRun(isContinue);
}

void MAalignmentLogic::notifyPause(bool isPause)
{
    m_waitPause.notifyPause(isPause);
    m_findDistance.notifyPause(isPause);
}

Result MAalignmentLogic::alignXY(const AAlignConfig &alignConfig)
{
    Result ret = AlignUtils::controlLight(true, true);
    if (!ret.success)
    {
        return ret;
    }

    if (alignConfig.autoMode){
        ret = autoAlignment(alignConfig);
        if (!ret.success && ret.errorCode == AA_AUTO_TO_MANUAL){
            ret = manualAlignment(alignConfig, true);
        }
    }else{
        ret = manualAlignment(alignConfig);
    }

    emit alignSignal(MALIGN_END, "");
    return ret;
}

Result MAalignmentLogic::autoAlignment(const AAlignConfig &alignConfig)
{
    // 1.capture DUT input image, projector move out image
    Result ret = AlignUtils::controlLight(false, true);
    if (!ret.success)
    {
        return ret;
    }

    ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    ret = AlignUtils::acsAbsoluteMove(alignConfig.outView);
    if (!ret.success)
    {
        return ret;
    }

    CameraModel::GetInstance()->SetMLExposureAuto();
    _sleep(300);
    cv::Mat inputImage = CameraModel::GetInstance()->GetImage();

    ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    cv::Point2f fiducialPixel;
    ret = FiducialRecognize::instance()->circleDetectInputFiducial(inputImage, fiducialPixel, alignConfig.fiducialTypeStr);
    if (!ret.success)
    {
        return Result(false,
                      QString("InputAA auto align failed, identify image error, %1.")
                          .arg(QString::fromStdString(ret.errorMsg))
                          .toStdString(),
                      AA_AUTO_TO_MANUAL);
    }

    cv::Point2f inputCenter;
    fiducialToInputPixel(alignConfig, fiducialPixel, inputCenter);
    if (!ret.success)
    {
        return ret;
    }

    //InputRe inputRet = mlDetect.getInputImgCoordinateRectangle(inputImage);
    //if (!inputRet.flag)
    //{
    //    return Result(false,
    //                  QString("InputAA auto align failed, identify image error, %1.")
    //                      .arg(QString::fromStdString(inputRet.errMsg))
    //                      .toStdString(),
    //                  AA_AUTO_TO_MANUAL);
    //}

    // 2. capture projector image, start calculate
    ret = AlignUtils::controlLight(true, false);
    if (!ret.success)
    {
        return ret;
    }

    ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    ret = AlignUtils::acsAbsoluteMove(alignConfig.inView);
    if (!ret.success)
    {
        return ret;
    }

    CameraModel::GetInstance()->SetMLExposureAuto();
    _sleep(300);

    cv::Point2f offsePoint = 100;
    cv::Point2f offseMtPoint;
    cv::Mat projectImage;

    int times = 0;
    while (times++ < 1)
    {
        ret = waitPause(m_waitPause.recipePause());
        if (!ret.success)
        {
            return ret;
        }

        projectImage = CameraModel::GetInstance()->GetImage();
        if (projectImage.empty())
        {
            return Result(false, "InputAA error, capture projector image is empty.");
        }

        cv::Point2f proInput;
        ret = FiducialRecognize::instance()->circleProjectorDetect(projectImage, proInput);
        if (!ret.success)
        {
            return Result(false, "InputAA error, The projector image detection faill.", AA_AUTO_TO_MANUAL);
        }

        //cv::Point2f proInput = mlDetect.getProjectImgCoordinate(projectImage);
        //if (proInput.x <= 0 || proInput.y <= 0)
        //{
        //    return Result(false, "InputAA error, The projector image detection faill.", AA_AUTO_TO_MANUAL);
        //}

        //{
        //    cv::Mat img_draw;
        //    cv::cvtColor(projectImage, img_draw, cv::COLOR_GRAY2RGB);
        //    circle(img_draw, proInput, 3, Scalar(0, 0, 255), -1, 8, 0);
        //    circle(img_draw, inputCenter, 4, Scalar(255, 0, 0), -1, 8, 0);
        //    qDebug() << "000 000 Test";
        //}

        offsePoint = proInput - inputCenter;
        offseMtPoint = offsePoint * PIXEL_SIZE / MAGNIFICATION / 1000.0;
        //LoggingWrapper::instance()->info(QString("AA auto alignment, offset X=%1, Y=%2.").arg(offseMtPoint.x).arg(offseMtPoint.y));
        emit updateOffset(offseMtPoint.x, offseMtPoint.y);

        if (qAbs(offsePoint.x) < OFFSET_PIXEL_MIN.x && qAbs(offsePoint.y) < OFFSET_PIXEL_MIN.y)
        {
            break;
        }

        ret = waitPause(m_waitPause.recipePause());
        if (!ret.success)
        {
            return ret;
        }

        ret = AlignUtils::acsRelMove(cv::Point2f(offseMtPoint.y, offseMtPoint.x));
        if (!ret.success)
        {
            return ret;
        }
    }

    //LoggingWrapper::instance()->info(QString("AA auto alignment end, offset X=%1, Y=%2.").arg(offseMtPoint.x).arg(offseMtPoint.y));
    //if (qAbs(offsePoint.x) > OFFSET_PIXEL_MIN.x && qAbs(offsePoint.y) > OFFSET_PIXEL_MIN.y)
    //{       
    //    return Result(false, "Auto align failed, offset large, please manual align.", AA_AUTO_TO_MANUAL);
    //}

    LoggingWrapper::instance()->info(QString("AA auto alignment end."));
    return Result();
}

Result MAalignmentLogic::manualAlignment(const AAlignConfig &alignConfig, bool auto2Manual)
{
    Result ret = AlignUtils::controlLight(true, true);
    if (!ret.success)
    {
        return ret;
    }

    ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    // 1.Move to init coordinate
    ret = AlignUtils::acsAbsoluteMove(alignConfig.inView);
    if (!ret.success)
    {
        return ret;
    }

    CameraModel::GetInstance()->SetMLExposureAuto();
    _sleep(300);

    if (auto2Manual)
    {
        emit alignSignal(MANUAL_START ,"Please calibrate inputgrating and projector manually, because auto alignment is failed.");
    }
    else
    {
        emit alignSignal(MANUAL_START, "Please calibrate inputgrating and projector manually, then select continue.");
    }

    Result result = waitPause(true);
    if (!result.success)
    {
        return result;
    }

    return moveOffset(alignConfig, 5);
}

Result MAalignmentLogic::moveOffset(const AAlignConfig &alignConfig, int maxNumber)
{
    InputProjectorPixel pixels = m_pixels;

    cv::Point2f offsePoint = pixels.projector - pixels.input;
    cv::Point2f offseMtPoint = offsePoint * PIXEL_SIZE / MAGNIFICATION / 1000.0;
    emit updateOffset(offseMtPoint.x, offseMtPoint.y);
    if (qAbs(offsePoint.x) < OFFSET_PIXEL_MIN.x && qAbs(offsePoint.y) < OFFSET_PIXEL_MIN.y)
    {
        return Result();
    }

    if (maxNumber <= 0)
    {
        return Result(false, "Inputgrating and projector alignment manually failed, align number is more than 5.");
    }

    Result ret = AlignUtils::acsRelMove(cv::Point2f(offseMtPoint.y, offseMtPoint.x));
    if (!ret.success)
    {
        return ret;
    }

    emit alignSignal(MANUAL_MIDDLE, "Please calibrate inputgrating and projector manually, then select continue.");

    Result result = waitPause(true);
    if (!result.success)
    {
        return result;
    }
    return moveOffset(alignConfig, --maxNumber);
}

Result MAalignmentLogic::fiducialToInputPixel(const AAlignConfig &alignConfig, cv::Point2f fiducialPixel, cv::Point2f& inputPixel)
{
    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
    if (dutConfig.fiducials.size() <= alignConfig.fiducialSn)
    {
        return Result(false, "InputAA auto align failed, fiducial number is less.");
    }
    cv::Point3f offsetFiducial3D = dutConfig.inputGrating - dutConfig.fiducials[alignConfig.fiducialSn];
    cv::Point2f offsetFiducial =
        cv::Point2f(offsetFiducial3D.x, offsetFiducial3D.y) * 1000.0 * MAGNIFICATION / PIXEL_SIZE;
    inputPixel = cv::Point2f(fiducialPixel.x + offsetFiducial.y, fiducialPixel.y + offsetFiducial.x);
    return Result();
}

Result MAalignmentLogic::waitPause(bool isPause)
{
    return m_waitPause.waitPause(isPause);
}

Result MAalignmentLogic::saveResultImage(bool flag)
{
    if(!flag){
        return Result();
    }

    if (!MetricsData::instance()->getAlignImageDir().isEmpty())
    {
        Result result = AlignUtils::controlLight(true, true);
        if (!result.success)
        {
            return result;
        }

        CameraModel::GetInstance()->SetMLExposureAuto();
        _sleep(300);
        cv::Mat image = CameraModel::GetInstance()->GetImage();

        int eyeType = MetricsData::instance()->getDutEyeType();
        QString eyeTypeStr = eyeType == 0 ? "left" : "right";

        QString fileName = MetricsData::instance()->getAlignImageDir() + "\\" +
            QString("input_%1_%2.png").arg(eyeTypeStr).arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
        bool ret = cv::imwrite(fileName.toStdString(), image);
        if(!ret){
            return Result(false, "Save input grating alignment image failed.");
        }
    }
    //QString dirName = MetricsData::instance()->getMTFImgsDir() + "alignment_image/";
    //QString fileName = "input_grating.tif";
    //cv::Mat image = CameraModel::GetInstance()->GetImage();

    //if (!image.empty())
    //{
    //    QDir dir(dirName);
    //    if (!dir.exists())
    //    {
    //        return Result(false, QString("Dir %1 is not exist.").arg(dirName).toStdString());
    //    }

    //    QString file = dirName + "\\" + fileName;
    //    bool ret = cv::imwrite(file.toStdString(), image);
    //    if(!ret){
    //        return Result(false, "Save input grating alignment image failed.");
    //    }
    //}
    return Result();
}

Result MAalignmentLogic::isConnected()
{
    // MV camera below
    if (!CameraModel::GetInstance()->isConnected())
    {
        return Result(false, "MV camera is not connectd.");
    }

    // Measure 3D move station
    if (!Motion3DModel::getInstance(withCamera)->Motion3DisConnected())
    {
        return Result(false, "Measure camera move 3D motion is not connectd.");
    }

    // DUT 3D move station
    if (!Motion3DModel::getInstance(withDUT)->Motion3DisConnected())
    {
        return Result(false, "DUT move 3D motion is not connectd.");
    }

    // DUT tilt 2D motion
    if (!Motion2DModel::getInstance(ACS2DDUT)->GetIsConnected())
    {
        return Result(false, "DUT tilt 2D motion is not connectd.");
    }

    // Projector tilt 2D motion
    if (!Motion2DModel::getInstance(ACS2DPro)->GetIsConnected())
    {
        return Result(false, "Projector tilt 2D motion is not connectd.");
    }

    // Reticle
    if (!ReticleMode2D::instance()->isConnected())
    {
        return Result(false, "Reticle is not connectd."); 
    }
    return Result();
}
