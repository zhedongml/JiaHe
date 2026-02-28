#include "IQMetricsSDKCall.h"
#include "PyHelper.h"
#include <pybind11\stl.h>
#include "pch.h"
#include "PrjCommon/metricsdata.h"
#include <QDir>
#include <iostream>
#include <sstream>

IQMetricsSDKCall::IQMetricsSDKCall(py::module_ package)
{
    try
    {
        py::gil_scoped_acquire acquire;
        auto IQmodule = package.attr("IQMetricsSDKCall");
        const char *path = "./config";
        QDir config_json(path);
        std::string json_path = config_json.absolutePath().toStdString();
        iq_sdk = IQmodule(json_path.c_str());
    }
    catch (py::error_already_set e)
    {
        auto errorStrings = PyHelper::SplitStringByNewline(e.what());
        dpaInitBug = errorStrings[0];
    }
}

IQMetricsSDKCall::~IQMetricsSDKCall()
{
}

bool IQMetricsSDKCall::IsInit()
{
    return iq_sdk.ptr() != nullptr;
}

std::string IQMetricsSDKCall::GetInitBug()
{
    return dpaInitBug;
}

void IQMetricsSDKCall::DistortionCalculate(string color, int eyeboxId, float degree)
{
    py::gil_scoped_acquire acquire;
    std::string folder_path = MetricsData::instance()->getMTFImgsDir().toStdString();
    iq_sdk.attr("distortion_cal")(folder_path, color, eyeboxId, degree);
}

void IQMetricsSDKCall::StrayLightCalculate(string color, int eyeboxId, float degree)
{
    py::gil_scoped_acquire acquire;
    std::string folder_path = MetricsData::instance()->getMTFImgsDir().toStdString();
    iq_sdk.attr("straylight_cal")(folder_path, color, eyeboxId, degree);
}

void IQMetricsSDKCall::PupilSwimCalculate(string color, int eyeboxId, float degree)
{
    py::gil_scoped_acquire acquire;
    std::string folder_path = MetricsData::instance()->getMTFImgsDir().toStdString();
    iq_sdk.attr("pupilswim_cal")(folder_path, color, eyeboxId, degree);
}

void IQMetricsSDKCall::GhostCalculate(string color, int eyeboxId, float degree)
{
    py::gil_scoped_acquire acquire;
    //std::string dutId = MetricsData::instance()->getDutBarCode().toStdString();
    std::string folder_path = MetricsData::instance()->getMTFImgsDir().toStdString();
    iq_sdk.attr("ghost_cal")(folder_path, color, eyeboxId, degree);
}

void IQMetricsSDKCall::FlareCalculate(string color, int eyeboxId, float degree)
{
    py::gil_scoped_acquire acquire;
    std::string folder_path = MetricsData::instance()->getMTFImgsDir().toStdString();
    iq_sdk.attr("flare_cal")(folder_path, color, eyeboxId, degree);
}

void IQMetricsSDKCall::ColorUniformityCalculate(string color, int eyeboxId, float degree,string ndstr)
{
    py::gil_scoped_acquire acquire;
    std::string folder_path = MetricsData::instance()->getMTFImgsDir().toStdString();
    iq_sdk.attr("coloruniformity_cal")(folder_path, color, eyeboxId, degree,ndstr);
}
