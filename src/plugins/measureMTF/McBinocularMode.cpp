#include "McBinocularMode.h"
#include <QDateTime>
#include <QDir>
#include <QHash>
#include <QThread>
#include "PrjCommon/service/McCommon.h"
#include <QtConcurrent>
#include <QThread>
#include "MLPostProcessing.h"
#include "ImageRotationConfig.h"
#include "Core/loggingwrapper.h"
//#include "MLAlgorithm/ml_chromaPara.h"
#include "Focus/AutoFocusConfig.h"
#include "LumStatistics.h"
#include "TaskAsync.h"
#include "Focus/AutoVidModel.h"
#include "EtManage.h"
#include "IQMetrics/ML_distortionCompensation.h"
#include "PrjCommon/DeviceManager.h"
#include "PrjCommon/PrjCommon.h"

using namespace ML::MLColorimeter;
using namespace ML::MLFilterWheel;
McBinocularMode *McBinocularMode::self = nullptr;

McBinocularMode::McBinocularMode(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<cv::Mat>("cv::Mat");
    m_bino = new MLBinoBusinessManage(this);
    Result ret = initConfig();
    if (!ret.success) {
        LoggingWrapper::instance()->error(QString::fromStdString(ret.errorMsg));
    }

    ret = loadFile();
    if(!ret.success){
        LoggingWrapper::instance()->error(QString::fromStdString(ret.errorMsg));
    }

    m_syncBestFocus = AutoFocusConfig::GetInstance().isAutoFocusByColor();

    EtManage::instance().setColorCamera(isColorCamera());
    DeviceManager::instance().addDevice("Measure camera", this);
}

McBinocularMode::~McBinocularMode()
{
    if (m_bino != nullptr)
    {
        m_bino->ML_DisconnectModules();
        delete m_bino;
        m_bino = nullptr;
    }
    if (m_process != nullptr)
    {
        delete m_process;
        m_process = nullptr;
    }
}

Result McBinocularMode::connect()
{
    try
    {
        Result ret;
        int startTime = QDateTime::currentMSecsSinceEpoch();
        LoggingWrapper::instance()->debug("Measure camera connect start.");
        m_isConnected = false;
        if (m_bino->ML_IsModulesConnect())
        {
            LoggingWrapper::instance()->debug("Measure camera is connected.");
            m_isConnected = true;
            return Result();
        }

        {
            ret = m_bino->ML_ConnectModules();
            if (!ret.success)
            {
                return Result(false, "Measure camera module connection failed. " + ret.errorMsg);
            }
            m_bino->ML_WaitForMovingStop();
            QThread::msleep(3000);
            m_bino->ML_WaitForMovingStop();
            m_isConnected = true;

            m_process = new MLColorimeterAlgorithms();

            m_process->ML_SetConfigPath(m_bino->ML_GetModuleByID(1)->ML_GetConfigPath().c_str());
            initAfterConnect();
        }

        emit connectStatus(true);

        int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
        LoggingWrapper::instance()->debug(QString("Measure camera connect time: %1 ms.").arg(takeTime));
        return Result();
    }
    catch (exception e)
    {
        return Result(false, e.what());
    }
}

Result McBinocularMode::initConfig()
{
    try{
        if (m_bino->ML_GetModulesIDList().size() == 0){
            static QMutex mutex;
            QMutexLocker locker(&mutex);

            if (m_bino->ML_GetModulesIDList().size() == 0){
                QDir dir(CONFIG_PATH);
                if (!dir.exists())
                {
                    LoggingWrapper::instance()->warn("Directory does not exist:" + CONFIG_PATH);
                    return Result(false, "Directory does not exist:" + CONFIG_PATH.toStdString());
                }

                Result ret;
                QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
                for (const QString &subDir : subDirs)
                {
                    QString subDirAbs = dir.absoluteFilePath(subDir);
                    ret = m_bino->ML_AddModule(subDirAbs.toStdString().c_str());

                    if (!ret.success)
                    {
                        return Result(false, "Init measure camera config failed, " + ret.errorMsg);
                    }
                }
            }
        }
        return Result();
    }
    catch (exception e)
    {
        return Result(false, QString("Measure camera init config error, %1").arg(e.what()).toStdString());
    }
}

Result McBinocularMode::disconnect()
{
    Result ret = m_bino->ML_DisconnectModules();
    m_isConnected = false;
    if (!ret.success)
    {
        return Result(false, "Measure camera disconnect failed, " + ret.errorMsg);
    }

    emit connectStatus(false);
    return Result();
}

bool McBinocularMode::isConnect()
{
    return m_isConnected;

    try
    {
        bool ret;
        {
            MLMonoBusinessManage *monocular = getMonocular();
            if (monocular == nullptr)
            {
                return false;
            }
            ret = monocular->ML_IsModuleConnect();
        }
        return ret;
    }
    catch (exception e)
    {
        return false;
    }
}

bool McBinocularMode::isColorCamera()
{
    if(getMonocular() == nullptr){
        return false;
    }

    ML::MLColorimeter::ModuleConfig config = getMonocular()->ML_GetBusinessManageConfig()->GetModuleInfo();
    m_isColorCamera = config.CameraConfig.ColourCamera;
    return m_isColorCamera;
}

GrabberBase *McBinocularMode::getCamera()
{
    if (getMonocular() == nullptr)
    {
        return nullptr;
    }

    MLMonoBusinessManage *monocular = getMonocular();
    GrabberBase *grabberBase = static_cast<GrabberBase *>(monocular->ML_GetOneModuleByName(CAMERA_KEY));
    return grabberBase;
}

ActuatorBase *McBinocularMode::getCameraMotion()
{
    if (getMonocular() == nullptr)
    {
        return nullptr;
    }

    MLMonoBusinessManage *monocular = getMonocular();
    ActuatorBase *base = static_cast<ActuatorBase *>(monocular->ML_GetOneModuleByName(FOCUS_KEY));
    return base;
}

Result McBinocularMode::updateAperture(const QString& aperture)
{
    MLMonoBusinessManage* mono = getMonocular();
    if (mono == nullptr) {
        return Result();
    }

    Result ret = mono->ML_SetAperture(aperture.toStdString());
    return ret;
}

Result McBinocularMode::setNdFilter(QString nd, bool judge)
{
    if (!isConnect())
    {
        return Result(false, "Set nd filter failed, measure camera is not connected.");
    }

    if(judge && m_cameraParam.ndFilter.toUpper() == nd.toUpper()){
        return Result();
    }

    ML::MLFilterWheel::MLFilterEnum ndEnum = ndFilterToEnum(nd);
    Result ret = getMonocular()->ML_MoveND_XYZFilterByEnumSync(ND_FILTER_KEY, ndEnum);
    if (!ret.success)
    {
        return Result(false, "Set nd filter failed." + ret.errorMsg);
    }

    MetricsData::instance()->updateND(nd);
    
    m_cameraParam.ndFilter = nd;
    return Result();
}

//Result McBinocularMode::setNdFilter(ND_Filter nd)
//{
//    return setNdFilter(ndFilter(nd));
//}

Result McBinocularMode::setXxzFilter(QString xyz, bool judge)
{
    int startTime = QDateTime::currentMSecsSinceEpoch();

    if (!isConnect())
    {
        return Result(false, "Set xyz filter failed, measure camera is not connected.");
    }

    if (judge && m_cameraParam.xyzFilter.toUpper() == xyz.toUpper()) {
        return Result();
    }

    ML::MLFilterWheel::MLFilterEnum xyzEnum = xyzFilterToEnum(xyz);
    Result ret = getMonocular()->ML_MoveND_XYZFilterByEnumSync(XYZ_FILTER_KEY, xyzEnum);
    if (!ret.success)
    {
        return Result(false, "Set xyz filter failed." + ret.errorMsg);
    }

    MetricsData::instance()->updateXYZFilter(xyz);

    m_cameraParam.xyzFilter = xyz;

    int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    qWarning() << "XYZ Filter set " << xyz << ", times:" << takeTime << " ms...";
    return Result();
}

//Result McBinocularMode::setXxzFilter(ColorFilter xyz)
//{
//    return setXxzFilter(QString::fromStdString(colorFilter(xyz)));
//}

McBinocularMode *McBinocularMode::instance(QObject *parent)
{
    if (!self)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self)
        {
            self = new McBinocularMode(parent);
        }
    }
    return self;
}

Result McBinocularMode::connectDevice()
{
    if (Core::PrjCommon::instance()->isLoopProcessTest()) {
        return Result();
    }

    try
    {
        int startTime = QDateTime::currentMSecsSinceEpoch();
        LoggingWrapper::instance()->debug("Measure camera connect start.");
        if (m_bino->ML_IsModulesConnect())
        {
            LoggingWrapper::instance()->debug("Measure camera is connected.");
            return Result();
        }

        m_isConnected = false;
        Result ret;
        {

            ret = getMonocular()->ConnectCamera();
            if (!ret.success) {
                LoggingWrapper::instance()->error("Measure camera connect failed.");
                return ret;
            }

            ret = getMonocular()->ConnectFilterWheel();
            if (!ret.success) {
                LoggingWrapper::instance()->error("Measure camera filter connect failed.");
                return ret;
            }

            ret = getMonocular()->ConnectRXFilterWheel();
            if (!ret.success) {
                LoggingWrapper::instance()->error("Measure camera RX filter connect failed.");
                return ret;
            }
        }

        {
            m_bino->ML_WaitForMovingStop();
            QThread::msleep(3000);
            m_bino->ML_WaitForMovingStop();
            m_isConnected = true;

            m_process = new MLColorimeterAlgorithms();

            m_process->ML_SetConfigPath(m_bino->ML_GetModuleByID(1)->ML_GetConfigPath().c_str());

            initAfterConnect_1();
        }

        emit connectStatus(true);

        int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
        LoggingWrapper::instance()->debug(QString("Measure camera connect time: %1 ms.").arg(takeTime));
        return Result();
    }
    catch (exception e)
    {
        return Result(false, e.what());
    }
}

Result McBinocularMode::disconnectDevice()
{
    return disconnect();
}

Result McBinocularMode::connectBefore()
{
    if (Core::PrjCommon::instance()->isLoopProcessTest()) {
        return Result();
    }

    try
    {
        int startTime = QDateTime::currentMSecsSinceEpoch();
        LoggingWrapper::instance()->debug("Measure camera focus motion connect start.");
        Result ret = initConfig();
        if (!ret.success)
        {
            return ret;
        }

        if (m_bino->ML_IsModulesConnect())
        {
            LoggingWrapper::instance()->debug("Measure camera focus is connected.");
            return Result();
        }

        {
            getMonocular()->ML_CreateModule();
            ret = getMonocular()->ConnectMotion();
            if (!ret.success) {
                LoggingWrapper::instance()->error("Measure camera focus motion connect failed.");
                return ret;
            }
        }

        int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
        LoggingWrapper::instance()->debug(QString("Measure camera focus motion connect time: %1 ms.").arg(takeTime));
        return Result();
    }
    catch (exception e)
    {
        return Result(false, e.what());
    }
}

Result McBinocularMode::connectAfter()
{
    if (Core::PrjCommon::instance()->isLoopProcessTest()) {
        return Result();
    }

    initAfterConnect_2();
    return Result();
}

Result McBinocularMode::setBitDepth(int bitDepth, bool judge)
{
    if (!isConnect())
    {
        return Result(false, "Set bit depth failed, measure camera is not connected.");
    }

    ML::CameraV2::MLPixelFormat format;
    bool isColorCamera = McBinocularMode::instance()->isColorCamera();

    if (bitDepth == 8)
    {
        isColorCamera ? format = ML::CameraV2::MLPixelFormat::MLBayerRG8 : format = ML::CameraV2::MLPixelFormat::MLMono8;
    }
    else if (bitDepth == 10)
    {
        isColorCamera ? format = ML::CameraV2::MLPixelFormat::MLBayerRG10 : format = ML::CameraV2::MLPixelFormat::MLMono10;
    }
    else if (bitDepth == 12)
    {
        isColorCamera ? format = ML::CameraV2::MLPixelFormat::MLBayerRG12 : format = ML::CameraV2::MLPixelFormat::MLMono12;
    }
    else if (bitDepth == 16)
    {
        isColorCamera ? format = ML::CameraV2::MLPixelFormat::MLBayerRG12 : format = ML::CameraV2::MLPixelFormat::MLMono16;
    }

    Result ret = setBitDepth(format, judge);
    return ret;
}

Result McBinocularMode::setBitDepth(ML::CameraV2::MLPixelFormat format, bool judge)
{
    if (!isConnect())
    {
        return Result(false, "Set bit depth failed, measure camera is not connected.");
    }

    if (judge && m_cameraParam.bitdepth == format) {
        return Result();
    }

    GrabberBase *grabberBase = getCamera();
    grabberBase->SetFormatType(format);
    //auto ft = grabberBase->GetFormatType();
    //getMonocular()->ML_SetPixelFormat(format);
    //auto ft = getMonocular()->ML_GetPixelFormat();

    m_cameraParam.bitdepth = format;
    return Result();
}

Result McBinocularMode::setBinning(int binning, bool judge)
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

    Result ret = setBinning(bin, judge);
    return ret;
}

Result McBinocularMode::setBinning(Binning binning, bool judge)
{
    if (!isConnect())
    {
        return Result(false, "Set binning failed, measure camera is not connected.");
    }

    if(judge && m_cameraParam.binning == binning){
        return Result();
    }

    GrabberBase *grabberBase = getCamera();
    grabberBase->SetBinning(binning);

    m_cameraParam.binning = binning;
    return Result();
}

Result McBinocularMode::setBinningMode(const QString &binningMode, bool judge)
{
    BinningMode mode = BinningMode::SUM;
    if (binningMode.toLower() == "average")
    {
        mode = BinningMode::AVERAGE;
    }

    Result ret = setBinningMode(mode, judge);
    return ret;
}

Result McBinocularMode::setBinningMode(BinningMode binningMode, bool judge)
{
    if (!isConnect())
    {
        return Result(false, "Set binning mode failed, measure camera is not connected.");
    }

    if(judge && m_cameraParam.binningMode == binningMode){
        return Result();
    }

    GrabberBase *grabberBase = getCamera();
    grabberBase->SetBinningMode(binningMode);

    m_cameraParam.binningMode = binningMode;
    return Result();
}

Result McBinocularMode::autoExposure_old(int initTime, bool isSet)
{
    int startTime = QDateTime::currentMSecsSinceEpoch();
    if (!isConnect())
    {
        return Result(false, "Auto exposure failed, measure camera is not connected.");
    }

    double eTime = getExposureTime();
    Result ret = setExposureTime(1);
    if (!ret.success)
    {
        return ret;
    }
    Sleep(eTime);

    ExposureSetting exposure;
    exposure.Mode = ExposureMode::Auto;
    exposure.ExposureTime = initTime * 1000.0;
    ret = getMonocular()->ML_SetExposure(exposure);
    if (!ret.success)
    {
        return Result(false, "Auto exposure error, " + ret.errorMsg);
    }

    int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    double curT = getExposureTime();
    ImageNameInfo info = MetricsData::instance()->getImageNameInfo();
    LoggingWrapper::instance()->debug(QString("Auto exposure time: %1 ms. exposure time is %2, pattern is %3, color is %4, XYZ filter is %5.")
        .arg(takeTime)
        .arg(QString::number(curT, 'f', 3))
        .arg(info.deviceImage)
        .arg(info.deviceColor)
        .arg(info.xyzFilter));

    m_cameraParam.exposureTime = curT;
    return Result();
}

