#include "aligmentWidget.h"
#include "circlePoint.h"
#include "piMotionActor\ConfigMAInfo.h"
#include "piMotionActor\configItem.h"
#include <LogPlus.h>
#include <MAalignment/DeviceControlWidget.h>
#include <MAalignment\MAalignmentLogic.h>
#include <PLCControl\PLCController.h>
#include <QtConcurrent>
#include <QtWidgets\qfiledialog.h>
#include <Core/icore.h>
#include <PrjCommon/metricsdata.h>
#include <Core/modemanager.h>
#include <qmessagebox.h>
#include "AlignUtils.h"
#include "Core/loggingwrapper.h"
#include "piMotionActor/MoveLimit.h"
#include "piMotionActor/calibrationconfig.h"
#include "integratingSphere/ReticleMode2D.h"
#include "integratingSphere/integratingspheretcpmodel.h"
#include "PrjCommon/WaitWidget.h"
#include "PrjCommon/PrjCommon.h"
#include "Core/IAction.h"
#include <Core/coreconstants.h>
#include "HistoryDataView\ImageDirectoryWatcher.h"
#include "piMotionActor/MoveLimitJX.h"

using namespace Core;
aligmentWidget::aligmentWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    m_devicesConnectSuccess = false;
    inputAAWidget = new dutPrCalibrateWidget();
    connect(&watcher, SIGNAL(finished()), this, SLOT(handleFinished()));
    connect(&watcherAlign, SIGNAL(finished()), this, SLOT(alignHandleFinished()));
    //connect(&watcherMoveInit, SIGNAL(finished()), this, SLOT(moveInitHandleFinished()));
    connect(Core::PrjCommon::instance(), SIGNAL(connectAllDevicesSig()), this, SLOT(on_connectBtn_clicked()));
    //connect(((DeviceControlWidget *)deviceWidget), SIGNAL(sendDeviceMoveFinshed(bool)), this,
    //        SLOT(UpdateOffsetStatusBar(bool)));
    init();
    initConfig();
    initMenu();
    CreateStatusBar();
    initDock();
    initButton();
    // timer == new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(watchAndHandleTime()));
    timer->start(1000);
    connect(ICore::instance(), &ICore::coreAboutToClose, this, [this]() { 
        on_disconnectBtn_clicked();
        m_dockManager->deleteLater(); 
        });

    initMenuAction();
}

aligmentWidget::~aligmentWidget()
{
    CameraModel::GetInstance()->disConnect();
}

