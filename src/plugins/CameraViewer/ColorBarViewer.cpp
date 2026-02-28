#include "ColorBarViewer.h"
#include "ui_ColorBarViewer.h"
#include <QFileDialog>
#include <QMessageBox>		

ColorBarViewer::ColorBarViewer(QWidget *parent) : 
    QWidget(parent), 
    ui(new Ui::ColorBarViewerClass())

{
	ui->setupUi(this);
    ui->btn_save->hide();
}

ColorBarViewer::~ColorBarViewer()
{
}

CameraViewer *ColorBarViewer::getCameraViewer()
{
    return ui->graphicsView;
}
void ColorBarViewer::Enqueue(Mat &sour)
{
        m_imgRaw = sour.clone();
        m_imgColor.data = NULL;
     //ui->graphicsView->Enqueue(sour);
    on_applyColorMap_stateChanged(m_applyColorMap);
}
void ColorBarViewer::SetImageModel(ImageModel model)
{
    getCameraViewer()->SetImageModel(model);
}
void ColorBarViewer::SetYxy(Mat &Y, Mat &x, Mat &y)
{
    getCameraViewer()->SetYxy(Y, x, y);
}
void ColorBarViewer::on_applyColorMap_stateChanged(int state)
{
    if (state)
    {
         m_applyColorMap = 1;     
         if (m_imgColor.data==NULL)
         {
             m_imgColor = converToColorMapMat(m_imgRaw);
         } 
         getCameraViewer()->Enqueue(m_imgColor);
         if (colorMapBar == nullptr)
         {
             colorMapBar = new ColorMapBar(ui->graphicsView);
             colorMapBar->setMinMax(0, 255);
             ui->verticalLayout_2->addWidget(colorMapBar);
         }
    }
    else
    {
        m_applyColorMap = 0;
        getCameraViewer()->Enqueue(m_imgRaw);
        ui->verticalLayout_2->removeWidget(colorMapBar);
        delete colorMapBar;
        colorMapBar = nullptr;
    }
}

void ColorBarViewer::on_btn_save_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(
        this, QStringLiteral("Save image:"), "d:", QStringLiteral("(*.png *.jpg *.xpm  *.tif);;All files (*.*)"));
    if (!fileName.isEmpty())
    {


        if (m_applyColorMap)
        {
            if (m_imgColor.data != NULL)
            {

                std::vector<int> compression_params;
                compression_params.push_back(cv::IMWRITE_TIFF_COMPRESSION);
                compression_params.push_back(1);
                cv::imwrite(fileName.toStdString(), m_imgColor, compression_params);
            }
        }
        else 
        {
            if (m_imgRaw.data != NULL)
            {
                std::vector<int> compression_params;
                compression_params.push_back(cv::IMWRITE_TIFF_COMPRESSION);
                compression_params.push_back(1);
                cv::imwrite(fileName.toStdString(), m_imgRaw, compression_params);
            }
        }
    }
}

cv::Mat ColorBarViewer::converToColorMapMat(cv::Mat img, int type)
{
    cv::Mat imgMap;
    cv::Mat img8 = cv::Mat::zeros(img.size(), CV_8U);
    if (img.type() != CV_8U)
    {
        cv::Mat tmp;
        cv::normalize(img, tmp, 0, 255, cv::NORM_MINMAX);
        cv::convertScaleAbs(tmp, img8);
    }
    else
        img8 = img.clone();
    cv::applyColorMap(img8, imgMap, type);
    return imgMap;
}
