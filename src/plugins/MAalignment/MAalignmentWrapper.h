#pragma once
#include "aligmentWidget.h"
#include "PrjCommon/irecipewrapper.h"
#include "PrjCommon/service/ml_motion.h"
#include <QObject>
#include "Result.h"
#include <piMotionActor/Motion2DModel.h>
#include "SLBWidget.h"

class MAalignmentWrapper : public IRecipeWrapper
{
    Q_OBJECT

  public:
    MAalignmentWrapper(QObject *parent, aligmentWidget *alignWidget, SLBWidget* slbWidget);
    ~MAalignmentWrapper();

    void Invoke(QString cmd, QString param) override;
    void notifyStop(bool isstop) override;
    void notifyPause(bool isPause) override;

    Result MAalignmentWrapper::MTMotionMove(QString param);

  private:
    Result startCalibrate(QString param);
    Result setAngle(QString param);
    Result judgeAngleAndMoveInitPos(motion2DType type, cv::Point2f coordTarget);
    Result slbTips(QString param);
    Result alignmentSLB(QString param);

  private:
    CORE::WrapperStatus m_Status;
    aligmentWidget * m_alignWidget;
    SLBWidget* m_slbWidget;
    bool m_isStop = false;
};
