#pragma once

#include <QDialog>
#include "ui_MtfDialog.h"
#include "PrjCommon/CustomPlot.h"
#include <opencv2\opencv.hpp>
#include <opencv2\videoio.hpp>
#include "../mtfpipeline/pipeline.h"
#include "CameraViewer.h"
#include "mtfmodelconfig.h"
#include "Result.h"

enum MTF_FOV_TYPE {
    CENTER,
    EIGHT,
    SIXTEEN,
    TWENTY,
    TWENTYFIVE,
    ALL
};

class MtfDialog : public QDialog
{
    Q_OBJECT

public:
    MtfDialog(CameraViewer* viewer, QWidget* parent = nullptr);
    ~MtfDialog();

    Result calculateRect(cv::Mat rawImage, const QMap<int, cv::Rect>& rectMap);
    Result calculateEdge(cv::Mat rawImage, const QMap<int, cv::Rect>& edgeMap);
    void setTableShowFreq(const std::vector<std::string>& freqStrs);

private:
    void initUI();
    void initChart();
    void initTableStyle();
    Result calMtf(Mat& roiImg, int roiIndex, int aoiIndex = 0);
    Result calMtfRect(cv::Mat& rawImage, cv::Rect rect, int AOIindex);

    void updateChart(int roiIndex, int aoiIndex = 0, bool first = false);
    void updateChart(double* x, double* y, int size, CustomPlot* chart, QString seriesName = "", bool delAllSeries = false);
    void updateTable(QMap<int, QMap<int, PipeLine*>> pipelineMap, bool isRect);
    void updateTableHeader(QMap<int, QMap<int, PipeLine*>> pipelineMap, bool isRect, std::vector<std::string> freqStrs);
    void updateTableHeaderROI(QMap<int, QMap<int, PipeLine*>> pipelineMap, bool isRect, std::vector<std::string> freqStrs);
    void updateTableData(QMap<int, QMap<int, PipeLine*>> pipelineMap, bool isRect, std::vector<std::string> freqStrs);
    void updateTableDataROI(QMap<int, QMap<int, PipeLine*>> pipelineMap, bool isRect, std::vector<std::string> freqStrs);
    
    void clear(bool initChart = true);
    void clearChart(bool initChart = true);

    int getBinning(const Mat& img);

    Result updateChartRectOne(int aoiIndex);
    Result calculateCurrentRect();

private slots:
    void on_maxfreq_textChanged();
    void on_queryfreq_editingFinished();
    void on_comboBox_rectIndex_activated();

private:
    Ui::MtfDialogClass ui;
    CameraViewer* m_viewer;

    CustomPlot* mtfChart;
    CustomPlot* lsfChart;

    // AOIindex, ROIindex, pipeLine
    QMap<int, QMap<int, PipeLine*>> m_pipelineMap;
    QMap<int, QMap<int, cv::Rect>> m_roiMap;

    double maxMTFFreq = 50;

    cv::Mat m_rawImage;
    QMap<int, cv::Rect> m_rectMap;

    std::vector<std::string> m_tableFreqStrs;
};