Result aligmentWidget::deviceConnect(bool isBtn)
{
    if (!isBtn)
    {
        if (m_devicesConnectSuccess)
        {
            LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "All devices are connected.");
            return Result();
        }
    }

    // DUT MV camera
    QString sn = ConfigItem::instance()->getMVCameraSn();
    Result result = connectCamera(sn);
    if (!result.success)
    {
        return Result(false, result.errorMsg);
    }

    //// DUT 2D move station
    //QString ipAndOther = ConfigItem::instance()->getMotion2DIpAndOther(ACS2DDUTPlane);
    //result = Motion2DModel::getInstance(ACS2DDUTPlane)->connectAndInit(ipAndOther);
    //if (!result.success)
    //{
    //    return Result(false, "DUT Move Station connect failed, " + result.errorMsg);        
    //}

    //// Measure 3D move station
    //QString mcIp = ConfigItem::instance()->getMotion3DIpAndOther(withCamera);
    //result = Motion3DModel::getInstance(withCamera)->connectAndInit(mcIp);
    //if (!result.success)
    //{
    //    LoggingWrapper::instance()->error(
    //        QString::fromStdString("Measure camera 3D motion connect failed, " + result.errorMsg));
    //    return Result(false, "Measure camera move Station connect failed, " + result.errorMsg);
    //}

    //// projector tilt station
    //QString ipPro = ConfigItem::instance()->getMotion2DIpAndOther(ACS2DPro);
    //result = connectTiltProjector(ipPro);
    //if (!result.success)
    //{
    //    return Result(false, "Projector Tilt Station connect failed, " + result.errorMsg);
    //}

    //// Reticle
    //result = ReticleMode2D::instance()->connect();
    //if (!result.success)
    //{
    //    return Result(false, "Reticle connect failed, " + result.errorMsg);
    //}

    //// Integrating sphere
    //result = IntegratingSphereTCPModel::getInstance()->Connect();
    //if (!result.success)
    //{
    //    return Result(false, "Integrating sphere connect failed, " + result.errorMsg);
    //}
    return Result();
}
void aligmentWidget::on_disconnectBtn_clicked()
{
    LoggingWrapper::instance()->info("Input grating aligning device disconnected.");

    Motion3DModel::getInstance(withDUT)->disconnect();
    Motion3DModel::getInstance(withCamera)->disconnect();

    Motion2DModel::getInstance(ACS2DDUT)->disconnect();
    Motion2DModel::getInstance(ACS2DPro)->disconnect();
    CameraModel::GetInstance()->disConnect();
    m_devicesConnectSuccess = false;
}
void aligmentWidget::on_autoAlignBtn_clicked()
{
    MetricsData::instance()->setAlignImageDir("");
    updateAlignEnabled(false);

    MAalignmentLogic::instance()->continueRun(true);
    
    AAlignConfig alignConfig;
    DUT_EYE_TYPE eyeType = (DUT_EYE_TYPE)ui.comboBox_dutEyeType->currentIndex();
    Result ret = AlignUtils::getAlignConfig(alignConfig, eyeType);
    if (!ret.success)
    {
        QMessageBox::critical(this, "Input grating alignment", QString::fromStdString(ret.errorMsg));
        updateAlignEnabled(true);
        return;
    }

    alignConfig.autoMode = true;
    QFuture<Result> future = QtConcurrent::run(MAalignmentLogic::instance(), &MAalignmentLogic::calibrate, alignConfig);
    watcherAlign.setFuture(future);
}
void aligmentWidget::on_manualAlignBtn_clicked()
{
    MetricsData::instance()->setAlignImageDir("");
    updateAlignEnabled(false);

    MAalignmentLogic::instance()->continueRun(true);

    AAlignConfig alignConfig;
    DUT_EYE_TYPE eyeType = (DUT_EYE_TYPE)ui.comboBox_dutEyeType->currentIndex();
    Result ret = AlignUtils::getAlignConfig(alignConfig, eyeType);
    if (!ret.success)
    {
        QMessageBox::critical(this, "Input grating alignment", QString::fromStdString(ret.errorMsg));
        updateAlignEnabled(true);
        return;
    }

    alignConfig.autoMode = false;
    QFuture<Result> future = QtConcurrent::run(MAalignmentLogic::instance(), &MAalignmentLogic::calibrate, alignConfig);
    watcherAlign.setFuture(future);
}
void aligmentWidget::on_resetLastAlignBtn_clicked()
{
    updateAlignEnabled(false);
    MAalignmentLogic::instance()->continueRun(true);

    //TODO:test
    /**float dutZ;
    AAlignConfig alignConfig;
    DUT_EYE_TYPE eyeType = (DUT_EYE_TYPE)ui.comboBox_dutEyeType->currentIndex();
    Result ret = AlignUtils::getAlignConfig(alignConfig, eyeType);
    if (!ret.success)
    {
        QMessageBox::critical(this, "Input grating alignment", QString::fromStdString(ret.errorMsg));
        updateAlignEnabled(true);
        return;
    }
    QFuture<Result> future = QtConcurrent::run(MAalignmentLogic::instance(), &MAalignmentLogic::calibrateZ, dutZ, alignConfig);*/

    QFuture<Result> future = QtConcurrent::run(MoveLimit::instance(), &MoveLimit::resetLastInputAA);
    watcherAlign.setFuture(future);
}
void aligmentWidget::on_stopAlignBtn_clicked()
{
    MAalignmentLogic::instance()->continueRun(false);
    updateAlignEnabled(true);
}
void aligmentWidget::on_saveImageBtn_clicked()
{
    if (!CameraModel::GetInstance()->isConnected())
    {
        QMessageBox::critical(this, "Save image error", "MV camera is not connected.", QMessageBox::Ok,
                              QMessageBox::NoButton);
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(
        this, QStringLiteral("Save image:"), "d:", QStringLiteral("(*.png *.jpg *.xpm  *.tif);;All files (*.*)"));
    if (!fileName.isEmpty())
    {
        cv::Mat img = CameraModel::GetInstance()->GetImage();
        if (!img.empty())
        {
            vector<int> compression_params;

            compression_params.push_back(cv::ImwriteFlags::IMWRITE_TIFF_COMPRESSION);
            compression_params.push_back(1);
            cv::imwrite(fileName.toStdString(), img, compression_params);
        }
    }
}

void aligmentWidget::on_btn_mvRelIgSave_clicked()
{
    cv::Point3f mcCoord;
    Result ret = MoveLimit::getCoordinate(withCamera, mcCoord);
    if(!ret.success){
        QMessageBox::critical(this, "Save Error", QString::fromStdString(ret.errorMsg));
        return;
    }

    cv::Point3f dutCoord;
    ret = MoveLimit::getCoordinate(withDUT, dutCoord);
    if (!ret.success)
    {
        QMessageBox::critical(this, "Save Error", QString::fromStdString(ret.errorMsg));
        return;
    }

    DUT_EYE_TYPE eyeType = (DUT_EYE_TYPE)ui.comboBox_mRiEyeType->currentIndex();
    cv::Point3f offset = mcCoord - dutCoord;
    ui.lineEdit_mvRelIgResult->setText(QString("%1 , %2 , %3").arg(offset.x).arg(offset.y).arg(offset.z));
}

void aligmentWidget::on_cbx_showCross_stateChanged(int state)
{
    if (state)
    {
        ui.graphicsView->EnableCross();
    }
    else
    {
        ui.graphicsView->UnenableCross();
    }
}

void aligmentWidget::on_connectBtn_clicked()
{
    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Start devices connection, please wait......");
    waitConnect(true);

    QFuture<Result> future = QtConcurrent::run(this, &aligmentWidget::deviceConnectAll, true);
    watcher.setFuture(future);
}
void aligmentWidget::updateDutConfigSlot()
{
    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
    ui.comboBox_dutEyeType->setCurrentIndex(dutConfig.eyeType);
    ui.comboBox_mRiEyeType->setCurrentIndex(dutConfig.eyeType);

    MetricsData::instance()->setDutEyeType(dutConfig.eyeType);
}
void aligmentWidget::waitConnect(bool flag)
{
    ui.connectBtn->setDisabled(flag);
    ui.disconnectBtn->setDisabled(flag);

    updateAlignEnabled(!flag);
}

void aligmentWidget::initConfig()
{
    ConfigItem::instance();
    ConfigMAInfo::GetInstance()->Load();
    ConfigMAInfo::GetInstance()->ReadMAInfo();
}

void aligmentWidget::init()
{
    // m_cameraModel = new CameraModel(this);
    connect(CameraModel::GetInstance(), &CameraModel::updateCameraStreamSignal, this,
            &aligmentWidget::NotifyCameraFrameReceived);

    ui.graphicsView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui.graphicsView, SIGNAL(customContextMenuRequested(const QPoint &)), this,
            SLOT(graphViewcustomContextMenuRequested(const QPoint &)));
    // ui.groupBox_3->setVisible(false);
    
    connect(MAalignmentLogic::instance(), SIGNAL(updateOffset(double, double)), this, SLOT(UpdateStatusBar(double, double)));
    connect(Core::PrjCommon::instance(), SIGNAL(recipeRunStartOrEnd(bool)), this, SLOT(recipeRunStartOrEnd(bool)));
    connect(MAalignmentLogic::instance(), SIGNAL(notifyStopSignal(bool)), this, SLOT(notifyStopSlot(bool)));

    ui.label_deviceNames->setText("MV camera, DUT move station:");
    ui.widget_8->setHidden(true);

    connect(Core::PrjCommon::instance(), SIGNAL(updateDutConfigSignal()), this, SLOT(updateDutConfigSlot()));
    updateDutConfigSlot();
    ui.widget_eyeType->hide();

    //TODO:test
    ui.groupBox_mvRelIgSave->hide();
    cv::Point3f offset = cv::Point3f(1.001,2.002,3.0003);
    ui.lineEdit_mvRelIgResult->setText(QString("%1 , %2 , %3").arg(offset.x).arg(offset.y).arg(offset.z));

    ui.groupBox_3->hide();
}

