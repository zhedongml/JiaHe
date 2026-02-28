#pragma once
#include "windows.h"

#include "CameraModel.h"
#include "DockManager.h"
#include "DockWidget.h"
#include "qfuturewatcher.h"
#include "ui_aligmentWidget.h"
#include <QWidget>
#include <integratingSphere\integratingspheretcpmodel.h>
#include <qstatusbar.h>
#include "dutPrCalibrateWidget.h"

class aligmentWidget : public QWidget
{
    Q_OBJECT

  public:
    aligmentWidget(QWidget *parent = nullptr);
    ~aligmentWidget();

    bool isAlignWidgetRun();
    Result deviceConnectAll(bool isBtn = false);

    Result connectTiltDUT(QString ipEtc);

  public slots:
    void on_connectBtn_clicked();
    void on_disconnectBtn_clicked();
    void on_autoAlignBtn_clicked();
    void on_manualAlignBtn_clicked();
    void on_resetLastAlignBtn_clicked();
    void on_stopAlignBtn_clicked();
    void on_saveImageBtn_clicked();
    void on_btn_mvRelIgSave_clicked();

    void on_cbx_showCross_stateChanged(int state);

    void handleFinished();
    void alignHandleFinished();

    void NotifyCameraFrameReceived();
    void updateFidPosTable(qreal x, qreal y, int id);
    void AddCircle(int id, QString title);
    void DeleteCircle(int id);
    int GetCircleCount();
    void graphViewcustomContextMenuRequested(const QPoint &pos);
    void on_btn_left1_clicked();
    void on_btn_right1_clicked();

    void messageBoxSlot(QString message);
    void notifyStopSlot(bool isStop);
    void alignSlot(int state, QString message);
    void continueActionSlot();
    void recipeRunStartOrEnd(bool isStart);

  private slots:
    Result deviceConnect(bool isBtn = false);
    void UpdateOffsetStatusBar(bool);
    void wdgContainerToggle();
    void watchAndHandleTime();
    //void on_checkBox_stateChanged();

  private slots:
    void UpdateStatusBar(double x, double y);
    void updateDutConfigSlot();

  private:
    void waitConnect(bool flag);
    void initConfig();
    Result connectACSC3D(QString ipEtc);
    Result connectTiltProjector(QString ipEtc);
    void init();
    void initMenu();
    void initButton();
    void initMenuAction();
    Result connectCamera(QString sn);
    void CreateStatusBar();
    void initDock();
    bool isAllFloating();
    ads::CDockWidget *addDockWdg(QString title, bool handing, QWidget *wdg);

    void deleteCircles();
    void updateAlignEnabled(bool enabled);

    Result deviceMoveInit();

  private:
    Ui::aligmentWidgetClass ui;
    QFutureWatcher<Result> watcher;
    QFutureWatcher<Result> watcherAlign;

    ads::CDockManager *m_dockManager;
    ads::CDockAreaWidget *dockPageManager;

    bool m_devicesConnectSuccess;

    const QString CONTINUE_NAME = "Continue";
    QMenu *m_menu;
    QAction *continueAction;
    QPointF circleInputPoint;
    QPointF circleProjectPoint;
    QStatusBar *m_statusBar;
    QLabel *m_AAOffset;

    QTimer *timer = new QTimer(this);
    bool dockFlag = true;
    bool m_showBox = true;
    int m_dockCounts = 0;
    QList<ads::CDockWidget *> m_docks;
    bool floatingFlag = false;
    QWidget *deviceWidget = nullptr;
    dutPrCalibrateWidget *inputAAWidget=nullptr;
};
