#pragma once
#include "windows.h"
#include "piMotionActor/Motion2DModel.h"
#include <QWidget>
#include <json.hpp>
#include <QMap>
#include "integratingsphere_global.h"

using Json = nlohmann::json;
using namespace std;

struct ReticleInfo{
    QMap<QString, double> xMap;
    float yPos;
    float darkYPos;
    cv::Point2f minLimit;
    cv::Point2f maxLimit;
};

class INTEGRATINGSPHERE_EXPORT ReticleMode2D : public QObject
{
    Q_OBJECT

  public:
    static ReticleMode2D *instance(QObject *parent = nullptr);
    ~ReticleMode2D();
    Result connect();
    void disconnect();
    bool isConnected();
    Result homing();
    Result homingForced();
    bool isHome();
    Result moveStop();

    Result changeReticle(QString reticle, int type = 0);
    Result absMove(cv::Point2f coord);
    Result relativeMove(cv::Point2f offset);
    void setStop(bool stop);
    QList<QString> getReticles(int type = 0);
    QString getReticleByPosition(double pos, int type = 0);

    Result moveUpDown(int eyeType);

    void setEyeType(int eyeType);
    int getEyeType();

    bool isDutSync();

  private:
    ReticleMode2D(QObject *parent = nullptr);
    Result loadconfig(std::string filename);
    Result getReticleMap(Json json, QMap<QString, double> &reticleMap);
    Result getReticleInfo(Json json, ReticleInfo &reticleInfo);
    Result getMotionIp(Json motion, QString &ipBuff);

    Result judgeLimit(cv::Point2f target, int type = 0);

signals:
    void updateMotion2DStatusSignal(MLMotionState status);
    void updateMotion2DPositionSignal(float x, float y);    

  private:
    static ReticleMode2D *self;
    Motion2DModel *m_motionModel = nullptr;
    
    // QString: 0 left eye, 1 right eye 
    QMap<int, ReticleInfo> m_reticleInfoMap;
    QString m_ip;
    int m_eyeType = 0;

    const QString FILE_NAME = "./config/device/reticle2D.json";
    Json m_json;
};
