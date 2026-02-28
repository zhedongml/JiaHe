#include "LuminanceWidget.h"
#include "McBinocularMode.h"
#include "metricsdatacache.h"
#include <QMessageBox>

LuminanceWidget::LuminanceWidget(CameraViewer* viewer, QMenu* menu, QWidget* parent)
    : MetricsWidgetBase(viewer, menu, parent)
{
    ui.setupUi(this);

    initUI();
    initMenu(menu);
    initAccordion((QAccordion*)parent);

    connect(&m_watcher, SIGNAL(finished()), this, SLOT(threadFinished()));
}

LuminanceWidget::~LuminanceWidget()
{
}

void LuminanceWidget::on_btn_takeImg_clicked()
{
    ui.btn_takeImg->setEnabled(false);
    Result ret = takeImages();
    ui.btn_takeImg->setEnabled(true);

    if (!ret.success)
    {
        QMessageBox::critical(this, "Luminance calculate error", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
            QMessageBox::NoButton);
        return;
    }
}

void LuminanceWidget::on_btn_calculate_clicked()
{
    WaitWidget::instance()->startAnimation();

    QFuture future = QtConcurrent::run(this, &LuminanceWidget::calculateMetrics);
    m_watcher.setFuture(future);
}

void LuminanceWidget::threadFinished()
{
    WaitWidget::instance()->stopAnimation();

    Result ret = m_watcher.future().result();
    if (!ret.success)
    {
        QMessageBox::critical(this, "Luminance calculate error", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
            QMessageBox::NoButton);
        return;
    }

    LuminanceDialog* dialog = new LuminanceDialog();
    dialog->setResult(m_drawImage, m_luminance);
    dialog->exec();

    delete dialog;
    dialog = nullptr;
}

void LuminanceWidget::initUI()
{
    CROPPER_INDEX_MAX = 9;
    MENU_NAME = "menuLuminance";

    ui.comboBox_roiNumber->addItem("10*10");
    ui.comboBox_roiNumber->setItemData(0, 10);
    ui.comboBox_roiNumber->addItem("8*8");
    ui.comboBox_roiNumber->setItemData(1, 8);
    ui.comboBox_roiNumber->addItem("4*4");
    ui.comboBox_roiNumber->setItemData(2, 4);
    ui.comboBox_roiNumber->setCurrentIndex(0);

    ui.comboBox_ratio->addItem("1:1");
    ui.comboBox_ratio->addItem("4:3");
    ui.comboBox_ratio->setCurrentIndex(0);

    ui.comboBox_degree->addItem("30");
    ui.comboBox_degree->addItem("40");
    ui.comboBox_degree->addItem("50");
    ui.comboBox_degree->setCurrentIndex(0);

    ui.btn_takeImg->hide();
    ui.widget->hide();
}

void LuminanceWidget::initAccordion(QAccordion* accordion)
{
    initAccordionBase(accordion, "Luminance Calculate", 500);
}

void LuminanceWidget::initMenu(QMenu* menu)
{
    //QAction *addCrossAreaAction = new QAction("Add ROI", menu);
    //QAction *delCrossAreaAction = new QAction("Delete ROI", menu);
    //QAction *deleteAllCrossAreaAction = new QAction("Delete All ROI", menu);

    //addCrossAreaAction->setData(MENU_NAME);
    //delCrossAreaAction->setData(MENU_NAME);
    //deleteAllCrossAreaAction->setData(MENU_NAME);

    //menu->addAction(addCrossAreaAction);
    //menu->addAction(delCrossAreaAction);
    //menu->addAction(deleteAllCrossAreaAction);

    //connect(addCrossAreaAction, SIGNAL(triggered()), this, SLOT(addAction()));
    //connect(delCrossAreaAction, SIGNAL(triggered()), this, SLOT(deleteAction()));
    //connect(deleteAllCrossAreaAction, SIGNAL(triggered()), this, SLOT(deleteAllAction()));

    menuUpdate(menu, false);
}

Result LuminanceWidget::calculateMetrics()
{
    try {
        cv::Mat rawImage;
        if (isOffline()) {
            rawImage = getCurrentImage(false);
            if (rawImage.empty())
            {
                return Result(false, "Please load the image first.");
            }
        }
        else {
            Result ret = takeImages();
            if (!ret.success)
            {
                return ret;
            }

            QMap<QString, cv::Mat> imageXYZ;
            ret = McBinocularMode::instance()->calculateXYZImage(imageXYZ, m_onlineRawMap);
            if (!ret.success) {
                return ret;
            }
            rawImage = imageXYZ["Y"];
        }

        Result result = judgeImages(rawImage);
        if (!result.success)
        {
            return result;
        }

        int roiNum = ui.comboBox_roiNumber->itemData(ui.comboBox_roiNumber->currentIndex()).toInt();
        int ratio = ui.comboBox_ratio->currentIndex() + 1;
        //double degree = ui.comboBox_degree->currentText().toDouble();


        //TODO: color
        //FOVCenter inform = MetricsDataCache::getInstance()->getFOVCenter("w");
        //chroma.SetFovInform(inform.centerX, inform.centerY, inform.rotateH, inform.rotateV);

        // TODO: color
        //float degree = MetricsDataCache::getInstance()->getRotationDeg("w");
        //chroma.SetRotationDeg(degree);

        m_luminance = lumi.getLuminance(rawImage);
        m_drawImage = m_luminance.imgdraw;
        return Result();
    }
    catch (std::exception e)
    {
        return Result(false, "Calculate Luminance exception, " + std::string(e.what()));
    }
}

Result LuminanceWidget::takeImage(const QString& xyzFilter, CaptureData& data, bool autoExposure, float exposure)
{
    McBinocularMode* model = McBinocularMode::instance();
    Result ret = model->setXxzFilter(xyzFilter);
    if (!ret.success)
    {
        return ret;
    }

    if (autoExposure)
    {
        ret = model->autoExposure(1, true);
    }
    else
    {
        ret = model->setExposureTime(exposure);
    }
    if (!ret.success)
    {
        return ret;
    }

    ret = model->getCaptureData(data);
    if (!ret.success)
    {
        return ret;
    }

    //m_viewer->Enqueue(data.frame.m_Image);
    return Result();
}

Result LuminanceWidget::takeImages()
{
    CaptureData dataX;
    Result ret = takeImage("X", dataX, true, 10);
    if (!ret.success)
    {
        return ret;
    }

    CaptureData dataY;
    ret = takeImage("Y", dataY, true, 10);
    if (!ret.success)
    {
        return ret;
    }

    CaptureData dataZ;
    ret = takeImage("Z", dataZ, true, 10);
    if (!ret.success)
    {
        return ret;
    }

    m_onlineRawMap.clear();
    m_onlineRawMap["X"] = dataX;
    m_onlineRawMap["Y"] = dataY;
    m_onlineRawMap["Z"] = dataZ;
    return Result();
}
