#include "imageViewWidget.h"

imageViewWidget::imageViewWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    ui.btn_last->setIcon(QIcon(":/image/image/double-left.png"));
    ui.btn_next->setIcon(QIcon(":/image/image/double-right.png"));
   
}

imageViewWidget::~imageViewWidget()
{
}

void imageViewWidget::showImg(cv::Mat img,int flag)
{
   
    ui.graphicsView->Enqueue(img);
   
}

void imageViewWidget::setDisenable(int type)
{
    if (type == -1)
    {
        ui.btn_last->setEnabled(true);
        ui.btn_next->setEnabled(true);
    }
    else if (type == btn_last){
        ui.btn_last->setEnabled(false);
        ui.btn_next->setEnabled(true);  
    }

    else
    {
        ui.btn_last->setEnabled(true);
        ui.btn_next->setEnabled(false);
    }
}

void imageViewWidget::on_btn_last_clicked()
{
    emit switchLastImg();
}

void imageViewWidget::on_btn_next_clicked()
{
    emit switchNextImg();
}


