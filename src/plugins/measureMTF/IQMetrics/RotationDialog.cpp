#include "RotationDialog.h"

RotationDialog::RotationDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    initUI();
}

RotationDialog::~RotationDialog()
{
}

void RotationDialog::setResult(cv::Mat image, float degree)
{
    ui.viewer->Enqueue(image);
    ui.lineEdit_value->setText(QString::number(degree, 'f', 3));
}

void RotationDialog::initUI()
{
    setWindowTitle("Rotation calculate result");
    setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
}
