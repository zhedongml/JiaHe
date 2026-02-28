#include "CameraViewer.h"
#include <QScrollBar>
#include <QtConcurrent/qtconcurrentrun.h>
#include <VmbTransform.h>

#define NUM_FRAMES 3
#define VIEW_CENTER viewport()->rect().center()
#define VIEW_WIDTH viewport()->rect().width()
#define VIEW_HEIGHT viewport()->rect().height()
Q_DECLARE_METATYPE(cv::Point)
CameraViewer::CameraViewer(QWidget *parent)
    : QGraphicsView(parent), m_isFirstTime(true), m_ZoomFactor(0), m_bIsFitToWindow(false), m_bIsPressed(false),
      m_Xaxis(nullptr), m_Yaxis(nullptr), m_XaxisAngle(nullptr), m_YaxisAngle(nullptr), m_GrayLeveText(nullptr),
      m_ImageFromat(Mono8), m_ImageModel(MTF), m_IQImageWidth(0), m_IQImageHeight(0), m_bGraylevelEnable(false)
{
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setMouseTracking(true);
    setCursor(Qt::CrossCursor);
    m_Scene = new QGraphicsScene();
    m_PixmapItem = new QGraphicsPixmapItem();
    m_LineCutItem = new QGraphicsLineItem();
    m_LineCutItem->setPen(QPen(QColor(59, 90, 255), 3));
    m_LineCutItem->setZValue(1);
    m_LineCutItem->hide();
    m_Scene->addItem(m_LineCutItem);

    m_GrayLevel = new QLabel();
    m_GrayLevel->setDisabled(true);
    m_circles.clear();
    QFont ft;
    ft.setPointSize(10);
    m_GrayLevel->setFont(ft);
    m_GrayLevel->setStyleSheet("color:red;background:transparent;");
    m_GrayLevel->setParent(this);
    m_GrayLevel->setGeometry(2, 0, 500, 40);
    m_GrayLevel->show();
    m_GrayLevel->setParent(this);
    m_Scene->addItem(m_PixmapItem);
    setScene(m_Scene);
    connect(this, SIGNAL(imageReadySignal()), this, SLOT(onImageReady()));
    loadAboutFOVConfig("./config/IQMetricsParametersConfig.json ");
}

CameraViewer::~CameraViewer()
{
}

void CameraViewer::setLineItemVisible(bool visible)
{
    {
        QMutexLocker lock(&m_lockLineData);
        m_CutLineAxisYGrayDatas.clear();
    }
    lineItemVisible = visible;
    if (visible)
    {
        setCursor(Qt::ArrowCursor);
        m_LineCutItem->show();
    }
    else
    {
        m_LineCutItem->hide();
    }
}

void CameraViewer::calMinMaxSta(bool isCal)
{
    QMutexLocker local_lock(&m_lockLine);

    m_isCalMinMax = isCal;
    if (m_isCalMinMax && !m_showImage.empty())
    {
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(m_showImage, &minVal, &maxVal, &minLoc, &maxLoc);
        emit sigMinMaxUpdate(minVal, maxVal, minLoc, maxLoc);
    }
}

void CameraViewer::calHistogram(bool isCal)
{
    m_isCalHist = isCal;
    if (isCal)
    {
        emit sigHistogramUpdate(m_showImage);
    }
}

void CameraViewer::onImageReady()
{
    if (m_isCalMinMax && !m_showImage.empty())
    {
        QMutexLocker local_lock(&m_lockLine);
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(m_showImage, &minVal, &maxVal, &minLoc, &maxLoc);
        emit sigMinMaxUpdate(minVal, maxVal, minLoc, maxLoc);
    }
    if (m_isCalHist && !m_showImage.empty())
    {
        QMutexLocker local_lock(&m_lockLine);
        emit sigHistogramUpdate(m_showImage);
    }
    if (lineItemVisible && lineCutItemFinished /*!m_CutLineAxisYGrayDatas.isEmpty()*/)
    {
        QtConcurrent::run(this, &CameraViewer::calPlotProfile);
        /*emit sigUpdateCutLine(m_CutLineAxisYGrayDatas);*/
    }
    if (m_ImageModel == MTF)
    {
        m_Image = GetQImage();
        m_IQImageWidth = m_Image.width();
        m_IQImageHeight = m_Image.height();
        m_Scene->setSceneRect(0, 0, m_IQImageWidth, m_IQImageHeight);
        if (m_Image.isNull())
        {
            return;
        }
        m_PixmapItem->setPixmap(QPixmap::fromImage(m_Image));
        if (m_isFirstTime)
        {
            setScene(m_Scene);
            m_isFirstTime = false;
        }
    }
    else if (m_ImageModel == IQ1)
    {
        m_Scene->setSceneRect(0, 0, m_Image.width(), m_Image.height());
        m_PixmapItem->setPixmap(QPixmap::fromImage(m_Image));
    }
    this->show();
}

void CameraViewer::SetCenter(const QPointF &centerPoint)
{
    /* Get the rectangle of the visible area in scene coords */
    QRectF visibleArea = mapToScene(rect()).boundingRect();

    /* Get the scene area */
    QRectF sceneBounds = sceneRect();

    double boundX = visibleArea.width() / 2.0;
    double boundY = visibleArea.height() / 2.0;
    double boundWidth = sceneBounds.width() - 2.0 * boundX;
    double boundHeight = sceneBounds.height() - 2.0 * boundY;

    /* The max boundary that the centerPoint can be to */
    QRectF bounds(boundX, boundY, boundWidth, boundHeight);

    if (bounds.contains(centerPoint))
    {
        /* We are within the bounds */
        m_CurrentCenterPoint = centerPoint;
    }
    else
    {
        /* We need to clamp or use the center of the screen */
        if (visibleArea.contains(sceneBounds))
        {
            /* Use the center of scene ie. we can see the whole scene */
            m_CurrentCenterPoint = sceneBounds.center();
        }
        else
        {
            m_CurrentCenterPoint = centerPoint;

            /* We need to clamp the center. The centerPoint is too large */
            if (centerPoint.x() > bounds.x() + bounds.width())
            {
                m_CurrentCenterPoint.setX(bounds.x() + bounds.width());
            }
            else if (centerPoint.x() < bounds.x())
            {
                m_CurrentCenterPoint.setX(bounds.x());
            }

            if (centerPoint.y() > bounds.y() + bounds.height())
            {
                m_CurrentCenterPoint.setY(bounds.y() + bounds.height());
            }
            else if (centerPoint.y() < bounds.y())
            {
                m_CurrentCenterPoint.setY(bounds.y());
            }
        }
    }

    /* Update the scrollbars */
    centerOn(m_CurrentCenterPoint);
}

