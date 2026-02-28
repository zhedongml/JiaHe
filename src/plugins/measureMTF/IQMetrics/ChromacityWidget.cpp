#include "MLColorimeterMode.h"
#include "ChromacityWidget.h"
#include "ChromacityDialog.h"
#include <QMessageBox>
#include <QFileDialog>

ChromacityWidget::ChromacityWidget(CameraViewer* viewer, QMenu* menu, QWidget* parent)
    : MetricsWidgetBase(viewer, menu, parent)
{
    ui.setupUi(this);
    initUI();
    initMenu(menu);
    initAccordion((QAccordion*)parent);

    connect(&m_watcher, SIGNAL(finished()), this, SLOT(threadFinished()));
}

ChromacityWidget::~ChromacityWidget()
{
}

void ChromacityWidget::on_btn_loadX_clicked()
{
    QString fileName = loadImage();
    if (fileName.isEmpty()) {
        ui.lineEdit_X->setText(offlineTipBase.arg("X"));
        ui.lineEdit_X->setProperty(LINEEDIT_DATA, false);
    }
    else {
        ui.lineEdit_X->setText(fileName);
        ui.lineEdit_X->setProperty(LINEEDIT_DATA, true);
    }
}

void ChromacityWidget::on_btn_loadY_clicked()
{
    QString fileName = loadImage();
    if (fileName.isEmpty())
    {
        ui.lineEdit_Y->setText(offlineTipBase.arg("Y"));
        ui.lineEdit_Y->setProperty(LINEEDIT_DATA, false);
    }
    else
    {
        ui.lineEdit_Y->setText(fileName);
        ui.lineEdit_Y->setProperty(LINEEDIT_DATA, true);
    }
}

void ChromacityWidget::on_btn_loadZ_clicked()
{
    QString fileName = loadImage();
    if (fileName.isEmpty())
    {
        ui.lineEdit_Z->setText(offlineTipBase.arg("Z"));
        ui.lineEdit_Z->setProperty(LINEEDIT_DATA, false);
    }
    else
    {
        ui.lineEdit_Z->setText(fileName);
        ui.lineEdit_Z->setProperty(LINEEDIT_DATA, true);
    }
}

void ChromacityWidget::on_btn_takeImg_clicked()
{
    ui.btn_takeImg->setEnabled(false);

    clear();

    CaptureData dataX;
    bool autoExposure = ui.checkBox_autoExpose->isChecked();
    Result ret = takeImage("X", dataX, autoExposure, ui.lineEdit_xET->text().toFloat());
    if (!ret.success) {
        ui.btn_takeImg->setEnabled(true);
        return;
    }
    ui.lineEdit_Xon->setProperty(LINEEDIT_DATA, true);
    ui.lineEdit_Xon->setText(onlineTakeImage.arg("X"));

    CaptureData dataY;
    ret = takeImage("Y", dataY, autoExposure, ui.lineEdit_yET->text().toFloat());
    if (!ret.success)
    {
        ui.btn_takeImg->setEnabled(true);
        return;
    }
    ui.lineEdit_Yon->setProperty(LINEEDIT_DATA, true);
    ui.lineEdit_Yon->setText(onlineTakeImage.arg("Y"));

    CaptureData dataZ;
    ret = takeImage("Z", dataZ, autoExposure, ui.lineEdit_zET->text().toFloat());
    if (!ret.success)
    {
        ui.btn_takeImg->setEnabled(true);
        return;
    }
    ui.lineEdit_Zon->setProperty(LINEEDIT_DATA, true);
    ui.lineEdit_Zon->setText(onlineTakeImage.arg("Z"));

    m_onlineRawMap.clear();
    m_onlineRawMap["X"] = dataX;
    m_onlineRawMap["Y"] = dataY;
    m_onlineRawMap["Z"] = dataZ;
    ui.btn_takeImg->setEnabled(true);
}

void ChromacityWidget::on_checkBox_autoExpose_stateChanged()
{
    bool manualET = !ui.checkBox_autoExpose->isChecked();
    ui.lineEdit_xET->setEnabled(manualET);
    ui.lineEdit_yET->setEnabled(manualET);
    ui.lineEdit_zET->setEnabled(manualET);
}

