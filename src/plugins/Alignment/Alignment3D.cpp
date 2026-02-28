#include "Alignment3D.h"
#include "ImageManage.h"
//#include "MLAlgorithm/FiducialDetect.h"
#include"ImageDetection/FiducialDetect.h"
#include "pluginsystem/Services.h"
#include "Core/loggingwrapper.h"
#include <QMutex>
#include <QThread>
#include <QVariant>
#include <QVector>
#include "PrjCommon/metricsdata.h"
#include <QDir>
#include "CaptureFiducialImage.h"
#include"ImageDetection/ml_AF.h"
//#include "MLAlgorithm/ml_ImageQuality.h"

#include "MLKeyenceLRF/MLKeyenceCL.h"
#include "piMotionActor/Motion2DModel.h"
#include "MAalignment/FiducialRecognize.h"
#include "piMotionActor/FiducialConfig.h"

using namespace Align;
using namespace MLImageDetection;

Alignment3D *Alignment3D::m_instance = nullptr;

Alignment3D *Alignment3D::instance(QObject *parent)
{
    if (!m_instance)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!m_instance)
        {
            m_instance = new Alignment3D(parent);
        }
    }
    return m_instance;
}

Alignment3D::Alignment3D(QObject *parent)
{
    QHash<QString, QVariant> props;
    props.insert("mv", "vimba");
    m_pCamera = ExtensionSystem::Internal::ServicesManger::getService<MLCamera>("CORE::MLCamera", props);

    MoveLimit::instance()->subscribe(this);
}

Alignment3D::~Alignment3D()
{
}

void Align::Alignment3D::notifyInputAAStateChanged(bool isAlign)
{
    m_eyeboxAcsCoords.clear();
}

Result Alignment3D::i29AlignEyebox(cv::Point3f acsCoord, const CORE::CoordinateLimit &acsCoordLimit)
{
    Result ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    return AlignUtils::acsAbsoluteMove(acsCoord, acsCoordLimit);
}

Result Alignment3D::i29AlignEyebox(cv::Point3f acsCoord, const QList<CORE::CoordinateLimit> &acsCoordLimitList)
{
    Result ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    return AlignUtils::acsAbsoluteMove(acsCoord, acsCoordLimitList);
}

Result Align::Alignment3D::moveAlignEyebox(int index)
{
    if (m_eyeboxAcsCoords.size() == 0)
    {
        Result result = Result(false, "Eyebox Acs coordinates is null, need to calibrate.");
        LoggingWrapper::instance()->error(QString::fromStdString(result.errorMsg));
        return result;
    }
    else if (m_eyeboxAcsCoords.size() <= index)
    {
        Result result = Result(false, QString("Eyebox Acs coordinates size less than index, index is %1, Eyebox Acs "
                                              "coordinates size is %2, check recipe file.")
                                          .arg(index)
                                          .arg(m_eyeboxAcsCoords.size())
                                          .toStdString());
        LoggingWrapper::instance()->error(QString::fromStdString(result.errorMsg));
        return result;
    }

    if (!MoveLimit::instance()->completeInputAA())
    {
        QString msg = "Move align eyebox failed, need first align input grating.";
        LoggingWrapper::instance()->error(msg);
        return Result(false, msg.toStdString());
    }

    cv::Point3f acsCoord = m_eyeboxAcsCoords.at(index);
    Result result = MoveLimit::judgeEyeboxLimit(acsCoord, index+1);
    if (!result.success)
    {
        return result;
    }

    // TODO:
    AlignBaseInfo info = AlignUtils::getAlignBaseInfo();
    // Result result = i29AlignEyebox(acsCoord, info.acsCoordLimit);
    result = i29AlignEyebox(acsCoord, info.acsCoordLimitList);
    if (!result.success)
    {
        emit moveAlignEyeboxSignal(-1);
        LoggingWrapper::instance()->error(QString::fromStdString(result.errorMsg));
    }
    else
    {
        MetricsData::instance()->setEyeboxIndexCurrent(index + 1);
        emit moveAlignEyeboxSignal(index);
        LoggingWrapper::instance()->info(QString("Alignment3D eyebox %1 success.ACS X:%2 Y:%3 Z :%4")
                                             .arg(index + 1)
                                             .arg(acsCoord.x)
                                             .arg(acsCoord.y)
                                             .arg(acsCoord.z));
    }
    return result;
}