/* Handles when the mouse button is pressed */
void CameraViewer::mousePressEvent(QMouseEvent *event)
{

    Qt::MouseButton mouseBtn = event->button();

    if (Qt::LeftButton == mouseBtn)
    {
        m_bIsPressed = true;
        lineCutItemFinished = false;
        m_LastPanPoint = event->pos();

        if (/*!lineItemVisible && */ (horizontalScrollBar()->isVisible() || verticalScrollBar()->isVisible()))
            setCursor(Qt::ClosedHandCursor);
        else if ((!horizontalScrollBar()->isVisible() && !verticalScrollBar()->isVisible()))
            setCursor(Qt::ArrowCursor);
        if (lineItemVisible)
        {
            QPointF scenePos = mapToScene(event->pos());
            m_LineCutItem->setLine(scenePos.x(), scenePos.y(), scenePos.x(), scenePos.y());
        }
        emit showProFileWindow(lineItemVisible);
    }

    if (Qt::RightButton == mouseBtn)
    {
    }
    else
    {
        /* For panning the view */
    }
    QGraphicsView::mousePressEvent(event);
}

/* Handles when the mouse button is released */
void CameraViewer::mouseReleaseEvent(QMouseEvent *event)
{
    m_bIsPressed = false;
    lineCutItemFinished = true;
    Qt::MouseButton mouseBtn = event->button();
    QPointF scenePos = mapToScene(event->pos());
    if (mouseBtn == Qt::LeftButton)
    {
        if (lineItemVisible){
            m_LineCutItem->setLine(m_LineCutItem->line().x1(), m_LineCutItem->line().y1(), scenePos.x(), scenePos.y());
            QtConcurrent::run(this, &CameraViewer::calPlotProfile);
        }
    }
    if (horizontalScrollBar()->isVisible() || verticalScrollBar()->isVisible())
        setCursor(Qt::CrossCursor);
    m_LastPanPoint = QPoint();
    QGraphicsView::mouseReleaseEvent(event);
}

/* Handles the mouse move event */
void CameraViewer::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
    QPointF scenePos = mapToScene(event->pos());

    if (m_bIsPressed)
    {
        m_LineCutItem->setLine(m_LineCutItem->line().x1(), m_LineCutItem->line().y1(), scenePos.x(), scenePos.y());

        if ((horizontalScrollBar()->isVisible() || verticalScrollBar()->isVisible()) && !lineItemVisible)
            setCursor(Qt::OpenHandCursor);
        else if ((!horizontalScrollBar()->isVisible() && !verticalScrollBar()->isVisible()))
            setCursor(Qt::ArrowCursor);

        if (!m_LastPanPoint.isNull())
        {
            QPointF point = mapToScene(event->pos());
            QGraphicsItem *item = scene()->itemAt(point, transform());
            if (item != NULL)
            {
                QGraphicsPixmapItem *imgPix = dynamic_cast<QGraphicsPixmapItem *>(item);
                if (imgPix != NULL)
                {
                    QPointF delta = mapToScene(event->pos()) - mapToScene(m_LastPanPoint);
                    m_LastPanPoint = event->pos();
                    translate(delta);
                }
            }
            else
            {
                m_LastPanPoint = event->pos();
            }
        }
    }
    else
    {
        QPointF point = mapToScene(event->pos());
        QPoint f = point.toPoint();
        int binnum;
        if (m_Image.height())
            // binnum = 6004 / m_Image.height();
            binnum = sqrt((9200 * 11264) / (m_Image.height() * m_Image.width()));
        else
            binnum = 4;
        if (0 < f.x() && f.x() < m_IQImageWidth && 0 < f.y() && f.y() < m_IQImageHeight)
        {
            if (!m_bGraylevelEnable)
            {
                m_bGraylevelEnable = true;
                m_GrayLevel->show();
            }
            if (m_ImageModel == MTF)
            {
                if (!m_isFirstTime)
                {
                    double lumiG = 0;
                    double lumiB = 0;
                    double lumiR = 0;

                    {
                        QMutexLocker local_lock(&m_lockLine);

                        if (f.y() < m_showImageClone.rows && f.y() >= 0 && f.x() >= 0 &&
                            f.x() < m_showImageClone.cols && m_showImageClone.data != NULL)
                        {
                            if (m_showImage.channels() == 3)
                            {
                                lumiB = m_showImageClone.at<cv::Vec3f>(f.y(), f.x())[0];
                                lumiG = m_showImageClone.at<cv::Vec3f>(f.y(), f.x())[1];
                                lumiR = m_showImageClone.at<cv::Vec3f>(f.y(), f.x())[2];
                            }
                            else if (m_showImageClone.channels() == 1)
                            {
                                lumiG = m_showImageClone.at<float>(f.y(), f.x());
                            }
                        }
                    }
                    
                    if (m_ImageFromat == RGB24)
                    {
                        std::vector<float> angleVec = getImageFOVAngle(f.x(), f.y(), binnum);

                        if (m_showFov)
                        {
                            m_GrayLevel->setText(QString("FOV X:%1,Y:%2\nPixel X:%3,Y:%4 \nGraylevel:R:%5, G:%6, B:%7")
                                                     .arg(QString::number(angleVec[0], 'f', 2))
                                                     .arg(QString::number(angleVec[1], 'f', 2))
                                                     .arg(QString::number(f.x()))
                                                     .arg(QString::number(f.y()))
                                                     .arg(QString::number(lumiR))
                                                     .arg(QString::number(lumiG))
                                                     .arg(QString::number(lumiB)));
                        }
                        else
                        {
                            m_GrayLevel->setText(QString("Pixel X:%1,Y:%2 \nGraylevel:R:%3, G:%4, B:%5")
                                                     .arg(QString::number(f.x()))
                                                     .arg(QString::number(f.y()))
                                                     .arg(QString::number(lumiR))
                                                     .arg(QString::number(lumiG))
                                                     .arg(QString::number(lumiB)));
                        }

                        m_GrayLevel->setGeometry(event->pos().x() + 15, event->pos().y() - 30, 300, 100);
                    }
                    else
                    {
                        std::vector<float> angleVec = getImageFOVAngle(f.x(), f.y(), binnum);
                        if (m_showFov)
                        {
                            if (m_showImage.channels() == 3)
                            {
                                m_GrayLevel->setText(QString("FOV X:%1,Y:%2\nPixel X:%3,Y:%4 \nGraylevel:R:%5, G:%6, B:%7")
                                    .arg(QString::number(angleVec[0], 'f', 2))
                                    .arg(QString::number(angleVec[1], 'f', 2))
                                    .arg(QString::number(f.x()))
                                    .arg(QString::number(f.y()))
                                    .arg(QString::number(lumiR))
                                    .arg(QString::number(lumiG))
                                    .arg(QString::number(lumiB)));
                            }
                            else
                            {
                                m_GrayLevel->setText(QString("FOV X:%1,Y:%2\nPixel X:%3,Y:%4 \nGraylevel:%5")
                                    .arg(QString::number(angleVec[0], 'f', 2))
                                    .arg(QString::number(angleVec[1], 'f', 2))
                                    .arg(QString::number(f.x()))
                                    .arg(QString::number(f.y()))
                                    .arg(QString::number(lumiG)));
                            }

                        }
                        else
                        {
                            m_GrayLevel->setText(QString("Pixel X:%1,Y:%2 \nGraylevel:%3")
                                                     .arg(QString::number(f.x()))
                                                     .arg(QString::number(f.y()))
                                                     .arg(QString::number(lumiG)));
                        }

                        m_GrayLevel->setGeometry(event->pos().x() + 15, event->pos().y() - 30, 150, 100);
                        // drawGrayLevelRegion(QPointF(point) + QPoint(20, -10),
                        // QString("%1").arg(QString::number(gray.red())));
                    }
                }
            }
            else if (m_ImageModel == IQ1)
            {
                QStringList mes = getImageYxy(f.x(), f.y());
                std::vector<float> angleVec = getImageFOVAngle(f.x(), f.y(), binnum);

                if (m_showFov)
                {
                    m_GrayLevel->setText(QString("FOV X:%1,Y:%2\nPixel X:%3,Y:%4 \nY:%5 \nx:%6 \ny:%7")
                                             .arg(QString::number(angleVec[0], 'f', 2))
                                             .arg(QString::number(angleVec[1], 'f', 2))
                                             .arg(QString::number(f.x()))
                                             .arg(QString::number(f.y()))
                                             .arg(mes.at(0))
                                             .arg(mes.at(1))
                                             .arg(mes.at(2)));
                }
                else
                {
                    m_GrayLevel->setText(QString("Pixel X:%1,Y:%2 \nY:%3 \nx:%4 \ny:%5")
                                             .arg(QString::number(f.x()))
                                             .arg(QString::number(f.y()))
                                             .arg(mes.at(0))
                                             .arg(mes.at(1))
                                             .arg(mes.at(2)));
                }
                m_GrayLevel->setGeometry(event->pos().x() + 15, event->pos().y() - 30, 150, 100);
            }
            m_GrayLevel->adjustSize();
        }
        else
        {
            if (m_bGraylevelEnable)
            {
                m_bGraylevelEnable = false;
                m_GrayLevel->hide();
            }
        }
    }
}

