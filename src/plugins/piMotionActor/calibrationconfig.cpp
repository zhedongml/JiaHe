#include "calibrationconfig.h"
#include <QDebug>
#include <QFile>
#include "Core/icore.h"
#include <QDateTime>
#include "PrjCommon/metricsdata.h"
#include "PrjCommon/PrjCommon.h"

CalibrationConfig *CalibrationConfig::self = nullptr;
CalibrationConfig *CalibrationConfig::instance()
{
    if (self == nullptr)
    {
        self = new CalibrationConfig();
    }
    return self;
}
float CalibrationConfig::getDutRotationAxisLengthY(CORE::DUT_EYE_TYPE eyeType)
{
    if (eyeType == CORE::DUT_EYE_LEFT)
    {
        return m_JsonCalibrateConfig["DutRotationAxisLength"]["LeftEye"]["Y"].get<float>();
    }else{
        return m_JsonCalibrateConfig["DutRotationAxisLength"]["RightEye"]["Y"].get<float>();
    }
}

DutRotationAxisInfo CalibrationConfig::getDutRotationAxisLength(CORE::DUT_EYE_TYPE eyeType)
{
    DutRotationAxisInfo info;
    if (eyeType == CORE::DUT_EYE_LEFT)
    {
        info.axisLength.x = m_JsonCalibrateConfig["DutRotationAxisLength"]["LeftEye"]["X"].get<float>();
        info.axisLength.y = m_JsonCalibrateConfig["DutRotationAxisLength"]["LeftEye"]["Y"].get<float>();
        info.initAngle.x = m_JsonCalibrateConfig["DutRotationAxisLength"]["LeftEye"]["InitAngleX"].get<float>();
        info.initAngle.y = m_JsonCalibrateConfig["DutRotationAxisLength"]["LeftEye"]["InitAngleY"].get<float>();
    }
    else
    {
        info.axisLength.x = m_JsonCalibrateConfig["DutRotationAxisLength"]["RightEye"]["X"].get<float>();
        info.axisLength.y = m_JsonCalibrateConfig["DutRotationAxisLength"]["RightEye"]["Y"].get<float>();
        info.initAngle.x = m_JsonCalibrateConfig["DutRotationAxisLength"]["RightEye"]["InitAngleX"].get<float>();
        info.initAngle.y = m_JsonCalibrateConfig["DutRotationAxisLength"]["RightEye"]["InitAngleY"].get<float>();
    }

    return info;
}

float CalibrationConfig::getLensInfinitelyCloseDutMotionZ()
{
    return m_JsonCalibrateConfig["LensInfinitelyCloseDutMotionZ"].get<float>();
}

cv::Point2f CalibrationConfig::getMvAlignProjectorCoord()
{
    cv::Point2f point;
    point.x = m_JsonCalibrateConfig["MvAlignProjectorCoord"]["X"].get<float>();
    point.y = m_JsonCalibrateConfig["MvAlignProjectorCoord"]["Y"].get<float>();
    return point;
}

CalibrationConfig::CalibrationConfig()
{
    load();
}
DutConfig CalibrationConfig::loadDutConfig(const QString &sn)

