#include "SLBWidget.h"
#include "piMotionActor/MoveLimitJX.h"
#include <QtConcurrent>
#include "Core/loggingwrapper.h"
#include <QMessageBox>
#include "piMotionActor\configItem.h"
#include "integratingSphere/ReticleMode.h"
#include <integratingSphere\integratingspheretcpmodel.h>
#include "piMotionActor/ConfigMAInfo.h"

SLBWidget::SLBWidget(aligmentWidget *alignWidget, QString toolboxName, QWidget *parent)
    : IToolBox(toolboxName, parent), m_alignWidget(alignWidget)
{
	ui.setupUi(this);

    init();
    setMinimumSize(0, 0);
}

SLBWidget::~SLBWidget()
{
}

void SLBWidget::on_btn_connect_clicked()
{
    LoggingWrapper::instance()->info("Start SLB devices connection, please wait......");
    waitConnect(true);

    QFuture<Result> future = QtConcurrent::run(this, &SLBWidget::connectDevices);
    m_watcher.setFuture(future);
}

void SLBWidget::on_btn_disconnect_clicked()
{
    LoggingWrapper::instance()->info("SLB device disconnected.");

    Motion3DModel::getInstance(withDUT)->disconnect();
    Motion3DModel::getInstance(withCamera)->disconnect();

    Motion2DModel::getInstance(ACS2DDUT)->disconnect();
    Motion2DModel::getInstance(ACS2DPro)->disconnect();

    ReticleMode::instance()->disconnect();
    ui.label_status->setText("Not Connected.");
}

void SLBWidget::on_btn_moveInitPosition_clicked()
{
    btnEnabled(false);
    QFuture<Result> future = QtConcurrent::run(this, &SLBWidget::moveInit);
    m_watcherMove.setFuture(future);
}

void SLBWidget::on_btn_moveAlignSLB_clicked()
{
    QMessageBox::StandardButton box = QMessageBox::question(
        this, "Tips", "Check whether the projector is rotated to the SLB state?", QMessageBox::Yes | QMessageBox::No);
    if (box == QMessageBox::No)
    {
        return;
    }

    btnEnabled(false);

    QFuture<Result> future = QtConcurrent::run(this, &SLBWidget::alignSLB);
    m_watcherMove.setFuture(future);
}

void SLBWidget::on_btn_save_clicked()
{
    cv::Point2f tilt;
    Result ret = getProjectorTilt(tilt);
    if (!ret.success)
    {
        QMessageBox::critical(this, "Save SLB position error", QString::fromStdString(ret.errorMsg));
        return;
    }

    cv::Point3f mcCoord;
    ret = getMcPosition(mcCoord);
    if (!ret.success)
    {
        QMessageBox::critical(this, "Save SLB position error", QString::fromStdString(ret.errorMsg));
        return;
    }

    ret = ConfigMAInfo::GetInstance()->updateSLB(mcCoord, tilt);
    if (!ret.success)
    {
        QMessageBox::critical(this, "Save SLB position error",
                              "Save failed, " + QString::fromStdString(ret.errorMsg));
        return;
    }
}

void SLBWidget::on_btn_stop_clicked()
{
    Motion3DModel::getInstance(withCamera)->Motion3DMoveStop();
    Motion3DModel::getInstance(withDUT)->Motion3DMoveStop();
}

void SLBWidget::handleFinished()
{
    waitConnect(false);

    QFuture<Result> future = m_watcher.future();
    Result ret = future.result();
    
    if (!ret.success)
    {
        QMessageBox::critical(this, "Device connect error", QString::fromStdString(ret.errorMsg));
        ui.label_status->setText(QString::fromStdString(ret.errorMsg));
    }else{
        ui.label_status->setText("Connect success.");
    }
}

void SLBWidget::moveHandleFinished()
{
    btnEnabled(true);

    QFuture<Result> future = m_watcherMove.future();
    Result ret = future.result();
    if (!ret.success && ret.errorCode == 0)
    {
        QMessageBox::critical(this, "Move error", QString::fromStdString(ret.errorMsg));
    }
}

void SLBWidget::waitConnect(bool flag)
{
    ui.btn_connect->setDisabled(flag);
    ui.btn_disconnect->setDisabled(flag);
}

void SLBWidget::init()
{
    ui.label_status->setText("Not Connected.");
    ui.label_status->setWordWrap(true);

    connect(&m_watcher, SIGNAL(finished()), this, SLOT(handleFinished()));
    connect(&m_watcherMove, SIGNAL(finished()), this, SLOT(moveHandleFinished()));
}

