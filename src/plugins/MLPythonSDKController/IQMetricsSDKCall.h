#pragma once
#pragma push_macro("slots")
#undef slots
#include <pybind11\embed.h>
#pragma pop_macro("slots")

#include "mlpythonsdkcontroller_global.h"
#include <vector>
using namespace std;
namespace py = pybind11;

class MLPYTHONSDKCONTROLLER_EXPORT IQMetricsSDKCall
{
  private:
    py::object iq_sdk;
    std::string dpaInitBug;

  private:
    template <class T> T castResultToNum(py::object py_result)
    {
        if (py::str(py_result).is(py::str(Py_False)))
        {
            return nan("");
        }
        return py_result.cast<T>();
    }

  public:
    IQMetricsSDKCall(py::module_ package);
    ~IQMetricsSDKCall();

    bool IsInit();
    std::string GetInitBug();

    void DistortionCalculate(string color, int eyeboxId,float degree);
    void StrayLightCalculate(string color, int eyeboxId, float degree);
    void PupilSwimCalculate(string color, int eyeboxId, float degree);
    void GhostCalculate(string color, int eyeboxId, float degree);
    void FlareCalculate(string color, int eyeboxId, float degree);
    void ColorUniformityCalculate(string color, int eyeboxId, float degree,string ndstr);
};
