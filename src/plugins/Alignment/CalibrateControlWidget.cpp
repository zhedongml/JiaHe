#include "CalibrateControlWidget.h"

CalibrateControlWidget::CalibrateControlWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    init();
}

CalibrateControlWidget::~CalibrateControlWidget()
{
}

void CalibrateControlWidget::init()
{
    connect(ui.cmbEyeIndex, SIGNAL(currentIndexChanged(int)), this, SLOT(EyeIndexChanged(int)));
    connect(&watcher, SIGNAL(finished()), this, SLOT(handleFinished()));
    CalibrationConfig::instance()->load();
    m_eyeboxAcsCoords = CalibrationConfig::instance()->readEyeboxAcsCoords();
    ui.cmbEyeIndex->setCurrentIndex(0);
    if (m_eyeboxAcsCoords.size() > 0)
    {
        ui.lineEdit_EyeX->setText(QString::number(m_eyeboxAcsCoords[0].x, 'f', 3));
        ui.lineEdit_EyeY->setText(QString::number(m_eyeboxAcsCoords[0].y, 'f', 3));
        ui.lineEdit_EyeZ->setText(QString::number(m_eyeboxAcsCoords[0].z, 'f', 3));
    }
}

void CalibrateControlWidget::on_btnSave_clicked()
{
    m_eyeboxAcsCoords = Alignment3D::instance()->getEyeboxAcsCoords();
    if (m_eyeboxAcsCoords.size() > 0)
        CalibrationConfig::instance()->writeEyeboxAcsCoords("", m_eyeboxAcsCoords);
}

void CalibrateControlWidget::on_btnGo_clicked()
{
    ui.btnGo->setEnabled(false);
    cv::Point3f relCoord;
    relCoord.x = ui.lineEdit_EyeX->text().toFloat();
    relCoord.y = ui.lineEdit_EyeY->text().toFloat();
    relCoord.z = ui.lineEdit_EyeZ->text().toFloat();

    QFuture<Result> future = QtConcurrent::run(this, &CalibrateControlWidget::acsRelMove, relCoord);
    watcher.setFuture(future);
}

void CalibrateControlWidget::EyeIndexChanged(int index)
{
    m_eyeboxAcsCoords = Alignment3D::instance()->getEyeboxAcsCoords();
    ui.lineEdit_EyeX->setText(QString::number(m_eyeboxAcsCoords[index].x, 'f', 3));
    ui.lineEdit_EyeY->setText(QString::number(m_eyeboxAcsCoords[index].y, 'f', 3));
    ui.lineEdit_EyeZ->setText(QString::number(m_eyeboxAcsCoords[index].z, 'f', 3));
}

Result CalibrateControlWidget::startCalibrate(AlignConfig alignConfig)
{
    Result result = PLCController::instance()->coaxialLight2(true);
    if (!result.success)
    {
        return result;
    }
    int eyeType = CalibrationConfig::instance()->getDutConfig().eyeType;

    AlignBaseInfo3D alignInfo;
    Result ret = AlignUtils::getAlignBaseInfo3D(alignInfo);
    if (!ret.success)
    {
        return ret;
    }

    EyeReliefBaseInfo eyeReliefInfo = AlignUtils::getEyeReliefBaseInfo(eyeType);
    Alignment3D::instance()->continueRun(true);
    result = Alignment3D::instance()->alignEyeboxAcsXYZ(&alignInfo, eyeReliefInfo, alignConfig);
    if (!result.success)
    {
        return result;
    }

    result = PLCController::instance()->coaxialLight2(false);
    if (!result.success)
    {
        return result;
    }
    return result;
}

void CalibrateControlWidget::handleFinished()
{
    ui.btnGo->setEnabled(true);
}

Result CalibrateControlWidget::acsRelMove(const cv::Point3f &relCoord)
{
    return AlignUtils::acsRelativeMove(relCoord);
}

void CalibrateControlWidget::on_btnCalibrate_clicked()
{
    if (m_devicesConnectSuccess)
    {
        AlignConfig alignConfig;
        alignConfig.autoMode = ui.cmbAlignMode->currentText().contains("Auto");
        alignConfig.moveOut = ui.chkMoveOut->isChecked();
        alignConfig.fiducialCount = 3;
        startCalibrate(alignConfig);
        int eyeIndex = ui.cmbEyeIndex->currentText().toInt();
        Alignment3D::instance()->continueRun(true);
        Alignment3D::instance()->moveAlignEyebox(ui.cmbEyeIndex->currentText().toInt());
        m_eyeboxAcsCoords = Alignment3D::instance()->getEyeboxAcsCoords();
        ui.lineEdit_EyeX->setText(QString::number(m_eyeboxAcsCoords[eyeIndex].x, 'f', 3));
        ui.lineEdit_EyeY->setText(QString::number(m_eyeboxAcsCoords[eyeIndex].y, 'f', 3));
        ui.lineEdit_EyeZ->setText(QString::number(m_eyeboxAcsCoords[eyeIndex].z, 'f', 3));
    }
}