{
    QString dutFile = dutFileName(sn);
    if (!QFile::exists(dutFile)){
        return DutConfig();
    }

    // m_dutConfig
    std::ifstream ifs(dutFile.toStdString(), std::fstream::in);
    ifs >> m_JsonDutConfig;
    DutConfig config;
    config.fiducials;
    Json valFiducialPos = m_JsonDutConfig["FiducialPos"];
    if (!valFiducialPos.is_null())
    {
        for (auto it = valFiducialPos.begin(); it != valFiducialPos.end(); ++it)
        {
            Json val = it.value();

            cv::Point3f pos;
            pos.x = val["X"].get<float>();
            pos.y = val["Y"].get<float>();
            pos.z = val["Z"].get<float>();
            config.fiducials.push_back(pos);
        }
    }

    Json valCenterPos = m_JsonDutConfig["EyeboxCenterPos"];
    if (!valCenterPos.is_null())
    {
        config.eyeboxCenter.x = valCenterPos["X"].get<float>();
        config.eyeboxCenter.y = valCenterPos["Y"].get<float>();
        config.eyeboxCenter.z = valCenterPos["Z"].get<float>();
        //config.eyeboxs.push_back(cv::Point3f(config.eyeboxCenter.x, config.eyeboxCenter.y, config.eyeboxCenter.z));

        Json valEyeboxOffset = m_JsonDutConfig["EyeboxOffset"];
        QStringList offsetList;
        if (!valEyeboxOffset.is_null())
        {
            config.exeboxOffset = valEyeboxOffset.get<std::string>();
        }
    }

    config.exeboxOffsetAll = readEyeboxOffsetAll(m_JsonDutConfig);

    Json igPos = m_JsonDutConfig["InputGratingPos"];
    if (!igPos.is_null())
    {
        config.inputGrating.x = igPos["X"].get<float>();
        config.inputGrating.y = igPos["Y"].get<float>();
        config.inputGrating.z = igPos["Z"].get<float>();
    }

    Json valEyeRelief = m_JsonDutConfig["EyeRelief"];
    if (!valEyeRelief.is_null())
    {
        config.eyeRelief = valEyeRelief.get<float>();
    }

    config.sn = QString::fromStdString(m_JsonDutConfig["SN"].get<std::string>());
    config.eyeType = QString::fromStdString(m_JsonDutConfig["EyeType"].get<std::string>()).toLower().contains("left") ? 0: 1;

    m_calibrateConfig.acsMvCoordinateOfFiducials = readFiducialMotion3DPos(m_JsonDutConfig);
    config.motPosOfFids = readFiducialMotion3DPosList(m_JsonDutConfig);

    config.mvRelativeDutInputGrating = readMvRelativeDutInputGrating(m_JsonCalibrateConfig).value((CORE::DUT_EYE_TYPE)config.eyeType);
    config.calculateMvFidsByInputFlag = readCalculateMvFidsByInputFlag(m_JsonCalibrateConfig);
    config.moveOutInfo = getMoveOutInfo(m_JsonCalibrateConfig);

    return config;
}
cv::Point3f CalibrationConfig::loadEyeTypeConfig(CORE::DUT_EYE_TYPE type)
{
    return readMvRelativeDutInputGrating(m_JsonCalibrateConfig).value(type);
}
bool CalibrationConfig::loadDutBase()
{
    std::ifstream ifs(dutBasefile_name.toStdString(), std::fstream::in);
    ifs >> m_JsonDutBase;
    return true;
}

DutConfig &CalibrationConfig::getDutConfigReference()
{
    return m_dutConfig;
}

DutConfig CalibrationConfig::getDutConfig(const QString &sn)
{
    if (sn.isEmpty()){
        return m_dutConfig;
    }else{
        DutConfig config = loadDutConfig(sn);
        return config;
    }
}

void CalibrationConfig::updateDutEyeboxs()
{
    updateDutEyeboxs(m_dutConfig);
}

