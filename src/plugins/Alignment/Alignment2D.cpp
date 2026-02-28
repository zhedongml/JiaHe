#include "Alignment2D.h"
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
#include "MLKeyenceLRF/MLKeyenceCL.h"
#include "piMotionActor/Motion2DModel.h"
#include "MAalignment/FiducialRecognize.h"
#include "piMotionActor/FiducialConfig.h"
#include "DistanceCheck.h"

using namespace Align;
using namespace MLImageDetection;

Alignment2D* Alignment2D::instance(QObject* parent)
{
    static Alignment2D self;
    return &self;
}

Alignment2D::Alignment2D(QObject* parent)
{
    QHash<QString, QVariant> props;
    props.insert("mv", "vimba");
    m_pCamera = ExtensionSystem::Internal::ServicesManger::getService<MLCamera>("CORE::MLCamera", props);
}

Alignment2D::~Alignment2D()
{
}

void Align::Alignment2D::notifyInputAAStateChanged(bool isAlign)
{
    m_eyeboxAcsCoords.clear();
}

Result Alignment2D::i29AlignEyebox(cv::Point3f acsCoord, const CORE::CoordinateLimit& acsCoordLimit)
{
    Result ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    return AlignUtils::acsAbsoluteMove(acsCoord, acsCoordLimit);
}

Result Alignment2D::i29AlignEyebox(cv::Point3f acsCoord, const QList<CORE::CoordinateLimit>& acsCoordLimitList)
{
    Result ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    return AlignUtils::acsAbsoluteMove(acsCoord, acsCoordLimitList);
}

Result Align::Alignment2D::moveAlignEyebox(int index)
{
    Result result = MoveLimitJX::instance()->resetLastInputAA();
    if(!result.success){
        return result;
    }

    if (m_eyeboxAcsCoords.size() == 0)
    {
        result = Result(false, "Eyebox Acs coordinates is null, need to calibrate.");
        LoggingWrapper::instance()->error(QString::fromStdString(result.errorMsg));
        return result;
    }
    else if (m_eyeboxAcsCoords.size() <= index)
    {
        result = Result(false, QString("Eyebox Acs coordinates size less than index, index is %1, Eyebox Acs "
            "coordinates size is %2, check recipe file.")
            .arg(index)
            .arg(m_eyeboxAcsCoords.size())
            .toStdString());
        LoggingWrapper::instance()->error(QString::fromStdString(result.errorMsg));
        return result;
    }

    cv::Point3f acsCoord = m_eyeboxAcsCoords.at(index);
    result = MoveLimitJX::judgeEyeboxLimit(acsCoord, index + 1);
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
        LoggingWrapper::instance()->info(QString("Alignment eyebox %1 success.ACS X:%2 Y:%3 Z :%4")
            .arg(index + 1)
            .arg(acsCoord.x)
            .arg(acsCoord.y)
            .arg(acsCoord.z));
    }
    return result;
}