Result McBinocularMode::autoExposure(int initTime, bool isSet)
{
    int startTime = QDateTime::currentMSecsSinceEpoch();
    if (!isConnect())
    {
        return Result(false, "Auto exposure failed, measure camera is not connected.");
    }

    if (initTime == 0)
    {
        initTime = 10;
        isSet = true;
    }

    ExposureSetting exposure;
    exposure.Mode = ExposureMode::Auto;
    exposure.ExposureTime = initTime * 1000.0;
    Result ret = SetMLExposureAuto(initTime * 1000.0, isSet);
    if (!ret.success)
    {
        return Result(false, "Auto exposure error, " + ret.errorMsg);
    }

    int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    double curT = getExposureTime();
    ImageNameInfo info = MetricsData::instance()->getImageNameInfo();
    LoggingWrapper::instance()->debug(QString("Auto exposure time: %1 ms. exposure time is %2, pattern is %3, color is %4, XYZ filter is %5.")
        .arg(takeTime)
        .arg(QString::number(curT, 'f', 3))
        .arg(info.deviceImage)
        .arg(info.deviceColor)
        .arg(info.xyzFilter));

    m_cameraParam.exposureTime = curT;
    return Result();
}

Result McBinocularMode::saveImage(QString fileName, CaptureData & data, Result autoExpInfo, bool saveRawImage, bool judgeET)
{
    if (!isConnect())
    {
        return Result(false, "Save image failed, camera is not connected.");
    }

    //CaptureData data;
    Result ret = captureImages(data);
    if (!ret.success)
    {
        return Result(false, "Capture image failed.");
    }

    ret = saveRawImg(saveRawImage, fileName, data);
    if (!ret.success)
    {
        return ret;
    }

    //QString fileNameFull = fileName + "_" + MetricsData::instance()->m_imageNameInfo.imageFile() + IMAGE_FILE_EXT;
    ret = saveImageInfo(fileName, data, autoExpInfo);
    if (!ret.success)   
    {
        return ret;
    }

    ret = saveImageJson(getImageName(4));
    if (!ret.success)
    {
        return ret;
    }

    if (judgeET) {
        ret = judgeExposure(data, fileName);
        if (!ret.success) {
            return ret;
        }
    }

    return Result();
}

Result McBinocularMode::saveImage_YY(QString fileName, ImgExposureCache aeCache, bool saveRawImage, int imgSaveBit)
{
    MetricsData::instance()->setImgFileName(fileName);
    Result ret = EtManage::instance().getExposureTime(aeCache.isAuto, aeCache.initTime);
    if (!ret.success) {
        return ret;
    }

    Result exposeRet;
    if(!EtManage::instance().getCustomize() || aeCache.isSet){
        if (aeCache.isAuto)
        {
            exposeRet = autoExposure(aeCache.initTime, aeCache.initTime > 0);
        }
        else
        {
            exposeRet = setExposureTime(aeCache.initTime);
        }
    }

    if (EtManage::instance().getCustomize() && aeCache.OverExposureFactor > 0)
    {
        double eTime = getExposureTime();
        exposeRet = setExposureTime(eTime * aeCache.OverExposureFactor);
    }

    CaptureData data;
    ret = saveImage(fileName, data, exposeRet, saveRawImage, aeCache.OverExposureFactor == 0);
    if (!ret.success)
    {
        return ret;
    }

    if (data.ColorFilter != ML::MLFilterWheel::MLFilterEnum::X ||
        data.ColorFilter != ML::MLFilterWheel::MLFilterEnum::Y ||
        data.ColorFilter != ML::MLFilterWheel::MLFilterEnum::Z)
    {
        data.ColorFilter = ML::MLFilterWheel::MLFilterEnum::Y;
    }

    std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap;
    dataMap[MLFilterEnum::Y] = data;

    std::map<CalibrationEnum, std::map<MLFilterEnum, CaptureData>> calibrationDatas;
    ret = this->calibration_YY(dataMap, calibrationDatas, fileName, true, imgSaveBit);
    if (!ret.success)
    {
        return ret;
    }

    //TODO:test
    if (!exposeRet.success)
    {
        // code for CORE::Wrapper_Warning
        exposeRet.errorCode = 15;
        return exposeRet;
    }

    ret = EtManage::instance().writeData();
    if (!ret.success)
    {
        return ret;
    }
    return ret;
}

Result McBinocularMode::saveImage_XYZ(QString fileName, XyzExposureCache aeCache, bool saveRawImage, bool saveSLBYY, bool luminanceSave, int imgSaveBit)
{
    MetricsData::instance()->setImgFileName(fileName);
    std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> xyzData;
    Result exposeRet;
    Result ret = captureXYZFilterImages(xyzData, exposeRet, fileName, aeCache, saveRawImage, luminanceSave);
    if (!ret.success)
    {
        return ret;
    }

    ret = this->saveCalibrationImage(fileName, xyzData, true, saveSLBYY, luminanceSave, imgSaveBit);
    if (!ret.success)
    {
        return ret;
    }

    //TODO:test
    if (!exposeRet.success)
    {
        // code for CORE::Wrapper_Warning
        exposeRet.errorCode = 15;
        return exposeRet;
    }
    return ret;
}

Result McBinocularMode::saveImage_xyY(QString fileName, XyzExposureCache aeCache, bool saveRawImage, bool saveSLBYY)
{
    std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> xyzData;
    Result exposeRet;
    Result ret = captureXYZFilterImages(xyzData, exposeRet, fileName, aeCache, saveRawImage);
    if (!ret.success)
    {
        return ret;
    }

    ret = this->saveCalibrationImage(fileName, xyzData, false, saveSLBYY);
    if (!ret.success)
    {
        return ret;
    }

    if (!exposeRet.success)
    {
        return exposeRet;
    }
    return ret;
}

Result McBinocularMode::saveImage_raw(QString fileName, ImgExposureCache aeCache)
{
    Result exposeRet;
    if (aeCache.isSet)
    {
        if (aeCache.isAuto)
        {
            exposeRet = autoExposure(aeCache.initTime, aeCache.initTime > 0);
        }
        else
        {
            exposeRet = setExposureTime(aeCache.initTime);
        }
    }

    CaptureData data;
    Result ret = saveImage(fileName, data, exposeRet, true);
    if (!ret.success)
    {
        return ret;
    }

    return ret;
}

Result McBinocularMode::saveImage_FFC(QString fileName, ImgExposureCache aeCache, QString imgPaths)
{
    Result exposeRet;
    if (aeCache.isSet)
    {
        if (aeCache.isAuto)
        {
            exposeRet = autoExposure(aeCache.initTime, aeCache.initTime > 0);
        }
        else
        {
            exposeRet = setExposureTime(aeCache.initTime);
        }
    }

    if (!isConnect())
    {
        return Result(false, "Save image failed, camera is not connected.");
    }

    CaptureData data;
    Result ret = captureImages(data);
    if (!ret.success)
    {
        return Result(false, "Capture image failed.");
    }

    QFileInfo fileInfo(fileName);
    QString path = fileInfo.absolutePath();
    QDir dir;
    if (!dir.exists(path))
    {
        dir.mkpath(path);
    }
    cv::imwrite(fileName.toStdString(), data.Img);

    {
        QFile infoFile(imgPaths + "\\info.txt");
        if (!infoFile.open(QIODevice::Append | QIODevice::WriteOnly))
        {
            return Result(false, "Save info.txt failed, open file error.");
        }

        QStringList infoLst;
        infoLst.append("name:" + fileName);
        double t1 = data.ExposureTime;
        infoLst.append("expTime:" + QString::number(t1, 'f', 3));
        infoFile.write(infoLst.join(";").toUtf8());
        infoFile.write("\n");
        infoFile.close();
    }
    return Result();
}

Result McBinocularMode::color_saveImage_YY(QString fileName, ImgExposureCache aeCache, bool saveRawImage, int imgSaveBit)
{
    Result ret;
    MetricsData::instance()->setImgFileName(fileName);
    std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap;
    Result exposeRet;
    ret = color_captureYImage(dataMap, exposeRet, fileName, aeCache, saveRawImage, aeCache.OverExposureFactor == 0);
    if (!ret.success)
    {
        return ret;
    }
    std::map<CalibrationEnum, std::map<MLFilterEnum, CaptureData>> calibrationDatas;
    ret = calibration_YY(dataMap, calibrationDatas, fileName, true, imgSaveBit);
    if (!ret.success)
    {
        return ret;
    }

    if (!exposeRet.success)
    {
        // code for CORE::Wrapper_Warning
        exposeRet.errorCode = 15;
        return exposeRet;
    }
    return ret;

}

Result McBinocularMode::color_saveImage_XYZ(QString fileName, ImgExposureCache aeCache, bool saveRawImage, bool saveSLBYY, bool luminanceSave, int imgSaveBit)
{
    MetricsData::instance()->setImgFileName(fileName);
    std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> xyzData;
    Result exposeRet;
    Result ret = color_captureXYZImages(xyzData, exposeRet, fileName, aeCache, saveRawImage, luminanceSave);
    if (!ret.success)
    {
        return ret;
    }

    ret = this->saveCalibrationImage(fileName, xyzData, true, saveSLBYY, luminanceSave, imgSaveBit);
    if (!ret.success)
    {
        return ret;
    }
    if (!exposeRet.success)
    {
        // code for CORE::Wrapper_Warning
        exposeRet.errorCode = 15;
        return exposeRet;
    }
    return ret;
}

Result McBinocularMode::color_saveImage_xyY(QString fileName, ImgExposureCache aeCache, bool saveRawImage, bool saveSLBYY, bool luminanceSave , int imgSaveBit)
{
    MetricsData::instance()->setImgFileName(fileName);
    std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> xyzData;
    Result exposeRet;
    Result ret = color_captureXYZImages(xyzData, exposeRet, fileName, aeCache, saveRawImage, luminanceSave);
    if (!ret.success)
    {
        return ret;
    }

    ret = this->saveCalibrationImage(fileName, xyzData, false, saveSLBYY, luminanceSave, imgSaveBit);
    if (!ret.success)
    {
        return ret;
    }
    if (!exposeRet.success)
    {
        // code for CORE::Wrapper_Warning
        exposeRet.errorCode = 15;
        return exposeRet;
    }
    return ret;
}

Result McBinocularMode::captureImages(CaptureData &data)
{
    Result ret = updateFocusByColor();
    if (!ret.success)
    {
        return Result(false, QString("Capture images failed.").toStdString());
    }

    MLMonoBusinessManage *monocular = getMonocular();
    ret = monocular->ML_CaptureImageSync();
    if (!ret.success)
    {
        return Result(false, QString("Capture images failed.").toStdString());
    }

    data.Img = getMonocular()->ML_GetImage();

    data.Aperture = getMonocular()->ML_GetAperture();
    data.NDFilter = ndFilterToEnum(m_cameraParam.ndFilter);
    data.ColorFilter = xyzFilterToEnum(m_cameraParam.xyzFilter);

    data.MovementRX.Sphere = m_cameraParam.sphereDiopters;
    data.MovementRX.Cylinder = m_cameraParam.cylinder.toUpper().remove("D").toFloat();
    data.MovementRX.Axis = m_cameraParam.cylinderAxisDegree;

    data.VID = m_cameraParam.VID;
    data.ExposureTime = m_cameraParam.exposureTime;
    data.Binning = m_cameraParam.binning;
    data.PixelFormat = m_cameraParam.bitdepth;
    data.LightSource = MetricsData::instance()->getColor().toStdString();

    //qWarning() << "==================================================== ";
    //qWarning() << "[Memory NdFilter]: " << m_cameraParam.ndFilter;
    //qWarning() << "[Memory ColorFilter]: " << m_cameraParam.xyzFilter;
    //qWarning() << "[Memory Sphere]: " << data.MovementRX.Sphere;
    //qWarning() << "[Memory Cylinder]: " << data.MovementRX.Cylinder;
    //qWarning() << "[Memory Axis]: " << data.MovementRX.Axis;
    //qWarning() << "[Memory VID]: " << data.VID;
    //qWarning() << "[Memory Focus]: " << m_cameraParam.focus;
    //qWarning() << "[Memory ExposureTime]: " << data.ExposureTime;
    //qWarning() << "[Memory Binning]: " << static_cast<int>(data.Binning) ;
    //qWarning() << "[Memory PixelFormat]: " << static_cast<int>(data.PixelFormat);
    //qWarning() << "[Memory LightSource]: " << data.LightSource.c_str();
    //qWarning() << "==================================================== ";

    //qWarning() << "[Device NDFilter]: " << ndFilterToStr(getMonocular()->ML_GetND_XYZFilterChannel("NDFilter"));
    //qWarning() << "[Device ColorFilter]: " << xyzFilterToStr(getMonocular()->ML_GetND_XYZFilterChannel("ColorFilter"));
    //qWarning() << "[Device Sphere]: " << getMonocular()->ML_GetRX().Sphere;
    //qWarning() << "[Device Cylinder]: " << getMonocular()->ML_GetRX().Cylinder;
    //qWarning() << "[Device Axis]: " << getMonocular()->ML_GetRX().Axis;
    //qWarning() << "[Device VID]: " << getMonocular()->ML_GetVID();
    //qWarning() << "[Device CameraMotion]: " << getMonocular()->ML_GetMotionPosition("CameraMotion");
    //qWarning() << "[Device ExposureTime]: " << getMonocular()->ML_GetExposureTime();
    //qWarning() << "[Device Binning]: " << static_cast<int>(getMonocular()->ML_GetBinning());
    //qWarning() << "[Device PixelFormat]: " << static_cast<int>(getMonocular()->ML_GetPixelFormat());
    //qWarning() << "[Device LightSource]: " << getMonocular()->ML_GetLightSource().c_str();
    //qWarning() << "==================================================== ";
    return Result();
}

Result McBinocularMode::color_captureYImage(std::map<MLFilterEnum, CaptureData>& ydata, Result& exposeRet, QString fileName, ImgExposureCache aeCache, bool saveRawImage, bool judgeET)
{
    if (!isConnect())
    {
        return Result(false, "Capture Y image failed, color camera is not connected.");
    }

    Result ret = updateFocusByColor();
    if (!ret.success)
    {
        return Result(false, QString("captureColorYImage failed.").toStdString());
    }
    ret = EtManage::instance().getExposureTime(aeCache.isAuto, aeCache.initTime);
    if (!ret.success) {
        return ret;
    }

    Result exposeRetTmp;
    if(!EtManage::instance().getCustomize() || aeCache.isSet){
        if (aeCache.isAuto)
        {
            exposeRetTmp = autoExposure(aeCache.initTime, aeCache.initTime > 0);
        }
        else
        {
            exposeRetTmp = setExposureTime(aeCache.initTime);
        }
    }

    if (EtManage::instance().getCustomize() && aeCache.OverExposureFactor > 0)
    {
        double eTime = getExposureTime();
        exposeRetTmp = setExposureTime(eTime * aeCache.OverExposureFactor);
    }
    if (!exposeRetTmp.success)
    {
        exposeRet = exposeRetTmp;
    }
    MLMonoBusinessManage* monocular = getMonocular();
    ret = monocular->ML_CaptureImageSync();
    if (!ret.success)
    {
        return Result(false, QString("ColorCamera capture images failed.").toStdString());
    }

    ret = EtManage::instance().writeData();
    if (!ret.success)
    {
        return ret;
    }

    std::map<MLFilterEnum, CaptureData> dataMap;
    cv::Mat colorImg = getMonocular()->ML_GetImage();
    if (colorImg.channels() == 3) {
        std::vector<cv::Mat> vecImg;
        cv::split(colorImg, vecImg);

        ML::MLColorimeter::CaptureData captureData;
        captureData.Aperture = getMonocular()->ML_GetAperture();
        captureData.LightSource = MetricsData::instance()->getColor().toStdString();
        captureData.NDFilter = ndFilterToEnum(m_cameraParam.ndFilter);
        captureData.ColorFilter = ML::MLFilterWheel::MLFilterEnum::Y;
        captureData.VID = m_cameraParam.VID;
        captureData.ExposureTime = m_cameraParam.exposureTime;
        captureData.Binning = m_cameraParam.binning;;
        captureData.PixelFormat = m_cameraParam.bitdepth;;
        captureData.Img = vecImg[1];
        captureData.MovementRX.Sphere = m_cameraParam.sphereDiopters;
        captureData.MovementRX.Cylinder = m_cameraParam.cylinder.toUpper().remove("D").toFloat();
        captureData.MovementRX.Axis = m_cameraParam.cylinderAxisDegree;
        dataMap.insert(std::make_pair(captureData.ColorFilter, captureData));
        ret = saveRawImg(saveRawImage, fileName, captureData);
        if (!ret.success)
        {
            return ret;
        }

        ret = saveImageInfo(fileName, captureData, exposeRetTmp);
        if (!ret.success)
        {
            return ret;
        }

        ret = saveImageJson(getImageName(4));
        if (!ret.success)
        {
            return ret;
        }

        if (judgeET) {
            ret = judgeExposure(captureData, fileName);
            if (!ret.success) {
                return ret;
            }
        }
        
    }
    ydata = dataMap;
}

