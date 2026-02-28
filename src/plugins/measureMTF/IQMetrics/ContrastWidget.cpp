#include "McBinocularMode.h"
#include "ContrastWidget.h"
#include "ContrastDialog.h"
#include <QMessageBox>
#include <QFileDialog>

ContrastWidget::ContrastWidget(CameraViewer* viewer, QMenu* menu, QWidget* parent)
    : MetricsWidgetBase(viewer, menu, parent)
{
    ui.setupUi(this);
    initUI();
    initMenu(menu);
    initAccordion((QAccordion*)parent);

    connect(&m_watcher, SIGNAL(finished()), this, SLOT(threadFinished()));
}

ContrastWidget::~ContrastWidget()
{
}

void ContrastWidget::on_btn_calculate_clicked()
{
    WaitWidget::instance()->startAnimation();

    QFuture future = QtConcurrent::run(this, &ContrastWidget::calculateMetrics);
    m_watcher.setFuture(future);
}

void ContrastWidget::on_btn_positive_clicked()
{
    QString fileName = loadImage();
    if (fileName.isEmpty())
    {
        ui.lineEdit_positive->setText(offlineTipBase.arg("positive"));
        ui.lineEdit_positive->setProperty(LINEEDIT_DATA, false);
    }
    else
    {
        ui.lineEdit_positive->setText(fileName);
        ui.lineEdit_positive->setProperty(LINEEDIT_DATA, true);
    }
}

void ContrastWidget::on_btn_negative_clicked()
{
    QString fileName = loadImage();
    if (fileName.isEmpty())
    {
        ui.lineEdit_negative->setText(offlineTipBase.arg("negative"));
        ui.lineEdit_negative->setProperty(LINEEDIT_DATA, false);
    }
    else
    {
        ui.lineEdit_negative->setText(fileName);
        ui.lineEdit_negative->setProperty(LINEEDIT_DATA, true);
    }
}

void ContrastWidget::on_btn_positiveOn_clicked()
{
    CaptureData positiveData;
    Result ret = takeImage(positiveData);
    if (!ret.success)
    {
        QMessageBox::critical(this, "Contrast take image error", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
            QMessageBox::NoButton);
        return;
    }
    ui.lineEdit_positiveOn->setProperty(LINEEDIT_DATA, true);
    ui.lineEdit_positiveOn->setText(onlineTakeImage.arg("Positive"));
    m_onlineRawMap["positive"] = positiveData;
}

void ContrastWidget::on_btn_negativeOn_clicked()
{
    CaptureData negativeData;
    Result ret = takeImage(negativeData);
    if (!ret.success)
    {
        QMessageBox::critical(this, "Contrast take image error", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
            QMessageBox::NoButton);
        return;
    }
    ui.lineEdit_negativeOn->setProperty(LINEEDIT_DATA, true);
    ui.lineEdit_negativeOn->setText(onlineTakeImage.arg("Negative"));
    m_onlineRawMap["negative"] = negativeData;
}

