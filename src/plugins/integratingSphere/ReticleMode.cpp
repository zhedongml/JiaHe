#include "windows.h"
#include "ReticleMode.h"
#include <fstream>
#include <iostream>
#include <QMutex>
#include <QDebug>
#include <PrjCommon/metricsdata.h>
#include "integratingspheretcpmodel.h"

ReticleMode *ReticleMode::self = nullptr;
ReticleMode *ReticleMode::instance(QObject *parent)
{
    if (!self)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self)
        {
            self = new ReticleMode(parent);
        }
    }
    return self;
}

ReticleMode::ReticleMode(QObject *parent){
    m_motionModel = Motion1DModel::getInstance("ACS");
    m_motionModel->Motion1DSubscribeMotionEvent(MLMotionEvent::kStateChanged);
    QObject::connect(m_motionModel, SIGNAL(updateMotion1DPositionSignal(float)), this,SIGNAL(updateMotion1DPositionSignal(float)));
    QObject::connect(m_motionModel, SIGNAL(updateMotion1DStatusSignal(MLMotionState)), this, SIGNAL(updateMotion1DStatusSignal(MLMotionState)));

    Result ret = loadconfig("./config/device/reticle.json");
    if(!ret.success){
        qCritical() << QString::fromStdString(ret.errorMsg);
    }
}

ReticleMode::~ReticleMode()
{
    disconnect();

    delete m_motionModel;
    m_motionModel = nullptr;
}

Result ReticleMode::connect()
{
    if (m_motion1dIp.isEmpty()){
        return Result(false, "Reticle connection failed, motion1d ip is null.");  
    }

    Result ok = m_motionModel->connectNew(m_motion1dIp);
    if (ok.success)
    {
        if (ok.errorCode != -2)
        {
            m_motionModel->Motion1DSubscribeMotionEvent(MLMotionEvent::kPositionChanged);
            //m_motionModel->Motion1DMoveAbs(m_baseCoord * 1000);
        }
        return Result(true, "Reticle is connected.");
    }
    else
    {
        return Result(false, QString("Reticle connection failed, " + QString::fromStdString(ok.errorMsg)).toStdString());
    }
}

void ReticleMode::disconnect()
{
    if (m_motionModel != nullptr)
    {
        m_motionModel->Motion1DUnsubscribeMotionEvent(MLMotionEvent::kPositionChanged);
        m_motionModel->disconnect();
    }
}

bool ReticleMode::isConnected()
{
    return m_motionModel->Motion1DisConnected();
}

Result ReticleMode::changeReticle(QString reticle)
{
    if (!m_motionModel->Motion1DisConnected())
    {
        return Result(false, "Reticle is not connected.");
    }

    if (reticle.isEmpty()){
        return Result(false, "Change reticle failed, reticle param is null.");
    }

    if (m_reticleMap.find(reticle) == m_reticleMap.end()){
        return Result(false, QString("Change reticle failed, %1 is mismatch.").arg(reticle).toStdString());
    }

    double absPos = m_reticleMap.value(reticle);
    if (abs(absPos) > m_maxCoordinate){
        return Result(
            false,
            QString("Change reticle failed, coordinate is over limit, abs coordinate is %1, max is %2.").arg(absPos).arg(m_maxCoordinate).toStdString());
    }

    Result ret = m_motionModel->Motion1DMoveAbs(int(absPos * ONE_THOUSAND));
    if (!ret.success)
    {
        return Result(false, QString("Change reticle failed, ").toStdString() + ret.errorMsg);
    }

    //MetricsData::instance()->setCurrentReticle(reticle);
    int pos = m_motionModel->getPosition();
    return Result(true, QString("Reticle coordinate is %1").arg(pos).toStdString());  
}

Result ReticleMode::absMove(double coordinate)
{
    return m_motionModel->Motion1DMoveAbs(coordinate * ONE_THOUSAND);
}

Result ReticleMode::relativeMove(double offset)
{
    return m_motionModel->Motion1DMoveRel(offset * ONE_THOUSAND);
}

void ReticleMode::setStop(bool stop)
{
    m_motionModel->setEStop(stop);
}

QList<QString> ReticleMode::getReticles()
{
    QMap<double, QString> map;
    QMap<QString, double>::Iterator iter;
    for (iter = m_reticleMap.begin(); iter != m_reticleMap.end(); ++iter)
    {
        map[iter.value()] = iter.key();
    }

    QList<QString> list;
    QMap<double, QString>::Iterator iterT;
    for (iterT = map.begin(); iterT != map.end(); ++iterT)
    {
        list.append(iterT.value());
    }
    return list;
}

QString ReticleMode::getReticleByPosition(double pos)
{
    QMap<QString, double>::Iterator iter;
    for (iter = m_reticleMap.begin(); iter != m_reticleMap.end(); ++iter)
    {
        if (abs(pos - iter.value()) < 0.01){
            return iter.key();
        }
    }
    return QString();
}

Result ReticleMode::loadconfig(std::string filename)
{
    std::ifstream jsonFile(filename);
    if (!jsonFile.is_open()){
        return Result(false, QString("Open reticle config file %1 failed.").arg(QString::fromStdString(filename)).toStdString());
    }
    
    std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
    jsonFile.close();
    Json settingJsonObj = Json::parse(contents);
    Json motionip = settingJsonObj["MotionIP"];
    if (!motionip.is_null())
    {
        std::string ipstr = motionip.get<std::string>();
        m_motion1dIp = QString::fromStdString(ipstr);
    }else{
        return Result(false, QString("Reticle config file %1 error, MotionIP is null.").arg(QString::fromStdString(filename)).toStdString());
    }

    Json baseInfo = settingJsonObj["base"];
    if (!baseInfo.is_null())
    {
        m_baseCoord = baseInfo["motionpos"].get<double>();
        m_baseInterval = baseInfo["interval"].get<double>();

        std::string refReticle = baseInfo["reticle"].get<std::string>();
        QString baseReticle = QString::fromStdString(refReticle);
    }
    else
    {
        return Result(false, QString("Reticle config file %1 error, base is null.")
                                 .arg(QString::fromStdString(filename))
                                 .toStdString());
    }

    Json reticleposMap = settingJsonObj["reticlepos"];
    if (!reticleposMap.is_null())
    {
        for (auto &[key, value] : reticleposMap.items())
        {
            QString keyReticle = QString::fromStdString(key);
            double pos = value.get<float>() * m_baseInterval + m_baseCoord;
            m_reticleMap.insert(keyReticle, pos);
        }
    }
    else
    {
        return Result(
            false,
            QString("Reticle config file %1 error, reticlepos is null.").arg(QString::fromStdString(filename)).toStdString());
    }

    Json max = settingJsonObj["maxCoordinate"];
    if (!max.is_null())
    {
        m_maxCoordinate = max.get<double>();
    }
    else
    {
        return Result(false, QString("Reticle config file %1 error, maxCoordinate is null.")
                                 .arg(QString::fromStdString(filename))
                                 .toStdString());
    }
    return Result(); 
}