#include "ChromacityDialog.h"
#include "CameraViewer\ColorBarViewer.h"

ChromacityDialog::ChromacityDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    initUI();
}

ChromacityDialog::~ChromacityDialog()
{
}

//void ChromacityDialog::setResult(cv::Mat image, ChromacityRE result, cv::Mat imageY)
//{
//    ui.viewer->Enqueue(image);
//    ui.lineEdit_value->setText(QString::number(result.chromUniformity, 'f', 3));
//    
//    ui.viewer->SetImageModel(IQ1);
//    ui.viewer->SetYxy(imageY, result.xMat, result.yMat);
//}

void ChromacityDialog::initUI()
{
    setWindowTitle("Chromacity calculate result");
    setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
}