void CalibrationConfig::updateDutEyeboxs(DutConfig &dutConfig)
{
    if (false)
    {
        dutConfig.eyeboxs.clear();
        QStringList offsetList = QString::fromStdString(dutConfig.exeboxOffset).split(',');
        if (offsetList.size() == 1)
        {
            float offset = offsetList[0].toFloat();
            dutConfig.eyeboxs.push_back(
                cv::Point3f(dutConfig.eyeboxCenter.x, dutConfig.eyeboxCenter.y, dutConfig.eyeboxCenter.z));
            dutConfig.eyeboxs.push_back(
                cv::Point3f(dutConfig.eyeboxCenter.x, dutConfig.eyeboxCenter.y + offset, dutConfig.eyeboxCenter.z));
            dutConfig.eyeboxs.push_back(
                cv::Point3f(dutConfig.eyeboxCenter.x, dutConfig.eyeboxCenter.y - offset, dutConfig.eyeboxCenter.z));
            dutConfig.eyeboxs.push_back(
                cv::Point3f(dutConfig.eyeboxCenter.x - offset, dutConfig.eyeboxCenter.y, dutConfig.eyeboxCenter.z));
            dutConfig.eyeboxs.push_back(
                cv::Point3f(dutConfig.eyeboxCenter.x + offset, dutConfig.eyeboxCenter.y, dutConfig.eyeboxCenter.z));
        }
    }
    else
    {
        dutConfig.eyeboxs.clear();
        cv::Point3f center = dutConfig.eyeboxCenter;
        std::vector<cv::Point3f> eyeboxs = dutConfig.exeboxOffsetAll;
        for each (cv::Point3f offset in eyeboxs)
        {
            dutConfig.eyeboxs.push_back(center + offset);
        }
    }
}

bool CalibrationConfig::writeDutConfig(DutConfig config)
{
    Json writeDutConfig;
    QList<QString> snList = getDutSns();
    if (!snList.contains(config.sn))
    {
        writeDutConfig = addDutConfig(config);

    }else{
        writeDutConfig = updateDutConfig(config);
    }

    std::ofstream ofs(dutFileName(config.sn).toStdString());
    if (!ofs.is_open())
    {
        return false;
    }
    ofs << writeDutConfig.dump(4);
    ofs.close();

    refreshDut();
    return true;
}

Result CalibrationConfig::deleteDutConfig(const QString &sn)
{
    bool ok = deleteSnName(sn);
    if (!ok)
    {
        return Result(false, "Delete " + sn.toStdString() + "DUT config failed.");
    }

    QString file = dutFileName(sn);
    QString fileNew = dutfilePath_name +
        QString("delete_%1_dut_%2.json")
        .arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"))
        .arg(sn);

    ok = QFile::rename(file, fileNew);
    if (!ok){
        return Result(false, "Delete " + sn.toStdString() + "DUT config failed, Please let the engineer handle it.");
    }

    return Result();
}

bool CalibrationConfig::writeMotPosOfFids(QString sn, const std::vector<cv::Point3f> &mvFiducials)
{
    Json writeCalibrateConfig = dutJson(sn.toStdString());
    if (writeCalibrateConfig.is_null()){
        qCritical() << "Write motion position of fiducial error, read DUT json is null.";
        return false;
    }

    writeCalibrateConfig["MotPosOfFids"].clear();
    int i = 0;
    for (auto fiducial : mvFiducials)
    {
        Json fiCofig;
        fiCofig["X"] = (int)round((fiducial.x * 1000)) / 1000.0;
        fiCofig["Y"] = (int)round((fiducial.y * 1000)) / 1000.0;
        fiCofig["Z"] = (int)round((fiducial.z * 1000)) / 1000.0;
        fiCofig["index"] = i;
        writeCalibrateConfig["MotPosOfFids"].push_back(fiCofig);
        i++;
    }

    std::ofstream ofs(dutFileName(sn).toStdString());
    if (!ofs.is_open())
    {
        return false;
    }
    ofs << writeCalibrateConfig.dump(4);
    ofs.close();

    refreshDut();
    return true;
}

bool CalibrationConfig::writeEyeboxAcsCoords(QString sn, std::vector<cv::Point3f> eyeboxAcsCoords)
{
    Json writeEyeboxAcsConfig = m_JsonDutConfig;
    if (writeEyeboxAcsConfig.contains("MotPosOfEyeboxs"))
    {
        writeEyeboxAcsConfig["MotPosOfEyeboxs"].clear();
    }
    int i = 0;
    for (auto coord : eyeboxAcsCoords)
    {
        Json fiCofig;
        fiCofig["X"] = (int)round((coord.x * 1000)) / 1000.0;
        fiCofig["Y"] = (int)round((coord.y * 1000)) / 1000.0;
        fiCofig["Z"] = (int)round((coord.z * 1000)) / 1000.0;
        fiCofig["index"] = i;
        writeEyeboxAcsConfig["MotPosOfEyeboxs"].push_back(fiCofig);
        i++;
    }
    std::ofstream ofs(dutFileName(sn).toStdString());
    if (!ofs.is_open())
    {
        return false;
    }
    ofs << writeEyeboxAcsConfig.dump(4);
    ofs.close();

    load();
    return false;
}

