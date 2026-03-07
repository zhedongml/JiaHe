#pragma once

#include <QObject>
#include "prjcommon_global.h"
#include <vector>
#include <map>
#include <QDateTime>
#include <QMap>
#include "opencv2/opencv.hpp"
#include "Result.h"
#include "MLUtilCommon.h"
#include <iostream>
#include <functional>
#include <memory>
#include <string>
#include <mutex>
#include <QQueue>
#include <QMutex>
#include <deque>
#include <optional>
#include <functional>
#include <QVariant>
#include <QMetaType>
#include <QHash>
#include <QList>
//using namespace std;

enum METRICS_DATA_TYPE
{
	METRICS_DATA_DUT_SN,
	METRICS_DATA_DUT_RESULT
};

enum DUT_TEST_RESULT
{
	DUT_TEST_SUCCESS,
	DUT_TEST_FAIL
};

enum DPACOLOR {
	RED = 0,
	GREEN = 1,
	BLUE = 2,
	WHITE = 3,
	CLOSE = 4
};

enum ROTATION_MIRROR_TYPE
{
	RMT_DUT_LEFT,
	RMT_DUT_RIGHT,
	RMT_SLB
};

enum DUT_MEASURE_STEP_STATUS
{
	DUT_STEP_INITIAL,
	FAIL_STEP_PLC,
	SUCCESS_STEP_PLC,
	FAIL_STEP_AA,
	SUCCESS_STEP_AA,
	FAIL_STEP_CAPTURE,
	SUCCESS_STEP_CAPTURE
};

struct ImageNameInfo
{
	QString deviceColor = "R";
	QString deviceImage = "Sloid";
	cv::Point3f eyeboxCoord = cv::Point3f(0, 0, 0);
	QString xyzFilter = "X"; // X,Y,Z
	QString ndFilter = "ND0";
	QString imgFileName;
	float focus = 0.0f;
	float diopter = 0.0f;
	bool isRawImage = true;

	ImageNameInfo()
	{
	}

	~ImageNameInfo()
	{
	}

	QString imageFileNotDevice()
	{
		return QString("X%1Y%2Z%3_EYE_%4")
			.arg(eyeboxCoord.x)
			.arg(eyeboxCoord.y)
			.arg(eyeboxCoord.z)
			.arg(xyzFilter);
	}

	QString imageFile()
	{
		return QString("result%1").arg(xyzFilter);
	}
};

struct OtherImageInfo {
	QString colorIS;                    // R,G,B
	QString reticle;                    // Clear, chessboard
	QMap<QString, float> labsphereNitsMap;  // key:R,G,B; value:labsphere nit

	void setLatestIScolor(QString color)
	{
		colorIS = color.toLower();
	}

	void setLatestReticle(QString reticle)
	{
		reticle = reticle.toLower();
	}

	void setLabsphereNitsMap(QString color, float nit) {
		labsphereNitsMap[color.toLower()] = nit;
	}
};

struct DutMeasureInfo {
	QDateTime StartTime;
	QDateTime EndTime;
	QString SN;
	QString ParentDir="";
	QString DutDir = "";
	QString AdpResultDir = "";
	QString ErrorMsg="";

	int Progress_AA = 0;
	int Progress_Capture = 0;
	int Progress_Measure = 0;
	int Progress_Entirety = 0;
	int DutIndex = 0;
	int LayerIndex = 0;
	bool Barcode_OK = false;
	bool AA_OK = false;
	bool Capture_OK = true;
	bool Metrics_OK = false;

	QString	ModelName = "AutoDP_WG";
	bool CheckEnabled = true;
	bool LiveRefresh = true;

	//DUT_MEASURE_STEP_STATUS Result = DUT_STEP_INITIAL;

	bool operator==(const DutMeasureInfo& other) const {
		return DutDir == other.DutDir;
	}

