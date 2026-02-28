#include "FiducialCalibrateWidget.h"
#include <plugins\piMotionActor\Motion3DModel.h>
#include <QMessageBox>
#include "AlignUtils.h"
#include "piMotionActor/MoveLimitJX.h"
#include "piMotionActor/calibrationconfig.h"
#include "CaptureFiducialImage.h"
#include "PrjCommon/metricsdata.h"
#include "FileCsv.h"
#include <QFileDialog>
#include "Alignment2D.h"

FiducialCalibrateWidget::FiducialCalibrateWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    initUI();
    loadConfig();

    ui.btnRefresh->hide();
}

FiducialCalibrateWidget::~FiducialCalibrateWidget()
{
}

void FiducialCalibrateWidget::initUI()
{
    int radio = 100;
    ui.btnUp->setFixedWidth(radio);
    ui.btnUp->setMinimumHeight(1);
    ui.btnUp->setMaximumHeight(radio);
    ui.btnUp->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui.btnUp->setIcon(QIcon(":/image/img/up-btn-fill.png"));
    ui.btnUp->setText("");

    ui.btnDown->setFixedWidth(radio);
    ui.btnDown->setMinimumHeight(1);
    ui.btnDown->setMaximumHeight(radio);
    ui.btnDown->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui.btnDown->setIcon(QIcon(":/image/img/down-btn-fill.png"));
    ui.btnDown->setText("");

    ui.btnCenter->setFixedWidth(radio);
    ui.btnCenter->setMinimumHeight(1);
    ui.btnCenter->setMaximumHeight(radio);
    ui.btnCenter->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui.btnCenter->setIcon(QIcon(":/image/img/copyright-circle-fil.png"));
    ui.btnCenter->setText("");

    ui.btnLeft->setFixedWidth(radio);
    ui.btnLeft->setMinimumHeight(1);
    ui.btnLeft->setMaximumHeight(radio);
    ui.btnLeft->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui.btnLeft->setIcon(QIcon(":/image/img/left-btn-fill.png"));
    ui.btnLeft->setText("");

    ui.btnRight->setFixedWidth(radio);
    ui.btnRight->setMinimumHeight(1);
    ui.btnRight->setMaximumHeight(radio);
    ui.btnRight->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui.btnRight->setIcon(QIcon(":/image/img/right-btn-fill.png"));
    ui.btnRight->setText("");

    ui.label_EyeboxCoords->setText("");
    ui.label_EyeName->setText("");

    ui.cmbFiducialType->addItem("Circle");
    ui.cmbFiducialType->addItem("Crosshair");
    ui.cmbFiducialType->addItem("BMW");
    ui.cmbFiducialType->addItem("Rectangle");
    ui.cmbFiducialType->addItem("PICO");
    ui.cmbFiducialType->setCurrentIndex(4);

    ui.cmbFiducialType->setEnabled(ui.cmbAlignMode->currentIndex() == 0);

    ui.label_EyeboxCoords->setWordWrap(true);
    ui.label_EyeboxCoords->hide();
    ui.chkCalculateMvFidsByInputFlag->hide();
    ui.chkCalculateMvFidsByInputFlag->setChecked(true);

    ui.cmb_mvExposure->setCurrentIndex(1);
    ui.spinBox_mvExposure->setEnabled(true);
    ui.spinBox_mvExposure->setValue(10);

    ui.btnSaveFiducialMotionCoods->hide();
    ui.groupBox_fiducials->hide();
    ui.chkSaveEye->hide();
    ui.cmbFiducialType->hide();

    connect(ui.btnCenter, &QPushButton::clicked, this, &FiducialCalibrateWidget::eyeboxClicked);
    connect(ui.btnUp, &QPushButton::clicked, this, &FiducialCalibrateWidget::eyeboxClicked);
    connect(ui.btnDown, &QPushButton::clicked, this, &FiducialCalibrateWidget::eyeboxClicked);
    connect(ui.btnLeft, &QPushButton::clicked, this, &FiducialCalibrateWidget::eyeboxClicked);
    connect(ui.btnRight, &QPushButton::clicked, this, &FiducialCalibrateWidget::eyeboxClicked);

    connect(&watcherCalibrate, SIGNAL(finished()), this, SLOT(calibrateHandleFinished()));
    connect(&watcherEyebox, SIGNAL(finished()), this, SLOT(eyeboxHandleFinished()));
    connect(&watcherMoveFiducial, SIGNAL(finished()), this, SLOT(fiducialMoveHandleFinished()));

    connect(Alignment2D::instance(), SIGNAL(moveAlignEyeboxSignal(int)), this, SLOT(moveAlignEyeboxSlot(int)));
    connect(Alignment2D::instance(), SIGNAL(alignEyeboxEndSignal(bool)), this, SLOT(alignEyeboxEndSlot(bool)));
    
    on_comboBox_eyeboxIndexs_activated("");
    ui.label_EyeboxCurrentCoord->setText("");

    ui.label_17->hide();
    ui.doubleSpinBox_EyeOffset->hide();
    ui.groupBox->hide();
}

