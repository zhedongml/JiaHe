#pragma once

#include "alignment_global.h"
#include <QList>
#include <QPushButton>
#include <PrjCommon/service/ml_motion.h>
#include <opencv2/opencv.hpp>
//#include "MLAlgorithm/ml_rigidTrans3D.h"
#include"ImageDetection/ml_rigidTrans3D.h"

#include "AlignUtils.h"
#include "DistanceManage.h"
#include "piMotionActor/calibrationconfig.h"
#include <QMutex>
#include <QWaitCondition>
#include <PrjCommon/service/ml_camera.h>
#include "WaitPause.h"
#include "piMotionActor/MoveLimit.h"

using namespace MLImageDetection;
using namespace CORE;

namespace Align
{
class ALIGNMENT_EXPORT Alignment3D : public QObject, public AlignCalibrateCallback
{
    Q_OBJECT
  public:
    static Alignment3D *instance(QObject *parent = nullptr);
    ~Alignment3D();

    virtual void notifyInputAAStateChanged(bool isAlign);

    /**
     * @describe: Align i29 and eyebox by moving
     * @param acsCoord: ACS coordinates when the i29 align eyebox
     */
    Result i29AlignEyebox(cv::Point3f acsCoord, const CORE::CoordinateLimit &acsCoordLimit);
    Result i29AlignEyebox(cv::Point3f acsCoord, const QList<CORE::CoordinateLimit> &acsCoordLimitList);

    // according to eyebox index, move i29 to align eyebox directly
    Result moveAlignEyebox(int index);

    // calibrate ACS coordinates when i29 align eyebox, and save to "m_eyeboxAcsCoords"
    Result alignEyebox(AlignBaseInfo3D *alignInfo, const EyeReliefBaseInfo &eyeReliefInfo, AlignConfig alignConfig);

    // calibrate ACS coordinates when i29 align eyebox, and save to "m_eyeboxAcsCoords"
    Result alignEyeboxAcsXYZ(AlignBaseInfo3D *alignInfo, const EyeReliefBaseInfo &eyeReliefInfo,
                             AlignConfig alignConfig);

    // 4 same fiducial distance
    Result alignEyeboxAcsXYZ(std::vector<cv::Point3f> &acsCoords, const AlignBaseInfo3D &alignInfo,
                             const EyeReliefBaseInfo &eyeReliefInfo, AlignConfig alignConfig);

    // 3 fiducial, compare with above "alignEyeboxAcsXY", add Z value
    Result alignEyeboxAcsXYZ_old(std::vector<cv::Point3f> &acsCoords, const AlignBaseInfo3D &alignInfo,
                             const EyeReliefBaseInfo &eyeReliefInfo, AlignConfig alignConfig);

    // continue or stop
    void continueRun(bool isContinue, cv::Point2f pixel = cv::Point2f());
    void notifyPause(bool isPause);
    bool isStop();

    Result judgeDataValidity(const AlignBaseInfo3D &alignInfo, const EyeReliefBaseInfo &eyeReliefInfo,
                             const AlignConfig &alignConfig);

  private:
    Alignment3D(QObject *parent = nullptr);

    Result autoToManualFiducial(cv::Mat image, MvExpose mvExpose, cv::Point2f &pixel);
    Result manualFiducial(cv::Point2f &pixel, MvExpose mvExpose, bool isAutoFiducial = false);

    Result waitPause(bool isPause);

    Result judgeDataValidity(const AlignBaseInfo3D &alignInfo, const AlignConfig &alignConfig);

    Result fiducialCenter(cv::Point2f &pixel, const AlignBaseInfo3D &alignInfo, AlignConfig alignConfig, int fiducialNo = 0);
    Result autoIdentifyFiducial(cv::Point2f &pixel, std::string fiducialType, MvExpose mvExpose, int number, int fiducialNo);
    Result fiducialCenterMove(const AlignBaseInfo3D &alignInfo, cv::Point2f pixel);

    Result saveFiducialImage(cv::Point2f pixel, bool autoMode, int fiducialNumber);
    Result saveImage(cv::Mat image, QString dirName, QString fileName);

    Result mvAutoFocus(MvExpose mvExpose);
    Result mvAutoFocusStep(bool &isEnd, bool down = true);

    Result isConnected();

    Result moveInitPosition(const QString &dutSn);

  public:
    std::vector<cv::Point3f> getEyeboxAcsCoords();

  signals:
    void messageBoxSignal(QString message);
    void notifyStopSignal(bool isStop);
    void moveAlignEyeboxSignal(int index);
    void alignEyeboxEndSignal(bool success);

  private:
    static Alignment3D *m_instance;
    MLCamera *m_pCamera;

    WaitPause m_waitPause;

    cv::Point2f m_fiducialPixel;
    std::vector<cv::Point3f> m_eyeboxAcsCoords;

    Align::DistanceManage m_distanceManage;

    const cv::Point2f FIDUCIAL_ERROR_MAX = cv::Point2f(0.05, 0.05);
    const int FIDUCIAL_MANUAL_MOVE_COUNT_MAX = 5;
    const int FIDUCIAL_AUTO_MOVE_COUNT_MAX = 10;
};
}; // namespace Align