Result Align::Alignment2D::alignEyebox(AlignBaseInfo2D* alignInfo, AlignConfig alignConfig)
{
    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
    LoggingWrapper::instance()->info(QString("DUT AA start, DUT SN %1, Eye type %2.")
        .arg(dutConfig.sn)
        .arg(dutConfig.eyeType == 0 ? "left" : "right"));

    MoveLimitJX::instance()->setAlignState(false);

    //TODO: test
    //Result result = DistanceCheck::instance().check();
    //if (!result.success)
    //{
    //    result.errorMsg = "DUT AA is error, " + result.errorMsg;
    //    LoggingWrapper::instance()->error(QString::fromStdString(result.errorMsg));
    //    return result;
    //}

    Result result = waitPause(m_waitPause.recipePause());
    if (!result.success)
    {
        result.errorMsg = "DUT AA is stoped, " + result.errorMsg;
        LoggingWrapper::instance()->error(QString::fromStdString(result.errorMsg));
        return result;
    }

    result = isConnected();
    if (!result.success)
    {
        LoggingWrapper::instance()->error("DUT AA failed," + QString::fromStdString(result.errorMsg));
        return result;
    }

    result = AlignUtils::fiducialLight(true);
    if (!result.success)
    {
        LoggingWrapper::instance()->error("DUT AA failed," + QString::fromStdString(result.errorMsg));
        return result;
    }

    result = waitPause(m_waitPause.recipePause());
    if (!result.success)
    {
        return result;
    }

    // DUT init position
    result = moveInitPosition(alignInfo->sn);
    if (!result.success)
    {
        LoggingWrapper::instance()->error("DUT AA failed, " + QString::fromStdString(result.errorMsg));
        return result;
    }

    result = MoveLimitJX::instance()->setMotion3DMoveAlignSpeed(true);
    if (!result.success)
    {
        LoggingWrapper::instance()->error("DUT AA failed, " + QString::fromStdString(result.errorMsg));
        return result;
    }

    result = alignEyeboxAcsXYZ(alignInfo, alignConfig);
    if (!result.success)
    {
        MoveLimitJX::instance()->setMotion3DMoveAlignSpeed(false);
        LoggingWrapper::instance()->error("DUT AA failed," + QString::fromStdString(result.errorMsg));
        return result;
    }

    {
        LoggingWrapper::instance()->info(
            QString("DUT AA alignment result, DUT input grating coordinate is X=%1, Y=%2.")
            .arg(m_inputCoord.x)
            .arg(m_inputCoord.y));

        for (int i = 0; i < m_eyeboxAcsCoords.size(); i++)
        {
            LoggingWrapper::instance()->info(QString("DUT AA alignment result, eyebox=%1, coordinate is X=%2, Y=%3, Z=%4.")
                .arg(i + 1)
                .arg(m_eyeboxAcsCoords[i].x)
                .arg(m_eyeboxAcsCoords[i].y)
                .arg(m_eyeboxAcsCoords[i].z));
        }
    }

    MoveLimitJX::instance()->setMotion3DMoveAlignSpeed(false);

    result = AlignUtils::fiducialLight(false);
    if (!result.success)
    {
        LoggingWrapper::instance()->error("DUT AA failed," + QString::fromStdString(result.errorMsg));
    }

    result = MoveLimitJX::judgeEyeboxLimit(m_eyeboxAcsCoords);
    if (!result.success)
    {
        LoggingWrapper::instance()->error("DUT AA errror, judge eyebox limit error, " + QString::fromStdString(result.errorMsg));
        return Result(false, "DUT AA errror,  judge eyebox limit error, " + result.errorMsg);
    }

    result = MoveLimitJX::instance()->setAlignState(true, m_inputCoord);
    if (!result.success)
    {
        LoggingWrapper::instance()->error("DUT AA errror, set align state error, " + QString::fromStdString(result.errorMsg));
        return Result(false, "DUT AA errror, set align state error, " + result.errorMsg);
    }

    LoggingWrapper::instance()->debug(QString("DUT AA success, DUT SN %1, Eye type %2.")
        .arg(dutConfig.sn)
        .arg(dutConfig.eyeType == 0 ? "left" : "right"));
    
    emit alignEyeboxEndSignal(true);
    return result;
}

Result Align::Alignment2D::alignEyeboxAcsXYZ(AlignBaseInfo2D* alignInfo, AlignConfig alignConfig)
{
    AlignBaseInfo2D* alignInfo2D = (AlignBaseInfo2D*)alignInfo;

    Result result = CaptureFiducialImage().calMvCaptureFiducialCoords2D(alignInfo2D->acsFiducials, alignInfo2D->sn, alignConfig.calculateMvFidsByInputFlag);
    if (!result.success)
    {
        return result;
    }

    result = judgeDataValidity(*alignInfo2D, alignConfig);
    if (!result.success)
    {
        return result;
    }

    result = waitPause(m_waitPause.recipePause());
    if (!result.success)
    {
        return result;
    }

    FiducialConfig::instance()->setFiducialCenter(false);
    // Use fiducial motion position in "FiducialConfig.json"
    if (FiducialConfig::instance()->fiduciualMotionEnabled())
    {
        std::vector<cv::Point3f> acsFiducialsNew = alignInfo2D->acsFiducials;
        result = FiducialConfig::instance()->getFiduciualMotionPosList(alignConfig.fiducialTypeStr, acsFiducialsNew);
        if (result.success)
        {
            FiducialConfig::instance()->setFiducialCenter(true);
            alignInfo2D->acsFiducials = acsFiducialsNew;
            LoggingWrapper::instance()->info("Use motion3D position in fiducial config json.");
        }
        else {
            LoggingWrapper::instance()->info("Use calculate fiducial motion3D position, " + QString::fromStdString(result.errorMsg));
        }
    }
    else
    {
        LoggingWrapper::instance()->info("Use calculate fiducial motion3D position.");
    }

    m_eyeboxAcsCoords.clear();
    result = alignEyeboxAcsXYZ_DutMove(m_eyeboxAcsCoords, *alignInfo2D, alignConfig);
    if (!result.success)
    {
        return result;
    }

    result = waitPause(m_waitPause.recipePause());
    if (!result.success)
    {
        return result;
    }

    //TODO: to be test, Move safe position
    //result = MoveLimitJX::moveRel(cv::Point3f(10, -20, -20), withCamera);
    //if (!result.success)
    //{
    //    return result;
    //}
    return result;
}

