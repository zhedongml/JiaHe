#pragma once

#include "alignment_global.h"
#include <QList>
#include <QPushButton>
#include <PrjCommon/service/ml_motion.h>
#include <opencv2/opencv.hpp>
#include "AlignUtils.h"
#include "piMotionActor/calibrationconfig.h"
#include <QMutex>
#include <QWaitCondition>
#include <PrjCommon/service/ml_camera.h>
#include "WaitPause.h"
#include "piMotionActor/MoveLimitJX.h"
#include "AxesTransfer2D.h"

using namespace CORE;
using namespace std;

namespace Align
{
    class ALIGNMENT_EXPORT Alignment2D : public QObject, public AlignCalibrateCallback
    {
        Q_OBJECT
    public:
        static Alignment2D* instance(QObject* parent = nullptr);
        ~Alignment2D();

        virtual void notifyInputAAStateChanged(bool isAlign);

        /**
         * @describe: Align i29 and eyebox by moving
         * @param acsCoord: ACS coordinates when the i29 align eyebox
         */
        Result i29AlignEyebox(cv::Point3f acsCoord, const CORE::CoordinateLimit& acsCoordLimit);
        Result i29AlignEyebox(cv::Point3f acsCoord, const QList<CORE::CoordinateLimit>& acsCoordLimitList);

        // according to eyebox index, move i29 to align eyebox directly
        Result moveAlignEyebox(int index);

        // calibrate ACS coordinates when i29 align eyebox, and save to "m_eyeboxAcsCoords"
        Result alignEyebox(AlignBaseInfo2D* alignInfo, AlignConfig alignConfig);

        // calibrate ACS coordinates when i29 align eyebox, and save to "m_eyeboxAcsCoords"
        Result alignEyeboxAcsXYZ(AlignBaseInfo2D* alignInfo, AlignConfig alignConfig);

        // fiducial identify and calculate
        Result alignEyeboxAcsXYZ(std::vector<cv::Point3f>& acsCoords, const AlignBaseInfo2D& alignInfo,
            AlignConfig alignConfig);

        // fiducial identify and calculate
        Result alignEyeboxAcsXYZ_DutMove(std::vector<cv::Point3f>& acsCoords, const AlignBaseInfo2D& alignInfo,
            AlignConfig alignConfig);

        // continue or stop
        void continueRun(bool isContinue, cv::Point2f pixel = cv::Point2f());
        void notifyPause(bool isPause);
        bool isStop();

        Result judgeDataValidity(const AlignBaseInfo2D& alignInfo, const AlignConfig& alignConfig);

        Result relativeMoveByWafer(const cv::Point3f &waferRelPos);

    private:
        Alignment2D(QObject* parent = nullptr);

        Result autoToManualFiducial(cv::Mat image, MvExpose mvExpose, cv::Point2f& pixel);
        Result manualFiducial(cv::Point2f& pixel, MvExpose mvExpose, bool isAutoFiducial = false);

        Result waitPause(bool isPause);

        Result fiducialIdentify(cv::Point3f& fiducialCenterAcs, const AlignBaseInfo2D& alignInfo, AlignConfig alignConfig, int fiducialNo = 0, bool dutMove = false);
        Result fiducialCenter(cv::Point3f& fiducialCenterAcs, const AlignBaseInfo2D& alignInfo, AlignConfig alignConfig, int fiducialNo = 0);
        Result fiducialIdentifyCal(cv::Point2f &pixel, cv::Point3f& fiducialCenterAcs, const AlignBaseInfo2D& alignInfo, AlignConfig alignConfig, int fiducialNo = 0, int number = 0, bool dutMove = false);
        Result autoIdentifyFiducial(cv::Point2f& pixel, std::string fiducialType, MvExpose mvExpose, int number, int fiducialNo);
        Result calFiducialCenterMoveCoord(cv::Point3f& fiducialCenterAcs, const AlignBaseInfo2D& alignInfo, cv::Point2f pixel);
        Result calFiducialCenterMoveCoord_DUT(cv::Point3f& fiducialCenterAcs, const AlignBaseInfo2D& alignInfo, cv::Point2f pixel);

        Result saveFiducialImage(cv::Point2f pixel, bool autoMode, int fiducialNumber);
        Result saveImage(cv::Mat image, QString dirName, QString fileName);

        Result isConnected();
        Result moveInitPosition(const QString& dutSn);

        vector<cv::Point2f> getList2D(const vector<cv::Point3f> &list3D);
        cv::Point2f getList2D(const cv::Point3f& point);

        // vector<cv::Point2f> inputAcsList
        Result calDutInput2D(cv::Point2f &dutCoord, const cv::Point2f& mvAlignProjectorCoord, const cv::Point2f & mvAlignInputCoord);

    public:
        std::vector<cv::Point3f> getEyeboxAcsCoords();

    signals:
        void messageBoxSignal(QString message);
        void notifyStopSignal(bool isStop);
        void moveAlignEyeboxSignal(int index);
        void alignEyeboxEndSignal(bool success);

    private:
        MLCamera* m_pCamera;
        WaitPause m_waitPause;

        cv::Point2f m_fiducialPixel;
        std::vector<cv::Point3f> m_eyeboxAcsCoords;
        cv::Point2f m_inputCoord;  // DUT input grating align projector, DUT motion 2D coordinate

        RigidTrans2D m_rigidTrans2D;

        const cv::Point2f FIDUCIAL_ERROR_MAX = cv::Point2f(0.05, 0.05);
        const int FIDUCIAL_MANUAL_MOVE_COUNT_MAX = 5;

    };
}; // namespace Align
