#include "AutoExposureWidget.h"
#include "CameraModel.h"
#include <pluginsystem\Services.h>
#include <QTimer>

AutoExposureWidget::AutoExposureWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    ui.statusLabel->setText("");
    connect(CameraModel::GetInstance(), SIGNAL(updateCameraGrayLevelSignal(int)), this, SLOT(ShowGrayValue(int)));
    ui.statusLabel->hide();
    ShowGrayValue(0);
}

AutoExposureWidget::~AutoExposureWidget()
{
}

void AutoExposureWidget::ShowGrayValue(int gray)
{
    ui.graylevelbar->setValue(gray);
    double exposurtime = CameraModel::GetInstance()->GetExposureTime() / 1000.00;
    ui.curExposureTimeTxt->setText(QString::number(exposurtime, 'f', 3));
}

void AutoExposureWidget::on_exposuretimetxt_returnPressed()
{
    QString exposureTime = ui.exposuretimetxt->text();
    CameraModel::GetInstance()->SetExposureTime(exposureTime.toDouble() * 1000);
}

void AutoExposureWidget::on_autoexposurebtn_clicked()
{
    ui.statusLabel->setText("AutoExposure...");
    QTimer::singleShot(50, this, [=] {
        CameraModel::GetInstance()->SetInitExposureTime(10000);
        CameraModel::GetInstance()->SetMLExposureAuto();

        AutoExposureStatus AEStatus = CameraModel::GetInstance()->getAEStatus();
        if (AEStatus != AutoExposureStatus::AE_normal_)
        {
            std::string AEStatusStr = CameraModel::GetInstance()->getAEStatusStr();
            ui.statusLabel->setText("AutoExposure Status:" + QString::fromStdString(AEStatusStr));
        }
        else
        {
            ui.statusLabel->setText("AutoExposure Fnished!");
        }
        ui.exposuretimetxt->setText(QString::number((double)CameraModel::GetInstance()->GetExposureTime() / 1000));
        });
}
void AutoExposureWidget::on_SetExposureTime_clicked()
{
    QString exposureTime = ui.exposuretimetxt->text();
    CameraModel::GetInstance()->SetExposureTime(exposureTime.toDouble() * 1000);
}
