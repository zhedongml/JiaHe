#include "mtfmeasurewidget.h"
#include "..\coreplugin\WaitWidget.h"
#include "MtfWidget.h"
#include "LuminanceWidget.h"
#include "LumEfficiencyWidget.h"
#include "ChromacityWidget.h"
#include "ContrastWidget.h"
#include "CraWidget.h"
#include "GhostWidget.h"

MTFmeasureWidget::MTFmeasureWidget(QWidget *parent)
    : QWidget(parent), SampleWidth(70), SampleHeight(70), m_AxisXmin(0), m_AxisXmax(1), m_AxisYmin(0), m_AxisYmax(1)
{
    ui.setupUi(this);
    /*  timer = new QTimer(this);
      connect(timer, SIGNAL(timeout()), this, SLOT(ReadFrame()));*/

    MTFModelConfig::instance()->load(mtfconfigFile.toStdString().c_str());
    //IQMetricConfig::instance()->load("config/IQMetricConfig.json");
    IQMetricConfig::instance()->load("config/IQMetricConfig_new.json");

    auto *myAccordion = new QAccordion();
    this->ui.widget_4->layout()->addWidget(myAccordion);
    //int firstPaneIndex = myAccordion->addContentPane("MTFInfo");
    //if (firstPaneIndex != -1)
    //{
    //    // get a QFrame* from the Content Pane. This is where we place our content
    //    MtfContentPane = myAccordion->getContentPane(firstPaneIndex);
    //    myAccordion->getContentPane(firstPaneIndex)->clicked();
    //    auto *contentFrame = myAccordion->getContentPane(firstPaneIndex)->getContentFrame();
    //    myAccordion->getContentPane(firstPaneIndex)->setMaximumHeight(1200);
    //    contentFrame->setLayout(new QVBoxLayout());
    //    contentFrame->layout()->addWidget(ui.widget_5);
    //}
    int secondPaneIndex = myAccordion->addContentPane("IQInfo");

    if (secondPaneIndex != -1)
    {
        IQContentPane = myAccordion->getContentPane(secondPaneIndex);
        auto *contentFrame2 = myAccordion->getContentPane(secondPaneIndex)->getContentFrame();
        contentFrame2->setLayout(new QVBoxLayout());
        myAccordion->getContentPane(secondPaneIndex)->setMaximumHeight(500);
        contentFrame2->layout()->addWidget(ui.widget_7);
    }
    ui.comboBox->setEnabled(true);
    getCrossAreaInfo();
    initToShow();
    initChart();
    qRegisterMetaType<QList<QImage>>("QList<QImage>");
    qRegisterMetaType<MLStatus>("MLStatus");
    connect(&CameraModel::GetInstance(), SIGNAL(updateCameraStreamSignal(QList<QImage>)), this,
            SLOT(NotifyCameraFrameReceived(QList<QImage>)));
    connect(&CameraModel::GetInstance(), SIGNAL(updateAutoFocusCurve(MLStatus, double, int *, double *, double *, int)),
            this, SLOT(getAutoFocusDateFromCallback(MLStatus, double, int *, double *, double *, int)));
    connect(ui.comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectCross(int)));
    connect(IQTModel::instance(), SIGNAL(updateResult(cv::Mat)), this, SLOT(updateIQResult(cv::Mat)));
    connect(visionAlgorithm::GetInstance(), &visionAlgorithm::drawResultMTF, this, &MTFmeasureWidget::autoROIUpdate);
    initIQWidget();
    {
        ui.viewer->setContextMenuPolicy(Qt::CustomContextMenu);
        menu = new QMenu(this);
        //addNewROIAction = new QAction("Add ROI", menu);
        //delROIAction = new QAction("Delete ROI", menu);
        //connect(addNewROIAction, SIGNAL(triggered()), this, SLOT(addNewROI()));
        //connect(delROIAction, SIGNAL(triggered()), this, SLOT(delOneROI()));
        //delAllAction = new QAction("DeleteAll ROI", menu);
        //connect(delAllAction, SIGNAL(triggered()), this, SLOT(clearROI()));
        //menu->addAction(delAllAction);
        //menu->addAction(addNewROIAction);
        //menu->addAction(delROIAction);
        ui.widget_7->setEnabled(false);

        // TODO: Add cross area edit widget
        CrossArea *crossArea = new CrossArea(menu, ui, myAccordion);
        connect(crossArea, SIGNAL(refreshAOICOnfig()), this, SLOT(getCrossAreaInfo()));

        // TODO: Add MTF, Luminance, lumEfficency, etc calculate widget
        MtfWidget *mtfWidget = new MtfWidget(ui.viewer, menu, myAccordion);
        LuminanceWidget *lumiWidget = new LuminanceWidget(ui.viewer, menu, myAccordion);
        LumEfficiencyWidget *lumEfficencyWidget = new LumEfficiencyWidget(ui.viewer, menu, myAccordion);
        ChromacityWidget *chromacityWidget = new ChromacityWidget(ui.viewer, menu, myAccordion);
        ContrastWidget *contrastWidget = new ContrastWidget(ui.viewer, menu, myAccordion);
        CraWidget *craWidget = new CraWidget(ui.viewer, menu, myAccordion);
        GhostWidget *ghostWidget = new GhostWidget(ui.viewer, menu, myAccordion);

        connect(this, SIGNAL(loadImageSignal(const QString &)), lumEfficencyWidget, SLOT(loadImageSlot(const QString &)));
        connect(ui.viewer, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(graphViewcustomContextMenuRequested(const QPoint &)));
    }
    {
        connect(ui.manualRadioButton, &QRadioButton::toggled, [=](bool isChecked) {
            if (isChecked == false)
            {
                ui.comboBox->setCurrentIndex(0);
                showManualROIsByAoiId(0);
            }
            else
            {
                ui.viewer->ClearALLROI();
                ui.comboBox->setCurrentIndex(0);
                showManualROIsByAoiId(0);
            }
        });
    }
    scene = ui.viewer->GetScene();
    connect(this, SIGNAL(enableWidget(QWidget *, bool)), this, SLOT(setWidgetEnable(QWidget *, bool)));

    QMenu *menu = new QMenu(this);
    QAction *calv = menu->addAction(tr("cal vertical"));
    calv->setCheckable(true);
    QAction *calh = menu->addAction(tr("cal horizontal"));
    calh->setCheckable(true);
    QActionGroup *Actiongroup = new QActionGroup(this);
    Actiongroup->addAction(calv);
    Actiongroup->addAction(calh);
    Actiongroup->setExclusive(true);
    ui.chooseButton->setMenu(menu);
    connect(ui.chooseButton, &QToolButton::triggered, [&](QAction *act) {
        if (act == calv)
            calvmode = true;
        else
            calvmode = false;
    });

    // test
    ui.widget->setObjectName("widget1");
    ui.widget_4->setObjectName("widget2");
    setQss(":/qss/uistyle.qss");

    connect(&watcher, SIGNAL(finished()), this, SLOT(handleFinished()));
    connect(Core::ICore::instance(), SIGNAL(connectAllDevicesSig()), this, SLOT(connectCamera()));
    
    ui.groupBox_7->hide();
    // ui.showCross->hide();
    // ui.checkBox->hide();
    ui.calMTFButton->hide();
    ui.chooseButton->hide();

    initPI();
}
void MTFmeasureWidget::setQss(QString filename)
{
    QFile file(filename);
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    this->setStyleSheet(stylesheet);
    file.close();
}
void MTFmeasureWidget::updateIQResult(cv::Mat IQre)
{
    // int nRow = ui.tableWidget->rowCount();
    // int nColumn = ui.tableWidget->columnCount();
    // for (int row = 0; row < nRow; row++)
    //    ui.tableWidget->removeRow(row);
    // for (int col = 0; col < nColumn; col++)
    //    ui.tableWidget->removeColumn(col);
    // ui.tableWidget->setRowCount(0);
    // ui.tableWidget->setColumnCount(0);

    int cols = IQre.cols;
    int rows = IQre.rows;
    int t_rowCount = ui.tableWidget->rowCount();
    int t_colCount = ui.tableWidget->columnCount();
    ui.tableWidget->setColumnCount(cols);
    ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int m = 0; m < rows; m++)
    {
        ui.tableWidget->insertRow(t_rowCount + m);
        for (int n = 0; n < cols; n++)
        {
            float metricVal = IQre.at<float>(m, n);
            ui.tableWidget->setItem(t_rowCount + m, t_colCount + n,
                                    new QTableWidgetItem(QString("%1").arg(metricVal, 0, 'f', 3)));
        }
    }
    // ui.tableWidget->verticalHeader()->setStyleSheet("QHeaderView::section{background:rgb(245,245,245);}");
    // ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;color: black;}");
}
void MTFmeasureWidget::on_mtfModeBtn_clicked()
{
    ui.viewer->SetImageModel(MTF);
    ui.chartStackedWidget->setCurrentIndex(0);
    /*  MtfContentPane->clicked();*/
    ui.widget_5->setEnabled(true);
    ui.widget_7->setEnabled(false);
    QFuture<void> future =
        QtConcurrent::run(ChromaticityCameraModel::instance(), &ChromaticityCameraModel::switchToMTFModel);
    // ChromaticityCameraModel::instance()->switchToMTFModel();
}
void MTFmeasureWidget::on_iqModeBtn_clicked()
{
    ui.viewer->SetImageModel(IQ1);
    ui.chartStackedWidget->setCurrentIndex(1);
    /* IQContentPane->clicked();*/
    ui.widget_5->setEnabled(false);
    ui.widget_7->setEnabled(true);
    QFuture<void> future =
        QtConcurrent::run(ChromaticityCameraModel::instance(), &ChromaticityCameraModel::switchToChromaticMode);
    // ChromaticityCameraModel::instance()->switchToChromaticMode();
}
void MTFmeasureWidget::on_mancollectimgsCkb_clicked(bool checked)
{
    ui.pushimgToParamsBtn->setEnabled(checked);
    ui.newImgsToParamsBtn->setEnabled(checked);
    ui.xxyimagetype->setEnabled(checked);
    ui.yimagetype->setEnabled(checked);
    IQTModel::instance()->setManualCollectImg(checked);
}
void MTFmeasureWidget::on_pushimgToParamsBtn_clicked()
{
    QtConcurrent::run([=] {
        emit enableWidget(ui.pushimgToParamsBtn, false);
        if (ui.yimagetype->isChecked())
        {
            IQTModel::instance()->pushImageToParams(METRICIMGTYPE::YIMAGE);
        }
        else if (ui.xyzimagetype->isChecked())
        {
            IQTModel::instance()->pushImageToParams(METRICIMGTYPE::XYZIMAGE);
        }
        else
        {
            IQTModel::instance()->pushImageToParams(METRICIMGTYPE::CHROMAIMAGE);
        }
        LoggingWrapper::instance()->info("push XYZ images done");
        emit enableWidget(ui.pushimgToParamsBtn, true);
    });
}
void MTFmeasureWidget::on_newImgsToParamsBtn_clicked()
{
    QtConcurrent::run([=] {
        emit enableWidget(ui.newImgsToParamsBtn, false);
        if (ui.yimagetype->isChecked())
        {
            IQTModel::instance()->clearImgParamsAndAdd(METRICIMGTYPE::YIMAGE);
        }
        else if (ui.xyzimagetype->isChecked())
        {
            IQTModel::instance()->clearImgParamsAndAdd(METRICIMGTYPE::XYZIMAGE);
        }
        else
        {
            IQTModel::instance()->clearImgParamsAndAdd(METRICIMGTYPE::CHROMAIMAGE);
        }
        LoggingWrapper::instance()->info("capture XYZ images done");
        emit enableWidget(ui.newImgsToParamsBtn, true);
    });
}
void MTFmeasureWidget::upDateTrisXYZImage(std::vector<cv::Mat> imgs)
{
    // ui.viewer->SetXYZImages(imgs.at(0), imgs.at(1), imgs.at(2));
    ui.viewer->SetXYZ(imgs.at(0), imgs.at(1), imgs.at(2));
    ui.viewer->SetYxy(imgs.at(3), imgs.at(4), imgs.at(5));
    ui.viewer->SelectImageShow(ImageType(1));
    ui.iq1Images->setCurrentIndex(1);
}
void MTFmeasureWidget::on_iq1Images_currentIndexChanged(int index)
{
    ui.viewer->SelectImageShow(ImageType(index));
}
void MTFmeasureWidget::setWidgetEnable(QWidget *wgt, bool enable)
{
    wgt->setEnabled(enable);
}
void MTFmeasureWidget::getAutoFocusDateFromCallback(MLStatus status, double progress, int *steparray, double *distance,
                                                    double *data, int len)
{
    if (steparray == nullptr || data == nullptr)
    {
        return;
    }
    m_AutoFocusStatus->setValue(progress * 100);

    if (status == MLStatus::ML_STATUS_FOCUS_ROUGH)
    {
        QList<QPointF> list = m_CoarseScan->points();

        if (list.size() >= len)
        {
            m_CoarseScan->clear();
            m_FineScan->clear();
        }

        double x = *(steparray + len - 1);
        double y = *(data + len - 1);
        if (len == 1)
        {
            m_AxisXmin = x - 1;
            m_AxisXmax = x + 1;
        }
        if (x < m_AxisXmin)
        {
            m_AxisXmin = x - 1;
        }
        if (x > m_AxisXmax)
        {
            m_AxisXmax = x + 1;
        }
        if (y < m_AxisYmin)
        {
            m_AxisYmin = y;
        }
        if (y > m_AxisYmax)
        {
            m_AxisYmax = y + y * 0.1;
        }
        m_CoarseScan->append(QPointF(x, y));
        m_AxisX->setRange(m_AxisXmin, m_AxisXmax);
        m_AxisY->setRange(m_AxisYmin, m_AxisYmax);
    }
    else if (status == MLStatus::ML_STATUS_FOCUS_FINE)
    {
        double x = *(steparray + len - 1);
        double y = *(data + len - 1);
        if (x < m_AxisXmin)
        {
            m_AxisXmin = x - 1;
        }
        if (x > m_AxisXmax)
        {
            m_AxisXmax = x + 1;
        }
        if (y < m_AxisYmin)
        {
            m_AxisYmin = y;
        }
        if (y > m_AxisYmax)
        {
            m_AxisYmax = y + y * 0.1;
        }
        m_FineScan->append(QPointF(x, y));
        m_AxisX->setRange(m_AxisXmin, m_AxisXmax);
        m_AxisY->setRange(m_AxisYmin, m_AxisYmax);
    }
    // m_ThroughFocusChart->update();
    if (m_ThroughFocusDock->isHidden())
    {
        m_ThroughFocusDock->show();
    }
}
void MTFmeasureWidget::on_showCross_stateChanged(int state)
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

