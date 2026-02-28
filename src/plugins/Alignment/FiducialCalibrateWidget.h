#pragma once

#include "piMotionActor/calibrationconfig.h"
#include "ui_FiducialCalibrateWidget.h"
#include "AlignUtils.h"
#include <QFutureWatcher>
#include <QWidget>
#include <QtConcurrent>
#include <plugins\PLCControl\PLCController.h>

using namespace Align;
using namespace CORE;

class FiducialCalibrateWidget : public QWidget
{
    Q_OBJECT

  public:
    FiducialCalibrateWidget(QWidget *parent = nullptr);
    ~FiducialCalibrateWidget();

    void loadConfig();
    bool isAlignWidgetRun();
    void recipeRunStartOrEnd(bool isStart);

  private slots:
    void on_bth_calFiducial_clicked();
    void on_btnSaveFiducialMotionCoods_clicked();
    void on_btnCalibrate_clicked();
    void on_btnStop_clicked();
    void on_btnSave_clicked();
    void on_btnRefresh_clicked();
    //MV camera move to see fiducial
    void on_btn_move_clicked();
    void on_cmbAlignMode_activated(const QString &text);
    void on_cmb_mvExposure_activated(const QString &text);

    void on_comboBox_eyeboxIndexs_activated(const QString &text);
    void on_btn_eyeboxMove_clicked();

    void on_btn_exportCsv_clicked();
    void on_btn_importCsv_clicked();

    void eyeboxClicked();
    void setBtnEnabled(bool);

    void calibrateHandleFinished();
    void eyeboxHandleFinished();
    void fiducialMoveHandleFinished();

    void refrashFiducialDisplay();
    void moveAlignEyeboxSlot(int index);
    void alignEyeboxEndSlot(bool success);

  private:
    void initUI();                                                                                            
    void setEnabledBtns(bool enabled);
    Result moveSeeFiducial();

    Result startCalibrate();
    Result getAlignData(AlignConfig &alignConfig, AlignBaseInfo3D &alignInfo, EyeReliefBaseInfo &eyeReliefInfo);
    Result getAlignData(AlignConfig& alignConfig, AlignBaseInfo2D& alignInfo);
    AlignConfig getAlignConfig();
    void updateDutConfig(DutConfig &dutConfig);

    void refrashFiducialDisplay(const QMap<int, cv::Point3f> &fiducials);

  private:
    Ui::FiducialCalibrateWidgetClass ui;
    bool m_devicesConnect = false;
    bool m_moveout = false;
    int m_currrentEyeIndex = 0;
    QString m_currentEyeName;
    std::vector<cv::Point3f> m_eyeboxAcsCoords;
    std::vector<cv::Point3f> m_fiducialCoords;

    QFutureWatcher<Result> watcherCalibrate;
    QFutureWatcher<Result> watcherEyebox;
    QFutureWatcher<Result> watcherMoveFiducial;

    const int DECIMAL = 2;
};
