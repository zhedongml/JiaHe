#include "MLColorimeterMode.h"
#include "MLColorimeterHelp.h"
#include "IQMetrics/ML_CrossMTF.h"
#include "ml_multiCrossHairDetection.h"
#include "IQMetricUtl.h"
#include "Core/loggingwrapper.h"
#include "PrjCommon/metricsdata.h"
#include <QThread>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QFutureSynchronizer>
#include <QMessageBox>
#include <QCoreApplication>
#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;
using namespace MLImageDetection;

MLColorimeterMode* MLColorimeterMode::self = nullptr;

MLColorimeterMode::MLColorimeterMode(QObject* parent) : QObject(parent)
{
    qRegisterMetaType<cv::Mat>("cv::Mat");
    m_bino = new MLBinoBusinessManage(this);
    Result res = InitConfig();
    if (!res.success)
    {
        QMessageBox::warning(NULL, "Error", QString::fromStdString(res.errorMsg), QMessageBox::Yes);
    }
}

MLColorimeterMode::~MLColorimeterMode()
{
    if (m_bino != nullptr)
    {
        m_bino->ML_DisconnectModules();
        delete m_bino;
        m_bino = nullptr;
    }
    if (m_taskManager != nullptr)
    {
        delete m_taskManager;
        m_taskManager = nullptr;
    }
}

MLColorimeterMode* MLColorimeterMode::Instance(QObject* parent)
{
    if (!self)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self)
        {
            self = new MLColorimeterMode(parent);
        }
    }
    return self;
}

Result MLColorimeterMode::Connect()
{
    try
    {
        int startTime = QDateTime::currentMSecsSinceEpoch();
        LoggingWrapper::instance()->info("colorimeter connect start.");
        m_IsConnected = false;
        Result ret;
        if (m_bino->ML_IsModulesConnect())
        {
            LoggingWrapper::instance()->info("colorimeter is connected.");
            m_IsConnected = true;
            return Result();
        }

        ret = m_bino->ML_ConnectModules();
		if (!ret.success)
		{
            return Result(false, "colorimeter module connection failed. " + ret.errorMsg);
		}
		m_IsConnected = true;
		ret = initAfterConnect();
        if (!ret.success)
        {
            return ret;
        }

        emit connectStatus(true);

        int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
        LoggingWrapper::instance()->debug(QString("colorimeter connect time: %1 ms.").arg(takeTime));
        return Result();
    }
    catch (exception e)
    {
        return Result(false, e.what());
    }
}

Result MLColorimeterMode::ConnectAsync() {
    Result res;
    QList<QFuture<Result>> futures;
    QFutureSynchronizer<Result> synchronizer;
    MLMonoBusinessManage* mono = GetMonocular();
    if (mono->ML_IsModuleConnect())
    {
        LoggingWrapper::instance()->info("colorimeter is connected.");
        m_IsConnected = true;
        return Result();
    }
    res = mono->ML_CreateModule();
    if (!res.success)
    {
        return res;
    }

	futures.append(QtConcurrent::run([=]() {
		Result ret = mono->ConnectFilterWheel();
		if (!ret.success)
			return ret;
		return mono->ConnectRXFilterWheel();
		}));

	futures.append(QtConcurrent::run([=]() { return mono->ConnectCamera(); }));
	futures.append(QtConcurrent::run([=]() { return mono->ConnectMotion(); }));
	futures.append(QtConcurrent::run([=]() { return mono->ConnectReflector(); }));
	futures.append(QtConcurrent::run([=]() { return mono->ConnectSpectrometer(); }));
	futures.append(QtConcurrent::run([=]() { return mono->ConnectSpecbos(); }));

    for (auto& future : futures) {
        synchronizer.addFuture(future);
    }
    synchronizer.waitForFinished();
    std::string errmsg;
    m_IsConnected = true;
    for (const auto& future : futures) {
        Result res = future.result();
        if (!res.success) {
            m_IsConnected = false;
            errmsg = res.errorMsg;
            break;
        }
    }

    if (!m_IsConnected)
    {
        return Result(false, "colorimeter module connection failed. " + errmsg);
    }

    //m_taskManager = new MLTaskManager(m_bino);
    Result ret = initAfterConnect();
    if (!ret.success)
    {
        return ret;
    }

    emit connectStatus(true);
    return Result();
}

void MLColorimeterMode::parseConfigPathPointer(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Unable to open the file for reading.";
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();

        line = line.trimmed();
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }

        int colonPos = line.indexOf('=');
        if (colonPos != -1) {
            QString key = line.left(colonPos).trimmed();
            QString value = line.mid(colonPos + 1).trimmed();

            if (key == "isValid") {
                m_isConfigPtr = value.toLower() == "true";
            }
            else if (key == "pointPath") {
                m_PointConfigPath = value;
            }
            else {
                qDebug() << "Unknown key:" << key << "with value:" << value;
            }
        }
    }

    file.close();
}


Result MLColorimeterMode::InitConfig()
{
    try {
        QString configPth = QCoreApplication::applicationDirPath() + "/config";
        parseConfigPathPointer(configPth + "/ConfigPathPointer.txt");

        if (m_isConfigPtr) {
            CONFIG_PATH = m_PointConfigPath;
        }

        if (m_bino->ML_GetModulesIDList().size() == 0) {
            static QMutex mutex;
            QMutexLocker locker(&mutex);

            if (m_bino->ML_GetModulesIDList().size() == 0) {
                QDir dir(CONFIG_PATH);
                if (!dir.exists())
                {
                    LoggingWrapper::instance()->warn("MLColorimeterMode: Directory does not exist:" + CONFIG_PATH);
                    return Result(false, "MLColorimeterMode: Directory does not exist:" + CONFIG_PATH.toStdString());
                }

                Result ret;
                QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
                for (const QString& subDir : subDirs)
                {
                    QString subDirAbs = dir.absoluteFilePath(subDir);
                    ret = m_bino->ML_AddModule(subDirAbs.toStdString().c_str());

                    if (!ret.success)
                    {
                        return Result(false, "MLColorimeterMode: Init colorimeter config failed, " + ret.errorMsg);
                    }
                }
            }
        }
        m_taskManager = new MLTaskManager(m_bino);
        if (!m_taskManager)
        {
            return Result{ false,"InitConfig error,task manager is nullptr" };
        }
        return Result();
    }
    catch (exception e)
    {
        return Result(false, QString("colorimeter init config error, %1").arg(e.what()).toStdString());
    }
}

