#include "ReticleMode2D.h"
#include <fstream>
#include <iostream>
#include <QMutex>
#include <QDebug>
#include <PrjCommon/metricsdata.h>
#include "integratingspheretcpmodel.h"
#include "piMotionActor/calibrationconfig.h"

ReticleMode2D *ReticleMode2D::self = nullptr;
ReticleMode2D *ReticleMode2D::instance(QObject *parent)
{
    if (!self)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self)
        {
            self = new ReticleMode2D(parent);
        }
    }
    return self;
}

ReticleMode2D::ReticleMode2D(QObject *parent){
    m_motionModel = Motion2DModel::getInstance(ACS2DReticle);
    m_motionModel->Motion2DSubscribeMotionEvent(MLMotionEvent::kStateChanged);
    QObject::connect(m_motionModel, SIGNAL(updateMotion2DPositionSignal(float, float)), this,
                     SIGNAL(updateMotion2DPositionSignal(float, float)));
    QObject::connect(m_motionModel, SIGNAL(updateMotion2DStatusSignal(MLMotionState)), this,
                     SIGNAL(updateMotion2DStatusSignal(MLMotionState)));

    Result ret = loadconfig(FILE_NAME.toStdString());
    if(!ret.success){
        qCritical() << QString::fromStdString(ret.errorMsg);
    }
}

ReticleMode2D::~ReticleMode2D()
{
    disconnect();

    delete m_motionModel;
    m_motionModel = nullptr;
}

Result ReticleMode2D::connect()
{
    if (m_ip.isEmpty())
    {
        return Result(false, "Reticle connection failed, ip is null.");  
    }

    Result ok = m_motionModel->connectAndInit(m_ip);
    if (ok.success)
    {
        m_motionModel->Motion2DSubscribeMotionEvent(MLMotionEvent::kPositionChanged);

        //TODO£º
        static bool first = false;
        if (first)
        {
            ok = m_motionModel->homingForced();
            if (!ok.success)
            {
                return ok;
            }
            first = false;
        }

        if(isDutSync()){
            moveUpDown(CalibrationConfig::instance()->getDutConfig().eyeType);
        }
        return Result(true, "Reticle is connected.");
    }
    else
    {
        return Result(false, QString("Reticle connection failed, " + QString::fromStdString(ok.errorMsg)).toStdString());
    }
}

void ReticleMode2D::disconnect()
{
    if (m_motionModel != nullptr)
    {
        m_motionModel->Motion2DUnsubscribeMotionEvent(MLMotionEvent::kPositionChanged);
        m_motionModel->disconnect();
    }
}

bool ReticleMode2D::isConnected()
{
    return m_motionModel->GetIsConnected();
}

Result ReticleMode2D::homing()
{
    //m_motionModel->Home();
    //return Result();
    return m_motionModel->Home();
}

Result ReticleMode2D::homingForced()
{
    Result ret = m_motionModel->homingForced();
    return ret;
}

bool ReticleMode2D::isHome()
{
    Result ret = m_motionModel->isHoming();
    return ret.success;
}

Result ReticleMode2D::moveStop()
{
    return m_motionModel->Motion2DMoveStop();
}

Result ReticleMode2D::changeReticle(QString reticle, int type)
{
    if (!m_motionModel->GetIsConnected())
    {
        return Result(false, "Reticle is not connected.");
    }

    if (reticle.isEmpty()){
        return Result(false, "Change reticle failed, reticle param is null.");
    }

    QMap<QString, double> reticleMap = m_reticleInfoMap[type].xMap;
    if (reticleMap.find(reticle) == reticleMap.end() && reticle != "Dark")
    {
        return Result(false, QString("Change reticle failed, %1 is mismatch.").arg(reticle).toStdString());
    }

    double absPos = reticleMap.value(reticle);
    float yPos = m_reticleInfoMap[type].yPos;
    if (reticle == "Dark")
    {
        absPos = m_json["Dark"]["X"].get<float>();
        yPos = m_json["Dark"]["Y"].get<float>();
    }

    Result ret = judgeLimit(cv::Point2f(absPos, yPos));
    if (!ret.success)
    {
        return ret;
    }

    ret = m_motionModel->Motion2DMoveAbs(absPos, yPos);
    if (!ret.success)
    {
        return Result(false, QString("Change reticle failed, ").toStdString() + ret.errorMsg);
    }

    //MetricsData::instance()->setCurrentReticle(reticle);
    return Result();  
}