Result aligmentWidget::connectACSC3D(QString ipEtc)
{
    Result result = Motion3DModel::getInstance(withDUT)->connectAndInit(ipEtc);
    if (!result.success)
    {
        return result;
    }

    return Result();
}

Result aligmentWidget::connectTiltDUT(QString ipEtc)
{
    Result result = Motion2DModel::getInstance(ACS2DDUT)->connectAndInit(ipEtc);
    if (!result.success)
    {
        return result;
    }
    Result res = Motion2DModel::getInstance(ACS2DDUT)->isHoming();
    if (!res.success)
    {
        if (res.errorMsg == "Havn't homing")
        {
            Result resul = MoveLimit::instance()->moveAbsToSafe(withCamera);
            if (!resul.success)
            {
                return resul;
            }
            resul = MoveLimit::instance()->moveAbsToSafe(withDUT);
            if (!resul.success)
            {
                return resul;
            }          

            Result re = Motion2DModel::getInstance(ACS2DDUT)->Home();
            if (!re.success)
            {
                return re;
            }
        }
        else
        {
            return res;
        }
    }
    return Result();
}

Result aligmentWidget::connectTiltProjector(QString ipEtc)
{
    Result result = Motion2DModel::getInstance(ACS2DPro)->connectAndInit(ipEtc);
    if (!result.success)
    {
        return result;
    }
    return Result();
}

