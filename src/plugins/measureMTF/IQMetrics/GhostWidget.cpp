#include "McBinocularMode.h"
#include "GhostWidget.h"
#include <QMessageBox>
#include <QFileDialog>

GhostWidget::GhostWidget(CameraViewer* viewer, QMenu* menu, QWidget* parent)
    : MetricsWidgetBase(viewer, menu, parent)
{
    ui.setupUi(this);
    initUI();
    initMenu(menu);
    initAccordion((QAccordion*)parent);

    connect(&m_watcher, SIGNAL(finished()), this, SLOT(threadFinished()));
}

GhostWidget::~GhostWidget()
{
}

void GhostWidget::on_btn_calculate_clicked()
{
    WaitWidget::instance()->startAnimation();

    QFuture future = QtConcurrent::run(this, &GhostWidget::calculateMetrics);
    m_watcher.setFuture(future);
}

void GhostWidget::on_btn_image_clicked()
{
    QString fileName = loadImage();
    if (fileName.isEmpty())
    {
        ui.lineEdit_ghost->setText(offlineTipBase.arg("ghost"));
        ui.lineEdit_ghost->setProperty(LINEEDIT_DATA, false);
    }
    else
    {
        ui.lineEdit_ghost->setText(fileName);
        ui.lineEdit_ghost->setProperty(LINEEDIT_DATA, true);
    }
}

void GhostWidget::on_btn_overExp_clicked()
{
    QString fileName = loadImage();
    if (fileName.isEmpty())
    {
        ui.lineEdit_overExp->setText(offlineTipBase.arg("ghost overexposure"));
        ui.lineEdit_overExp->setProperty(LINEEDIT_DATA, false);
    }
    else
    {
        ui.lineEdit_overExp->setText(fileName);
        ui.lineEdit_overExp->setProperty(LINEEDIT_DATA, true);
    }
}

void GhostWidget::on_btn_imageOn_clicked()
{
    CaptureData image;
    Result ret = takeImage(image);
    if (!ret.success)
    {
        return;
    }
    ui.lineEdit_ghostOn->setProperty(LINEEDIT_DATA, true);
    ui.lineEdit_ghostOn->setText(onlineTakeImage.arg("Ghost"));
    m_onlineRawMap["ghost"] = image;
}

void GhostWidget::on_btn_overExpOn_clicked()
{
    CaptureData image;
    Result ret = takeImage(image);
    if (!ret.success)
    {
        return;
    }
    ui.lineEdit_overExpOn->setProperty(LINEEDIT_DATA, true);
    ui.lineEdit_overExpOn->setText(onlineTakeImage.arg("Ghost overexposure"));
    m_onlineRawMap["overExp"] = image;
}

