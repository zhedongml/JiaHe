#include "CameraViewer.h"
#include <QCoreApplication>
#include <QScrollBar>
#include <iostream>
#define VIEW_CENTER viewport()->rect().center()
#define VIEW_WIDTH  viewport()->rect().width()
#define VIEW_HEIGHT viewport()->rect().height()
CameraViewer::CameraViewer(QWidget *parent)
    : QGraphicsView(parent), 
    m_translateButton(Qt::LeftButton),
    m_scale(1.0),
    m_zoomDelta(0.1),
    m_translateSpeed(1.0),
    m_bMouseTranslate(false)
{
    //setMouseTracking(true);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setAlignment(Qt::AlignCenter | Qt::AlignCenter);
    m_Scene = new QGraphicsScene();
    m_PixmapItem = new QGraphicsPixmapItem();
    //m_CaluDataItem = new QGraphicsItemGroup();
    
    m_Scene->addItem(m_PixmapItem);
    setScene(m_Scene);
    m_MaxSize = 3;
    m_bHasJustStarted = true;
    m_bStop = false;
    m_bIsPressed = false;
    m_WidthScale = 0.0;
    m_HeightScale = 0.0;
    m_Left = { 0,0 };
    m_Right = { 0,0 };
    m_DUTData.left.x = 0;
    m_DUTData.right.x = 0;

    pen1.setColor(Qt::red);
    pen1.setWidth(2);

    pen2.setColor(Qt::blue);
    pen2.setWidth(2);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this,
            SLOT(graphViewcustomContextMenuRequested(const QPoint &)));

    setMouseTracking(true);

    {
        m_GrayLevel= new QLabel(this);
        m_GrayLevel->setDisabled(true);
        QFont ft;
        ft.setPointSize(10);
        m_GrayLevel->setFont(ft);
        m_GrayLevel->setStyleSheet("color:red;background:transparent;");
        m_GrayLevel->setParent(this);
        m_GrayLevel->setGeometry(2, 0, 500, 40);
        m_GrayLevel->show();
    }
}

CameraViewer::~CameraViewer()
{
    delete m_Scene;

    ClearImageQueue();
}

void CameraViewer::SetMouseTracking()
{
    this->setMouseTracking(true);
}

void CameraViewer::mouseMoveEvent(QMouseEvent* event)
{
    //QGraphicsView::mouseMoveEvent(event);
    if (!m_bIsPressed) 
    {
        QPointF sceneMousePos = this->mapToScene(event->pos());
        QPointF itemMousePos = m_PixmapItem->mapFromScene(sceneMousePos);
        itemMousePos = itemMousePos.toPoint();

        if (!m_bStop && !m_Image.isNull()) 
        {
            double max = m_WidthScale <= m_HeightScale ? m_HeightScale : m_WidthScale;
            int x = int(itemMousePos.x());
            int y = int(itemMousePos.y());
            QPointF p(x, y);

            QColor gray= m_Image.pixelColor(x,y);
            int c = (gray.red() * 11 + gray.green() * 16 + gray.blue() * 5) / 32;
            
            QString mRgbStr = QString::number(c);

            emit SendMousePosSignal(p, mRgbStr);
        }
        else
        {
            emit SendMousePosSignal(itemMousePos, "0");
        }
        
    }

    {
        if (!m_Image.isNull())
        {
            QPointF point = mapToScene(event->pos());
            QPoint f = point.toPoint();

            if (0 < f.x() && f.x() < m_Image.width() && 0 < f.y() && f.y() < m_Image.height())
            {
                if (!m_isFirstTime)
                {
                    QColor gray = m_Image.pixelColor(f.x(), f.y());
                    m_GrayLevel->setText(
                        QString("Pixel X:%1,Y:%2").arg(QString::number(f.x())).arg(QString::number(f.y())));
                    m_GrayLevel->setGeometry(event->pos().x() + 15, event->pos().y() - 30, 300, 100);
                }
                m_GrayLevel->show();
            }
            else
            {
                m_GrayLevel->hide();
            }
            m_GrayLevel->adjustSize();
        }
    }

    if (m_bMouseTranslate) {
        QPointF mouseDelta = mapToScene(event->pos()) - mapToScene(m_lastMousePos);
        translate(mouseDelta);
    }

    m_lastMousePos = event->pos();

    QGraphicsView::mouseMoveEvent(event);
}

void CameraViewer::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == m_translateButton) {
        m_bIsPressed = true;
        QPointF point = mapToScene(event->pos());
        QGraphicsScene* sceneItem = scene();
        if (sceneItem == nullptr) return;
        QGraphicsItem* item = scene()->itemAt(point, transform());
        if (item != NULL) {
            QGraphicsPixmapItem* imgPix = dynamic_cast<QGraphicsPixmapItem*>(item);
            if (imgPix != NULL) {
                m_bMouseTranslate = true;
                m_lastMousePos = event->pos();
            }
        }
        else {
            m_bMouseTranslate = true;
            m_lastMousePos = event->pos();
        }
    }

    QGraphicsView::mousePressEvent(event);
}
void CameraViewer::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == m_translateButton)
        m_bMouseTranslate = false;

    QGraphicsView::mouseReleaseEvent(event);
    m_bIsPressed = false;
}