std::vector<cv::Point3f> CalibrationConfig::readEyeboxAcsCoords()
{
    std::vector<cv::Point3f> eyeboxCoords;
    if (!m_JsonDutConfig.contains("MotPosOfEyeboxs"))
    {
        return eyeboxCoords;
    }

    Json eyeboxCoordsJson = m_JsonDutConfig["MotPosOfEyeboxs"];
    if (!eyeboxCoordsJson.is_null())
    {
        for (auto it = eyeboxCoordsJson.begin(); it != eyeboxCoordsJson.end(); ++it)
        {
            Json val = it.value();

            cv::Point3f pos;
            pos.x = val["X"].get<float>();
            pos.y = val["Y"].get<float>();
            pos.z = val["Z"].get<float>();
            eyeboxCoords.push_back(pos);
        }
    }
    return eyeboxCoords;
}

AlignConfig &CalibrationConfig::getAlignConfigReference()
{
    return m_alignConfig;
}
AlignConfig CalibrationConfig::getAlignConfig()
{
    return m_alignConfig;
}
QMap<QString, QString> CalibrationConfig::getMotion3DAndDutCoordRelate()
{
    return m_calibrateConfig.motion3DAndDutCoordRelate;
}

QMap<QString, QString> CalibrationConfig::getMotion3DAndMVCoordRelate()
{
    return m_calibrateConfig.motion3DAndMVCoordRelate;
}

cv::Point2f CalibrationConfig::getMeasureCameraAngle()
{
    return m_calibrateConfig.measureCameraAngle;
}

cv::Point2f CalibrationConfig::getDutAngleXY()
{
    return m_calibrateConfig.dutAngleXY;
}

float CalibrationConfig::getMvFocus()
{
    return m_calibrateConfig.mvFocus;
}

CalibrateConfig &CalibrationConfig::getCalibrateConfig()
{
    return m_calibrateConfig;
}

CalibrationConfig::~CalibrationConfig()
{
    if (self != nullptr)
        delete self;
}

void CalibrationConfig::load()
{
    std::ifstream jsonFile(FILE_NAME.toStdString());
    if (jsonFile.is_open())
    {
        std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
        jsonFile.close();
        Json jsonObj = Json::parse(contents);
        m_JsonCalibrateConfig = jsonObj;

        m_calibrateConfig.motion3DAndDutCoordRelate = readMotion3DAndDutCoordRelate(jsonObj);
        m_calibrateConfig.motion3DAndMVCoordRelate = readMotion3DAndMVRelate(jsonObj);
        m_calibrateConfig.mvFocus = readMvFocus(jsonObj);
    }

    refreshDut();
}

cv::Point3f CalibrationConfig::getMVCameraRelPos()
{
    return readMVCameraRelPos(m_JsonCalibrateConfig);
}

cv::Point3f CalibrationConfig::getKeyenceRelPos()
{
    return readKeyenceRelPos(m_JsonCalibrateConfig);
}

cv::Point3f CalibrationConfig::getMeasureRelPos()
{
    return readMeasureRelPos(m_JsonCalibrateConfig);
}

cv::Point3f CalibrationConfig::getInputGratingForBaseZ()
{
    //TODO: not used
    // m_inputGratingForBaseZ
    cv::Point3f pos;
    pos.x = -299;

    if (m_dutConfig.fiducials.size() == 0)
    {
        return cv::Point3f();
    }

    pos.x = m_dutConfig.fiducials.at(0).x;
    pos.x = m_dutConfig.fiducials.at(0).y;
    return pos;
}