void MTFmeasureWidget::on_saveimgbtn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(
        this, QStringLiteral("Save image:"), "d:", QStringLiteral("(*.png *.jpg *.xpm  *.tif);;All files (*.*)"));
    if (!fileName.isEmpty())
    {
        cv::Mat img = CameraModel::GetInstance().GetImage();
        if (!img.empty())
        {
            vector<int> compression_params;

            compression_params.push_back(IMWRITE_TIFF_COMPRESSION);
            compression_params.push_back(1);
            cv::imwrite(fileName.toStdString(), img, compression_params);
        }
    }
}
void MTFmeasureWidget::postInitalize()
{
    qRegisterMetaType<std::vector<cv::Mat>>("std::vector<cv::Mat>");
}
void MTFmeasureWidget::NotifyMotionStateChanged(MLMotionState old_state, MLMotionState new_state)
{
    if (old_state != MLMotionState::kMLStatusDisConnected && new_state == MLMotionState::kMLStatusDisConnected)
    {
        on_disconnectCamera_clicked();
    }
}

void MTFmeasureWidget::on_connectCamera_clicked()
{
    WaitWidget::instance()->startAnimation();
    isUseWaitWidget = true;
    connectCamera();
}

void MTFmeasureWidget::connectCamera()
{
    disconnect(ChromaticityCameraModel::instance(), SIGNAL(refreshTrisYImage(std::vector<cv::Mat>)), this,
               SLOT(upDateTrisXYZImage(std::vector<cv::Mat>)));
    connect(ChromaticityCameraModel::instance(), SIGNAL(refreshTrisYImage(std::vector<cv::Mat>)), this,
            SLOT(upDateTrisXYZImage(std::vector<cv::Mat>)));

    disconnect(IQTModel::instance(), SIGNAL(updateFOVPoints(std::vector<cv::Point>)), this,
               SLOT(disFOVPositionPts(std::vector<cv::Point>)));
    connect(IQTModel::instance(), SIGNAL(updateFOVPoints(std::vector<cv::Point>)), this,
            SLOT(disFOVPositionPts(std::vector<cv::Point>)));
    disconnect(ChromaticityCameraModel::instance(), SIGNAL(imageBinningChange(int)), this,
               SLOT(onBitDeepthChanged(int)));
    connect(ChromaticityCameraModel::instance(), SIGNAL(imageBinningChange(int)), this, SLOT(onBitDeepthChanged(int)));
    emit enableWidget(ui.connectCamera, false);
    QFuture<Result> future = QtConcurrent::run([=] {
        // CameraModel::GetInstance().connect(CameraConfig::GetInstance().GetCameraInfo().SN.c_str());
        QHash<QString, QVariant> props;
        props.insert("intrument", "Intrument_Controll");
        IIntrument *pFilter = ExtensionSystem::Internal::ServicesManger::getService<IIntrument>("IIntrument", props);
        ML::MLResult ok = pFilter->ConnectAll();
        if (ok.code == ML_STATUS_OK)
        {
            if (CameraModel::GetInstance().connect("0"))
            {
                CameraModel::GetInstance().SubscribeCameraEvent(MLCameraEvent::kFrameReceived);
                CameraModel::GetInstance().SubscribeCameraEvent(MLCameraEvent::kGrayLevel);
                LoggingWrapper::instance()->info("Instrument connection success.");
                emit enableWidget(ui.connectCamera, true);
                return Result();
            }
            else
            {
                emit enableWidget(ui.connectCamera, true);
                return Result(false, "Camera connection failed");
            }
        }
        else
        {
            LoggingWrapper::instance()->error(
                QString("Instrument connection failed, %1").arg(QString::fromStdString(ok.msg)));
            emit enableWidget(ui.connectCamera, true);
            return Result(false, "Instrument connection failed");
        }
    });

    watcher.setFuture(future);
}
void MTFmeasureWidget::on_disconnectCamera_clicked()
{
    QHash<QString, QVariant> props;
    props.insert("intrument", "Intrument_Controll");
    IIntrument *pFilter = ExtensionSystem::Internal::ServicesManger::getService<IIntrument>("IIntrument", props);
    pFilter->CloseAll();
    CameraModel::GetInstance().UnsubscribeCameraEvent(MLCameraEvent::kGrayLevel);
    CameraModel::GetInstance().UnsubscribeCameraEvent(MLCameraEvent::kFrameReceived);
    CameraModel::GetInstance().disConnect();
    LoggingWrapper::instance()->info("Instrument disconnect.");
    emit enableWidget(ui.connectCamera, true);
}
void MTFmeasureWidget::on_loadPictureButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, QStringLiteral("Load image:"), "d:", QStringLiteral("(*.png *.jpg *.xpm  *.tif);;All files (*.*)"));
    //if (disImage.load(fileName))
    {
        curImgPath = fileName;
        curNineSquareImg = cv::imread(curImgPath.toStdString().c_str(), cv::IMREAD_UNCHANGED);
        ui.viewer->Enqueue(curNineSquareImg);

        emit loadImageSignal(fileName);
        //TODO:test
        return;

        // ui.viewer->getPixmapItem()->setPixmap(QPixmap::fromImage(disImage));
        // scene->addPixmap(QPixmap::fromImage(disImage));
        if (ui.semiAutoRadioButton->isChecked())
        {
            semiAutoROI();
        }
        else if (ui.autoRadioButton->isChecked())
        {
            autoROI();
        }
        ui.widget_4->setProperty(CROSS_AREA_IMGE_PATH, fileName);
    }
}

