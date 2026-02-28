#include "calibratewidget.h"
#include "AlignUtils.h"
#include "piMotionActor/CalibrationConfig.h"
#include "piMotionActor/ConfigItem.h"
#include "MLKeyenceLRF/MLKeyenceCL.h"
#include "Core/icore.h"
#include "pluginsystem/pluginspec.h"
#include "Core/loggingwrapper.h"
#include "piMotionActor/Motion3DModel.h"
#include <QDebug>
#include <QFuture>
#include <QMessageBox>
#include <QThread>
#include <QtConcurrent>
#include <plugins/Core/icore.h>
#include <plugins/PrjCommon/metricsdata.h>
#include <plugins/piMotionActor/Motion2DModel.h>
#include <plugins/piMotionActor/configItem.h>
#include <plugins/piMotionActor/MoveLimitJX.h>
#include "PrjCommon/PrjCommon.h"
#include "AxesTransfer2D.h"
//#include "Alignment2D.h"
#include "MotionProcess.h"
#include "Core/imode.h"
#include "Core/modemanager.h"
#include <Core/coreconstants.h>
#include "RecipeXMLEngine/RecipeXMLEngine.h"
#include "SelectFiducialWidget.h"
#include <plugins/Core/IAction.h>

using namespace Core;
using namespace AAProcess;
CalibrateWidget::CalibrateWidget(QWidget *parent) : QWidget(parent), m_devicesConnectSuccess(false)
{
    ui.setupUi(this);
    init();
    connect(Core::ICore::instance(), &ICore::coreAboutToClose, this, [this]() { 
        delete m_dockManager; 

        //delete Motion2DModel::getInstance(ACS2DDUT);
        //delete Motion2DModel::getInstance(ACS2DPro);
        MLKeyenceCL::MakeRangeFinder()->threadStop();
        });
    connect(&MotionProcess::getInstance(), SIGNAL(messageBoxSignal(QString)), this, SLOT(messageBoxSlot(QString)));
    connect(&MotionProcess::getInstance(), &MotionProcess::onSignalgetFiducialPos, this, &CalibrateWidget::onSlotgetFiducialPos, Qt::BlockingQueuedConnection);

    emit Core::PrjCommon::instance()->updateDutConfigSignal();
}

CalibrateWidget::~CalibrateWidget()
{
    releaseFiducials();
}

Result CalibrateWidget::deviceConnect(bool isBtn)
{
    std::string msg;
    Result result;
    LoggingWrapper::instance()->info("Star DUT aligning device connections.");
    m_devicesConnectSuccess = false;
    msg = MotionProcess::getInstance().ConnectMVCamera();
    if (msg != "")
    {
        LoggingWrapper::instance()->error(QString::fromStdString(msg));
        result.success = false;
        result.errorMsg = msg;
        return result;
    }
    msg = MotionProcess::getInstance().ConnectMeasureCameraMotionModule();
    if (msg != "")
    {
        LoggingWrapper::instance()->error(QString::fromStdString(msg));
        result.success = false;
        result.errorMsg = msg;
        return result;
    }
    msg = MotionProcess::getInstance().ConnectDutMotionModule();
    if (msg != "")
    {
        LoggingWrapper::instance()->error(QString::fromStdString(msg));
        result.success = false;
        result.errorMsg = msg;
        return result;
    }
    msg = MotionProcess::getInstance().ConnectProjectorMotionModule();
    if (msg != "")
    {
        LoggingWrapper::instance()->error(QString::fromStdString(msg));
        result.success = false;
        result.errorMsg = msg;
        return result;
    }
    msg = MotionProcess::getInstance().ConnectKeyence();
    if (msg != "")
    {
        LoggingWrapper::instance()->error(QString::fromStdString(msg));
        result.success = false;
        result.errorMsg = msg;
        return result;
    }
    msg = MotionProcess::getInstance().ConnectCollimator();
    if (msg != "")
    {
        LoggingWrapper::instance()->error(QString::fromStdString(msg));
        result.success = false;
        result.errorMsg = msg;
        return result;
    }
    msg = MotionProcess::getInstance().ConnectPLC();
    if (msg != "")
    {
        LoggingWrapper::instance()->error(QString::fromStdString(msg));
        result.success = false;
        result.errorMsg = msg;
        return result;
    }
    m_devicesConnectSuccess = true;
    LoggingWrapper::instance()->info("Output grating aligning device successfully connected.");
    return Result();
}

