#include "LumEfficiencyDialog.h"
#include "CameraViewer\ColorBarViewer.h"

LumEfficiencyDialog::LumEfficiencyDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    initUI();
}

LumEfficiencyDialog::~LumEfficiencyDialog()
{
}

void LumEfficiencyDialog::setResult(cv::Mat image, double lumEfficiency)
{
    ui.viewer->Enqueue(image);
    ui.lineEdit_value->setText(QString::number(lumEfficiency, 'f', 3));
}

void LumEfficiencyDialog::initUI()
{
    setWindowTitle("Luminance efficiency calculate result");
    setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
}
