#include "ContrastDialog.h"
#include "CameraViewer\ColorBarViewer.h"

ContrastDialog::ContrastDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    initUI();
}

ContrastDialog::~ContrastDialog()
{
}

void ContrastDialog::setResult(cv::Mat imagePos, cv::Mat imageNeg, double contrast)
{
    ui.viewer_positive->Enqueue(imagePos);
    ui.viewer_negative->Enqueue(imageNeg);
    ui.lineEdit_value->setText(QString::number(contrast, 'f', 3));
}

void ContrastDialog::initUI()
{
    setWindowTitle("Contrast calculate result");
    setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
}
