#include "LuminanceDialog.h"
#include"CameraViewer\ColorBarViewer.h"

LuminanceDialog::LuminanceDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    initUI();
}

LuminanceDialog::~LuminanceDialog()
{
}

void LuminanceDialog::setResult(cv::Mat image, MLIQMetrics::LuminanceRe lu)
{
    ui.viewer->Enqueue(image);

    if (lu.flag == false) {
        ui.lineEdit_valueMin->setText("Error");
        ui.lineEdit_valueMax->setText("Error");
    }
    else {
        ui.lineEdit_valueMin->setText(QString::number(lu.rectMean, 'f', 3));
        ui.lineEdit_valueMax->setText(QString::number(lu.rectCov, 'f', 3));
    }
}

void LuminanceDialog::initUI()
{
    setWindowTitle("Luminance calculate result");
    setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
}
