#pragma once

#include "MLColorimeterMode.h"
#include "Result.h"
#include "Core/itoolbox.h"
#include "ui_ExposureWidget.h"
#include <plugins/Core/icore.h>
#include <json.hpp>

using Json = nlohmann::json;
using namespace ML::CameraV2;

class ExposureWidget : public Core::IToolBox
{
	Q_OBJECT

public:
	ExposureWidget(QString toolboxName = "", QWidget* parent = nullptr);
	~ExposureWidget();

signals:
	void sendAEStatus(Result status);
public slots:
	void on_autoexposurebtn_clicked();
	void ShowGrayValue(int gray);
    void updateCameraImage(const cv::Mat &img);
	void on_SetExposureTime_clicked();
    void on_exposuretimetxt_returnPressed();

    void on_curBinningSelectorBtn_clicked();
	void on_curBinningBtn_clicked();
	void on_curtBitdepthBtn_clicked();
    void on_btnRefresh_clicked();
    void setEnableFinish();
    void on_biningselectorCombox_activated(int index);
	void on_biningCombox_activated(int index);
    void on_bitdepthCombox_activated(int index);
    void on_comboBox_binningMode_activated(int index);
    void on_btn_binningMode_clicked();
    void UpdateAEStatus(Result status);
	void connectStatus(bool connected);

  private:
    void cameraSetEnable(bool);
    void calculateThread();
    int calculateGray(const cv::Mat &img);
    cv::Mat getImage();
    Result readFile(const QString &fileName = "./config/mlcolorimeter/AutoExposureConfig.json");

  private:
	Ui::ExposureWidgetClass ui;
    std::thread m_thread;
    std::atomic<bool> m_runThread = false;
    QString m_oldColor;
    Core::ICore *m_coreImpl = nullptr;
    QFutureWatcher<void> watcher;

    bool m_threadStart = true;
    mutable QMutex m_mutex;
    QList<cv::Mat> m_images;
    int m_maxNumberGray = 2000;
};
