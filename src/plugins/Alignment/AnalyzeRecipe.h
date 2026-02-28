#pragma once
#include <QString>
#include "piMotionActor/CalibrationConfig.h"
#include "PrjCommon/service/ml_motion.h"

class AnalyzeRecipe
{
  public:
    // return fiducial mode: 0 manual calibrate fiducial; 1 auto identify fiducial.
    AlignConfig analyzCalibrateParam(QString param);

    Result analyzMvExposeParam(QString param);

    Result analyzDutConfigParam(QString param, DutConfig &dutConfig);

    // return eyebox index
    Result analyzEeyeboxParam(QString param, int &index, bool &moveout);

    QList<QString> selectCalibrateInfo(QString fileName, DutConfig &dutConfig, AlignConfig &alignConfig);

    Result analyzeLines(QList<QString> lines, DutConfig &dutConfig, AlignConfig &alignConfig);

    Result analyzeCalibrateConfig(QString param, CalibrateConfig &calConfig);

    std::vector<cv::Point3f> analyzCoords(QString coordParam);

  private:

    QList<QString> selectInfo(QList<QString> lineDatas, DutConfig &dutConfig, AlignConfig &alignConfig);
    QList<QString> readFile(QString fileName);
    QList<QString> splitParam(QString cmd, QString param);

};
