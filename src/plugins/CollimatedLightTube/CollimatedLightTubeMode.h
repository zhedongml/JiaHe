#pragma once

#include "collimatedlighttube_global.h"
#include "PrjCommon/service/ml_camera.h"
#include "MLMindVisionCamera.h"
#include "CollimatorAngleCal.h"
#include "opencv2/opencv.hpp"
#include "Result.h"
#include <QObject>
#include <QWaitCondition>
#include <QImage>
#include <QPointer>
#include <QMutex>

using namespace CORE;

class COLLIMATEDLIGHTTUBE_EXPORT CollimatedLightTubeMode : public QObject, public MLCameraCallback
{
	Q_OBJECT
public:
	CollimatedLightTubeMode(QObject* parent = Q_NULLPTR);
	~CollimatedLightTubeMode();

	static CollimatedLightTubeMode* GetInstance(QObject* parent = nullptr);


public:
	Result Connect(const char* cameraSn);

	bool DisConnect();

	bool IsConnected();

	void SetMLExposureAuto();

	void SetExposureTime(double time);

	double GetExposureTime();

	cv::Mat GetImage();

	void SubscribeCameraEvent(CORE::MLCameraEvent event);

	void UnsubscribeCameraEvent(CORE::MLCameraEvent event);

	void ImageReceived(const QImage image);
	QImage GetQImage();

	CORE::MLCamera* GetCamera();

	void OnAngleUpdated(double angleX, double angleY);

	QString GetCollimatorAngleX();

	QString GetCollimatorAngleY();

	Result WriteAngleToCSV(const QString& filePath, const QString& angleX, const QString& angleY);

private:
	virtual void NotifyCameraStateChanged(CORE::MLCameraState old_state, CORE::MLCameraState new_state) {};
	virtual void NotifyCameraFrameReceived(CORE::MLFrame* frame_ptr, void* buffer_ptr);
	virtual void NotifyCameraGrayLevel(int gray_level) {};
	virtual void NotifyAutoFocus(CORE::MLStatus status, double progress, int* steparray, double* distance, double* data,
		int len) {};

signals:
	void showImageSignal();
	void updateCollimatorAngle(QString deltaX, QString deltaY);

protected:
	QPointer<CORE::MLCamera> m_pCamera = nullptr;
	bool m_bConnected = false;
	static CollimatedLightTubeMode* self;

	QMutex m_mutex;
	std::queue<QImage> m_Images;

	CollimatorAngleCal* m_collimatorAngleCal = nullptr;
	QString m_collimatorDeltaX = "NULL";
	QString m_collimatorDeltaY = "NULL";
};