void aligmentWidget::handleFinished()
{
    waitConnect(false);

    QFuture<Result> future = watcher.future();
    Result ret = future.result();
    if (ret.success)
    {
        MetricsData::instance()->addConnectedDevicesNum();
    }else{
        QMessageBox::critical(this, "Device connect error", QString::fromStdString(ret.errorMsg));
    }
}

void aligmentWidget::alignHandleFinished()
{
    updateAlignEnabled(true);

    QFuture<Result> future = watcherAlign.future();
    Result ret = future.result();
    if (!ret.success && ret.errorCode == 0)
    {
        QMessageBox::critical(this, "Input grating alignment", QString::fromStdString(ret.errorMsg));
    }
}

//void aligmentWidget::moveInitHandleFinished()
//{
//    //WaitWidget::instance()->stopAnimation();
//
//    QFuture<Result> future = watcherMoveInit.future();
//    Result ret = future.result();
//    if (!ret.success && ret.errorCode == 0)
//    {
//        QMessageBox::critical(this, "Devices move init position failed", QString::fromStdString(ret.errorMsg));
//    }
//    else
//    {
//        QMessageBox::information(this, "Devices move init position", "End of move: move measure camera and DUT motion platform to the initial position.");
//    }
//}

Result aligmentWidget::connectCamera(QString sn)
{
    if (sn.isEmpty())
    {
        return Result(false, "Camera connection failed, SN is empty.");
    }

    Result ret = CameraModel::GetInstance()->connect(sn.toStdString().c_str());
    if (!ret.success)
    {
        CameraModel::GetInstance()->disConnect();
        QThread::msleep(1000);

        ret = CameraModel::GetInstance()->connect(sn.toStdString().c_str());
        if (!ret.success)
        {
            return ret;
        }
    }
    CameraModel::GetInstance()->SubscribeCameraEvent(MLCameraEvent::kFrameReceived);
    CameraModel::GetInstance()->SubscribeCameraEvent(CORE::MLCameraEvent::kGrayLevel);
    return Result();
}

void aligmentWidget::NotifyCameraFrameReceived()
{
    QImage img = CameraModel::GetInstance()->GetQImage();
    ui.graphicsView->Show(img);
}

void aligmentWidget::updateFidPosTable(qreal x, qreal y, int id)
{
    if (id == 1)
    {
        circleInputPoint = QPointF(x, y);
    }
    else
    {
        circleProjectPoint = QPointF(x, y);
    }
}