Result Align::Alignment3D::alignEyebox(AlignBaseInfo3D *alignInfo, const EyeReliefBaseInfo &eyeReliefInfo,
                                       AlignConfig alignConfig)
{
    Result result = waitPause(m_waitPause.recipePause());
    if (!result.success)
    {
        return result;
    }

    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
    LoggingWrapper::instance()->debug(QString("Align eyebox start, DUT SN %1, Eye type %2.")
                                         .arg(dutConfig.sn)
                                         .arg(dutConfig.eyeType == 0 ? "left" : "right"));

    result = isConnected();
    if (!result.success)
    {
        LoggingWrapper::instance()->error("Align eyebox failed," + QString::fromStdString(result.errorMsg));
        return result;
    }

    if (!MoveLimit::instance()->completeInputAA())
    {
        QString msg = "Align eyebox failed, need first align input grating.";
        LoggingWrapper::instance()->error(msg);
        return Result(false, msg.toStdString());
    }

    bool isCoaxialLight = abs(alignInfo->dutAngleXY.x) < 0.01 && abs(alignInfo->dutAngleXY.y) < 0.01;
    result = AlignUtils::fiducialLight(true,isCoaxialLight);
    if (!result.success)
    {
        LoggingWrapper::instance()->error("Align eyebox failed," + QString::fromStdString(result.errorMsg));
        return result;
    }

    result = waitPause(m_waitPause.recipePause());
    if (!result.success)
    {
        return result;
    }

    result = MoveLimit::moveBeforeInputAA();
    if (!result.success)
    {
        LoggingWrapper::instance()->info(
            QString("Align eyebox failed, %1.").arg(QString::fromStdString(result.errorMsg)));
        return result;
    }

    result = waitPause(m_waitPause.recipePause());
    if (!result.success)
    {
        return result;
    }

    result = moveInitPosition(alignInfo->sn);
    if (!result.success)
    {
        LoggingWrapper::instance()->error("Align eyebox failed, " + QString::fromStdString(result.errorMsg));
        return result;
    }

    result = MoveLimit::instance()->setMotion3DMoveAlignSpeed(true);
    if (!result.success)
    {
        LoggingWrapper::instance()->error("Align eyebox failed, " + QString::fromStdString(result.errorMsg));
        return result;
    }

    result = alignEyeboxAcsXYZ(alignInfo, eyeReliefInfo, alignConfig);
    if (!result.success)
    {
        MoveLimit::instance()->setMotion3DMoveAlignSpeed(false);
        LoggingWrapper::instance()->error("Align eyebox failed," + QString::fromStdString(result.errorMsg));
        return result;
    }

    emit alignEyeboxEndSignal(true);

    {
        for (int i = 0; i <  m_eyeboxAcsCoords.size(); i++)
        {
            LoggingWrapper::instance()->info(QString("Eyebox AA alignment result, eyebox=%1, coordinate is X=%2, Y=%3, Z=%4.")
                                                 .arg(i + 1)
                                                 .arg(m_eyeboxAcsCoords[i].x)
                                                 .arg(m_eyeboxAcsCoords[i].y)
                                                 .arg(m_eyeboxAcsCoords[i].z));
        }
    }

    MoveLimit::instance()->setMotion3DMoveAlignSpeed(false);

    result = AlignUtils::fiducialLight(false, isCoaxialLight);
    if (!result.success)
    {
        LoggingWrapper::instance()->error("Align eyebox failed," + QString::fromStdString(result.errorMsg));
    }
    else
    {
        LoggingWrapper::instance()->debug(QString("Align eyebox success, DUT SN %1, Eye type %2.")
                                             .arg(dutConfig.sn)
                                             .arg(dutConfig.eyeType == 0 ? "left" : "right"));
    }

    result = MoveLimit::judgeEyeboxLimit(m_eyeboxAcsCoords);
    if (!result.success)
    {
        LoggingWrapper::instance()->error("Align eyebox errror, judge eyebox limit error, " + QString::fromStdString(result.errorMsg));
    }
    return result;
}