void FiducialCalibrateWidget::setEnabledBtns(bool enabled)
{
    ui.btnCalibrate->setEnabled(enabled);

    ui.btn_move->setEnabled(enabled);
    ui.btnSaveFiducialMotionCoods->setEnabled(enabled);

    ui.btnCenter->setEnabled(enabled);
    ui.btnUp->setEnabled(enabled);
    ui.btnDown->setEnabled(enabled);
    ui.btnLeft->setEnabled(enabled);
    ui.btnRight->setEnabled(enabled);

    ui.btn_eyeboxMove->setEnabled(enabled);
}

Result FiducialCalibrateWidget::moveSeeFiducial()
{
    Result ret = MoveLimitJX::instance()->moveDutInitTestPos();
    if(!ret.success)
    {
        return ret;
    }

    MoveOutInfo moveOutInfo = CalibrationConfig::instance()->getDutConfig().moveOutInfo;
    if(moveOutInfo.enable){
        ret = MoveLimitJX::moveRel(cv::Point3f(moveOutInfo.offset.x, moveOutInfo.offset.y, 0), withDUT);
        if(!ret.success){
            return Result(false, "DUT move out failed, " + ret.errorMsg);
        }
    }

    //std::vector<cv::Point3f> acsFiducials = CalibrationConfig::instance()->getFiducialMotion3DCoords();
    std::vector<cv::Point3f> acsFiducials = m_fiducialCoords;
    if (acsFiducials.size() < 2)
    {
        return Result(false, "ACS fiducial number is less than 2 in config file.");
    }

    cv::Point3f coord;
    if (ui.rdbFiducial1->isChecked())
    {
        coord = acsFiducials.at(0);
    }
    else if (ui.rdbFiducial2->isChecked())
    {
        coord = acsFiducials.at(1);
    }
    else
    {
        coord = acsFiducials.at(2);
    }

    return AlignUtils::acsAbsoluteMove(coord);
}

Result FiducialCalibrateWidget::getAlignData(AlignConfig &alignConfig, AlignBaseInfo3D &alignInfo,
                                           EyeReliefBaseInfo &eyeReliefInfo)
{
    //updateDutConfig(CalibrationConfig::instance()->getDutConfigReference());
    alignConfig = getAlignConfig();

    Result ret = AlignUtils::getAlignBaseInfo3D(alignInfo);
    if (!ret.success)
    {
        return ret;
    }

    //DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
    //alignInfo.eyeboxs = AlignUtils::calculateEyeboxs(dutConfig.eyeboxCenter, ui.doubleSpinBox_EyeOffset->value());

    eyeReliefInfo = AlignUtils::getEyeReliefBaseInfo();
    eyeReliefInfo.dueDistance = ui.doubleSpinBox_EyeRelief->value();
    return Result();
}

Result FiducialCalibrateWidget::getAlignData(AlignConfig& alignConfig, AlignBaseInfo2D& alignInfo)
{
    alignConfig = getAlignConfig();

    Result ret = AlignUtils::getAlignBaseInfo2D(alignInfo);
    if (!ret.success)
    {
        return ret;
    }

    alignInfo.eyeRelief = ui.doubleSpinBox_EyeRelief->value();
    return Result();
}

