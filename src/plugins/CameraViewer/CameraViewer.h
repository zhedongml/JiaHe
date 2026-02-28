#pragma once

#include "cameraviewer_global.h"
#include "fiducialpoint.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsView>
#include <QLabel>
#include <QMutex>
#include <QWaitCondition>
#include <QWheelEvent>
#include <fstream>
#include <json.hpp>
#include <memory>
#include <opencv2/opencv.hpp>
using cv::Mat;
using Json = nlohmann::json;

enum ImageFromat
{
    Mono8,

    Mono16,

    RGB24
};

enum CAMERAVIEWER_EXPORT ImageModel
{
    MTF,

    IQ1
};

enum CAMERAVIEWER_EXPORT ImageType
{
    ImageX = 0,

    ImageY = 1,

    ImageZ = 2
};
class MyLineItem : public QGraphicsLineItem {
public:
    MyLineItem(QLineF line) : QGraphicsLineItem(line) {};
    ~MyLineItem() {};
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
        QWidget* widget) {
        double scaleFactor = painter->transform().m11();
        QPen myPen = this->pen();
        myPen.setWidthF(myPen.widthF() / scaleFactor);
        painter->setPen(myPen);
        painter->drawLine(this->line());
    };
};
class CAMERAVIEWER_EXPORT CameraViewer : public QGraphicsView
{
    Q_OBJECT

  signals:
    void imageReadySignal();
    void sigUpdateCutLine(QList<QPointF>);
    void showProFileWindow(bool);
    void sigMinMaxUpdate(double, double, cv::Point, cv::Point);
    void sigHistogramUpdate(cv::Mat &);

  public slots:
    void onImageReady();
    void setLineItemVisible(bool);
    void calMinMaxSta(bool);
    void calHistogram(bool);

  public:
    CameraViewer(QWidget *parent);
    ~CameraViewer();

  public:
    void Enqueue(Mat &source);

    void SetImageList(QList<QImage> imageList);

    void EnableCross();
    void EnableFOVCross();

    void EnableFOV();

    void UnenableCross();
    void UnenableFOVCross();

    void UnenableFOV();

    void AddROI(int index, QPoint center, QPoint size);

    void DeleteROIByIndex(int index);

    void MoveROIByIndex(int index, QPoint step);

    void ChangeROISizeByIndex(int index, QPoint center, QPoint size);

    void HideROI();

    void ShowROI();

    void ClearALLROI();

    void SetImageModel(ImageModel model);

    void SetXYZImages(Mat &X, Mat &Y, Mat &Z);

    void SetXYZ(Mat &X, Mat &Y, Mat &Z);

    void SetYxy(Mat &Y, Mat &x, Mat &y);

    void SelectImageShow(ImageType type);

    QGraphicsPixmapItem *getPixmapItem();

    QGraphicsScene *GetScene();

    void clearFiducialPoints();
    void addFiducialPoint(int x, int y);

    QMap<int, cv::Rect> getRoiMap();
    cv::Mat currentImage();
    void setCurrentImage(cv::Mat image);

    void clearImage();
    void setShowFov(bool display);
    void calPlotProfile();

  public:
    cv::Mat getShowImage();

  private:
    bool ImageTransform(Mat &src, QImage &dst);

    QImage GetQImage();

    void ClearImageQueue();

    void showFOV(float radius, int i);

    void drawGrayLevelRegion(QPointF origin, QString text);

    QGraphicsItemGroup *drawROIRegion(int index, QPoint center, int width, int height);

    QStringList getImageYxy(int x, int y);

  protected:
    QPointF m_CurrentCenterPoint;

    /* From panning the view */
    QPoint m_LastPanPoint;

    void zoomIn();
    void zoomOut();
    void zoom(float scaleFactor);
    /* Set the current center point in the */
    void SetCenter(const QPointF &centerPoint);
    QPointF GetCenter();
    void updateCenter(QPointF pointBeforeScale, QPointF screenCenter);
    void translate(QPointF delta);
    /* Take over the interaction */
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

  private:
    QImage cvMat2QImage(const cv::Mat &mat);
    QImage cvMat2QImageType_old(const cv::Mat &mat);
    QImage cvMat2QImageType(const cv::Mat& mat);
    QImage cvMat2QImageType_jiaxing(const cv::Mat& mat);

    float calculateFOVRadius(float angle);
    float calculateFOV(float length);
    float calculateFOVRadiusDisCorrect(float angle);
    std::vector<float> calculateFOVDisCorrect(float xlen, float ylen);
    std::vector<float> getImageFOVAngle(int xPixel, int yPixel, int binNum);
    void loadAboutFOVConfig(const char *path);
    void paintCenter();

    cv::Point2f fovCenter(int binNum);

  public:
    void setFOVCenter(QMap<int, cv::Point2f> centerMap);

  private:
    qreal m_zoomDelta = 0.2;
    qreal m_scale = 1.0;
    QList<QImage> m_Data;
    mutable QMutex m_Lock;
    QWaitCondition m_DataAvailable;
    bool m_isFirstTime;
    QGraphicsScene *m_Scene;
    QGraphicsPixmapItem *m_PixmapItem;
    QGraphicsLineItem *m_LineCutItem;
    QGraphicsLineItem *m_Xaxis;
    QGraphicsLineItem *m_Yaxis;
    QGraphicsLineItem *m_XaxisAngle;
    QGraphicsLineItem *m_YaxisAngle;
    QGraphicsSimpleTextItem *m_GrayLeveText;
    QGraphicsEllipseItem* m_circle = nullptr;

    ImageFromat m_ImageFromat;

    QImage m_Image;
    QLabel *m_GrayLevel;
    QMap<int, QGraphicsItemGroup *> m_ROIMap;

    double m_ZoomFactor;
    bool m_bIsFitToWindow;
    bool m_bIsPressed;

    ImageModel m_ImageModel;

    QVector<Mat> m_XYZ;
    QVector<Mat> m_Yxy;
    Json settingJsonObj;
    std::string filepath;
    int m_IQImageWidth;
    int m_IQImageHeight;
    bool m_bGraylevelEnable;
    std::vector<FiducialPoint *> fidusVec;
    cv::Mat m_showImage;
    cv::Mat m_showImageClone;
    cv::Point2i m_circleCenter;
    float m_addRadius = 5;
    QList<QGraphicsEllipseItem *> m_circles;
    QMap<QGraphicsEllipseItem *, QGraphicsTextItem *> m_items;
    float m_initAngle = 5;
    float m_focalLength;
    float m_pixel;
    QMap<int, cv::Point2f> m_fovCenterMap; // binNum, fovCenter
    bool m_showFov = true;
    bool lineItemVisible = false;
    bool m_isCalMinMax = false;
    bool m_isCalHist = false;
    bool lineCutItemFinished = false;
    QList<float> m_CutLineAxisXDatas;
    QList<QPointF> m_CutLineAxisYGrayDatas;

    mutable QMutex m_lockLine;
    mutable QMutex m_lockLineData;
};