Result Align::Alignment3D::alignEyeboxAcsXYZ(AlignBaseInfo3D *alignInfo, const EyeReliefBaseInfo &eyeReliefInfo,
                                           AlignConfig alignConfig)
{
    AlignBaseInfo3D *alignInfo3D = (AlignBaseInfo3D *)alignInfo;

    Result result = CaptureFiducialImage().calMvCaptureFiducialCoords(alignInfo3D->acsFiducials, alignInfo3D->sn,                                                         alignConfig.calculateMvFidsByInputFlag);
    if (!result.success)
    {
        return result;
    }

    result = judgeDataValidity(*alignInfo3D, eyeReliefInfo, alignConfig);
    if (!result.success)
    {
        return result;
    }

    result = waitPause(m_waitPause.recipePause());
    if (!result.success)
    {
        return result;
    }

    // DUT move out
    if (alignConfig.moveOut)
    {
        result = AlignUtils::dutMoveOutInputgratingOffset(true, alignInfo3D->dutOutOffset, alignInfo3D->dutCoordLimit);
        if (!result.success)
        {
            return result;
        }
    }

    FiducialConfig::instance()->setFiducialCenter(false);
    // Use fiducial motion position in "FiducialConfig.json"
    if (FiducialConfig::instance()->fiduciualMotionEnabled())
    {
        std::vector<cv::Point3f> acsFiducialsNew = alignInfo3D->acsFiducials;
        result = FiducialConfig::instance()->getFiduciualMotionPosList(alignConfig.fiducialTypeStr, acsFiducialsNew);
        if (result.success)
        {
            FiducialConfig::instance()->setFiducialCenter(true);
            alignInfo3D->acsFiducials = acsFiducialsNew;
            LoggingWrapper::instance()->info("Use motion3D position in fiducial config json.");
        }else{
            LoggingWrapper::instance()->info("Use calculate fiducial motion3D position, " + QString::fromStdString(result.errorMsg));
        }
    }
    else
    {
        LoggingWrapper::instance()->info("Use calculate fiducial motion3D position.");
    }

    m_eyeboxAcsCoords.clear();
    result = alignEyeboxAcsXYZ(m_eyeboxAcsCoords, *alignInfo3D, eyeReliefInfo, alignConfig);
    if (!result.success)
    {
        return result;
    }

    result = waitPause(m_waitPause.recipePause());
    if (!result.success)
    {
        return result;
    }

    // Move safe position
    result = MoveLimit::moveRel(cv::Point3f(10, -20, -20), withCamera);
    if(!result.success)
    {
        return result;
    }

    // DUT move in
    if (alignConfig.moveOut)
    {
        m_eyeboxAcsCoords = AlignUtils::eyeboxAddDutOffset(m_eyeboxAcsCoords, alignInfo3D->dutOutOffset);
        result = AlignUtils::dutMoveOutInputgratingOffset(false, alignInfo3D->dutOutOffset, alignInfo3D->dutCoordLimit);
    }

    // All eyeboxs Z are based on the center
    //for (cv::Point3f &point : m_eyeboxAcsCoords)
    //{
    //    point.z = m_eyeboxAcsCoords[0].z;
    //}

    return result;
}