	void display() const {
		std::cout << "StartTime: " << StartTime.toString().toStdString()
			<< ", EndTime: " << EndTime.toString().toStdString()
			<< ", SN: " << SN.toStdString()
			<< ", ParentDir: " << ParentDir.toStdString()
			<< ", TrayIndex: " << LayerIndex
			<< ", DutIndex: " << DutIndex
			//<< ", Result: " << Result
			<< ", Progress_AA: " << Progress_AA
			<< ", Progress_Capture: " << Progress_Capture
			<< ", Progress_Measure: " << Progress_Measure
			<< ", Progress_Entirety: " << Progress_Entirety
			<< std::endl;
	}

	//¿½±´¸³ÖµÔËËã·û
	//DutMeasureInfo& operator=(const DutMeasureInfo& rhs)
	//{
	//	ps = make_shared<string>(*rhs.ps);
	//	StartTime = rhs.StartTime;
	//	EndTime = rhs.EndTime;
	//	SN = rhs.SN;
	//	ParentDir = rhs.ParentDir;
	//	Progress_AA = rhs.Progress_AA;
	//	Progress_Capture = rhs.Progress_Capture;
	//	Progress_Measure = rhs.Progress_Measure;
	//	Progress_Entirety = rhs.Progress_Entirety;
	//	DutIndex = rhs.DutIndex;
	//	TrayIndex = rhs.TrayIndex;
	//	Result = rhs.Result;;
	//	return *this;
	//}

	//Îö¹¹º¯Êý
	~DutMeasureInfo() = default;

	//DutMeasureInfo& operator=(const DutMeasureInfo& other) {
	//	if (this != &other) {

	//		//delete[] name; // ÊÍ·Å¾ÉÄÚ´æ
	//		//age = other.age;
	//		//name = new char[strlen(other.name) + 1];
	//		//strcpy(name, other.name);

	//		StartTime = other.StartTime;
	//		EndTime = other.EndTime;
	//		SN = other.SN;
	//		ParentDir = other.ParentDir;
	//		Progress_AA = other.Progress_AA;
	//		Progress_Capture = other.Progress_Capture;
	//		Progress_Measure = other.Progress_Measure;
	//		Progress_Entirety = other.Progress_Entirety;
	//		DutIndex = other.DutIndex;
	//		TrayIndex = other.TrayIndex;
	//		Result = other.Result;;
	//	}
	//	return *this;
	//}

//private:
//	std::shared_ptr<string> ps;
};

enum METRICS_STATE {
	METRICS_PASS,
	METRICS_FAIL
};

struct McMetricsResult {
	METRICS_STATE result = METRICS_PASS;
	QStringList failList;
	QString failStr;
};

template<typename T>
std::optional<std::reference_wrapper<const T>> peek_front_ref_safe(const std::deque<T>& dq) {
	if (dq.empty()) {
		return std::nullopt;
	}
	return std::cref(dq.front());
}

template<typename T>
std::optional<std::reference_wrapper<T>> peek_front_ref_mutable(std::deque<T>& dq) {
	if (dq.empty()) {
		return std::nullopt;
	}
	return std::ref(dq.front());
}

//template<typename T>
class PRJCOMMON_EXPORT IDataChangeListener {
public:
	IDataChangeListener();
	~IDataChangeListener();
	virtual void onDataChanged(const QHash<QString, int>& newValueMap) = 0;
};


class PRJCOMMON_EXPORT MetricsData : public QObject //DataChangeListener
{
	Q_OBJECT

public:
	~MetricsData();
	static MetricsData* instance();
	QString getEyeDirection();
	void setEyeDirection(QString dir);
	void setCaptureFormatTime(QDateTime time);
	QString getCaptureFormatTime();
	int getFiducialType();
	void setFiducialType(int type);
	QString getMTFImgsDir();
	void setMTFImgsDir(QString direct);
	QString getRecipeSeqDir();
	void setRecipeSeqDir(QString path);
	void setEyeboxCount(int count);
	int getEyeboxCount();
	void setSequenceId(QString uuid);
	QString getSequenceId();
	void setIsConnectErrorShow(bool isShow);
	bool getIsConnectErrorShow();
	void setCurDpaColor(DPACOLOR color);
	DPACOLOR getCurDpaColor();
	QString getCurDpaColorStr();
	int getFiducialCount();
	void setFiducialCount(int val);

