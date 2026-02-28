#include "MeasureWidget.h"
#include "PrjCommon/WaitWidget.h"
#include "Core/loggingwrapper.h"
#include <QFileDialog>
#include <QtConcurrent>
#include "ImageRotationConfig.h"

//#include "IQMetrics/ChromacityWidget.h"
//#include "IQMetrics/ContrastWidget.h"
//#include "IQMetrics/CraWidget.h"
//#include "IQMetrics/GhostWidget.h"
//#include "IQMetrics/LumEfficiencyWidget.h"
//#include "IQMetrics/LuminanceWidget.h"
//#include "IQMetrics/MtfWidget.h"
#include "ImageSwitch.h"
//#include "MLAlgorithm/ml_image_public.h"
#include "TaskAsync.h"
#include"ImageDetection/ml_image_public.h"
#include "PrjCommon/PrjCommon.h"
#include "PrjCommon/metricsdata.h"
#include "Core/imode.h"
#include "Core/modemanager.h"
#include "RecipeXMLEngine/RecipeXMLEngine.h"

MeasureWidget::MeasureWidget(QWidget *parent)
{
    ui.setupUi(this);

    //MTFModelConfig::instance()->load(mtfconfigFile.toStdString().c_str());
    IQMetricConfig::instance()->load("config/IQMetricConfig.json");
    qRegisterMetaType<QList<QImage>>("QList<QImage>");
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<MLStatus>("MLStatus");


    initAccordion();
    initUI();
    initFovROI();
    setQss(":/qss/uistyle.qss");
    initLiveImageRadio();

    connect(MLColorimeterMode::Instance(), SIGNAL(connectStatus(bool)), this, SLOT(connectStatus(bool)));
    connect(MLColorimeterMode::Instance(), SIGNAL(stopGrab(bool)), this, SLOT(stopGrabSlot(bool)));

    connect(this, SIGNAL(enableWidget(QWidget *, bool)), this, SLOT(setWidgetEnable(QWidget *, bool)));
    connect(this, SIGNAL(messageShow(QString)), this, SLOT(showConnectWarning(QString)));
    connect(&watcher, SIGNAL(finished()), this, SLOT(handleFinished()));
    connect(Core::PrjCommon::instance(), SIGNAL(connectAllDevicesSig()), this, SLOT(connectCameraSlot()));

    connect(Core::PrjCommon::instance(), SIGNAL(updateDutConfigSignal()), this, SLOT(updateDutConfigSlot()));
    //TaskAsync::instance();
}

void MeasureWidget::setQss(QString filename)
{
    QFile file(filename);
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    this->setStyleSheet(stylesheet);
    file.close();
}

void MeasureWidget::initAccordion()
{
    auto *myAccordion = new QAccordion();
    m_accordion = myAccordion;
    this->ui.widget_accordion->layout()->addWidget(myAccordion);

    ui.viewer->setContextMenuPolicy(Qt::CustomContextMenu);
    menu = new QMenu(this);

    // TODO: Add MTF, Luminance, lumEfficency, etc calculate widget
    //MtfWidget *mtfWidget = new MtfWidget(ui.viewer, menu, myAccordion);
    //LuminanceWidget *lumiWidget = new LuminanceWidget(ui.viewer, menu, myAccordion);
    //LumEfficiencyWidget *lumEfficencyWidget = new LumEfficiencyWidget(ui.viewer, menu, myAccordion);
    //ChromacityWidget *chromacityWidget = new ChromacityWidget(ui.viewer, menu, myAccordion);
    //ContrastWidget *contrastWidget = new ContrastWidget(ui.viewer, menu, myAccordion);
    //CraWidget *craWidget = new CraWidget(ui.viewer, menu, myAccordion);
    //GhostWidget *ghostWidget = new GhostWidget(ui.viewer, menu, myAccordion);

    //m_iqMetricsWidgets << mtfWidget << lumiWidget << lumEfficencyWidget << chromacityWidget << contrastWidget
    //                   << craWidget << ghostWidget;

    //m_iqMetricsWidgets  << contrastWidget << craWidget;

    for (MetricsWidgetBase *widget : m_iqMetricsWidgets)
    {
        widget->setMeasureWidgetUI(&ui);
    }

    //connect(this, SIGNAL(loadImageSignal(const QString &)), lumEfficencyWidget, SLOT(loadImageSlot(const QString &)));  
    connect(ui.viewer, SIGNAL(customContextMenuRequested(const QPoint &)), this,
            SLOT(graphViewcustomContextMenuRequested(const QPoint &)));
}

