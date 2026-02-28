#pragma once

#include "MLColorimeterMode.h"
#include <QWidget>
#include <QMutex>
#include "ui_measureWidget.h"
#include "Core/icore.h"
#include "accordion/qaccordion.hpp"
#include "Result.h"
#include <QFutureWatcher>
#include <opencv2\opencv.hpp>
#include "IQMetrics/MetricsWidgetBase.h"
#include "iqmetricconfig.h"
//#include "Focus/AutoFocusWidget.h"
#include "LineCutWidget.h"
#include "HistogramWidget.h"
#include "MinMaxStaWidget.h"

class MeasureWidget : public QWidget, public MLCameraCallback
{
    Q_OBJECT

public:
    MeasureWidget(QWidget *parent = nullptr);
    ~MeasureWidget();

    void postInitalize();
    Result connectCamera();
    void setImageRotateMode(bool isSLB);

public:
    virtual void NotifyCameraStateChanged(MLCameraState old_state, MLCameraState new_state) override;
    virtual void NotifyCameraFrameReceived(cv::Mat img, MLPixelFormat format) override;
    virtual void NotifyCameraGrayLevel(int gray_level) override;

public slots:
    void on_connectCamera_clicked();
    void on_disconnectCamera_clicked();
    void connectCameraSlot();

private slots:
    void on_loadPictureButton_clicked();
    void graphViewcustomContextMenuRequested(const QPoint &pos);

    void on_btn_unfold_clicked();
    void on_btn_fold_clicked();
    void on_radioBtn_liveImage_toggled();
    void on_checkBox_SLBImgRotate_stateChanged(int state);

    //TODO: test
    void on_btn_takeRGB_clicked();
    void showConnectWarning(QString message);
    void setWidgetEnable(QWidget *wgt, bool enable);
    void on_showCross_stateChanged(int state);
    void on_showFOV_stateChanged(int state);
    void on_saveimgbtn_clicked();

    void handleFinished();

    void connectStatus(bool connected);
    void stopGrabSlot(bool stop);
    void setImageRotateModeSlot(bool isSLB);

    void updateDutConfigSlot();
    void onActionToggled();
    void recipeRunStartOrEnd(bool isStart);

  signals:
    void enableWidget(QWidget *wgt, bool enable);
    void loadImageSignal(const QString &fileName);
    void messageShow(QString msg);
    void setImageRotateModeSignal(bool isSLB);
    void sigPlotProfileChanged(bool);
    void sigHistogramChanged(bool);
    void sigMinMaxStaChanged(bool);
  private:
    void setQss(QString filename);
    void initAccordion();
    void initUI();
    void initFovROI();
    void updateIqMetricsModel(bool offline);
    void initLiveImageRadio();
    void subscribeCamera(bool flag);
    QMap<int, cv::Point2f> updateFOVCenter();
    void updateAllFOVCenter(int isSLB);

  private:
    Ui::MeasureWidgetclass ui;
    QString mtfconfigFile = "./config/mtfconfig.json";

    cv::Mat curNineSquareImg;
    QMenu *menu;
    QToolBar* m_toolBar; 
    QPoint curpost;
    QFutureWatcher<Result> watcher;
    QAccordion *m_accordion;
    QList<MetricsWidgetBase*> m_iqMetricsWidgets;
    LineCutWidget *m_lineCutWindow;
    HistogramWidget *m_histogramWidget;
    MinMaxStaWidget* m_MinMaxStaWidget;
    //AutoFocusWidget *m_autoFocusWidget;
};