AlignConfig FiducialCalibrateWidget::getAlignConfig()
{
    AlignConfig alignConfig;
    alignConfig.autoMode = ui.cmbAlignMode->currentText().contains("Auto");
    alignConfig.moveOut = CalibrationConfig::instance()->getDutConfig().moveOutInfo.enable;
    alignConfig.fiducialCount = 2;
    alignConfig.fiducialType = AlignUtils::analyzeFiducialType(ui.cmbFiducialType->currentText());
    alignConfig.calculateMvFidsByInputFlag = ui.chkCalculateMvFidsByInputFlag->isChecked();

    alignConfig.mvExpose.isAuto = ui.cmb_mvExposure->currentText().contains("Auto");
    alignConfig.mvExpose.manualExposeTime = ui.spinBox_mvExposure->value();
    return alignConfig;
}

void FiducialCalibrateWidget::updateDutConfig(DutConfig &dutConfig)
{
    dutConfig.eyeRelief = ui.doubleSpinBox_EyeRelief->value();
    dutConfig.exeboxOffset = QString::number(ui.doubleSpinBox_EyeOffset->value(), 'f', 2).toStdString();
    dutConfig.moveOutInfo.enable = ui.chkCalculateMvFidsByInputFlag->isChecked();
}

void FiducialCalibrateWidget::loadConfig()
{
    refrashFiducialDisplay();

    DutConfig dutConfig = CalibrationConfig::instance()->loadDutConfig("");
    ui.doubleSpinBox_EyeRelief->setValue(dutConfig.eyeRelief);

    QStringList offsetList = QString::fromStdString(dutConfig.exeboxOffset).split(',');
    if (offsetList.size() > 0)
        ui.doubleSpinBox_EyeOffset->setValue(offsetList[0].toFloat());

    m_moveout = CalibrationConfig::instance()->getDutConfig().moveOutInfo.enable;

    m_eyeboxAcsCoords = CalibrationConfig::instance()->readEyeboxAcsCoords();

    QString exeboxOffset = QString::fromStdString(dutConfig.exeboxOffset);
    QStringList strList = exeboxOffset.split(",");
    if (strList.size() > 0){
        ui.doubleSpinBox_EyeOffset->setValue(strList.at(0).toFloat());
    }

    ui.chkCalculateMvFidsByInputFlag->setChecked(dutConfig.calculateMvFidsByInputFlag);

    // update eyeboxs
    ui.comboBox_eyeboxIndexs->clear();
    for (int i = 0; i < dutConfig.exeboxOffsetAll.size(); ++i)
    {
        ui.comboBox_eyeboxIndexs->addItem(QString::number(i + 1));
    }
}

bool FiducialCalibrateWidget::isAlignWidgetRun()
{
    return ui.btnStop->property("RUNNING").toBool();
}

void FiducialCalibrateWidget::recipeRunStartOrEnd(bool isStart)
{
    setEnabledBtns(!isStart);
    ui.btnStop->setEnabled(!isStart);
}