void MeasureWidget::initUI()
{
    ui.radioBtn_liveImage->setChecked(false);
    ui.btn_unfold->setIcon(QIcon(":/image/image/double-left.png"));
    ui.btn_fold->setIcon(QIcon(":/image/image/double-right.png"));
    ui.btn_unfold->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui.btn_fold->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui.btn_unfold->setVisible(false);

    ui.widget_rgb->setHidden(true);
    ui.viewer->SetImageModel(MTF);
    ui.checkBox_SLBImgRotate->setChecked(true);
    if (!ImageRotationConfig::instance()->isRotation()){
        ui.checkBox_SLBImgRotate->hide();
    }

    QHBoxLayout *toolWidgetLayout = new QHBoxLayout();
    ui.viewTool->setLayout(toolWidgetLayout);
    // ui.topWidget->setStyleSheet("QWidget{background-color:#FFFFFF}");
    m_toolBar = new QToolBar(this);

    //m_toolBar->setStyleSheet("QToolButton { padding-right: 30px; }"
    //                         "QToolButton:hover,QToolButton:pressed,QToolButton:checked {background-color:#adceda;}"
    //                         "QToolBar{background-color:#FFFFFF;color:#000000;}");
    m_toolBar->setFixedHeight(25);
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    toolWidgetLayout->addWidget(m_toolBar);
    toolWidgetLayout->setMargin(0);

    QList<QAction *> actionList;
    //QAction *histogramAct = new QAction(m_toolBar);
    //histogramAct->setText("Histogram");
    //m_toolBar->addAction(histogramAct);
    //histogramAct->setCheckable(true);
    ////histogramAct->setIcon(QIcon(":/icon/images/Save.png"));
    //actionList.append(histogramAct);

    QAction *profileAct = new QAction(m_toolBar);
    profileAct->setText("Plot Profile");
    m_toolBar->addAction(profileAct);
    profileAct->setCheckable(true);
    //profileAct->setIcon(QIcon(":/icon/images/selection.png"));
    actionList.append(profileAct);

    QAction *minmaxAct = new QAction(m_toolBar);
    minmaxAct->setText("Min-Max");
    m_toolBar->addAction(minmaxAct);
    minmaxAct->setCheckable(true);
    //minmaxAct->setIcon(QIcon(":/icon/images/Move.png"));
    actionList.append(minmaxAct);
    //QActionGroup *Actiongroup = new QActionGroup(this);
    for (int i = 0; i < actionList.size(); i++)
    {
        connect(actionList.at(i), &QAction::changed, this, &MeasureWidget::onActionToggled);
    }
    m_lineCutWindow = new LineCutWidget();
    connect(this, &MeasureWidget::sigPlotProfileChanged, ui.viewer, &CameraViewer::setLineItemVisible);
    connect(this, &MeasureWidget::sigMinMaxStaChanged, ui.viewer, &CameraViewer::calMinMaxSta);
    connect(this, &MeasureWidget::sigHistogramChanged, ui.viewer, &CameraViewer::calHistogram);
    //connect(this, &MeasureWidget::sigHistogramChanged,ui.viewer,&CameraViewer::set);
    if (m_lineCutWindow)
    {
        m_lineCutWindow->setAction(profileAct);
        connect(ui.viewer, &CameraViewer::sigUpdateCutLine, m_lineCutWindow, &LineCutWidget::onUpdateCutLine,Qt::ConnectionType::QueuedConnection);
        connect(ui.viewer, &CameraViewer::showProFileWindow, m_lineCutWindow, &LineCutWidget::setVisible);
    }
    //m_histogramWidget = new HistogramWidget();
    //if (m_histogramWidget)
    //{
    //    m_histogramWidget->setAction(histogramAct);
    //    connect(ui.viewer, &CameraViewer::sigHistogramUpdate, m_histogramWidget, &HistogramWidget::calculateHistogram);    
    //}
    m_MinMaxStaWidget = new MinMaxStaWidget();
    if (m_MinMaxStaWidget)
    {
        m_MinMaxStaWidget->setAction(minmaxAct);
        connect(ui.viewer, &CameraViewer::sigMinMaxUpdate, m_MinMaxStaWidget, &MinMaxStaWidget::updateMinMaxSta);
    }
    ui.showFOV->hide();
    ui.groupBox->hide();
    connect(this, SIGNAL(setImageRotateModeSignal(bool)), this, SLOT(setImageRotateModeSlot(bool)));
    //m_autoFocusWidget = new AutoFocusWidget(tr("Auto Focus"), this);

    on_btn_fold_clicked();


    Core::IMode* mode = Core::ModeManager::instance()->findMode("Recipe.RecipeXMLMode");
    connect(static_cast<RecipeXMLEngine::Internal::RecipeXMLMode*>(mode)->GetRecipe2Plugin(), &Recipe2::updateTreeSystemIsRunDone,
        this, [this](bool value) {
            recipeRunStartOrEnd(!value);
        });
    //connect(Core::PrjCommon::instance(), SIGNAL(recipeRunStartOrEnd(bool)), this, SLOT(recipeRunStartOrEnd(bool)));

    ui.btn_fold->setVisible(false);
    ui.btn_unfold->setVisible(false);
    ui.widget2->setVisible(false);
}