Result MLColorimeterMode::Disconnect()
{
    if(!IsConnect())
        return Result();
    Result ret = m_bino->ML_DisconnectModules();
    if (!ret.success)
    {
        return Result(false, "colorimeter disconnect failed, " + ret.errorMsg);
    }
    m_IsConnected = false;
    emit connectStatus(false);
    return Result();
}

bool MLColorimeterMode::IsConnect()
{
    return m_IsConnected;
}

MLMonoBusinessManage* MLColorimeterMode::GetMonocular()
{
	if (m_bino)
	{
		std::vector<int> ids = m_bino->ML_GetModulesIDList();
		if (ids.size() == 0) {
			return nullptr;
		}
		return m_bino->ML_GetModuleByID(ids.at(0));
	}
	else {
		return nullptr;
	}
}

GrabberBase* MLColorimeterMode::GetCamera()
{
    if (GetMonocular() == nullptr)
    {
        return nullptr;
    }
    MLMonoBusinessManage* monocular = GetMonocular();
    GrabberBase* grabberBase = static_cast<GrabberBase*>(monocular->ML_GetOneModuleByName(CAMERA_KEY));
    return grabberBase;
}

ActuatorBase* MLColorimeterMode::GetCameraMotion()
{
    if (GetMonocular() == nullptr)
    {
        return nullptr;
    }

    MLMonoBusinessManage* monocular = GetMonocular();
    ActuatorBase* base = static_cast<ActuatorBase*>(monocular->ML_GetOneModuleByName(FOCUS_KEY));
    return base;
}

MLBinoBusinessManage* MLColorimeterMode::GetBinocular()
{
    return m_bino;
}

ML_Task::MLTaskManager* MLColorimeterMode::GetCaptureTaskManager()
{
    return m_taskManager;
}

//ND XYZ Filter Interface
Result MLColorimeterMode::SetNDFilter(QString nd)
{
    MLFilterEnum ndEnum = MLColorimeterHelp::instance()->TransStrToFilterEnum(nd.toStdString());
    return SetNDFilter(ndEnum);
}

Result MLColorimeterMode::SetNDFilter(MLFilterEnum nd)
{
    if (!IsConnect())
    {
        return Result(false, "Set nd filter failed, colorimeter is not connected.");
    }
    Result ret = GetMonocular()->ML_MoveND_XYZFilterByEnumSync(ND_FILTER_KEY, nd);
    if (!ret.success)
    {
        return Result(false, "Set nd filter failed." + ret.errorMsg);
    }
    return Result();
}

Result MLColorimeterMode::SetColorFilter(QString xyz)
{
    MLFilterEnum xyzEnum = MLColorimeterHelp::instance()->TransStrToFilterEnum(xyz.toStdString());
    return SetColorFilter(xyzEnum);
}

Result MLColorimeterMode::SetColorFilter(MLFilterEnum xyzEnum)
{
    if (!IsConnect())
    {
        return Result(false, "Set XYZ filter failed, colorimeter is not connected.");
    }
    Result ret = GetMonocular()->ML_MoveND_XYZFilterByEnumSync(XYZ_FILTER_KEY, xyzEnum);
    if (!ret.success)
    {
        return Result(false, "Set XYZ filter failed." + ret.errorMsg);
    }
    return Result();
}

Result MLColorimeterMode::SetNDFilterAsyn(QString nd)
{
    if (!IsConnect())
    {
        return Result(false, "Set ND Filter ASYN failed, colorimeter is not connected.");
    }

    MLFilterEnum ndEnum = MLColorimeterHelp::instance()->TransStrToFilterEnum(nd.toStdString());
    Result ret = GetMonocular()->ML_MoveND_XYZFilterByEnumAsync(ND_FILTER_KEY, ndEnum);
    if (!ret.success)
    {
        return Result(false, "Asyn set nd filter failed." + ret.errorMsg);
    }
    return Result();
}

Result MLColorimeterMode::SetXYZFilterAsyn(QString xyz)
{
    if (!IsConnect())
    {
        return Result(false, "Set XYZ Filter ASYN failed, colorimeter is not connected.");
    }

    MLFilterEnum xyzEnum = MLColorimeterHelp::instance()->TransStrToFilterEnum(xyz.toStdString());
    Result ret = GetMonocular()->ML_MoveND_XYZFilterByEnumAsync(XYZ_FILTER_KEY, xyzEnum);
    if (!ret.success)
    {
        return Result(false, "Asyn set XYZ filter failed." + ret.errorMsg);
    }
    return Result();
}

QString MLColorimeterMode::GetNDFilter()
{
    std::string nd = MLColorimeterHelp::instance()->TransFilterEnumToStr(GetNDFilterEnum());
    return QString::fromStdString(nd);
}

MLFilterEnum MLColorimeterMode::GetNDFilterEnum()
{
    if (IsConnect())
    {
        MLFilterEnum ndEnum = GetMonocular()->ML_GetND_XYZFilterChannel(ND_FILTER_KEY);
        return ndEnum;
    }
    return MLFilterEnum();
}

QString MLColorimeterMode::GetXYZFilter()
{
    std::string xyz = MLColorimeterHelp::instance()->TransFilterEnumToStr(GetXYZFilterEnum());
    return QString::fromStdString(xyz);
}

MLFilterEnum MLColorimeterMode::GetXYZFilterEnum()
{
    if (IsConnect())
    {
        MLFilterEnum xyzEnum = GetMonocular()->ML_GetND_XYZFilterChannel(XYZ_FILTER_KEY);
        return xyzEnum;
    }
    return MLFilterEnum();
}