void FiducialCalibrateWidget::on_bth_calFiducial_clicked()
{
    Result ret = MoveLimitJX::moveMcInitTestPos();
    if (!ret.success) {
        QMessageBox::critical(this, "Calculate MV fiducial", QString::fromStdString(ret.errorMsg), QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    ret = MoveLimitJX::moveDutInitTestPos();
    if (!ret.success)
    {
        QMessageBox::critical(this, "Calculate MV fiducial", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
            QMessageBox::NoButton);
        return;
    }

    DutConfig config = CalibrationConfig::instance()->getDutConfig();

    vector<cv::Point3f> fiducials;
    ret = CaptureFiducialImage().calMvCaptureFiducialCoords2D(fiducials, config.sn, config.calculateMvFidsByInputFlag);
    if (!ret.success)
    {
        QMessageBox::critical(this, "Calculate MV fiducial", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
                              QMessageBox::NoButton);
        return;
    }

    m_fiducialCoords = fiducials;

    QMap<int, cv::Point3f> fiducialMap;
    for (int i = 0; i < fiducials.size(); ++i){
        fiducialMap[i] = fiducials[i];
    }
    refrashFiducialDisplay(fiducialMap);
}

void FiducialCalibrateWidget::on_btnSaveFiducialMotionCoods_clicked()
{
    if (m_devicesConnect)
    {
        std::vector<cv::Point3f> mvFiducials = CalibrationConfig::instance()->getDutConfig().motPosOfFids;
        if (mvFiducials.size() < 3){
            return;
        }

        ML_Point3D point = Motion3DModel::getInstance(withCamera)->getPosition();
        if (ui.rdbFiducial1->isChecked())
        {
            mvFiducials[0] = cv::Point3f(point.x, point.y, point.z);
        }
        else if (ui.rdbFiducial2->isChecked())
        {
            mvFiducials[1] = cv::Point3f(point.x, point.y, point.z);
        }
        else if (ui.rdbFiducial3->isChecked())
        {
            mvFiducials[2] = cv::Point3f(point.x, point.y, point.z);
        }

        CalibrationConfig::instance()->writeMotPosOfFids("", mvFiducials);
        refrashFiducialDisplay();
    }
}

void FiducialCalibrateWidget::on_btnCalibrate_clicked()
{
    MetricsData::instance()->setAlignImageDir("");

    setEnabledBtns(false);
    ui.btnStop->setProperty("RUNNING", true);

    QFuture<Result> future = QtConcurrent::run(this, &FiducialCalibrateWidget::startCalibrate);
    watcherCalibrate.setFuture(future);
}

void FiducialCalibrateWidget::on_btnStop_clicked()
{
    Alignment2D::instance()->continueRun(false);
}

void FiducialCalibrateWidget::on_btnSave_clicked()
{
    

   /* CalibrateConfig &caliConfig = CalibrationConfig::instance()->getCalibrateConfig();

    CalibrationConfig::instance()->writeCalibrationConfig();*/
}

void FiducialCalibrateWidget::on_btnRefresh_clicked()
{
    loadConfig();
}

void FiducialCalibrateWidget::on_btn_move_clicked()
{
    setEnabledBtns(false);
    ui.btnStop->setProperty("RUNNING", true);

    QFuture<Result> future = QtConcurrent::run(this, &FiducialCalibrateWidget::moveSeeFiducial);
    watcherMoveFiducial.setFuture(future);
}

void FiducialCalibrateWidget::on_cmbAlignMode_activated(const QString &text)
{
    ui.cmbFiducialType->setEnabled(ui.cmbAlignMode->currentIndex() == 0);
}

void FiducialCalibrateWidget::on_cmb_mvExposure_activated(const QString &text)
{
    ui.spinBox_mvExposure->setEnabled(ui.cmb_mvExposure->currentIndex() != 0);
}

void FiducialCalibrateWidget::on_comboBox_eyeboxIndexs_activated(const QString &text)
{
    std::vector<cv::Point3f> eyeboxs = Alignment2D::instance()->getEyeboxAcsCoords();
    int index = ui.comboBox_eyeboxIndexs->currentIndex();
    if (eyeboxs.size() > index)
    {
        cv::Point3f point = eyeboxs[index];
        ui.label_eyeboxIndexCoord->setText(
            QString("index %1 (%2,%3,%4)")
            .arg(index + 1).arg(point.x).arg(point.y).arg(point.z));
    }else{
        ui.label_eyeboxIndexCoord->setText("Please calibrate first.");
    }
}

void FiducialCalibrateWidget::on_btn_eyeboxMove_clicked()
{
    std::vector<cv::Point3f> eyeboxs = Alignment2D::instance()->getEyeboxAcsCoords();
    if(eyeboxs.size() == 0){
        QMessageBox::warning(this, "Eyebox move error","Please calibrate first.");
        ui.label_EyeboxCurrentCoord->setText("");
        return;
    }

    setEnabledBtns(false);
    ui.btnStop->setProperty("RUNNING", true);

    Alignment2D::instance()->continueRun(true);
    QFuture<Result> future = QtConcurrent::run(Alignment2D::instance(), &Alignment2D::moveAlignEyebox,
                                               ui.comboBox_eyeboxIndexs->currentIndex());
    watcherMoveFiducial.setFuture(future);
}

void FiducialCalibrateWidget::on_btn_exportCsv_clicked()
{
}

void FiducialCalibrateWidget::on_btn_importCsv_clicked()
{
}

Result FiducialCalibrateWidget::startCalibrate()
{
    Alignment2D::instance()->continueRun(true);

    AlignConfig alignConfig = getAlignConfig();
    AlignBaseInfo2D alignInfo;
    Result result = getAlignData(alignConfig, alignInfo);
    if (!result.success)
    {
        return result;
    }

    Alignment2D::instance()->continueRun(true);
    result = Alignment2D::instance()->alignEyebox(&alignInfo, alignConfig);
    if (!result.success)
    {
        return result;
    }

    //TODO: test
    //m_currrentEyeIndex = 0;
    //result = Alignment2D::instance()->moveAlignEyebox(m_currrentEyeIndex);
    return result;
}

void FiducialCalibrateWidget::eyeboxClicked()
{
    QObject *object = QObject::sender();
    QToolButton *btn_tool = qobject_cast<QToolButton *>(object);
    Alignment2D::instance()->continueRun(true);
    m_eyeboxAcsCoords = Alignment2D::instance()->getEyeboxAcsCoords();
    if (btn_tool->objectName().toLower().contains("center"))
    {
        m_currrentEyeIndex = 0;
    }
    else if (btn_tool->objectName().toLower().contains("up"))
    {
        m_currrentEyeIndex = 1;
    }
    else if (btn_tool->objectName().toLower().contains("down"))
    {
        m_currrentEyeIndex = 2;
    }
    else if (btn_tool->objectName().toLower().contains("left"))
    {
        m_currrentEyeIndex = 3;
    }
    else if (btn_tool->objectName().toLower().contains("right"))
    {
        m_currrentEyeIndex = 4;
    }
    m_currentEyeName = btn_tool->objectName().replace("btn", "");

    Alignment2D::instance()->continueRun(true);
    QFuture<Result> future = QtConcurrent::run(Alignment2D::instance(), &Alignment2D::moveAlignEyebox, m_currrentEyeIndex);
    watcherEyebox.setFuture(future);

    setBtnEnabled(false);
    ui.btnStop->setProperty("RUNNING", true);
}

void FiducialCalibrateWidget::setBtnEnabled(bool enable)
{
    ui.btnCenter->setEnabled(enable);
    ui.btnUp->setEnabled(enable);
    ui.btnDown->setEnabled(enable);
    ui.btnLeft->setEnabled(enable);
    ui.btnRight->setEnabled(enable);
    ui.btn_eyeboxMove->setEnabled(enable);
}

void FiducialCalibrateWidget::calibrateHandleFinished()
{
    setEnabledBtns(true);
    ui.btnStop->setProperty("RUNNING", false);

    Result result = watcherCalibrate.future().result();
    if (result.success)
    {
        m_eyeboxAcsCoords = Alignment2D::instance()->getEyeboxAcsCoords();
        m_currentEyeName = "Center";
        QString eyeboxCoords = "";
        if (m_eyeboxAcsCoords.size() > m_currrentEyeIndex)
            eyeboxCoords = QString("Coords(x:%1;y:%2,z:%3)")
                               .arg(QString::number(m_eyeboxAcsCoords[m_currrentEyeIndex].x, 'f', 3))
                               .arg(QString::number(m_eyeboxAcsCoords[m_currrentEyeIndex].y, 'f', 3))
                               .arg(QString::number(m_eyeboxAcsCoords[m_currrentEyeIndex].z, 'f', 3));
        ui.label_EyeboxCoords->setText(eyeboxCoords);
        ui.label_EyeName->setText("Current Eyebox:" + m_currentEyeName);
        if (ui.chkSaveEye->isChecked())
        {
            CalibrationConfig::instance()->writeEyeboxAcsCoords("",  m_eyeboxAcsCoords);
        }
    }
    else
    {
        if (result.errorCode == 0){
            QMessageBox::critical(this, "Output grating alignment", QString::fromStdString(result.errorMsg));
        }
    }
}

void FiducialCalibrateWidget::eyeboxHandleFinished()
{
    setBtnEnabled(true);
    ui.btnStop->setProperty("RUNNING", false);

    Result result = watcherEyebox.future().result();
    if (result.success)
    {
        QString eyeboxCoords = "";
        if (m_eyeboxAcsCoords.size() > m_currrentEyeIndex)
            eyeboxCoords = QString("Coords(x:%1;y:%2,z:%3)")
                               .arg(QString::number(m_eyeboxAcsCoords[m_currrentEyeIndex].x, 'f', 3))
                               .arg(QString::number(m_eyeboxAcsCoords[m_currrentEyeIndex].y, 'f', 3))
                               .arg(QString::number(m_eyeboxAcsCoords[m_currrentEyeIndex].z, 'f', 3));
        ui.label_EyeboxCoords->setText(eyeboxCoords);

        ui.label_EyeName->setText("Current Eyebox:" + m_currentEyeName);
    }
}

void FiducialCalibrateWidget::fiducialMoveHandleFinished()
{
    setEnabledBtns(true);
    ui.btnStop->setProperty("RUNNING", false);

    Result result = watcherMoveFiducial.future().result();
    if (!result.success){
        QMessageBox::critical(this, "Confirm fiducial", QString::fromStdString(result.errorMsg));
    }
}

void FiducialCalibrateWidget::refrashFiducialDisplay()
{
    CalibrationConfig::instance()->load();
    CalibrateConfig &caliConfig = CalibrationConfig::instance()->getCalibrateConfig();
    refrashFiducialDisplay(caliConfig.acsMvCoordinateOfFiducials);
}

void FiducialCalibrateWidget::moveAlignEyeboxSlot(int index)
{
    if(index >= 0){
        std::vector<cv::Point3f> eyeboxs = Alignment2D::instance()->getEyeboxAcsCoords();
        if (eyeboxs.size() > index)
        {
            cv::Point3f point = eyeboxs[index];
            ui.label_EyeboxCurrentCoord->setText(
                QString("index %1 (%2,%3,%4)").arg(index + 1).arg(point.x).arg(point.y).arg(point.z));
        }
    }else{
        ui.label_EyeboxCurrentCoord->setText("");
    }
}

void FiducialCalibrateWidget::alignEyeboxEndSlot(bool success)
{
    ui.comboBox_eyeboxIndexs->clear();
    std::vector<cv::Point3f> eyeboxs = Alignment2D::instance()->getEyeboxAcsCoords();
    for (int i = 0; i < eyeboxs.size(); ++i)
    {
        ui.comboBox_eyeboxIndexs->addItem(QString::number(i + 1));
    }
}

void FiducialCalibrateWidget::refrashFiducialDisplay(const QMap<int, cv::Point3f> &fiducials)
{
    QString f1Str = "";
    QString f2Str = "";
    QString f3Str = "";
    if (fiducials.size() > 0)
        f1Str = QString("F1(x:%1;y:%2,z:%3)")
                    .arg(QString::number(fiducials[0].x, 'f', 3))
                    .arg(QString::number(fiducials[0].y, 'f', 3))
                    .arg(QString::number(fiducials[0].z, 'f', 3));
    if (fiducials.size() > 1)
        f2Str = QString("F2(x:%1;y:%2,z:%3)")
                    .arg(QString::number(fiducials[1].x, 'f', 3))
                    .arg(QString::number(fiducials[1].y, 'f', 3))
                    .arg(QString::number(fiducials[1].z, 'f', 3));
    if (fiducials.size() > 2)
        f3Str = QString("F3(x:%1;y:%2,z:%3)")
                    .arg(QString::number(fiducials[2].x, 'f', 3))
                    .arg(QString::number(fiducials[2].y, 'f', 3))
                    .arg(QString::number(fiducials[2].z, 'f', 3));
    ui.label_Fiducial->setText(f1Str + "\n" + f2Str + "\n" + f3Str);
}