Result Alignment3D::alignEyeboxAcsXYZ(std::vector<cv::Point3f> &acsCoords, const AlignBaseInfo3D &alignInfo,
                                    const EyeReliefBaseInfo &eyeReliefInfo, AlignConfig alignConfig)
{
    Result ret; 
    std::vector<cv::Point3f> fiducialCoords;
    bool isAuto = alignConfig.autoMode;
    float distanceKeyenceDut = 0;
    vector<cv::Point3f> mcMotions;
    for (int i = 0; i < alignInfo.acsFiducials.size(); ++i)
    {
        ret = waitPause(m_waitPause.recipePause());
        if (!ret.success)
        {
            return ret;
        }

        cv::Point3f coord = alignInfo.acsFiducials.at(i);
        ret = AlignUtils::acsAbsoluteMove(coord, alignInfo.acsCoordLimitList);
        if (!ret.success)
        {
            return ret;
        }

        ret = waitPause(m_waitPause.recipePause());
        if (!ret.success)
        {
            return ret;
        }

        ret = mvAutoFocus(alignConfig.mvExpose);
        if (!ret.success)
        {
            return ret;
        }

        cv::Point2f pixel;
        ret = fiducialCenter(pixel, alignInfo, alignConfig, i);
        if (!ret.success)
        {
            return ret;
        }
		//saveFiducialImage(pixel, MetricsData::instance()->getMTFImgsDir() + "alignment_image/", i);
        saveFiducialImage(pixel, alignConfig.autoMode, i);

        {
            cv::Point3f currentCoord;
            ret = AlignUtils::getAcsCoordinate(currentCoord);
            mcMotions.push_back(currentCoord);
            if (!ret.success)
            {
                return ret;
            }

            EyeReliefBaseInfo eyeReliefInfoTmp =  eyeReliefInfo;
            eyeReliefInfoTmp.acsCoordinate = m_distanceManage.keyenceAcsByMv(alignInfo, eyeReliefInfo, currentCoord);
            ret = m_distanceManage.fiducialSameDistance(distanceKeyenceDut, eyeReliefInfoTmp);
            if (!ret.success)
            {
                return ret;
            }
            
            ret = waitPause(m_waitPause.recipePause());
            if (!ret.success)
            {
                return ret;
            }
        }

        cv::Point3f currentCoord;
        ret = AlignUtils::getAcsCoordinate(currentCoord);
        if (!ret.success)
        {
            return ret;
        }
        fiducialCoords.push_back(currentCoord);
    }
 
    //TODO: print log
    for (int i = 0; i < fiducialCoords.size(); i++)
    {
        LoggingWrapper::instance()->info(
            QString("Eyebox AA alignment, fiducial=%1, keyence align fiducial is X=%2, Y=%3, Z=%4.")
                .arg(i)
                .arg(fiducialCoords[i].x)
                .arg(fiducialCoords[i].y)
                .arg(fiducialCoords[i].z));
    }

    // keyence to lens (x, y), set z distance to eye relief
    fiducialCoords = m_distanceManage.keyenceToLens(eyeReliefInfo, fiducialCoords, distanceKeyenceDut);

    // call algorithm: dut fiducials, dut eyeboxs, eye relief, fiducial acs coords
    //std::vector<cv::Point3f> eyeboxList = alignInfo.eyeboxs;
    //TODO: to be done , Z offset set 0  ------0000
    std::vector<cv::Point3f> eyeboxList;
    for (cv::Point3f point: alignInfo.eyeboxs)
    {
        eyeboxList.push_back(cv::Point3f(point.x, point.y, 0));
    }
    FiducialBase fiducialBase;
    fiducialBase.fiudcialWafer = alignInfo.fiducials;
    fiducialBase.fiducialACS = fiducialCoords;
    EyeboxACS eyeboxACS = MLRigidTrans3D().getEyboxLocation(&fiducialBase, eyeboxList);
    if(eyeboxACS.eyebox.size() == 0){
        return Result(false, "Calculate eyebox acs coordinates is NULL."); 
    }

    MetricsData::instance()->setDutAngle(eyeboxACS.eulerAngle.z);
    LoggingWrapper::instance()->info(QString("Eyebox alignment, calculate DUT angle(x,y,z)=(%1,%2,%3)")
                                         .arg(eyeboxACS.eulerAngle.x)
                                         .arg(eyeboxACS.eulerAngle.y)
                                         .arg(eyeboxACS.eulerAngle.z));

    // TODO: to be done , Z offset add  ------0000
    acsCoords.clear();
    for (int i = 0; i < alignInfo.eyeboxs.size(); ++i){
        acsCoords.push_back(cv::Point3f(eyeboxACS.eyebox[i].x, eyeboxACS.eyebox[i].y, eyeboxACS.eyebox[i].z + alignInfo.eyeboxs[i].z));
    }
    //acsCoords = eyeboxACS.eyebox;

    // Write MC motion3D relative DUT motion3D coordinate when MV see fiduciual
    cv::Point3f dutMotion;
    MoveLimit::instance()->getCoordinate(withDUT, dutMotion);
    FiducialConfig::instance()->writeFiduciualMotionPosRelativeDut(alignConfig.fiducialTypeStr, dutMotion, mcMotions);
    return Result();
}

