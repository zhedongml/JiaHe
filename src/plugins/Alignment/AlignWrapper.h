#pragma once
#include "DistanceManage.h"
#include "PrjCommon/irecipewrapper.h"
#include "AnalyzeRecipe.h"
#include "calibratewidget.h"
#include <piMotionActor/Motion2DModel.h>

using namespace CORE;

class AlignWrapper: public IRecipeWrapper
{
    Q_OBJECT

  public:
    AlignWrapper(QObject *parent, CalibrateWidget *calibrateWidget);
    ~AlignWrapper();

    void Invoke(QString cmd, QString param) override;
    void notifyStop(bool isstop) override;
    void notifyPause(bool isPause) override;

  private:
    Result eyeRelief(QString param);
    // not used
    Result startCalibrate(QString param);
    Result startCalibrate3D(QString param);
    Result startCalibrate2D(QString param);
    Result moveAlignEyebox(QString param);
    Result dealDutConfig(QString param);

    Result loadInitPosition(QString param);

    Result deviceConnect(QString param);
    Result calibrateConfig(QString param);
    Result setCamera(QString param);
    Result mvExpose(QString param);

    Result createImageDir(QString param);
    Result motion3DConnect(QString param);

    Result setAngle(QString param);
    Result judgeAngleAndMoveInitPos(motion2DType type, cv::Point2f coordTarget);

    Result pupilDecenter(QString param);

  private:
    CalibrateWidget *m_calibrateWidget;
    bool m_isStop = false;
};
