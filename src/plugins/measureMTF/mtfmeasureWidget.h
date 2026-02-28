#pragma once

#include <QDebug>
#include <QFileDialog>
#include <QFuture>
#include <QtCharts>
#include <QtConcurrent>
#include <QtWidgets/QVBoxLayout>
#include <opencv2\opencv.hpp>
#include <opencv2\videoio.hpp>
#include <plugins/MLAlgorithm/ml_cross_center.h>
#include <plugins/coreplugin/icore.h>

#include "yaml-cpp/yaml.h"

#include "../mtfpipeline/pipeline.h"
#include "../visionAlgorithm/visionAlgorithm.h"
#include "./cropper.hpp"
#include "CameraModel.h"
#include "CameraViewer.h"
#include "CrossArea.h"
#include "MLAlgorithm/CrossCenter.h"
#include "WPCCalibration.h"
#include "accordion/qaccordion.hpp"
#include "callout.h"
#include "chromaticitycameramodel.h"
#include "coreplugin/metricsdata.h"
#include "coreplugin/service/Module.h"
#include "crossimgmtfmodel.h"
#include "extensionsystem/Services.h"
#include "icecream.hpp"
#include "imagepixmap.hpp"
#include "iqmetricconfig.h"
#include "iqmetricwidget.h"
#include "iqtmodel.h"
#include "loggingwrapper.h"
#include "mtfmodelconfig.h"
#include "roimanager.h"
#include "ui_mtfmeasureWidget.h"
#include "coreplugin/service/ml_motion.h"

class ROIInfo
{
  public:
    int index = 1;
    int x, y, width, height;
    ROIInfo(int startx, int starty, int rectWidth, int rectHeight, int id)
    {
        x = startx;
        y = starty;
        width = rectWidth;
        height = rectHeight;
        index = id;
    }
};
struct AutoROIOption
{
    int type = 1;
    int count = 4;
    bool applytoallImg = false;
    // int width = 100;
    // int height = 120;
    // int offset = 30;
    int width = 150;
    int height = 120;
    int offset = 90;
    bool closeCenter = true;
    // int direct = AutoROIDir::DIRHORIZONTAL;
};

class MTFmeasureWidget : public QWidget, public MLMotionCallback
{
    Q_OBJECT

  public:
    MTFmeasureWidget(QWidget *parent = nullptr);
    ~MTFmeasureWidget();
    enum MTFRUNTYPE
    {
        ONEROIIMG,
        RAWIMG,
        IMGCOMPARE
    };
    int SampleWidth;
    int SampleHeight;
    QGraphicsScene *scene;
    void postInitalize();
    int getBinning();
    void NotifyMotionStateChanged(MLMotionState old_state, MLMotionState new_state) override;
    void NotifyMotionPosition(int position) override{};
  public slots:
    void autoROIUpdate();
    void on_connectCamera_clicked();
    void connectCamera();
  private slots:
    void on_queryfreq_editingFinished();
    void on_disconnectCamera_clicked();
    void on_loadPictureButton_clicked();
    void on_calMTFButton_clicked();
    void on_okButton_clicked();
    void graphViewcustomContextMenuRequested(const QPoint &pos);
    void addNewROI(bool updatescene = true);
    void clearROI();
    void loadROIImg(int x, int y, int width, int height);
    void tooltip(QPointF point, bool state);
    void selectCross(int index);
    void NotifyCameraFrameReceived(QList<QImage> imageList);
    void on_maxfreq_textChanged();
    void on_calIqtMetricBtn_clicked();

    void delOneROI();

    void on_manualMtf_clicked();
    void updateIQResult(cv::Mat mat);
    void on_mtfModeBtn_clicked();
    void on_iqModeBtn_clicked();

    // void on_mancollectimgsCkb_stateChanged(int state);
    void on_mancollectimgsCkb_clicked(bool checked = false);
    void on_pushimgToParamsBtn_clicked();
    void on_newImgsToParamsBtn_clicked();

    void upDateTrisXYZImage(std::vector<cv::Mat>);
    void on_iq1Images_currentIndexChanged(int index);