Result Align::Alignment3D::alignEyeboxAcsXYZ_old(std::vector<cv::Point3f> &acsCoords, const AlignBaseInfo3D &alignInfo,
                                                 const EyeReliefBaseInfo &eyeReliefInfo, AlignConfig alignConfig)
{
    Result ret;
    std::vector<cv::Point3f> fiducialCoords;
    bool isAuto = alignConfig.autoMode;
    for (int i = 0; i < alignInfo.acsFiducials.size(); ++i)
    {
        ret = waitPause(m_waitPause.recipePause());
        if (!ret.success)
        {
            return ret;
        }

        cv::Point3f coord = alignInfo.acsFiducials.at(i);
        ret = AlignUtils::acsAbsoluteMove(coord, alignInfo.acsCoordLimitList);
        if (!ret.success)
        {
            return ret;
        }

        ret = waitPause(m_waitPause.recipePause());
        if (!ret.success)
        {
            return ret;
        }

        ret = mvAutoFocus(alignConfig.mvExpose);
        if (!ret.success)
        {
            return ret;
        }

        cv::Point2f pixel;
        ret = fiducialCenter(pixel, alignInfo, alignConfig, i);
        if (!ret.success)
        {
            return ret;
        }
        saveFiducialImage(pixel, alignConfig.autoMode, i);

        cv::Point3f currentCoord;
        ret = AlignUtils::getAcsCoordinate(currentCoord);
        if (!ret.success)
        {
            return ret;
        }
        fiducialCoords.push_back(currentCoord);
    }

    // call algorithm: dut fiducials, dut eyeboxs, eye relief, fiducial acs coords
    FiducialBase fiducialBase;
    fiducialBase.fiudcialWafer = alignInfo.fiducials;
    fiducialBase.fiducialACS = fiducialCoords;
    EyeboxACS eyeboxACS = MLRigidTrans3D().getEyboxLocation(&fiducialBase, alignInfo.eyeboxs);
    if (eyeboxACS.eyebox.size() == 0)
    {
        return Result(false, "Calculate eyebox acs coordinates is NULL.");
    }

    eyeboxACS.eyebox = AlignUtils::mvToMeasureCameraCoordXY(eyeboxACS.eyebox, alignInfo.i29RelativeMvCoord);

    // calculate Z
    EyeReliefBaseInfo eyeInfo = eyeReliefInfo;
    ret = m_distanceManage.keyenceAcsByLens(eyeInfo.acsCoordinate, alignInfo, eyeReliefInfo,
                                                   eyeboxACS.eyebox.at(0));
    if (!ret.success)
    {
        return ret;
    }

    double acsZ;
    ret = m_distanceManage.eyeRelief(eyeInfo, acsZ);
    if (!ret.success)
    {
        return ret;
    }
    LoggingWrapper::instance()->info("Calibrate eye-relief end.");

    for (cv::Point3f coord : eyeboxACS.eyebox)
    {
        acsCoords.push_back(cv::Point3f(coord.x, coord.y, acsZ));
    }
    return Result();
}

void Alignment3D::continueRun(bool isContinue, cv::Point2f pixel)
{
    m_distanceManage.continueRun(isContinue);

    if (isContinue)
    {
        m_fiducialPixel = pixel;
    }

    if (!isContinue)
    {
        Motion3DModel::getInstance(withCamera)->Motion3DMoveStop();
        Motion3DModel::getInstance(withDUT)->Motion3DMoveStop();

        AlignUtils::fiducialLight(false);
    }

    m_waitPause.continueRun(isContinue);
    emit notifyStopSignal(!isContinue);
}

void Align::Alignment3D::notifyPause(bool isPause)
{
    m_waitPause.notifyPause(isPause);
    m_distanceManage.notifyPause(isPause);
}

bool Align::Alignment3D::isStop()
{
    return m_waitPause.isStop();
}

Result Alignment3D::autoToManualFiducial(cv::Mat image, MvExpose mvExpose, cv::Point2f &pixel)
{
    ImageManage().saveFiducialErrorImage(image);
    return manualFiducial(pixel, mvExpose, true);
}

Result Alignment3D::manualFiducial(cv::Point2f &pixel, MvExpose mvExpose, bool isAutoFiducial)
{
    // MV Expose
    if (mvExpose.isAuto)
    {
        m_pCamera->SetMLExposureAuto();
    }
    else
    {
        m_pCamera->SetExposureTime(mvExpose.manualExposeTime * 1000);
    }

    if (isAutoFiducial)
    {
        emit messageBoxSignal("Please select fiducial manually, because fiducial can't be recognized by image.");
    }
    else
    {
        emit messageBoxSignal("Please select fiducial manually then click the confirm button.");
    }

    Result result = waitPause(true);
    if (!result.success)
    {
        return result;
    }

    if (m_fiducialPixel == cv::Point2f())
    {
        return Result(false, "Manual calibrate fiducial error, fiducial pixel map is null.");
    }
    pixel = m_fiducialPixel;
    return Result();
}

Result Align::Alignment3D::waitPause(bool isPause)
{
    return m_waitPause.waitPause(isPause);
}