std::vector<cv::Point3f> CalibrationConfig::getFiducialMotion3DCoords()
{
    std::vector<cv::Point3f> posVec;
    QMap<int, cv::Point3f>::Iterator iter = m_calibrateConfig.acsMvCoordinateOfFiducials.begin();
    while (iter != m_calibrateConfig.acsMvCoordinateOfFiducials.end())
    {
        posVec.push_back(iter.value());
        ++iter;
    }
    return posVec;
}

QMap<int, cv::Point3f> CalibrationConfig::readFiducialMotion3DPos(Json jsonObj)
{
    QMap<int, cv::Point3f> posMap;
    if (!jsonObj.contains("MotPosOfFids"))
    {
        return posMap;
    }

    Json vals = jsonObj["MotPosOfFids"];
    for (auto it = vals.begin(); it != vals.end(); ++it)
    {
        Json val = it.value();
        int id = val["index"].get<int>();

        cv::Point3f pos;
        pos.x = val["X"].get<float>();
        pos.y = val["Y"].get<float>();
        pos.z = val["Z"].get<float>();
        posMap[id] = pos;
    }
    return posMap;
}

std::vector<cv::Point3f> CalibrationConfig::readEyeboxOffsetAll(Json jsonObj)
{
    std::vector<cv::Point3f> list;
    Json vals = jsonObj["EyeboxOffsetAll"];
    for (auto it = vals.begin(); it != vals.end(); ++it)
    {
        Json val = it.value();
        cv::Point3f pos;
        pos.x = val["X"].get<float>();
        pos.y = val["Y"].get<float>();
        pos.z = val["Z"].get<float>();
        list.push_back(pos);
    }

    if(list.size() == 0){
        list.push_back(cv::Point3f(0,0,0));
    }
    return list;
}

std::vector<cv::Point3f> CalibrationConfig::readFiducialMotion3DPosList(Json jsonObj)
{
    std::vector<cv::Point3f> posList;

    QMap<int, cv::Point3f> map = readFiducialMotion3DPos(jsonObj);
    QList<int> keys = map.keys();
    std::sort(keys.begin(), keys.end());
    for (int i = 0; i < keys.size(); i++)
    {
        posList.push_back(map.value(keys[i]));
    }

    return posList;
}

cv::Point3f CalibrationConfig::readMVCameraRelPos(Json jsonObj)
{
    cv::Point3f pos;
    pos.x = jsonObj["MVCameraPostion"]["posX"].get<float>();
    pos.y = jsonObj["MVCameraPostion"]["posY"].get<float>();
    pos.z = jsonObj["MVCameraPostion"]["posZ"].get<float>();
    return pos;
}

cv::Point3f CalibrationConfig::readKeyenceRelPos(Json jsonObj)
{
    cv::Point3f pos;
    pos.x = jsonObj["KeyencePostion"]["posX"].get<float>();
    pos.y = jsonObj["KeyencePostion"]["posY"].get<float>();
    pos.z = jsonObj["KeyencePostion"]["posZ"].get<float>();
    return pos;
}

cv::Point3f CalibrationConfig::readMeasureRelPos(Json jsonObj)
{
    cv::Point3f pos;
    pos.x = jsonObj["MeasureCameraPostion"]["posX"].get<float>();
    pos.y = jsonObj["MeasureCameraPostion"]["posY"].get<float>();
    pos.z = jsonObj["MeasureCameraPostion"]["posZ"].get<float>();
    return pos;
}

QMap<QString, QString> CalibrationConfig::readMotion3DAndDutCoordRelate(Json jsonObj)
{
    QMap<QString, QString> map;
    map[CC_MOTION_3D_X] =
        QString::fromStdString(jsonObj["motion3DAndDutCoordinateRelate"][CC_MOTION_3D_X].get<std::string>());
    map[CC_MOTION_3D_Y] =
        QString::fromStdString(jsonObj["motion3DAndDutCoordinateRelate"][CC_MOTION_3D_Y].get<std::string>());
    return map;
}

