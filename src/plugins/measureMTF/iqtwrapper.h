#pragma once

#include <QObject>
#include "MLColorimeterMode.h"
#include "PrjCommon/irecipewrapper.h"
#include <json.hpp>
#include "PrjCommon/RecipeAsync.h"

using Json = nlohmann::json;

class IQTWrapper : public IRecipeWrapper
{
    Q_OBJECT

  public:
    IQTWrapper(QWidget *measureWidget, QObject *parent);
    ~IQTWrapper();

  public:
    void Invoke(QString cmd, QString param);
    void notifyStop(bool isstop) override;
    void notifyPause(bool isPause) override;

    bool writeFileJson(const char *path, Json json);
    void loadconfig();
    void updateconfig(Json &Luminance,QStringList list);

  private:
    Result createNewResultFile(QString param);
    bool isDirExist(QString fullPath);
    Result measure(QString param);
    Result switchND(QString param);
    Result switchXYZ(QString param);
    Result setExpTime(QString param);
    Result getXyzImage(QString param, bool isCorrectionImage);
    Result getxyYImage(QString param, bool isCorrectionImage);
    Result captureYImage(QString param);
    Result captureRawImage(QString param);
    Result captureImageFFC(QString param);
    Result createMtricTable(QString param);
    Result createMetricHistoryTable(QString param);
    Result calcOneMetris(QString param);
    Result exportToCSVFile(QString param);
    Result calculateTime(QString param);
    Result changeCameraFeature(QString param);
    Result stopCameraGrabing(QString param);
    Result compareMetrics(QString param);
    Result changeBrightnessLevel(QString param);

    Result CameraConnect(QString param);
    QString getTxtLine(QString name,QString line);
    Result judgeEyebox(QString param);
    QString getEyebox(QString param);
    Result saveMetricsInfo(QString line);
    Result isSLB(QString param);

    Result imageRotateMode(QString param);
    Result cylinderMirrorChange(QString param);

    Result setOutputDir(QString param);
    Result setAlignmentDir(QString param);
    Result setMeasureDir(QString param);

    Result setEyeboxIndex(QString param);
    Result setImageNewName(QString param);

    Result luminanceStatistics(QString param);
    Result LuminanceIS(QString param);

    Result autoFocus(QString param);
    Result waitCaptureImageEnd(QString param);
    Result exposureManage(QString param);

    //TODO: test
    Result recipeAsync(QString param);
    Result testData(int mSecond);

  private:
    bool checkStrEquals(QString str0, QString str1);
    Result anlyzeCaptureImage(QString &fileName, XyzExposureCache &aeCache, bool &saveRawImage, bool &saveSLBYY, bool& luminanceSave, QString param);
    Result anlyzeCaptureImage(QString &fileName, ImgExposureCache &aeCache, bool &saveRawImage, QString param);

    Result createCsv();
  private:
    QWidget *m_measureWidget = nullptr;
    int metricTableNum = 9;
    Json settingJsonObj;
    QMap<QString, int> modeTime;
    QDateTime start;
    string m_configPath = "./config/Algorithmconfig.json";
    QString m_box;
};