Result Align::Alignment3D::judgeDataValidity(const AlignBaseInfo3D &alignInfo, const EyeReliefBaseInfo &eyeReliefInfo,
                                           const AlignConfig &alignConfig)
{
    Result result = judgeDataValidity(alignInfo, alignConfig);
    if (!result.success)
    {
        return result;
    }

    if (!eyeReliefInfo.judgeDataValidity())
    {
        return Result(false, "Eye relief base info is incomplete.");
    }
    return Result();
}

Result Align::Alignment3D::judgeDataValidity(const AlignBaseInfo3D &alignInfo, const AlignConfig &alignConfig)
{
    if (!alignInfo.judgeDataValidity())
    {
        return Result(false, "Align base info is incomplete.");
    }

    if (alignInfo.fiducials.size() < alignConfig.fiducialCount)
    {
        return Result(false, "Align base info fiducials count is mismatching config.");
    }

    if (alignInfo.acsFiducials.size() < alignConfig.fiducialCount)
    {
        return Result(false, "Align base info fiducials count is mismatching config.");
    }
    return Result();
}

Result Align::Alignment3D::fiducialCenter(cv::Point2f &pixel, const AlignBaseInfo3D &alignInfo, AlignConfig alignConfig, int fiducialNo)
{
    Result ret;
    for (int i = 0; i < FIDUCIAL_MANUAL_MOVE_COUNT_MAX + 1; ++i)
    {
        ret = waitPause(m_waitPause.recipePause());
        if (!ret.success)
        {
            return ret;
        }

        if (alignConfig.autoMode)
        {
            ret = autoIdentifyFiducial(pixel, alignConfig.fiducialTypeStr, alignConfig.mvExpose, i, fiducialNo);
            if (!ret.success)
            {
                return ret;
            }
        }else{
            ret = manualFiducial(pixel, alignConfig.mvExpose, false);
            if (!ret.success)
            {
                return ret;
            }
        }

        cv::Point2f offset = MvImage::pixelConvert(cv::Point2f(pixel.x - MvImage::center.x, pixel.y - MvImage::center.y));
        if (i > 0 && abs(offset.x) < FIDUCIAL_ERROR_MAX.x && abs(offset.y) < FIDUCIAL_ERROR_MAX.y)
        {
            return Result();
        }

        if (i == FIDUCIAL_MANUAL_MOVE_COUNT_MAX)
        {
            return Result(false, QString("Move %1 times, but Fiducial is still not in the center.")
                                     .arg(FIDUCIAL_MANUAL_MOVE_COUNT_MAX)
                                     .toStdString());
        }

        ret = waitPause(m_waitPause.recipePause());
        if (!ret.success)
        {
            return ret;
        }

        Result ret = fiducialCenterMove(alignInfo, pixel);
        if (!ret.success)
        {
            return ret;
        }
    }
    return Result(false, "Aligment fiducial center is failed.");
}

Result Align::Alignment3D::autoIdentifyFiducial(cv::Point2f &pixel, std::string fiducialType, MvExpose mvExpose, int number, int fiducialNo)
{
    // MV Expose
    if (mvExpose.isAuto)
    {
        m_pCamera->SetMLExposureAuto();
    }
    else
    {
        m_pCamera->SetExposureTime(mvExpose.manualExposeTime*1000);
    }
    QThread::msleep(1500);

    cv::Mat img = m_pCamera->GetImage();
    if (img.empty())
    {
        return Result(false, "Auto identify fiducial failed, MV camera get fiducial image null.");
    }

    FiducialInfo fiducialInfo(fiducialType, fiducialNo, number > 0);
    Result ret = FiducialRecognize::instance()->circleDetect(img, pixel, fiducialInfo);
    if (!ret.success)
    {
        Result ret = autoToManualFiducial(img, mvExpose, pixel);
        if (!ret.success)
        {
            return ret;
        }
    }
    else
    {
        cv::Mat fidcialDraw = FiducialRecognize::instance()->getImgDraw();
        ImageManage().saveFiducialIdentifyImage(fidcialDraw);
    }
    return Result();
}