void ChromacityWidget::threadFinished()
{
    WaitWidget::instance()->stopAnimation();

    Result ret = m_watcher.future().result();
    if (!ret.success)
    {
        QMessageBox::critical(this, "Chromacitys calculate error", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
            QMessageBox::NoButton);
        return;
    }
    //ChromacityDialog *dialog = new ChromacityDialog();
    //dialog->ui.viewer->getCameraViewer()->setFOVCenter(fovCenter());
    //dialog->setResult(m_luminance.getImgDraw(), m_calculateResult, m_xyzMap.value("Y"));
    //dialog->exec();

    //delete dialog;
    //dialog = nullptr;
    //{
    //    return;  //TODO: test
    //    m_viewer->SetImageModel(IQ1);
    //    cv::Mat YY = m_xyzMap.value("Y").clone();
    //    m_viewer->SetYxy(YY, m_calculateResult.xMat, m_calculateResult.yMat);
    //}
}

void ChromacityWidget::iqMetricsModeChange(bool offline)
{
    //explain: EXE first time displaying UI, it must be tab_online, otherwise UI widget width will be large
    static bool first = true;
    if (first) {
        ui.tabWidget->setCurrentWidget(ui.tab_online);
        first = false;
        return;
    }

    if (offline) {
        ui.tabWidget->setCurrentWidget(ui.tab_offline);
    }
    else {
        ui.tabWidget->setCurrentWidget(ui.tab_online);
    }
}

void ChromacityWidget::initClear()
{
    clear();
    iqMetricsModeChange(isOffline());
}

void ChromacityWidget::on_btn_calculate_clicked()
{
    WaitWidget::instance()->startAnimation();

    QFuture future = QtConcurrent::run(this, &ChromacityWidget::calculateMetrics);
    m_watcher.setFuture(future);
}

void ChromacityWidget::initUI()
{
    CROPPER_INDEX_MAX = 9;
    MENU_NAME = "menuChromacity";

    ui.lineEdit_xET->setText("500");
    ui.lineEdit_yET->setText("200");
    ui.lineEdit_zET->setText("800");

    initImageUI();

    ui.checkBox_autoExpose->setChecked(true);
}

void ChromacityWidget::initImageUI()
{
    clear();
}

void ChromacityWidget::initAccordion(QAccordion* accordion)
{
    initAccordionBase(accordion, "Chromacity Calculate", 500);
}

void ChromacityWidget::initMenu(QMenu* menu)
{
    menuUpdate(menu, false);
}

void ChromacityWidget::clear()
{
    ui.lineEdit_X->setEnabled(false);
    ui.lineEdit_Y->setEnabled(false);
    ui.lineEdit_Z->setEnabled(false);
    ui.lineEdit_Xon->setEnabled(false);
    ui.lineEdit_Yon->setEnabled(false);
    ui.lineEdit_Zon->setEnabled(false);

    ui.lineEdit_X->setText(offlineTipBase.arg("X"));
    ui.lineEdit_Y->setText(offlineTipBase.arg("Y"));
    ui.lineEdit_Z->setText(offlineTipBase.arg("Z"));
    ui.lineEdit_Xon->setText(onlineTipBase.arg("X"));
    ui.lineEdit_Yon->setText(onlineTipBase.arg("Y"));
    ui.lineEdit_Zon->setText(onlineTipBase.arg("Z"));

    ui.lineEdit_X->setProperty(LINEEDIT_DATA, false);
    ui.lineEdit_Y->setProperty(LINEEDIT_DATA, false);
    ui.lineEdit_Z->setProperty(LINEEDIT_DATA, false);
    ui.lineEdit_Xon->setProperty(LINEEDIT_DATA, false);
    ui.lineEdit_Yon->setProperty(LINEEDIT_DATA, false);
    ui.lineEdit_Zon->setProperty(LINEEDIT_DATA, false);
}

