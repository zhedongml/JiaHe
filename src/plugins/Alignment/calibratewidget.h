#pragma once
#include "ui_calibratewidget.h"
#include <QWidget>
#include "CameraModel.h"
#include "fiducialpoint.h"
#include <QFutureWatcher>
#include "DockWidget.h"
#include "DockManager.h"
#include <QTimer>
#include "AlignWidget.h"
#include "DeviceControlWidget.h"
#include "FiducialCalibrateWidget.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
class CalibrateWidgetClass;
};
QT_END_NAMESPACE

class CalibrateWidget : public QWidget
{
    Q_OBJECT

  public:
    CalibrateWidget(QWidget *parent = nullptr);
    ~CalibrateWidget();

    Result deviceConnect(bool isBtn = false);
    bool isAlignWidgetRun();

private slots:
    void on_connectCameraBtn_clicked();
    void on_disconnectCameraBtn_clicked();
    void on_cbx_cross_stateChanged(int state);
    void graphViewcustomContextMenuRequested(const QPoint &pos);
    void addFiducial(bool updateScene = true);
    void deleteFiducial();
    void continueFiducial();
    void updateFidPosTable(qreal x, qreal y, int id);
    void NotifyCameraFrameReceived();
    void manualFiducialModeToggled(bool checked);
    void messageBoxSlot(QString message);
    void notifyStopSlot(bool isStop);
    void handleFinished();
    void handleFinishedLoad();
    void emgStopBtnClicked();
    void watchAndHandleTime();
    void on_btn_right_clicked();
    void on_btn_left_clicked();

    void recipeRunStartOrEnd(bool isStart);

    void addProjector(bool updateScene = true);

  public slots:
    void UpdateConfigSlot();
    QPointF onSlotgetFiducialPos(QImage& image);
    void deviceMoveInitDUTPosition();
    void deviceMoveInitSLBPosition();

  signals:
    void addFiducialSignal(QPointF scenePos);
    void deleteFiducialSignal(QPointF scenePos);
    void updateFiducialSignal(QPointF scenePos);

  private:
    void init();
    void initMenu();
    void initActionMenu();
    void initTabwidget();
    void initRadioButton();
    void initConfig();
    void waitConnect(bool flag);
    Result connectTiltDUT(QString ipEtc);
    // void setQss(QString filename);
    Result connectCamera(QString sn);
    Result connectAcsCamera(QString ipEtc);
    void releaseFiducials();
    FiducialPoint* getCurrentFiducial();
    void addDockWdg(QString title, bool handing, QWidget *wdg);
    bool isAllFloating();
    void initButton();

  private:
    Ui::CalibrateWidgetClass ui;
    bool m_devicesConnectSuccess;
    QFutureWatcher<Result> watcher;
    QFutureWatcher<string> watcherMoveInit;

    QMenu *m_menu;
    const QString ADD_FIDUCIAL_ACTION_NAME = "Add fiducial";
    const QString DELETE_FIDUCIAL_ACTION_NAME = "Delete fiducial";
    const QString CONTINUE_FIDUCIAL_ACTION_NAME = "Continue";

    AlignWidget *alignWidget = new AlignWidget;
    FiducialCalibrateWidget *fiducialWidget = nullptr/* = new FiducialCalibrateWidget*/;

    int m_dockCounts = 0;
    bool dockFlag = true;
    ads::CDockAreaWidget *dockPageManager = nullptr;
    ads::CDockManager *m_dockManager;
    QList<ads::CDockWidget *> m_docks;
    bool floatingFlag = false;
    QTimer *timer = new QTimer(this);
};