Result Align::Alignment3D::fiducialCenterMove(const AlignBaseInfo3D &alignInfo, cv::Point2f pixel)
{
    cv::Point2f offset = MvImage::pixelConvert(cv::Point2f(pixel.x - MvImage::center.x, pixel.y - MvImage::center.y));
    
    //TODO: maybe not need, to be tested
    //{
    //    float angleOffsetX = alignInfo.mvFocus * tan(alignInfo.dutAngleXY.x * PI / 180.0);
    //    float angleOffsetY = alignInfo.mvFocus * tan(alignInfo.dutAngleXY.y * PI / 180.0);
    //    offset = cv::Point2f(offset.x - angleOffsetX, offset.y - angleOffsetY);
    //}

    cv::Point3f acsCurrentCoord;
    Result ret = AlignUtils::getAcsCoordinate(acsCurrentCoord);
    if (!ret.success)
    {
        return ret;
    }

    ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    // TODO: + - offset, need test
    acsCurrentCoord = cv::Point3f(acsCurrentCoord.x + offset.y, acsCurrentCoord.y - offset.x, acsCurrentCoord.z);
    ret = AlignUtils::acsAbsoluteMove(acsCurrentCoord, alignInfo.acsCoordLimitList);
    if (!ret.success)
    {
        return ret;
    }
    return Result();
}

Result Align::Alignment3D::saveFiducialImage(cv::Point2f pixel, bool autoMode, int fiducialNumber)
{
    //QString dirName = MetricsData::instance()->getMTFImgsDir() + "alignment_image/";
    //cv::Mat img_draw = m_pCamera->GetImage();
    //MLimagePublic pl;
    //img_draw = pl.convertTo3Channels(img_draw);
    //cv::circle(img_draw, pixel, 5, Scalar(0, 0, 255), 1);
    //return saveImage(img_draw, dirName, QString("fiducial_%1.tif").arg(fiducialNumber + 1));

    // save image
    if (!MetricsData::instance()->getAlignImageDir().isEmpty())
    {
        cv::Mat img_draw = m_pCamera->GetImage();
        MLimagePublic pl;
        img_draw = pl.convertTo3Channels(img_draw);
        cv::circle(img_draw, pixel, 5, cv::Scalar(0, 0, 255), -1);

        int eyeType = MetricsData::instance()->getDutEyeType();
        QString eyeTypeStr = eyeType == 0 ? "left" : "right";

        QString fileName = QString("fiducial_%1_%2_%3.png")
                               .arg(eyeTypeStr)
                               .arg(fiducialNumber + 1)
                               .arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
        //if (autoMode)
        //{
        //    fileName = QString("auto_fiducial_%1_%2.png")
        //                   .arg(fiducialNumber + 1)
        //                   .arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
        //}
        //else
        //{
        //    fileName = QString("manual_fiducial_%1_%2.png")
        //                   .arg(fiducialNumber + 1)
        //                   .arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
        //}
        return saveImage(img_draw, MetricsData::instance()->getAlignImageDir(), fileName);
    }
    return Result();
}

Result Align::Alignment3D::saveImage(cv::Mat image, QString dirName, QString fileName)
{
    if (!image.empty())
    {
        QDir dir(dirName);
        if (!dir.exists())
        {
            return Result(false, QString("Dir %1 is not exist.").arg(dirName).toStdString());
        }

        QString file = dirName + "\\" + fileName;
        cv::imwrite(file.toStdString(), image);
    }
    return Result();
}

Result Align::Alignment3D::mvAutoFocus(MvExpose mvExpose)
{
    //TODO:
    return Result();
    //if (mvExpose.isAuto)
    //{
    //    m_pCamera->SetMLExposureAuto();
    //}
    //else
    //{
    //    m_pCamera->SetExposureTime(mvExpose.manualExposeTime*1000);
    //}

    m_pCamera->SetMLExposureAuto();
    double eTime = m_pCamera->GetExposureTime();
    m_pCamera->SetExposureTime(eTime - eTime*0.0);
    //m_pCamera->SetExposureTime(500*1000);


    cv::Point3f initCoord;
    Result ret = MoveLimit::getCoordinate(withCamera, initCoord);
    if (!ret.success)
    {
        return ret;
    }

    bool isEnd = false;
    // 1. to down
    ret = mvAutoFocusStep(isEnd, true);
    if (!ret.success)
    {
        return ret;
    }

    if (isEnd)
    {
        // 3.reset MV expose
        if (mvExpose.isAuto)
        {
            m_pCamera->SetMLExposureAuto();
        }
        else
        {
            m_pCamera->SetExposureTime(mvExpose.manualExposeTime * 1000);
        }
        return Result();
    }

    ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    // 2. to up 
    ret = MoveLimit::moveAbs(initCoord, withCamera);
    if (!ret.success)
    {
        return ret;
    }

    //TODO:
    ret = mvAutoFocusStep(isEnd, false);
    if (!ret.success)
    {
        return ret;
    }

    // 3.reset MV expose
    if (mvExpose.isAuto)
    {
        m_pCamera->SetMLExposureAuto();
    }
    else
    {
        m_pCamera->SetExposureTime(mvExpose.manualExposeTime * 1000);
    }
    return Result();
}