void MTFmeasureWidget::saveCrossedAreaToJson(const QString &fileName, double deltax, double deltay)
{
    QJsonObject obj;
    for (size_t i = 1; i <= 9; i++)
    {
        QJsonArray array1;
        if (i % 3 != 0)
        {
            array1.insert(0, QJsonValue(deltax * ((i % 3) - 1)));
            array1.insert(1, QJsonValue(deltay * (i / 3)));
        }
        else
        {
            array1.insert(0, QJsonValue(deltax * 2));
            array1.insert(1, QJsonValue(deltay * ((i / 3) - 1)));
        }
        array1.insert(2, QJsonValue(deltax));
        array1.insert(3, QJsonValue(deltay));
        obj.insert("CrossArea" + QString::number(i), QJsonArray(array1));
        array1.removeAt(0);
        array1.removeAt(1);
        array1.removeAt(2);
        array1.removeAt(3);
    }

    QFileInfo FileInfo(fileName);
    if (FileInfo.isFile())
    {
        QFile::remove(fileName);
    }
    QFile file1(fileName);
    file1.open(QIODevice::ReadWrite);
    QJsonDocument jdoc(obj);
    file1.write(jdoc.toJson());
    file1.close();
    IQMetricConfig::instance()->load("config/IQMetricConfig.json");
    getCrossAreaInfo();
}