void CameraViewer::SetSence()
{
    if(m_bHasJustStarted)
    {
        this->setScene(m_Scene);
        m_bHasJustStarted = false;
    }
}

void CameraViewer::CalScale(int width, int height)
{
    m_HeightScale = double(height) / this->height();
    m_WidthScale = double(width) / this->width();
    /*double scale = double(width) / this->width();
    if (m_WidthScale != scale && m_WidthScale!=0) 
    {
        m_WidthScale = scale;
        AfterResizeEvent();
    }*/
    
}

QPoint CameraViewer::getcontextMenuCoord()
{
    return m_contextMenuPos;
}

void CameraViewer::ShowImage()
{
    // TODO: make buffer size can be written 
    // turn m_bStop variable ture before stop grabbing ,then will not excute the GetImage function
    // l also judge the m_Image and the data_ptr of m_Image are nullptr or not,but it also crash 
    if (!m_bStop)
    {
        QSize s = this->size();
        m_Image = this->GetImage();
        if (m_Image.byteCount()>0)
        {
            m_PixmapItem->setPixmap(QPixmap::fromImage(m_Image).scaled(s, Qt::KeepAspectRatio));
            CalScale(m_Image.width(), m_Image.height());
            this->show();
        }
    }
    else
    {
        ClearImageQueue();
    }

}

void CameraViewer::Show(QImage image)
{
    if (!image.isNull())
    {
        m_Image = image;
        QSize s = this->size();
        m_PixmapItem->setPixmap(QPixmap::fromImage(image)); // .scaled(s, Qt::KeepAspectRatio));
        this->show();
        //QCoreApplication::processEvents();

        if (m_isFirstTime)
        {
            setScene(m_Scene);
            m_isFirstTime = false;
            m_Scene->setSceneRect(0, 0, image.width(), image.height());
        }
    }
}

void CameraViewer::ImageReceived(const QImage image)
{

    m_Mutex.lock();

    if (m_Images.size() == m_MaxSize)
    {
        m_Images.pop();
    }
    m_Images.push(image);

    m_Mutex.unlock();
}

QImage CameraViewer::GetImage()
{
    m_Mutex.lock();
    QImage res;
    if (!m_Images.empty())
    {
        res = m_Images.front();
        m_Images.pop();
    }
    m_Mutex.unlock();

    return res;
}

void CameraViewer::ClearImageQueue()
{
    m_Mutex.lock();
    if (!m_Images.empty())
    {
        std::queue<QImage> empty;
        std::swap(m_Images, empty);
    }
    m_Mutex.unlock();
}

void CameraViewer::EnableCross()
{
    UnenableCross();
    int height = m_Scene->height();
    int width = m_Scene->width();
    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(1);
    m_Xaxis = new QGraphicsLineItem(QLineF(QPoint(0, height / 2), QPoint(width, height / 2)));
    m_Xaxis->setPen(pen);
    m_Yaxis = new QGraphicsLineItem(QLineF(QPoint(width / 2, 0), QPoint(width / 2, height)));
    m_Yaxis->setPen(pen);
    m_Scene->addItem(m_Xaxis);
    m_Scene->addItem(m_Yaxis);

    if (1) {
        qreal radius = 20;
        QPointF center(width / 2, height / 2);
        m_circle = new QGraphicsEllipseItem(center.x() - radius,
            center.y() - radius,
            2 * radius,
            2 * radius
        );

        QPen pen(Qt::red, 2);
        m_circle->setPen(pen);
        m_Scene->addItem(m_circle);
    }
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

// 放大/缩小
void CameraViewer::wheelEvent(QWheelEvent* event)
{
    // 滚轮的滚动量
    QPoint scrollAmount = event->angleDelta();
    // 正值表示滚轮远离使用者（放大），负值表示朝向使用者（缩小）
    scrollAmount.y() > 0 ? zoomIn() : zoomOut();
}

// 放大
void CameraViewer::zoomIn()
{
    zoom(1 + m_zoomDelta);
}

// 缩小
void CameraViewer::zoomOut()
{
    zoom(1 - m_zoomDelta);
}

// 缩放 - scaleFactor：缩放的比例因子
void CameraViewer::zoom(float scaleFactor)
{
    // 防止过小或过大
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.1 || factor > 25)
        return;

    scale(scaleFactor, scaleFactor);
    m_scale *= scaleFactor;
}

// 平移
void CameraViewer::translate(QPointF delta)
{
    // 根据当前 zoom 缩放平移数
    delta *= m_scale;
    delta *= m_translateSpeed;

    // view 根据鼠标下的点作为锚点来定位 scene
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    QPoint newCenter(VIEW_WIDTH / 2 - delta.x(), VIEW_HEIGHT / 2 - delta.y());
    centerOn(mapToScene(newCenter));

    // scene 在 view 的中心点作为锚点
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}

void CameraViewer::graphViewcustomContextMenuRequested(const QPoint &pos)
{
    m_contextMenuPos = pos;
}