void CameraViewer::calPlotProfile()
{
    cv::Mat image;
    {
        QMutexLocker local_lock(&m_lockLine);
        image = m_showImage.clone();
    }

    QPoint start = m_LineCutItem->line().p1().toPoint();
    QPoint end = m_LineCutItem->line().p2().toPoint();
    if (image.empty())
    {
        return;
    }
    if (end.x() == start.x())
    {
        return;
    }
    if (start.x() > end.x())
    {
        QPoint temp = start;
        start = end;
        end = temp;
    }

    // check range
    start.setX(start.x() < 0 ? 0 : start.x());
    start.setY(start.y() < 0 ? 0 : start.y());
    start.setX(start.x() >= image.cols ? image.cols - 1 : start.x());
    start.setY(start.y() >= image.rows ? image.rows - 1 : start.y());

    end.setX(end.x() < 0 ? 0 : end.x());
    end.setY(end.y() < 0 ? 0 : end.y());
    end.setX(end.x() >= image.cols ? image.cols - 1 : end.x());
    end.setY(end.y() >= image.rows ? image.rows - 1 : end.y());

    QMutexLocker lock(&m_lockLineData);
    m_CutLineAxisYGrayDatas.clear();

    double k = double(end.y() - start.y()) / double(end.x() - start.x());
    int x = start.x();
    int count = end.x() - x;
    int step = count / 50;
    step = 1;
    int lastGray = -1;
    int nextGray = -1;
    while (x <= end.x())
    {
        int col = x;
        int y = round(start.y() + (x - start.x()) * k);
        int row = y;

        if (row < 0 || col < 0 || row > image.rows || col > image.cols)
        {
            return;
        }

        float fAxisx = abs(sqrt(x * x + y * y));
        //m_CutLineAxisXDatas.append(fAxisx);

        float gray = 0;

        if (image.type() == CV_16U)
        {
            ushort pixelValue = image.at<ushort>(row, col);
            gray = pixelValue /*/ 16.0*/;
        }
        else if (image.type() == CV_8U)
        {
            uchar pixelValue = image.at<uchar>(row, col);
            gray = pixelValue;
        }
        else if (image.type() == CV_32F)
        {
            gray = image.at<float>(row, col);
        }

        if (std::isnan(fAxisx) || std::isnan(gray) || std::isinf(fAxisx) || std::isinf(gray))
        {
            qDebug() << "Invalid data: fAxisx =" << fAxisx << "gray =" << gray;
        }
        if ((gray != lastGray) || (x == end.x()))
        {
            m_CutLineAxisYGrayDatas.append(QPointF(fAxisx, gray));
            lastGray = gray;
        }
        if (x == end.x())
        {
            x++;
        }
        else if (x + step <= end.x())
        {
            x += step;
        }
        else
        {
            x = end.x();
        }
    }

    emit sigUpdateCutLine(m_CutLineAxisYGrayDatas);
}

/*  Zoom the view in and out. */
void CameraViewer::wheelEvent(QWheelEvent *event)
{
    QPoint scrollAmount = event->angleDelta();
    scrollAmount.y() > 0 ? zoomIn() : zoomOut();
}

/**
 * Need to update the center so there is no jolt in the
 * interaction after resizing the widget.
 */