void MeasureWidget::initFovROI()
{
    ui.doubleSpinBox_fovROIStep->setValue(5);
    ui.doubleSpinBox_fovCenterX->setValue(0);
    ui.doubleSpinBox_fovCenterY->setValue(0);
    ui.doubleSpinBox_fovWidthX->setValue(35);
    ui.doubleSpinBox_fovHeightY->setValue(35);
    
    ui.doubleSpinBox_fovCenterX->setMinimum(-60);
    ui.doubleSpinBox_fovCenterY->setMinimum(-60);
    ui.doubleSpinBox_fovCenterX->setMaximum(60);
    ui.doubleSpinBox_fovCenterY->setMaximum(60);

    ui.doubleSpinBox_fovWidthX->setMinimum(5);
    ui.doubleSpinBox_fovHeightY->setMinimum(5);
    ui.doubleSpinBox_fovWidthX->setMaximum(60);
    ui.doubleSpinBox_fovHeightY->setMaximum(60);

    ui.doubleSpinBox_fovROIStep->setMinimum(1);
}

void MeasureWidget::updateIqMetricsModel(bool offline)
{
    for (MetricsWidgetBase *widget: m_iqMetricsWidgets){
        widget->iqMetricsModeChange(offline);
    }
}

void MeasureWidget::setWidgetEnable(QWidget *wgt, bool enable)
{
    wgt->setEnabled(enable);
}

void MeasureWidget::on_showCross_stateChanged(int state)
{
    if (state)
    {
        ui.viewer->EnableCross();
    }
    else
    {
        ui.viewer->UnenableCross();
    }
}

void MeasureWidget::on_showFOV_stateChanged(int state)
{
    if (state)
    {
        ui.viewer->EnableFOV();
    }
    else
    {
        ui.viewer->UnenableFOV();
    }
}