bool CalibrateWidget::isAlignWidgetRun()
{
    bool ret = fiducialWidget->isAlignWidgetRun();
    if (!ret)
    {
        deleteFiducial();
    }
    return ret;
}


void CalibrateWidget::on_connectCameraBtn_clicked()
{
    LoggingWrapper::instance()->debug("Start devices connection, please wait......");
    waitConnect(true);

    QFuture<Result> future = QtConcurrent::run(this, &CalibrateWidget::deviceConnect, true);
    watcher.setFuture(future);
}

void CalibrateWidget::on_disconnectCameraBtn_clicked()
{
    LoggingWrapper::instance()->info("Devices disconnect.");

    // TODO: to be open
    //   if (!m_devicesConnectSuccess)
    //   {
    //	return;
    //}

    CameraModel::GetInstance()->disConnect();
    //Motion3DModel::getInstance(withCamera)->disconnect();

    m_devicesConnectSuccess = false;
}

void CalibrateWidget::on_cbx_cross_stateChanged(int state)
{
    if (state)
    {
        ui.cameraview->EnableCross();
    }
    else
    {
        ui.cameraview->UnenableCross();
    }
}

void CalibrateWidget::graphViewcustomContextMenuRequested(const QPoint &pos)
{
    //if (!m_devicesConnectSuccess)
    //{
    //    return;
    //}

    m_menu->exec(this->cursor().pos());
}

void CalibrateWidget::init()
{
    initMenu();
    initTabwidget();
    initRadioButton();
    initConfig();
    initButton();
    initActionMenu();
    /* m_cameraModel = new CameraModel(this);*/
    connect(CameraModel::GetInstance(), &CameraModel::updateCameraStreamSignal, this,
            &CalibrateWidget::NotifyCameraFrameReceived);

    ui.cameraview->setContextMenuPolicy(Qt::CustomContextMenu);
    //ui.label->setWordWrap(true);
    connect(ui.cameraview, SIGNAL(customContextMenuRequested(const QPoint &)), this,
            SLOT(graphViewcustomContextMenuRequested(const QPoint &)));
    connect(&MotionProcess::getInstance(), SIGNAL(notifyStopSignal(bool)), this, SLOT(notifyStopSlot(bool)));
    connect(&watcher, SIGNAL(finished()), this, SLOT(handleFinished()));
    connect(&watcherMoveInit, SIGNAL(finished()), this, SLOT(handleFinishedLoad()));
    connect(Core::ICore::instance(), SIGNAL(emgStopBtnClicked()), this, SLOT(emgStopBtnClicked()));
    connect(Core::PrjCommon::instance(), SIGNAL(connectAllDevicesSig()), this, SLOT(on_connectCameraBtn_clicked()));
    connect(timer, SIGNAL(timeout()), this, SLOT(watchAndHandleTime()));
    timer->start(1000);

	Core::IMode* mode = Core::ModeManager::instance()->findMode("Recipe.RecipeXMLMode");
	connect(static_cast<RecipeXMLEngine::Internal::RecipeXMLMode*>(mode)->GetRecipe2Plugin(), &Recipe2::updateTreeSystemIsRunDone,
		this, [this](bool value) {
			recipeRunStartOrEnd(!value);
		});
    //connect(Core::PrjCommon::instance(), SIGNAL(recipeRunStartOrEnd(bool)), this, SLOT(recipeRunStartOrEnd(bool)));

    ui.label->setText("MV camera, DUT/Measure camera move station, Confocal Distance Sensor, DUT/Projector tilt station: ");
}