Result ReticleMode2D::absMove(cv::Point2f coord)
{
    Result ret = judgeLimit(coord);
    if (!ret.success)
    {
        return ret;
    }
    return m_motionModel->Motion2DMoveAbs(coord.x, coord.y);
}

Result ReticleMode2D::relativeMove(cv::Point2f offset)
{
    cv::Point2f currentCoord;
    bool flag = m_motionModel->getPosition(currentCoord.x, currentCoord.y);
    if (!flag)
    {
        return Result(false, "Reticle get poaition failed, it is not connected.");
    }

    currentCoord = currentCoord / 1000.0;
    Result ret = judgeLimit(currentCoord + offset);
    if (!ret.success)
    {
        return ret;
    }

    return m_motionModel->Motion2DMoveRel(offset.x, offset.y);
}

void ReticleMode2D::setStop(bool stop)
{
    m_motionModel->setEStop(stop);
}

QList<QString> ReticleMode2D::getReticles(int eyeType)
{
    QMap<QString, double> reticleMap = m_reticleInfoMap[eyeType].xMap;
    QMap<double, QString> map;
    QMap<QString, double>::Iterator iter;
    for (iter = reticleMap.begin(); iter != reticleMap.end(); ++iter)
    {
        map[iter.value()] = iter.key();
    }

    QList<QString> list;
    QMap<double, QString>::Iterator iterT;
    for (iterT = map.begin(); iterT != map.end(); ++iterT)
    {
        list.append(iterT.value());
    }

    list.append("Dark");
    return list;
}

QString ReticleMode2D::getReticleByPosition(double pos, int eyeType)
{
    QMap<QString, double> reticleMap = m_reticleInfoMap[eyeType].xMap;
    QMap<QString, double>::Iterator iter;
    for (iter = reticleMap.begin(); iter != reticleMap.end(); ++iter)
    {
        if (abs(pos - iter.value()) < 0.01){
            return iter.key();
        }
    }
    return QString();
}

Result ReticleMode2D::moveUpDown(int type)
{
    if(!m_motionModel->GetIsConnected()){
        return Result(false, "Reticle is not connected.");
    }

    cv::Point2f pos;
    bool ret = m_motionModel->getPositionRaw(pos);
    if(!ret){
        return Result(false, "Reticle get position failed.");
    }

    string typestr = type == 0 ? "Small" : "Large";
    float yPos = m_json[typestr]["YPosition"].get<float>();

    setEyeType(type);
    return m_motionModel->Motion2DMoveAbs(pos.x, yPos); 
}

void ReticleMode2D::setEyeType(int type)
{
    m_eyeType = type;
    MetricsData::instance()->setReticleEyeType(type);
}

int ReticleMode2D::getEyeType()
{
    return m_eyeType;
}

bool ReticleMode2D::isDutSync()
{
    return m_json["DutSync"].get<bool>();
}

Result ReticleMode2D::loadconfig(std::string filename)
{
    std::ifstream jsonFile(filename);
    if (!jsonFile.is_open()){
        return Result(false, QString("Open reticle config file %1 failed.").arg(QString::fromStdString(filename)).toStdString());
    }
    
    std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
    jsonFile.close();
    Json settingJsonObj = Json::parse(contents);
    m_json = settingJsonObj;

    Result ret = getMotionIp(settingJsonObj, m_ip);
    if(!ret.success){
        return ret;
    }

    if (settingJsonObj.contains("Small")){
        ReticleInfo reticleInfo;
        ret = getReticleInfo(settingJsonObj["Small"], reticleInfo);
        if(!ret.success)
        {
            return ret;
        }
        m_reticleInfoMap[0] = reticleInfo;
    }

    if (settingJsonObj.contains("Large")){
        ReticleInfo reticleInfo;
        ret = getReticleInfo(settingJsonObj["Large"], reticleInfo);
        if (!ret.success)
        {
            return ret;
        }
        m_reticleInfoMap[1] = reticleInfo;
    }

    return Result(); 
}