void MTFmeasureWidget::on_calMTFButton_clicked()
{
    if (ui.autoRadioButton->isChecked())
    {
        autoROI();
    }
    // QVector<QVector<Cropper*>*>::iterator iter;
    QVector<Cropper *>::iterator iter;
    int index = 0;
    ui.queryfreq->clear();
    ui.query_mtfval->clear();
    mtfChart->removeAllSeries();
    lsfChart->removeAllSeries();
    // cv::Mat tempMat = CameraModel::GetInstance().GetImage();
    QDate Date = QDate::currentDate();
    int binning = getBinning();
    for (iter = aoiCrossvector.begin(); iter != aoiCrossvector.end(); iter++, index++)
    {
        crossmtfModel = new CrossImgMTFModel(this);
        Cropper *crp = *iter;
        if (!curNineSquareImg.empty())
        {
            cv::Mat cvImg = curNineSquareImg(
                cv::Rect(crp->Xgen / binning, crp->Ygen / binning, crp->width / binning, crp->height / binning));
            // cv::imwrite("d:/" + std::to_string(index) + ".tif", cvImg);
            crossmtfModel->setImage(cvImg);
            crossmtfModel->setAOIId(iter - aoiCrossvector.begin());
            crossmtfModel->calculateMTF(false);
            if (crossmtfModel->m_mtfValsVec.isEmpty())
            {
                continue;
            }

            if (calvmode)
            {
                ROIMTFValues *valItem;
                valItem = crossmtfModel->m_mtfValsVec[2];
                updateChart(valItem->m_freqVals, valItem->m_mtfVals, valItem->binLen, mtfChart, QString::number(index));
                updateChart(valItem->m_lsfreqVals, valItem->m_lsfVals, valItem->binLen * 2, lsfChart,
                            QString::number(index));
            }
            else
            {
                ROIMTFValues *valItem;
                valItem = crossmtfModel->m_mtfValsVec[0];
                updateChart(valItem->m_freqVals, valItem->m_mtfVals, valItem->binLen, mtfChart, QString::number(index));
                updateChart(valItem->m_lsfreqVals, valItem->m_lsfVals, valItem->binLen * 2, lsfChart,
                            QString::number(index));
            }

            if (ui.checkBox->isChecked())
            {
                QString path = QString("./measureData/%1/cross%2").arg(Date.toString("yyyy-MM-dd")).arg(index);
                if (CreateDir(path))
                {
                    crossmtfModel->saveMTFData(path, "Hydrus", index);
                }
            }
        }
        if (crossmtfModel != NULL)
        {
            delete crossmtfModel;
            crossmtfModel = NULL;
        }
    }
}
void MTFmeasureWidget::on_exportCsvBtn_clicked()
{
    /*Save iq metrics data with csv*/
    QString iqpath = QString("./measureData/%1/iqmetrics").arg(QDate::currentDate().toString("yyyy-MM-dd"));
    if (CreateDir(iqpath))
    {
        QString iqFile = iqpath + QDir::separator() + "Hydrus" + "_Iq_Metrics" + ".csv";
        IQTModel::instance()->saveMetricsToCsv(iqFile);
    }
}
void MTFmeasureWidget::disFOVPositionPts(std::vector<cv::Point> pts)
{
    ui.viewer->clearFiducialPoints();
    for each (cv::Point2f pt in pts)
    {
        ui.viewer->addFiducialPoint(pt.x, pt.y);
    }
}
void MTFmeasureWidget::onBitDeepthChanged(int bit)
{
    if (bit == 16)
    {
        ui.iqModeBtn->setChecked(true);
        on_iqModeBtn_clicked();
    }
    else if (bit == 8)
    {
        ui.mtfModeBtn->setChecked(true);
        on_mtfModeBtn_clicked();
    }
}
void MTFmeasureWidget::on_okButton_clicked()
{
    roiConfig.centerOffset = ui.offsetROI->text().toInt();
    roiConfig.roiHeight = ui.heightROI->text().toInt();
    roiConfig.roiWidth = ui.widthROI->text().toInt();
    MTFModelConfig::instance()->setROIPosCfg(roiConfig);
}

// void MTFmeasureWidget::on_manualRadioButton_clicked(){
//   // delALLROIData();
//    ui.viewer->setContextMenuPolicy(Qt::CustomContextMenu);
//    menu = new QMenu(this);
//    addNewROIAction = new QAction("Add ROI", menu);
//    delAllAction = new QAction("Delete ROI", menu);
//    connect(ui.viewer, SIGNAL(customContextMenuRequested(const QPoint&)), this,
//    SLOT(graphViewcustomContextMenuRequested(const QPoint&))); connect(addNewROIAction, SIGNAL(triggered()), this,
//    SLOT(addNewROI())); connect(delAllAction, SIGNAL(triggered()), this, SLOT(delALLROIData()));
//    menu->addAction(addNewROIAction);
//    menu->addAction(delAllAction);
//}

void MTFmeasureWidget::autoROI()
{
    setEnabled(false);
    visionAlgorithm::GetInstance()->SourceImage = curNineSquareImg;
    visionAlgorithm::GetInstance()->Mode = MTF_WIDGET;
    QThread *thread = new QThread();
    visionAlgorithm::GetInstance()->moveToThread(thread);
    QObject::connect(thread, &QThread::started, visionAlgorithm::GetInstance(), &visionAlgorithm::Match);
    thread->start();
}

void MTFmeasureWidget::autoROIUpdate()
{
    resultAutoROI = visionAlgorithm::GetInstance()->ResultData;
    if (resultAutoROI.size() < 9)
    {
        setEnabled(true);
        return;
    }
    rewriteCrossAreaInfo();
    QVector<Cropper *>::iterator iter;
    int index = 0;
    int binning = getBinning();
    for (iter = aoiCrossvector.begin(); iter != aoiCrossvector.end(); iter++, index++)
    {
        Cropper *crp = *iter;
        cv::Rect rect(crp->Xgen / binning, crp->Ygen / binning, crp->width / binning, crp->height / binning);
        cv::Mat cvImg = curNineSquareImg(rect);
        CrossCenter crossCenter(cvImg, roiConfig.centerOffset);
        crossCenter.get_crossCenterMTF();
        std::vector<cv::Point> rois_center = crossCenter.get_newroi_center();

        cv::Point2d roiPt;
        std::vector<cv::Rect> autoRois;

        for (int i = 0; i < rois_center.size(); i++)
        {
            rois_center.at(i).x += crp->Xgen / binning;
            rois_center.at(i).y += crp->Ygen / binning;
        }

        drawAutoROIs(rois_center, index);
    }
    ui.viewer->Enqueue(visionAlgorithm::GetInstance()->SourceImage);
    ui.comboBox->setEnabled(true);
    setEnabled(true);
}

void MTFmeasureWidget::semiAutoROI()
{
    // delALLROIData();
    // initToShow();
    getCrossAreaInfo();
    QVector<Cropper *>::iterator iter;
    int index = 0;
    int binning = getBinning();
    for (iter = aoiCrossvector.begin(); iter != aoiCrossvector.end(); iter++, index++)
    {
        Cropper *crp = *iter;
        cv::Rect rect(crp->Xgen / binning, crp->Ygen / binning, crp->width / binning, crp->height / binning);
        cv::Mat cvImg = curNineSquareImg(rect);
        // algorithm::MLCrossCenter my_cross;
        // Test
        CrossCenter crossCenter(cvImg, roiConfig.centerOffset);
        crossCenter.get_crossCenterMTF();
        std::vector<cv::Point> rois_center = crossCenter.get_newroi_center();
        // std::vector<cv::Point2f> rois_center = my_cross.GetROICenter(roiConfig.centerOffset);
        // cv::Point2f relCenter = my_cross.GetCrossCenter(cvImg);
        /*TODO*/
        cv::Point2d roiPt;
        std::vector<cv::Rect> autoRois;
        // std::vector<cv::Point2f> rois_center = my_cross.GetROICenter(roiConfig.centerOffset);
        for (int i = 0; i < rois_center.size(); i++)
        {
            rois_center.at(i).x += crp->Xgen / binning;
            rois_center.at(i).y += crp->Ygen / binning;
        }
        // drawAutoROIs(rois_center, this->autoroiOpts.offset, disImage.width(), disImage.height(),i);
        drawAutoROIs(rois_center, index);
    }
}