Result Alignment2D::alignEyeboxAcsXYZ(std::vector<cv::Point3f>& acsCoords, const AlignBaseInfo2D& alignInfo,
    AlignConfig alignConfig)
{
    Result ret;
    std::vector<cv::Point3f> fiducialCoords;
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

        cv::Point3f fiducialCenterAcs;
        ret = fiducialIdentify(fiducialCenterAcs, alignInfo, alignConfig, i, false);
        if (!ret.success)
        {
            return ret;
        }

        fiducialCoords.push_back(fiducialCenterAcs);
    }

    for (int i = 0; i < fiducialCoords.size(); i++)
    {
        LoggingWrapper::instance()->info(
            QString("DUT AA alignment, fiducial=%1, MV align fiducial, measure camera coordinate is X=%2, Y=%3, Z=%4.")
            .arg(i+1)
            .arg(fiducialCoords[i].x)
            .arg(fiducialCoords[i].y)
            .arg(fiducialCoords[i].z));
    }

    // calculate eyeboxs ACS XY coordinate
    RigidTrans2D rigidTrans2D;
    ret = AxesTransfer2D().rigidTransform(rigidTrans2D, getList2D(alignInfo.fiducials), getList2D(fiducialCoords));
    if(!ret.success){
        return Result(false, "DUT alignment error," + ret.errorMsg);
    }
    vector<cv::Point2f> mvAlignEyeboxList = AxesTransfer2D().applyTransformList(getList2D(alignInfo.eyeboxs), rigidTrans2D);
    cv::Point2f mvAlignInputCoord = AxesTransfer2D().applyTransform(getList2D(alignInfo.inputGrating), rigidTrans2D);

    //TODO: input align, eyebox align
    ret = calDutInput2D(m_inputCoord, alignInfo.mvAlignProjectorCoord, mvAlignInputCoord);
    if(!ret.success){
        return ret;
    }

    float angleDeg = rigidTrans2D.angleRad * 180.0f / CV_PI;
    MetricsData::instance()->setDutAngle(angleDeg);
    LoggingWrapper::instance()->info(QString("DUT alignment, calculate DUT angle = %1 degree.").arg(angleDeg));

    // finally eyebox
    cv::Point2f offset = alignInfo.mvAlignProjectorCoord - mvAlignInputCoord;
    acsCoords.clear();
    float lensInfinitelyCloseDutMotionZ = CalibrationConfig::instance()->getLensInfinitelyCloseDutMotionZ();
    for (int i = 0; i < alignInfo.eyeboxs.size(); ++i) {
        cv::Point2f lensAlignEyebox;
        lensAlignEyebox.x = mvAlignEyeboxList[i].x + alignInfo.i29RelativeMvCoord.x;
        lensAlignEyebox.y = mvAlignEyeboxList[i].y + alignInfo.i29RelativeMvCoord.y;
        lensAlignEyebox += offset;

        acsCoords.push_back(cv::Point3f(lensAlignEyebox.x, lensAlignEyebox.y, lensInfinitelyCloseDutMotionZ - alignInfo.eyeRelief));
    }

    // Write MC motion3D relative DUT motion3D coordinate when MV see fiduciual
    //cv::Point3f dutMotion;
    //MoveLimitJX::getCoordinate3D(dutMotion, withCamera);
    //FiducialConfig::instance()->writeFiduciualMotionPosRelativeDut(alignConfig.fiducialTypeStr, dutMotion, mcMotions);
    m_rigidTrans2D = rigidTrans2D;
    return Result();
}

