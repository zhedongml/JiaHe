#pragma once
#include <QString>
#include <json.hpp>
#include <fstream>
#include "Result.h"
#include "MLColorimeterCommon.h"

using Json = nlohmann::json;
using namespace ML::MLColorimeter;

class ImageRotationConfig
{
  public:
    static ImageRotationConfig *instance();
    ~ImageRotationConfig();

    Result GetFilpRotationConfig_SLB(Flipping& flip_x, Flipping& flip_y, Rotation& rotate);
    Result GetFilpRotationConfig_DutLeftEye(Flipping& flip_x, Flipping& flip_y, Rotation& rotate);
    Result GetFilpRotationConfig_DutRightEye(Flipping& flip_x, Flipping& flip_y, Rotation& rotate);
    Result GetFilpRotationConfig(Flipping &flip_x, Flipping &flip_y, Rotation &rotate);

    bool isRotation();

  private:
    ImageRotationConfig();
    Result load();

  private:
    const QString FILE_NAME = "./config/mlcolorimeter/EYE1/ImageRotation.json";
    static ImageRotationConfig *m_instance;
    Json m_json;
};