Result MLColorimeterMode::ClearAlarmFilter()
{
    if (!IsConnect())
    {
        return Result(false, "Motion filter clear alarm failed, colorimeter is not connected.");
    }

    MLMonoBusinessManage* monocular = GetMonocular();
    if (isEnabled(ND_FILTER_KEY))
    {
        FilterWheelBase* grabberBaseND =
            static_cast<FilterWheelBase*>(monocular->ML_GetOneModuleByName(ND_FILTER_KEY));
        pluginException ex = grabberBaseND->ML_ClearAlarm();
        if (!ex.getStatusFlag())
        {
            std::string message = "ND filter clear alarm error, " + ex.getExceptionMsg();
            LoggingWrapper::instance()->error(QString::fromStdString(message));
            return Result(false, message);
        }
    }

    if (isEnabled(XYZ_FILTER_KEY))
    {
        FilterWheelBase* grabberBaseXYZ =
            static_cast<FilterWheelBase*>(monocular->ML_GetOneModuleByName(XYZ_FILTER_KEY));
        pluginException ex = grabberBaseXYZ->ML_ClearAlarm();
        if (!ex.getStatusFlag())
        {
            std::string message = "XYZ filter clear alarm error, " + ex.getExceptionMsg();
            LoggingWrapper::instance()->error(QString::fromStdString(message));
            return Result(false, message);
        }
    }

    Result ret = ClearAlarmRx();
    return ret;
}

//RX Interface
Result MLColorimeterMode::SetRXAsync(RXCombination rx)
{
    if (!IsConnect())
    {
        return Result(false, "Set RX async failed, colorimeter is not connected.");
    }
    Result ret = GetMonocular()->ML_SetRXAsync(rx);
    if (!ret.success)
    {
        return Result(false, "Set RX async failed." + ret.errorMsg);
    }
    return Result();
}

Result MLColorimeterMode::SetRXSync(RXCombination rx)
{
    if (!IsConnect())
    {
        return Result(false, "Set RX sync failed, colorimeter is not connected.");
    }
    Result ret = GetMonocular()->ML_SetRXSync(rx);
    if (!ret.success)
    {
        return Result(false, "Set RX sync failed." + ret.errorMsg);
    }
    return Result();
}

Result MLColorimeterMode::SetCylinder(QString cylinder, int axis)
{
    if (!IsConnect())
    {
        return Result(false, "Set Cylinder failed, colorimeter is not connected.");
    }
    Result ret = GetMonocular()->ML_MoveRXFilterByNameSync(cylinder.toStdString(), axis);
    if (!ret.success)
    {
        return Result(false, "Set Cylinder failed." + ret.errorMsg);
    }
    return Result();
}

ML::MLColorimeter::RXCombination MLColorimeterMode::GetRX()
{
    if (!IsConnect())
    {
        return RXCombination();
    }
    return GetMonocular()->ML_GetRX();
}

Result MLColorimeterMode::ClearAlarmRx()
{
    if (!IsConnect())
    {
        return Result(false, "Cylindry mirror clear alarm failed, colorimeter is not connected.");
    }

    if (!isEnabled(RX_KEY))
    {
        return Result();
    }

    MLMonoBusinessManage* monocular = GetMonocular();
    RxFilterWheelBase* grabberBase = static_cast<RxFilterWheelBase*>(monocular->ML_GetOneModuleByName(RX_KEY));
    pluginException ex = grabberBase->ML_ClearAlarm();
    if (!ex.getStatusFlag())
    {
        std::string message = "Cylindry mirror revolution clear alarm error, " + ex.getExceptionMsg();
        LoggingWrapper::instance()->error(QString::fromStdString(message));
        return Result(false, message);
    }
    ex = grabberBase->ML_ClearAxisAlarm();
    if (!ex.getStatusFlag())
    {
        std::string message = "Cylindry mirror rotation clear alarm error, " + ex.getExceptionMsg();
        LoggingWrapper::instance()->error(QString::fromStdString(message));
        return Result(false, message);
    }
    return Result();
}

//Focus Motion Interface
Result MLColorimeterMode::SetFocusMotionPosAsync(double position)
{
    if (!IsConnect())
    {
        return Result(false, "Set focus ASYN failed, colorimeter is not connected.");
    }

    Result ret = GetMonocular()->ML_SetPositionAbsAsync(FOCUS_KEY, position);

    if (!ret.success)
    {
        return Result(false, "Asyn set focus failed." + ret.errorMsg);
    }
    return Result();
}

Result MLColorimeterMode::SetFocusMotionPosSync(double position)
{
    if (!IsConnect())
    {
        return Result(false, "Set focus ASYN failed, colorimeter is not connected.");
    }

    Result ret = GetMonocular()->ML_SetPositionAbsSync(FOCUS_KEY, position);

    if (!ret.success)
    {
        return Result(false, "Sync set focus failed." + ret.errorMsg);
    }
    return Result();
}

Result MLColorimeterMode::SetFocusMotionPosRelAsync(double position)
{
    if (!IsConnect())
    {
        return Result(false, "Set focus ASYN failed, colorimeter is not connected.");
    }

    Result ret = GetMonocular()->ML_SetPositionAbsAsync(FOCUS_KEY, position);

    if (!ret.success)
    {
        return Result(false, "Asyn set focus failed." + ret.errorMsg);
    }
    return Result();
}

Result MLColorimeterMode::SetFocusMotionPosRelSync(double position)
{
    if (!IsConnect())
    {
        return Result(false, "Set focus ASYN failed, colorimeter is not connected.");
    }

    Result ret = GetMonocular()->ML_SetPositionAbsSync(FOCUS_KEY, position);

    if (!ret.success)
    {
        return Result(false, "Sync set focus failed." + ret.errorMsg);
    }
    return Result();
}

Result MLColorimeterMode::SetVidSync(double vid)
{
    if (!IsConnect())
    {
        return Result(false, "Set vid ASYN failed, colorimeter is not connected.");
    }

    Result ret = GetMonocular()->ML_SetVIDSync(vid);

    if (!ret.success)
    {
        return Result(false, "Sync set vid failed." + ret.errorMsg);
    }
    return Result();
}

Result MLColorimeterMode::SetVidAsync(double vid)
{
    if (!IsConnect())
    {
        return Result(false, "Set vid ASYN failed, colorimeter is not connected.");
    }

    Result ret = GetMonocular()->ML_SetVIDAsync(vid);

    if (!ret.success)
    {
        return Result(false, "Asyn set vid failed." + ret.errorMsg);
    }
    return Result();
}