void aligmentWidget::initMenu()
{
    m_menu = new QMenu(this);
    continueAction = new QAction(CONTINUE_NAME, m_menu);
    continueAction->setVisible(false);
    connect(continueAction, &QAction::triggered, this, &aligmentWidget::continueActionSlot);
    m_menu->addAction(continueAction);

    connect(MAalignmentLogic::instance(), SIGNAL(alignSignal(int, QString)), this, SLOT(alignSlot(int, QString)));
}

void aligmentWidget::initButton()
{
    ui.btn_left1->setIcon(QIcon(":/image/image/double-left.png"));
    ui.btn_right1->setIcon(QIcon(":/image/image/double-right.png"));
    ui.btn_left1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui.btn_right1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui.btn_left1->setVisible(false);
}

void aligmentWidget::initMenuAction()
{
    //IAction* actions_his = new IAction(Constants::TOGGLE_HISTORY_IMAGE_DATA, Constants::M_TOOLS);
    //actions_his->setText("History Image Data");
    //connect(actions_his, &IAction::triggered, [=]() {
    //    QString dir = MetricsData::instance()->getHistoryImagePath();
    //    emit Core::PrjCommon::instance()->historyimageShow(dir); });
}

void aligmentWidget::AddCircle(int id, QString title)
{
    QGraphicsScene *sceneItem = ui.graphicsView->scene();
    if (sceneItem != nullptr)
    {
        QPointF scenePos = ui.graphicsView->mapToScene(ui.graphicsView->getcontextMenuCoord());
        if (scenePos.x() < 10)
        {
            scenePos = QPointF(300, 300);
        }
        CirclePoint *cp = new CirclePoint(NULL, QPointF(scenePos.x(), scenePos.y()));
        cp->setZValue(1);
        cp->setId(id);
        cp->SetTitle(title);
        sceneItem->addItem(cp);
        connect(cp, SIGNAL(postionChanged(qreal, qreal, int)), this, SLOT(updateFidPosTable(qreal, qreal, int)));
    }
}

void aligmentWidget::DeleteCircle(int id)
{
    QGraphicsScene *sceneItem = ui.graphicsView->scene();
    if (sceneItem != nullptr)
    {
        for (QGraphicsItem *item : sceneItem->items())
        {
            CirclePoint *fid = dynamic_cast<CirclePoint *>(item);
            if (fid == NULL)
            {
                continue;
            }
            if (fid->getId() == id)
            {
                sceneItem->removeItem(item);
                fid->disconnect();
                delete fid;
                fid = nullptr;
            }
        }

        ui.graphicsView->scene()->update();
    }
}

int aligmentWidget::GetCircleCount()
{
    int count = 0;
    QGraphicsScene *sceneItem = ui.graphicsView->scene();
    if (sceneItem != nullptr)
    {
        for (QGraphicsItem *item : sceneItem->items())
        {
            CirclePoint *fid = dynamic_cast<CirclePoint *>(item);
            if (fid == NULL)
            {
                continue;
            }
            count++;
        }
    }
    return count;
}

void aligmentWidget::graphViewcustomContextMenuRequested(const QPoint &pos)
{
    m_menu->exec(this->cursor().pos());
}

bool aligmentWidget::isAlignWidgetRun()
{
    bool isRun = ui.stopAlignBtn->property("RUNNING").toBool();
    if(!isRun){
        deleteCircles();
    }
    return isRun;
}

Result aligmentWidget::deviceConnectAll(bool isBtn)
{
    LoggingWrapper::instance()->info("Start input grating aligning device connections.");

    Result ret = deviceConnect(isBtn);
    if(ret.success){
        LoggingWrapper::instance()->info("Input grating aligning device successfully connected.");
        m_devicesConnectSuccess = true;
    }else{
        LoggingWrapper::instance()->error(QString::fromStdString(ret.errorMsg));
        m_devicesConnectSuccess = false;
    }
    return ret;
}

void aligmentWidget::CreateStatusBar()
{
    m_statusBar = new QStatusBar(ui.widget);
    m_AAOffset = new QLabel(this);
    ui.verticalLayout->addWidget(m_statusBar);
    m_statusBar->setStyleSheet("background-color: rgba(0,0, 0,0)");
    m_AAOffset->setStyleSheet("color: black;");
    m_statusBar->addWidget(m_AAOffset);

    m_statusBar->hide();
}

