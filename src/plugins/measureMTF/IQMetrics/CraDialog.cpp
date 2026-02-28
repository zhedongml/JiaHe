#include "CraDialog.h"
#include "CameraViewer\ColorBarViewer.h"

CraDialog::CraDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    initUI();
}

CraDialog::~CraDialog()
{
}

void CraDialog::setResult(cv::Mat image, FovOffsetRe craResult)
{
    ui.viewer->Enqueue(image);
    //ui.lineEdit_value->setText(QString::number(craResult.angle, 'f', 3));
    ui.lineEdit_x->setText(QString::number(craResult.H, 'f', 3));
    ui.lineEdit_y->setText(QString::number(craResult.V, 'f', 3));
}

void CraDialog::initUI()
{
    setWindowTitle("CRA calculate result");
    setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);

    ui.lineEdit_value->hide();
    ui.label->hide();
}