Result McBinocularMode::color_captureXYZImages(std::map<MLFilterEnum, CaptureData>& xyzData, Result& exposeRet,
    QString fileName, ImgExposureCache aeCache,
    bool saveRawImage, bool luminanceSave, bool judgeET)
{
    Result ret;
    std::map<MLFilterEnum, CaptureData> dataMap;

    if (!isConnect())
    {
        return Result(false, "captureColorXYZImages failed, color camera is not connected.");
    }

    ret = updateFocusByColor();
    if (!ret.success)
    {
        return Result(false, QString("captureColorXYZImages failed.").toStdString());
    }

    ret = waitPause(m_recipePause);
    if (!ret.success)
    {
        return ret;
    }

        ret = EtManage::instance().getExposureTime(aeCache.isAuto, aeCache.initTime);
        if (!ret.success) {
            return ret;
        }

        Result exposeRetTmp;
        if (aeCache.isSet)
        {
            if (aeCache.isAuto)
            {
                exposeRetTmp = autoExposure(aeCache.initTime, aeCache.initTime > 0);
            }
            else
            {
                exposeRetTmp = setExposureTime(aeCache.initTime);
            }
        }

        if (!exposeRetTmp.success)
        {
            exposeRet = exposeRetTmp;
        }

        if (luminanceSave) {
            ret = saveISluminance("ColorCamera-IS-Luminance");
            if (!ret.success) {
                return ret;
            }
        }

        MLMonoBusinessManage* monocular = getMonocular();
        ret = monocular->ML_CaptureImageSync();
        if (!ret.success)
        {
            return Result(false, QString("captureColorXYZImages failed.").toStdString());
        }

        ret = EtManage::instance().writeData();
        if (!ret.success)
        {
            return ret;
        }

        cv::Mat colorImg = getMonocular()->ML_GetImage();
        if (colorImg.channels() == 3) {
            std::vector<cv::Mat> vecImg;
            cv::split(colorImg, vecImg);

            std::map<int, ML::MLFilterWheel::MLFilterEnum> channelMap = {
                   {0, ML::MLFilterWheel::MLFilterEnum::X},
                   {1, ML::MLFilterWheel::MLFilterEnum::Y},
                   {2, ML::MLFilterWheel::MLFilterEnum::Z} };
            for (int i = 0; i < vecImg.size(); i++) {
                ML::MLColorimeter::CaptureData captureData;
                captureData.Aperture = getMonocular()->ML_GetAperture();
                captureData.LightSource = MetricsData::instance()->getColor().toStdString();
                captureData.NDFilter = ndFilterToEnum(m_cameraParam.ndFilter);
                captureData.ColorFilter = channelMap[i];
                captureData.VID = m_cameraParam.VID;
                captureData.ExposureTime = m_cameraParam.exposureTime;
                captureData.Binning = m_cameraParam.binning;;
                captureData.PixelFormat = m_cameraParam.bitdepth;;
                captureData.Img = vecImg[i];
                captureData.MovementRX.Sphere = m_cameraParam.sphereDiopters;
                captureData.MovementRX.Cylinder = m_cameraParam.cylinder.toUpper().remove("D").toFloat();
                captureData.MovementRX.Axis = m_cameraParam.cylinderAxisDegree;
                dataMap.insert(std::make_pair(captureData.ColorFilter, captureData));
                ret = saveRawImg(saveRawImage, fileName, captureData);
                if (!ret.success)
                {
                    return ret;
                }

                ret = saveImageInfo(fileName, captureData, exposeRetTmp);
                if (!ret.success)
                {
                    return ret;
                }

                ret = saveImageJson(getImageName(4));
                if (!ret.success)
                {
                    return ret;
                }

                if (judgeET) {
                    ret = judgeExposure(captureData, fileName);
                    if (!ret.success) {
                        return ret;
                    }
                }
            }
            if (dataMap.size() < 3)
            {
                return Result(false, "captureColorXYZImages get XYZ image error.");
            }
        }

 
        ret = waitPause(m_recipePause);
        if (!ret.success)
        {
            return ret;
        }

        xyzData = dataMap;
        return ret;
    }

Result McBinocularMode::captureImages_old(CaptureData& data)
{
    Result ret = updateFocusByColor();
    if (!ret.success)
    {
        return Result(false, QString("Capture images failed.").toStdString());
    }

    MLMonoBusinessManage* monocular = getMonocular();
    ret = monocular->ML_CaptureImageSync();
    if (!ret.success)
    {
        return Result(false, QString("Capture images failed.").toStdString());
    }

    data = monocular->ML_GetCaptureData();
    data.LightSource = MetricsData::instance()->getColor().toStdString();
    return Result();
}

Result McBinocularMode::captureImageFast(cv::Mat& img)
{
    if (!isConnect())
    {
        return Result(false, "Take image failed, measure camera is not connected.");
    }

    Result ret = updateFocusByColor();
    if (!ret.success)
    {
        return Result(false, QString("Capture images failed.").toStdString());
    }

    MLMonoBusinessManage* monocular = getMonocular();
    img = monocular->ML_GetImage();
    return Result();
}

cv::Mat McBinocularMode::takeImage()
{
    //cv::Mat img;
    //captureImageFast(img);
    //return img;

    if (!isConnect())
    {
        return cv::Mat();
    }

    Result ret = updateFocusByColor();
    if (!ret.success)
    {
        LoggingWrapper::instance()->error(QString::fromStdString(ret.errorMsg));
        return cv::Mat();
    }

    MLMonoBusinessManage *monocular = getMonocular();
    monocular->ML_CaptureImageSync();
    CaptureData data = monocular->ML_GetCaptureData();
    return data.Img;
}

Result McBinocularMode::takeImage(cv::Mat &img)
{
    if (!isConnect())
    {
        return Result(false, "Take image failed, measure camera is not connected.");
    }

    Result ret = updateFocusByColor();
    if (!ret.success)
    {
        return Result(false, QString("Capture images failed.").toStdString());
    }

    MLMonoBusinessManage *monocular = getMonocular();
    monocular->ML_CaptureImageSync();
    CaptureData data = monocular->ML_GetCaptureData();
    img = data.Img;
    return Result();
}

bool McBinocularMode::isGrabbing()
{
    GrabberBase *grabberBase = getCamera();
    if (grabberBase == nullptr || !grabberBase->IsOpened())
    {
        return false;
    }
    return grabberBase->IsGrabbing();
}

Result McBinocularMode::stopGrabbing(bool stop, bool judge)
{
    if (!isConnect())
    {
        return Result(false, "Stop grabbing failed, measure camera is not connected.");
    }

    static bool first = true;
    if(!first && judge && m_cameraParam.stopGrabbing == stop){
        return Result();
    }

    GrabberBase *grabberBase =  getCamera();
    if (grabberBase == nullptr || !grabberBase->IsOpened())
    {
        return Result(false, "Measure camera stop/start grabbing failed, camera is not connected.");
    }

    if (stop)
    {
        LoggingWrapper::instance()->debug("Stop grabbing.");
        grabberBase->StopGrabbing();
    }
    else
    {
        LoggingWrapper::instance()->debug("start grabbing.");
        grabberBase->StartGrabbingAsync();
    }

    emit stopGrab(stop);

    m_cameraParam.stopGrabbing = stop;
    first = false;
    return Result();
}

Result McBinocularMode::setNdFilterAsyn(QString nd)
{
    if (!isConnect())
    {
        return Result(false, "Set nd filter ASYN failed, measure camera is not connected.");
    }

    ML::MLFilterWheel::MLFilterEnum ndEnum = ndFilterToEnum(nd);
    Result ret = getMonocular()->ML_MoveND_XYZFilterByEnumAsync(ND_FILTER_KEY, ndEnum);
    if (!ret.success)
    {
        return Result(false, "Set nd filter failed." + ret.errorMsg);
    }
    return Result();
}

Result McBinocularMode::setXxzFilterAsyn(QString xyz)
{
    if (!isConnect())
    {
        return Result(false, "Set xyz filter ASYN failed, measure camera is not connected.");
    }

    ML::MLFilterWheel::MLFilterEnum xyzEnum = ndFilterToEnum(xyz);
    Result ret = getMonocular()->ML_MoveND_XYZFilterByEnumAsync(XYZ_FILTER_KEY, xyzEnum);
    if (!ret.success)
    {
        return Result(false, "Set nd filter failed." + ret.errorMsg);
    }
    return Result();
}

Result McBinocularMode::setFocus(double focus, bool judge)
{
    if (!isConnect())
    {
        return Result(false, "Set focus failed, measure camera is not connected.");
    }

    double focusNew = focus / 1000.0;
    if(judge && fabs(m_cameraParam.focus - focusNew) < ZERO_JUDGE){
        m_cameraParam.focus = focusNew;
        m_cameraParam.sphereDiopters = calDioptersByPi(focusNew);
        m_cameraParam.VID = calVidByPi(focusNew);
        return Result();
    }

    focus = round(focus * 100.0) / 100.0;
    Result ret = getMonocular()->ML_SetPositionAbsSync(FOCUS_KEY, focusNew);
    if(!ret.success)
    {
        return ret;
    }

    MetricsData::instance()->setFocus(focusNew);
    m_cameraParam.focus = focusNew;
    m_cameraParam.sphereDiopters = calDioptersByPi(focusNew);
    m_cameraParam.VID = calVidByPi(focusNew);
    return ret;
}

Result McBinocularMode::setFocusAsyn(double focus)
{
    if (!isConnect())
    {
        return Result(false, "Set focus ASYN failed, measure camera is not connected.");
    }

    focus = round(focus * 100.0) / 100.0;
    Result ret = getMonocular()->ML_SetPositionAbsAsync(FOCUS_KEY, focus / 1000.0);
    return ret;
}

float McBinocularMode::getFocus()
{
    if (!isConnect())
    {
        return 0.0f;
    }

    float focus = getMonocular()->ML_GetMotionPosition(FOCUS_KEY) * 1000.0f;
    return focus;
}

Result McBinocularMode::setDiopters(double diopters, bool judge)
{
    int startTime = QDateTime::currentMSecsSinceEpoch();

    float pi = calPiByDiopters(diopters);
    Result ret = setFocus(pi * 1000.0, judge);
    if(!ret.success){
        return ret;
    }

    MetricsData::instance()->setDiopter(diopters);

    int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    qWarning() << "Diopters set " << diopters << "D, times:" << takeTime << " ms...";
    return Result();
}

MLBinoBusinessManage *McBinocularMode::getIInstrument()
{
    return m_bino;
}

Result McBinocularMode::getBinocularInfo(BinocularInfo &info)
{
    if (!isConnect())
    {
        return Result(false, "Measure camera is not connected.");
    }

    {
        info = {-1, "", "", "", -1, (enum MLPixelFormat)0, -1, (enum Binning)0, (enum BinningMode)1};

        MLMonoBusinessManage *monocular = getMonocular();
        {
            double m_vid = monocular->ML_GetVID();
            string m_nd = ndFilterToStr(monocular->ML_GetND_XYZFilterChannel(ND_FILTER_KEY)).toStdString();
            string m_color = xyzFilterToStr(monocular->ML_GetND_XYZFilterChannel(XYZ_FILTER_KEY)).toStdString();
            string m_revolution = monocular->ML_GetRXFilterChannel();
            int m_selfRotation = monocular->ML_GetRXFilterAxis();

            MLPixelFormat m_Format = monocular->ML_GetPixelFormat();
            double m_et = monocular->ML_GetExposureTime();
            Binning m_binning = monocular->ML_GetBinning();
            BinningMode m_binmode = monocular->ML_GetBinningMode();

            ML::MLColorimeter::RXCombination rxC = monocular->ML_GetRX();
            string rx = QString("Sphere=%1, Cylinder=%2, Axis=%3").arg(rxC.Sphere).arg(rxC.Cylinder).arg(rxC.Axis).toStdString();
            {
                info.vid = m_vid;
                info.nd = m_nd;
                info.color = m_color;
                info.revolutionCylPos = m_revolution;
                info.selfRotationCylPos = m_selfRotation;
                info.format = m_Format;
                info.et = m_et;
                info.binning = m_binning;
                info.binmode = m_binmode;
                info.RX = rx;
            }
        }
    }

    return Result();
}

void McBinocularMode::notifyPause(bool isPause)
{
    m_recipePause = isPause;
    if (!isPause)
    {
        m_waitCondition.wakeOne();
    }
}

void McBinocularMode::continueRun(bool isContinue)
{
    m_isStop = !isContinue;
    m_waitCondition.wakeOne();

    if (!isContinue)
    {
        if(m_recipePause){
            QThread::msleep(300);
        }
        m_recipePause = false;
    }
}

Result McBinocularMode::getCaptureData(CaptureData &captureData)
{
    if (!isConnect())
    {
        return Result(false, "Set nd filter failed, measure camera is not connected.");
    }

    Result ret = updateFocusByColor();
    if (!ret.success)
    {
        return Result(false, QString("Capture images failed.").toStdString());
    }

    ret = getMonocular()->ML_CaptureImageSync();
    captureData = getMonocular()->ML_GetCaptureData();
    captureData.LightSource = MetricsData::instance()->getColor().toStdString();

    if (!ret.success)
    {
        return Result(false, "Capture image failed." + ret.errorMsg);
    }

    return Result();
}

bool McBinocularMode::createResutCSVFile(QString cvFileName)
{
    QFile csvFile(cvFileName);
    if (!csvFile.open(QIODevice::ReadWrite))
    {
        return false;
    }
    else
    {
        QStringList mtfHeadersList;
        mtfHeadersList.append("id");
        //for (int i = 0; i < rois.size(); i++)
        //{
        //    QString idxStr = QString::number(i);
        //    mtfHeadersList << "x" + idxStr;
        //    mtfHeadersList << "y" + idxStr;
        //    mtfHeadersList << "Y" + idxStr;
        //}
        csvFile.write(mtfHeadersList.join(",").toUtf8());
        csvFile.write("\n");
        csvFile.close();
        return true;
    }
}

Result McBinocularMode::calculateYImage(cv::Mat &imageY, QString lightColor)
{
    Result ret = setXxzFilter("Y");
    if (!ret.success)
    {
        return ret;
    }

    Result result = autoExposure(1, false);
    if (!result.success)
    {
        return result;
    }

    CaptureData data;
    result = getCaptureData(data);
    if (!result.success)
    {
        return result;
    }

    return calculateYImage(imageY, data, lightColor);
}