	QString getMTFRecipeName();
	void setMTFRecipeName(QString val);

	QString getDutBarCode();
	void setDutBarCode(QString val);

	QString getDutName();
	void setDutName(QString val);

	QString getImageNDFilter(QString fileName);
	QString getImageNDFilter();
	void setImageNDFilter(QString val);

	qint64 getMTFStartTime();
	void setMTFStartTime(qint64 val);

	QString getIQRecipeName();
	void setIQRecipeName(QString val);

	QString getIQRecipeSeqDir();
	void setIQRecipeSeqDir(QString path);

	QString getHistoryImagePath();
	void setHistoryImagePath(QString path);

	// new output data dir 2025.3.7
	QString getOutputRootDir();
	void setOutputRootDir(QString val);
	QString getOutputImageDir();
	void setOutputImageDir(QString val);

	void setISPluminanceData(QString color, double luminance);
	QMap<QString, double> getISPluminanceData();
	void addConnectedDevicesNum();

	void setLuminanceEfficiencyPara(QString para);
	QString getLuminanceEfficiencyPara();
	void setLuminancePara(QString para);
	QString getLuminancePara();
	void setChrominancePara(QString para);
	QString getChrominancePara();
	void setLuminanceInfo(QString para);
	QString getLuminanceInfo();

	int getACSType();
	void setACSType(int val);

	int getMotion2DType();
	void setMotion2DType(int val);

	Result createCsvResultFile();
	Result writeImageInfoCsv(QString message);

	void setDutEyeType(int eyeType);
	int getDutEyeType();
	void setReticleEyeType(int eyeType);
	int getReticleEyeType();

	void setRotationMirrorType(ROTATION_MIRROR_TYPE type);
	ROTATION_MIRROR_TYPE getRotationMirrorType();

	QString getEyeboxQueue();
	void setEyeboxQueue(QString val);

	void setDutAngle(float dutAngle);
	float getDutAngle();

	void setAlignImageDir(QString dir);
	QString getAlignImageDir();

	void setEyeboxIndexCurrent(int index);
	int getEyeboxIndexCurrent();

	void setLuminanceFile(QString file);
	QString getLuminanceFile();

	//Result csv path
	void setCsvPath(QString csv, QString allcsv = "");
	QString getCsvPath();
	QString getAllCsvPath();
	//Dut seq name
	void setDutTestSeqName(QString name);
	QString getDutTestSeqName();
	//Seq img save dir
	void setSeqImageDir(QString dir);
	QString getSeqImageDir();
	//Recipe time
	QDateTime getStartRecipeTime();
	void setStartRecipeTime(QDateTime time);
	//Dut type state
	void SetTestState(MLUtils::TestState state);
	MLUtils::TestState GetTestState();

	//DUT SN
	QString getDutSN();
	void setDutSN(QString val);

	//DUT Name
	void setDutType(QString);
	QString getDutType();

	void setAllCsvDir(QString allcsvDir);
	QString getAllCsvDir();

	//DUT Success or Fail?
	void resetDutMeasureHash();
	void insertDutMeasureHash();
	QHash<QString, bool> getDutMeasureHash();
	void setDutMeasureResult(bool succ);
	bool getDutMeasureResult();
	//void updateDutMeasureResult();
	//void updateDutPickResult();
	//void updateDutAutoDPResult(bool pass);
	void updateDutAutoDPResult(QString ErrorMsg);
	//void updateDutAutoDPResult(QString SN, bool pass);
	//void updateDutAutoDPResult(QString SN, QString ErrorMsg);

	void updateDutProcessResult(DutMeasureInfo);
	void updateDutPickResult(DutMeasureInfo);
	void updateMetricsProgressBar(int);

	void setAutoTaskDutIndex(int);
	int getAutoTaskDutIndex();

	void setAutoTaskDutCount(int);
	int getAutoTaskDutCount();

	void updateNowStatCount(int);

	void setAutoTaskLayerCount(int value);
	int getAutoTaskLayerCount();

	void setAutoTaskDoneCount(int value);
	int getAutoTaskDoneCount();