void ContrastWidget::threadFinished()
{
    WaitWidget::instance()->stopAnimation();

    Result ret = m_watcher.future().result();
    if (!ret.success)
    {
        QMessageBox::critical(this, "Contrast calculate error", QString::fromStdString(ret.errorMsg),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    ContrastDialog* dialog = new ContrastDialog();
    dialog->ui.viewer_positive->getCameraViewer()->setFOVCenter(fovCenter());
    dialog->ui.viewer_negative->getCameraViewer()->setFOVCenter(fovCenter());
    dialog->setResult(m_calculateResult.imgdrawP, m_calculateResult.imgdrawN, m_calculateResult.meanCR);
    dialog->exec();
    delete dialog;
    dialog = nullptr;
}

void ContrastWidget::iqMetricsModeChange(bool offline)
{
    if (offline)
    {
        ui.tabWidget->setCurrentWidget(ui.tab_offline);
    }
    else
    {
        ui.tabWidget->setCurrentWidget(ui.tab_online);
    }
}

void ContrastWidget::initClear()
{
    clear();
    iqMetricsModeChange(isOffline());
}

void ContrastWidget::initUI()
{
    CROPPER_INDEX_MAX = 9;
    MENU_NAME = "menuContrast";
    initImageUI();
}

void ContrastWidget::initImageUI()
{
    clear();
}

void ContrastWidget::initAccordion(QAccordion* accordion)
{
    initAccordionBase(accordion, "Contrast Calculate", 500, true);
}

void ContrastWidget::initMenu(QMenu* menu)
{
    menuUpdate(menu, false);
}

void ContrastWidget::clear()
{
    ui.lineEdit_positive->setEnabled(false);
    ui.lineEdit_negative->setEnabled(false);
    ui.lineEdit_positiveOn->setEnabled(false);
    ui.lineEdit_negativeOn->setEnabled(false);

    ui.lineEdit_positive->setText(offlineTipBase.arg("positive"));
    ui.lineEdit_negative->setText(offlineTipBase.arg("negative"));

    ui.lineEdit_positiveOn->setText(onlineTipBase.arg("Positive"));
    ui.lineEdit_negativeOn->setText(onlineTipBase.arg("Negative"));

    ui.lineEdit_positive->setProperty(LINEEDIT_DATA, false);
    ui.lineEdit_negative->setProperty(LINEEDIT_DATA, false);
    ui.lineEdit_positiveOn->setProperty(LINEEDIT_DATA, false);
    ui.lineEdit_negativeOn->setProperty(LINEEDIT_DATA, false);
}

QString ContrastWidget::loadImage()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, QStringLiteral("Load image:"), "d:", QStringLiteral("(*.png *.jpg *.xpm *.tif *.tiff *.bmp);;All files (*.*)"));

    cv::Mat imge = cv::imread(fileName.toStdString().c_str(), cv::IMREAD_UNCHANGED);
    if (!imge.empty())
    {
        m_viewer->Enqueue(imge);
        return fileName;
    }
    return "";
}

Result ContrastWidget::images(QMap<QString, cv::Mat>& imageMap)
{
    Result ret = judgeData();
    if (!ret.success)
    {
        return ret;
    }

    if (ui.tabWidget->currentWidget() == ui.tab_offline)
    {
        imageMap["positive"] = cv::imread(ui.lineEdit_positive->text().toStdString().c_str(), cv::IMREAD_UNCHANGED);
        imageMap["negative"] = cv::imread(ui.lineEdit_negative->text().toStdString().c_str(), cv::IMREAD_UNCHANGED);
    }
    else
    {
        if (m_onlineRawMap.size() < 2)
        {
            return Result(false, "Please take positive, negative chessboard images.");
        }

        cv::Mat positiveImag;
        Result ret = McBinocularMode::instance()->calculateYImage(positiveImag, m_onlineRawMap["positive"]);
        if (!ret.success) {
            return ret;
        }

        cv::Mat negativeImag;
        ret = McBinocularMode::instance()->calculateYImage(negativeImag, m_onlineRawMap["negative"]);
        if (!ret.success)
        {
            return ret;
        }

        imageMap["positive"] = positiveImag;
        imageMap["negative"] = negativeImag;
    }
    return Result();
}

Result ContrastWidget::judgeData()
{
    if (ui.tabWidget->currentWidget() == ui.tab_offline)
    {
        if (!ui.lineEdit_positive->property(LINEEDIT_DATA).toBool() || !ui.lineEdit_negative->property(LINEEDIT_DATA).toBool())
        {
            return Result(false, "Please load positive, negative chessboard images.");
        }
    }
    else
    {
        if (!ui.lineEdit_positiveOn->property(LINEEDIT_DATA).toBool() || !ui.lineEdit_negativeOn->property(LINEEDIT_DATA).toBool())
        {
            return Result(false, "Please take positive, negative chessboard images.");
        }
    }
    return Result();
}

Result ContrastWidget::takeImage(CaptureData& data)
{
    McBinocularMode* model = McBinocularMode::instance();

    Result ret = model->autoExposure(1, false);
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

Result ContrastWidget::calculateMetrics()
{
    try {
        QMap<QString, cv::Mat> imageMap;
        Result result = images(imageMap);
        if (!result.success)
        {
            return result;
        }

        result = judgeImages(imageMap);
        if (!result.success)
        {
            return result;
        }

        bool flag = MetricsData::instance()->getIQSLB();
        m_calculateResult = m_algorithm.getContrastRatio(imageMap.value("positive"), imageMap.value("negative"), flag);
        if (!m_calculateResult.flag)
        {
            return Result(false, m_calculateResult.errMsg);
        }
        //m_algorithm = chessboard;
        //m_calculateResult = ret;

        return Result();
    }
    catch (std::exception e) {
        return Result(false, "Calculate contrast exception, " + std::string(e.what()));
    }
}
