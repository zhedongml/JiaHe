#include "GhostDialog.h"
#include "CameraViewer\ColorBarViewer.h"

GhostDialog::GhostDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    initUI();
}

GhostDialog::~GhostDialog()
{
}

void GhostDialog::setResult(cv::Mat image, cv::Mat imageOverExpose)
{
    ui.viewer->Enqueue(image);
    ui.viewer_overExpose->Enqueue(imageOverExpose);
    //ui.lineEdit_value->setText(QString::number(value, 'f', 3));
}

void GhostDialog::initUI()
{
    setWindowTitle("Ghost calculate result");
    setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    ui.groupBox->hide();
}
