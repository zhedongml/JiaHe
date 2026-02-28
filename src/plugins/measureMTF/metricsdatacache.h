#pragma once

#include <QMap>
#include <QMutex>
#include <QObject>
#include <vector>
struct FOVCenter
{
    float centerX = 0.0;
    float centerY = 0.0;
    std::vector<double> rotateH;
    std::vector<double> rotateV;
};
struct FOVAngle
{
    float fovV = 0.0;
    float fovH = 0.0;
    float fovD = 0.0;
};
class MetricsDataCache : public QObject
{
    Q_OBJECT

  public:
    MetricsDataCache(QObject *parent = nullptr);
    ~MetricsDataCache();
    static MetricsDataCache *getInstance(QObject *parent = nullptr);
    void updateFOVCenter(QString color, float centerX, float centerY, std::vector<double> rotateH,
                         std::vector<double> rotateV);
    void updateFOVAngle(QString color, float h, float v, float d);
    void updateRotation(QString color, float degree);
    FOVAngle getFOVAngle(QString color);
    FOVCenter getFOVCenter(QString color);
    float getRotationDeg(QString color);

  private:
    ;
    static MetricsDataCache *self;
    QMap<QString, FOVCenter> fovCenterMap;
    QMap<QString, FOVAngle> fovAngleMap;
    QMap<QString, float> rotationDeg;
    QMutex mutex1;
};