Result Align::Alignment3D::mvAutoFocusStep(bool &isEnd, bool down)
{
    QThread::msleep(2000);

    cv::Point3f initCoord;
    Result ret = MoveLimit::getCoordinate(withCamera, initCoord);
    if (!ret.success)
    {
        return ret;
    }
    cv::Point3f bestCoord = initCoord;

    cv::Mat img = m_pCamera->GetImage();
    //double maxQuality = MLImageDetection::MLImageQuality().tenengard(img);
    double maxQuality = MLImageDetection::AutoFocus().tenengard(img);

    float step = 1;
    int moveNumber = 8.0 / step;
    isEnd = false;

    int compareFreq = 0;
    for (int i = 0; i < moveNumber; i++)
    {
        ret = waitPause(m_waitPause.recipePause());
        if (!ret.success)
        {
            return ret;
        }

        if (down)
        {
            ret = MoveLimit::moveRel(cv::Point3f(0, 0, -step), withCamera);
        }
        else
        {
            ret = MoveLimit::moveRel(cv::Point3f(0, 0, step), withCamera);
        }

        if (!ret.success)
        {
            return ret;
        }

        QThread::msleep(500);
        img = m_pCamera->GetImage();

       // double iqValue = algorithm::MLImageQuality().tenengard(img);
        double iqValue = MLImageDetection::AutoFocus().tenengard(img);

        if (maxQuality > iqValue || abs(maxQuality - iqValue) < 0.01)
        {
            compareFreq++;
        }
        else
        {
            ret = MoveLimit::getCoordinate(withCamera, bestCoord);
            if (!ret.success)
            {
                return ret;
            }

            maxQuality = iqValue;
            compareFreq = 0;
        }

        if (compareFreq >= 3)
        {
            isEnd = true;
            break;
        }
    }

    if (down && abs(initCoord.z - bestCoord.z) <= 0.01)
    {
        isEnd = false;
    }

    if (abs(initCoord.z - bestCoord.z) >= step)
    {
        isEnd = true;
    }

    ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    if (isEnd)
    {
        ret = MoveLimit::moveAbs(bestCoord, withCamera);
        if (!ret.success)
        {
            return ret;
        }
    }

    return Result();
}

Result Align::Alignment3D::isConnected()
{
    // MV camera below
    if (m_pCamera == nullptr || !m_pCamera->IsOpened())
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

    // Keyence
    if (!MLKeyenceCL::MakeRangeFinder()->IsConnected())
    {
        return Result(false, "Confocal Distance Sensor is not connectd.");
    }

    // DUT tilt 2D motion
    if (!Motion2DModel::getInstance(ACS2DDUT)->GetIsConnected())
    {
        return Result(false, "DUT tilt 2D motion is not connectd.");
    }
    return Result();
}

Result Align::Alignment3D::moveInitPosition(const QString &dutSn)
{
    //TODO:test
    return Result();

    cv::Point3f coord;
    Result ret = CaptureFiducialImage().getInitMoveCoordinate(dutSn, coord);
    if(!ret.success){
        return ret;
    }

    ret = MoveLimit::mcMoveAbs(coord);
    if(!ret.success){
        return Result(false, "Move init position error, " + ret.errorMsg);
    }
    return ret;
}

std::vector<cv::Point3f> Align::Alignment3D::getEyeboxAcsCoords()
{
    //hxw test
    /*m_eyeboxAcsCoords.clear();
    m_eyeboxAcsCoords.push_back(cv::Point3f(1, 1, 1));
    m_eyeboxAcsCoords.push_back(cv::Point3f(2, 2, 2));
    m_eyeboxAcsCoords.push_back(cv::Point3f(3, 3, 3));
    m_eyeboxAcsCoords.push_back(cv::Point3f(4, 4, 4));
    m_eyeboxAcsCoords.push_back(cv::Point3f(5, 5, 5));*/

    return m_eyeboxAcsCoords;
}

