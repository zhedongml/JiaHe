#pragma once

#include "windows.h"
#include "piMotionActor/Motion3DModel.h"
#include <QMutex>
#include <QObject>
#include <QtCore/qwaitcondition.h>
#include "AlignUtils.h"
#include "WaitPause.h"
#include "FindDistance.h"

#define AA_AUTO_TO_MANUAL 1

struct InputProjectorPixel
{
    cv::Point2f input;
    cv::Point2f projector;

    InputProjectorPixel(){}
};

enum MALIGN_STATE{
    MANUAL_START,
    AUTO_START,
    MANUAL_MIDDLE,
    MALIGN_END
};

class MAalignmentLogic : public QObject
{
    Q_OBJECT

  public:
    MAalignmentLogic(QObject *parent);
    ~MAalignmentLogic();
    static MAalignmentLogic *instance(QObject *parent = nullptr);
    
    Result calibrate(const AAlignConfig &alignConfig);
    Result calibrateZ(float &dutZ, const AAlignConfig &alignConfig);

    // continue or stop
    void continueRun(bool isContinue, InputProjectorPixel pixels = InputProjectorPixel());
    void notifyPause(bool isPause);

  signals:
    void updateOffset(double x, double y);
    void alignSignal(int state, QString message);
    void notifyStopSignal(bool isStop);

  private:
    Result alignXY(const AAlignConfig &alignConfig);
    Result autoAlignment(const AAlignConfig &alignConfig);
    Result manualAlignment(const AAlignConfig &alignConfig, bool auto2Manual = false);
    Result moveOffset(const AAlignConfig &alignConfig, int maxNumber);

    Result fiducialToInputPixel(const AAlignConfig &alignConfig, cv::Point2f fiducialPixel, cv::Point2f& inputPixel);

    Result waitPause(bool isPause);
    Result saveResultImage(bool flag);

    Result isConnected();

  private:
    static MAalignmentLogic *m_instance;
    Result m_Result;
    const cv::Point2f OFFSET_PIXEL_MIN = cv::Point2f(50, 50);

    FindDistance m_findDistance;
    WaitPause m_waitPause;
    InputProjectorPixel m_pixels;
};