QMap<QString, QString> CalibrationConfig::readMotion3DAndMVRelate(Json jsonObj)
{
    QMap<QString, QString> map;
    map[CC_MOTION_3D_X] =
        QString::fromStdString(jsonObj["motion3DAndMVRelate"][CC_MOTION_3D_X].get<std::string>());
    map[CC_MOTION_3D_Y] =
        QString::fromStdString(jsonObj["motion3DAndMVRelate"][CC_MOTION_3D_Y].get<std::string>());
    return map;
}

float CalibrationConfig::readMvFocus(Json jsonObj)
{
    if (!jsonObj.contains("mvFocus"))
    {
        qCritical() << "Read eyeboxAlignJX.json error,  'mvFocus' is not exist.";
        return 0.0f;
    }

    return jsonObj["mvFocus"].get<float>();
}

QMap<CORE::DUT_EYE_TYPE, cv::Point3f> CalibrationConfig::readMvRelativeDutInputGrating(Json jsonObj)
{
    QMap<CORE::DUT_EYE_TYPE, cv::Point3f> mapPoint;
    cv::Point3f left;
    left.x = jsonObj["MvRelativeDutInputGrating"]["LeftEye"]["X"].get<float>();
    left.y = jsonObj["MvRelativeDutInputGrating"]["LeftEye"]["Y"].get<float>();
    left.z = jsonObj["MvRelativeDutInputGrating"]["LeftEye"]["Z"].get<float>();
    mapPoint[DUT_EYE_LEFT] = left;

    cv::Point3f right;
    right.x = jsonObj["MvRelativeDutInputGrating"]["RightEye"]["X"].get<float>();
    right.y = jsonObj["MvRelativeDutInputGrating"]["RightEye"]["Y"].get<float>();
    right.z = jsonObj["MvRelativeDutInputGrating"]["RightEye"]["Z"].get<float>();
    mapPoint[DUT_EYE_RIGHT] = right;

    return mapPoint;
}

bool CalibrationConfig::readCalculateMvFidsByInputFlag(Json jsonObj)
{
    return jsonObj["MvRelativeDutInputGrating"]["CalculateMvFidsByInputFlag"].get<bool>();
}

MoveOutInfo CalibrationConfig::getMoveOutInfo(Json jsonObj)
{
    MoveOutInfo info;
    info.enable = jsonObj["MoveOut"]["Enable"].get<bool>();
    info.offset.x = jsonObj["MoveOut"]["Offset"]["X"].get<float>();
    info.offset.y = jsonObj["MoveOut"]["Offset"]["Y"].get<float>();
    return info;
}

QString CalibrationConfig::getDefaultDutName()
{
    std::string sn = m_JsonDutBase["DefaultDutSN"].get<std::string>();
    return QString::fromStdString(sn);
}

Result CalibrationConfig::setDefaultDutName(QString sn)
{
    Json &newConfig = m_JsonDutBase;
    std::vector<std::string> list = newConfig["SNList"];
    if (std::find(list.begin(), list.end(), sn.toStdString()) == list.end())
    {
        return Result(false, "DUT info is missing, please manual config, sn=" + sn.toStdString());
    }

    newConfig["DefaultDutSN"] = sn.toStdString();
    std::ofstream ofs(dutBasefile_name.toStdString());
    if (!ofs.is_open())
    {
        return Result(false, QString("Set default dut name error, file %1 open failed.").arg(dutBasefile_name).toStdString());
    }
    ofs << newConfig.dump(4);
    ofs.close();

    m_dutConfig = loadDutConfig("");
    MetricsData::instance()->setDutEyeType(m_dutConfig.eyeType);

    emit Core::PrjCommon::instance()->updateDutConfigSignal();
    return Result();
}

bool CalibrationConfig::addSnName(QString sn)
{
    Json &writeBaseConfig = m_JsonDutBase;
    writeBaseConfig["SNList"].push_back(sn.toStdString());
    std::ofstream ofs(dutBasefile_name.toStdString());
    if (!ofs.is_open())
    {
        return false;
    }
    ofs << writeBaseConfig.dump(4);
    ofs.close();
    return true;
}