Result McBinocularMode::calculateYImage(cv::Mat &imageY, CaptureData &rawData, QString lightColor)
{
    std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> xyzData;
    rawData.ColorFilter = ML::MLFilterWheel::MLFilterEnum::Y;
    xyzData[rawData.ColorFilter] = rawData;

    compensationExposure(xyzData);
    Result ret = m_process->ML_SetCaptureDataMap(xyzData);
    if (!ret.success)
    {
        return ret;
    }

    CalibrationConfig config;
    config.Aperture = getMonocular()->ML_GetAperture();
    config.NDFilterList = { getMonocular()->ML_GetND_XYZFilterChannel("NDFilter") };
    config.RX = getMonocular()->ML_GetRX();
    config.ColorFilterList = {ML::MLFilterWheel::MLFilterEnum::Y};
    config.LightSourceList = {MetricsData::instance()->getColor().toStdString()};
    config.Bin = m_cameraParam.binning;

    std::map<CalibrationEnum, std::map<MLFilterEnum, CaptureData>> calibrationDatas;
    ret = calibration(config, calibrationDatas);
    if (!ret.success)
    {
        return ret;
    }

    imageY = calibrationDatas[CalibrationEnum::Result][MLFilterEnum::Y].Img;
    return Result();
}

Result McBinocularMode::calculateXYZImage(QMap<QString, cv::Mat> &imageXYZ, QMap<QString, CaptureData> &rawDataMap,
                                          QString lightColor)
{
    MLFilterEnum xyzName[3] = {MLFilterEnum::X, MLFilterEnum::Y, MLFilterEnum::Z};
    std::map<MLFilterEnum, CaptureData> dataMap;
    for (int i = 0; i < 3; ++i)
    {
        if (!rawDataMap.contains(xyzFilterToStr(xyzName[i]))){
            return Result(false, "Calculate XYZ image error, image missing.");
        }

        CaptureData cData = rawDataMap[xyzFilterToStr(xyzName[i])];
        cData.LightSource = MetricsData::instance()->getColor().toStdString();
        dataMap[xyzName[i]] = cData;
    }

    compensationExposure(dataMap);
    m_process->ML_SetCaptureDataMap(dataMap);

    CalibrationConfig config;
    config.Aperture = getMonocular()->ML_GetAperture();
    config.NDFilterList = { getMonocular()->ML_GetND_XYZFilterChannel("NDFilter") };
    config.RX = getMonocular()->ML_GetRX();
    config.ColorFilterList = {ML::MLFilterWheel::MLFilterEnum::Y};
    config.LightSourceList = { MetricsData::instance()->getColor().toStdString() };
    config.Bin = m_cameraParam.binning;

    std::map<CalibrationEnum, std::map<MLFilterEnum, CaptureData>> calibrationDatas;
    Result ret = calibration(config, calibrationDatas);
    if (!ret.success)
    {
        return ret;
    }

    imageXYZ.clear();
    imageXYZ["X"] = calibrationDatas[CalibrationEnum::Result][MLFilterEnum::X].Img;
    imageXYZ["Y"] = calibrationDatas[CalibrationEnum::Result][MLFilterEnum::Y].Img;
    imageXYZ["Z"] = calibrationDatas[CalibrationEnum::Result][MLFilterEnum::Z].Img;

    imageXYZ["xx"] = calibrationDatas[CalibrationEnum::Chrom][MLFilterEnum::X].Img;
    imageXYZ["yy"] = calibrationDatas[CalibrationEnum::Chrom][MLFilterEnum::Y].Img;
    return Result();
}

double McBinocularMode::getExposureTime(bool judge)
{
    if (isConnect()){
        if(judge){
            return m_cameraParam.exposureTime;
        }

        return getCamera()->GetExposureTime()/1000.0;
    }
    return 0.0;
}