Result MLColorimeterMode::SetSphereSync(double sphere)
{
    if (!IsConnect())
    {
        return Result(false, "Set sphere sync failed, colorimeter is not connected.");
    }

    Result ret = GetMonocular()->ML_SetSphericalSync(sphere);

    if (!ret.success)
    {
        return Result(false, "Set sphere sync failed," + ret.errorMsg);
    }
    return Result();
}

Result MLColorimeterMode::SetSphereAsync(double sphere)
{
    if (!IsConnect())
    {
        return Result(false, "Set sphere async failed, colorimeter is not connected.");
    }

    Result ret = GetMonocular()->ML_SetSphericalSync(sphere);

    if (!ret.success)
    {
        return Result(false, "Set sphere async failed," + ret.errorMsg);
    }
    return Result();
}

double MLColorimeterMode::GetVid()
{
    if (!IsConnect())
    {
        return DBL_MAX;
    }

    return GetMonocular()->ML_GetVID();
}

double MLColorimeterMode::GetFocusMotionPos()
{
    if (!IsConnect())
    {
        return DBL_MAX;
    }

    return GetMonocular()->ML_GetMotionPosition(FOCUS_KEY);
}

cv::Point3f MLColorimeterMode::TransVidToFocusMotionSph(double vid)
{
    MotionConfig motionConfig = GetCameraMotionConfig();
    if (vid == 0)
    {
        return cv::Point3f(FLT_MAX, FLT_MAX, FLT_MAX);
    }
    double motionPos;
    if (motionConfig.IsReverse)
    {
        motionPos = motionConfig.FocalLength * motionConfig.FocalLength /
            (vid - motionConfig.FocalPlanesObjectSpace) +
            motionConfig.InfinityPosition;
    }
    else
    {
        motionPos = -(motionConfig.FocalLength * motionConfig.FocalLength) /
            (vid + motionConfig.FocalPlanesObjectSpace) +
            motionConfig.InfinityPosition;
    }
    double sphere = 1000 / vid;
    return cv::Point3f(sphere, vid, motionPos);
}

cv::Point3f MLColorimeterMode::TransSphToFocusMotionVid(double sphere)
{
    MotionConfig motionConfig = GetCameraMotionConfig();
	if (sphere == 0)
    {
        return cv::Point3f(sphere, FLT_MAX, motionConfig.InfinityPosition);
    }
    double vid = -1000 / sphere;
    double motionPos;
    if (motionConfig.IsReverse)
    {
        motionPos = motionConfig.FocalLength * motionConfig.FocalLength /
            (vid - motionConfig.FocalPlanesObjectSpace) +
            motionConfig.InfinityPosition;
    }
    else
    {
        motionPos = -(motionConfig.FocalLength * motionConfig.FocalLength) /
            (vid + motionConfig.FocalPlanesObjectSpace) +
            motionConfig.InfinityPosition;
    }
    return cv::Point3f(sphere, vid, motionPos);
}

cv::Point3f MLColorimeterMode::TransFocusMotionToSphVid(double position)
{
    MotionConfig motionConfig = GetCameraMotionConfig();
	if (abs(position - motionConfig.InfinityPosition) < 0.01)
	{
		return cv::Point3f(0.0, FLT_MAX, position);
	}
    double vid;
    if (motionConfig.IsReverse)
    {
        vid = -motionConfig.FocalLength * motionConfig.FocalLength /
            (motionConfig.InfinityPosition - position) +
            motionConfig.FocalPlanesObjectSpace;
    }
    else
    {
        vid = motionConfig.FocalLength * motionConfig.FocalLength /
            (motionConfig.InfinityPosition - position) -
            motionConfig.FocalPlanesObjectSpace;
    }
    double sphere = -1000 / vid;
    return cv::Point3f(sphere, vid, position);
}

//Camera Interface
bool MLColorimeterMode::IsGrabbing()
{
    GrabberBase* grabberBase = GetCamera();
    if (grabberBase == nullptr || !grabberBase->IsOpened())
    {
        return false;
    }
    return grabberBase->IsGrabbing();
}

Result MLColorimeterMode::StopGrabbing(bool stop)
{
    if (!IsConnect())
    {
        return Result(false, "Stop grabbing failed, colorimeter is not connected.");
    }

    GrabberBase* grabberBase = GetCamera();
    if (grabberBase == nullptr || !grabberBase->IsOpened())
    {
        return Result(false, "Measure camera stop/start grabbing failed, camera is not connected.");
    }

    if (stop)
    {
        grabberBase->StopGrabbing();
    }
    else
    {
        grabberBase->StartGrabbingAsync();
    }

    emit stopGrab(stop);
    return Result();
}

Result MLColorimeterMode::SetExposure(bool is_auto_et, double et, double et_factor)
{
    MLMonoBusinessManage* mono = GetMonocular();
	ExposureMode mode = is_auto_et == 1 ? ExposureMode::Auto : ExposureMode::Fixed;
	ExposureSetting targetSetting = { mode ,et };
	if (et_factor != 1)
	{
		if (mode == ExposureMode::Auto) {
            Result ret;
            ML::CameraV2::MLPixelFormat curFormat = mono->ML_GetPixelFormat();
            if (curFormat != MLPixelFormat::MLMono12)
            {
                ret = mono->ML_SetPixelFormat(MLPixelFormat::MLMono12);
                if (!ret.success)
                    return ret;
            }
			Result res = mono->ML_SetExposure({ ExposureMode::Auto ,et });
            ret = mono->ML_SetPixelFormat(curFormat);
			if (!res.success) {
                return Result(false, res.errorMsg + "," + ret.errorMsg);
			}
			double et = mono->ML_GetExposureTime();
			targetSetting = { ExposureMode::Fixed, et * et_factor };
		}
		else
			targetSetting.ExposureTime *= et_factor;
	}
	return mono->ML_SetExposure(targetSetting);
}