    void setWidgetEnable(QWidget *wgt, bool enable);
    void getAutoFocusDateFromCallback(MLStatus status, double progress, int *steparray, double *distance, double *data,
                                      int len);
    void on_showCross_stateChanged(int state);
    void on_saveimgbtn_clicked();
    void on_exportCsvBtn_clicked();
    void disFOVPositionPts(std::vector<cv::Point> pts);
    void onBitDeepthChanged(int bit);
    void on_WPC_clicked();
    void handleFinished();
    private:
    void initPI();
  signals:
    void enableWidget(QWidget *wgt, bool enable);
    void loadImageSignal(const QString &fileName);

  private:
    cv::Rect calROI;
    bool isCalbration = false;
    int curSelectIndex = -1;
    bool isAutoROI = true;
    bool calvmode;
    std::vector<CrossAreaConfig *> aoiCrosses;
    cv::Mat curNineSquareImg;
    Ui::MTFWidgetclass ui;
    VideoCapture capture;
    // PipeLine* mtfPipeline = NULL;
    QTimer *timer;
    Mat mFrame;
    Mat disMat;
    QImage disImage;
    Callout *m_tooltip = 0;
    double maxMTFFreq = 50;
    bool singleCrossM = false;
    int curImgROIIdx = 0;
    int areaNum = 0;
    double deltaX = 0;
    double deltaY = 0;

    QString mtfconfigFile = "./config/mtfconfig.json";

    QMenu *menu;
    QAction *addNewROIAction;
    QAction *delROIAction;
    QAction *delAllAction;
    QPoint curpost;
    QString curImgPath = "";
    QString aoipath, roipath;
    QVector<Cropper *> roivector;
    QVector<Cropper *> aoiCrossvector;
    QVector<PipeLine *> pipelinevec;
    CrossImgMTFModel *crossmtfModel = NULL;
    ROIPosConfig roiConfig;
    ContentPane *IQContentPane, *MtfContentPane;

    QChart *mtfChart;
    QValueAxis *xAxis_mtf;
    QValueAxis *yAxis_mtf;
    QChart *lsfChart;
    QValueAxis *xAxis_lsf;
    QValueAxis *yAxis_lsf;
    QMap<QString, QVector<ROIInfo> *> *roisinfoMap = new QMap<QString, QVector<ROIInfo> *>();
    AutoROIOption autoroiOpts;
    std::vector<s_SingleTargetMatch> resultAutoROI;
    void ReadFrame();
    void cal_mtf(Mat &img, QString seriesName = "");
    void cal_rawimg_mtf(QMap<QString, QVector<ROIInfo> *> *roisinfoMap);
    void initChart();
    void initToShow();
    void updateChart(double *x, double *y, int size, QChart *chart, QString seriesName = "", bool delAllSeries = false);
    void saveCrossedAreaToJson(const QString &fileName, double, double);
    void drawAutoROIs(std::vector<cv::Point> pt, int aoiId);
    void getCrossAreaInfo();
    void rewriteCrossAreaInfo();
    void roiDatasChange(QVector<Cropper *> roivector);
    bool CreateDir(const QString &path);
    void aoiDatasChange(cv::Mat &crossImg, int aoiIdx = 0);
    void clearChart();
    void initIQWidget();
    void showManualROIsByAoiId(int aoiid);
    void autoROI();
    void semiAutoROI();
    void setQss(QString filename);



    //WPCCalibration *m_CalibrationWidget;
    QDockWidget *m_ThroughFocusDock;
    QDockWidget *m_CalibrationDock;
    QChart *m_ThroughFocusChart;
    QValueAxis *m_AxisX;
    QValueAxis *m_AxisY;
    QLineSeries *m_CoarseScan;
    QLineSeries *m_FineScan;
    double m_AxisXmin, m_AxisXmax, m_AxisYmin, m_AxisYmax;
    QProgressBar *m_AutoFocusStatus;
    QFutureWatcher<Result> watcher;
    bool isUseWaitWidget = false;
};