	const int getAutoTaskTrayGridCount();
	void updateAutoTaskDoneCount();
	void resetAutoTaskDoneCount();

	void pushDutPutQueue(DutMeasureInfo);
	void pushDutMeasureQueue(DutMeasureInfo);
	void pushDutPickQueue(DutMeasureInfo);
	void pushDutMetricsQueue(DutMeasureInfo);
	bool pushDutAdpHistoryQueue(DutMeasureInfo);

	std::optional<DutMeasureInfo> popPutQueueFront();
	std::optional<DutMeasureInfo> popMeasureQueueFront();
	std::optional<DutMeasureInfo> popPickQueueFront();
	std::optional<DutMeasureInfo> popMetricsQueueFront();
	std::optional<DutMeasureInfo> popAdpHistoryQueueFront();
	//std::optional<DutMeasureInfo> popMetricsQueueBackSN(QString);

	const int getDutMeasureSize();
	const int getDutPickSize();

	std::string swapPutMeasurePickQueue();
	bool resetPutMeasurePickQueue();
	bool resetAdpHistoryQueue();

	std::string printAllQueue();
	int getDutPendingCount();
	int getDutPutPendingCount();
	int getDutUnpickCount();
	
	std::optional<std::reference_wrapper<DutMeasureInfo>> queryPutQueueFront();
	std::optional<std::reference_wrapper<DutMeasureInfo>> queryMeasureQueueFront();
	std::optional<std::reference_wrapper<DutMeasureInfo>> queryPickQueueFront();
	std::optional<std::reference_wrapper<DutMeasureInfo>> queryMetricsQueueFront();
	//std::optional<std::reference_wrapper<DutMeasureInfo>> queryMetricsQueueBack();
	std::optional<std::reference_wrapper<DutMeasureInfo>> queryAdpHistoryQueueFront();

	//bool queryMetricDutInfo(QString SN);

	void setDiopter(float diopter);
	float getDiopter();

	void setMetricsResult(McMetricsResult metricsResult);
	void setRecipeResultState(METRICS_STATE retState);
	McMetricsResult getMetricsResult();

	void setMetricsResult_autoDP(McMetricsResult metricsResult);
	void setRecipeResultState_autoDP(METRICS_STATE retState);
	McMetricsResult getMetricsResult_autoDP();

	void setHolderID(const QString& holderID);
	QString getHolderID();

	QString getRecipeStartTime();
	void setRecipeStartTime(QString val);
	QString getRecipeStartDate();
	void setRecipeStartDate(QString val);
	void setRecipeTotalSeconds(int seconds);
	int getRecipeTotalSeconds();

	QDateTime getStartEyeboxTime();
	void setStartEyeboxTime(QDateTime time);

	void updateNowLayer(int);
	int getNowLayer();

	void updateSoftRunningState();

	bool showTipsDialog(QString title, QString msg, QString yesInfo, QString noInfo);

	bool popTipsDialog(QString title, QString info, QString yesInfo, QString noInfo);

	int getExposureMode();
	void setExposureMode(int mode);

	void clearDutGridStatus();

	void clearHistoryDoneList();
	
public slots:
	void appendDutPickResult(const QList<QVector<QString>> list);
	void appendDutTestResult(const QList<QVector<QString>> list);

	void addListener(std::shared_ptr<IDataChangeListener> listener) {
		std::lock_guard<std::mutex> lock(mutex_);
		listeners_.push_back(listener);
	}

	void removeListener(std::shared_ptr<IDataChangeListener> listener) {
		std::lock_guard<std::mutex> lock(mutex_);
		listeners_.erase(
			std::remove(listeners_.begin(), listeners_.end(), listener),
			listeners_.end()
		);
	}

signals:
	void devicesNumChanged(int);
	//void sig_appendDutTestResult(const QList<QPair<QString, bool>> list);
	void sig_appendDutPickResult(QList<QHash<QString, QVariant>>);
	void sig_appendDutTestResult(const QList<QVector<QString>> list);
	void sig_updateMetricsProgressBar(int, bool);
	void sig_dutProcessEnd(DutMeasureInfo info);
	void sig_dutPickEnd(DutMeasureInfo info);
	void sig_resetGridStatus();
	void sig_updateNowLayer(int);
	void sig_updateSoftRunningState();
	void sig_clearHistoryDoneList();
	//void sig_DutTestDone(bool);

private:
	Result writeToCSV(QString filename, QString msg, QString errorMsg);
	bool updatePutMeasureQueue();
	bool updateMeasurePickQueue();
	//bool updatePickMetricsQueue();
	QString printQueueInfo(QString type, std::deque<DutMeasureInfo> data);


public:
	QString m_eyeboxQueue;

