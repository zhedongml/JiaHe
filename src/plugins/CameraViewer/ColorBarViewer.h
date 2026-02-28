#pragma once

#include <QWidget>
#include "CameraViewer.h"
#include "cameraviewer_global.h"
#include "colorMapBar.h"
namespace Ui
{
class ColorBarViewerClass;
}

class CAMERAVIEWER_EXPORT ColorBarViewer : public QWidget
{
    Q_OBJECT 

public:
	ColorBarViewer(QWidget *parent = nullptr);
	~ColorBarViewer();
	CameraViewer *getCameraViewer();
    int m_applyColorMap = 0;

    void Enqueue(Mat &sour);
    void SetImageModel(ImageModel model);
    void SetYxy(Mat &Y, Mat &x, Mat &y);


  private slots:
    void on_applyColorMap_stateChanged(int state);
    void on_btn_save_clicked();

  private:
    cv::Mat converToColorMapMat(cv::Mat img, int type = 2);
    cv::Mat m_imgRaw;
    cv::Mat m_imgColor;

  private:
	Ui::ColorBarViewerClass *ui;
    ColorMapBar* colorMapBar = nullptr;
   // CameraViewer *graphicsView;
};