void aligmentWidget::UpdateStatusBar(double x, double y)
{
    m_AAOffset->setText("Offset Pos X: " + QString::number(x, 'f', 2) + ", Y: " + QString::number(y, 'f', 2));
}

void aligmentWidget::UpdateOffsetStatusBar(bool motionFinshed)
{
    if (motionFinshed)
    {
        QFuture<void> future = QtConcurrent::run([=] {
            if (Motion3DModel::getInstance(withDUT)->Motion3DisConnected())
            {
                while (Motion3DModel::getInstance(withDUT)->is3DMoving())
                {
                    _sleep(20);
                }
                QString inputDir = QString("./config/AutoAlignImgs/input.tif");
                cv::Mat inputImage = cv::imread(inputDir.toStdString(), -1);
                cv::Mat projectImage = CameraModel::GetInstance()->GetImage();
                cv::Point2f offsePoint;
                //mlDetect.getInputAndProjectPixelDistance(projectImage, inputImage, offsePoint);
                offsePoint = -offsePoint;
                cv::Point2f offseMtPoint = offsePoint * PIXEL_SIZE / MAGNIFICATION / 1000;
                UpdateStatusBar(-offseMtPoint.x, -offseMtPoint.y);
            }
        });
    }
}
void aligmentWidget::wdgContainerToggle()
{
    ui.widget_2->setVisible(true);
}
void aligmentWidget::watchAndHandleTime()
{
    if (m_dockCounts < m_docks.size() || !(ui.widget_2->isVisible()))
    {
        return;
    }
    if (floatingFlag)
    {
        ui.widget_2->setVisible(true);

        if (isAllFloating())
        {
            return;
        }
        floatingFlag = false;
    }
    if (isAllFloating())
    {
        ui.widget_2->setVisible(false);
        ui.btn_left1->setVisible(true);
        ui.btn_right1->setVisible(false);
        floatingFlag = true;
    }
}
//void aligmentWidget::on_checkBox_stateChanged()
//{
//    bool res = ui.widget_2->isVisible();
//    ui.widget_2->setVisible(!res);
//}
void aligmentWidget::on_btn_left1_clicked()
{
    ui.btn_right1->setVisible(true);
    ui.btn_left1->setVisible(false);
    ui.widget_2->setVisible(true);
}
void aligmentWidget::on_btn_right1_clicked()
{
    ui.btn_left1->setVisible(true);
    ui.btn_right1->setVisible(false);
    ui.widget_2->setVisible(false);
}
void aligmentWidget::messageBoxSlot(QString message)
{
    if(message.isEmpty()){
        return;
    }

    QMessageBox::information(this, "Input grating alignment", message);
}
void aligmentWidget::notifyStopSlot(bool isStop)
{
    if (isStop)
    {
        deleteCircles();
    }
}
void aligmentWidget::alignSlot(int state, QString message)
{
    if (MANUAL_START == state){       
        messageBoxSlot(message);

        AddCircle(1, "Inputgrating");
        AddCircle(2, "Projector");
        continueAction->setVisible(true);
    }
    else if (MANUAL_MIDDLE == state)
    {
        messageBoxSlot(message);
    }
    else if (MALIGN_END == state)
    {
        continueAction->setVisible(false);
        deleteCircles();
    }
}

void aligmentWidget::continueActionSlot()
{
    InputProjectorPixel pixels;
    pixels.input.x = circleInputPoint.x();
    pixels.input.y = circleInputPoint.y();
    pixels.projector.x = circleProjectPoint.x();
    pixels.projector.y = circleProjectPoint.y();
    MAalignmentLogic::instance()->continueRun(true, pixels);
}

void aligmentWidget::recipeRunStartOrEnd(bool isStart)
{
    if(isAlignWidgetRun()){
        return;
    }

    ui.connectBtn->setEnabled(!isStart);
    ui.disconnectBtn->setEnabled(!isStart);

    ui.autoAlignBtn->setEnabled(!isStart);
    ui.manualAlignBtn->setEnabled(!isStart);
    ui.stopAlignBtn->setEnabled(!isStart);
    ui.resetLastAlignBtn->setEnabled(!isStart);
}

