#pragma once

#include <QWidget>
#include "piMotionActor/Motion3DModel.h"
#include "ui_MoveWidget2D.h"
#include <QFutureWatcher>
#include "piMotionActor/calibrationconfig.h"

class MoveWidget2D : public QWidget
{
    Q_OBJECT

  public:
    MoveWidget2D(QWidget *parent = nullptr);
    ~MoveWidget2D();

    void setTitle(QString title);
    void setEnabled(bool enabled);

  private:
    void init();

  protected:
    Ui::MoveWidgetClass2D ui;
};

/*
 * front -x，rear x，left y，right -y，up z，down -z
 */
class MotionMoveWidget2D : public MoveWidget2D
{
    Q_OBJECT
  public:
    MotionMoveWidget2D(QWidget *parent = nullptr);
    ~MotionMoveWidget2D();

    void setParentWidget(QWidget *widget);
signals:
    void moveEnabled(bool isEnable);

  private slots:
    void moveXSub();
    void moveXAdd();
    void moveYAdd();
    void moveYSub();

    void update2dMotionPos(float x, float y);
    void handleFinished();

  private:
    void init();
    Result moveRel(const cv::Point2f &coord);

  private:
    QWidget *m_parent;
    QFutureWatcher<Result> watcher;
};
