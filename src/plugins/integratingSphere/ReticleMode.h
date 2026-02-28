#pragma once
#include "piMotionActor/Motion1DModel.h"
#include <QWidget>
#include <json.hpp>
#include <QMap>
#include "integratingsphere_global.h"

using Json = nlohmann::json;

class INTEGRATINGSPHERE_EXPORT ReticleMode : public QObject
{
    Q_OBJECT

  public:
    static ReticleMode *instance(QObject *parent = nullptr);
    ~ReticleMode();
    Result connect();
    void disconnect();
    bool isConnected();

    Result changeReticle(QString reticle);
    Result absMove(double coordinate);
    Result relativeMove(double offset);
    void setStop(bool stop);
    QList<QString> getReticles();
    QString getReticleByPosition(double pos);

  private:
    ReticleMode(QObject *parent = nullptr);
    Result loadconfig(std::string filename);

signals:
    void updateMotion1DStatusSignal(MLMotionState status);
    void updateMotion1DPositionSignal(float x);    
  private:
    static ReticleMode *self;
    QMap<QString, double> m_reticleMap;
    double m_baseCoord = 0.0f;
    double m_baseInterval = 0.0f;

    double m_maxCoordinate = 0.0f;
    QString m_motion1dIp;
    Motion1DModel *m_motionModel = nullptr;

    const double ONE_THOUSAND = 1000.0;
};
