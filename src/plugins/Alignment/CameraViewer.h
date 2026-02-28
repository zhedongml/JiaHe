#ifndef XML_CAMERA_VIEW_H_
#define XML_CAMERA_VIEW_H_

#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QThread>
#include <queue>
#include <QObject>
#include <QMutex>
#include <QMouseEvent>
#include <QMenu>
#include <QMatrix>
#include <PrjCommon/service/ml_motion.h>
#include <QLabel>

using namespace CORE;
using namespace std;

class CameraViewer : public QGraphicsView
{
    Q_OBJECT

public:
    CameraViewer(QWidget *parent);
    ~CameraViewer();

public:
    void Show(QImage image);

    void ImageReceived(const QImage image);

    QImage GetImage();

    void ClearImageQueue();

    void EnableCross();

    void UnenableCross();

    void SetSence();

    void CalScale(int width, int height);

    QPoint getcontextMenuCoord();

private:
    QGraphicsScene* m_Scene;
    QGraphicsPixmapItem* m_PixmapItem;
    QImage m_Image;

    std::queue<QImage> m_Images;
    
    QMutex m_Mutex;
    size_t m_MaxSize;

    bool m_bIsPressed;

    vector<MLROI> m_ROIs;

    DUTData m_DUTData;

    QList<CORE::SpotData> m_Dots;
    ML_Point2f m_Left; ML_Point2f m_Right;

    QGraphicsItemGroup* m_DUTDataItem;
    QGraphicsItemGroup* m_CaluDataItem;
    QPen pen1;
    QPen pen2;

    QLabel *m_GrayLevel;
    bool m_isFirstTime = true;

    QGraphicsEllipseItem* m_circle = nullptr;

public:
    bool m_bHasJustStarted;
    bool m_bStop;
    double m_WidthScale;
    double m_HeightScale;

private slots:
    void ShowImage();
public Q_SLOTS:
    void zoomIn();  // 放大
    void zoomOut();  // 缩小
    void zoom(float scaleFactor); // 缩放 - scaleFactor：缩放的比例因子
    void translate(QPointF delta);  // 平移
    void graphViewcustomContextMenuRequested(const QPoint &pos);

signals:
    void SendMousePosSignal(QPointF,QString);

private:
    void SetMouseTracking();
    //void AfterResizeEvent();

    Qt::MouseButton m_translateButton;  // 平移按钮
    qreal m_translateSpeed;  // 平移速度
    qreal m_zoomDelta;  // 缩放的增量
    bool m_bMouseTranslate;  // 平移标识
    QPoint m_lastMousePos;  // 鼠标最后按下的位置
    qreal m_scale;  // 缩放值
    QPoint m_contextMenuPos;
    QGraphicsLineItem *m_Xaxis=nullptr;
    QGraphicsLineItem *m_Yaxis=nullptr;
  protected:
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;

};
#endif // !XML_CAMERA_VIEW_H_