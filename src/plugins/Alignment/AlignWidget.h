#pragma once
#include "ui_AlignWidget.h"
#include <QWidget>
#include <QFutureWatcher>
//#include "Alignment.h"
#include "DistanceManage.h"
#include "PrjCommon/service/ml.h"

using namespace Align;
using namespace CORE;

class ALIGNMENT_EXPORT AlignWidget : public QWidget
{
    enum State
    {
        STOP,
        RUNNING
    };

    Q_OBJECT

  public:
    AlignWidget( QWidget *parent = nullptr);
    ~AlignWidget();

    Result getTableFiducial(cv::Point2f &fiducialPixel);
    bool isAlignWidgetRun();
    
  private:
    void init();
    void updateState(State state);
    Result start();
    Result startThread(const AlignBaseInfo &alignInfo, const EyeReliefBaseInfo &eyeReliefInfo, AlignConfig alignConfig);
    void stop();
    void confirmFiducial();

    void addTableFiducial(const cv::Point2f &fiducialPixel);
    void updateTableFiducial(const cv::Point2f &fiducialPixel);
    void clearTableFiducial();

    Result analyzeFile();

    void binningOneToTwo();
    void imageBinningSwitch(QString file, int bin);
    void addSubFolderImages(QString path, QStringList &string_list);
    cv::Mat binningSwitch(cv::Mat &src, int bin);

  private slots:
    void on_btn_start_clicked();
    void on_btn_loadFile_clicked();
    void on_btn_confirmFiducial_clicked();
    void on_saveImageBtn_clicked();

    void threadFinished();
    void messageBoxSlot(QString message);

    void addFiducialSlot(QPointF scenePos);
    void deleteFiducialSlot(QPointF scenePos);
    void updateFiducialSlot(QPointF scenePos);

signals:
    void deleteFiducialSignal();

  private:
    Ui::AlignWidgetClass ui;
    const QString BTN_START_TEXT = "Start";
    const QString BTN_STOP_TEXT = "Stop";

    QFutureWatcher<Result> watcher;
};