void GhostWidget::threadFinished()
{
    WaitWidget::instance()->stopAnimation();

    Result ret = m_watcher.future().result();
    if (!ret.success)
    {
        QMessageBox::critical(this, "Ghost calculate error", QString::fromStdString(ret.errorMsg), QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    //GhostDialog *dialog = new GhostDialog();
    //dialog->ui.viewer->getCameraViewer()->setFOVCenter(fovCenter());
    //dialog->ui.viewer_overExpose->getCameraViewer()->setFOVCenter(fovCenter());
    //dialog->setResult(m_calculateResult.img_draw, m_calculateResult.img_drawOverET);
    //dialog->exec();

    //delete dialog;
    //dialog = nullptr;
}

void GhostWidget::iqMetricsModeChange(bool offline)
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

void GhostWidget::initClear()
{
    clear();
    iqMetricsModeChange(isOffline());
}

void GhostWidget::initUI()
{
    CROPPER_INDEX_MAX = 9;
    MENU_NAME = "menuGhost";

    initImageUI();
}

void GhostWidget::initImageUI()
{
    clear();
}

void GhostWidget::initAccordion(QAccordion* accordion)
{
    initAccordionBase(accordion, "Ghost Calculate", 500);
}

void GhostWidget::initMenu(QMenu* menu)
{
    menuUpdate(menu, false);
}

void GhostWidget::clear()
{
    ui.lineEdit_ghost->setEnabled(false);
    ui.lineEdit_overExp->setEnabled(false);
    ui.lineEdit_ghostOn->setEnabled(false);
    ui.lineEdit_overExpOn->setEnabled(false);

    ui.lineEdit_ghost->setText(offlineTipBase.arg("ghost"));
    ui.lineEdit_overExp->setText(offlineTipBase.arg("ghost overexposure"));

    ui.lineEdit_ghostOn->setText(onlineTipBase.arg("Ghost"));
    ui.lineEdit_overExpOn->setText(onlineTipBase.arg("Ghost overexposure"));

    ui.lineEdit_ghost->setProperty(LINEEDIT_DATA, false);
    ui.lineEdit_overExp->setProperty(LINEEDIT_DATA, false);
    ui.lineEdit_ghostOn->setProperty(LINEEDIT_DATA, false);
    ui.lineEdit_overExpOn->setProperty(LINEEDIT_DATA, false);
}

QString GhostWidget::loadImage()
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

Result GhostWidget::images(QMap<QString, cv::Mat>& imageMap)
{
    Result ret = judgeData();
    if (!ret.success)
    {
        return ret;
    }

    if (ui.tabWidget->currentWidget() == ui.tab_offline)
    {
        imageMap["ghost"] = cv::imread(ui.lineEdit_ghost->text().toStdString().c_str(), cv::IMREAD_UNCHANGED);
        imageMap["overExp"] = cv::imread(ui.lineEdit_overExp->text().toStdString().c_str(), cv::IMREAD_UNCHANGED);
    }
    else
    {
        if (m_onlineRawMap.size() < 2)
        {
            return Result(false, "Please take ghost, overexposure images.");
        }

        cv::Mat ghostImg;
        Result ret = McBinocularMode::instance()->calculateYImage(ghostImg, m_onlineRawMap["ghost"]);
        if (!ret.success)
        {
            return ret;
        }

        cv::Mat overImg;
        ret = McBinocularMode::instance()->calculateYImage(overImg, m_onlineRawMap["overExp"]);
        if (!ret.success)
        {
            return ret;
        }

        imageMap["ghost"] = ghostImg;
        imageMap["overExp"] = overImg;
    }
    return Result();
}

Result GhostWidget::judgeData()
{
    if (ui.tabWidget->currentWidget() == ui.tab_offline)
    {
        if (!ui.lineEdit_ghost->property(LINEEDIT_DATA).toBool() ||
            !ui.lineEdit_overExp->property(LINEEDIT_DATA).toBool())
        {
            return Result(false, "Please load ghost, overexposure images.");
        }
    }
    else
    {
        if (!ui.lineEdit_ghostOn->property(LINEEDIT_DATA).toBool() ||
            !ui.lineEdit_overExpOn->property(LINEEDIT_DATA).toBool())
        {
            return Result(false, "Please take ghost, overexposure images.");
        }
    }
    return Result();
}

Result GhostWidget::takeImage(CaptureData& data)
{
    McBinocularMode* model = McBinocularMode::instance();
    Result ret = model->setXxzFilter("Y");
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

Result GhostWidget::calculateMetrics()
{
    QMap<QString, cv::Mat> imageMap;
    Result  result = images(imageMap);
    if (!result.success)
    {
        return result;
    }

    result = judgeImages(imageMap);
    if (!result.success)
    {
        return result;
    }

    try {
        //MLGhostPara ghost;
        //ROIParaNew para = fovROI();
        //GhostRe ghostRe = ghost.getGhostResult(imageMap.value("ghost"), imageMap.value("overExp"),para.rotationAngle,para.mirror);
        //m_algorithm = ghost;
        //m_calculateResult = ghostRe;

        //if (!ghostRe.result.success){
        //    return Result(false, ghostRe.result.errorMsg);
        //}
    }
    catch (std::exception e)
    {
        return Result(false, "Calculate ghost exception, " + std::string(e.what()));
    }
    return Result();
}