void CalibrateWidget::initMenu()
{
    m_menu = new QMenu(this);
    QAction *addFiduAction = new QAction(ADD_FIDUCIAL_ACTION_NAME, m_menu);
    QAction *delFiduAction = new QAction(DELETE_FIDUCIAL_ACTION_NAME, m_menu);
    QAction *continueAction = new QAction(CONTINUE_FIDUCIAL_ACTION_NAME, m_menu);
    connect(addFiduAction, SIGNAL(triggered()), this, SLOT(addFiducial()));
    connect(delFiduAction, SIGNAL(triggered()), this, SLOT(deleteFiducial()));
    connect(continueAction, SIGNAL(triggered()), this, SLOT(continueFiducial()));
    m_menu->addAction(addFiduAction);
    m_menu->addAction(delFiduAction);
    m_menu->addAction(continueAction);

    continueAction->setEnabled(false);

    //TODO: test
    //QAction* addProAction = new QAction("Add projector", m_menu);
    //m_menu->addAction(addProAction);
    //connect(addProAction, SIGNAL(triggered()), this, SLOT(addProjector()));
}

void CalibrateWidget::initActionMenu()
{
    IAction* action_dut = new IAction(Constants::TOGGLE_DEVICES_MOVE_INIT_POS_DUT, Constants::M_TOOLS);

    action_dut->setText("Devices Move Init DUT Position");
    connect(action_dut, SIGNAL(triggered()), this, SLOT(deviceMoveInitDUTPosition()));

    IAction* action_slb = new IAction(Constants::TOGGLE_DEVICES_MOVE_INIT_POS_SLB, Constants::M_TOOLS);
    action_slb->setText("Devices Move Init SLB Position");
    connect(action_slb, SIGNAL(triggered()), this, SLOT(deviceMoveInitSLBPosition()));
}

void CalibrateWidget::initTabwidget()
{
    m_dockManager = new ads::CDockManager(ui.widget_2);
    m_dockManager->setMinimumWidth(460);
    m_dockManager->setMinimumHeight(950);
    m_dockManager->setConfigFlag(ads::CDockManager::DockAreaHasCloseButton, false);
    m_dockManager->setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, false);
    m_dockManager->setContextMenuPolicy(Qt::NoContextMenu);
    m_dockManager->ads::CDockManager::setConfigFlag(ads::CDockManager::ActiveTabHasCloseButton, false); 
    addDockWdg("MV Camera", true, alignWidget);
    connect(this, SIGNAL(addFiducialSignal(QPointF)), alignWidget, SLOT(addFiducialSlot(QPointF)));
    connect(this, SIGNAL(deleteFiducialSignal(QPointF)), alignWidget, SLOT(deleteFiducialSlot(QPointF)));
    connect(this, SIGNAL(updateFiducialSignal(QPointF)), alignWidget, SLOT(updateFiducialSlot(QPointF)));
    connect(alignWidget, SIGNAL(deleteFiducialSignal()), this, SLOT(deleteFiducial()));

    //addDockWdg("Alignment", true, fiducialWidget);

    if(m_docks.size() > 0){
        m_docks.at(0)->setAsCurrentTab();
    }
}

void CalibrateWidget::initRadioButton()
{
}

void CalibrateWidget::initConfig()
{
    CalibrationConfig::instance()->load();
    ConfigItem::instance();
}

void CalibrateWidget::waitConnect(bool flag)
{
    ui.connectCameraBtn->setDisabled(flag);
    ui.disconnectCameraBtn->setDisabled(flag);
}

