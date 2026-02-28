#include "LumEfficiencyWidget.h"
#include "LumEfficiencyDialog.h"
#include <QFile>
#include "McBinocularMode.h"
#include <QMessageBox>

LumEfficiencyWidget::LumEfficiencyWidget(CameraViewer* viewer, QMenu* menu, QWidget* parent)
    : MetricsWidgetBase(viewer, menu, parent)
{
    ui.setupUi(this);
    initUI();
    initMenu(menu);
    initAccordion((QAccordion*)parent);

    connect(&m_watcher, SIGNAL(finished()), this, SLOT(threadFinished()));
}

LumEfficiencyWidget::~LumEfficiencyWidget()
{
}

void LumEfficiencyWidget::loadImageSlot(const QString& fileName)
{
    //if (!m_isCurrentAccordion){
    //    return;
    //}

    QFileInfo file(fileName);
    QString name = file.baseName();

    if (name.startsWith("W_", Qt::CaseInsensitive))
    {
        ui.comboBox_color->setCurrentText("W");
    }
    else if (name.startsWith("R_", Qt::CaseInsensitive))
    {
        ui.comboBox_color->setCurrentText("R");
    }
    else if (name.startsWith("G_", Qt::CaseInsensitive))
    {
        ui.comboBox_color->setCurrentText("G");
    }
    else if (name.startsWith("B_", Qt::CaseInsensitive))
    {
        ui.comboBox_color->setCurrentText("B");
    }
}

void LumEfficiencyWidget::threadFinished()
{
    WaitWidget::instance()->stopAnimation();

    Result ret = m_watcher.future().result();
    if (!ret.success)
    {
        QMessageBox::critical(this, "Luminance efficiency calculate error", QString::fromStdString(ret.errorMsg), QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    //LumEfficiencyDialog *dialog = new LumEfficiencyDialog();
    //dialog->ui.viewer->getCameraViewer()->setFOVCenter(fovCenter());
    //dialog->setResult(m_luminance.getImgDraw(), m_calculateResult.efficiency);
    //dialog->exec();

    //delete dialog;
    //dialog = nullptr;
}

void LumEfficiencyWidget::on_btn_takeImg_clicked()
{
    //ui.btn_takeImg->setEnabled(false);
    Result ret = takeImages();
    //ui.btn_takeImg->setEnabled(true);

    if (!ret.success)
    {
        QMessageBox::critical(this, "Luminance calculate error", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
            QMessageBox::NoButton);
    }
}

void LumEfficiencyWidget::on_btn_calculate_clicked()
{
    WaitWidget::instance()->startAnimation();

    QFuture future = QtConcurrent::run(this, &LumEfficiencyWidget::calculateMetrics);
    m_watcher.setFuture(future);
}

void LumEfficiencyWidget::initUI()
{
    CROPPER_INDEX_MAX = 9;
    MENU_NAME = "menuLumEfficiency";

    ui.comboBox_color->addItem("W");
    ui.comboBox_color->addItem("R");
    ui.comboBox_color->addItem("G");
    ui.comboBox_color->addItem("B");
    ui.comboBox_color->setCurrentIndex(0);
}

void LumEfficiencyWidget::initAccordion(QAccordion* accordion)
{
    initAccordionBase(accordion, "Luminance Efficiency Calculate", 500);
}

void LumEfficiencyWidget::initMenu(QMenu* menu)
{
    menuUpdate(menu, false);
}

Result LumEfficiencyWidget::calculateMetrics()
{
    try {
        string color;
        cv::Mat rawImage;
        if (isOffline())
        {
            rawImage = getCurrentImage(false);
            if (rawImage.empty())
            {
                return Result(false, "Please load the image first.");
            }

            color = ui.comboBox_color->currentText().toStdString();
        }
        else
        {
            Result ret = takeImages();
            if (!ret.success)
            {
                return ret;
            }

            QMap<QString, cv::Mat> imageXYZ;
            ret = McBinocularMode::instance()->calculateXYZImage(imageXYZ, m_onlineRawMap);
            if (!ret.success)
            {
                return ret;
            }
            rawImage = imageXYZ["Y"];

            color = MetricsData::instance()->getColor().toStdString();
        }

        Result result = judgeImages(rawImage);
        if (!result.success)
        {
            return result;
        }

        int binNum = sqrt(11264 * 9200 / rawImage.total());

        //ROIParaNew para = fovROI();
        //MLLuminance luminance;
        //EfficiencyRe ret = luminance.getLuminanceEfficiency(rawImage, color, para, binNum);
        //if (!ret.result.success)
        //{
        //    return Result(false, ret.result.errorMsg);
        //}

        //m_luminance = luminance;
        //m_calculateResult = ret;
        return Result();
    }
    catch (std::exception e)
    {
        return Result(false, "Calculate Luminance efficiency exception, " + std::string(e.what()));
    }
}

Result LumEfficiencyWidget::takeImage(const QString& xyzFilter, CaptureData& data, bool autoExposure, float exposure)
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
        QMessageBox::critical(this, "Luminance calculate error", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
            QMessageBox::NoButton);
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

Result LumEfficiencyWidget::takeImages()
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
