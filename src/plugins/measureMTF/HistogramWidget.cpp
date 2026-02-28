#include "HistogramWidget.h"
#include <QPainter>
Q_DECLARE_METATYPE(HistImageInfo)

HistogramWidget::HistogramWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    setWindowFlag(Qt::WindowStaysOnTopHint, true);
    connect(this, &HistogramWidget::sigUpdateHistogram, this, &HistogramWidget::updataHistogramResult,Qt::ConnectionType::QueuedConnection);
    //connect(&watcher, &QFutureWatcher<void>::finished, this, &HistogramWidget::TnreadFinished);
}

HistogramWidget::~HistogramWidget()
{
}

void HistogramWidget::updataHistogramResult(HistImageInfo info)
{
    ui.lb_histir->setPixmap(QPixmap::fromImage(info.image));
    ui.min_val->setText(QString::number(info.minVal));
    ui.max_val->setText(QString::number(info.maxVal));
    ui.min_pos->setText("(" + QString::number(info.minLoc.x) + "," + QString::number(info.minLoc.y) + ")");
    ui.max_pos->setText("(" + QString::number(info.maxLoc.x) + "," + QString::number(info.maxLoc.y) + ")");
    ui.mean_val->setText(QString::number(info.meanVal));
    ui.std_val->setText(QString::number(info.stddevVal));
    //QFuture<void> future = QtConcurrent::run(this,&HistogramWidget::calculateHistogram, image);
    //watcher.setFuture(future);
}

void HistogramWidget::calculateHistogram(const cv::Mat &image)
{
    QtConcurrent::run([=]() {
        HistImageInfo info;
        cv::Mat grayImage;
        switch (image.type())
        {
        case CV_8UC1:
            grayImage = image;
            break;
        case CV_16UC1:
        //{
        //    //         double minv = 0.0, maxv = 0.0;
        //    //         double* minp = &minv;
        //    //         double* maxp = &maxv;
        //    // cv::minMaxIdx(image, minp, maxp);
        //    // cv::Mat tmp = image / 4095.0 * 255;
        //    // tmp.convertTo(grayImage, CV_8UC1);
        //    image.convertTo(grayImage, CV_8UC1, 255.0 / 4095);
        //    break;
        //}
        case CV_32FC1:
            double minVal, maxVal;
            cv::minMaxLoc(image, &minVal, &maxVal);
            double alpha = 255.0 / (maxVal - minVal);
            double beta = -minVal * 255.0 / (maxVal - minVal);
            grayImage = cv::Mat::zeros(image.size(), CV_8UC1);
            image.convertTo(grayImage, CV_8U, alpha, beta);
            // image.convertTo(grayImage, CV_8UC1, 255.0 / 4095);
            break;
        }

        cv::minMaxLoc(grayImage, &info.minVal, &info.maxVal, &info.minLoc, &info.maxLoc);
        cv::Mat meanVal, stddevVal;
        cv::meanStdDev(grayImage, meanVal, stddevVal);
        info.meanVal = meanVal.at<double>(0, 0);
        info.stddevVal = stddevVal.at<double>(0, 0);

        int histSize = static_cast<int>(info.maxVal - info.minVal + 1);                        
        float range[] = {static_cast<float>(info.minVal), static_cast<float>(info.maxVal + 1)}; 
        const float *histRange = {range};
        cv::Mat hist;
        cv::calcHist(&grayImage, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);

        double maxPixelCount;
        cv::minMaxLoc(hist, nullptr, &maxPixelCount);

        int start = std::to_string(maxPixelCount).length() / 4 * 20;

        int histHeight = 200;
        cv::normalize(hist, hist, 0, histHeight, cv::NORM_MINMAX);

        int histWidth = 500 + start;
        int binWidth = cvRound((double)histWidth / histSize);
        cv::Mat histImage(histHeight + 50, histWidth + 50, CV_8UC3, cv::Scalar(255, 255, 255));

        for (int i = 0; i < histSize; i++)
        {
            cv::rectangle(histImage, cv::Point(binWidth * i + start, histHeight - cvRound(hist.at<float>(i)) + 25),
                          cv::Point(binWidth * (i + 1) + start, histHeight + 25), cv::Scalar(0, 0, 0), cv::FILLED);
        }

        int xStep = std::max(1, histSize / 5);

        cv::line(histImage, cv::Point(start, 25), cv::Point(start, histHeight + 25), cv::Scalar(0, 0, 0), 2);
        cv::line(histImage, cv::Point(start, histHeight + 25),
                 cv::Point(binWidth * histSize / xStep + start, histHeight + 25), cv::Scalar(0, 0, 0), 2);

        for (int i = 0; i <= histSize; i += xStep)
        {
            int x = binWidth * i + start;
            cv::line(histImage, cv::Point(x, histHeight + 25), cv::Point(x, histHeight + 30), cv::Scalar(0, 0, 0), 2);
            cv::putText(histImage, std::to_string(static_cast<int>(info.minVal) + i),
                        cv::Point(x - 10, histHeight + 45), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 0.3, cv::Scalar(0, 0, 0),
                        1);
        }

        int yStep = histHeight / 5; 
        int num = 0;
        for (int i = 0; i <= histHeight; i += yStep)
        {
            int yVal = num * maxPixelCount / 5;
            int y = histHeight - i + 25;
            cv::line(histImage, cv::Point(start - 4, y), cv::Point(start, y), cv::Scalar(0, 0, 0), 2);
            cv::putText(histImage, std::to_string(yVal), cv::Point(0, y + 5), cv::FONT_HERSHEY_SIMPLEX, 0.3,
                        cv::Scalar(0, 0, 0), 1);
            num++;
        }

        QImage img = QImage(histImage.data, histImage.cols, histImage.rows, histImage.step, QImage::Format_RGB888).rgbSwapped();
        info.image = img.copy();
        emit sigUpdateHistogram(info);
    }); 
}

void HistogramWidget::setAction(QAction *action)
{
    m_action = action;
}

void HistogramWidget::closeEvent(QCloseEvent *event)
{
    if (m_action)
    {
        m_action->setChecked(false);
    }
}