void CalibrateWidget::deviceMoveInitDUTPosition()
{
    std::string state;
    std::string msg = MotionProcess::getInstance().JudgeHolderState(state);
    if (msg != "") {
        QMessageBox::warning(this, "Warning!",QString::fromStdString( msg));
        return;
    }
    if (state != "000") {
        QMessageBox::StandardButton box = QMessageBox::question(
            this, "Question!",
            "Confirm whether to move the 3D motion platform and tip/tilt device to the initial position in DUT mode?");
        if (box == QMessageBox::Yes)
        {

            QFuture<std::string> future = QtConcurrent::run(&MotionProcess::getInstance(), &MotionProcess::LoadDUT);
            watcherMoveInit.setFuture(future);
        }
    }
    else {
        QMessageBox::warning(this, "Warning!", "Please check current mode and your choice!");
    }
}

void CalibrateWidget::deviceMoveInitSLBPosition()
{
    std::string state;
    std::string msg = MotionProcess::getInstance().JudgeHolderState(state);
    if (msg != "") {
        QMessageBox::warning(this, "Warning!", QString::fromStdString(msg));
        return;
    }
    if (state == "000") {
        QMessageBox::StandardButton box = QMessageBox::question(
            this, "Question!",
            "Confirm whether to move the 3D motion platform and tip/tilt device to the initial position in SLB mode?");
        if (box == QMessageBox::Yes)
        {
			QFuture<std::string> future = QtConcurrent::run([&]() {
				std::string message = MotionProcess::getInstance().ConnectMeasureCameraMotionModule();
				if (!message.empty())
					return message;
				message = MotionProcess::getInstance().ConnectDutMotionModule();
				if (!message.empty())
					return message;
				message = MotionProcess::getInstance().ConnectProjectorMotionModule();
				if (!message.empty())
					return message;
                return MotionProcess::getInstance().SLBAlignment();
				});
            watcherMoveInit.setFuture(future);
        }
        return;
    }
    else {
        QMessageBox::warning(this, "Warning!", "Please check current mode and your choice!");
    }
}

Result CalibrateWidget::connectTiltDUT(QString ipEtc)
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
            Result resul = MoveLimitJX::instance()->moveAbsToSafe(withCamera);
            if (!resul.success)
            {
                return resul;
            }
            resul = MoveLimitJX::instance()->moveAbsToSafe(withDUT);
            if (!resul.success)
            {
                return resul;
            }

            Result re= Motion2DModel::getInstance(ACS2DDUT)->Home();
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

Result CalibrateWidget::connectCamera(QString sn)
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

Result CalibrateWidget::connectAcsCamera(QString ipEtc)
{
    if (ipEtc.isEmpty())
    {
        return Result(false, "Motion3D connection failed, IP and other is empty.");
    }

    Result result = Motion3DModel::getInstance(withCamera)->connectAndInit(ipEtc);
    if (!result.success)
    {
        string errMsg = result.errorMsg;
        int index = errMsg.find_first_of(":");
        result.errorMsg = errMsg.substr(0, index);
        return result;
    }
    return Result();
}

void CalibrateWidget::releaseFiducials()
{
    QList<QGraphicsItem *> items = ui.cameraview->items();
    for (QGraphicsItem *item : items)
    {
        FiducialPoint *fid = dynamic_cast<FiducialPoint *>(item);
        if (fid == NULL)
        {
            continue;
        }

        emit deleteFiducialSignal(QPointF(fid->getPoint().x(), fid->getPoint().y()));

        fid->disconnect();
        delete fid;
        fid = nullptr;
    }
}

FiducialPoint *CalibrateWidget::getCurrentFiducial()
{
    QList<QGraphicsItem *> items = ui.cameraview->items();
    for (QGraphicsItem *item : items)
    {
        FiducialPoint *fid = dynamic_cast<FiducialPoint *>(item);
        if (fid != NULL)
        {
            return fid;
        }
    }
    return nullptr;
}

