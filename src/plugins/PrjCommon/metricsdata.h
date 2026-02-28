#pragma once

#include <QObject>
#include "prjcommon_global.h"
#include <vector>
#include <map>
#include <QDateTime>
#include <QMap>
#include "opencv2/opencv.hpp"
#include "Result.h"
#include "MLUtilCommon.h"

using namespace std;

enum DPACOLOR {
	RED = 0,
    GREEN = 1,
	BLUE = 2,
	WHITE =3,
	CLOSE =4
};

enum ROTATION_MIRROR_TYPE
{
    RMT_DUT_LEFT,
    RMT_DUT_RIGHT,
    RMT_SLB
};

struct ImageNameInfo
{
    QString deviceColor = "R";
    QString deviceImage = "Sloid";
    cv::Point3f eyeboxCoord = cv::Point3f(0, 0, 0);
    QString xyzFilter = "X"; // X,Y,Z
    QString ndFilter = "ND0";
    QString imgFileName;

    bool isRawImage = true;

    ImageNameInfo()
    {
    }

    ~ImageNameInfo()
    {
    }

    QString imageFileNotDevice()
    {
        return QString("X%1Y%2Z%3_EYE_%4")
            .arg(eyeboxCoord.x)
            .arg(eyeboxCoord.y)
            .arg(eyeboxCoord.z)
            .arg(xyzFilter);
    }

    QString imageFile()
    {
        return QString("result%1").arg(xyzFilter);
    }
};

struct OtherImageInfo{
    QString colorIS;                    // R,G,B
    QString reticle;                    // Clear, chessboard
	QMap<QString, float> labsphereNitsMap;  // key:R,G,B; value:labsphere nit

	void setLatestIScolor(QString color)
    {
        colorIS = color.toLower();
    }

	void setLatestReticle(QString reticle)
    {
        reticle = reticle.toLower();
    }

	void setLabsphereNitsMap(QString color, float nit){
        labsphereNitsMap[color.toLower()] = nit;
	}
};

class PRJCOMMON_EXPORT MetricsData : public QObject
{
	Q_OBJECT

public:
	~MetricsData();
	static MetricsData* instance();
	QString getEyeDirection();
	void setEyeDirection(QString dir);
    void setCaptureFormatTime(QDateTime time);
    QString getCaptureFormatTime();
	int getFiducialType();
	void setFiducialType(int type);
	QString getMTFImgsDir();
	void setMTFImgsDir(QString direct);
	QString getRecipeSeqDir();
	void setRecipeSeqDir(QString path);
	void setEyeboxCount(int count);
	int getEyeboxCount();
	void setSequenceId(QString uuid);
	QString getSequenceId();
    void setIsConnectErrorShow(bool isShow);
    bool getIsConnectErrorShow();
	void setCurDpaColor(DPACOLOR color);
	DPACOLOR getCurDpaColor();
	QString getCurDpaColorStr();
	int getFiducialCount();
	void setFiducialCount(int val);

	QString getMTFRecipeName();
	void setMTFRecipeName(QString val);

	QString getDutBarCode();
	void setDutBarCode(QString val);

    QString getImageNDFilter(QString fileName);
    QString getImageNDFilter();
    void setImageNDFilter(QString val);

	qint64 getMTFStartTime();
	void setMTFStartTime(qint64 val);

	QString getIQRecipeName();
    void setIQRecipeName(QString val);

	QString getIQRecipeSeqDir();
    void setIQRecipeSeqDir(QString path);

    QString getHistoryImagePath();
    void setHistoryImagePath(QString path);

    // new output data dir 2025.3.7
    QString getOutputRootDir();
    void setOutputRootDir(QString val);
    QString getOutputImageDir();
    void setOutputImageDir(QString val);

	void setISPluminanceData(QString color, double luminance);
    QMap<QString, double> getISPluminanceData();
    void addConnectedDevicesNum();

    void setLuminanceEfficiencyPara(QString para);
    QString getLuminanceEfficiencyPara();
    void setLuminancePara(QString para);
    QString getLuminancePara();
    void setChrominancePara(QString para);
    QString getChrominancePara();
    void setLuminanceInfo(QString para);
    QString getLuminanceInfo();

	int getACSType();
    void setACSType(int val);

	int getMotion2DType();
    void setMotion2DType(int val);

	Result createCsvResultFile();
	Result writeImageInfoCsv(QString message);

    void setDutEyeType(int eyeType);
    int getDutEyeType();
    void setReticleEyeType(int eyeType);
    int getReticleEyeType();

    void setRotationMirrorType(ROTATION_MIRROR_TYPE type);
    ROTATION_MIRROR_TYPE getRotationMirrorType();

    QString getEyeboxQueue();
    void setEyeboxQueue(QString val);

    void setDutAngle(float dutAngle);
    float getDutAngle();

    void setAlignImageDir(QString dir);
    QString getAlignImageDir();

    void setEyeboxIndexCurrent(int index);
    int getEyeboxIndexCurrent();

    void setLuminanceFile(QString file);
    QString getLuminanceFile();

    //Result csv path
    void setCsvPath(QString csv, QString allcsv = "");
    QString getCsvPath();
    QString getAllCsvPath();
    //Dut seq name
    void setDutTestSeqName(QString name);
    QString getDutTestSeqName();
    //Seq img save dir
    void setSeqImageDir(QString dir);
    QString getSeqImageDir();
    //Recipe time
    QDateTime getStartRecipeTime();
    void setStartRecipeTime(QDateTime time);
    //Dut type state
    void SetTestState(MLUtils::TestState state);
    MLUtils::TestState GetTestState();

  signals:
    void devicesNumChanged(int);

private:
    Result writeToCSV(QString filename, QString msg, QString errorMsg);

  public:
    QString m_eyeboxQueue;

	// other info: Integrating sphere, Reticle
    //OtherImageInfo m_otherImageInfo;

  private :
	MetricsData();

    static MetricsData *self;

	QString eyeDirection = "Left";
	QString mtfimgsDir = "";
	int fiducialType = 1;
	QString recipeSeqDir = "d:";
	QString seqId = "";
	int eyeboxCount = 9;
	DPACOLOR curdpacolor = DPACOLOR::CLOSE;
	int fiducialCount = 2;
	QString recipeName = "";
	QString dutId = "";
    QString iqRecipeName = "";
	qint64 mtfStartTime;
    QString iqRecipeDir = "";
    int devicesNum = 0;
    QMap<QString, double> m_ISPluminance;

    QString luminanceEfficiencyPara = "";
    QString luminancePara = "";
    QString chrominancePara = "";
    QString luminanceInfo = "";
    QString formatTime;
	QString dutSN = "";
	int ACSType = 0;
	int Motion2DType = 0;
    bool isConnectErrorShow = true;
	const QString m_imageInfoFileName = "Image info.csv";

    QDateTime startTime;

    int m_dutEyeType = RMT_DUT_RIGHT;
    ROTATION_MIRROR_TYPE m_rotationMirrorType = RMT_SLB;
    int m_reticleEyeType = RMT_DUT_LEFT;

    bool m_IQSLB = false;

    float m_dutAngle = 0.0f;

    QString m_alignImageDir;

    QString m_outputRootDir;
    QString m_outputImageDir;
    int m_eyeboxIndexCurrent;

    QString m_luminanceFile;
    QString m_imageNDFilter;
	QString m_historyImagePath;

    QString m_allcsvpath;
    QString m_csvpath;
    QString m_dutTestSeqName;
    QString m_seqImgDir;
    QDateTime m_startRecipeTime;
    MLUtils::TestState m_TestState;
};