int McBinocularMode::getBinning()
{
    MLMonoBusinessManage *monocular = getMonocular();
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

QString McBinocularMode::getNDFilter()
{
    if(!isConnect()){
        return "";
    }

    return ndFilterToStr(getMonocular()->ML_GetND_XYZFilterChannel(ND_FILTER_KEY));
}

QString McBinocularMode::getXYZFilter()
{
    if (!isConnect())
    {
        return "";
    }

    return xyzFilterToStr(getMonocular()->ML_GetND_XYZFilterChannel(XYZ_FILTER_KEY));
}

Result McBinocularMode::imageRotation(cv::Mat matSrc, cv::Mat &matDest)
{
    if(!ImageRotationConfig::instance()->isRotation()){
        matDest = matSrc.clone();
        return Result();
    }

    CORE::Rotation rotate;
    CORE::Mirror mirror;
    Result ret = getRotationMirror(rotate, mirror);
    if (!ret.success)
    {
        return ret;
    }
    matDest = postporce(matSrc, rotate, mirror);
    return Result();
}

Result McBinocularMode::getRotationMirror(float& rotation, CORE::Mirror& mirror)
{
    ROTATION_MIRROR_TYPE type = MetricsData::instance()->getRotationMirrorType();
    QString mirrorStr;
    Result ret = ImageRotationConfig::instance()->getRotationMirror(type, rotation, mirrorStr);
    if (!ret.success)
    {
        return ret;
    }

    if (mirrorStr == "NO")
    {
        mirror = CORE::NO_OP;
    }
    else if (mirrorStr == "LeftRight")
    {
        mirror = CORE::LEFT_RIGHT;
    }
    else if (mirrorStr == "UpDown")
    {
        mirror = CORE::UP_DOWN;
    }
    else
    {
        return Result(
            false, QString("Image rotation error, rotation=%1, mirror=%2.").arg(rotation).arg(mirrorStr).toStdString());
    }
    return Result();
}

Result McBinocularMode::getRotationMirror(CORE::Rotation &rotate, CORE::Mirror &mirror)
{
    ROTATION_MIRROR_TYPE type = MetricsData::instance()->getRotationMirrorType();
    int rotation;
    QString mirrorStr;
    Result ret = ImageRotationConfig::instance()->getRotationMirror(type, rotation, mirrorStr);
    if (!ret.success)
    {
        return ret;
    }

    if (rotation == 0)
    {
        rotate = CORE::R0;
    }
    else if (rotation == 90)
    {
        rotate = CORE::R90;
    }
    else if (rotation == 180)
    {
        rotate = CORE::R180;
    }
    else if (rotation == 270)
    {
        rotate = CORE::R270;
    }
    else
    {
        return Result(
            false, QString("Image rotation error, rotation=%1, mirror=%2.").arg(rotation).arg(mirrorStr).toStdString());
    }

    if (mirrorStr == "NO")
    {
        mirror = CORE::NO_OP;
    }
    else if (mirrorStr == "LeftRight")
    {
        mirror = CORE::LEFT_RIGHT;
    }
    else if (mirrorStr == "UpDown")
    {
        mirror = CORE::UP_DOWN;
    }
    else
    {
        return Result(
            false, QString("Image rotation error, rotation=%1, mirror=%2.").arg(rotation).arg(mirrorStr).toStdString());
    }
    return Result();
}

Result McBinocularMode::getRotationMirror(int &angle, MLImageDetection::MirrorALG &mirror)
{
    ROTATION_MIRROR_TYPE type = MetricsData::instance()->getRotationMirrorType();
    int rotation;
    QString mirrorStr;
    Result ret = ImageRotationConfig::instance()->getRotationMirror(type, rotation, mirrorStr);
    if (!ret.success)
    {
        return ret;
    }
    angle = rotation;
    if (mirrorStr == "NO")
    {
        mirror = MLImageDetection::NO_OP;
    }
    else if (mirrorStr == "LeftRight")
    {
        mirror = MLImageDetection::LEFT_RIGHT;
    }
    else if (mirrorStr == "UpDown")
    {
        mirror = MLImageDetection::UP_DOWN;
    }
    else
    {
        return Result(
            false, QString("Image rotation error, rotation=%1, mirror=%2.").arg(rotation).arg(mirrorStr).toStdString());
    }
    return Result();

}

BinningMode McBinocularMode::getBinningMode()
{
    //GetCameraBinningMode
    MLMonoBusinessManage *monocular = getMonocular();
    if (monocular == nullptr)
    {
        return BinningMode::AVERAGE;
    }
    return monocular->ML_GetBinningMode();
}

ML::MLFilterWheel::MLFilterEnum McBinocularMode::ndFilterToEnum(QString filter)
{
    QString ndStr = filter.toUpper();
    if (ndStr == "ND0")
    {
        return ML::MLFilterWheel::MLFilterEnum::ND0;
    }
    else if (ndStr == "ND1")
    {
        return ML::MLFilterWheel::MLFilterEnum::ND1;
    }
    else if (ndStr == "ND2")
    {
        return ML::MLFilterWheel::MLFilterEnum::ND2;
    }
    else if (ndStr == "ND3")
    {
        return ML::MLFilterWheel::MLFilterEnum::ND3;
    }
    else if (ndStr == "BLOCK")
    {
        return ML::MLFilterWheel::MLFilterEnum::Block;
    }
    else if (ndStr == "CLEAR")
    {
        return ML::MLFilterWheel::MLFilterEnum::Clear;
    }
    return ML::MLFilterWheel::MLFilterEnum();
}

ML::MLFilterWheel::MLFilterEnum McBinocularMode::xyzFilterToEnum(QString filter)
{
    QString colorStr = filter.toUpper();
    if (colorStr == "X")
    {
        return ML::MLFilterWheel::MLFilterEnum::X;
    }
    else if (colorStr == "Y")
    {
        return ML::MLFilterWheel::MLFilterEnum::Y;
    }
    else if (colorStr == "Z")
    {
        return ML::MLFilterWheel::MLFilterEnum::Z;
    }
    //else if (colorStr == "BLOCK")
    //{
    //    return ML::MLFilterWheel::MLFilterEnum::Block;
    //}
    //TODO: to be add in MC dll
    //else if (colorStr == "EMPTY")
    //{
    //    return ML::MLFilterWheel::MLFilterEnum::EMPTY;
    //}
    else if (colorStr == "CLEAR")
    {
        return ML::MLFilterWheel::MLFilterEnum::Clear;
    }
    else if (colorStr == "YA")
    {
        return ML::MLFilterWheel::MLFilterEnum::Customer1;
    }

    return ML::MLFilterWheel::MLFilterEnum();
}

QString McBinocularMode::ndFilterToStr(ML::MLFilterWheel::MLFilterEnum filter)
{
    QString ndStr;
    switch (filter)
    {
    case ML::MLFilterWheel::MLFilterEnum::ND0:
        ndStr = "ND0";
        break;
    case ML::MLFilterWheel::MLFilterEnum::ND1:
        ndStr = "ND1";
        break;
    case ML::MLFilterWheel::MLFilterEnum::ND2:
        ndStr = "ND2";
        break;
    case ML::MLFilterWheel::MLFilterEnum::ND3:
        ndStr = "ND3";
        break;
    case ML::MLFilterWheel::MLFilterEnum::Block:
        ndStr = "Block";
        break;
    case ML::MLFilterWheel::MLFilterEnum::Clear:
        ndStr = "Clear";
        break;
    default:
        break;
    }
    return ndStr;
}

QString McBinocularMode::xyzFilterToStr(ML::MLFilterWheel::MLFilterEnum filter)
{
    QString filterStr;
    switch (filter)
    {
    case ML::MLFilterWheel::MLFilterEnum::X:
        filterStr = "X";
        break;
    case ML::MLFilterWheel::MLFilterEnum::Y:
        filterStr = "Y";
        break;
    case ML::MLFilterWheel::MLFilterEnum::Z:
        filterStr = "Z";
        break;
    //case ML::MLFilterWheel::MLFilterEnum::Block:
    //    filterStr = "Block";
    //    break;
    case ML::MLFilterWheel::MLFilterEnum::Customer1:
        filterStr = "YA";
        break;
    case ML::MLFilterWheel::MLFilterEnum::Clear:
        filterStr = "Clear";
        break;
    default:
        break;
    }
    return filterStr;
}

MLMonoBusinessManage *McBinocularMode::getMonocular()
{
    std::vector<int> ids = m_bino->ML_GetModulesIDList();
    if (ids.size() == 0){
        return nullptr;
    }
    return m_bino->ML_GetModuleByID(ids.at(0));
}

Result McBinocularMode::setCylinder(QString astigma, int degree, bool judge)
{
    int startTime = QDateTime::currentMSecsSinceEpoch();

    if(judge && m_cameraParam.cylinder.toUpper() == astigma.toUpper() && fabs(m_cameraParam.cylinderAxisDegree - degree) < ZERO_JUDGE){
        return Result();
    }

    bool existPre = false;
    Result ret = moveOtherCylinder(astigma, existPre);
    if (!ret.success)
    {
        return ret;
    }

    ret = getMonocular()->ML_MoveRXFilterByNameSync(astigma.toStdString(), 0);
    if (!ret.success)
    {
        return ret;
    }

    if (!existPre) {
        return ret;
    }

    degree = updateCylindricalRotationOffset(degree);
    ret = getMonocular()->ML_MoveRXFilterByNameSync(astigma.toStdString(), degree);
    if (!ret.success)
    {
        return ret;
    }else{
        m_cameraParam.cylinder = astigma;
        m_cameraParam.cylinderAxisDegree = degree;
    }

    int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    qWarning() << "Cylinder astigma=" << astigma << "D, degree=" << degree << ", times:" << takeTime << " ms...";
    return ret;
}

Result McBinocularMode::setCylinderFilter(QString astigma)
{
    if (!isConnect())
    {
        return Result(false, "Set cylinder filter failed, measure camera is not connected.");
    }

    Result ret = getMonocular()->ML_MoveRXFilterByNameSync(astigma.toStdString(), 0);
    if (!ret.success){
        return ret;
    }

    //TODO: judge moving
    //while (motor->isMoving(cylinderId))
    //{
    //    Sleep(200);
    //}
    return Result();
}


Result McBinocularMode::setRxRotation(int degree)
{
    std::string channel = getMonocular()->ML_GetRXFilterChannel();
    Result ret = setCylinder(QString::fromStdString(channel), degree);
    return ret;
}

Result McBinocularMode::getCylinder(QString &astigma, int &degree)
{
    astigma = QString::fromStdString(getMonocular()->ML_GetRXFilterChannel());
    degree = getMonocular()->ML_GetRXFilterAxis();
    return Result();
}

Result McBinocularMode::clearAlarmFilter()
{
    if (!isConnect())
    {
        return Result(false, "Motion filter clear alarm failed, measure camera is not connected.");
    }

    MLMonoBusinessManage *monocular = getMonocular(); 
    if (isEnabled(ND_FILTER_KEY))
    {
        GrabberBase* camera = dynamic_cast<GrabberBase*> (monocular->ML_GetOneModuleByName(CAMERA_KEY));
        FilterWheelBase *grabberBaseND =
            static_cast<FilterWheelBase *>(monocular->ML_GetOneModuleByName(ND_FILTER_KEY));
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
        FilterWheelBase *grabberBaseXYZ =
            static_cast<FilterWheelBase *>(monocular->ML_GetOneModuleByName(XYZ_FILTER_KEY));
        pluginException ex = grabberBaseXYZ->ML_ClearAlarm();
        if (!ex.getStatusFlag())
        {
            std::string message = "XYZ filter clear alarm error, " + ex.getExceptionMsg();
            LoggingWrapper::instance()->error(QString::fromStdString(message));
            return Result(false, message);
        }
    }

    Result ret = clearAlarmRx();
    return ret;
}

QString McBinocularMode::getMcSn()
{
    return QString::fromStdString(getMonocular()->ML_GetModuleName());
}

Result McBinocularMode::captureImage()
{
    Result ret = updateFocusByColor();
    if (!ret.success)
    {
        return Result(false, QString("Capture images failed.").toStdString());
    }

    MLMonoBusinessManage *monocular = getMonocular();
    ret = monocular->ML_CaptureImageSync();
    if (!ret.success)
    {
        return Result(false, ret.errorMsg);
    }
    return Result();
}

Result McBinocularMode::saveImage(ImageNameInfo imageInfo, const cv::Mat &image, QString fileName)
{
    fileName += "_" + imageInfo.imageFile();
    if (imageInfo.isRawImage)
    {
        fileName += "_raw";
    }
    QString imageName = MetricsData::instance()->getIQRecipeSeqDir() + "\\" + fileName + IMAGE_FILE_EXT;
    
    imageName = getImageName(0, imageName);
    renameExistFile(fileName);

    //bool ret = cv::imwrite(imageName.toStdString(), image);
    Result ret = TaskAsync::instance().saveImage(imageName, image);
    if (!ret.success)
    {
        return Result(false, "Save image error.");
    }
    return Result();
}

Result McBinocularMode::saveRawImg(bool isSave, QString &imgName, const CaptureData &data)
{
    if (!isSave)
    {
        return Result();
    }

    QString path = MetricsData::instance()->getMTFImgsDir() + "raw\\";
    QDir dir(path);
    if (!dir.exists())
    {
        if (!dir.mkpath(path))
        {
            return Result(false, "Save raw image failed, because make raw dir error.");
        }
    }

    QString fileName = path + imgName;
    if (data.ColorFilter == ML::MLFilterWheel::MLFilterEnum::X)
    {
        fileName += "_rawX";
    }
    else if (data.ColorFilter == ML::MLFilterWheel::MLFilterEnum::Y)
    {
        fileName += "_rawY";
    }
    else if (data.ColorFilter == ML::MLFilterWheel::MLFilterEnum::Z)
    {
        fileName += "_rawZ";
    }
    else if (data.ColorFilter == ML::MLFilterWheel::MLFilterEnum::Clear)
    {
        fileName += "_rawClear";
    }
    //else if (QString::fromStdString(data.color).toUpper() == "Empty")
    //{
    //    fileName += "_rawEmpty";
    //}

    fileName += IMAGE_FILE_EXT;

    fileName = getImageName(0, fileName);
    renameExistFile(fileName);

    QFileInfo fileInfo(fileName);
    imgName = fileInfo.completeBaseName();

    //cv::imwrite(fileName.toStdString(), data.Img);
    Result ret = TaskAsync::instance().saveImage(fileName, data.Img);
    return ret;
}

Result McBinocularMode::waitPause(bool isPause)
{
    m_recipePause = isPause;
    if (isPause)
    {
        if (!m_mutex.try_lock())
        {
            return Result(false, "Try pause wait failed.");
        }

        m_waitCondition.wait(&m_mutex);
        m_mutex.unlock();
        m_recipePause = false;
        if (m_isStop)
        {
            return Result(false, "Recipe manual stop.", 1);
        }
    }
    return Result();
}

Result McBinocularMode::saveImageInfo(const QString &imgName, const ML::MLColorimeter::CaptureData &data, Result autoExpInfo)
{
    QString imgPaths = MetricsData::instance()->getMTFImgsDir();
    QFile infoFile(imgPaths + "info.txt");

    bool newFile = false;
    if(!infoFile.exists()){
        newFile = true;
    }

    if (!infoFile.open(QIODevice::Append)){
        return Result(false, "Save info.txt failed, open file error.");
    }

    if(newFile){
        infoFile.write(MetricsData::instance()->getIGParamInfo().toUtf8());
    }

    QString eye = MetricsData::instance()->getReticleEyeType() == 0 ? "Left" : "Right";
    bool isSLB = MetricsData::instance()->getIQSLB();
    QString pupil = QString("%1").arg(MetricsData::instance()->getEyeboxIndexCurrent(), 2, 10, QChar('0'));
    pupil = isSLB ? "0" : pupil;

    QStringList infoLst;
    infoLst.append("fileName:" + imgName);
    //infoLst.append(" DutSn:" + MetricsData::instance()->getDutBarCode());
    infoLst.append(" pupil:" + pupil);
    //infoLst.append(" eyeSide:" + eye);
    infoLst.append(" ndFilter:" + ndFilterToStr(data.NDFilter));
    infoLst.append(" colorFilter:" + xyzFilterToStr(data.ColorFilter));
    infoLst.append(" exposureTime:" + QString::number(data.ExposureTime, 'f', 3));
    infoLst.append(" vid:" + QString::number(calVidByPi(m_cameraParam.focus), 'f', 3));
    infoLst.append(" aperture:" + QString::fromStdString(data.Aperture));
    infoLst.append(" binning:" + QString::number(McUtils::binningToInt(data.Binning)));
    infoLst.append(" PixelFomat:" + McUtils::pixelFormatToStr(data.PixelFormat));
    infoLst.append(" FocusPI:" + QString::number(m_cameraParam.focus, 'f', 3));

    if (1)
    {
        RXCombination rxCom = data.MovementRX;
        infoLst.append(" RxCylinder:" + QString("%1").arg(QString::number(rxCom.Cylinder, 'f', 2)));
        infoLst.append(" RxAxis:" + QString("%1").arg(rxCom.Axis));
    }

    if (!autoExpInfo.success){
        infoLst.append(" expWarnning:" + QString::fromStdString(autoExpInfo.errorMsg));
    }

    infoFile.write(infoLst.join(";").toUtf8());
    infoFile.write("\n");
    infoFile.close();
    return Result();
}

Result McBinocularMode::saveCalibrationImage(QString fileName, const std::map<MLFilterEnum, CaptureData>& dataMap,
    bool luminanceXYZ, bool saveSLBYY, bool luminanceSave, int imgSaveBit)
{
    Result ret;
    std::map<CalibrationEnum, std::map<MLFilterEnum, CaptureData>> calibrationDatas;
    if (luminanceXYZ)
    {
        ret = calibration_XYZ(dataMap, calibrationDatas, fileName, saveSLBYY, imgSaveBit);
    }
    else
    {
        ret = calibration_xyY(dataMap, calibrationDatas, fileName, saveSLBYY);
    }

    if (!ret.success)
    {
        return ret;
    }

    if(luminanceSave){
        cv::Mat imageYY = calibrationDatas[CalibrationEnum::Result][MLFilterEnum::Y].Img;
        ret = saveImagelumCsv(imageYY);
        if (!ret.success)
        {
            return ret;
        }
    }
    return ret;
}

Result McBinocularMode::calibration_YY(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap,
    std::map<CalibrationEnum,
    std::map<MLFilterEnum, CaptureData>> &calibrationDatas, 
    QString fileName, bool saveImage, int imgSaveBit)
{
    CalibrationConfig config;
    config.Aperture = getMonocular()->ML_GetAperture();
    config.NDFilterList = { getMonocular()->ML_GetND_XYZFilterChannel("NDFilter") };
    config.RX = getMonocular()->ML_GetRX();
    config.ColorFilterList = {ML::MLFilterWheel::MLFilterEnum::Y};
    config.LightSourceList = { MetricsData::instance()->getColor().toStdString() };
    config.Bin = m_cameraParam.binning;

    Result ret;
    if (m_calibrationAsync) {
        ret = TaskAsync::instance().calibrationYY(dataMap, config, calibrationDatas, fileName, imgSaveBit);
    }
    else {
        ret = calibration_YY(dataMap, config, calibrationDatas, fileName, MetricsData::instance()->getMTFImgsDir(),
            MetricsData::instance()->getCalibrationImgsDir(),
            m_process, imgSaveBit);
    }
    return ret;
}

Result McBinocularMode::calibration_XYZ(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap, 
    std::map<ML::MLColorimeter::CalibrationEnum,
    std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>> &calibrationDatas,
    QString fileName, bool saveSLBYY, int imgSaveBit)
{
    CalibrationConfig config;
    config.Dark_Flag = false;
    config.Aperture = getMonocular()->ML_GetAperture();
    config.NDFilterList = { getMonocular()->ML_GetND_XYZFilterChannel("NDFilter") };
    config.RX = getMonocular()->ML_GetRX();
    config.LightSourceList = { MetricsData::instance()->getColor().toStdString() };
    config.Bin = m_cameraParam.binning;

    Result ret;
    if (m_calibrationAsync) {
        ret = TaskAsync::instance().calibrationXYZ(dataMap, config, calibrationDatas, fileName, saveSLBYY, false, imgSaveBit);
    }
    else {
        ret = calibration_XYZ(dataMap, config, calibrationDatas, fileName, MetricsData::instance()->getMTFImgsDir(),
            MetricsData::instance()->getCalibrationImgsDir(),
            saveSLBYY, m_process, imgSaveBit);
    }
    return ret;
}

Result McBinocularMode::calibration_xyY(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap,
    std::map<ML::MLColorimeter::CalibrationEnum,
             std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>> &calibrationDatas,
    QString fileName, bool saveSLBYY)
{
    CalibrationConfig config;
    config.Aperture = getMonocular()->ML_GetAperture();
    config.NDFilterList = { getMonocular()->ML_GetND_XYZFilterChannel("NDFilter") };
    config.RX = getMonocular()->ML_GetRX();
    config.LightSourceList = { MetricsData::instance()->getColor().toStdString() };
    config.Bin = m_cameraParam.binning;

    Result ret;
    if (m_calibrationAsync) {
        ret = TaskAsync::instance().calibrationXYZ(dataMap, config, calibrationDatas, fileName, saveSLBYY, true);
    }
    else {
        ret = calibration_xyY(dataMap, config, calibrationDatas, fileName, saveSLBYY);
    }
    return ret;
}

Result McBinocularMode::calibration_YY(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap, 
    CalibrationConfig config, std::map<CalibrationEnum, std::map<MLFilterEnum, CaptureData>>& calibrationDatas, 
    QString fileName, QString fileDir, QString calibrationDir,
    ML::MLColorimeter::MLColorimeterAlgorithms* process, int imgSaveBit)
{
    if (process == nullptr) {
        process = m_process;
    }

    compensationExposure(dataMap);
    Result ret = process->ML_SetCaptureDataMap(dataMap);
    if (!ret.success)
    {
        return ret;
    }

    ret = calibration(config, calibrationDatas, process);
    if (!ret.success)
    {
        return ret;
    }

    {
        QString fileNameBase = fileName;

        QDir dir;
        if (!dir.exists(fileDir))
        {
            return Result(false, QString("Save calibration image failed, [%1] folder does not exist.")
                .arg(fileDir).toStdString());
        }

        fileName = fileDir + fileName + "_Y" + IMAGE_FILE_EXT;

        getImageName(1, fileName, "Y");
        cv::Mat image = calibrationDatas[CalibrationEnum::Result][MLFilterEnum::Y].Img;
        //bool wRet = cv::imwrite((fileName).toStdString(), image);
        ret = TaskAsync::instance().saveImage(fileName, image, imgSaveBit);
        if (!ret.success)
        {
            return Result(false, "Save calibration image failed.");
        }

        if (McUtils::disCompEnabled()) {
            fileName = calibrationDir + fileNameBase + "_Y" + IMAGE_FILE_EXT;
            cv::Mat image = calibrationDatas[CalibrationEnum::Result][MLFilterEnum::Customer9].Img;
            //bool wRet = cv::imwrite((fileName).toStdString(), image);
            ret = TaskAsync::instance().saveImage(fileName, image, imgSaveBit);
            if (!ret.success)
            {
                return Result(false, "Save calibration not distortion compensation image failed.");
            }
        }
    }
    return Result();
}

Result McBinocularMode::calibration_XYZ(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap, 
    CalibrationConfig config, std::map<ML::MLColorimeter::CalibrationEnum, std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>>& calibrationDatas, 
    QString fileName, QString fileDir, QString calibrationDir, bool saveSLBYY, ML::MLColorimeter::MLColorimeterAlgorithms* process, int imgSaveBit)
{
    if(process == nullptr){
        process = m_process;
    }

    compensationExposure(dataMap);
    Result ret = process->ML_SetCaptureDataMap(dataMap);
    if (!ret.success)
    {
        return ret;
    }

    ret = calibration(config, calibrationDatas, process);
    if (!ret.success)
    {
        return ret;
    }

    return saveXYZImage(calibrationDatas, fileName, fileDir, calibrationDir, saveSLBYY, imgSaveBit);
}

Result McBinocularMode::calibration_xyY(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> dataMap, 
    CalibrationConfig config, std::map<ML::MLColorimeter::CalibrationEnum, std::map<ML::MLFilterWheel::MLFilterEnum,
    ML::MLColorimeter::CaptureData>>& calibrationDatas, QString fileName, bool saveSLBYY
    , ML::MLColorimeter::MLColorimeterAlgorithms* process)
{
    if(process == nullptr){
        process = m_process;
    }

    compensationExposure(dataMap);
    Result ret = process->ML_SetCaptureDataMap(dataMap);
    if (!ret.success)
    {
        return ret;
    }

    ret = calibration(config, calibrationDatas, process);
    if (!ret.success)
    {
        return ret;
    }
    ret = saveXYZImage(calibrationDatas, fileName, MetricsData::instance()->getMTFImgsDir(), MetricsData::instance()->getCalibrationImgsDir(), saveSLBYY);
    if (!ret.success)
    {
        return ret;
    }
    return savexyImage(calibrationDatas, fileName);
}

Result McBinocularMode::calibration(
    CalibrationConfig config,
    std::map<ML::MLColorimeter::CalibrationEnum,
             std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>> &calibrationDatas,
    ML::MLColorimeter::MLColorimeterAlgorithms* process)
{
    config.Dark_Flag = true;
    config.FFC_Flag = true;
    config.ColorShift_Flag = true;
    config.Distortion_Flag = true;
    config.Exposure_Flag = true;
    config.FourColor_Flag = false;
    config.Luminance_Flag = true;

    if(process == nullptr){
        process = m_process;
    }

    Result rt = process->ML_LoadCalibrationData(config);
    if (!rt.success)
    {
        return rt;
    }

    rt = process->ML_Process(config);
    if (!rt.success)
    {
        return rt;
    }
    calibrationDatas = process->ML_GetProcessedData();

    rt = distortionCompensation(calibrationDatas);
    if (!rt.success)
    {
        return rt;
    }

    rt = calibrationRotation(calibrationDatas);
    if (!rt.success)
    {
        return rt;
    }
    return rt;
}

Result McBinocularMode::calibrationRotation(
    std::map<ML::MLColorimeter::CalibrationEnum,
             std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>> &calibrationDatas)
{
    if(!ImageRotationConfig::instance()->isRotation()){
        return Result();
    }

    Result ret;
    for (auto &it : calibrationDatas)
    {
        if (!it.second.empty())
        {
            for (auto &item : it.second)
            {
                if (!item.second.Img.empty())
                {
                    ret = imageRotation(item.second.Img, item.second.Img);
                    if (!ret.success)
                    {
                        return ret;
                    }
                }
            }
        }
    }
    return Result();
}

Result McBinocularMode::distortionCompensation(std::map<ML::MLColorimeter::CalibrationEnum, std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>>& calibrationDatas)
{
    if(!McUtils::disCompEnabled()){
        return Result();
    }

    QString keyComp = McUtils::getDisCompKey(MetricsData::instance()->getIQSLB(), MetricsData::instance()->getDutEyeType());

    CaptureData oldCDataX;
    CaptureData oldCDataY;
    CaptureData oldCDataZ;
    {
        std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData> calResult = calibrationDatas[ML::MLColorimeter::CalibrationEnum::Result];
        if(calResult.find(ML::MLFilterWheel::MLFilterEnum::X) != calResult.end()){
            ML::MLColorimeter::CaptureData cData = calResult[ML::MLFilterWheel::MLFilterEnum::X];
            cData.Img = cData.Img.clone();
            oldCDataX = cData;
        }

        if (calResult.find(ML::MLFilterWheel::MLFilterEnum::Y) != calResult.end()) {
            ML::MLColorimeter::CaptureData cData = calResult[ML::MLFilterWheel::MLFilterEnum::Y];
            cData.Img = cData.Img.clone();
            oldCDataY = cData;
        }

        if (calResult.find(ML::MLFilterWheel::MLFilterEnum::Z) != calResult.end()) {
            ML::MLColorimeter::CaptureData cData = calResult[ML::MLFilterWheel::MLFilterEnum::Z];
            cData.Img = cData.Img.clone();
            oldCDataZ = cData;
        }
    }

    for (auto& it : calibrationDatas)
    {
        if (!it.second.empty())
        {
            for (auto& item : it.second)
            {
                if (!item.second.Img.empty())
                {
                    cv::Mat map_x = m_disCompMap[keyComp][McUtils::getBinningNum(item.second.Img.size())]["X"];
                    cv::Mat map_y = m_disCompMap[keyComp][McUtils::getBinningNum(item.second.Img.size())]["Y"];
                    MLIQMetrics::DistortionCompensationRe re = MLIQMetrics::MLDistortionCompensation().getDistortionCompensationMapImg(item.second.Img, map_x, map_y);
                    if (!re.flag)
                    {
                        return Result(false, "Distortion compensation error, " + re.errMsg);
                    }
                    item.second.Img = re.corrtedImg;
                }
            }
        }
    }

    calibrationDatas[ML::MLColorimeter::CalibrationEnum::Result][ML::MLFilterWheel::MLFilterEnum::Customer8] = oldCDataX;
    calibrationDatas[ML::MLColorimeter::CalibrationEnum::Result][ML::MLFilterWheel::MLFilterEnum::Customer9] = oldCDataY;
    calibrationDatas[ML::MLColorimeter::CalibrationEnum::Result][ML::MLFilterWheel::MLFilterEnum::Customer10] = oldCDataZ;
    return Result();
}

Result McBinocularMode::setDisCompMap()
{
    if (!McUtils::disCompEnabled()) {
        return Result();
    }

    cv::Point2f comp;
    bool flag = ImageRotationConfig::instance()->getKeystoneCompensation(comp, MetricsData::instance()->getIQSLB(), MetricsData::instance()->getDutEyeType());
    if(!flag){
        return Result();
    }

    QString folderPath = "./config/DistortionCompensationMap/";
    QDir dir(folderPath);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            return Result(false, QString("Create dir error, %1.").arg(folderPath).toStdString());
        }
    }

    QString keyComp = McUtils::getDisCompKey(MetricsData::instance()->getIQSLB(), MetricsData::instance()->getDutEyeType());
    QString mapX_bin1 = QString("%1_mapX_bin1.tif").arg(keyComp);
    QString mapY_bin1 = QString("%1_mapY_bin1.tif").arg(keyComp);
    QString mapX_bin4 = QString("%1_mapX_bin4.tif").arg(keyComp);
    QString mapY_bin4 = QString("%1_mapY_bin4.tif").arg(keyComp);

    QList<QString> fileList;
    fileList << mapX_bin1 << mapY_bin1 << mapX_bin4 << mapY_bin4;
    bool exist = true;
    for(QString file: fileList){
        QString filePath = folderPath + file;
        if (!QFile::exists(filePath)) {
            exist = false;
            break;
        }
    }

    if(!exist){
        // binning 1X1
        cv::Mat map_x_1;
        cv::Mat map_y_1;
        QString file = folderPath + mapX_bin1;
        MLIQMetrics::MLDistortionCompensation().getDistortionCompensationMap(McUtils::getSizeByBinningNum(1), map_x_1, map_y_1, comp.x, comp.y);
        bool re = cv::imwrite(file.toStdString(), map_x_1);
        if(!re){
            return Result(false, QString("Save Distortion compensation map error, %1.").arg(file).toStdString());
        }

        file = folderPath + mapY_bin1;
        cv::imwrite(file.toStdString(), map_y_1);
        if (!re) {
            return Result(false, QString("Save Distortion compensation map error, %1.").arg(file).toStdString());
        }

        // binning 4X4
        cv::Mat map_x_4;
        cv::Mat map_y_4;
        file = folderPath + mapX_bin4;
        MLIQMetrics::MLDistortionCompensation().getDistortionCompensationMap(McUtils::getSizeByBinningNum(4), map_x_4, map_y_4, comp.x, comp.y);
        re = cv::imwrite(file.toStdString(), map_x_4);
        if (!re) {
            return Result(false, QString("Save Distortion compensation map error, %1.").arg(file).toStdString());
        }
        file = folderPath + mapY_bin4;
        cv::imwrite(file.toStdString(), map_y_4);
        if (!re) {
            return Result(false, QString("Save Distortion compensation map error, %1.").arg(file).toStdString());
        }

        m_disCompMap.clear();
        m_disCompMap[keyComp][1]["X"] = map_x_1;
        m_disCompMap[keyComp][1]["Y"] = map_y_1;
        m_disCompMap[keyComp][4]["X"] = map_x_4;
        m_disCompMap[keyComp][4]["Y"] = map_y_4;
    }

    if(!m_disCompMap.contains(keyComp)){
        QString file = folderPath + mapX_bin1;
        m_disCompMap[keyComp][1]["X"] = cv::imread(file.toStdString(), -1);

        file = folderPath + mapY_bin1;
        m_disCompMap[keyComp][1]["Y"] = cv::imread(file.toStdString(), -1);

        file = folderPath + mapX_bin4;
        m_disCompMap[keyComp][4]["X"] = cv::imread(file.toStdString(), -1);

        file = folderPath + mapY_bin4;
        m_disCompMap[keyComp][4]["Y"] = cv::imread(file.toStdString(), -1);
    }
    return Result();
}