void CalibrateWidget::addDockWdg(QString title, bool handing, QWidget *wdg)
{
    m_dockCounts++;
    ads::CDockWidget *DockWidget = new ads::CDockWidget(title);
    DockWidget->setContextMenuPolicy(Qt::NoContextMenu);
    DockWidget->setFeature(ads::CDockWidget::DockWidgetClosable, false);
    DockWidget->setFeature(ads::CDockWidget::NoDockWidgetFeatures, false);
    DockWidget->setFeature(ads::CDockWidget::DockWidgetForceCloseWithArea, true);
    //DockWidget->setFeature(ads::CDockWidget::CustomCloseHandling, handing);
    DockWidget->setWidget(wdg);
    m_docks.append(DockWidget);
    m_dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, DockWidget);
}

bool CalibrateWidget::isAllFloating()
{
    int size = m_docks.size();
    int count = 0;
    int index = 0;
    while (index < size)
    {
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

void CalibrateWidget::initButton()
{
    ui.btn_left->setIcon(QIcon(":/image/img/double-left.png"));
    ui.btn_right->setIcon(QIcon(":/image/img/double-right.png"));
    ui.btn_left->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui.btn_right->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui.btn_left->setVisible(false);
}

void CalibrateWidget::addFiducial(bool updateScene)
{
    QGraphicsScene *sceneItem = ui.cameraview->scene();
    if (sceneItem != nullptr)
    {
        QPointF scenePos = ui.cameraview->mapToScene(ui.cameraview->getcontextMenuCoord());
        FiducialPoint *fiducial = new FiducialPoint(NULL, QPointF(scenePos.x(), scenePos.y()));
        fiducial->setZValue(1);
        fiducial->setId(1);
        sceneItem->addItem(fiducial);
        connect(fiducial, SIGNAL(postionChanged(qreal, qreal, int)), this, SLOT(updateFidPosTable(qreal, qreal, int)));

        if (updateScene)
        {
            ui.cameraview->scene()->update();
        }
        QAction *action = (QAction *)sender();
        action->setEnabled(false);
        for (QAction *actionTmp : m_menu->actions())
        {
            if (actionTmp->text() == CONTINUE_FIDUCIAL_ACTION_NAME)
            {
                actionTmp->setEnabled(true);
                break;
            }
        }

        emit addFiducialSignal(scenePos);
    }
}

void CalibrateWidget::deleteFiducial()
{
    releaseFiducials();

    for (QAction *action : m_menu->actions())
    {
        if (action->text() == ADD_FIDUCIAL_ACTION_NAME)
        {
            action->setEnabled(true);
        }
        else if (action->text() == CONTINUE_FIDUCIAL_ACTION_NAME)
        {
            action->setEnabled(false);
        }
    }
    ui.cameraview->scene()->update();
}

void CalibrateWidget::continueFiducial()
{
    FiducialPoint *fiducialPoint = getCurrentFiducial();
    if (fiducialPoint == nullptr)
    {
        messageBoxSlot("Fiducial is null, please add fiducial.");
        return;
    }
    QPointF pointF = fiducialPoint->getPoint();

    cv::Point2f fiducialPixel = cv::Point2f(pointF.x(), pointF.y());
    MotionProcess::getInstance().continueRun(true, fiducialPixel);
    deleteFiducial();
}

void CalibrateWidget::updateFidPosTable(qreal x, qreal y, int id)
{
    emit updateFiducialSignal(QPointF(x, y));
}

void CalibrateWidget::NotifyCameraFrameReceived()
{
    QImage img = CameraModel::GetInstance()->GetQImage();
    ui.cameraview->Show(img);
}

void CalibrateWidget::manualFiducialModeToggled(bool checked)
{
    // AlignUtils::setAutoFiducialMode(!checked);
}

void CalibrateWidget::messageBoxSlot(QString message)
{
    QMessageBox *msgBox = new QMessageBox(QMessageBox::Information, "Information", message, QMessageBox::Ok);
    msgBox->exec();

    delete msgBox;
    msgBox = nullptr;
}

void CalibrateWidget::notifyStopSlot(bool isStop)
{
    if (isStop)
    {
        deleteFiducial();
    }
}

void CalibrateWidget::handleFinished()
{
    waitConnect(false);
    QFuture<Result> future = watcher.future();
    Result result = future.result();
    if (result.success)
    {
        MetricsData::instance()->addConnectedDevicesNum();
    }else{
        QMessageBox::critical(this, "Device connect error", QString::fromStdString(result.errorMsg));
    }
}
void CalibrateWidget::handleFinishedLoad()
{
    waitConnect(false);
    QFuture<string> future = watcherMoveInit.future();
    if (future.result()=="")
    {
        QMessageBox::information(this, "Tips","Device init succeed!");
    }
    else {
        QMessageBox::critical(this, "Device connect error", QString::fromStdString(future.result()));
    }
}

void CalibrateWidget::emgStopBtnClicked()
{
    Motion3DModel::getInstance(withCamera)->Motion3DMoveStop();
    Motion3DModel::getInstance(withDUT)->Motion3DMoveStop();
    Motion2DModel::getInstance(ACS2DDUT)->Motion2DMoveStop();
    Motion2DModel::getInstance(ACS2DPro)->Motion2DMoveStop();
    Motion2DModel::getInstance(ACS2DReticle)->Motion2DMoveStop();
}

void CalibrateWidget::watchAndHandleTime()
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
        ui.btn_left->setVisible(true);
        ui.btn_right->setVisible(false);
        floatingFlag = true;
    }
}

