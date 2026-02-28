#ifndef COLLIMATOR_ANGLE_CAL_H
#define COLLIMATOR_ANGLE_CAL_H
#include <thread>
#include "ml.h"
#include "ml_camera.h"
#include "collimatedlighttube_global.h"
#include <QObject>

class CORE::MLCamera;
class CrossCenterPro;

class COLLIMATEDLIGHTTUBE_EXPORT CollimatorAngleCal :public QObject
{
	Q_OBJECT
public:
	CollimatorAngleCal(double baseX, double baseY, double pixelSize, double focalLength, CORE::MLCamera* cameraPtr);
	~CollimatorAngleCal();

public:
	bool AngleCalculate();
	void SubscribeAngleCallBack(CORE::MLAngleXYCallback angleCallBack);

	double GetOffsetX();
	double GetOffsetY();
private:
	void RunThread();

private:
	CORE::MLCamera* m_camera = nullptr;
	CrossCenterPro* m_CCPro = nullptr;
	std::thread m_thread;
	bool m_isEndThread;
	CORE::MLAngleXYCallback m_angleCallBack;
	double m_offsetX;
	double m_offsetY;
};
#endif // COLLIMATOR_ANGLE_CAL_H
