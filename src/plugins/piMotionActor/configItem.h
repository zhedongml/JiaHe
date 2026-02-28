#pragma once
#include "pimotionactor_global.h"
#include "PrjCommon/service/ml_motion.h"
#include <QString>
#include <QJsonObject>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include "PrjCommon/service/ml.h"
#include "Motion3DModel.h"

//using Json = nlohmann::json;
using namespace std;

struct RangingPoint
{
	int rangingPointX;
	int rangingPointY;
	int rangingPointZ;
};

struct KeyenceInfo
{
	RangingPoint rangingPoint;
	int keyenceZeroPos;
	int keyenceRangeMax;
	int keyenceRangeMin;
};

struct KeyenceDviceInfo {
	QString type;
	int id;
};

typedef struct MotionLimit
{
	float x_pos;
	float x_neg;
	float y_pos;
	float y_neg;
} MotionLimit;

class PIMOTIONACTOR_EXPORT ConfigItem
{
public:
	static ConfigItem* instance();
	~ConfigItem();
	QString getMotion1DIpAndOther();
	QString getMotion3DIpAndOther();
	QString getMotion2DIpAndOther(int type);
	QString getMotion3DIpAndOther(int type);

	cv::Point2f getMotion1DLimit();
	MotionLimit getMotion2DLimits(int type);
	CoordinateLimit getMotion3DLimit(int type = withCamera);
	CoordinateLimit getOrientalLimit();
	Limit2D getMotion2DTiltLimit();
	Limit2D getMotion2DLimit();
	Limit2D getMotion2DLimit(int type);

	cv::Point2f getMotion2DZeroPositionRaw(int type, DUT_EYE_TYPE eyeType);
	cv::Point2f getMotion2DZeroPosition(int type, DUT_EYE_TYPE eyeType);
	void updateMotion2DZeroPosition(int type, DUT_EYE_TYPE eyeType, cv::Point2f tilt, bool forced = false);
	Result writeDutZeroPosition(DUT_EYE_TYPE eyeType, cv::Point2f tilt);

	QString getMotionType(nlohmann::json motion, QString type);

	QString getMVCameraSn();
	Result getCameraCoefficient(float& pixelSize, float& magnification);
	Result getCameraResolution(int& width, int& height);


	void setRealTimeUpdate(bool realTime);
	bool getRealTimeUpdate();



	// x:limit_min,y:limit_max



	KeyenceDviceInfo getKeyenceType();
	KeyenceInfo getKeyenceInfo();
	double getThorlabsAngle();

private:
	ConfigItem();
	Result loadFile();
	Result readJson(QString fileName);
	Result readJson(nlohmann::json& json, QString fileName);

	cv::Point2f getMotion2DZeroPositionOrigin(int type, DUT_EYE_TYPE eyeType = DUT_EYE_LEFT);

private:
	const QString FILE_MOTION = "./config/device/Motion.json";
	const QString FILE_MVCAMERA = "./config/device/camera.json";
	const QString FILE_KEYENCE = "./config/device/keyence.json";

	static ConfigItem* self;
	QJsonObject m_rootObj;
	nlohmann::json m_motionJson;
	nlohmann::json m_keyenceJson;
	nlohmann::json m_mvCameraJson; 

	// int type, int eyeType, zero pos
	QMap<int, QMap<int, cv::Point2f>> m_2DZeroPos;
	bool m_realTimeUpdate = false;
};