bool CalibrationConfig::updateSnName(QString sn, QString snOld)
{
    if (sn == snOld){
        return true;
    }

    Json &writeBaseConfig = m_JsonDutBase;

    {
        // delete old sn
        int idx = -1;
        std::vector<std::string> list = writeBaseConfig["SNList"];
        for (int i = 0; i < list.size(); ++i)
        {
            if (list[i] == snOld.toStdString()){
                idx = i;
                break;
            }
        }

        if (idx >= 0){
            writeBaseConfig["SNList"].erase(idx);
        }
    }

    writeBaseConfig["SNList"].push_back(sn.toStdString());
    std::ofstream ofs(dutBasefile_name.toStdString());
    if (!ofs.is_open())
    {
        return false;
    }
    ofs << writeBaseConfig.dump(4);
    ofs.close();

    QString dutFile = dutFileName(snOld);
    bool ok = QFile::remove(dutFile);
    return ok;
}

bool CalibrationConfig::deleteSnName(QString sn)
{
    QList<QString> list = getDutSns();

    Json &writeBaseConfig = m_JsonDutBase;
    writeBaseConfig["SNList"].clear();

    for (QString tmp : list){
        if (tmp != sn){
            writeBaseConfig["SNList"].push_back(tmp.toStdString());
        }
    }

    std::ofstream ofs(dutBasefile_name.toStdString());
    if (!ofs.is_open())
    {
        return false;
    }
    ofs << writeBaseConfig.dump(4);
    ofs.close();
    return true;
}

QString CalibrationConfig::dutFileName(QString sn)
{
    if (sn.isEmpty())
    {
        sn = QString::fromStdString(m_JsonDutBase["DefaultDutSN"].get<std::string>());
    }
    return dutfilePath_name + QString("dut_%1.json").arg(sn);
}

QString CalibrationConfig::dutFileName(const std::string &sn)
{
    return dutFileName(QString::fromStdString(sn));
}

void CalibrationConfig::refreshDut()
{
    loadDutBase();
    m_dutConfig = loadDutConfig("");
}

Json CalibrationConfig::dutJson(const std::string &sn)
{
    Json duJson;
    QString dutFile = dutFileName(sn);
    if (!QFile::exists(dutFile)){
        qCritical() << "DUT dile is not exist, " << dutFile;
        return Json();
    }

    std::ifstream ifs(dutFile.toStdString(), std::fstream::in);
    ifs >> duJson;
    return duJson;
}

Json CalibrationConfig::updateDutConfig(DutConfig config)
{
    Json writeDutConfig = dutJson(config.sn.toStdString());
    if (writeDutConfig.is_null())
    {
        qCritical() << "Update DUT config error, read DUT json is null.";
        return Json();
    }

    writeDutConfig["FiducialPos"].clear();
    for (auto fiducial : config.fiducials)
    {
        Json fiCofig;
        fiCofig["X"] = (int)round((fiducial.x * 1000)) / 1000.0;
        fiCofig["Y"] = (int)round((fiducial.y * 1000)) / 1000.0;
        fiCofig["Z"] = (int)round((fiducial.z * 1000)) / 1000.0;
        writeDutConfig["FiducialPos"].push_back(fiCofig);
    }

    writeDutConfig["EyeboxOffsetAll"].clear();
    for (auto eye : config.exeboxOffsetAll)
    {
        Json fiCofig;
        fiCofig["X"] = (int)round((eye.x * 1000)) / 1000.0;
        fiCofig["Y"] = (int)round((eye.y * 1000)) / 1000.0;
        fiCofig["Z"] = (int)round((eye.z * 1000)) / 1000.0;
        writeDutConfig["EyeboxOffsetAll"].push_back(fiCofig);
    }

    writeDutConfig["EyeboxCenterPos"]["X"] = (int)round((config.eyeboxCenter.x * 1000)) / 1000.0;
    writeDutConfig["EyeboxCenterPos"]["Y"] = (int)round((config.eyeboxCenter.y * 1000)) / 1000.0;
    writeDutConfig["EyeboxCenterPos"]["Z"] = (int)round((config.eyeboxCenter.z * 1000)) / 1000.0;

    writeDutConfig["InputGratingPos"]["X"] = (int)round((config.inputGrating.x * 1000)) / 1000.0;
    writeDutConfig["InputGratingPos"]["Y"] = (int)round((config.inputGrating.y * 1000)) / 1000.0;
    writeDutConfig["InputGratingPos"]["Z"] = (int)round((config.inputGrating.z * 1000)) / 1000.0;

    writeDutConfig["EyeboxOffset"] = config.exeboxOffset;
    writeDutConfig["EyeRelief"] = (int)round((config.eyeRelief * 1000)) / 1000.0;

    writeDutConfig["SN"] = config.sn.toStdString();
    writeDutConfig["EyeType"] = config.eyeType == 0 ? "Left" : "Right";
    return writeDutConfig;
}