Result McBinocularMode::captureXYZFilterImages(std::map<MLFilterEnum, CaptureData> &xyzData, Result &exposeRet,
                                               QString fileName, XyzExposureCache aeCache,
                                               bool saveRawImage, bool luminanceSave)
{
    if (!isConnect())
    {
        return Result(false, "Save XYZ image failed, camera is not connected.");
    }

    Result ret;
    std::string lightColor = MetricsData::instance()->getColor().toStdString();
    MLFilterEnum xyzName[3] = {MLFilterEnum::X, MLFilterEnum::Y, MLFilterEnum::Z};
    std::map<MLFilterEnum, CaptureData> dataMap;
    for (int i = 0; i < 3; ++i)
    {
        ret = waitPause(m_recipePause);
        if (!ret.success)
        {
            return ret;
        }

        QString xyzFilter = xyzFilterToStr(xyzName[i]);
        ret = setXxzFilter(xyzFilter);
        if (!ret.success)
        {
            return ret;
        }

        ret = waitPause(m_recipePause);
        if (!ret.success)
        {
            return ret;
        }

        ret = EtManage::instance().getExposureTime(aeCache.isAuto, aeCache.initTime[xyzFilter]);
        if (!ret.success) {
            return ret;
        }

        Result exposeRetTmp;
        if (aeCache.isAuto)
        {
            if (xyzFilter.toUpper() == "Z" && MetricsData::instance()->getColor().toUpper() == "R") {
                exposeRetTmp = setExposureTime(100, true);
            }else{
                exposeRetTmp = autoExposure(aeCache.initTime[xyzFilter], aeCache.initTime[xyzFilter] > 0);
            }
        }
        else
        {
            exposeRetTmp = setExposureTime(aeCache.initTime[xyzFilter]);
        }
        if (!exposeRetTmp.success)
        {
            exposeRet = exposeRetTmp;
        }

        if(luminanceSave){
            ret = saveISluminance(xyzFilter);
            if(!ret.success){
                return ret;
            }
        }

        CaptureData cData;
        ret = saveImage(fileName, cData, exposeRetTmp, saveRawImage);

        if (cData.Img.empty()) {
        	std::string result = McUtils::TransFilterEnumToStr(xyzName[i]);
            return Result(false, "Capture XYZ filter images error, Filter " + result + " image is null.");
        }

        dataMap[xyzName[i]] = cData;
        ret = waitPause(m_recipePause);
        if (!ret.success)
        {
            return ret;
        }

        ret = EtManage::instance().writeData();
        if (!ret.success)
        {
            return ret;
        }
    }

    if (dataMap.size() < 3)
    {
        return Result(false, "Capture XYZ filter image error.");
    }

    xyzData = dataMap;
    return ret;
}

Result McBinocularMode::checkLightSourceLC()
{
    Result ret;
    std::array<float, 3> trueSpecbosData = MetricsData::instance()->getSpecbosData();
    QString curColor = MetricsData::instance()->getColor();
    std::array<float, 3> stdSpecbosData = m_LuminChromCig[curColor];

    // 亮度允许范围（百分比）
    float luminLower = stdSpecbosData[0] * (1.0f - luminBiasPerCent);
    float luminUpper = stdSpecbosData[0] * (1.0f + luminBiasPerCent);

    if (trueSpecbosData[0] < luminLower || trueSpecbosData[0] > luminUpper) {
        ret.success = false;
        ret.errorMsg += QString("Luminance out of range [%1, %2], measured: %3\n")
            .arg(luminLower).arg(luminUpper).arg(trueSpecbosData[0]).toStdString();
    }

    // 色度 X 允许范围（固定偏差）
    float chromXLower = stdSpecbosData[1] - chromXBias;
    float chromXUpper = stdSpecbosData[1] + chromXBias;

    if (trueSpecbosData[1] < chromXLower || trueSpecbosData[1] > chromXUpper) {
        ret.success = false;
        ret.errorMsg += QString("ChromX out of range [%1, %2], measured: %3\n")
            .arg(chromXLower).arg(chromXUpper).arg(trueSpecbosData[1]).toStdString();
    }

    // 色度 Y 允许范围（固定偏差）
    float chromYLower = stdSpecbosData[2] - chromYBias;
    float chromYUpper = stdSpecbosData[2] + chromYBias;

    if (trueSpecbosData[2] < chromYLower || trueSpecbosData[2] > chromYUpper) {
        ret.success = false;
        ret.errorMsg += QString("ChromY out of range [%1, %2], measured: %3\n")
            .arg(chromYLower).arg(chromYUpper).arg(trueSpecbosData[2]).toStdString();
    }

    if (ret.success)
        ret.errorMsg = "All luminance and chrom values within allowed range.";

    return ret;
}

Result McBinocularMode::setCameraMotion(int pos)
{
    if (!isConnect())
    {
        return Result(false, "Set camera motion failed, measure camera is not connected.");
    }

    try
    {
        setFocus(pos);
    }
    catch (exception e)
    {
        return Result(false, "Set camera motion error, " + string(e.what()));
    }
    return Result();
}

Result McBinocularMode::saveXYZImage(std::map<ML::MLColorimeter::CalibrationEnum,
    std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>> calibrationDatas,
    QString fileName, QString fileDir, QString calibrationDir, bool saveSLBYY, int imgSaveBit)
{
    QDir dir;
    if (!dir.exists(fileDir))
    {
        return Result(false, QString("Save calibration XYZ image failed, [%1] folder does not exist.")
                                 .arg(fileDir)
                                 .toStdString()); 
    }

    QString fileNameBase = fileName;
    fileName = fileDir + fileName;

    cv::Mat image = calibrationDatas[CalibrationEnum::Result][MLFilterEnum::X].Img;
    QString fileNamePath = fileName + "_X" + IMAGE_FILE_EXT;

    getImageName(2, fileNamePath , "X");
    //bool wRet = cv::imwrite(fileNamePath.toStdString(), image);
    Result ret = TaskAsync::instance().saveImage(fileNamePath, image, imgSaveBit);
    if (!ret.success)
    {
        return Result(false, "Save calibration XX image failed.");
    }

    cv::Mat imageYY = calibrationDatas[CalibrationEnum::Result][MLFilterEnum::Y].Img;
    fileNamePath = fileName + "_Y" + IMAGE_FILE_EXT;

    getImageName(2, fileNamePath, "Y");
    //wRet = cv::imwrite(fileNamePath.toStdString(), imageYY);
    ret = TaskAsync::instance().saveImage(fileNamePath, imageYY, imgSaveBit);
    if (!ret.success)
    {
        return Result(false, "Save calibration YY image failed.");
    }

    image = calibrationDatas[CalibrationEnum::Result][MLFilterEnum::Z].Img;
    fileNamePath = fileName + "_Z" + IMAGE_FILE_EXT;

    getImageName(2, fileNamePath, "Z");
    //wRet = cv::imwrite(fileNamePath.toStdString(), image);
    ret = TaskAsync::instance().saveImage(fileNamePath, image, imgSaveBit);
    if (!ret.success)
    {
        return Result(false, "Save calibration ZZ image failed.");
    }

    if (saveSLBYY)
    {
        QString eyeType = MetricsData::instance()->getReticleEyeType() == 0 ? "left" : "right";
        QString slbluminusY = QString("./config/slbImgs/%2_Y.tif").arg(fileNameBase);
        //cv::Mat imageYYnew = MLChromaPara().getSolidImgRotated(imageYY);
        //wRet = cv::imwrite(slbluminusY.toStdString(), imageYY);
        Result ret = TaskAsync::instance().saveImage(slbluminusY, imageYY);
        if (!ret.success)
        {
            return Result(false, "Save calibration SLB YY image failed.");
        }
    }

    if (McUtils::disCompEnabled()) {
        fileName = calibrationDir + fileNameBase + "_X" + IMAGE_FILE_EXT;
        cv::Mat image = calibrationDatas[CalibrationEnum::Result][MLFilterEnum::Customer8].Img;
        //bool wRet = cv::imwrite((fileName).toStdString(), image);
        ret = TaskAsync::instance().saveImage(fileName, image, imgSaveBit);
        if (!ret.success)
        {
            return Result(false, "Save calibration not distortion compensation image failed.");
        }

        fileName = calibrationDir + fileNameBase + "_Y" + IMAGE_FILE_EXT;
        image = calibrationDatas[CalibrationEnum::Result][MLFilterEnum::Customer9].Img;
        //bool wRet = cv::imwrite((fileName).toStdString(), image);
        ret = TaskAsync::instance().saveImage(fileName, image, imgSaveBit);
        if (!ret.success)
        {
            return Result(false, "Save calibration not distortion compensation image failed.");
        }

        fileName = calibrationDir + fileNameBase + "_Z" + IMAGE_FILE_EXT;
        image = calibrationDatas[CalibrationEnum::Result][MLFilterEnum::Customer10].Img;
        //bool wRet = cv::imwrite((fileName).toStdString(), image);
        ret = TaskAsync::instance().saveImage(fileName, image, imgSaveBit);
        if (!ret.success)
        {
            return Result(false, "Save calibration not distortion compensation image failed.");
        }
    }

    return Result();
}

Result McBinocularMode::savexyImage(std::map<CalibrationEnum, std::map<MLFilterEnum, CaptureData>> calibrationDatas,
                                    QString fileName)
{
    fileName = MetricsData::instance()->getMTFImgsDir() + fileName;

    cv::Mat image = calibrationDatas[CalibrationEnum::Chrom][MLFilterEnum::X].Img;
    QString fileNamePath = fileName + "_xx" + IMAGE_FILE_EXT;

    getImageName(3, fileNamePath, "xx");
    //bool wRet = cv::imwrite(fileNamePath.toStdString(), image);
    Result ret = TaskAsync::instance().saveImage(fileNamePath, image);
    if (!ret.success)
    {
        return Result(false, "Save calibration image failed.");
    }

    image = calibrationDatas[CalibrationEnum::Chrom][MLFilterEnum::Y].Img;
    fileNamePath = fileName + "_yy" + IMAGE_FILE_EXT;

    getImageName(3, fileNamePath, "yy");
    //wRet = cv::imwrite(fileNamePath.toStdString(), image);
    ret = TaskAsync::instance().saveImage(fileNamePath, image);
    if (!ret.success)
    {
        return Result(false, "Save calibration image failed.");
    }

    return Result();
}

