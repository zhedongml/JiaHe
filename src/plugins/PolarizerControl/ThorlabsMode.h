#pragma once
#include "polarizercontrol_global.h"
#include "piMotionActor/Motion1DModel.h"
#include "Result.h"
#include <QObject>

class POLARIZERCONTROL_EXPORT ThorlabsMode : public QObject {
	Q_OBJECT
public:
    static ThorlabsMode* instance(QObject* parent = nullptr);
    ~ThorlabsMode();
    Result Connect();
    bool Disconnect();
    bool IsConnected();

    Result AbsMoveAsync(double coordinate);
    Result RelativeMoveAsync(double offset);
    Result AbsMoveSync(double coordinate);
    Result RelativeMoveSync(double offset);
    Result StopMove();
    double GetPosition();

private:
    ThorlabsMode(QObject* parent = nullptr);

    Result rangeJudge(double coordinate);

private:
    static ThorlabsMode* self;
    Motion1DModel* m_motionModel = nullptr;
};