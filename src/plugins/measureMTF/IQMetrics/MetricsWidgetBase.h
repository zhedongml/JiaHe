#pragma once

#include <QObject>
#include "CameraViewer.h"
#include "accordion/qaccordion.hpp"
#include "cropper.hpp"
#include <QMenu>
#include "accordion/contentpane.hpp"
#include <QtConcurrent>
#include <QFutureWatcher>
#include "PrjCommon/WaitWidget.h"
#include "ui_measureWidget.h"
//#include "MLAlgorithm/ml_image_public.h"
//#include"ml_image_public.h"
#include"ImageDetection/ml_image_public.h"
#include "Result.h"

#define IQ_METRICS_TEST_MODE_OFFLINE "iq_metrics_test_mode_offline"

using namespace MLImageDetection;

class MetricsWidgetBase : public QWidget
{
    Q_OBJECT

public:
    MetricsWidgetBase(CameraViewer* viewer, QMenu* menu, QWidget* parent);
    virtual ~MetricsWidgetBase();

    void setFovCenter(const QMap<int, cv::Point2f>& fovCenterMap);
    void setMeasureWidgetUI(Ui::MeasureWidgetclass* ui);

    // metrics test: offline or online
    virtual void iqMetricsModeChange(bool offline) {};

protected slots:
    void graphViewcustom(const QPoint& pos);
    void cropperMousePressEvent(QGraphicsSceneMouseEvent* event);
    void loadImg(int x, int y, int width, int height);

    void addAction();
    void deleteAction();
    void deleteAllAction();

    void deleteCropper(Cropper* cropper);

    void accordionChanged(ContentPane* cpane);
    virtual void loadImageSlot(const QString& fileName) {}

    void setDialogViewer(CameraViewer* viewer);

    MLImageDetection::ROIParaNew fovROI();

protected:
    void initSence();
    void initAccordionBase(QAccordion* accordion, QString name, int maxHeight = 500, bool openContentPane = false);

    void drawIndex(Cropper* cropper);
    int getNewIndex(QList<Cropper*> crossAreas);

    void addOneCropper(int x, int y, int width, int height);
    void addOneCropper(int index, int x, int y, int width, int height);
    void addOneCropper(int index, cv::Rect rect);

    void menuUpdate(QMenu* menu, bool isCrossArea);

    bool isOffline();
    cv::Mat getCurrentImage(bool isRawImage = true);

    Result judgeImages(const cv::Mat& image, bool isRawImage = true);
    Result judgeImages(const QMap<QString, cv::Mat>& imageMap, bool isRawImage = true);
    Result judgeImages(const QList<cv::Mat>& images, bool isRawImage = true);

    virtual void initClear() = 0;
    QMap<int, cv::Point2f> fovCenter();

protected:
    ContentPane* m_cpane;
    QMenu* m_menu;
    QPoint m_curpost;
    QList<Cropper*> m_crossAreas;

    CameraViewer* m_viewer;
    int CROPPER_INDEX_MAX = 9;
    QString MENU_NAME;

    QPoint m_cropperWidthHeight;
    bool m_isCurrentAccordion = false;

    QAccordion* m_accordion;
    QFutureWatcher<Result> m_watcher;

    CameraViewer* m_dialogViewer = nullptr;

    QMap<int, cv::Point2f> m_fovCenterMap;

    Ui::MeasureWidgetclass* m_measureWidgetUi;
};