Json CalibrationConfig::addDutConfig(DutConfig config)
{
    QFile file(dutFileName(config.sn));
    file.open(QIODevice::WriteOnly);
    file.close();

    Json writeDutConfig;
    std::vector<Json> fiducials;
    for (auto fiducial : config.fiducials)
    {
        Json fiCofig;
        fiCofig["X"] = (int)round((fiducial.x * 1000)) / 1000.0;
        fiCofig["Y"] = (int)round((fiducial.y * 1000)) / 1000.0;
        fiCofig["Z"] = (int)round((fiducial.z * 1000)) / 1000.0;
        fiducials.push_back(fiCofig);
    }
    writeDutConfig["FiducialPos"] = fiducials;

    std::vector<Json> eyeboxOffsetAll;
    for (auto eye : config.exeboxOffsetAll)
    {
        Json fiCofig;
        fiCofig["X"] = (int)round((eye.x * 1000)) / 1000.0;
        fiCofig["Y"] = (int)round((eye.y * 1000)) / 1000.0;
        fiCofig["Z"] = (int)round((eye.z * 1000)) / 1000.0;
        eyeboxOffsetAll.push_back(fiCofig);
    }
    writeDutConfig["EyeboxOffsetAll"] = eyeboxOffsetAll;

    Json eyeboxCenter;
    eyeboxCenter["X"] = (int)round((config.eyeboxCenter.x * 1000)) / 1000.0;
    eyeboxCenter["Y"] = (int)round((config.eyeboxCenter.y * 1000)) / 1000.0;
    eyeboxCenter["Z"] = (int)round((config.eyeboxCenter.z * 1000)) / 1000.0;
    writeDutConfig["EyeboxCenterPos"] = eyeboxCenter;


    Json inputGratingPos;
    inputGratingPos["X"] = (int)round((config.inputGrating.x * 1000)) / 1000.0;
    inputGratingPos["Y"] = (int)round((config.inputGrating.y * 1000)) / 1000.0;
    inputGratingPos["Z"] = (int)round((config.inputGrating.z * 1000)) / 1000.0;
    writeDutConfig["InputGratingPos"] = inputGratingPos;

    writeDutConfig["EyeboxOffset"] = config.exeboxOffset;
    writeDutConfig["EyeRelief"] = (int)round((config.eyeRelief * 1000)) / 1000.0;

    writeDutConfig["SN"] = config.sn.toStdString();
    writeDutConfig["EyeType"] = config.eyeType == 0 ? "Left" : "Right";
    return writeDutConfig;
}

QList<QString> CalibrationConfig::getDutSns()
{
    QList<QString> retList;
    std::vector<std::string> list = m_JsonDutBase["SNList"];
    for(int i = 0; i < list.size(); ++i){
        retList.push_back(QString::fromStdString(list[i]));
    }
    return retList;
}