Result Align::Alignment2D::alignEyeboxAcsXYZ_DutMove(std::vector<cv::Point3f>& acsCoords, const AlignBaseInfo2D& alignInfo, AlignConfig alignConfig)
{
    cv::Point3f mcCurrentCoord;
    Result ret = MoveLimitJX::getCoordinate3D(mcCurrentCoord, withCamera);
    if(!ret.success){
        return ret;
    }

    cv::Point2f dutCurrentCoord;
    ret = MoveLimitJX::getCoordinate2D(dutCurrentCoord, ACS2DDUTPlane);
    if (!ret.success) {
        return ret;
    }
    
    std::vector<cv::Point3f> dutFiducialCoords;
    for (int i = 0; i < alignInfo.acsFiducials.size(); ++i)
    {
        ret = waitPause(m_waitPause.recipePause());
        if (!ret.success)
        {
            return ret;
        }

        cv::Point3f coord = alignInfo.acsFiducials.at(i);
        cv::Point3f mcOffset = coord - mcCurrentCoord;
        ret = MoveLimitJX::instance()->dutMoveAbs(dutCurrentCoord + cv::Point2f(-mcOffset.x, -mcOffset.y));
        if (!ret.success)
        {
            return ret;
        }

        ret = waitPause(m_waitPause.recipePause());
        if (!ret.success)
        {
            return ret;
        }

        cv::Point3f fiducialCenterAcs;
        ret = fiducialIdentify(fiducialCenterAcs, alignInfo, alignConfig, i, true);
        if (!ret.success)
        {
            return ret;
        }

        dutFiducialCoords.push_back(fiducialCenterAcs);
    }

    cv::Point3f mcInitTest = MoveLimitConfigJX::instance()->getInitTestMcPosition();
    cv::Point2f dutInitTest = MoveLimitConfigJX::instance()->getInitTestDutPosition();

    LoggingWrapper::instance()->info(
        QString("DUT AA alignment, measure camera init test coordinate X=%1, Y=%2, Z=%3; DUT init test coordinate X=%4, Y=%5.")
        .arg(mcInitTest.x)
        .arg(mcInitTest.y)
        .arg(mcInitTest.z)
        .arg(dutInitTest.x)
        .arg(dutInitTest.y));

    for (int i = 0; i < dutFiducialCoords.size(); i++)
    {
        LoggingWrapper::instance()->info(
            QString("DUT AA alignment, fiducial=%1, MV align fiducial, DUT coordinate is X=%2, Y=%3.")
            .arg(i + 1)
            .arg(dutFiducialCoords[i].x)
            .arg(dutFiducialCoords[i].y));
    }

    {
        // calculate DUT rotate offset
        RigidTrans2D dutRigidTrans2D;
        ret = AxesTransfer2D().rigidTransform(dutRigidTrans2D, getList2D(alignInfo.fiducials), getList2D(dutFiducialCoords));
        if (!ret.success) {
            return Result(false, "DUT alignment error," + ret.errorMsg);
        }
        vector<cv::Point2f> mvAlignEyeboxListDut = AxesTransfer2D().applyTransformList(getList2D(alignInfo.eyeboxs), dutRigidTrans2D);
        cv::Point2f mvAlignInputCoordDut = AxesTransfer2D().applyTransform(getList2D(alignInfo.inputGrating), dutRigidTrans2D);

        LoggingWrapper::instance()->info(
            QString("DUT AA alignment, MV align input grating, DUT coordinate is X=%1, Y=%2.")
            .arg(mvAlignInputCoordDut.x)
            .arg(mvAlignInputCoordDut.y));
    }

    std::vector<cv::Point3f> mcFiducialCoords;
    for (int i = 0; i < dutFiducialCoords.size(); i++)
    {
        cv::Point3f dutOffset = dutFiducialCoords[i] - cv::Point3f(dutCurrentCoord.x, dutCurrentCoord.y, 0);
        dutOffset.z = 0;
        mcFiducialCoords.push_back(mcCurrentCoord - dutOffset);
    }

    //TODO: test
    for (int i = 0; i < mcFiducialCoords.size(); i++)
    {
        LoggingWrapper::instance()->info(
            QString("DUT AA alignment, fiducial=%1, MV align fiducial, measure camera coordinate is X=%2, Y=%3, Z=%4.")
            .arg(i + 1)
            .arg(mcFiducialCoords[i].x)
            .arg(mcFiducialCoords[i].y)
            .arg(mcFiducialCoords[i].z));
    }

    // calculate eyeboxs ACS XY coordinate
    RigidTrans2D rigidTrans2D;
    ret = AxesTransfer2D().rigidTransform(rigidTrans2D, getList2D(alignInfo.fiducials), getList2D(mcFiducialCoords));
    if (!ret.success) {
        return Result(false, "DUT alignment error," + ret.errorMsg);
    }
    vector<cv::Point2f> mvAlignEyeboxList = AxesTransfer2D().applyTransformList(getList2D(alignInfo.eyeboxs), rigidTrans2D);
    cv::Point2f mvAlignInputCoord = AxesTransfer2D().applyTransform(getList2D(alignInfo.inputGrating), rigidTrans2D);

    //TODO: test
    LoggingWrapper::instance()->info(
        QString("DUT AA alignment, MV align input grating, measure camera coordinate is X=%1, Y=%2, Z=%3.")
        .arg(mvAlignInputCoord.x)
        .arg(mvAlignInputCoord.y)
        .arg(mcCurrentCoord.z));


    //TODO: input align, eyebox align
    cv::Point2f offset = alignInfo.mvAlignProjectorCoord - mvAlignInputCoord;
    m_inputCoord = dutCurrentCoord + offset;

    float angleDeg = rigidTrans2D.angleRad * 180.0f / CV_PI;
    MetricsData::instance()->setDutAngle(angleDeg);
    LoggingWrapper::instance()->info(QString("DUT alignment, calculate DUT angle = %1 degree.").arg(angleDeg));

    // finally eyebox
    acsCoords.clear();
    float lensInfinitelyCloseDutMotionZ = CalibrationConfig::instance()->getLensInfinitelyCloseDutMotionZ();
    for (int i = 0; i < alignInfo.eyeboxs.size(); ++i) {
        cv::Point2f lensAlignEyebox;
        lensAlignEyebox.x = mvAlignEyeboxList[i].x + alignInfo.i29RelativeMvCoord.x;
        lensAlignEyebox.y = mvAlignEyeboxList[i].y + alignInfo.i29RelativeMvCoord.y;
        lensAlignEyebox += offset;

        acsCoords.push_back(cv::Point3f(lensAlignEyebox.x, lensAlignEyebox.y, lensInfinitelyCloseDutMotionZ - alignInfo.eyeRelief));
    }

    m_rigidTrans2D = rigidTrans2D;
    return Result();
}