void CameraViewer::resizeEvent(QResizeEvent *event)
{
    /* Call the subclass resize so the scrollbars are updated correctly */
    if (m_bIsFitToWindow)
    {
        fitInView(scene()->itemsBoundingRect(), Qt::IgnoreAspectRatio);
    }

    QGraphicsView::resizeEvent(event);
}

QImage CameraViewer::cvMat2QImage(const cv::Mat &mat)
{
    if (mat.empty())
    {
        return QImage();
    }

    cv::Mat dst8;
    //if (mat.type() == CV_16U)
    //{
    //    cv::Mat tmp;
    //    dst8 = cv::Mat::zeros(mat.size(), CV_8U);

    //    double minv = 0.0, maxv = 0.0;
    //    double *minp = &minv;
    //    double *maxp = &maxv;
    //    cv::minMaxIdx(mat, minp, maxp);

    //    if (maxv > 4095.1)
    //    {
    //        //cv::Mat tmp = mat / 65535 * 255;
    //        //tmp.convertTo(dst8, CV_8UC1);
    //        mat.convertTo(dst8, CV_8UC1, 255.0 / 65535.0, 0);
    //    }
    //    else
    //    {
    //        //cv::Mat tmp = mat / 4095.0 * 255;
    //        //tmp.convertTo(dst8, CV_8UC1);
    //        mat.convertTo(dst8, CV_8UC1, 255.0 / 4095, 0);
    //    }
    //}
    //else if (mat.type() == CV_16UC3)
    //{
    //    cv::Mat tmp;
    //    dst8 = cv::Mat::zeros(mat.size(), CV_8UC3);

    //    double minv = 0.0, maxv = 0.0;
    //    double *minp = &minv;
    //    double *maxp = &maxv;
    //    cv::minMaxIdx(mat, minp, maxp);

    //    if (maxv > 4095.1)
    //    {
    //        //cv::Mat tmp = mat / 65535 * 255;
    //        //tmp.convertTo(dst8, CV_8UC3);
    //        mat.convertTo(dst8, CV_8UC3, 255.0 / 65535.0, 0);

    //    }
    //    else
    //    {
    //        //cv::Mat tmp = mat / 4095.0 * 255;
    //        //tmp.convertTo(dst8, CV_8UC3);
    //        mat.convertTo(dst8, CV_8UC3, 255.0 / 4095, 0);
    //    }
    //}

    if(mat.type() == CV_16U || mat.type() == CV_16UC3){
        double minv, maxv;
        cv::minMaxIdx(mat, &minv, &maxv);

        double scale = (maxv > 4095.1) ? (255.0 / 65535.0) : (255.0 / 4095.0);
        mat.convertTo(dst8, mat.type() == CV_16UC3 ? CV_8UC3 : CV_8U, scale);
    }
    else
    {
        dst8 = mat.clone();
    }

    QImage tmp = cvMat2QImageType(dst8);
    if (!tmp.isNull())
    {
        return tmp;
    }

    QImage img;
    int chana = dst8.channels();

    if (3 == chana)
    {
        cv::cvtColor(dst8, dst8, cv::COLOR_BGR2RGB);
        img = QImage(static_cast<uchar *>(dst8.data), dst8.cols, dst8.rows, QImage::Format_RGB888);
    }
    else if (4 == chana)
    {
        // argb
        img = QImage(static_cast<uchar *>(dst8.data), dst8.cols, dst8.rows, QImage::Format_ARGB32);
    }
    else
    {
        img = QImage(dst8.cols, dst8.rows, QImage::Format_Grayscale8);
        uchar *matdata = dst8.data;
        for (int row = 0; row < dst8.rows; ++row)
        {
            uchar *rowdata = img.scanLine(row);
            memcpy(rowdata, matdata, dst8.cols);
            matdata += dst8.cols;
        }
    }

    return img;
}

QImage CameraViewer::cvMat2QImageType_old(const cv::Mat &mat)
{
    if (mat.type() == CV_8UC1)
    {
        cv::Mat bgr;
        cv::cvtColor(mat, bgr, cv::COLOR_GRAY2RGB);
        cv::Mat mask = (mat == 255);
        bgr.setTo(cv::Scalar(128, 128, 255), mask);

        // Copy input Mat
        const uchar *pSrc = (const uchar *)bgr.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, bgr.cols, bgr.rows, bgr.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if (mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar *)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if (mat.type() == CV_8UC4)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar *)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else if (mat.type() == CV_32FC1)
    {
        Mat normalize_mat;
        normalize(mat, normalize_mat, 0, 255, cv::NORM_MINMAX, -1);
        normalize_mat.convertTo(normalize_mat, CV_8U);
        const uchar *pSrc = (const uchar *)normalize_mat.data;
        QImage image(pSrc, normalize_mat.cols, normalize_mat.rows, normalize_mat.step, QImage::Format_Grayscale8);
        return image.copy();
    }
    else
    {
        return QImage();
    }
}

QImage CameraViewer::cvMat2QImageType_jiaxing(const cv::Mat& mat)
{
    if (mat.empty()) {
        return QImage();
    }

    if (mat.type() == CV_8UC1)
    {
        cv::Mat bgr;
        cv::cvtColor(mat, bgr, cv::COLOR_GRAY2RGB);

        cv::Mat mask = (mat == 255);
        bgr.setTo(cv::Scalar(128, 128, 255), mask);
        return QImage(bgr.data, bgr.cols, bgr.rows, bgr.step, QImage::Format_RGB888).rgbSwapped().copy();
    }

    if (mat.type() == CV_8UC3)
    {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).rgbSwapped().copy();
    }

    if (mat.type() == CV_8UC4)
    {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32).copy();
    }

    if (mat.type() == CV_32FC1)
    {
        cv::Mat normMat, u8Mat;
        cv::normalize(mat, normMat, 0, 255, cv::NORM_MINMAX);
        normMat.convertTo(u8Mat, CV_8U);

        return QImage(u8Mat.data, u8Mat.cols, u8Mat.rows, u8Mat.step, QImage::Format_Grayscale8).copy();
    }

    return QImage();
}