Result MLColorimeterMode::TransExposureSetting(ExposureMode mode, double et_factor, ExposureSetting& setting)
{
    if (qFuzzyCompare(et_factor, 1.0))
        return Result();

    MLMonoBusinessManage* mono = GetMonocular();
	if (mode == ExposureMode::Auto) {
		Result ret;
		ML::CameraV2::MLPixelFormat curFormat = mono->ML_GetPixelFormat();
		if (curFormat != MLPixelFormat::MLMono12)
		{
			ret = mono->ML_SetPixelFormat(MLPixelFormat::MLMono12);
			if (!ret.success)
				return ret;
		}
		Result res = mono->ML_SetExposure({ ExposureMode::Auto ,setting.ExposureTime });
		ret = mono->ML_SetPixelFormat(curFormat);
		if (!res.success) {
			return Result(false, res.errorMsg + "," + ret.errorMsg);
		}
		double et = mono->ML_GetExposureTime();
		setting = { ExposureMode::Fixed, et * et_factor };
	}
	else
	{
        if (setting.ExposureTime <= 0)
        {
			double exposure = mono->ML_GetExposureTime();
			setting.ExposureTime = exposure * et_factor;
		}
		else
			setting.ExposureTime *= et_factor;
	}
	return Result();
}

Result MLColorimeterMode::SetExposureTime(double ms)
{
    if (!IsConnect())
    {
        return Result(false, "Set exposure time failed, colorimeter is not connected.");
    }

    GrabberBase* grabberBase = GetCamera();
    grabberBase->SetExposureTime(ms * 1000.0);
    return Result();
}

Result MLColorimeterMode::SetExposureTime(ExposureSetting exposure)
{
    if (!IsConnect())
    {
        return Result(false, "Set exposure failed, colorimeter is not connected.");
    }

    Result ret;
    MLMonoBusinessManage* mono = GetMonocular();
    ML::CameraV2::MLPixelFormat curFormat = mono->ML_GetPixelFormat();
    if (exposure.Mode == ExposureMode::Auto)
    {
        if (curFormat != MLPixelFormat::MLMono12)
        {
            ret = mono->ML_SetPixelFormat(MLPixelFormat::MLMono12);
            if (!ret.success)
                return ret;
        }
        ret = mono->ML_SetExposure(exposure);
        if (!ret.success)
            return ret;
        ret = mono->ML_SetPixelFormat(curFormat);
        if (!ret.success)
            return ret;
    }
    else
    {
        ret = mono->ML_SetExposure(exposure);
    }
    if (!ret.success)
    {
        return Result(false, "Set exposure failed." + ret.errorMsg);
    }
    return Result();
}

Result MLColorimeterMode::SetBitDepth(int bitDepth)
{
    if (!IsConnect())
    {
        return Result(false, "Set bit depth failed, colorimeter is not connected.");
    }

    MLPixelFormat format;
    if (bitDepth == 8)
    {
        format = ML::CameraV2::MLPixelFormat::MLMono8;
    }
    else if (bitDepth == 10)
    {
        format = ML::CameraV2::MLPixelFormat::MLMono10;
    }
    else if (bitDepth == 12)
    {
        format = ML::CameraV2::MLPixelFormat::MLMono12;
    }
    else if (bitDepth == 16)
    {
        format = ML::CameraV2::MLPixelFormat::MLMono12;
    }
    Result ret = SetBitDepth(format);
    return ret;
}

Result MLColorimeterMode::SetBitDepth(ML::CameraV2::MLPixelFormat format)
{
    if (!IsConnect())
    {
        return Result(false, "Set bit depth failed, colorimeter is not connected.");
    }
    Result ret = GetMonocular()->ML_SetPixelFormat(format);
    if (!ret.success)
    {
        return Result(false, QString("Set bit depth failed.").toStdString() + ret.errorMsg);
    }
    return Result();
}

Result MLColorimeterMode::SetBinning(int binning)
{
    ML::CameraV2::Binning bin;
    if (binning == 1)
    {
        bin = ML::CameraV2::Binning::ONE_BY_ONE;
    }
    else if (binning == 2)
    {
        bin = ML::CameraV2::Binning::TWO_BY_TWO;
    }
    else if (binning == 4)
    {
        bin = ML::CameraV2::Binning::FOUR_BY_FOUR;
    }
    else if (binning == 8)
    {
        bin = ML::CameraV2::Binning::EIGHT_BY_EIGHT;
    }

    Result ret = SetBinning(bin);
    return ret;
}

Result MLColorimeterMode::SetBinning(Binning binning)
{
    if (!IsConnect())
    {
        return Result(false, "Set binning failed, colorimeter is not connected.");
    }

    Result ret = GetMonocular()->ML_SetBinning(binning);
    if (!ret.success)
    {
        return Result(false, QString("Set binning failed.").toStdString() + ret.errorMsg);
    }
    return Result();
}

Result MLColorimeterMode::SetBinningMode(const QString& binningMode)
{
    ML::CameraV2::BinningMode mode = ML::CameraV2::BinningMode::SUM;
    if (binningMode.toLower() == "average")
    {
        mode = ML::CameraV2::BinningMode::AVERAGE;
    }

    Result ret = SetBinningMode(mode);
    return ret;
}

Result MLColorimeterMode::SetBinningMode(BinningMode binningMode)
{
    if (!IsConnect())
    {
        return Result(false, "Set binning mode failed, colorimeter is not connected.");
    }

    Result ret = GetMonocular()->ML_SetBinningMode(binningMode);
    if (!ret.success)
    {
        return Result(false, QString("Set binning mode failed.").toStdString() + ret.errorMsg);
    }
    return Result();
}

Result MLColorimeterMode::SetBinningSelector(ML::CameraV2::BinningSelector binningSelector)
{
    if (!IsConnect())
    {
        return Result(false, "Set binning mode failed, colorimeter is not connected.");
    }

    Result ret = GetMonocular()->ML_SetBinningSelector(binningSelector);
    if (!ret.success)
    {
        return Result(false, QString("Set binning selector failed.").toStdString() + ret.errorMsg);
    }
    return Result();
}

double MLColorimeterMode::GetExposureTime()
{
    if (IsConnect()) {
        return GetCamera()->GetExposureTime() / 1000.0;
    }
    return 0.0;
}

