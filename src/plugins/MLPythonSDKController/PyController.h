#pragma once

#pragma push_macro("slots")
#undef slots
#include <pybind11\embed.h>
#pragma pop_macro("slots")

#include "IQMetricsSDKCall.h"
#include "mlpythonsdkcontroller_global.h"
#include <QMutex>
#include <QObject>

namespace py = pybind11;

class MLPYTHONSDKCONTROLLER_EXPORT PyController : public QObject
{
    Q_OBJECT

  public:
    static PyController *getInstance(QObject *parent = nullptr);
    ~PyController();

    	std::string getCurrDirectory();
    std::string printSysPath();

    void InitPyController();

    void InitPackage();

    void handleError(py::error_already_set e);

signals:
    void sigCalculateFinish(bool isSuccess);

  public slots:
    void calculateDistortion(string color, int eyeboxId,float degree);
    void calculateFlare(string color, int eyeboxId, float degree);
    void calculateGhost(string color, int eyeboxId, float degree);
    void calculatePupilSwim(string color, int eyeboxId, float degree);
    void calculateStrayLight(string color, int eyeboxId, float degree);
    void calculateColorUniformity(string color, int eyeboxId, float degree,string ndstr);

    void endPython();

  private:
    static PyController *self;
    PyController(QObject *parent = nullptr);
    IQMetricsSDKCall *iq_sdk = nullptr;
    static QMutex mutex;
};