QImage CameraViewer::cvMat2QImageType(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if (mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for (int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar* pSrc = mat.data;
        for (int row = 0; row < mat.rows; row++)
        {
            uchar* pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image.copy();
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if (mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar* pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if (mat.type() == CV_8UC4)
    {
        // Copy input Mat
        const uchar* pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else if (mat.type() == CV_32FC1) {
        Mat normalize_mat;
        normalize(mat, normalize_mat, 0, 255, cv::NORM_MINMAX, -1);
        normalize_mat.convertTo(normalize_mat, CV_8U);
        const uchar* pSrc = (const uchar*)normalize_mat.data;
        QImage image(pSrc, normalize_mat.cols, normalize_mat.rows, normalize_mat.step, QImage::Format_Grayscale8);
        return image.copy();
    }
    else
    {
        return QImage();
    }
}

float CameraViewer::calculateFOVRadius(float angle)
{
    int binnum = sqrt(9200 * 11264 / (m_Image.height() * m_Image.width()));
    float lendiag = tan(angle / 180.0 * CV_PI) * m_focalLength / m_pixel;
    return lendiag / binnum;
}

float CameraViewer::calculateFOV(float length)
{
    // int binnum = 6004 / m_Image.height();
    int binnum = sqrt((9200 * 11264) / (m_Image.height() * m_Image.width()));

    return atan((length * binnum * m_pixel) / m_focalLength) / CV_PI * 180.0;
}

float CameraViewer::calculateFOVRadiusDisCorrect(float angle)
{
    float A5 = 8.31546069E-08;
    float A4 = 7.08240775E-06;
    float A3 = -5.19862068E-04;
    float A2 = 2.03026865E-04;
    float A1 = 9.99716870E-01;
    float A0 = 8.73331432E-05;
    // int binnum = 6004 / m_Image.height();
    int binnum = sqrt((9200 * 11264) / (m_Image.height() * m_Image.width()));
    float h = tan(angle / 180.0 * CV_PI) * m_focalLength;
    float h1 = A5 * pow(h, 5) + A4 * pow(h, 4) + A3 * pow(h, 3) + A2 * pow(h, 2) + A1 * pow(h, 1) + A0;
    float lendiag = h1 / m_pixel;
    // tan(angle / 180.0 * CV_PI) * m_focalLength / m_pixel;
    return lendiag / binnum;
}
std::vector<float> CameraViewer::calculateFOVDisCorrect(float xPixellen, float yPixellen)
{
    if (fabs(xPixellen) < 1e-6)
        xPixellen = 1e-6;
    if (fabs(yPixellen) < 1e-6)
        yPixellen = 1e-6;
    // int binnum = 6004 / m_Image.height();
    int binnum = sqrt((9200 * 11264) / (m_Image.height() * m_Image.width()));
    float A5 = 2.0288625E-07;
    float A4 = 4.6523659E-08;
    float A3 = 4.6232961E-04;
    float A2 = 5.0126933E-06;
    float A1 = 9.9999147E-01;
    float A0 = 2.9715198E-06;
    std::vector<float> angleVec;
    float hx = m_pixel * xPixellen * binnum;
    float hy = m_pixel * yPixellen * binnum;
    float h = m_pixel * binnum * sqrt(pow(xPixellen, 2) + pow(yPixellen, 2));
    float h1 = A5 * pow(h, 5) + A4 * pow(h, 4) + A3 * pow(h, 3) + A2 * pow(h, 2) + A1 * pow(h, 1) + A0;
    float hx1 = h1 / h * hx;
    float hy1 = h1 / h * hy;
    float angle = atan2(h1, m_focalLength) * 180.0 / CV_PI;
    float anglex = atan2(hx1, m_focalLength) * 180.0 / CV_PI;
    float angley = atan2(hy1, m_focalLength) * 180.0 / CV_PI;
    angleVec.push_back(anglex);
    angleVec.push_back(angley);
    angleVec.push_back(angle);
    return angleVec;
}

std::vector<float> CameraViewer::getImageFOVAngle(int xPixel, int yPixel, int binNum)
{
    std::vector<float> angleVec;
    double anglex = 0;
    double angley = 0;
    if (m_showImage.depth() == CV_16U || m_showImage.depth() == CV_8U)
    {

        std::vector<float> angleVec =
            calculateFOVDisCorrect((xPixel - fovCenter(binNum).x), (yPixel - fovCenter(binNum).y));
        anglex = angleVec[0];
        angley = angleVec[1];
    }
    else
    {
        anglex = calculateFOV((xPixel - fovCenter(binNum).x));
        angley = calculateFOV((yPixel - fovCenter(binNum).y));
    }
    angleVec.push_back(anglex);
    angleVec.push_back(angley);
    return angleVec;
}

void CameraViewer::loadAboutFOVConfig(const char *path)
{
    filepath = path;
    std::ifstream jsonFile(filepath);
    if (jsonFile.is_open())
    {
        std::string contents =
            std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
        jsonFile.close();
        settingJsonObj = Json::parse(contents);

        Json &system = settingJsonObj["system"];
        if (!system.is_null())
        {
            m_focalLength = system["FocalLength"].get<float>();
            m_pixel = system["pixel_size"].get<float>();
            Json center = system["W_opticalcenter"];
            QList<int> list;
            for (auto it : center.items())
            {
                list.append(it.value().get<int>());
            }

            m_fovCenterMap[1] = cv::Point2f(list[0], list[1]);
            m_fovCenterMap[2] = cv::Point2f(list[0] / 2.0, list[1] / 2.0);
            m_fovCenterMap[4] = cv::Point2f(list[0] / 4.0, list[1] / 4.0);
        }
    }
}

void CameraViewer::paintCenter()
{
    QPen pen;
    pen.setColor(Qt::red);
    int binnum;
    if (m_Image.height())
        // binnum = 6004 / m_Image.height();
        binnum = sqrt((9200 * 11264) / (m_Image.height() * m_Image.width()));
    else
        binnum = 4;
    pen.setWidth(5 * 4 / binnum);
    pen.setColor(Qt::red);
    QRectF rect1(fovCenter(binnum).x - 2.5 * 4 / binnum, fovCenter(binnum).y - 2.5 * 4 / binnum, 5 * 4 / binnum,
                 5 * 4 / binnum);
    QGraphicsEllipseItem *center = m_Scene->addEllipse(rect1, pen, QBrush(Qt::SolidPattern));
    m_circles.append(center);
}

cv::Point2f CameraViewer::fovCenter(int binNum)
{
    if (m_fovCenterMap.contains(binNum))
    {
        return m_fovCenterMap[binNum];
    }
    return cv::Point2f(0, 0);
}

void CameraViewer::setFOVCenter(QMap<int, cv::Point2f> centerMap)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    m_fovCenterMap = centerMap;
}

QPointF CameraViewer::GetCenter()
{
    return m_CurrentCenterPoint;
}
void CameraViewer::zoom(float scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.05 || factor > 25)
        return;

    scale(scaleFactor, scaleFactor);
    m_scale *= scaleFactor;
}

void CameraViewer::zoomIn(void)
{
    zoom(1 + m_zoomDelta);
}

void CameraViewer::zoomOut(void)
{
    zoom(1 - m_zoomDelta);
}

void CameraViewer::updateCenter(QPointF pointBeforeScale, QPointF screenCenter)
{
    /* Get the position after scaling, in scene coords */
    QPointF pointAfterScale(mapToScene(pos()));

    /* Get the offset of how the screen moved */
    QPointF offset = pointBeforeScale - pointAfterScale;

    /* Adjust to the new center for correct zooming */
    QPointF newCenter = screenCenter + offset;
    SetCenter(newCenter);
}

void CameraViewer::Enqueue(Mat &sour)
{
    if(sour.empty()){
        return;
    }

    QImage img = cvMat2QImage(sour);
    if (!img.isNull())
    {
        {
            QMutexLocker local_lock(&m_Lock);

            if (m_Data.size() == NUM_FRAMES)
            {
                m_Data.pop_front();
            }
            m_Data.push_back(img);
        }

        QMutexLocker local_lock(&m_lockLine);
        //m_Image = img;
        m_showImage = sour.clone();
        m_showImage.convertTo(m_showImageClone, CV_32F);
        emit imageReadySignal();
    }
}

void CameraViewer::SetImageList(QList<QImage> imageList)
{
    QMutexLocker local_lock(&m_Lock);
    m_Data = imageList;

    emit imageReadySignal();
}

void CameraViewer::EnableCross()
{
    UnenableCross();
    int height = m_Scene->height();
    int width = m_Scene->width();
    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(1);
    m_Xaxis = new MyLineItem(QLineF(QPoint(0, height / 2), QPoint(width, height / 2)));
    m_Xaxis->setPen(pen);
    m_Yaxis = new MyLineItem(QLineF(QPoint(width / 2, 0), QPoint(width / 2, height)));
    m_Yaxis->setPen(pen);
    m_Scene->addItem(m_Xaxis);
    m_Scene->addItem(m_Yaxis);

    if (1) {
        qreal radius = 10;
        QPointF center(width/2, height/2);                           
        m_circle = new QGraphicsEllipseItem(center.x() - radius, 
            center.y() - radius, 
            2 * radius,          
            2 * radius          
        );

        //QPen pen(Qt::red, 1);
        m_circle->setPen(pen);
        m_Scene->addItem(m_circle);
    }
}

void CameraViewer::EnableFOVCross()
{
    UnenableFOVCross();
    int binnum = 1;
    if(!m_Image.isNull()){
        binnum = sqrt((9200 * 11264) / (m_Image.height() * m_Image.width()));
    }

    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(2 * 2 / binnum);
    Qt::PenStyle style = Qt::DashLine;
    pen.setStyle(style);
    int height = m_Scene->height();
    int width = m_Scene->width();
    m_XaxisAngle = new QGraphicsLineItem(QLineF(QPoint(0, fovCenter(binnum).y), QPoint(width, fovCenter(binnum).y)));
    m_XaxisAngle->setPen(pen);
    m_YaxisAngle = new QGraphicsLineItem(QLineF(QPoint(fovCenter(binnum).x, 0), QPoint(fovCenter(binnum).x, height)));
    m_YaxisAngle->setPen(pen);
    m_Scene->addItem(m_XaxisAngle);
    m_Scene->addItem(m_YaxisAngle);
}

void CameraViewer::EnableFOV()
{
    UnenableFOV();
    EnableFOVCross();
    if (m_Image.height() == 0)
    {
        paintCenter();
        return;
    }

    int i = 0;
    for (float degree = m_initAngle; degree <= 30.0; degree += m_addRadius)
    {
        // float length = calculateFOVRadius(degree);
        float length;
        if (m_showImage.depth() == CV_16U || m_showImage.depth() == CV_8U)
            length = calculateFOVRadiusDisCorrect(degree);
        else if (m_showImage.depth() == CV_32F)
            length = calculateFOVRadius(degree);
        showFOV(length, i++);
    }
}

void CameraViewer::showFOV(float radius, int i)
{
    // int binnum = 6004 / m_Image.height();
    int binnum = sqrt((9200 * 11264) / (m_Image.height() * m_Image.width()));

    cv::Point2i outPoint = cv::Point2i(fovCenter(binnum).x - radius, fovCenter(binnum).y - radius);
    QRectF rect(outPoint.x, outPoint.y, radius * 2, radius * 2);
    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(4 * 4 / binnum);
    Qt::PenStyle style = Qt::DashLine;
    pen.setStyle(style);
    QGraphicsEllipseItem *circle = m_Scene->addEllipse(rect, pen);

    QGraphicsTextItem *pItem = new QGraphicsTextItem();
    pItem->setPlainText(QString("%1").arg(QString::number((m_initAngle + i * m_addRadius), 'f', 0)));
    QFont font = pItem->font();
    // font.setPixelSize(67 * 4 / binnum);
    font.setPixelSize(57 * 4 / binnum);
    pItem->setFont(font);
    pItem->setDefaultTextColor(Qt::red);
    float x = fovCenter(binnum).x + radius / 1.0 + 100 / binnum;
    float y = fovCenter(binnum).y - 160 / binnum; // - radius / sqrt(2);
    pItem->setPos(x - 50, y - 50);
    // pItem->setPos(x , y );
    m_Scene->addItem(pItem);
    //paintCenter();
    m_circles.append(circle);
    m_items.insert(circle, pItem);
}

void CameraViewer::UnenableCross()
{
    if (m_Xaxis)
    {
        m_Scene->removeItem(m_Xaxis);
        delete m_Xaxis;
        m_Xaxis = nullptr;
    }

    if (m_Yaxis)
    {
        m_Scene->removeItem(m_Yaxis);
        delete m_Yaxis;
        m_Yaxis = nullptr;
    }

    if (m_circle) {
        m_Scene->removeItem(m_circle);
        delete m_circle;
        m_circle = nullptr;
    }
}

void CameraViewer::UnenableFOVCross()
{
    if (m_XaxisAngle)
    {
        m_Scene->removeItem(m_XaxisAngle);
        delete m_XaxisAngle;
        m_XaxisAngle = nullptr;
    }

    if (m_YaxisAngle)
    {
        m_Scene->removeItem(m_YaxisAngle);
        delete m_YaxisAngle;
        m_YaxisAngle = nullptr;
    }
}

void CameraViewer::UnenableFOV()
{
    UnenableFOVCross();
    if (!m_circles.isEmpty())
    {
        for (auto circle : m_circles)
        {
            m_Scene->removeItem(circle);
            m_Scene->removeItem(m_items.value(circle));
            delete m_items.value(circle);
            m_items.remove(circle);
            delete circle;
            circle = nullptr;
        }
        m_circles.clear();
    }
}

void CameraViewer::AddROI(int index, QPoint center, QPoint size)
{
    if (m_ROIMap.contains(index))
        return;
    QGraphicsItemGroup *roi = drawROIRegion(index, center, size.x(), size.y());
    m_Scene->addItem(roi);
    m_ROIMap.insert(index, roi);
}

void CameraViewer::DeleteROIByIndex(int index)
{
    if (m_ROIMap.contains(index))
    {
        QGraphicsItemGroup *roi = m_ROIMap.value(index);
        m_Scene->removeItem(roi);
        m_ROIMap.remove(index);
        delete roi;
    }
}

void CameraViewer::MoveROIByIndex(int index, QPoint step)
{
    if (m_ROIMap.contains(index))
    {
        QGraphicsItemGroup *roi = m_ROIMap.value(index);
        roi->setPos(roi->pos() + step);
    }
}

void CameraViewer::ChangeROISizeByIndex(int index, QPoint center, QPoint size)
{
    if (m_ROIMap.contains(index))
    {
        DeleteROIByIndex(index);
        AddROI(index, center, size);
    }
}

void CameraViewer::HideROI()
{
    QMapIterator<int, QGraphicsItemGroup *> iterator(m_ROIMap);
    while (iterator.hasNext())
    {

        iterator.next();
        m_Scene->removeItem(iterator.value());
    }
}

void CameraViewer::ShowROI()
{
    QMapIterator<int, QGraphicsItemGroup *> iterator(m_ROIMap);
    while (iterator.hasNext())
    {

        iterator.next();
        m_Scene->addItem(iterator.value());
    }
}

void CameraViewer::ClearALLROI()
{
    QMapIterator<int, QGraphicsItemGroup *> iterator(m_ROIMap);
    while (iterator.hasNext())
    {

        iterator.next();
        m_Scene->removeItem(iterator.value());
    }
    m_ROIMap.clear();
}

void CameraViewer::SetImageModel(ImageModel model)
{
    m_Yxy.clear();
    m_ImageModel = model;
}

void CameraViewer::SetXYZImages(Mat &X, Mat &Y, Mat &Z)
{
    m_XYZ.clear();
    m_Yxy.clear();
    m_IQImageWidth = X.cols;
    m_IQImageHeight = X.rows;
    Mat XShow, YShow, ZShow;
    X.convertTo(XShow, CV_32FC1);
    Y.convertTo(YShow, CV_32FC1);
    Z.convertTo(ZShow, CV_32FC1);
    m_XYZ.append(X);
    m_XYZ.append(YShow);
    m_XYZ.append(ZShow);
    Mat XFloat, YFloat, ZFloat;
    X.convertTo(XFloat, CV_32FC1);
    Y.convertTo(YFloat, CV_32FC1);
    Z.convertTo(ZFloat, CV_32FC1);
    Mat sum = XFloat + YFloat + ZFloat;
    Mat x = XFloat / sum;
    Mat y = YFloat / sum;
    m_Yxy.append(Y.clone());
    m_Yxy.append(x.clone());
    m_Yxy.append(y.clone());
}

void CameraViewer::SetXYZ(Mat &X, Mat &Y, Mat &Z)
{
    m_XYZ.clear();
    Mat XShow, YShow, ZShow;
    X.convertTo(XShow, CV_32FC1);
    Y.convertTo(YShow, CV_32FC1);
    Z.convertTo(ZShow, CV_32FC1);
    m_XYZ.append(XShow);
    m_XYZ.append(YShow);
    m_XYZ.append(ZShow);
}

void CameraViewer::SetYxy(Mat &Y, Mat &x, Mat &y)
{
    m_IQImageWidth = Y.cols;
    m_IQImageHeight = Y.rows;

    Mat YFloat, xFloat, yFloat;
    Y.convertTo(YFloat, CV_32FC1);
    x.convertTo(xFloat, CV_32FC1);
    y.convertTo(yFloat, CV_32FC1);

    m_Yxy.clear();
    m_Yxy.append(YFloat);
    m_Yxy.append(xFloat);
    m_Yxy.append(yFloat);
}

void CameraViewer::SelectImageShow(ImageType type)
{
    if (!m_XYZ.isEmpty())
    {
        Mat sour = m_XYZ.at(type);
        m_Image = QImage(sour.cols, sour.rows, QImage::Format_RGB32);
        bool res = ImageTransform(sour, m_Image);
        if (res)
        {
            emit imageReadySignal();
        }
    }
}

QGraphicsPixmapItem *CameraViewer::getPixmapItem()
{
    return m_PixmapItem;
}

QGraphicsScene *CameraViewer::GetScene()
{
    return m_Scene;
}

QImage CameraViewer::GetQImage()
{
    QMutexLocker local_lock(&m_Lock);
    QImage res;
    if (!m_Data.isEmpty())
    {
        try
        {
            res = m_Data.front().copy();
            m_Data.pop_front();
        }
        catch (...)
        {
        }
    }
    return res;
}

void CameraViewer::ClearImageQueue()
{
    QMutexLocker local_lock(&m_Lock);
    m_Data.clear();
}

void CameraViewer::drawGrayLevelRegion(QPointF origin, QString text)
{
    if (m_GrayLeveText)
    {
        m_GrayLeveText->setPos(origin);
        m_GrayLeveText->setText(text);
    }
    else
    {
        QPen pen;
        pen.setColor(Qt::yellow);
        pen.setWidth(3);
        m_GrayLeveText = new QGraphicsSimpleTextItem(text);
        m_GrayLeveText->setPen(pen);
        m_GrayLeveText->setFont(QFont("Arial", 20));
        m_GrayLeveText->setPos(origin);
        m_Scene->addItem(m_GrayLeveText);
    }
}

QGraphicsItemGroup *CameraViewer::drawROIRegion(int index, QPoint center, int width, int height)
{
    QGraphicsItemGroup *groupItem = new QGraphicsItemGroup();
    groupItem->setFlag(QGraphicsItem::ItemIsMovable);
    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor(255, 0, 0));
    QGraphicsRectItem *rectItem = new QGraphicsRectItem();
    rectItem->setRect(QRect(0, 0, int(width), int(height)));

    rectItem->setPen(pen);
    rectItem->setFlag(QGraphicsItem::ItemIsMovable);
    rectItem->setPos(0, 0);
    QPen txtpen;
    txtpen.setColor(QColor(255, 127, 80));

    int fontSize = 50 * height / 200;
    if (fontSize > 50)
    {
        fontSize = 50;
    }
    if (fontSize < 5)
    {
        fontSize = 5;
    }
    QGraphicsSimpleTextItem *txtitem = new QGraphicsSimpleTextItem(QString("").number(index).append("#"));
    txtitem->setPen(txtpen);
    txtitem->setFont(QFont("Arial", fontSize));
    txtitem->setFlag(QGraphicsItem::ItemIsMovable);
    txtitem->setPos(rectItem->rect().width() + 5, 0);

    groupItem->addToGroup(rectItem);
    groupItem->addToGroup(txtitem);

    QPointF point = {qreal(center.x() - (width / 2)), qreal(center.y() - (height / 2))};
    groupItem->setPos(point);
    return groupItem;
}

QStringList CameraViewer::getImageYxy(int x, int y)
{
    QStringList list;
    if (m_Yxy.size() >= 3)
    {
        if (0 < x && x < m_IQImageWidth && 0 < y && y < m_IQImageHeight)
        {
            list.append(QString::number(m_Yxy[0].at<float>(y, x)));
            list.append(QString::number(m_Yxy[1].at<float>(y, x), 'g', 5));
            list.append(QString::number(m_Yxy[2].at<float>(y, x), 'g', 5));
        }
        else
        {
            list.append("0");
            list.append("0");
            list.append("0");
        }
    }
    else
    {
        list.append("0");
        list.append("0");
        list.append("0");
    }
    return list;
}

void CameraViewer::clearFiducialPoints()
{
    QList<QGraphicsItem *> itemlst = m_Scene->items();
    for each (QGraphicsItem *item in itemlst)
    {
        FiducialPoint *fid = dynamic_cast<FiducialPoint *>(item);
        if (fid != NULL)
        {
            fid->disconnect();
            delete fid;
        }
    }
    fidusVec.clear();
}

void CameraViewer::addFiducialPoint(int x, int y)
{
    FiducialPoint *fiducial = new FiducialPoint(NULL, QPointF(x, y));
    fiducial->setZValue(3);
    int size = fidusVec.size();
    fiducial->setId(size);
    fidusVec.push_back(fiducial);
    m_Scene->addItem(fiducial);
    m_Scene->update();
}

QMap<int, cv::Rect> CameraViewer::getRoiMap()
{
    QMap<int, cv::Rect> map;

    QMap<int, QGraphicsItemGroup *>::iterator iter;
    for (iter = m_ROIMap.begin(); iter != m_ROIMap.end(); ++iter)
    {
        QGraphicsItemGroup *item = iter.value();
        QRectF rectF = item->boundingRect();

        cv::Rect rect = cv::Rect(rectF.x(), rectF.y(), rectF.width(), rectF.height());
        map.insert(iter.key(), rect);
    }
    return map;
}

cv::Mat CameraViewer::currentImage()
{
    QMutexLocker local_lock(&m_lockLine);
    return m_showImage;
}

void CameraViewer::setCurrentImage(cv::Mat image)
{
    QMutexLocker local_lock(&m_lockLine);
    m_showImage = image.clone();
}

void CameraViewer::clearImage()
{
    QMutexLocker local_lock(&m_lockLine);

    m_Scene->setSceneRect(0, 0, 0, 0);
    m_PixmapItem->setPixmap(QPixmap());

    m_showImage = cv::Mat();
}

void CameraViewer::setShowFov(bool display)
{
    m_showFov = display;
}

cv::Mat CameraViewer::getShowImage()
{
    QMutexLocker local_lock(&m_lockLine);
    return m_showImage;
}

bool CameraViewer::ImageTransform(Mat &src, QImage &dst)
{
    VmbError_t Result;
    if (src.empty())
    {
        return false;
    }
    VmbImage source_image;
    VmbImage destination_image;
    source_image.Size = sizeof(source_image);
    destination_image.Size = sizeof(destination_image);
    std::string pixel_in;
    switch (src.depth())
    {
    case CV_8U:
        pixel_in = "Mono8";
        m_ImageFromat = Mono8;
        break;
    case CV_16U:
        pixel_in = "Mono16";
        m_ImageFromat = Mono16;
        break;
    default:
        pixel_in = "BGR24";
        m_ImageFromat = RGB24;
        ;
        break;
    }
    Result = VmbSetImageInfoFromString(pixel_in.c_str(), static_cast<VmbUint32_t>(pixel_in.size()),
                                       static_cast<VmbUint32_t>(src.cols), static_cast<VmbUint32_t>(src.rows),
                                       &source_image);
    if (VmbErrorSuccess != Result)
    {
        return false;
    }

    unsigned int dst_bits_per_pixel = dst.depth();
    std::string pixel_out;
    QImage::Format format = dst.format();
    switch (format)
    {
    default:
        return false;
    case QImage::Format_Mono:
        pixel_out = "Mono8";
        break;
    case QImage::Format_BGR888:
        pixel_out = "BGR24";
        break;
    case QImage::Format_RGB32:
        pixel_out = "BGRA32";
        break;
    }
    Result = VmbSetImageInfoFromString(pixel_out.c_str(), static_cast<VmbUint32_t>(pixel_out.size()),
                                       static_cast<VmbUint32_t>(dst.width()), static_cast<VmbUint32_t>(dst.height()),
                                       &destination_image);
    source_image.Data = src.data;
    destination_image.Data = dst.bits();
    Result = VmbImageTransform(&source_image, &destination_image, NULL, 0);
    if (VmbErrorSuccess == Result)
    {
        return true;
    }
    else
    {
        return false;
    }
}
void CameraViewer::translate(QPointF delta)
{
    delta *= m_scale;

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    QPoint newCenter(VIEW_WIDTH / 2 - delta.x(), VIEW_HEIGHT / 2 - delta.y());
    centerOn(mapToScene(newCenter));

    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}