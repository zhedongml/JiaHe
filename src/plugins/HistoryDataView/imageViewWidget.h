#pragma once

#include <QWidget>
#include "ui_imageViewWidget.h"
#include "CameraViewer/CameraViewer.h"
#include"CameraViewer\ColorBarViewer.h"
#include"MLAlgorithm\ml_image_public.h"

enum btnType
{
    btn_last,
    btn_next
};
class imageViewWidget : public QWidget
{
	Q_OBJECT

public:
	imageViewWidget(QWidget *parent = nullptr);
	~imageViewWidget();
    void showImg(cv::Mat img,int flag=0);
    void setDisenable(int type);

  signals:
    void switchNextImg();
    void switchLastImg();
 private slots:
    void on_btn_last_clicked();
    void on_btn_next_clicked();

  private:
	Ui::imageViewWidgetClass ui;
    static imageViewWidget* m_imgWidget;
};
