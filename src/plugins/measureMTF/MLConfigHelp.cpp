#include "MLConfigHelp.h"
#include "loggingwrapper.h"

MLConfigHelp::MLConfigHelp()
{
}

MLConfigHelp::~MLConfigHelp()
{
}

Result MLConfigHelp::GetExposureCheckParam(ExposureCheckParam& param)
{
    std::ifstream ifs(EXPOSURECHECK_FILE_NAME, std::fstream::in);
    if (ifs.fail())
    {
		return Result(false, "open exposure check config file " + EXPOSURECHECK_FILE_NAME + " error!");
    }

    try
    {
        Json JsonExposureCheck;
        ifs >> JsonExposureCheck;

        if (JsonExposureCheck.contains("topN"))
            param.topN = JsonExposureCheck["topN"];
        if (JsonExposureCheck.contains("threadCount"))
            param.threadCount = JsonExposureCheck["threadCount"];
        if (JsonExposureCheck.contains("minGray"))
            param.minGray = JsonExposureCheck["minGray"];
        if (JsonExposureCheck.contains("over_ratio"))
            param.over_ratio = JsonExposureCheck["over_ratio"];
        if (JsonExposureCheck.contains("low_ratio"))
            param.low_ratio = JsonExposureCheck["low_ratio"];
    }
    catch (const std::exception& e)
    {
		return Result(false, std::string(EXPOSURECHECK_FILE_NAME + " json parse error: ") + e.what());
    }
    return Result();
}
