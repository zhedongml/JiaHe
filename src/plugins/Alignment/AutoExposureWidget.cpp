#include "AutoExposureWidget.h"
#include "CameraModel.h"

AutoExposureWidget::AutoExposureWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    connect(CameraModel::GetInstance(), SIGNAL(updateCameraGrayLevelSignal(int)), this, SLOT(ShowGrayValue(int)));
    ShowGrayValue(0);
}

AutoExposureWidget::~AutoExposureWidget()
{}

void AutoExposureWidget::ShowGrayValue(int gray)
{
    ui.graylevelbar->setValue(gray);
    double exposurtime = CameraModel::GetInstance()->GetExposureTime() / 1000.00;
    ui.curExposureTimeTxt->setText(QString::number(exposurtime, 'f', 3));
}

void AutoExposureWidget::on_autoexposurebtn_clicked()
{
    CameraModel::GetInstance()->SetInitExposureTime(10000);
    CameraModel::GetInstance()->SetMLExposureAuto();
    ui.exposuretimetxt->setText(QString::number((double)CameraModel::GetInstance()->GetExposureTime() / 1000));
}
void AutoExposureWidget::on_SetExposureTime_clicked()
{
    QString exposureTime = ui.exposuretimetxt->text();
    CameraModel::GetInstance()->SetExposureTime(exposureTime.toDouble() * 1000);
}

void AutoExposureWidget::on_exposuretimetxt_returnPressed()
{
    QString exposureTime = ui.exposuretimetxt->text();
    CameraModel::GetInstance()->SetExposureTime(exposureTime.toDouble() * 1000);
}