void MeasureWidget::on_saveimgbtn_clicked()
{
    if (!MLColorimeterMode::Instance()->IsConnect())
    {
        QMessageBox::warning(this, "Save image error", "Please connect camera first.", QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(
        this, QStringLiteral("Save image:"), "d:", QStringLiteral("(*.png *.jpg *.xpm  *.tif);;All files (*.*)"));
    if (!fileName.isEmpty())
    {
        cv::Mat img = MLColorimeterMode::Instance()->GetCamera()->GetImage();
        if (!img.empty())
        {
            vector<int> compression_params;

            compression_params.push_back(IMWRITE_TIFF_COMPRESSION);
            compression_params.push_back(1);
            cv::imwrite(fileName.toStdString(), img, compression_params);
        }
    }
}

void MeasureWidget::postInitalize()
{
    qRegisterMetaType<std::vector<cv::Mat>>("std::vector<cv::Mat>");
}

void MeasureWidget::NotifyCameraStateChanged(MLCameraState old_state, MLCameraState new_state)
{
    if (old_state != MLCameraState::kDisconnected && new_state == MLCameraState::kConnected)
    {
        on_disconnectCamera_clicked();
    }
}

void MeasureWidget::NotifyCameraFrameReceived(cv::Mat img, MLPixelFormat format)
{
    if (ui.viewer != nullptr)
    {
        cv::Mat matDest = img.clone();
        if (!img.empty() && ImageRotationConfig::instance()->isRotation())
        {
            Result ret/* = MLColorimeterMode::Instance()->imageRotation(img, matDest)*/;
            if (!ret.success)
            {
                LoggingWrapper::instance()->error(QString::fromStdString(ret.errorMsg));
            }
        }

        ui.viewer->Enqueue(matDest);
        emit MLColorimeterMode::Instance()->updateCameraImageSignal(matDest.clone());
    }
}

void MeasureWidget::NotifyCameraGrayLevel(int gray_level)
{
    emit MLColorimeterMode::Instance()->updateCameraGrayLevelSignal(gray_level);
}

void MeasureWidget::on_connectCamera_clicked()
{
    WaitWidget::instance()->startAnimation();
    QFuture<Result> future = QtConcurrent::run(this, &MeasureWidget::connectCamera);
    watcher.setFuture(future);
}

Result MeasureWidget::connectCamera()
{
    emit enableWidget(ui.connectCamera, false);
    emit enableWidget(ui.disconnectCamera, false);

    Result ret = MLColorimeterMode::Instance()->ConnectAsync();
    if (!ret.success)
    {
        emit messageShow(QString::fromStdString(ret.errorMsg));
        emit enableWidget(ui.connectCamera, true);
        emit enableWidget(ui.disconnectCamera, true);
        return ret;
    }
    emit enableWidget(ui.connectCamera, true);
    emit enableWidget(ui.disconnectCamera, true);
    return ret;
}

void MeasureWidget::setImageRotateMode(bool isSLB)
{
    emit setImageRotateModeSignal(isSLB);
    updateAllFOVCenter(isSLB);
}

void MeasureWidget::initLiveImageRadio()
{
    ui.radioBtn_liveImage->setChecked(false);
    on_checkBox_SLBImgRotate_stateChanged(true);
}

void MeasureWidget::subscribeCamera(bool flag)
{
    static bool preSub = false;

    GrabberBase *camera = MLColorimeterMode::Instance()->GetCamera();
    if (camera == nullptr)
    {
        return;
    }

    if(flag){
        if(!preSub){
            camera->Subscribe(MLCameraEvent::kFrameReceived, this);
            camera->Subscribe(MLCameraEvent::kStateChanged, this);
            //camera->Subscribe(MLCameraEvent::kGrayLevel, this);
        }
        preSub = true;
    }else{
        if (preSub)
        {
            camera->Unsubscribe(MLCameraEvent::kFrameReceived, this);
            camera->Unsubscribe(MLCameraEvent::kStateChanged, this);
            // camera->Unsubscribe(MLCameraEvent::kGrayLevel, this);
        }
        preSub = false;
    }
}

QMap<int, cv::Point2f> MeasureWidget::updateFOVCenter()
{
    //CORE::Rotation rotation;
    //CORE::Mirror mirror;
    //McBinocularMode::instance()->getRotationMirror(rotation, mirror);
    //int rotationAngle = 0;
    //MirrorALG m = MLImageDetection::NO_OP;
    //if (rotation == CORE::R90)
    //    rotationAngle = 90;
    //else if (rotation == CORE::R180)
    //    rotationAngle = 180;
    //else if (rotation == CORE::R270)
    //    rotationAngle = 270;
    //if (mirror == CORE::LEFT_RIGHT)
    //    m = MLImageDetection::LEFT_RIGHT;
    //else if (mirror == CORE::UP_DOWN)
    //    m = MLImageDetection::UP_DOWN;

    cv::Point2f base /*= McBinocularMode::instance()->getFOVCenter()*/;
    QMap<int, cv::Point2f> centerMap;
    MLimagePublic lu;
    //centerMap[1] = lu.updateOpticalCenter(base, cv::Size(11264 , 9200), rotationAngle, m);
    //centerMap[2] = lu.updateOpticalCenter(base / 2.0, cv::Size(5632, 4600), rotationAngle, m);
    //centerMap[4] = lu.updateOpticalCenter(base / 4.0, cv::Size(2816, 2300), rotationAngle, m);
    return centerMap;
}

void MeasureWidget::updateAllFOVCenter(int isSLB)
{
    if (isSLB)
    {
        MetricsData::instance()->setRotationMirrorType(RMT_SLB);
    }
    else
    {
        ROTATION_MIRROR_TYPE type = MetricsData::instance()->getDutEyeType() == 0 ? RMT_DUT_LEFT : RMT_DUT_RIGHT;
        MetricsData::instance()->setRotationMirrorType(type);
    }

    QMap<int, cv::Point2f> centerMap = updateFOVCenter();
    ui.viewer->setFOVCenter(centerMap);

    for (MetricsWidgetBase *widget : m_iqMetricsWidgets)
    {
        widget->setFovCenter(centerMap);
    }
}

void MeasureWidget::on_disconnectCamera_clicked()
{
    emit enableWidget(ui.connectCamera, false);
    emit enableWidget(ui.disconnectCamera, false);

    subscribeCamera(false);
    Sleep(200);
    MLColorimeterMode::Instance()->Disconnect();

    emit enableWidget(ui.connectCamera, true);
    emit enableWidget(ui.disconnectCamera, true);

    ui.radioBtn_liveImage->setChecked(false);
}
void MeasureWidget::connectCameraSlot()
{
    connectCamera();
}
void MeasureWidget::connectStatus(bool connected)
{
    //TODO: update
    //if (connected)
    //{
    //    ui.connectStatus->setText("Connect Success.");
    //}
    //else
    //{
    //    ui.connectStatus->setText("Not connected.");
    //}

    if (!connected)
    {
        subscribeCamera(false);
        LoggingWrapper::instance()->info("Measure camera disconnected.");
    }
    else
    {
        //ui.viewer->SetImageModel(MTF);
        ui.radioBtn_liveImage->setChecked(true);
        m_accordion->setProperty(IQ_METRICS_TEST_MODE_OFFLINE, false);

        subscribeCamera(true);

        LoggingWrapper::instance()->info("Measure camera connect success.");
    }
}
void MeasureWidget::stopGrabSlot(bool stop)
{
    if (ui.radioBtn_liveImage->isChecked() == stop)
    {
        ui.radioBtn_liveImage->setChecked(!stop);
    }

    if (ui.radioBtn_liveImage->isChecked())
    {
        //ui.viewer->SetImageModel(MTF);
        m_accordion->setProperty(IQ_METRICS_TEST_MODE_OFFLINE, false);
    }
    else
    {
        //ui.viewer->SetImageModel(IQ1);
    }
}
void MeasureWidget::setImageRotateModeSlot(bool isSLB)
{
    ui.checkBox_SLBImgRotate->setChecked(isSLB);
}
void MeasureWidget::updateDutConfigSlot()
{
    if (ui.checkBox_SLBImgRotate->isChecked())
    {
        ui.checkBox_SLBImgRotate->setChecked(false);
    }
    else{
        on_checkBox_SLBImgRotate_stateChanged(false);
    }
}
void MeasureWidget::onActionToggled()
{
    QAction *action = static_cast<QAction *>(sender());
    if(ui.viewer->currentImage().empty()){
        action->setChecked(false);
        return;
    }

    bool checked = action->isChecked();
/*    if (action->text() == "Histogram")
    {
        m_histogramWidget->setVisible(checked);
        emit sigHistogramChanged(checked);
    }
    else */if (action->text() == "Plot Profile")
    {
        m_lineCutWindow->setVisible(checked);
        emit sigPlotProfileChanged(checked);

    }
    else if (action->text() == "Min-Max")
    {
        m_MinMaxStaWidget->setVisible(checked);
        emit sigMinMaxStaChanged(checked);
    }
}
void MeasureWidget::recipeRunStartOrEnd(bool isStart)
{
    ui.radioBtn_liveImage->setEnabled(!isStart);
    ui.connectCamera->setEnabled(!isStart);
    ui.disconnectCamera->setEnabled(!isStart);
}
void MeasureWidget::on_loadPictureButton_clicked()
{
    //m_histogramWidget->close();
    m_lineCutWindow->close();
    //m_MinMaxStaWidget->close();

    if (ui.radioBtn_liveImage->isChecked()){
        QMessageBox::warning(this, "Load image error", "Please stop live image first.", QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(
        this, QStringLiteral("Load image:"), "d:", QStringLiteral("(*.png *.jpg *.xpm  *.tif *.bmp);;All files (*.*)"));

    if (fileName.isEmpty()){
        return;
    }

    curNineSquareImg = cv::imread(fileName.toStdString().c_str(), cv::IMREAD_UNCHANGED);

    //ui.set
    ui.viewer->Enqueue(curNineSquareImg);
    if (fileName != "")
    {
        on_showFOV_stateChanged(0);
        ui.showFOV->setChecked(false);    
    }

    emit loadImageSignal(fileName);
}

void MeasureWidget::handleFinished()
{
    WaitWidget::instance()->stopAnimation();

    QFuture<Result> future = watcher.future();
    Result ret = future.result();
    if (ret.success)
    {
        MetricsData::instance()->addConnectedDevicesNum();
        //LoggingWrapper::instance()->info("Measure camera connection success.");
    }else{
        LoggingWrapper::instance()->error(QString("Measure camera connection failed, %1").arg(QString::fromStdString(ret.errorMsg)));
    }
}

void MeasureWidget::on_btn_unfold_clicked()
{
     ui.btn_fold->setVisible(true);
     ui.btn_unfold->setVisible(false);
     ui.widget2->setVisible(true);
}

void MeasureWidget::on_btn_fold_clicked()
{
    ui.btn_unfold->setVisible(true);
    ui.btn_fold->setVisible(false);
    ui.widget2->setVisible(false);
}

void MeasureWidget::on_radioBtn_liveImage_toggled()
{
    bool isChecked = ui.radioBtn_liveImage->isChecked();
    if (isChecked)
    {
        if (!MLColorimeterMode::Instance()->IsConnect())
        {
            ui.radioBtn_liveImage->setChecked(false);
            QMessageBox::warning(this, "Set live image error", "Please connect camera first.", QMessageBox::Ok,
                                 QMessageBox::NoButton);
            return;
        }
    }

    MLColorimeterMode::Instance()->StopGrabbing(!isChecked);

    bool offline = !isChecked;
    m_accordion->setProperty(IQ_METRICS_TEST_MODE_OFFLINE, offline);
    updateIqMetricsModel(offline);
}

void MeasureWidget::on_checkBox_SLBImgRotate_stateChanged(int state)
{
    updateAllFOVCenter(state);

    if (ui.showFOV->isChecked())
    {
        ui.showCross->setChecked(false);
        ui.showFOV->setChecked(false);
        //ui.showFOV->setChecked(true);
    }
}

void MeasureWidget::on_btn_takeRGB_clicked()
{
    cv::Mat rgbImg;
    //cv::Mat xImage = cv::imread("D:/00_RGB/bin4x4/R_X.tif", cv::IMREAD_UNCHANGED);
    //cv::Mat yImage = cv::imread("D:/00_RGB/bin4x4/R_Y.tif", cv::IMREAD_UNCHANGED);
    //cv::Mat zImage = cv::imread("D:/00_RGB/bin4x4/R_Z.tif", cv::IMREAD_UNCHANGED);

    //cv::Mat xImage = cv::imread("D:/00_RGB/R_X.tif", cv::IMREAD_UNCHANGED);
    //cv::Mat yImage = cv::imread("D:/00_RGB/R_Y.tif", cv::IMREAD_UNCHANGED);
    //cv::Mat zImage = cv::imread("D:/00_RGB/R_Z.tif", cv::IMREAD_UNCHANGED);

    //cv::Mat xImage = cv::imread("D:/00_RGB/G_X.tif", cv::IMREAD_UNCHANGED);
    //cv::Mat yImage = cv::imread("D:/00_RGB/G_Y.tif", cv::IMREAD_UNCHANGED);
    //cv::Mat zImage = cv::imread("D:/00_RGB/G_Z.tif", cv::IMREAD_UNCHANGED);

    //cv::Mat xImage = cv::imread("D:/00_RGB/B_X.tif", cv::IMREAD_UNCHANGED);
    //cv::Mat yImage = cv::imread("D:/00_RGB/B_Y.tif", cv::IMREAD_UNCHANGED);
    //cv::Mat zImage = cv::imread("D:/00_RGB/B_Z.tif", cv::IMREAD_UNCHANGED);

    //cv::Mat xImage = cv::imread("D:/00_RGB/W_X.tif", cv::IMREAD_UNCHANGED);
    //cv::Mat yImage = cv::imread("D:/00_RGB/W_Y.tif", cv::IMREAD_UNCHANGED);
    //cv::Mat zImage = cv::imread("D:/00_RGB/W_Z.tif", cv::IMREAD_UNCHANGED);

    cv::Mat xImage = cv::imread("D:/00_RGB/new_rgb/X.tif", cv::IMREAD_UNCHANGED);
    cv::Mat yImage = cv::imread("D:/00_RGB/new_rgb/Y.tif", cv::IMREAD_UNCHANGED);
    cv::Mat zImage = cv::imread("D:/00_RGB/new_rgb/Z.tif", cv::IMREAD_UNCHANGED);

    qCritical() << "0000 0000 start: "<< QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"); 
    ImageSwitch().xyz2rgbMat(rgbImg, xImage, yImage, zImage);
    qCritical() << "0000 0000 end  : " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"); 

    ui.viewer->Enqueue(rgbImg);
}

void MeasureWidget::showConnectWarning(QString message)
{
    QMessageBox::warning(this, tr("Connect Error"), message);
}

MeasureWidget::~MeasureWidget()
{
    subscribeCamera(false);

    delete MLColorimeterMode::Instance();

    TaskAsync::instance().stopThread();
}
void MeasureWidget::graphViewcustomContextMenuRequested(const QPoint &pos)
{
    QPoint clickPos = this->cursor().pos();
    curpost.setX(pos.x());
    curpost.setY(pos.y());
    menu->exec(clickPos);
}