int MLColorimeterMode::GetBinning()
{
    MLMonoBusinessManage* monocular = GetMonocular();
    if (monocular == nullptr)
    {
        return 0;
    }
    Binning bin = monocular->ML_GetBinning();

    int binInt = 1;
    switch (bin)
    {
    case ML::CameraV2::Binning::ONE_BY_ONE:
        binInt = 1;
        break;
    case ML::CameraV2::Binning::TWO_BY_TWO:
        binInt = 2;
        break;
    case ML::CameraV2::Binning::FOUR_BY_FOUR:
        binInt = 4;
        break;
    case ML::CameraV2::Binning::EIGHT_BY_EIGHT:
        binInt = 8;
        break;
    default:
        break;
    }

    return binInt;
}

ML::CameraV2::Binning MLColorimeterMode::TransIntToBinning(int bin)
{
    if (bin == 1)
        return ML::CameraV2::Binning::ONE_BY_ONE;
    else if (bin == 2)
        return ML::CameraV2::Binning::TWO_BY_TWO;
    else if (bin == 4)
        return ML::CameraV2::Binning::FOUR_BY_FOUR;
    else if (bin == 8)
        return ML::CameraV2::Binning::EIGHT_BY_EIGHT;
    else
        return ML::CameraV2::Binning::ONE_BY_ONE;
}

std::string MLColorimeterMode::GetBitDepth()
{
    if (IsConnect()) {
        return MLColorimeterHelp::instance()->TransMLPixelFormatToStr(GetMonocular()->ML_GetPixelFormat());
    }
    return "";
}

BinningMode MLColorimeterMode::GetBinningMode()
{
    MLMonoBusinessManage* monocular = GetMonocular();
    if (monocular == nullptr)
    {
        return BinningMode::AVERAGE;
    }
    return monocular->ML_GetBinningMode();
}

Result MLColorimeterMode::GetImage(cv::Mat &image)
{
    if (!IsConnect())
    {
        return Result(false, "GetImage failed, colorimeter is not connected.");
    }
    Result ret = GetMonocular()->ML_CaptureImageSync();
    if (!ret.success)
    {
        return Result(false, "GetImage failed," + ret.errorMsg);
    }
    image = GetMonocular()->ML_GetImage();
    return Result();
}

Result MLColorimeterMode::MotionScanProcess(FocusScanConfig config, std::string path)
{
    return Result();
    //if (!IsConnect())
    //{
    //    return Result(false, "MotionScanProcess failed, colorimeter is not connected.");
    //}
    //cv::Mat image;
    //Result ret = GetImage(image);
    //if (!ret.success)
    //{
    //    return Result(false, "MotionScanProcess failed," + ret.errorMsg);
    //}

    //int binNum = GetBinning();
    //MultiCrossHairDetection md;
    //md.setBinNum(binNum);
    //MultiCrossHairRe algo_ret = md.getMuliCrossHairCenter(image, MLIQMetrics::IQMetricsParameters::crossBinNum / binNum, false);
    //if (!algo_ret.flag)
    //{
    //    std::string save_folder = path;
    //    std::string sn = MetricsData::instance()->getDutBarCode().toStdString();
    //    if (isDirExist(QString::fromStdString(save_folder)))
    //    {
    //        std::string path = save_folder + sn + "_through_focus_init.tif";
    //        cv::imwrite(path, image);
    //    }
    //    return Result(false, algo_ret.errMsg);
    //}

    //config.ROIs = algo_ret.rectVec;
    //ret = GetMonocular()->ML_ThroughFocus(FOCUS_KEY, config);
    //if (!ret.success)
    //{
    //    return Result(false, "MotionScanProcess failed," + ret.errorMsg);
    //}
    //if (!path.empty())
    //{
    //    ret = GetMonocular()->ML_SaveThroughFocusResult(path);
    //}
    //return ret;
}


//double MLColorimeterMode::GetDiopterScanHResult()
//{
//    bool isFine = false;
//    Result ret = GetMonocular()->ML_CalculateCombinMap("H", { 0,1 }, isFine);
//    if (!ret.success)
//    {
//        LoggingWrapper::instance()->warn("MLColorimeterMode get diopterScan h result error, " + QString::fromStdString(ret.errorMsg));
//        return 0.0;
//    }
//    return GetMonocular()->ML_GetDiopterScanHResult().x;
//}
//
//double MLColorimeterMode::GetDiopterScanVResult()
//{
//    bool isFine = false;
//    Result ret = GetMonocular()->ML_CalculateCombinMap("V", { 2,3 }, isFine);
//    if (!ret.success)
//    {
//        LoggingWrapper::instance()->warn("MLColorimeterMode get diopterScan v result error, " + QString::fromStdString(ret.errorMsg));
//        return 0.0;
//    }
//    return GetMonocular()->ML_GetDiopterScanVResult().x;
//}

//double MLColorimeterMode::GetDiopterScanAVGResult()
//{
//    cv::Point3f retH = GetMonocular()->ML_GetDiopterScanHResult();
//    cv::Point3f retV = GetMonocular()->ML_GetDiopterScanVResult();
//    return TransFocusMotionToSphVid((TransSphToFocusMotionVid(retH.x).z + TransSphToFocusMotionVid(retV.x).z) / 2.0).x;
//}

double MLColorimeterMode::GetMotionScanHResult()
{
    //std::vector<cv::Point3f> roisRe = GetMonocular()->ML_GetThroughfocusResult();
    //if (roisRe.size() == 4)
    //{
    //    return TransFocusMotionToSphVid((roisRe[0].x + roisRe[1].x) / 2.0).x;
    //}
    return -1;
}

double MLColorimeterMode::GetMotionScanVResult()
{
    //std::vector<cv::Point3f> roisRe = GetMonocular()->ML_GetThroughfocusResult();
    //if (roisRe.size() == 4)
    //{
    //    return TransFocusMotionToSphVid((roisRe[2].x + roisRe[3].x) / 2.0).x;
    //}
    return -1;
}

double MLColorimeterMode::GetMotionScanAVGResult()
{
    //std::vector<cv::Point3f> roisRe = GetMonocular()->ML_GetThroughfocusResult();
    //if (roisRe.size() == 4)
    //{
    //    return TransFocusMotionToSphVid((roisRe[0].x + roisRe[1].x + roisRe[2].x + roisRe[3].x) / 4.0).x;
    //}
    return -1;
}