cv::Mat McBinocularMode::postporce(cv::Mat mat, CORE::Rotation rotate, CORE::Mirror mirror)
{
    cv::Mat rotateImage;
    cv::Mat mirrorImage;
    MLPostProcessing postProcessing;
    switch (rotate)
    {
    case CORE::R0:
        rotateImage = postProcessing.ImageRotate(enImageRotate::R0, mat);
        break;
    case CORE::R90:
        rotateImage = postProcessing.ImageRotate(enImageRotate::R90, mat);
        break;
    case CORE::R180:
        rotateImage = postProcessing.ImageRotate(enImageRotate::R180, mat);
        break;
    case CORE::R270:
        rotateImage = postProcessing.ImageRotate(enImageRotate::R270, mat);
        break;
    default:
        break;
    }

    switch (mirror)
    {
    case CORE::LEFT_RIGHT:
        mirrorImage = postProcessing.ImageMirror(enImageMirror::LeftRight, rotateImage);
        break;
    case CORE::UP_DOWN:
        mirrorImage = postProcessing.ImageMirror(enImageMirror::UpDown, rotateImage);
        break;
    case CORE::NO_OP:
        mirrorImage = postProcessing.ImageMirror(enImageMirror::Non, rotateImage);
        break;
    default:
        break;
    }
    return mirrorImage;
}

Result McBinocularMode::initAfterConnect()
{
    Result ret = initAfterConnect_1();
    ret = initAfterConnect_2();
    return ret;
}

Result McBinocularMode::initAfterConnect_1()
{
    Result ret = clearAlarmRx();
    if (!ret.success)
    {
        LoggingWrapper::instance()->error("Init after connect error, " + QString::fromStdString(ret.errorMsg));
    }

    ret = clearAlarmFilter();
    if (!ret.success)
    {
        LoggingWrapper::instance()->error("Init after connect error, " + QString::fromStdString(ret.errorMsg));
    }

    updateFocusByColor();

    ret = setXxzFilter("Clear");
    if (!ret.success) {
        LoggingWrapper::instance()->error("Init after connect error, set Y filter failed, " + QString::fromStdString(ret.errorMsg));
    }
    return ret;
}

Result McBinocularMode::initAfterConnect_2()
{
    Result ret = setRxRotation(0);
    if (!ret.success) {
        LoggingWrapper::instance()->error("Init after connect error, set Cylindrical mirror rotation 0 failed, " + QString::fromStdString(ret.errorMsg));
    }

    MLMonoBusinessManage* monocular = getMonocular();
    ret = monocular->ML_CaptureImageSync();
    if (!ret.success)
    {
        return Result(false, QString("Capture images failed.").toStdString());
    }

    //TODO: to be test
    bool isSLB = MetricsData::instance()->getIQSLB();
    QString aperture = ImageRotationConfig::instance()->getAperture(isSLB);
    McBinocularMode::instance()->updateAperture(aperture);

    ret = getCameraParam(m_cameraParam);
    return ret;
}