void aligmentWidget::initDock()
{
    ui.widget_2->setContextMenuPolicy(Qt::NoContextMenu);
    m_dockManager = new ads::CDockManager(ui.widget_2);
    m_dockManager->setMinimumWidth(460);
    m_dockManager->setMinimumHeight(950);
    m_dockManager->setConfigFlag(ads::CDockManager::DockAreaHasCloseButton, false);
    m_dockManager->setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, false);
    m_dockManager->ads::CDockManager::setConfigFlag(ads::CDockManager::ActiveTabHasCloseButton, false); 
    m_dockManager->ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHideDisabledButtons, true); 

    m_dockManager->setContextMenuPolicy(Qt::NoContextMenu);
    // ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);

    ads::CDockWidget* pDock= addDockWdg("Alignment", false, ui.widget_5);
    //addDockWdg("DutPrNearCalibrate", true, inputAAWidget);
    pDock->setAsCurrentTab();
    // ads::CDockWidget* wdg=addDockWdg("Move Motion", true, ui.widget_3);

    // wdg->setFeature(ads::CDockWidget::DockWidgetFocusable, true);
}
bool aligmentWidget::isAllFloating()
{
    int size = m_docks.size();
    int count = 0;
    int index = 0;
    while (index < size)
    {
        //bool a = connect(m_docks[index], SIGNAL(closeRequested()), ui.widget_2, SLOT(wdgContainerToggle()));
        if (m_docks[index]->isFloating() || m_docks[index]->isInFloatingContainer())
        {
            count++;
        }
        index++;
    }
    if (count == size)
    {
        return true;
    }
    return false;
}
ads::CDockWidget *aligmentWidget::addDockWdg(QString title, bool handing, QWidget *wdg)
{
    m_dockCounts++;
    ads::CDockWidget *DockWidget = new ads::CDockWidget(title);
    QList<QAction *> actions;
    DockWidget->setTitleBarActions(actions);

    DockWidget->setFeature(ads::CDockWidget::DockWidgetClosable, false);
    DockWidget->setFeature(ads::CDockWidget::NoDockWidgetFeatures, false);
    DockWidget->setFeature(ads::CDockWidget::DockWidgetForceCloseWithArea, true);
    DockWidget->setWidget(wdg);
    m_docks.append(DockWidget);
    m_dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, DockWidget);
    return DockWidget;
}
void aligmentWidget::deleteCircles()
{
    QGraphicsScene *sceneItem = ui.graphicsView->scene();
    if (sceneItem != nullptr)
    {
        for (QGraphicsItem *item : sceneItem->items())
        {
            CirclePoint *fid = dynamic_cast<CirclePoint *>(item);
            if (fid == NULL)
            {
                continue;
            }
            
            sceneItem->removeItem(item);
            fid->disconnect();
            delete fid;
            fid = nullptr;
        }

        ui.graphicsView->scene()->update();
    }
}

void aligmentWidget::updateAlignEnabled(bool enabled)
{
    ui.autoAlignBtn->setEnabled(enabled);
    ui.manualAlignBtn->setEnabled(enabled);
    ui.resetLastAlignBtn->setEnabled(enabled);
    ui.stopAlignBtn->setProperty("RUNNING", !enabled);
}

Result aligmentWidget::deviceMoveInit()
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

    result = MoveLimitJX::instance()->moveInit();
    if (!result.success)
    {
        return result;
    }

    //// projector tilt station
    //QString ipPro = ConfigItem::instance()->getMotion2DIpAndOther(ACS2DPro);
    //result = connectTiltProjector(ipPro);
    //if (!result.success)
    //{
    //    return Result(false, "Projector Tilt Station connect failed, " + result.errorMsg);
    //}

    //result = MoveLimit::instance()->moveAbs2DTilt(cv::Point2f(0, 0), ACS2DPro);
    //if (!result.success)
    //{
    //    return result;
    //}

    return Result();
}