void MLColorimeterMode::StopThroughFocusProcess()
{
    GetMonocular()->StopThroughFocus();
}

ML::MLColorimeter::MotionConfig MLColorimeterMode::GetCameraMotionConfig()
{
    return GetMonocular()->ML_GetBusinessManageConfig()->GetModuleInfo().MotionConfig_Map[FOCUS_KEY];
}

ML::MLColorimeter::ModuleConfig MLColorimeterMode::GetModuleConfig()
{
    return GetMonocular()->ML_GetBusinessManageConfig()->GetModuleInfo();
}

Result MLColorimeterMode::LoadCalibrationData(const CalibrationConfig2& c_config, const CalibrationFlag& c_flag)
{
    struct FlagProcessPair {
        bool flag;
        ProcessType process;
    };

    const FlagProcessPair flags[] = {
        { c_flag.Dark_Flag, ProcessType::Dark },
        { c_flag.FFC_Flag, ProcessType::FFC },
        { c_flag.Distortion_Flag, ProcessType::Distortion },
        { c_flag.ColorShift_Flag, ProcessType::ColorShift },
        { c_flag.FourColor_Flag, ProcessType::FourColor },
        { c_flag.Exposure_Flag, ProcessType::Exposure },
        { c_flag.Flip_Rotate_Flag, ProcessType::Flip_Rotate },
        { c_flag.FOVCrop_Flag, ProcessType::FOVCrop },
        { c_flag.Luminance_Flag, ProcessType::Luminance },
        { c_flag.Keystone_Flag, ProcessType::Keystone }
    };

    Result ret;
    for (const auto& fp : flags) {
        if (fp.flag) {
            ret = m_taskManager->LoadCalibrationModule(fp.process, c_config);
            if (!ret.success)
                return ret;
        }
    }
    return ret;
}

cv::Mat MLColorimeterMode::Flip_Rotation(cv::Mat srcImg)
{
    ML::MLColorimeter::ProcessPathConfig ppconfig;
    ppconfig.InputPath = CONFIG_PATH.toStdString();
    std::string outPath =
        GetMonocular()->ML_GetBusinessManageConfig()->ProcessPath("Flip_Rotate", ppconfig);
    bool ok = MLColorimeterHelp::instance()->ReadFlip_RotateJson(
        (ppconfig.InputPath + outPath).c_str(), "Flip_Rotate");
    ML::MLColorimeter::Flipping FlipX = MLColorimeterHelp::instance()->GetFlippingX();
    ML::MLColorimeter::Flipping FlipY = MLColorimeterHelp::instance()->GetFlippingY();
    ML::MLColorimeter::Rotation Rotate = MLColorimeterHelp::instance()->GetRotation();

    if (FlipX == ML::MLColorimeter::Flipping::ReverseX) {
        srcImg = MLColorimeterHelp::instance()->Flip(srcImg, FlipX);
    }
    if (FlipY == ML::MLColorimeter::Flipping::ReverseY) {
        srcImg = MLColorimeterHelp::instance()->Flip(srcImg, FlipY);
    }
    if (Rotate != ML::MLColorimeter::Rotation::R0) {
        srcImg = MLColorimeterHelp::instance()->Rotation(srcImg, Rotate);
    }
    return srcImg;
}

bool MLColorimeterMode::UpdateFlipRotation(Flipping flip_x, Flipping flip_y, Rotation rotate)
{
    ML::MLColorimeter::ProcessPathConfig ppconfig;
    ppconfig.InputPath = GetMonocular()->ML_GetConfigPath();
    std::string outPath =
        GetMonocular()->ML_GetBusinessManageConfig()->ProcessPath("Flip_Rotate", ppconfig);
    return MLColorimeterHelp::instance()->WriteFlip_RotateToJson(
        (ppconfig.InputPath + "\\" + outPath).c_str(), "Flip_Rotate", flip_x, flip_y, rotate);
}

QString MLColorimeterMode::GetProcessPath()
{
    return CONFIG_PATH + "\\EYE1";
}

bool MLColorimeterMode::isEnabled(std::string key)
{
    if (!IsConnect())
    {
        return false;
    }

    bool enabled = false;
    ML::MLColorimeter::ModuleConfig config = GetMonocular()->ML_GetBusinessManageConfig()->GetModuleInfo();
    if (ND_FILTER_KEY == key || XYZ_FILTER_KEY == key)
    {
        if (config.NDFilterConfig_Map.find(key) != config.NDFilterConfig_Map.end())
        {
            enabled = config.NDFilterConfig_Map[key].enable;
        }
    }
    else if (FOCUS_KEY == key)
    {
        if (config.MotionConfig_Map.find(key) != config.MotionConfig_Map.end())
        {
            enabled = config.MotionConfig_Map[key].Enable;
        }
    }
    else if (CAMERA_KEY == key)
    {
        enabled = config.CameraConfig.Enable;
    }
    else if (RX_KEY == key)
    {
        enabled = config.RXFilterConfig.enable;
    }
    return enabled;
}

Result MLColorimeterMode::initAfterConnect()
{
    Result ret = ClearAlarmFilter();
    if (!ret.success)
    {
        LoggingWrapper::instance()->warn("Init after connect error, " + QString::fromStdString(ret.errorMsg));
    }
    return ret;
}

bool MLColorimeterMode::isDirExist(QString fullPath)
{
    if (fullPath.isEmpty())
    {
        return true;
    }
    QDir dir(fullPath);
    if (dir.exists())
    {
        return true;
    }
    else
    {
        bool ok = dir.mkpath(fullPath);
        return ok;
    }
}

Result MLColorimeterMode::ClearFixExposureTimeConfig()
{
	QFile file(QString::fromStdString(FIXEXPOSURE_FILE));
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		file.close();
		return Result();
	}
	else
	{
		LoggingWrapper::instance()->error("MLColorimeterMode:cannot clear file " + QString::fromStdString(FIXEXPOSURE_FILE));
		return Result(false, "MLColorimeterMode:cannot clear file " + FIXEXPOSURE_FILE);
	}
}

