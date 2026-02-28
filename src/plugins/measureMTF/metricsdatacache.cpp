#include "metricsdatacache.h"
MetricsDataCache *MetricsDataCache::self = nullptr;
MetricsDataCache::MetricsDataCache(QObject *parent) : QObject(parent)
{
}

MetricsDataCache::~MetricsDataCache()
{
}

MetricsDataCache *MetricsDataCache::getInstance(QObject *parent)
{
    if (!self)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self)
        {
            self = new MetricsDataCache(parent);
            return self;
        }
    }
    if (parent != nullptr)
    {
        self->setParent(parent);
    }
    return self;
}

void MetricsDataCache::updateFOVCenter(QString color, float centerX, float centerY, std::vector<double> rotateH,
                                       std::vector<double> rotateV)
{
    QString colorLow = color.toLower();
    if (colorLow.isEmpty())
        return;
    FOVCenter cen;
    cen.centerX = centerX;
    cen.centerY = centerY;
    cen.rotateH = rotateH;
    cen.rotateV = rotateV;
    fovCenterMap.insert(colorLow.trimmed(), cen);
}

void MetricsDataCache::updateFOVAngle(QString color, float h, float v, float d)
{
    QString colorLow = color.toLower();
    if (colorLow.isEmpty())
        return;
    FOVAngle angle;
    angle.fovH = h;
    angle.fovV = v;
    angle.fovD = d;
    fovAngleMap.insert(colorLow.trimmed(), angle);
}
void MetricsDataCache::updateRotation(QString color, float degree)
{
    QString colorLow = color.toLower();
    if (colorLow.isEmpty())
        return;
    rotationDeg.insert(colorLow.trimmed(), degree);
}

FOVAngle MetricsDataCache::getFOVAngle(QString color)
{
    QString colorLow = color.toLower();
    FOVAngle angle;
    QMap<QString, FOVAngle>::const_iterator it = fovAngleMap.find(color);
    if (it != fovAngleMap.end())
    {
        angle = it.value();
    }
    else
    {
        QList<FOVAngle> vals = fovAngleMap.values();
        if (vals.size() > 0)
        {
            angle = vals.at(0);
        }
        else
        {
            angle.fovH = 14.0;
            angle.fovV = 14.0;
            angle.fovD = 14.5;
        }
    }
    return angle;
}

FOVCenter MetricsDataCache::getFOVCenter(QString color)
{
    QString colorLow = color.toLower();
    FOVCenter center;
    QMap<QString, FOVCenter>::const_iterator it = fovCenterMap.find(color);
    if (it != fovCenterMap.end())
    {
        center = it.value();
    }
    else
    {
        QList<FOVCenter> vals = fovCenterMap.values();
        if (vals.size() > 0)
        {
            center = vals.at(0);
        }
    }
    if (abs(center.centerX - 0.0) < 0.00001)
    {
        center.centerX = 3961;
        center.centerY = 3073;
        std::vector<double> h;
        h.push_back(0);
        h.push_back(-1);
        h.push_back(0);
        center.rotateH = h;
        std::vector<double> v;
        v.push_back(1e6);
        v.push_back(-1);
        v.push_back(0);
        center.rotateV = v;
    }
    return center;
}
float MetricsDataCache::getRotationDeg(QString color)
{
    QString colorLow = color.toLower();
    float degree;
    QMap<QString, float>::const_iterator it = rotationDeg.find(color);
    if (it != rotationDeg.end())
    {
        degree = it.value();
    }
    else
    {
        QList<float> vals = rotationDeg.values();
        if (vals.size() > 0)
        {
            degree = vals.at(0);
        }
        else
        {
            degree = 0.0/*-25.0*/;
        }
    }
    return degree;
}