void Alignment2D::continueRun(bool isContinue, cv::Point2f pixel)
{
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

void Align::Alignment2D::notifyPause(bool isPause)
{
    m_waitPause.notifyPause(isPause);
}

bool Align::Alignment2D::isStop()
{
    return m_waitPause.isStop();
}

Result Alignment2D::autoToManualFiducial(cv::Mat image, MvExpose mvExpose, cv::Point2f& pixel)
{
    ImageManage().saveFiducialErrorImage(image);
    return manualFiducial(pixel, mvExpose, true);
}

Result Alignment2D::manualFiducial(cv::Point2f& pixel, MvExpose mvExpose, bool isAutoFiducial)
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

Result Align::Alignment2D::waitPause(bool isPause)
{
    return m_waitPause.waitPause(isPause);
}

Result Align::Alignment2D::judgeDataValidity(const AlignBaseInfo2D& alignInfo, const AlignConfig& alignConfig)
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

Result Align::Alignment2D::relativeMoveByWafer(const cv::Point3f& waferRelPos)
{
    if(m_rigidTrans2D.translation == cv::Point2f()){
        return Result(false, QString("Measure camera relative wafer move error, wafer relative position is (%1,%2).")
            .arg(QString::number(waferRelPos.x, 'f', 3))
            .arg(QString::number(waferRelPos.y, 'f', 3))
            .toStdString());
    }

    if (abs(waferRelPos.x) > 10 || abs(waferRelPos.y) > 5 || abs(waferRelPos.z) > 5) {
        return Result(false, QString("Measure camera relative wafer is too large, wafer relative position is (%1,%2,%3).")
            .arg(QString::number(waferRelPos.x, 'f', 3))
            .arg(QString::number(waferRelPos.y, 'f', 3))
            .arg(QString::number(waferRelPos.z, 'f', 3))
            .toStdString());
    }

    cv::Point2f waferRelPos2D = cv::Point2f(waferRelPos.x, waferRelPos.y);
    cv::Point2f relAcs = AxesTransfer2D().relativeAcs(waferRelPos2D, m_rigidTrans2D);
    Result ret = MoveLimitJX::mcMoveRel(cv::Point3f(relAcs.x, relAcs.y, waferRelPos.z));
    if (!ret.success)
    {
        return Result(false, "Measure camera relative wafer move error, " + ret.errorMsg);
    }
    return Result();
}

Result Align::Alignment2D::fiducialIdentify(cv::Point3f& fiducialCenterAcs, const AlignBaseInfo2D& alignInfo, AlignConfig alignConfig, int fiducialNo, bool dutMove)
{
    cv::Point2f pixel;
    Result ret = fiducialIdentifyCal(pixel, fiducialCenterAcs, alignInfo, alignConfig, fiducialNo, 0, dutMove);
    if (!ret.success)
    {
        return ret;
    }

    saveFiducialImage(pixel, alignConfig.autoMode, fiducialNo);
    return Result();
}

Result Align::Alignment2D::fiducialCenter(cv::Point3f &fiducialCenterAcs, const AlignBaseInfo2D& alignInfo, AlignConfig alignConfig, int fiducialNo)
{
    cv::Point2f pixel;
    Result ret;
    for (int i = 0; i < FIDUCIAL_MANUAL_MOVE_COUNT_MAX + 1; ++i)
    {
        ret = fiducialIdentifyCal(pixel, fiducialCenterAcs, alignInfo, alignConfig, fiducialNo, i);
        if (!ret.success)
        {
            return ret;
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

        ret = AlignUtils::acsAbsoluteMove(fiducialCenterAcs, alignInfo.acsCoordLimitList);
        if (!ret.success)
        {
            return ret;
        }
    }

    saveFiducialImage(pixel, alignConfig.autoMode, fiducialNo);
    return Result(false, "Aligment fiducial center is failed.");
}

Result Align::Alignment2D::fiducialIdentifyCal(cv::Point2f& pixel, cv::Point3f& fiducialCenterAcs, const AlignBaseInfo2D& alignInfo, AlignConfig alignConfig, int fiducialNo, int number, bool dutMove)
{
    Result ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    if (MvImage::center == cv::Point2f()) {
        cv::Mat img = m_pCamera->GetImage();
        if (img.empty())
        {
            return Result(false, "Fiducial identify error, MV camera get fiducial image null.");
        }
        MvImage::center = cv::Point2f(img.cols / 2.0, img.rows / 2.0);
    }

    if (alignConfig.autoMode)
    {
        ret = autoIdentifyFiducial(pixel, alignConfig.fiducialTypeStr, alignConfig.mvExpose, number, fiducialNo);
        if (!ret.success)
        {
            return ret;
        }
    }
    else {
        ret = manualFiducial(pixel, alignConfig.mvExpose, false);
        if (!ret.success)
        {
            return ret;
        }
    }

    if(dutMove)
    {
        ret = calFiducialCenterMoveCoord_DUT(fiducialCenterAcs, alignInfo, pixel);
    }
    else
    {
        ret = calFiducialCenterMoveCoord(fiducialCenterAcs, alignInfo, pixel);
    }

    if (!ret.success)
    {
        return ret;
    }
    return Result();
}

Result Align::Alignment2D::autoIdentifyFiducial(cv::Point2f& pixel, std::string fiducialType, MvExpose mvExpose, int number, int fiducialNo)
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
    QThread::msleep(1500);

    cv::Mat img = m_pCamera->GetImage();
    if (img.empty())
    {
        return Result(false, "Auto identify fiducial failed, MV camera get fiducial image null.");
    }

    FiducialInfo fiducialInfo(fiducialType, fiducialNo, number > 0);
    Result ret = FiducialRecognize::instance()->circleDetectTemp(img, pixel, fiducialInfo);
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

Result Align::Alignment2D::calFiducialCenterMoveCoord(cv::Point3f& fiducialCenterAcs, const AlignBaseInfo2D& alignInfo, cv::Point2f pixel)
{
    cv::Point2f offset = MvImage::pixelConvert(cv::Point2f(pixel.x - MvImage::center.x, pixel.y - MvImage::center.y));

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
    cv::Point2f offsetAcs = AlignUtils::relMoveCoordMvToAcs(offset, alignInfo.motion3DAndMVRelate);
    fiducialCenterAcs = cv::Point3f(acsCurrentCoord.x + offsetAcs.x, acsCurrentCoord.y + offsetAcs.y, acsCurrentCoord.z);
    return Result();
}

Result Align::Alignment2D::calFiducialCenterMoveCoord_DUT(cv::Point3f& fiducialCenterAcs, const AlignBaseInfo2D& alignInfo, cv::Point2f pixel)
{
    cv::Point2f offset = MvImage::pixelConvert(cv::Point2f(pixel.x - MvImage::center.x, pixel.y - MvImage::center.y));

    cv::Point2f acsCurrentCoord;
    Result ret = MoveLimitJX::getCoordinate2D(acsCurrentCoord, ACS2DDUTPlane);
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
    cv::Point2f offsetAcs = AlignUtils::relMoveCoordMvToAcs(offset, alignInfo.motion3DAndMVRelate);
    fiducialCenterAcs = cv::Point3f(acsCurrentCoord.x - offsetAcs.x, acsCurrentCoord.y - offsetAcs.y, 0);
    return Result();
}

Result Align::Alignment2D::saveFiducialImage(cv::Point2f pixel, bool autoMode, int fiducialNumber)
{
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

Result Align::Alignment2D::saveImage(cv::Mat image, QString dirName, QString fileName)
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

Result Align::Alignment2D::isConnected()
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

    // DUT 2D move station
    if (!Motion2DModel::getInstance(ACS2DDUTPlane)->GetIsConnected())
    {
        return Result(false, "DUT move 2D motion is not connectd.");
    }

    // Keyence
    if (!MLKeyenceCL::MakeRangeFinder()->IsConnected())
    {
        return Result(false, "Confocal Distance Sensor is not connectd.");
    }
    return Result();
}

Result Align::Alignment2D::moveInitPosition(const QString& dutSn)
{
    Result ret = MoveLimitJX::moveMcInitTestPos();
    if (!ret.success) {
        return Result(false, "Move measure camera init test position error, " + ret.errorMsg);
    }

    ret = MoveLimitJX::moveDutInitTestPos();
    if (!ret.success) {
        return Result(false, "Move DUT init test position error, " + ret.errorMsg);
    }

    return Result();
}

vector<cv::Point2f> Align::Alignment2D::getList2D(const vector<cv::Point3f>& list3D)
{
    vector<cv::Point2f> list;
    for (cv::Point3f point : list3D) {
        list.push_back(cv::Point2f(point.x, point.y));
    }
    return list;
}

cv::Point2f Align::Alignment2D::getList2D(const cv::Point3f& point)
{
    return cv::Point2f(point.x, point.y);
}

Result Align::Alignment2D::calDutInput2D(cv::Point2f& dutCoord, const cv::Point2f& mvAlignProjectorCoord, const cv::Point2f& mvAlignInputCoord)
{
    cv::Point2f offset = mvAlignProjectorCoord - mvAlignInputCoord;

    cv::Point2f dutCoordCurrent;
    Result ret = MoveLimitJX::getCoordinate2D(dutCoordCurrent);
    if(!ret.success){
        return Result(false, "Calculate DUT input grating alignment coordinate error, " + ret.errorMsg);
    }

    dutCoord = dutCoordCurrent + offset;
    return Result();
}

std::vector<cv::Point3f> Align::Alignment2D::getEyeboxAcsCoords()
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

