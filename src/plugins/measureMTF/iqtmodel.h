#pragma once

#include "MLColorimeterMode.h"
#include <QObject>
#include "iqmetricconfig.h"
#include <QHash>
#include <opencv2\opencv.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include "Result.h"
// IQmetrics
#include "ML_FOVOffset.h"
#include"ML_Rotation.h"
#include"ML_FOV.h"
#include"ML_Luminance.h"
#include"ML_Efficiency.h"
#include"ML_LuminanceRolloff.h"
#include"ML_SequentialContrast.h"
#include"MLContrastRatio.h"
#include"ML_Distortion.h"
#include"ML_Flare.h"
#include "ML_Ghost.h"
#include"ML_PupilSwim.h"
#include"ML_denseMTF.h"
#include"ML_LateralColor.h"
#include"ML_CrossMTF.h"

using namespace MLImageDetection;

enum METRICIMGTYPE{
	CHROMAIMAGE = 0,
	YIMAGE = 1,
	MTFIMAGE = 2,
	XYZIMAGE =3
};
class IQTModel  : public QObject
{
	Q_OBJECT

public:
    //typedef QString(IQTModel::*MetricCalculator)(QString, QString, QString);
    typedef QString(IQTModel::* MetricCalculator)(QString, QString, std::vector<cv::Mat>, QString);

	static IQTModel* instance(QObject* parent = nullptr);
	~IQTModel();

	QString calculateMetric(QString metricName,QString color = "w");
	QString generateCsvHeader(QString metricName, QString suffix,QString color="");
	void updateCSVHeaderValues(int id, std::vector<cv::Mat> val, QString metricName, QString color = "");
    void updateCSVHeaderValues(int id,cv::Mat val, QString metricName, QString color = "");
	void updateCSVHeaderValues(int id, QString metricName, double val,QString color = "");
    void updateCompareCSVHeaderValues();
	void updateCSVHeaderItem(QString id, QString header, double val);
    void updatepathItem(QString path, QString id);
	QString getCSVHeaderValue(QString header);
	void setManualCollectImg(bool val);
    void saveMetricsToCsv(QString PathName);
    Result saveRecipeMetricsToCsv(QString PathName);
    bool transposeCsv(QString PathName);
    void updateTestTimeinfo(QString date, QString time);
	bool createMetricsResutCSVFile(QString PathName);
	void calulateAllMetrics();
	Result calulateOneMetricsByRecipe(QString name, QString eyeboxIDs = "", QString color = "");
	cv::Mat getGraySacleImage(bool saveRawData,QString name,QString binnstr="2*2");
    void calCompareMetric(QString h_name, QString m_name, QString id);
    void changeAlogParam(QString param, QString val);

    void setSaveSlbData(bool isSave);
    void deleteDataBase();

    static QHash<QString, MetricCalculator> metricCalculators;

    public slots:
    void wakeupThread(bool isSuccess);

   signals:
	void updateResult(cv::Mat);
	void updateFOVPoints(std::vector<cv::Point2f>);
    void sigInitpython();
   //python sdk call metrics
    void sigCalculateDistortion(string color, int eyeboxId,float degree);
    void sigCalculateFlare(string color, int eyeboxId, float degree);
    void sigCalculateGhost(string color, int eyeboxId, float degree);
    void sigCalculatePupilSwim(string color, int eyeboxId, float degree);
    void sigCalculateStrayLight(string color, int eyeboxId, float degree);
    void sigCalculateColorUniformity(string color, int eyeboxId, float degree,string ndstr);

private:
    bool isDebug = true;
	bool createDir(QString fullPath);
	QString processDataDir = "ProcessData\\";
	IQTModel(QObject* parent = nullptr);
	static IQTModel* self;
	bool isOffline = false;
	MetricDescription curTargetMetric;
	std::vector<MetricDescription*>* metricVec;
	std::vector<cv::Mat> params;

    QThread *m_thread = nullptr;
    QMutex mutex;
    QWaitCondition condition;
    bool m_CalResult = false;

	std::vector<cv::Mat> manualImgs;
	double percent;
	bool manualCollectImgs = false;

    bool m_saveSlbData = false;


    MLIQMetrics::MLLuminance luminance;
    MLIQMetrics::MLEfficiency efficiency;
    MLIQMetrics::MLLuminanceRolloff rollOff;
    MLIQMetrics::MLSequentialContrast seqCR;
    MLIQMetrics::MLFOV fov;
    MLIQMetrics::MLContrastRatio checkerCR;
    MLIQMetrics::MLDistortion distortion;
    MLIQMetrics::MLFOVOffset offset;
    MLIQMetrics::MLRotation rotation;
    MLIQMetrics::MLFlare flare;
    MLIQMetrics::MLGhost ghost;
    MLIQMetrics::MLPupilSwim pupilswim;
  //  MLIQMetrics::MLdenseMTF denseMTF;
    MLIQMetrics::MLLateralColor lateralColor;
    MLIQMetrics::MLCrossMTF crossMTF;
   
	
    QString calculateFOV(QString color, QString mtcName, std::vector<cv::Mat> params,QString id = "");
    QString calculateFOVBig(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");
    QString calculateFOVOffset(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");
    QString calculateRotation(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");
    QString calculateGridDistortion(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");
    QString calculateSequentialContrast(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");
    QString calculateLuminanceUniformity(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");
    QString calculateLuminanceRolloff(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");
    //QString calculateLuminanceRolloff(QString color, QString mtcName, QString id = "");
    QString calculateSmallContrastChessboard(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");
    QString calculateLuminanceEfficiency(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");
    QString calculateFlareC(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");
    QString calculateFlareHDRC(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");

    QString calculateGhostC(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");
    QString calculatePupilSwimC(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");
    QString calculateGridDenseMTF(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");
    QString calculate2degCheckerDenseMTF(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");
    QString calculate5degCheckerDenseMTF(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");

    QString calculateCrossMTF(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");
    QString calculateLateralColor(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");

    // python SDK call metrics
    QString calculateCheckboardDistortion(QString color, QString mtcName, QString id = "");
    QString calculateFlare(QString color, QString mtcName, QString id = "");
    QString calculateGhost(QString color, QString mtcName, QString id = "");
    QString calculatePupilSwim(QString color, QString mtcName, QString id = "");
    QString calculateStrayLight(QString color, QString mtcName, QString id = "");
    QString calculateColorUniformity(QString color, QString mtcName, std::vector<cv::Mat> params, QString id = "");

    void myRotateAntiClockWise90(cv::Mat &src);
    float getExposureTime(QString filePath, QString name);
    MLIQMetrics::ROIParaNew getLuminanceROIandRoMiInfo();
    void writeMatTOCSV(std::string filename, cv::Mat m);
    void writeMatTOCSV(std::string filename, vector<cv::Mat> mVec, vector<string> infoVec);

    void initPythonSDKCall();
    void saveCRToConfigFile(cv::Mat cr, string color, string id);
    void saveROICRToConfigFile(vector<double> cr, string color, string id);
    MLIQMetrics::FOVTYPE getFOVType();
};