QString ChromacityWidget::loadImage()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, QStringLiteral("Load image:"), "d:", QStringLiteral("(*.png *.jpg *.xpm *.tif *.tiff *.bmp);;All files (*.*)"));

    cv::Mat imge = cv::imread(fileName.toStdString().c_str(), cv::IMREAD_UNCHANGED);
    if (!imge.empty()) {
        m_viewer->Enqueue(imge);
        return fileName;
    }
    return "";
}

Result ChromacityWidget::xyzImages(QMap<QString, cv::Mat>& imageMap)
{
    Result ret = judgeData();
    if (!ret.success)
    {
        return ret;
    }

    imageMap.clear();
    if (ui.tabWidget->currentWidget() == ui.tab_offline) {
        imageMap["X"] = cv::imread(ui.lineEdit_X->text().toStdString().c_str(), cv::IMREAD_UNCHANGED);
        imageMap["Y"] = cv::imread(ui.lineEdit_Y->text().toStdString().c_str(), cv::IMREAD_UNCHANGED);
        imageMap["Z"] = cv::imread(ui.lineEdit_Z->text().toStdString().c_str(), cv::IMREAD_UNCHANGED);
    }
    else {
        QMap<QString, cv::Mat> imageXYZ;
        McBinocularMode::instance()->calculateXYZImage(imageMap, m_onlineRawMap);
    }
    return Result();
}

Result ChromacityWidget::judgeData()
{
    if (ui.tabWidget->currentWidget() == ui.tab_offline)
    {
        if (!ui.lineEdit_X->property(LINEEDIT_DATA).toBool()
            || !ui.lineEdit_Y->property(LINEEDIT_DATA).toBool()
            || !ui.lineEdit_Z->property(LINEEDIT_DATA).toBool())
        {
            return Result(false, "Please load X, Y, Z images.");
        }
    }
    else
    {
        if (!ui.lineEdit_Xon->property(LINEEDIT_DATA).toBool() || !ui.lineEdit_Yon->property(LINEEDIT_DATA).toBool() ||
            !ui.lineEdit_Zon->property(LINEEDIT_DATA).toBool())
        {
            return Result(false, "Please take X, Y, Z images.");
        }
    }
    return Result();
}

void ChromacityWidget::message(const std::string& info)
{
    QMessageBox::critical(this, "Chromacity calculate error", QString::fromStdString(info), QMessageBox::Ok, QMessageBox::NoButton);
}

Result ChromacityWidget::takeImage(const QString& xyzFilter, CaptureData& data, bool autoExposure, float exposure)
{
    McBinocularMode* model = McBinocularMode::instance();
    Result ret = model->setXxzFilter(xyzFilter);
    if (!ret.success)
    {
        return ret;
    }

    if (autoExposure) {
        //TODO: init value
        ret = model->autoExposure(1, true);
    }
    else {
        ret = model->setExposureTime(exposure);
    }
    if (!ret.success)
    {
        message(ret.errorMsg);
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

Result ChromacityWidget::calculateMetrics()
{
    try {
        QMap<QString, cv::Mat> imageMap;
        Result result = ChromacityWidget::xyzImages(imageMap);
        if (!result.success)
        {
            return result;
        }

        result = judgeImages(imageMap);
        if (!result.success)
        {
            return result;
        }

        //MLLuminance luminance;
        //ROIParaNew para = fovROI();
        //ChromacityRE ret;
        //if (imageMap.contains("xx") && !imageMap.contains("yy"))
        //{
        //    ret = luminance.getChromacity(imageMap.value("xx"), imageMap.value("yy"), para);
        //}else{
        //    ret = luminance.getChromacity(imageMap.value("X"), imageMap.value("Y"), imageMap.value("Z"), para);
        //}

        //if (!ret.result.success)
        //{
        //    return Result(false, ret.result.errorMsg);
        //}

        //m_luminance = luminance;
        //m_calculateResult = ret;
        //m_xyzMap = imageMap;
        return Result();
    }
    catch (std::exception e)
    {
        return Result(false, "Calculate chromacity exception, " + std::string(e.what()));
    }
}