bool MLColorimeterMode::DeleteFixExposureTimeCsv()
{
    if (!QFile::exists(QString::fromStdString(FIXEXPOSURE_FILE))) {
        return true;
    }

    if (QFile::remove(QString::fromStdString(FIXEXPOSURE_FILE))) {
        return true;
    }
    else {
        LoggingWrapper::instance()->error("MLColorimeterMode: file " + QString::fromStdString(FIXEXPOSURE_FILE) + " delete error!");
        return false;
    }
}

Result MLColorimeterMode::UpdateFixExposureTimeConfig(const QString& inputFilePath, bool isReplace)
{
    QString inputPath = inputFilePath + "\\metaData.csv";

    if (isReplace){
        fs::path sourceFilePath = fs::path(inputPath.toStdString());
        if (!fs::exists(sourceFilePath) && fs::is_regular_file(sourceFilePath))
            return Result(false, "MLColorimeterMode: " + inputPath.toStdString() + " is not exist!");
        if (!fs::copy_file(sourceFilePath, FIXEXPOSURE_FILE, fs::copy_options::overwrite_existing))
            return Result(false, "MLColorimeterMode: copy " + inputPath.toStdString() + " to " + FIXEXPOSURE_FILE + " error!");
        return Result();
    }

    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LoggingWrapper::instance()->error("MLColorimeterMode:cannot open input file " + inputPath);
        return Result(false, "MLColorimeterMode:cannot open input file " + inputPath.toStdString());
    }

    QFile outputFile(QString::fromStdString(FIXEXPOSURE_FILE));
    if (!outputFile.open(QIODevice::Append | QIODevice::Text)) {
        LoggingWrapper::instance()->error("MLColorimeterMode:cannot open output file " + QString::fromStdString(FIXEXPOSURE_FILE));
        return Result(false, "MLColorimeterMode:cannot open output file " + FIXEXPOSURE_FILE);
    }

    bool outputExists = outputFile.exists() && outputFile.size() > 0;

    QTextStream in(&inputFile);
    QTextStream out(&outputFile);

    bool firstLine = true;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.isEmpty()) continue;

        if (firstLine && outputExists) {
            firstLine = false;
            continue;
        }
        firstLine = false;

        QStringList parts = line.split(',');
        out << parts.join(',') << "\n";
    }

    inputFile.close();
    outputFile.close();
    return Result();
}

bool MLColorimeterMode::ReadFixExposureTime()
{
	try {
        m_FixExposureTupleVec.clear();
		std::ifstream file(FIXEXPOSURE_FILE);
        if (!file.is_open()) {
            LoggingWrapper::instance()->error(QString("MLColorimeterMode: %1 does not exist or cannot be opened.")
                .arg(QString::fromStdString(FIXEXPOSURE_FILE)), false);
            return false;
        }
		std::string line;
		std::string token;
		std::vector<std::string> headers;

		if (std::getline(file, line)) {
			std::istringstream ss(line);
			while (std::getline(ss, token, ',')) {
				headers.push_back(token);
			}
		}

		while (std::getline(file, line)) {
			std::istringstream ss(line);
			std::vector<std::string> values;
			while (std::getline(ss, token, ',')) {
				values.push_back(token);
			}

			if (values.size() > 1) {
				std::string imageName = values[0];
                std::string eyebox = values[1];
				double exposureTime = std::stod(values[5]);
                std::string str = "#EyeboxID#";
                size_t pos = imageName.find(str);
                if (pos != std::string::npos) {
                    imageName.replace(pos, str.length(), eyebox);
                }
                m_FixExposureTupleVec.push_back(std::make_tuple(imageName, eyebox, exposureTime));
			}
		}
		return true;
	}
	catch (std::exception e)
	{
		LoggingWrapper::instance()->error("MLColorimeterMode: read fix exposure time error," + QString::fromStdString(e.what()), false);
		return false;
	}
}

Result MLColorimeterMode::GetFixExposureTime(std::string name, std::string eyebox, double& time)
{
	if (m_FixExposureTupleVec.empty())
	{
		bool ret = ReadFixExposureTime();
		if (!ret)
		{
			LoggingWrapper::instance()->error(QString::fromStdString("Get Fix exposure time error."), false);
			return Result(false, "Get Fix exposure time error.");
		}
	}
    auto it = std::find_if(
        m_FixExposureTupleVec.begin(),
        m_FixExposureTupleVec.end(),
        [&](const auto& t)
        {
            return m_isMatchEB
                ? (std::get<0>(t) == name && std::get<1>(t) == eyebox)
                : std::get<0>(t) == name;
        });

    if (it == m_FixExposureTupleVec.end())
    {
        std::string err = "Fix exposure time vector is not contain name:" +
            name + " eyebox:" + eyebox;
        LoggingWrapper::instance()->error(QString::fromStdString(err), false);
        return Result(false, err);
    }

    time = std::get<2>(*it);
	return Result();
}

QString MLColorimeterMode::AnalyzeImageName(QString imageNameRule, QString nd, QString lightSource, QString imageType, QString eyeboxID, QString colorFilter)
{
	QMap<QString, QString> valueMap;
	if (nd != "") {
		valueMap["NDFilter"] = nd;
	}
	if (lightSource != "") {
		valueMap["LightSource"] = lightSource;
	}
	if (!imageType.isEmpty()) {
		valueMap["Pattern"] = imageType;
	}
	if (!eyeboxID.isEmpty()) {
		valueMap["EyeboxID"] = eyeboxID;
	}

	if (MLColorimeterHelp::instance()->TransStrToFilterEnum(colorFilter.toStdString()) != ML::MLFilterWheel::MLFilterEnum::Unknown) {
		valueMap["ColorFilter"] = colorFilter;
	}

    //QString meta_rule = imageNameRule.remove("#EyeboxID#");
    //QStringList parts = meta_rule.split('_', Qt::SkipEmptyParts);

    QStringList parts = imageNameRule.split('_');

    for (QString& part : parts) {
        if (part.startsWith('#') && part.endsWith('#')) {
            part = part.mid(1, part.length() - 2);
        }
        if (valueMap.contains(part)) {
            part = valueMap[part];
        }
    }

    return parts.join('_');
}

void MLColorimeterMode::SetFixExposureTimeMatchRule(bool isMatchEB)
{
    m_isMatchEB = isMatchEB;
}
