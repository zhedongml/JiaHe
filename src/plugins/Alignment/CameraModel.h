/****************************************************************************
 * @author  : Michael Gao
 * @date    : 2021/12/21
 * @file    :
 * @brief   : CameraModel,handle camera interactions,
 *            such as exposure, stream and so on
 *----------------------------------------------------------------------------
 *                           Change History
 *----------------------------------------------------------------------------
 * Date        | Version   | Author         | Description
 *----------------------------------------------------------------------------
 *             |           |                |
 *****************************************************************************/
#pragma once

#include "PrjCommon/service/ml_camera.h"
#include "PrjCommon/service/ml_motion.h"
#include <QImage>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QPointer>
#include <QVector>
#include <QWaitCondition>

using namespace CORE;

class CameraModel : public QObject, public MLCameraCallback
{
	Q_OBJECT
public:
	CameraModel(QObject* parent = Q_NULLPTR);
	~CameraModel();
	static CameraModel* GetInstance(QObject* parent = nullptr);
	Result connect(const char* cameraSn);
	bool disConnect();
	bool isConnected();

public slots:
	Result SetMLExposureAuto();

	Result SetExposureTime(double time);

	double GetExposureTime();

	cv::Mat GetImage();

	Result StopGrabbing();

	Result StartGrabbing();

	Result GrabOne();

	void SubscribeCameraEvent(MLCameraEvent event);

	void UnsubscribeCameraEvent(MLCameraEvent event);

	void Enqueue(const QImage image);

	QImage GetQImage();

	void ClearImageQueue();

	int GetGrayLevel();

	void SetInitExposureTime(double initTime);

signals:
	void updateCameraStatusSignal(MLCameraState status);
signals:
	void updateCameraStreamSignal();
signals:
	void updateCameraGrayLevelSignal(int gray_level);

private:
	void NotifyCameraStateChanged(MLCameraState old_state, MLCameraState new_state);
	void NotifyCameraFrameReceived(MLFrame* frame_ptr, void* buffer_ptr);
	void NotifyCameraGrayLevel(int gray_level);
	void NotifyAutoFocus(MLStatus status, double progress, int* steparray, double* distance, double* data, int len);
	bool m_bConnected = false;
	static CameraModel* self;

protected:
	QPointer<MLCamera> m_pCamera;

	QImage m_Image;

	int m_GrayLevel;

	QList<QImage> m_Data;
	mutable QMutex m_Lock;
	QWaitCondition m_DataAvailable;
	bool m_Stopping;
	const size_t m_MaxSize = 3;
};