	// other info: Integrating sphere, Reticle
	//OtherImageInfo m_otherImageInfo;

private:
	MetricsData();
	static MetricsData* self;

	QString eyeDirection = "Left";
	QString mtfimgsDir = "";
	int fiducialType = 1;
	QString recipeSeqDir = "d:";
	QString seqId = "";
	int eyeboxCount = 9;
	DPACOLOR curdpacolor = DPACOLOR::CLOSE;
	int fiducialCount = 2;
	QString recipeName = "";
	QString dutId = "";
	QString dutName = "";
	QString iqRecipeName = "";
	qint64 mtfStartTime;
	QString iqRecipeDir = "";
	int devicesNum = 0;
	QMap<QString, double> m_ISPluminance;

	QString luminanceEfficiencyPara = "";
	QString luminancePara = "";
	QString chrominancePara = "";
	QString luminanceInfo = "";
	QString formatTime;
	QString dutSN = "";
	QString dutType = "";

	int ACSType = 0;
	int Motion2DType = 0;
	bool isConnectErrorShow = true;
	const QString m_imageInfoFileName = "Image info.csv";

	QDateTime startTime;

	int m_dutEyeType = RMT_DUT_RIGHT;
	ROTATION_MIRROR_TYPE m_rotationMirrorType = RMT_SLB;
	int m_reticleEyeType = RMT_DUT_LEFT;

	bool m_IQSLB = false;
	bool m_MeasureResult = false;
	int m_ExposureMode = 0;
	float m_dutAngle = 0.0f;

	QString m_alignImageDir;

	QString m_outputRootDir;
	QString m_outputImageDir;
	int m_eyeboxIndexCurrent;
	int m_AutoTaskDutCount = 0;
	int m_AutoTaskLayerCount = 0;
	int m_AutoTaskDutIndex = 0;
	int m_AutoTaskDoneCount = 0;
	int m_NowLayerNum = 0;

	QString m_luminanceFile;
	QString m_imageNDFilter;
	QString m_historyImagePath;

	QString m_allcsvDir;
	QString m_allcsvpath;
	QString m_csvpath;
	QString m_dutTestSeqName;
	QString m_seqImgDir;
	QDateTime m_startRecipeTime;
	MLUtils::TestState m_TestState;

	QHash<QString, bool> m_DutMeasureHash;
	std::deque<DutMeasureInfo> m_DutPutQueue;
	std::deque<DutMeasureInfo> m_DutMeasureQueue;
	std::deque<DutMeasureInfo> m_DutPickQueue;
	std::deque<DutMeasureInfo> m_DutMetricsQueue;

	McMetricsResult m_metricsResult;
	McMetricsResult m_metricsResult_autoDP;

	// image name info
	ImageNameInfo m_imageNameInfo;

	QString m_holderID;
	int m_recipeTotalSeconds = 0;

	QString recipeStartTime;
	QString recipeStartDate;

private:
	//T value_;
	QMutex m_PutMeasureMutex;
	QMutex m_MeasurePickMutex;
	
	std::vector<std::shared_ptr<IDataChangeListener>> listeners_;
	mutable std::mutex mutex_;

	void notifyListeners(const QHash<QString,int>& newValueMap) {
		std::vector<std::shared_ptr<IDataChangeListener>> listenersCopy;
		{
			std::lock_guard<std::mutex> lock(mutex_);
			listenersCopy = listeners_;
		}

		for (auto& listener : listenersCopy) {
			if (listener) {
				listener->onDataChanged(newValueMap);
			}
		}
	}
};