Result SLBWidget::connectDevices()
{
    // DUT 2D move station
    QString ipAndOther = ConfigItem::instance()->getMotion2DIpAndOther(ACS2DDUTPlane);
    Result result = Motion2DModel::getInstance(ACS2DDUTPlane)->connectAndInit(ipAndOther);
    if (!result.success)
    {
        return Result(false, "DUT Move Station connect failed, " + result.errorMsg);
    }

    // Measure 3D move station
    QString mcIp = ConfigItem::instance()->getMotion3DIpAndOther(withCamera);
    result = Motion3DModel::getInstance(withCamera)->connectAndInit(mcIp);
    if (!result.success)
    {
        return Result(false, "Measure camera move Station connect failed, " + result.errorMsg);
    }

    // projector tilt station
    QString ipPro = ConfigItem::instance()->getMotion2DIpAndOther(ACS2DPro);
    result = Motion2DModel::getInstance(ACS2DPro)->connectAndInit(ipPro);
    if (!result.success)
    {
        return Result(false, "Projector Tilt Station connect failed, " + result.errorMsg);
    }

    //// Reticle
    //result = ReticleMode::instance()->connect();
    //if (!result.success)
    //{
    //    return Result(false, "Reticle connect failed, " + result.errorMsg);
    //}

    //// Integrating Sphere
    //result = IntegratingSphereTCPModel::getInstance()->Connect();
    //if (!result.success)
    //{
    //    return Result(false, "Integrating Sphere connect failed, " + result.errorMsg);
    //}
    return Result();
}

void SLBWidget::btnEnabled(bool enabled)
{
    ui.btn_connect->setEnabled(enabled);
    ui.btn_disconnect->setEnabled(enabled);
    ui.btn_moveInitPosition->setEnabled(enabled);
    ui.btn_moveAlignSLB->setEnabled(enabled);
    ui.btn_save->setEnabled(enabled);
}

Result SLBWidget::moveInit()
{
    return MoveLimitJX::moveInit();
}

Result SLBWidget::alignSLB()
{
    Result ret = moveMeasureCameraSafe();
    if (!ret.success)
    {
        return ret;
    }

    ret = moveProjectorTilt();
    if (!ret.success)
    {
        return ret;
    }

    ret = moveMeasureCamera();
    if (!ret.success)
    {
        return ret;
    }

    return ret;
    //return setReticleAndIS();
}

Result SLBWidget::moveProjectorTilt()
{
    cv::Point2f tilt;
    Result ret = ConfigMAInfo::GetInstance()->getProjectorTiltSLB(tilt);
    if (!ret.success)
    {
        return ret;
    }

    cv::Point2f saveTilt;
    ret = ConfigMAInfo::GetInstance()->getSafeTiltSLB(saveTilt);
    if (!ret.success)
    {
        return ret;
    }

    if (fabs(tilt.x - saveTilt.x) > 0.5 || fabs(tilt.y - saveTilt.y) > 0.5)
    {
        return Result(
            false,
            QString("SLB projector tilt is out of limit degree, tilt coordinate is (%1,%2), safe coordinate is (%3,%4)")
            .arg(tilt.x)
            .arg(tilt.y)
            .arg(saveTilt.x)
            .arg(saveTilt.y)
            .toStdString());
    }

    ret = Motion2DModel::getInstance(ACS2DPro)->Motion2DMoveAbsRaw(tilt); 
    if(!ret.success){
        return Result(false, "Projector tilt failed, " + ret.errorMsg);
    }
    return Result();
}

Result SLBWidget::moveMeasureCamera()
{
    cv::Point3f alignPos;
    Result ret = ConfigMAInfo::GetInstance()->getAlignPositonSLB(alignPos);
    if (!ret.success)
    {
        return ret;
    }

    cv::Point3f safePos;
    ret = ConfigMAInfo::GetInstance()->getSafePositonSLB(safePos);
    if (!ret.success)
    {
        return ret;
    }

    if (alignPos.z > safePos.z || fabs(alignPos.x - safePos.x) > 10 || fabs(alignPos.y - safePos.y) > 10)
    {
        return Result(
            false,
            QString("Align SLB position is out of limit, align coordinate is (%1,%2,%3), safe coordinate is (%4,%5,%6)")
                .arg(alignPos.x)
                .arg(alignPos.y)
                .arg(alignPos.z)
                .arg(safePos.x)
                .arg(safePos.y)
                .arg(safePos.z)
                .toStdString());
    }

    MoveLimitJX::moveAbsNoCollision(alignPos, withCamera);
    if (!ret.success)
    {
        return Result(false, "Measure camera move failed, " + ret.errorMsg);
    }
    return Result();
}

