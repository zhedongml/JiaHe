#include <windows.h>
#include "CraWidget.h"
#include <QMessageBox>
#include "RotationDialog.h"
#include"ImageRotationConfig.h"
#include"McBinocularMode.h"

CraWidget::CraWidget(CameraViewer* viewer, QMenu* menu, QWidget* parent)
    : MetricsWidgetBase(viewer, menu, parent)
{
    ui.setupUi(this);
    initUI();
    initMenu(menu);
    initAccordion((QAccordion*)parent);

    connect(&m_watcher, SIGNAL(finished()), this, SLOT(threadFinished()));
}

CraWidget::~CraWidget()
{
}

void CraWidget::threadFinished()
{
    WaitWidget::instance()->stopAnimation();

    Result ret = m_watcher.future().result();
    QString errorTitle = m_isCRA ? "CRA calculate error" : "Rotation calculate error";
    if (!ret.success)
    {
        QMessageBox::critical(this, errorTitle, QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
            QMessageBox::NoButton);
        return;
    }

    if (m_isCRA) {
        CraDialog* dialog = new CraDialog();
        // dialog->ui.viewer->getCameraViewer()->setFOVCenter(fovCenter());
        dialog->setResult(m_offsetRe.imgdraw, m_offsetRe);
        dialog->exec();

        delete dialog;
        dialog = nullptr;
    }
    else {
        RotationDialog* dialog = new RotationDialog();
        dialog->setResult(m_rotationRe.imgdraw, m_rotationRe.angle);
        dialog->exec();

        delete dialog;
        dialog = nullptr;
    }
}

void CraWidget::initUI()
{
    CROPPER_INDEX_MAX = 9;
    MENU_NAME = "menuCRA";
}

void CraWidget::initAccordion(QAccordion* accordion)
{
    initAccordionBase(accordion, "CRA/Rotation Calculate", 500);
}

void CraWidget::initMenu(QMenu* menu)
{
    menuUpdate(menu, false);
}


Result CraWidget::calculateMetrics()
{

    //  crosshairBorsightBatchTest();

    try {
        cv::Mat rawImage = getCurrentImage();
        if (rawImage.empty())
        {
            return Result(false, "Please load the image first.");
        }

        bool flag = ImageRotationConfig::instance()->isRotation();
        MLIQMetrics::FovOffsetRe ret;
        //if (!flag)
        //{
        //    ret = offset.BoresightNoBorder(rawImage);
        //}
        //else
        {
            MLIQMetrics::ROIParaNew para;
            McBinocularMode::instance()->getRotationMirror(para.rotationAngle, para.mirror);
            //ret = offset.BoresightNoBorder(rawImage, para.rotationAngle, para.mirror);
           // ret = offset.getMultiCrossBoresight(rawImage, para.rotationAngle, MetricsData::instance()->getReticleEyeType(), para.mirror);
            ret = offset.getBoresightGrid(rawImage);
        }

        m_offsetRe = ret;
        return Result();
    }
    catch (std::exception e)
    {
        return Result(false, "Calculate CRA exception, " + std::string(e.what()));
    }
}

Result CraWidget::calculateMetricsRotation()
{
    try
    {
        cv::Mat rawImage = getCurrentImage();
        if (rawImage.empty())
        {
            return Result(false, "Please load the image first.");
        }

        //m_rotationRe = rotation.getCrossRotationNoBorder(rawImage);
       // m_rotationRe = rotation.getMultiCrossRotation(rawImage, MetricsData::instance()->getReticleEyeType());
        m_rotationRe = rotation.getGridRotation(rawImage);
        return Result();
    }
    catch (std::exception e)
    {
        return Result(false, "Calculate CRA exception, " + std::string(e.what()));
    }
}

void CraWidget::on_btn_calculate_clicked()
{
    m_isCRA = true;
    WaitWidget::instance()->startAnimation();

    QFuture future = QtConcurrent::run(this, &CraWidget::calculateMetrics);
    m_watcher.setFuture(future);
}

void CraWidget::on_btn_rotation_clicked()
{
    m_isCRA = false;
    WaitWidget::instance()->startAnimation();

    QFuture future = QtConcurrent::run(this, &CraWidget::calculateMetricsRotation);
    m_watcher.setFuture(future);
}
