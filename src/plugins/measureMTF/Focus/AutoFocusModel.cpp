#include "AutoFocusModel.h"
#include "Core/loggingwrapper.h"
#include "FocusConfig.h"
#include "ML_denseMTF.h"
#include <QMutex>

AutoFocusModel* AutoFocusModel::m_instance = nullptr;

AutoFocusModel* AutoFocusModel::instance(QObject* parent)
{
	if (!m_instance)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		if (!m_instance)
		{
			m_instance = new AutoFocusModel(parent);
		}
	}
	return m_instance;
}

AutoFocusModel::AutoFocusModel(QObject* parent)
{
	init();
}

AutoFocusModel::~AutoFocusModel()
{
	if (m_AFProcess)
	{
		delete m_AFProcess;
		m_AFProcess = nullptr;
	}
}

void AutoFocusModel::init()
{
	MLMonoBusinessManage* monocular = MLColorimeterMode::Instance()->GetMonocular();
	if (monocular)
	{
		m_AFProcess = new MLDiopterScan(monocular);
	}
}

Result AutoFocusModel::DiopterScanProcess(DiopterScanConfig config, std::string file_name, std::string direction)
{
	std::string message = "DiopterScanProcess diopter scan " +
		(direction.empty() ? "" : direction + " direction ");

	if (!MLColorimeterMode::Instance()->IsConnect())
		return Result(false, message +"failed, measure camera is not connected!");

	if(!m_AFProcess)
		return Result(false, message + "failed, mldiopterscan ptr is nullptr!");

	// Algo obtains MTF calculation rois
	MLMonoBusinessManage* monocular = MLColorimeterMode::Instance()->GetMonocular();
	Result ret = monocular->ML_CaptureImageSync();
	if (!ret.success)
		return Result(false, message + "failed, " + ret.errorMsg);
	CaptureData data = monocular->ML_GetCaptureData();
	cv::Mat image = data.Img;
	std::vector<cv::Rect> rois;
	ret = GetMTFROIs_Grid(file_name, rois, image);
	if (!ret.success)
		return Result(false, message + "failed, " + ret.errorMsg);
	config.ROIs = rois;
	
	std::string prefix;
	ret = m_AFProcess->ML_DiopterScan("CameraMotion", file_name, config);
	if (config.StoreResultImg)
	{
		m_AFProcess->WaitForTasksFinish(20);
	}
	if (!ret.success)
	{
		return Result(false, message + "failed, " + ret.errorMsg);
	}
	cv::Point3f result = { 0,0,0 };
	if (!direction.empty())
	{
		std::map<std::string, std::vector<int>> indexMap{
	{"H",   {0, 1}},
	{"V",   {2, 3}},
	{"AVG", {0, 1, 2, 3}}
		};

		auto it = indexMap.find(direction);
		if (it == indexMap.end())
			return Result(false, message + "failed, invalid direction: " + direction);

		Result ret = m_AFProcess->ML_CalculateCombinMap(direction, it->second, config.IsUseFineAdjust);
		if (!ret.success)
		{
			LoggingWrapper::instance()->warn(QString::fromStdString(message + "failed,get diopterScan " + direction + " result error, "
				+ ret.errorMsg));
			return Result(false, message + "failed, " + ret.errorMsg);
		}
		std::map<string, cv::Point3f> resultmap = m_AFProcess->ML_GetDiopterScanHVAVGReMap();
		if (!resultmap.count(direction))
			return Result(false, message + "failed, result map is not contain " + direction);
		result = resultmap[direction];
	}
	else
	{
		std::vector<cv::Point3f> reVec = m_AFProcess->ML_GetDiopterScanResult();
		result = reVec[0];
	}
	if (result.z == 0)
		return Result(false, message + "failed, best mtf result=0.");
	if (!file_name.empty())
	{
		ret = m_AFProcess->ML_SaveDiopterScanResult(file_name, prefix);
		if (!ret.success)
			return Result(false, message + "failed, " + ret.errorMsg);
	}
	ret = MLColorimeterMode::Instance()->SetVidSync(result.y);
	if (!ret.success)
		return Result(false, message + "failed, " + ret.errorMsg);
	return ret;
}

Result AutoFocusModel::FocusScanProcess(FocusScanConfig config, std::string file_name, std::string direction)
{
	std::string message;
	qint64 startTime = QDateTime::currentMSecsSinceEpoch();
	if (!MLColorimeterMode::Instance()->IsConnect())
	{
		return Result(false, "FocusScanProcess "+ direction +" direction failed, measure camera is not connected!");
	}

	std::string prefix;
	Result ret = MLColorimeterMode::Instance()->GetMonocular()->ML_FocusScan("CameraMotion", config);
	if (!ret.success)
	{
		return Result(false, "FocusScanProcess " + direction + " direction failed, " + ret.errorMsg);
	}
	double position = DBL_MAX;
	std::vector<cv::Point3f> result = MLColorimeterMode::Instance()->GetMonocular()->ML_GetFocusScanResult();
	if (result.size() == 0)
	{
		return Result(false, "FocusScanProcess " + direction + " direction failed, rois result is empty.");
	}

	LoggingWrapper::instance()->info(QString::fromStdString("FocusScanProcess " + direction + " direction failed, position=" 
		+ to_string(result[0].x)+", vid=" + to_string(result[0].y)
			+ ", mtf=" + to_string(result[0].z)));

	ret = MLColorimeterMode::Instance()->SetFocusMotionPosSync(result[0].x);
	if (!ret.success)
	{
		return Result(false, "FocusScanProcess " + direction + " direction failed, " + ret.errorMsg);
	}
	qint64 takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
	qWarning() << "---- FocusScanProcess ----" << " times:" << takeTime << " ms...";
	return ret;
}

void AutoFocusModel::StopTreeSystemCallback(bool isStopTreeSystem)
{
	if (isStopTreeSystem)
		MLColorimeterMode::Instance()->GetMonocular()->StopThroughFocus();
}

Result AutoFocusModel::SetFocusMotionByLight(QString lightsource)
{
	if (lightsource.isEmpty()) {
		return Result(false, "light source is empty.");
	}
	float pos = FocusConfig::GetInstance().GetLightFocus(lightsource.toStdString());
	if (pos == 0)
		return Result(false, "config is not contained "+ lightsource.toStdString());
	return MLColorimeterMode::Instance()->SetFocusMotionPosSync(pos);
}

Result AutoFocusModel::GetMTFROIs_Grid(std::string file_path, vector<cv::Rect>& rois, const cv::Mat& image)
{
	if (image.empty())
	{
		return Result(false, "algo get mtf rois error, image is empty.");
	}
	try
	{
		MLIQMetrics::MLdenseMTF densemtf;
		MLIQMetrics::CenterRects re = densemtf.getGridCenterRects(image);
		if (!re.flag)
		{
		    cv::imwrite(file_path + "/through_focus_raw.tif", image);
			return Result(false, "algo get mtf rois error, " + re.errMsg);
		}
		cv::imwrite(file_path + "/through_focus_result.tif", re.imgdraw);
		rois = re.rectVec;
		return Result();
	}
	catch (exception e)
	{
		return Result(false, string("algo get mtf rois is exception, ") + e.what());
	}
}

std::vector<cv::Point3f> AutoFocusModel::GetDiopterScanResult()
{
	return m_AFProcess->ML_GetDiopterScanResult();
}

MLDiopterScan* AutoFocusModel::GetDiopterScanManage()
{
	return m_AFProcess;
}