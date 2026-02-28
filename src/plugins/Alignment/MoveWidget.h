#pragma once

#include <QWidget>
#include "piMotionActor/Motion3DModel.h"
#include "ui_MoveWidget.h"
#include <QFutureWatcher>
#include "piMotionActor/calibrationconfig.h"

enum RelMoveType {
    xAdd = 0,
    xSub,
    yAdd,
    ySub,
    zAdd,
    zSub
};

class MoveWidget : public QWidget
{
    Q_OBJECT

  public:
    MoveWidget(QWidget *parent = nullptr);
    ~MoveWidget();

    void setTitle(QString title);
    void setEnabled(bool enabled);

  private:
    void init();

  protected:
    Ui::MoveWidgetClass ui;
};

/*
 * front -x，rear x，left y，right -y，up z，down -z
 */
class MotionMoveWidget : public MoveWidget
{
    Q_OBJECT
  public:
    MotionMoveWidget(QWidget *parent = nullptr);
    ~MotionMoveWidget();

    void setParentWidget(QWidget *widget);
    int type;
signals:
    void moveEnabled(bool isEnable);

  private slots:
    void moveXSub();
    void moveXAdd();
    void moveYAdd();
    void moveYSub();
    void moveUp();
    void moveDown();

    void update3dMotionPos(float x, float y, float z);
    void handleFinished();

  private:
    void init();
    Result moveRel(const cv::Point3f &coord, int type);

  private:
    QWidget *m_parent;
    QFutureWatcher<Result> watcher;
    cv::Point3f m_motion3DPos;
};