bool McBinocularMode::isEnabled(std::string key)
{
    if (!isConnect())
    {
        return false;
    }

    bool enabled = false;
    ML::MLColorimeter::ModuleConfig config = getMonocular()->ML_GetBusinessManageConfig()->GetModuleInfo();
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


Result McBinocularMode::clearAlarmRx()
{
    if (!isConnect())
    {
        return Result(false, "Cylindry mirror clear alarm failed, measure camera is not connected.");
    }

    if (!isEnabled(RX_KEY))
    {
        return Result();
    }

    MLMonoBusinessManage *monocular = getMonocular();
    RxFilterWheelBase *grabberBase = static_cast<RxFilterWheelBase *>(monocular->ML_GetOneModuleByName(RX_KEY));
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

Result McBinocularMode::loadFile()
{
    //Load VIDFormula.json
    const QString VID_FORMULA_FILE = ".\\config\\measureCamera\\EYE1\\VIDFormula.json";
    std::ifstream jsonFile(VID_FORMULA_FILE.toStdString());
    if (!jsonFile.is_open())
    {
        return Result(false, QString("Open config file failed, %1.").arg(VID_FORMULA_FILE).toStdString());
    }

    std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
    jsonFile.close();

    Json json = Json::parse(contents);
    if (!json.contains("a") || !json.contains("b") || !json.contains("c"))
    {
        return Result(false, QString("Config file error, %1.").arg(VID_FORMULA_FILE).toStdString());
    }

    m_vidFormulaMap["a"] = json["a"].get<float>();
    m_vidFormulaMap["b"] = json["b"].get<float>();
    m_vidFormulaMap["c"] = json["c"].get<float>();

    m_compensationET = json["cameraEtComp"].get<float>();

    //Load SpectroLuminCheck.json
    const QString LuminCheck_File = ".\\config\\measureCamera\\EYE1\\SpectroLuminCheck.json";
    std::ifstream jFile(LuminCheck_File.toStdString());
    if (!jFile.is_open())
    {
        return Result(false, QString("Open config file failed, %1.").arg(LuminCheck_File).toStdString());
    }

    std::string contents_ = std::string((std::istreambuf_iterator<char>(jFile)), (std::istreambuf_iterator<char>()));
    jFile.close();

    Json json_ = Json::parse(contents_);
    if (!json_.contains("Luminance") || !json_.contains("ChromX") || !json_.contains("ChromY"))
    {
        return Result(false, QString("Config file error, %1.").arg(LuminCheck_File).toStdString());
    }
 
    for (auto key : { "R", "G", "B" })
    {
        m_LuminChromCig[key] = {
            json_["Luminance"][key].get<float>(), 
            json_["ChromX"][key].get<float>(), 
            json_["ChromY"][key].get<float>()
        };
    }
    float luminBiasPerCent = json_["Luminance"]["BiasPercent"].get<float>();
    float chromXBias = json_["ChromX"]["Bias"].get<float>();
    float chromYBias = json_["ChromY"]["Bias"].get<float>();
    return Result();
}

QString McBinocularMode::getImageName(int imgType, QString oldName, QString lumChroma)
{
    if (MetricsData::instance()->getOutputImageDir().isEmpty())
    {
        return oldName;
    }

    bool isSLB = MetricsData::instance()->getIQSLB();

    QString reticle = reticleNameNew(MetricsData::instance()->getCurrentReticle());
    QString lightColor = MetricsData::instance()->getColor();
    QString pupil = QString("%1").arg(MetricsData::instance()->getEyeboxIndexCurrent(), 2, 10, QChar('0'));
    QString eye = MetricsData::instance()->getReticleEyeType() == 0 ? "1" : "2";
    QString ndFilter = getNDFilter();

    QString colorFilter = getXYZFilter();
    //if (colorFilter == "X")
    //    colorFilter = "R";
    //else if (colorFilter == "Y")
    //    colorFilter = "G";
    //else if (colorFilter == "Z")
    //    colorFilter = "B";
    
    QString imageName;
    // raw image
    if (0 == imgType)
    {
        imageName = QString("%1 %2 pupil_%3_EYE%4_%5_%6_raw%7.tif")
                        .arg(reticle)
                        .arg(lightColor)
                        .arg(pupil)
                        .arg(eye)
                        .arg(ndFilter)
                        .arg(lightColor)
                        .arg(colorFilter);
        if(isSLB)
        {
            imageName = QString("%1 %2_EYE%3_%4_%5_raw%6.tif")
                            .arg(reticle)
                            .arg(lightColor)
                            .arg(eye)
                            .arg(ndFilter)
                            .arg(lightColor)
                            .arg(colorFilter);
        }
    }
    // YY image
    else if (1 == imgType){
        imageName = QString("%1 %2 pupil_%3_EYE%4_%5_%6_result%7.tif")
                        .arg(reticle)
                        .arg(lightColor)
                        .arg(pupil)
                        .arg(eye)
                        .arg(ndFilter)
                        .arg(lightColor)
                        .arg(lumChroma);
        if (isSLB)
        {
            imageName = QString("%1 %2_EYE%3_%4_%5_result%6.tif")
                            .arg(reticle)
                            .arg(lightColor)
                            .arg(eye)
                            .arg(ndFilter)
                            .arg(lightColor)
                            .arg(lumChroma);
        }
    }
    // XYZ image
    else if (2 == imgType)
    {
        //if (lumChroma == "X")
        //    colorFilter = "R";
        //else if (colorFilter == "Y")
        //    lumChroma = "G";
        //else if (colorFilter == "Z")
        //    lumChroma = "B";

        imageName = QString("%1 %2 pupil_%3_EYE%4_%5_%6_result%7.tif")
                        .arg(reticle)
                        .arg(lightColor)
                        .arg(pupil)
                        .arg(eye)
                        .arg(ndFilter)
                        .arg(lightColor)
                        .arg(lumChroma);
        if (isSLB)
        {
            imageName = QString("%1 %2_EYE%3_%4_%5_result%6.tif")
                            .arg(reticle)
                            .arg(lightColor)
                            .arg(eye)
                            .arg(ndFilter)
                            .arg(lightColor)
                            .arg(lumChroma);
        }
    }
    // xy image 
    else if(3 == imgType)
    {
        imageName = QString("%1 %2 pupil_%3_EYE%4_%5_%6_result%7.tif")
                        .arg(reticle)
                        .arg(lightColor)
                        .arg(pupil)
                        .arg(eye)
                        .arg(ndFilter)
                        .arg(lightColor)
                        .arg(lumChroma);
        if (isSLB)
        {
            imageName = QString("%1 %2_EYE%3_%4_%5_result%6.tif")
                            .arg(reticle)
                            .arg(lightColor)
                            .arg(eye)
                            .arg(ndFilter)
                            .arg(lightColor)
                            .arg(lumChroma);
        }
    }
    // json file
    else if (4 == imgType)
    {
        if (colorFilter.toLower() == "clear")
        {
            colorFilter = "Y";
        }

        imageName = QString("%1 %2 pupil_%3_EYE%4_%5_%6_%7.json")
                        .arg(reticle)
                        .arg(lightColor)
                        .arg(pupil)
                        .arg(eye)
                        .arg(ndFilter)
                        .arg(lightColor)
                        .arg(colorFilter);
        if (isSLB)
        {
            imageName = QString("%1 %2_EYE%3_%4_%5_%6.json")
                            .arg(reticle)
                            .arg(lightColor)
                            .arg(eye)
                            .arg(ndFilter)
                            .arg(lightColor)
                            .arg(colorFilter);
        }
    }
    

    QString imagePath;
    // raw image
    if (0 == imgType)
    {
        imagePath = MetricsData::instance()->getMTFImgsDir() + "raw\\" + imageName;
    }else{
        imagePath = MetricsData::instance()->getMTFImgsDir() + imageName;
    }

    if (0 != imgType && 4 != imgType)
    {
        m_oldNewImageMap[oldName] = imagePath;
    }
    return imagePath;
}

Result McBinocularMode::saveImageJson(QString filePath)
{
    if (MetricsData::instance()->getOutputImageDir().isEmpty())
    {
        return Result();
    }

    QString eye = MetricsData::instance()->getReticleEyeType() == 0 ? "Left" : "Right";
    bool isSLB = MetricsData::instance()->getIQSLB();

    MLMonoBusinessManage *monocular = getMonocular();
    if (monocular == nullptr)
    {
        return Result(false, "Save image json error, monocular is null.");
    }
    CaptureData data = monocular->ML_GetCaptureData();
    QString ndFilter = m_cameraParam.ndFilter;
    QString colorFilter = m_cameraParam.xyzFilter;
    QString pupil = QString("%1").arg(MetricsData::instance()->getEyeboxIndexCurrent(), 2, 10, QChar('0'));
    pupil = isSLB ? "0" : pupil;
    QString expTime = QString::number(m_cameraParam.exposureTime, 'f', 3);

    Json json;
    json["DutSn"] = MetricsData::instance()->getDutBarCode().toStdString();
    json["lightSourceColor"] = MetricsData::instance()->getColor().toStdString();
    json["pupil"] = pupil.toStdString();
    json["eyeSide"] = eye.toStdString();
    json["ndFilter"] = ndFilter.toStdString();
    json["colorFilter"] = colorFilter.toStdString();
    json["exposureTime"] = expTime.toStdString();
    json["vid"] = QString::number(calVidByPi(m_cameraParam.focus), 'f', 3).toStdString();
    json["aperture"] = data.Aperture;
    json["binning"] = McUtils::binningToInt(m_cameraParam.binning);
    json["PixelFomat"] = McUtils::pixelFormatToStr(m_cameraParam.bitdepth).toStdString();

    if (1)
    {
        RXCombination rxCom = data.MovementRX;
        json["RxCylinder"] = QString("%1").arg(QString::number(rxCom.Cylinder, 'f', 2)).toStdString();
        json["RxAxis"] = QString("%1").arg(QString::number(rxCom.Axis, 'f', 0)).toStdString();
    }

    {
        renameExistFile(filePath);

        QFile file(filePath);
        bool ok = file.open(QIODevice::WriteOnly);
        if (!ok)
        {
            return Result(false, QString("Create file error, %1.").arg(filePath).toStdString());
        }
        file.close();

        std::ofstream ofs(filePath.toStdString());
        if (!ofs.is_open())
        {
            return Result(false, QString("Save file error, %1.").arg(filePath).toStdString());
        }
        ofs << json.dump(4);
        ofs.close();
    }
    return Result();
}

QString McBinocularMode::reticleNameNew(QString name)
{
    QString nameNew = name + "_mismatchPattern";
    if (m_patternJson.contains(name.toStdString()))
    {
        nameNew = QString::fromStdString(m_patternJson[name.toStdString()].get<std::string>());
    }
    return nameNew;

    //if (name.toLower() == QString("Clear").toLower()){
    //    nameNew = "solid white";
    //}
    //else if (name.toLower() == QString("PositiveChecker").toLower())
    //{
    //    nameNew = "+checkerboard";
    //}
    //else if (name.toLower() == QString("NegativeChecker").toLower())
    //{
    //    nameNew = "-checkerboard";
    //}
    //else if (name.toLower() == QString("Cross").toLower())
    //{
    //    nameNew = "crosses";
    //}
    //else if (name.toLower() == QString("Flare").toLower())
    //{
    //    nameNew = "flare";
    //}
    //else if (name.toLower() == QString("Grid").toLower())
    //{
    //    nameNew = "grid";
    //}
    //else if (name.toLower() == QString("Checker0.5deg").toLower())
    //{
    //    nameNew = "checkerboard0.5deg";
    //}
    //else if (name.toLower() == QString("ClearBig").toLower())
    //{
    //    nameNew = "solid white big";
    //}
    //return nameNew;
}

QString McBinocularMode::pixelFormatToString(ML::CameraV2::MLPixelFormat pixelFormat)
{
    QString formatStr;
    if (ML::CameraV2::MLPixelFormat::MLMono8 == pixelFormat)
    {
        formatStr = "MLMono8";
    }
    else
    {
        formatStr = "MLMono12";
    }
    return formatStr;
}

Result McBinocularMode::renameExistFile(QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists())
    {
        QString baseName = fileInfo.completeBaseName();
        QString suffix = fileInfo.suffix();
        QString path = fileInfo.path();

        int counter = 1;
        QString newFileName;
        do
        {
            newFileName = QString("%1/%2_%3.%4").arg(path, baseName, QString::number(counter), suffix);
            counter++;
        } while (QFileInfo(newFileName).exists());

        filePath = newFileName;
    }
    return Result();
}

Result McBinocularMode::saveISluminance(QString XYZFilter)
{
    QString color = MetricsData::instance()->getColor().toUpper();
    Result ret = LumStatistics::instance().saveISluminance(XYZFilter, color);
    return ret;
}

Result McBinocularMode::saveImagelumCsv(cv::Mat image)
{
    QString lightColor = MetricsData::instance()->getColor().toUpper();
    QString file = MetricsData::instance()->getLuminanceFile();
    Result ret = LumStatistics::instance().saveImagelumCsv(file, lightColor, image);
    return ret;
}

Result McBinocularMode::judgeExposure(const CaptureData& data, const QString& fileName)
{
    //return Result();
    QString reticle = MetricsData::instance()->getImageNameInfo().deviceImage;
    bool isCal = false;
    bool warnIn = false;
    if (reticle.toLower() == QString("Solid").toLower() || 
        reticle.toLower() == QString("CheckerNegative").toLower() ||
        reticle.toLower() == QString("CheckerPositive").toLower()) 
    {
        isCal = true;
    }

    if (0) {
        QString color = MetricsData::instance()->getColor();
        if (data.ColorFilter == ML::MLFilterWheel::MLFilterEnum::X) {
            if (color.toUpper() == "R") {
                isCal = true;
            }
        }
        else  if (data.ColorFilter == ML::MLFilterWheel::MLFilterEnum::Y) {
            if (color.toUpper() == "G") {
                isCal = true;
            }
        }
        else  if (data.ColorFilter == ML::MLFilterWheel::MLFilterEnum::Z) {
            if (color.toUpper() == "B") {
                isCal = true;
            }
        }
        else {
            isCal = true;
            warnIn = true;
        }
    }

    if(isCal){
        Result ret = McUtils::judgeExposure(data.Img);
        if (!ret.success) {
            if (warnIn) ret.errorCode = CORE::Wrapper_Warning;
            return Result(false, QString("Image is overexposure, %1, file name is %2.").
                arg(QString::fromStdString(ret.errorMsg)).
                arg(fileName).
                toStdString());
        }
    }
    return Result();
}

Result McBinocularMode::SetMLExposureAuto(int initTime, bool manual)
{
    if (!isConnect()) {
        return Result(false, "Auto exposure failed, measure camera is not connected.");
    }

    bool grab_status = isGrabbing();
    if (grab_status)
    {
        LoggingWrapper::instance()->debug("Auto exposure stop grabbing.");
        stopGrabbing(true);
        Sleep(3000);
    }

    try
    {
        //int startTime = QDateTime::currentMSecsSinceEpoch();

        GrabberBase* grabberBase = getCamera();
        AutoExposureAlgorithmA aExposure("VieCamera1",
            "./config/measureCamera/EYE1/AutoExposureConfigA.json",
            "MLM24259S102M",
            initTime, manual);

        AEStatusA statusAe = aExposure.capture_ae<GrabberBase>(grabberBase);

        //int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
        //double curT = getExposureTime();
        //ImageCaptureInfo info = MetricsData::instance()->getImageCaptureInfo();
        //LoggingWrapper::instance()->debug(
        //    QString("AutoExposureAlgorithmA Auto exposure time: %1 ms.").arg(takeTime));

        if (grab_status)
        {
            LoggingWrapper::instance()->debug("Auto exposure end, restart grabbing.");
            stopGrabbing(false);
        }

        if (statusAe == AEStatusA::AE_too_bright) {
            return Result(false, "Auto exposure error, image is too bright.");
        }
        else if (statusAe == AEStatusA::AE_single_acquisition_error)
        {
            return Result(false, "Auto exposure error, single acquisition error.");
        }
        else if (statusAe == AEStatusA::AE_continuous_acquisition_error)
        {
            return Result(false, "Auto exposure error, continuous acquisition error.");
        }
        else if (statusAe == AEStatusA::AE_continuous_acquisition_error)
        {
            return Result(false, "Auto exposure error, stop acquisition error.");
        }
        else if (statusAe == AEStatusA::AE_time_out)
        {
            return Result(false, "Auto exposure error, time out.");
        }
        return Result();
    }
    catch (const std::exception& e)
    {
        if (grab_status)
        {
            stopGrabbing(false);
        }
        return Result(false, std::string("Auto exposure exception, ") + e.what());
    }
    return Result();
}

int McBinocularMode::updateCylindricalRotationOffset(int angle)
{
    CylindricalRotationOffset info = AutoFocusConfig::GetInstance().getCylindricalRotationOffset();
    float angleNew = angle + info.angleOffset;
    if(!info.rotationDirectionSame){
        angleNew = -angleNew;
        if(angleNew < 0){
            angleNew += 360;
        }

        if(angleNew > 180 && angle < 180){
            angleNew -= 180; 
        }else if(angleNew < 180 && angle > 180){
            angleNew += 180;
        }
    }
    return angleNew;
}

Result McBinocularMode::moveOtherCylinder(QString astigmaCurrent, bool& existPre)
{
    QList<QString> list;
    for(int i = 0; i < 17; ++i){
        float data = -(float)i * 0.25;
        QString str = QString::number(data, 'f', 2).remove(QRegExp("0+$")).remove(QRegExp("\\.$"));
        list << str + "d";
    }

    if(!astigmaCurrent.toLower().contains("d")){
        astigmaCurrent = astigmaCurrent + "d";
    }

    QString astigmaPre;
    QList<QString> astigmaHomings = AutoFocusConfig::GetInstance().getRevolutionHoming();
    if (astigmaHomings.contains(astigmaCurrent)) {
        astigmaPre = list[0];
    }
    else {
        for (int i = 0; i < list.size(); ++i) {
            if (list[i].contains(astigmaCurrent.toLower())) {
                if (i > 0) {
                    astigmaPre = list[i - 1];
                }
                break;
            }
        }
    }

    existPre = false;
    if(!astigmaPre.isEmpty()){
        existPre = true;
        Result ret = getMonocular()->ML_MoveRXFilterByNameSync(astigmaPre.toStdString(), 0.0f);
        if (!ret.success)
        {
            return ret;
        }
    }
    return Result();
}

void McBinocularMode::compensationExposure(std::map<ML::MLFilterWheel::MLFilterEnum, ML::MLColorimeter::CaptureData>& xyzData)
{
    for (auto& pair : xyzData) {
        ML::MLColorimeter::CaptureData &captureData = pair.second;
        captureData.ExposureTime += m_compensationET;
    }
}

Result McBinocularMode::getCameraParam(McCameraParam& cameraParam)
{
    if (!isConnect())
    {
        return Result(false, "Get camera param error, measure camera is not connected.");
    }

    GrabberBase* grabberBase = getCamera();

    cameraParam.stopGrabbing = !isGrabbing();
    cameraParam.binningMode = getBinningMode();
    cameraParam.bitdepth = grabberBase->GetFormatType();
    cameraParam.binning = McUtils::intToBinning(getBinning());

    cameraParam.exposureTime = getExposureTime();

    cameraParam.ndFilter = getNDFilter();
    cameraParam.xyzFilter = getXYZFilter();

    cameraParam.focus = getFocus() / 1000.0f;
    cameraParam.sphereDiopters = getCurrentDiopters();

    QString astigma;
    int degree;
    Result ret = getCylinder(astigma, degree);
    if(!ret.success){
        return ret;
    }
    cameraParam.cylinder = astigma;
    cameraParam.cylinderAxisDegree = degree;
    return Result();
}

Result McBinocularMode::setExposureTime(double ms, bool judge)
{
    if (!isConnect())
    {
        return Result(false, "Set exposure time failed, measure camera is not connected.");
    }

    if (judge && fabs(m_cameraParam.exposureTime - ms) < ZERO_JUDGE) {
        return Result();
    }

    GrabberBase *grabberBase = getCamera();
    grabberBase->SetExposureTime(ms * 1000.0);

    m_cameraParam.exposureTime = ms;
    return Result();
}

Result McBinocularMode::setAdjustExposureTime(double ms, bool judge)
{
    if (!isConnect())
    {
        return Result(false, "Set adjust exposure time failed, measure camera is not connected.");
    }

    if (judge && fabs(m_cameraParam.exposureTime - ms) < ZERO_JUDGE) {
        return Result();
    }

    GrabberBase *grabberBase = getCamera();
    grabberBase->SetAdjustExposureTime(ms * 1000.0);

    m_cameraParam.exposureTime = ms;
    return Result();
}

ML::MLColorimeter::FOVCrop McBinocularMode::getFOVCrop()
{
    //ML::MLColorimeter::FOVCrop crop = MLColorimeterHelp::instance()->ReadJsonToFOVCrop(
    //    (getMonocular()->ML_GetConfigPath() + "/FOVCrop/FOVCrop.json").c_str(), "FOVCrop");

    ML::MLColorimeter::FOVCrop crop = MLColorimeterHelp::instance()->ReadJsonToFOVCrop(
        (string("./config/measureCamera/EYE1") + "/FOVCrop/FOVCrop.json").c_str(), "FOVCrop");
    return crop;
}

cv::Point2f McBinocularMode::getFOVCenter()
{
    ML::MLColorimeter::FOVCrop crop = getFOVCrop();
    return crop.Center;
}

float McBinocularMode::getCurrentDiopters(QString color)
{
    if(!isConnect()){
        return 0.0f;
    }

    float position = McBinocularMode::instance()->getFocus() / 1000.0f;
    float bestFocus = McBinocularMode::instance()->getBestFocus(color);
    double dif = position - bestFocus;
    if (abs(dif) < ZERO_JUDGE)
        return 0.0;
    else
    {
        /*double vid = 0 - calVidByPi(position);
        return vid  / 1000.0f;*/
        double vid = calVidByPi(position);
        return -1.0 / vid;
    }
}

float McBinocularMode::calPiByVid(float VID)
{
    // "c" is best focus
    //float piCoord = m_vidFormulaMap["a"] / (VID + m_vidFormulaMap["b"]) + m_vidFormulaMap["c"];
    VID *= 1000.0;
    float bestFocus = getBestFocus();
    float piCoord = m_vidFormulaMap["a"] / (VID + m_vidFormulaMap["b"]) + bestFocus;
    return piCoord;
}

float McBinocularMode::calVidByPi(float piCoord)
{
    // "c" is best focus
    // float VID = m_vidFormulaMap["a"] / (piCoord - m_vidFormulaMap["c"]) - m_vidFormulaMap["b"];

    float bestFocus = getBestFocus();
    float VID = m_vidFormulaMap["a"] / (piCoord - bestFocus) - m_vidFormulaMap["b"];
    VID = VID / 1000.0;
    return VID;
}

QString McBinocularMode::calCurrentVidByPi()
{
    QString vidStr;
    float position = getFocus();
    double curpos = (double)position / 1000.0;

    float bestFocus = getBestFocus() * 1000.0;
    double dif = (position - bestFocus) / 1000.0;
    if (abs(dif) < ZERO_JUDGE)
        vidStr = "inf";
    else
    {
        double vid = McBinocularMode::instance()->calVidByPi(curpos);
        //vid = 0 - vid;
        //vidStr = QString::number(vid / 1000, 'f', 3);
        vidStr = QString::number(vid, 'f', 3);
    }
    return vidStr;
}

float McBinocularMode::calPiByDiopters(float diopters)
{
    if(abs(diopters) < ZERO_JUDGE){
        float bestFocus = getBestFocus();
        return bestFocus;
    }

    float vid = -1.0f / diopters;
    return calPiByVid(vid);
}

float McBinocularMode::calDioptersByPi(float piCoord)
{
    float vid = calVidByPi(piCoord);
    return -1.0 / vid;
}

Result McBinocularMode::updateFocusByColor()
{
    QString color = MetricsData::instance()->getColor();
    return updateFocusByColor(color);
}

Result McBinocularMode::updateFocusByColor(QString color)
{
    if (!m_syncBestFocus)
    {
        return Result();
    }

    if (!isConnect())
    {
        return Result(false,
                      QString("Update focus by color, color is %1, measure camera is not connected.").arg(color).toStdString());
    }

    if (color.isEmpty())
    {
        return Result();
    }

    float focus = AutoFocusConfig::GetInstance().getPiByColor(color.toUpper().toStdString());
    Result ret = setFocus(focus * 1000.0);
    if (!ret.success)
    {
        return Result(false,
                      QString("Update focus by color, color is %1, focus is %2").arg(color).arg(focus).toStdString() +
                                 ret.errorMsg);
    }
    return ret;
}

float McBinocularMode::getBestFocus(QString color)
{
    //if (!m_syncBestFocus)
    //{
    //    motionInfo focusInfo = AutoFocusConfig::GetInstance().GetAutoFocusInfo();
    //    return focusInfo.focusInfo.referencePos;
    //}

    if(color.isEmpty()){
        color = MetricsData::instance()->getColor();
    } 
    float bestFocus = AutoFocusConfig::GetInstance().getPiByColor(color.toUpper().toStdString());
    return bestFocus;
}

bool McBinocularMode::getSyncBestFocus()
{
    return m_syncBestFocus;
}

void McBinocularMode::setSyncBestFocus(bool sync, bool isUpdateDiopter)
{
    m_syncBestFocus = sync;

    //TODO: Green
    //if(sync){
    //    MetricsData::instance()->setDiopter(0.0f);
    //}else{
    //    float diop = getCurrentDiopters("G");
    //    MetricsData::instance()->setDiopter(diop);
    //}

    if (isUpdateDiopter) {
        MetricsData::instance()->setDiopter(0.0f);
    }
}

Result McBinocularMode::updateOutputImageName()
{
    QMap<QString, QString>::iterator iter = m_oldNewImageMap.begin();
    for (; iter != m_oldNewImageMap.end(); ++iter){
        QString oldName = iter.key();
        QString newName = iter.value();
        renameExistFile(newName);

        //QFile file(newName);
        //if (file.exists())
        //{
        //    file.remove();
        //}

        if (!QFile::rename(oldName, newName))
        {
            LoggingWrapper::instance()->warn(QString("Image rename error, old name %1, new name %2.").arg(oldName).arg(newName));
            //return Result(false, QString("Image rename error, old name %1, new name %2.").arg(oldName).arg(newName).toStdString());
        }
    }

    m_oldNewImageMap.clear();
    return Result();
}

Result McBinocularMode::loadPatternNameFile()
{
    const QString PATTERN_FILE_NAME = ".\\config\\PatternName.json";
    {
        std::ifstream jsonFile(PATTERN_FILE_NAME.toStdString());
        if (!jsonFile.is_open())
        {
            return Result(false, QString("Open config file failed, %1.").arg(PATTERN_FILE_NAME).toStdString());
        }

        std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
        jsonFile.close();

        try
        {
            m_patternJson = Json::parse(contents);
        }
        catch (const json::parse_error &e)
        {
            return Result(false, QString("Parsing config file error, %1. ").arg(PATTERN_FILE_NAME).toStdString() + e.what());
        }
    }

    QList<QString> reticleList;
    const QString RETICLE_FILE_NAME = ".\\config\\device\\reticle2D.json";
    {
        std::ifstream jsonFile(RETICLE_FILE_NAME.toStdString());
        if (!jsonFile.is_open())
        {
            return Result(false, QString("Open config file failed, %1.").arg(RETICLE_FILE_NAME).toStdString());
        }

        std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
        jsonFile.close();

        Json reticleJson;
        try
        {
            reticleJson = Json::parse(contents);
        }
        catch (const json::parse_error &e)
        {
            return Result(false, QString("Parsing config file error, %1. ").arg(RETICLE_FILE_NAME).toStdString() + e.what());
        }

        Json reticleposMap = reticleJson["leftEye"]["reticlepos"];
        if (reticleposMap.is_null())
        {
            return Result(false, QString("Get reticle info error, %1. ").arg(RETICLE_FILE_NAME).toStdString());
        }

        for (auto &[key, value] : reticleposMap.items())
        {
            QString keyReticle = QString::fromStdString(key);
            reticleList.push_back(keyReticle);
        }
    }

    for (QString reticle : reticleList)
    {
        if (!m_patternJson.contains(reticle.toStdString()))
        {
            return Result(false, QString("Reticle name %1 is not exist in pattern name file, please check %2 and %3.")
                                     .arg(reticle)
                                     .arg(PATTERN_FILE_NAME)
                                     .arg(RETICLE_FILE_NAME)
                                     .toStdString());
        }
    }

    return Result();
}

ML::MLColorimeter::MLColorimeterAlgorithms* McBinocularMode::getProcess()
{
    return m_process;
}

QString McBinocularMode::getProcessPath()
{
    return CONFIG_PATH + "\\EYE1";
}