Result ReticleMode2D::getReticleMap(Json json, QMap<QString, double> &reticleMap)
{
    Json baseInfo = json["base"];
    double baseCoord = 0.0f;
    if (!baseInfo.is_null())
    {
        baseCoord = baseInfo["motionpos"].get<double>();
        std::string refReticle = baseInfo["reticle"].get<std::string>();
        QString baseReticle = QString::fromStdString(refReticle);
    }
    else
    {
        return Result(
            false, QString("Reticle config file %1 error, base is null.")
                                 .arg(FILE_NAME)
                                 .toStdString());
    }

    Json reticleposMap = json["reticlepos"];
    if (!reticleposMap.is_null())
    {
        for (auto &[key, value] : reticleposMap.items())
        {
            QString keyReticle = QString::fromStdString(key);
            double pos = value.get<float>() + baseCoord;
            reticleMap.insert(keyReticle, pos);
        }
    }
    else
    {
        return Result(false, QString("Reticle config file %1 error, reticlepos is null.")
                                 .arg(FILE_NAME)
                                 .toStdString());
    }

    return Result();
}

Result ReticleMode2D::getReticleInfo(Json json, ReticleInfo &reticleInfo)
{
    QMap<QString, double> reticleMap;
    Result ret = getReticleMap(json, reticleMap);
    if (!ret.success){
        return ret;
    }

    if (!json.contains("minLimit") || !json.contains("maxLimit")){
        return Result(false,
                      QString("Reticle config file %1 error, minLimit or maxLimit is null.").arg(FILE_NAME).toStdString());
    }

    reticleInfo.xMap = reticleMap;
    reticleInfo.yPos = json["YPosition"].get<float>();
    reticleInfo.minLimit.x = json["minLimit"]["X"].get<float>();
    reticleInfo.minLimit.y = json["minLimit"]["Y"].get<float>();
    reticleInfo.maxLimit.x = json["maxLimit"]["X"].get<float>();
    reticleInfo.maxLimit.y = json["maxLimit"]["Y"].get<float>();
    return Result();
}

Result ReticleMode2D::getMotionIp(Json motion, QString& ipBuff)
{
    if (!motion.contains("IP") || !motion.contains("InitBuffers"))
    {
        return Result(false, "Get motion ip and buffer error in reticle config file.");
    }

    QString ip = QString::fromStdString(motion["IP"]);
    std::vector<int> list = motion["InitBuffers"];
    QString buffer;
    for (int i = 0; i < list.size(); ++i)
    {
        buffer += "\\" + QString::number(list[i]);
    }
    ipBuff = ip + buffer;

    return Result();
}

Result ReticleMode2D::judgeLimit(cv::Point2f target, int eyeType)
{
    if (target.x > m_reticleInfoMap[eyeType].maxLimit.x || target.x < m_reticleInfoMap[eyeType].minLimit.x ||
        target.y > m_reticleInfoMap[eyeType].maxLimit.y || target.y < m_reticleInfoMap[eyeType].minLimit.y)
    {
        return Result(
            false,
            QString("Change reticle failed, coordinate is over limit, target coordinate (%1,%2), Min (%3,%4), Max (%5,%6).")
            .arg(target.x)
            .arg(target.y)
            .arg(m_reticleInfoMap[eyeType].minLimit.x)
            .arg(m_reticleInfoMap[eyeType].minLimit.y)
            .arg(m_reticleInfoMap[eyeType].maxLimit.x)
            .arg(m_reticleInfoMap[eyeType].maxLimit.y)
            .toStdString());
    }
    return Result();
}