void CalibrateWidget::on_btn_right_clicked()
{
    ui.btn_left->setVisible(true);
    ui.btn_right->setVisible(false);
    ui.widget_2->setVisible(false);
}

void CalibrateWidget::on_btn_left_clicked()
{
    ui.btn_right->setVisible(true);
    ui.btn_left->setVisible(false);
    ui.widget_2->setVisible(true);
}

void CalibrateWidget::recipeRunStartOrEnd(bool isStart)
{
    ui.connectCameraBtn->setEnabled(!isStart);
    ui.disconnectCameraBtn->setEnabled(!isStart);

    if (m_docks.size() > 0)
    {
        for (int i = 1; i < m_docks.size(); ++i)
        {
            m_docks.at(i)->setEnabled(!isStart);
        }

        if (isStart){
            m_docks.at(0)->setAsCurrentTab();
        }
    }

    if (fiducialWidget)
    {
        fiducialWidget->recipeRunStartOrEnd(isStart);
    }
}

void CalibrateWidget::addProjector(bool updateScene)
{
    QGraphicsScene* sceneItem = ui.cameraview->scene();
    if (sceneItem != nullptr)
    {
        QPointF scenePos = ui.cameraview->mapToScene(ui.cameraview->getcontextMenuCoord());
        FiducialPoint* fiducial = new FiducialPoint(NULL, QPointF(scenePos.x(), scenePos.y()), 261);
        fiducial->setZValue(1);
        fiducial->setId(1);
        sceneItem->addItem(fiducial);
        connect(fiducial, SIGNAL(postionChanged(qreal, qreal, int)), this, SLOT(updateFidPosTable(qreal, qreal, int)));

        if (updateScene)
        {
            ui.cameraview->scene()->update();
        }
    }
}

QPointF CalibrateWidget::onSlotgetFiducialPos(QImage& image)
{
    SelectFiducialWidget fiddlg;
	fiddlg.setFiducialImage(image);
    if (1 == fiddlg.exec())
    {
        return fiddlg.getFiducialPos();
    }
    else
    {
		return QPointF(0, 0);
    }
	return QPointF(0, 0);
}

void CalibrateWidget::UpdateConfigSlot()
{
    fiducialWidget->loadConfig();
}
