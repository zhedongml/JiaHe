#pragma once
#include "PrjCommon/taskManage/TaskManager.h"
#include "Result.h"
//#include "TaskImageSave.h"
//#include "TaskCalibrationYY.h"
//#include "TaskCalibrationXYZ.h"
//#include "TaskCalibrationVid.h"
#include "opencv2/opencv.hpp"

using namespace PrjCommon;

class TaskAsync: public QObject, public TaskCallback
{
	Q_OBJECT
public:
	static TaskAsync &instance(QObject* parent = nullptr);
	TaskAsync(QObject* parent = nullptr);
	~TaskAsync();

	//Result saveImage(const QString &file, cv::Mat image);

	//Result calibrationYY(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap,
	//	CalibrationConfig config,
	//	std::map<CalibrationEnum, std::map<MLFilterEnum, CaptureData>> calibrationDatas,
	//	QString fileName);

	//Result calibrationXYZ(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap, 
	//	CalibrationConfig config,
	//	std::map<ML::MLColorimeter::CalibrationEnum,
	//	std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>> calibrationDatas,
	//	QString fileName, bool saveSLBYY, bool isChroma);

	//Result calibrationVID(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap, 
	//	CalibrationConfig config, std::map<CalibrationEnum, std::map<MLFilterEnum, CaptureData>> calibrationDatas,
	//	QString fileName, bool saveImage, AutoVidCallback* callback);

	Result stopTask();
	void stopThread();
	Result waitAllTask();

	void notifyResult(Result ret) override;

	//TODO: test
	Result saveImageTest(const QString& file, cv::Mat image);

private:
	TaskManager m_managerImage;
	TaskManager m_managerCalibrate;
};