Result SLBWidget::setReticleAndIS()
{
    QString reticle;
    Result ret = ConfigMAInfo::GetInstance()->getReticleSLB(reticle);
    if (!ret.success)
    {
        return ret;
    }
    ret = ReticleMode::instance()->changeReticle(reticle);
    if (!ret.success)
    {
        return ret;
    }

    ISSolution sulution;
    ConfigMAInfo::GetInstance()->getIntergratingSphereSLB(sulution.name, sulution.scale);
    sulution.colorFocus = sulution.name;
    sulution.currentOutputs.push_back(sulution.scale);
    ret = IntegratingSphereTCPModel::getInstance()->setSolution(sulution);
    return ret;
}

Result SLBWidget::getProjectorTilt(cv::Point2f &tilt)
{
    bool ret = Motion2DModel::getInstance(ACS2DPro)->getPositionRaw(tilt);
    if (!ret)
    {
        return Result(false,"Get projector tilt error.");
    }

    cv::Point2f saveTilt;
    Result result = ConfigMAInfo::GetInstance()->getSafeTiltSLB(saveTilt);
    if (!result.success)
    {
        return result;
    }

    if (fabs(tilt.x - saveTilt.x) > 1 || fabs(tilt.y - saveTilt.y) > 1)
    {
        return Result(
            false,
            QString("SLB projector tilt is out of limit degree, tilt coordinate is (%1,%2), safe coordinate is (%3,%4)")
            .arg(tilt.x)
            .arg(tilt.y)
            .arg(saveTilt.x)
            .arg(saveTilt.y)
            .toStdString());
    }
    return Result();
}

Result SLBWidget::getMcPosition(cv::Point3f &mcCoord)
{
    Result ret = MoveLimitJX::getCoordinate3D(mcCoord, withCamera);
    if (!ret.success)
    {
        return Result(false, "Get measure camera coordinate error, " + ret.errorMsg);
    }

    cv::Point3f safePos;
    ret = ConfigMAInfo::GetInstance()->getSafePositonSLB(safePos);
    if (!ret.success)
    {
        return ret;
    }

    if (mcCoord.z > safePos.z || fabs(mcCoord.x - safePos.x) > 10 || fabs(mcCoord.y - safePos.y) > 10)
    {
        return Result(
            false,
            QString("Align SLB position is out of limit, align coordinate is (%1,%2,%3), safe coordinate is (%4,%5,%6)")
                .arg(mcCoord.x)
                .arg(mcCoord.y)
                .arg(mcCoord.z)
                .arg(safePos.x)
                .arg(safePos.y)
                .arg(safePos.z)
                .toStdString());
    }

    return Result();
}

Result SLBWidget::moveMeasureCameraSafe()
{
    cv::Point2f coordPro;
    Result ret = MoveLimitJX::getCoordinate2DTiltRaw(coordPro, ACS2DPro);
    if (!ret.success)
    {
        return ret;
    }

    cv::Point2f tilt;
    ret = ConfigMAInfo::GetInstance()->getProjectorTiltSLB(tilt);
    if (!ret.success)
    {
        return ret;
    }

    if (abs(coordPro.x - tilt.x) > 0.05 || abs(coordPro.y - tilt.y) > 0.05)
    {
        cv::Point3f loadPos = MoveLimitConfigJX::instance()->getLoadMeasureCameraPosition();

        cv::Point3f coord;
        ret = MoveLimitJX::getCoordinate3D(coord, withCamera);
        if (!ret.success)
        {
            return ret;
        }

        if (abs(coord.z - loadPos.z) > 1.0 || abs(coord.x - loadPos.x) > 1.0 || abs(coord.y - loadPos.y) > 1.0)
        {
            ret = MoveLimitJX::moveAbsToSafe(withCamera);
            if (!ret.success)
            {
                return ret;
            }
        }
    }

    ret = MoveLimitJX::moveDutAbsToSafe();
    if (!ret.success) {
        return Result(false, "Move DUT to init position error, " + ret.errorMsg);
    }
    return Result();
}