void MTFmeasureWidget::drawAutoROIs(std::vector<cv::Point> pt, int aoiId)
{

    roivector.clear();
    for (int j = 0; j < pt.size(); j++)
    {
        cv::Point2f roiCenter = pt.at(j);
        int width = roiConfig.roiWidth;
        int height = roiConfig.roiHeight;
        ui.viewer->AddROI(aoiId * 4 + j, QPoint(roiCenter.x, roiCenter.y), QPoint(width, height));
    }
}
void MTFmeasureWidget::aoiDatasChange(cv::Mat &crossImg, int aoiIdx)
{
    mtfChart->legend()->setAlignment(Qt::AlignTop);
    QVector<ROIInfo>::iterator vecit;
    QVector<ROIInfo> roiInfsVec;
    std::vector<cv::Rect> autoRois;
    algorithm::MLCrossCenter my_cross;
    Cropper *crp = aoiCrossvector.at(aoiIdx);
    CrossCenter crossCenter(crossImg, roiConfig.centerOffset);
    // crossCenter.get_crossCenter(2);
    crossCenter.get_crossCenterMTF();
    vector<Point> roi_centers = crossCenter.get_newroi_center(); // top, bottom, left, right
    cv::Rect fullRect(0, 0, crossImg.cols, crossImg.rows);
    int binning = getBinning();
    for (int i = 0; i < roi_centers.size(); i++)
    {
        cv::Point2f roiCenter = roi_centers.at(i);
        cv::Rect roi(roiCenter.x - roiConfig.roiWidth / 2, roiCenter.y - roiConfig.roiHeight / 2, roiConfig.roiWidth,
                     roiConfig.roiHeight);
        roi = roi & fullRect;
        cv::Mat roiimg = crossImg(roi);
        if (roiimg.data)
        {
            cal_mtf(roiimg, QString::number(i));
        }
        roi_centers.at(i).x += crp->Xgen / binning;
        roi_centers.at(i).y += crp->Ygen / binning;
        QPoint size(roiConfig.roiWidth, roiConfig.roiHeight);
        ui.viewer->AddROI(aoiIdx * 4 + i, QPoint(roi_centers.at(i).x, roi_centers.at(i).y), size);
    }
}
void MTFmeasureWidget::showManualROIsByAoiId(int aoiid)
{
    ui.viewer->ClearALLROI();
    QList<QGraphicsItem *> allitems = scene->items();
    for (int i = 0; i < allitems.size(); i++)
    {
        QGraphicsItem *item = allitems.at(i);
        Cropper *manuROI = dynamic_cast<Cropper *>(item);
        if (manuROI != NULL)
        {
            item->setVisible(false);
        }
    }
    QVector<Cropper *> manuRois = ROIManager::getInstance()->getAllROIsById(curSelectIndex);
    for each (Cropper *var in manuRois)
    {
        var->setVisible(true);
    }
}
void MTFmeasureWidget::clearChart()
{
    mtfChart->removeAllSeries();
    lsfChart->removeAllSeries();
    QVector<PipeLine *>::iterator iter0;
    for (iter0 = pipelinevec.begin(); iter0 != pipelinevec.end(); iter0++)
    {
        PipeLine *item = *iter0;
        delete item;
    }
    pipelinevec.clear();
    ui.query_mtfval->clear();
}
void MTFmeasureWidget::selectCross(int index)
{
    clearChart();
    ui.viewer->ClearALLROI();
    curImgROIIdx = 0;
    curSelectIndex = index - 1;
    singleCrossM = true;

    getCrossAreaInfo();
    int binnig = getBinning();
    cv::Mat tempMat = CameraModel::GetInstance().GetImage();
    if (CameraModel::GetInstance().isConnect() && !tempMat.empty())
    {
        curNineSquareImg = tempMat;
    }
    else
    {
        tempMat = curNineSquareImg.clone();
    }
    if (tempMat.empty())
    {
        return;
    }
    if (ui.manualRadioButton->isChecked())
    {
        showManualROIsByAoiId(curSelectIndex);
        // return;
    }
    QVector<Cropper *>::iterator iter;
    QGraphicsPixmapItem *m_PixmapItem = ui.viewer->getPixmapItem();
    QPixmap pixMap = m_PixmapItem->pixmap();
    for (iter = aoiCrossvector.begin(); iter != aoiCrossvector.end(); iter++)
    {
        Cropper *crp = *iter;
        if ((index - 1) == crp->AreaIndex)
        {
            ui.roifromx->setText(QString::number(crp->Xgen / binnig));
            ui.roifromy->setText(QString::number(crp->Ygen / binnig));
            ui.roiwidth->setText(QString::number(crp->width / binnig));
            ui.roiheight->setText(QString::number(crp->height / binnig));
            // QImage cImage = (disImage).copy(crp->Xgen, crp->Ygen, crp->width, crp->height);
            // QImage cImage = (qImg).copy(crp->Xgen, crp->Ygen, crp->width, crp->height);
            // QPixmap AuxPxmColor = QPixmap::fromImage(cImage).scaled(SampleWidth,SampleHeight, Qt::IgnoreAspectRatio,
            // Qt::SmoothTransformation);
            QPixmap AuxPxmColor =
                pixMap.copy(crp->Xgen / binnig, crp->Ygen / binnig, crp->width / binnig, crp->height / binnig)
                    .scaled(SampleWidth, SampleHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            ui.roimagelable->setPixmap(AuxPxmColor.scaled(ui.roimagelable->size()));
            if (!ui.manualRadioButton->isChecked())
            {
                cv::Rect rect(crp->Xgen / binnig, crp->Ygen / binnig, crp->width / binnig, crp->height / binnig);
                cv::Mat crossItemImg = tempMat(rect);
                aoiDatasChange(crossItemImg, index - 1);
            }
            break;
        }
        else if (index == 0)
        {
            ui.roifromx->clear();
            ui.roifromy->clear();
            ui.roiwidth->clear();
            ui.roiheight->clear();
            ui.roimagelable->clear();
            mtfChart->removeAllSeries();
            lsfChart->removeAllSeries();
        }
    }
}
void MTFmeasureWidget::NotifyCameraFrameReceived(QList<QImage> imageList)
{
    // ui.viewer->showImage(CameraModel::GetInstance().GetQImage());

    ui.viewer->SetImageList(imageList);
}
void MTFmeasureWidget::on_calIqtMetricBtn_clicked()
{
    ui.tableWidget->setRowCount(0);
    ui.tableWidget->setColumnCount(0);
    QString metrName = ui.metricscombox->currentText();
    QString val = IQTModel::instance(this)->calculateMetric(metrName);
    QWidget *lyt = ui.widget_7;
    IQMetricWidget *resultWidget = lyt->findChild<IQMetricWidget *>(metrName);
    QString suf = "";
    if (resultWidget != nullptr)
    {
        // resultWidget->setMetricValue(val);
        suf = resultWidget->getSuffix();
    }
    else
    {
        LoggingWrapper::instance()->error("can not find the metricwidget:" + metrName);
        return;
    }
    QString strColor = MetricsData::instance()->getCurDpaColorStr();
    QString csvHeader = IQTModel::instance()->generateCsvHeader(metrName, suf, strColor);
    QString res = IQTModel::instance()->getCSVHeaderValue(csvHeader);

    IC(metrName.toStdString(), strColor.toStdString(), csvHeader.toStdString());
    resultWidget->setMetricValue(res);
}
void MTFmeasureWidget::on_queryfreq_editingFinished()
{
    double mtfFreq = ui.queryfreq->text().toDouble();
    QString mtfValstr;
    double mtfVal;
    if (singleCrossM && (!pipelinevec.isEmpty()))
    {
        QVector<PipeLine *>::iterator iter;
        for (iter = pipelinevec.begin(); iter != pipelinevec.end(); iter++)
        {
            PipeLine *item = *iter;
            mtfVal = item->getMtfByFreq(mtfFreq);
            if (iter == pipelinevec.begin())
                mtfValstr = QString::number(mtfVal);
            else
            {
                mtfValstr = mtfValstr + "\n" + QString::number(mtfVal);
            }
        }
        ui.query_mtfval->clear();
        ui.query_mtfval->setText(mtfValstr);
    }
    else
        ui.query_mtfval->setText("null");
}

void MTFmeasureWidget::initToShow()
{
    // first cross info
    // Cropper* crp = *(aoiCrossvector.begin());
    // ui.roifromx->setText(QString::number(crp->Xgen));
    // ui.roifromy->setText(QString::number(crp->Ygen));
    // ui.roiwidth->setText(QString::number(crp->width));
    // ui.roiheight->setText(QString::number(crp->height));
    // QPixmap AuxPxmColor = QPixmap::fromImage(disImage).copy(crp->Xgen, crp->Ygen, crp->width,
    // crp->height).scaled(SampleWidth,
    //    SampleHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    // ui.roimagelable->setPixmap(AuxPxmColor.scaled(ui.roimagelable->size()));
    // comboBox item
    ui.comboBox->clear();
    ui.comboBox->addItem("None");
    for (size_t i = 0; i < areaNum; i++)
        ui.comboBox->addItem("AOI" + QString::number(i));
    // roi info
    ROIPosConfig cfg = MTFModelConfig::instance()->getROIPosCfg();
    roiConfig.centerOffset = cfg.centerOffset;
    roiConfig.roiHeight = cfg.roiHeight;
    roiConfig.roiWidth = cfg.roiWidth;
    ui.offsetROI->setText(QString::number(roiConfig.centerOffset));
    ui.widthROI->setText(QString::number(roiConfig.roiWidth));
    ui.heightROI->setText(QString::number(roiConfig.roiHeight));

    ui.chartStackedWidget->hide();
    ui.widget_5->hide();
}

void MTFmeasureWidget::getCrossAreaInfo()
{
    aoiCrossvector.clear();
    for each (CrossAreaConfig *var in aoiCrosses)
    {
        delete var;
    }
    aoiCrosses.clear();
    areaNum = 0;
    aoiCrosses = MTFModelConfig::instance()->getCrossAreaConfig();
    aoiCrossvector.clear();
    if (!aoiCrosses.empty())
    {
        std::vector<CrossAreaConfig *>::iterator iter;
        for (iter = aoiCrosses.begin(); iter != aoiCrosses.end(); iter++)
        {
            CrossAreaConfig *crossConfig = *iter;
            int x = crossConfig->FromX;
            int y = crossConfig->FromY;
            int width = crossConfig->aoiWidth;
            int height = crossConfig->aoiHeight;

            Cropper *crp = new Cropper(this, curImgPath, x, y, width, height, areaNum);
            aoiCrossvector.append(crp);
            areaNum++;
        }
    }
}

void MTFmeasureWidget::rewriteCrossAreaInfo()
{
    areaNum = 0;
    aoiCrosses = MTFModelConfig::instance()->getCrossAreaConfig();
    aoiCrossvector.clear();
    if (!aoiCrosses.empty())
    {
        std::vector<CrossAreaConfig *>::iterator iter;
        int index = 0;
        for (iter = aoiCrosses.begin(); iter != aoiCrosses.end(); iter++, index++)
        {
            CrossAreaConfig *crossConfig = *iter;
            int width = crossConfig->aoiWidth;
            int height = crossConfig->aoiHeight;
            int x = resultAutoROI[index].sourceImageCenterPoint.x - int(width / 2);
            int y = resultAutoROI[index].sourceImageCenterPoint.y - int(height / 2);

            Cropper *crp = new Cropper(this, curImgPath, x, y, width, height, areaNum);
            aoiCrossvector.append(crp);
            areaNum++;
        }
    }
}

void MTFmeasureWidget::on_WPC_clicked()
{
    if (ui.iqModeBtn->isChecked())
    {
        if (m_CalibrationDock->isHidden())
        {
            m_CalibrationDock->show();
        }
    }
    else
    {
        QMessageBox::warning(this, "WPC Warning", tr("Please use the WPC feature in IQ mode!"));
    }
}

void MTFmeasureWidget::handleFinished()
{
    QFuture<Result> future = watcher.future();
    Result ret = future.result();
    if (ret.success)
    {
        MetricsData::instance()->addConnectedDevicesNum();
    }
    if (isUseWaitWidget)
    {
        WaitWidget::instance()->stopAnimation();
        isUseWaitWidget = false;
    }
    
}

void MTFmeasureWidget::initPI()
{
    QHash<QString, QVariant> props;
    props.insert("PI", "PI1D");
    MLMotion *motion = ExtensionSystem::Internal::ServicesManger::getService<MLMotion>("MLMotion", props);
    motion->Subscribe(MLMotionEvent::kStateChanged, this);
}

void MTFmeasureWidget::delOneROI()
{
    QGraphicsItem *item = ui.viewer->itemAt(curpost);
    Cropper *cropper = dynamic_cast<Cropper *>(item);
    if (cropper != NULL)
    {
        ROIManager::getInstance()->delROICropper(curSelectIndex, cropper);
        delete cropper;
    }
}

void MTFmeasureWidget::on_manualMtf_clicked()
{
    clearChart();
    if (ui.autoRadioButton->isChecked())
    {
        curNineSquareImg = CameraModel::GetInstance().GetImage();
        autoROI();
    }
    else
    {
        QVector<Cropper *> manuRois = ROIManager::getInstance()->getAllROIsById(curSelectIndex);
        for (int i = 0; i < manuRois.size(); i++)
        {
            if (!curNineSquareImg.empty())
            {
                Cropper *crp = manuRois.at(i);
                cv::Rect roi(crp->Xgen, crp->Ygen, crp->width, crp->height);
                cv::Mat roiimg = curNineSquareImg(roi);
                if (roiimg.data)
                {
                    cal_mtf(roiimg, QString::number(i));
                }
            }
        }
        ui.comboBox->setEnabled(true);
    }
}

void MTFmeasureWidget::addNewROI(bool updatescene)
{
    if (!ui.manualRadioButton->isChecked() || curSelectIndex < 0)
        return;
    CrossAreaConfig *info = aoiCrosses.at(curSelectIndex);
    QPointF scenePos = ui.viewer->mapToScene(curpost);

    if (scenePos.x() < info->FromX || scenePos.y() < info->FromY || scenePos.x() > (info->FromX + info->aoiWidth) ||
        scenePos.y() > (info->FromY + info->aoiHeight))
    {
        return;
    }
    curImgROIIdx++;
    int binning = getBinning();
    Cropper *crp = new Cropper(this, curImgPath, scenePos.x(), scenePos.y());
    crp->setProperty("roimode", "manual");
    connect(crp, SIGNAL(ROIRegionChange(int, int, int, int)), this, SLOT(loadROIImg(int, int, int, int)));
    crp->setZValue(1);
    crp->Index = curImgROIIdx;
    scene->addItem(crp);
    loadROIImg(crp->Xgen / binning, crp->Ygen / binning, crp->width / binning, crp->height / binning);
    ROIManager::getInstance()->addROICropper(curSelectIndex, crp);
    roivector.append(crp);

    if (updatescene)
        scene->update();
}
void MTFmeasureWidget::clearROI()
{
    if (!roivector.isEmpty())
    {
        roivector.clear();
    }
    QList<QGraphicsItem *> allitems = scene->items();
    for (int i = 0; i < allitems.size(); i++)
    {
        QGraphicsItem *item = allitems.at(i);
        Cropper *crp = dynamic_cast<Cropper *>(item);
        if (crp != NULL)
        {
            scene->removeItem(item);
            ROIManager::getInstance()->delROICropper(curSelectIndex, crp);
            delete crp;
        }
    }
}

MTFmeasureWidget::~MTFmeasureWidget()
{
    QMap<QString, QVector<ROIInfo> *>::iterator it = roisinfoMap->begin();
    while (it != roisinfoMap->end())
    {
        delete *it;
        it++;
    }
    if (m_tooltip != 0)
    {
        delete m_tooltip;
    }
    QVector<PipeLine *>::iterator iter0;
    for (iter0 = pipelinevec.begin(); iter0 != pipelinevec.end(); iter0++)
    {
        PipeLine *item = *iter0;
        delete item;
    }
    pipelinevec.clear();
    /*if (mtfPipeline != NULL)
    {
        delete mtfPipeline;
        mtfPipeline = NULL;
    }*/
    if (crossmtfModel != NULL)
    {
        delete crossmtfModel;
        crossmtfModel = NULL;
    }
}
void MTFmeasureWidget::graphViewcustomContextMenuRequested(const QPoint &pos)
{
    QPoint clickPos = this->cursor().pos();
    curpost.setX(pos.x());
    curpost.setY(pos.y());
    menu->exec(clickPos);
}
void MTFmeasureWidget::loadROIImg(int x, int y, int width, int height)
{
    // Cropper* cropper = (Cropper*)QObject::sender();
    QPixmap AuxPxmColor = QPixmap::fromImage(disImage)
                              .copy(x, y, width, height)
                              .scaled(SampleWidth, SampleHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui.roimagelable->setPixmap(AuxPxmColor.scaled(ui.roimagelable->size()));
    ui.roistartxtext->setText(QString::number(x));
    ui.roistartytxt->setText(QString::number(y));
    ui.roiwidthtxt->setText(QString::number(width));
    ui.roisheighttxt->setText(QString::number(height));
}
void MTFmeasureWidget::initChart()
{
    ui.chartStackedWidget->setCurrentIndex(0);
    // QGridLayout* layout = new QGridLayout(ui.chartWidget);
    QGridLayout *layout = new QGridLayout(ui.widget_11);
    layout->setSpacing(0);
    layout->setMargin(0);
    mtfChart = new QChart();
    mtfChart->setTitle("mtf");
    xAxis_mtf = new QValueAxis;
    xAxis_mtf->setRange(0, 600);
    xAxis_mtf->setTitleText("lp/degree");
    yAxis_mtf = new QValueAxis;
    yAxis_mtf->setRange(0, 100);
    mtfChart->addAxis(xAxis_mtf, Qt::AlignBottom);
    mtfChart->addAxis(yAxis_mtf, Qt::AlignLeft);
    QLineSeries *mtfseries = new QLineSeries();
    mtfChart->addSeries(mtfseries);
    // mtfChart->setAxisX(xAxis_mtf, mtfseries);
    // mtfChart->setAxisY(yAxis_mtf, mtfseries);
    QChartView *mtfChartView = new QChartView(mtfChart);
    mtfChartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(mtfChartView, 0, 0);

    lsfChart = new QChart();
    lsfChart->setTitle("lsf");
    xAxis_lsf = new QValueAxis;
    xAxis_lsf->setRange(0, 100);
    xAxis_lsf->setTitleText(" ");
    yAxis_lsf = new QValueAxis;
    yAxis_lsf->setRange(0, 100);
    lsfChart->addAxis(xAxis_lsf, Qt::AlignBottom);
    lsfChart->addAxis(yAxis_lsf, Qt::AlignLeft);
    QLineSeries *lsfseries = new QLineSeries();
    lsfChart->addSeries(lsfseries);
    QChartView *lsfChartView = new QChartView(lsfChart, this);
    layout->addWidget(lsfChartView, 0, 1);

    m_ThroughFocusDock = new QDockWidget(tr("Auto Focus"), this);
    // QVBoxLayout *focusLayout = new QVBoxLayout();
    QWidget *container = new QWidget(m_ThroughFocusDock);
    QGridLayout *focusLayout = new QGridLayout(m_ThroughFocusDock);
    focusLayout->setContentsMargins(0, 0, 0, 0);
    container->setLayout(focusLayout);
    m_AutoFocusStatus = new QProgressBar(m_ThroughFocusDock);
    m_AutoFocusStatus->show();
    m_AutoFocusStatus->setValue(0);
    m_AutoFocusStatus->setMaximum(100);
    focusLayout->addWidget(m_AutoFocusStatus, 0, 0);
    m_ThroughFocusDock->setMinimumWidth(600);
    m_ThroughFocusDock->setMinimumHeight(500);
    m_ThroughFocusChart = new QChart();
    m_AxisX = new QValueAxis;
    m_AxisX->setRange(0, 100);
    m_AxisY = new QValueAxis;
    m_AxisY->setRange(0, 100);
    m_ThroughFocusChart->addAxis(m_AxisX, Qt::AlignBottom);
    m_ThroughFocusChart->addAxis(m_AxisY, Qt::AlignLeft);
    m_CoarseScan = new QLineSeries();
    m_FineScan = new QLineSeries();
    m_ThroughFocusChart->addSeries(m_CoarseScan);
    m_ThroughFocusChart->addSeries(m_FineScan);
    m_ThroughFocusChart->setAxisX(m_AxisX, m_CoarseScan);
    m_ThroughFocusChart->setAxisY(m_AxisY, m_CoarseScan);
    m_ThroughFocusChart->setAxisX(m_AxisX, m_FineScan);
    m_ThroughFocusChart->setAxisY(m_AxisY, m_FineScan);
    QChartView *chartView = new QChartView(m_ThroughFocusChart, m_ThroughFocusDock);
    chartView->setRenderHint(QPainter::Antialiasing);
    focusLayout->addWidget(chartView, 1, 0);
    m_ThroughFocusDock->hide();
    // m_ThroughFocusDock->setWidget(chartView);
    m_ThroughFocusDock->setWidget(container);
    m_ThroughFocusDock->setFloating(true);
    connect(m_CoarseScan, &QLineSeries::hovered, this, &MTFmeasureWidget::tooltip);
    connect(m_FineScan, &QLineSeries::hovered, this, &MTFmeasureWidget::tooltip);

    m_CalibrationDock = new QDockWidget(tr("Calibration"), this);
    // m_CalibrationWidget = new WPCCalibration(m_CalibrationDock);
    m_CalibrationDock->hide();
    // m_CalibrationDock->setWidget(m_CalibrationWidget);
    m_CalibrationDock->setFloating(true);
}

void MTFmeasureWidget::cal_mtf(Mat &img, QString seriesName)
{
    // checkSettingChange();
    // pipelinewidget->culc_mtf(img);
    PipeLine *mtfPipeline = new PipeLine();
    mtfPipeline->set_freq_unit(ML::FREQ_UNIT::lp_deg, MTFModelConfig::instance()->getFocusLength());
    mtfPipeline->culc_mtf(img);
    int size = mtfPipeline->get_bin_len();
    double *freq = mtfPipeline->get_freq();
    double *mtf = mtfPipeline->get_mtf();
    double *lsf = mtfPipeline->get_lsf();
    double *lsf_esf_x = mtfPipeline->get_esf_lsf_x();

    pipelinevec.append(mtfPipeline);

    updateChart(freq, mtf, size, mtfChart, seriesName);
    updateChart(lsf_esf_x, lsf, size * 2, lsfChart, seriesName);
}
void MTFmeasureWidget::on_maxfreq_textChanged()
{
    maxMTFFreq = ui.maxfreq->text().toDouble();
}
void MTFmeasureWidget::updateChart(double *x, double *y, int size, QChart *chart, QString seriesName, bool delAllSeries)
{
    QList<QAbstractSeries *> series = chart->series();
    if (delAllSeries)
    {
        chart->removeAllSeries();
    }
    QLineSeries *lineSeries = new QLineSeries;

    if (size <= 30)
        lineSeries->setPointsVisible(true);
    lineSeries->setName(seriesName);
    double max = 0;
    for (int i = 0; i < size; i++)
    {
        if (chart->title() == "mtf" && maxMTFFreq > 0)
        {
            if (x != NULL && x[i] > maxMTFFreq)
            {
                break;
            }
        }
        if (y != NULL && y[i] <= 100)
        {
            lineSeries->append(x[i], y[i]);
            if (x[i] > max)
                max = x[i];
        }
    }
    chart->axisX()->setRange(0, max);
    chart->addSeries(lineSeries);
    if (chart->title() == "mtf")
    {
        chart->setAxisX(chart->axisX(), lineSeries);
        chart->setAxisY(chart->axisY(), lineSeries);
    }
    connect(lineSeries, &QLineSeries::hovered, this, &MTFmeasureWidget::tooltip);
}
void MTFmeasureWidget::tooltip(QPointF point, bool state)
{
    QLineSeries *lineSeries = (QLineSeries *)sender();
    if (m_tooltip == 0)
        m_tooltip = new Callout(lineSeries->chart());
    if (m_tooltip->getChart() != lineSeries->chart())
    {
        delete m_tooltip;
        m_tooltip = new Callout(lineSeries->chart());
    }
    if (state)
    {
        m_tooltip->setText(QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y()));

        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    }
    else
    {
        m_tooltip->hide();
    }
}
void MTFmeasureWidget::ReadFrame()
{
    capture >> mFrame;
    QImage disImage = QImage((const uchar *)mFrame.data, mFrame.cols, mFrame.rows, QImage::Format_RGB888).rgbSwapped();
}
void MTFmeasureWidget::roiDatasChange(QVector<Cropper *> roivector)
{

    roisinfoMap->clear();
    int binning = getBinning();
    QVector<Cropper *>::iterator iter;
    for (iter = roivector.begin(); iter != roivector.end(); iter++)
    {
        Cropper *crp = *iter;
        QString imgname = crp->imgpath;
        if (roisinfoMap->contains(imgname))
        {
            QVector<ROIInfo> *roiInfsVec = roisinfoMap->value(imgname);
            ROIInfo rect(crp->Xgen / binning, crp->Ygen / binning, crp->width / binning, crp->height / binning,
                         crp->Index);
            roiInfsVec->append(rect);
        }
        else
        {
            QVector<ROIInfo> *roiInfsVec = new QVector<ROIInfo>();
            // QRect rect(crp->Xgen, crp->Ygen, crp->width, crp->height);
            ROIInfo rect(crp->Xgen / binning, crp->Ygen / binning, crp->width / binning, crp->height / binning,
                         crp->Index);
            roiInfsVec->append(rect);
            roisinfoMap->insert(imgname, roiInfsVec);
        }
    }
    cal_rawimg_mtf(roisinfoMap);
}

void MTFmeasureWidget::cal_rawimg_mtf(QMap<QString, QVector<ROIInfo> *> *roisinfoMap)
{
    clearChart();
    mtfChart->legend()->setAlignment(Qt::AlignTop);
    QVector<ROIInfo>::iterator vecit;
    QVector<ROIInfo> *roiInfsVec;

    QMap<QString, QVector<ROIInfo> *>::iterator mapiter = roisinfoMap->begin();
    int idx = 1;
    while (mapiter != roisinfoMap->end())
    {
        roiInfsVec = mapiter.value();
        QString roisrc = mapiter.key();
        // qDebug() << "image path:" << roisrc << endl;
        cv::Mat img = imread(roisrc.toStdString(), cv::IMREAD_UNCHANGED);
        for (vecit = roiInfsVec->begin(); vecit != roiInfsVec->end(); vecit++)
        {
            ROIInfo qrect = *vecit;
            cv::Rect rect(qrect.x, qrect.y, qrect.width, qrect.height);
            cv::Mat roiimg = img(rect);
            if (roiimg.data)
            {
                cal_mtf(roiimg, QString::number(qrect.index));
            }
            idx++;
        }
        mapiter++;
    }
}
bool MTFmeasureWidget::CreateDir(const QString &path)
{
    QDir dir(path);
    if (dir.exists())
    {
        qDebug() << dir.path();
        return true;
    }
    else
    {
        dir.setPath(".");
        bool ok = dir.mkpath(path);
        return ok;
    }
}

void MTFmeasureWidget::initIQWidget()
{
    ui.exportCsvBtn->setStyleSheet("QToolButton{background-color:transparent;}");
    QGridLayout *lyt = ui.metricsGLyt;
    std::vector<MetricDescription *> *metricVec = IQMetricConfig::instance()->getMetricDescripts();
    for (int i = 0; i < metricVec->size(); i++)
    {
        const MetricDescription *var = metricVec->at(i);
        if (var->dispaly)
        {
            IQMetricWidget *metricWgt = new IQMetricWidget(this);
            QString name = QString::fromStdString(var->name);
            metricWgt->setMetricName(name + ":");
            metricWgt->setMetricUnit(QString::fromStdString(var->unit));
            metricWgt->setObjectName(name);
            lyt->addWidget(metricWgt, i / 2, i % 2);
            ui.metricscombox->addItem(name);
            for each (std::string suf in var->suffix)
            {
                metricWgt->addSuffixItem(QString::fromStdString(suf));
            }
            metricWgt->show();
            connect(metricWgt, &IQMetricWidget::suffixTxtChanged, [=](QString text) {
                QObject *sende = sender();
                QString metrName = ui.metricscombox->currentText();
                QString strColor = MetricsData::instance()->getCurDpaColorStr();
                QString csvHeader = IQTModel::instance()->generateCsvHeader(metrName, text, strColor);
                QString res = IQTModel::instance()->getCSVHeaderValue(csvHeader);
                IC(strColor.toStdString(), csvHeader.toStdString());
                metricWgt->setMetricValue(res);
            });
        }
    }
}
int MTFmeasureWidget::getBinning()
{
    //TODO: test 
    return 1;
    return CameraModel::GetInstance().GetBinning();
}
