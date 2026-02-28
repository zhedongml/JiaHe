#include "metricsdata.h"
#include <QFile>
#include <QMutex>
#include <QTextStream>

MetricsData* MetricsData::self = nullptr;

QString MetricsData::getEyeDirection()
{
	return eyeDirection;
}

void MetricsData::setEyeDirection(QString dir)
{
	eyeDirection = dir;
}

void MetricsData::setCaptureFormatTime(QDateTime time)
{
    formatTime = time.toString("yyyy-MM-dd hh:mm:ss");
}

QString MetricsData::getCaptureFormatTime()
{
    return formatTime;
}

QString MetricsData::getDutBarCode()
{
	return dutId;
}

void MetricsData::setDutBarCode(QString val)
{
	dutId = val;
}

QString MetricsData::getImageNDFilter(QString fileName)
{
    if(m_imageNDFilter.isEmpty()){
        return fileName;
    }else{
        return m_imageNDFilter + "_" + fileName;
    }
}

QString MetricsData::getImageNDFilter()
{
    return m_imageNDFilter;
}

void MetricsData::setImageNDFilter(QString val)
{
    m_imageNDFilter = val;
}

QDateTime MetricsData::getStartRecipeTime()
{
    return m_startRecipeTime;
}

void MetricsData::setStartRecipeTime(QDateTime time)
{
    m_startRecipeTime = time;
}

qint64 MetricsData::getMTFStartTime()
{
	return mtfStartTime;
}

void MetricsData::setMTFStartTime(qint64 val)
{
	mtfStartTime = val;
}

QString MetricsData::getIQRecipeName()
{
    return iqRecipeName;
}

void MetricsData::setIQRecipeName(QString val)
{
    iqRecipeName = val;
}

QString MetricsData::getIQRecipeSeqDir()
{
    return iqRecipeDir;
}

void MetricsData::setIQRecipeSeqDir(QString path)
{
    iqRecipeDir = path;
}

QString MetricsData::getHistoryImagePath()
{
    return m_historyImagePath;
}

void MetricsData::setHistoryImagePath(QString path)
{
    m_historyImagePath = path;
}

QString MetricsData::getOutputRootDir()
{
    return m_outputRootDir;
}

void MetricsData::setOutputRootDir(QString val)
{
    m_outputRootDir = val;
}

QString MetricsData::getOutputImageDir()
{
    return m_outputImageDir;
}

void MetricsData::setOutputImageDir(QString val)
{
    m_outputImageDir = val;
}

void MetricsData::addConnectedDevicesNum()
{
    devicesNum++;
    emit devicesNumChanged(devicesNum);
}

void MetricsData::setLuminanceEfficiencyPara(QString para)
{
    luminanceEfficiencyPara = para;
}

QString MetricsData::getLuminanceEfficiencyPara()
{
    return luminanceEfficiencyPara;
}

void MetricsData::setLuminancePara(QString para)
{
    luminancePara = para;
}

QString MetricsData::getLuminancePara()
{
    return luminancePara;
}

void MetricsData::setChrominancePara(QString para)
{
    chrominancePara = para;
}

QString MetricsData::getChrominancePara()
{
    return chrominancePara;
}

void MetricsData::setLuminanceInfo(QString para)
{
    luminanceInfo=para;
}

QString MetricsData::getLuminanceInfo()
{
    return luminanceInfo;
}

MetricsData::MetricsData()
	: QObject()
{
}

MetricsData::~MetricsData()
{
	self = nullptr;
}

MetricsData* MetricsData::instance()
{
    if (!self)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self)
        {
            self = new MetricsData();
        }
    }
    return self;
}
int  MetricsData::getFiducialType() {
	return fiducialType;
}
void MetricsData::setFiducialType(int type) {
	fiducialType = type;
}
QString MetricsData::getMTFImgsDir() {
	return mtfimgsDir;
}
void MetricsData::setMTFImgsDir(QString direct) {
	mtfimgsDir = direct;
}

QString MetricsData::getRecipeSeqDir()
{
	return recipeSeqDir;
}

void MetricsData::setRecipeSeqDir(QString path)
{
	recipeSeqDir = path;
}


void MetricsData::setEyeboxCount(int count)
{
	eyeboxCount = count;
}

int MetricsData::getEyeboxCount()
{
	return eyeboxCount;
}

void MetricsData::setSequenceId(QString uuid)
{
	seqId = uuid;
}

QString MetricsData::getSequenceId()
{
	return seqId;
}

void MetricsData::setIsConnectErrorShow(bool isShow)
{
    isConnectErrorShow = isShow;
}

bool MetricsData::getIsConnectErrorShow()
{
    return isConnectErrorShow;
}

void MetricsData::setCurDpaColor(DPACOLOR color) {
	curdpacolor = color;
}
QString MetricsData::getCurDpaColorStr() {
	QString strColor = "w";
	switch (curdpacolor)
	{
	case 0:
		strColor = "r";
		break;
	case 1:
		strColor = "g";
		break;
	case 2:
		strColor = "b";
		break;
	case 3:
		strColor = "w";
		break;
	}
	return strColor;
}
DPACOLOR MetricsData::getCurDpaColor() {
	return curdpacolor;
}

int MetricsData::getFiducialCount()
{
	return fiducialCount;
}

void MetricsData::setFiducialCount(int val)
{
	fiducialCount = val;
}

QString MetricsData::getMTFRecipeName()
{
	return recipeName;
}

void MetricsData::setMTFRecipeName(QString val)
{
	recipeName = val;
}

void MetricsData::setISPluminanceData(QString color, double luminance)
{
    m_ISPluminance.insert(color, luminance);
}

QMap<QString, double> MetricsData::getISPluminanceData()
{
    return m_ISPluminance;
}

int MetricsData::getACSType()
{
    return ACSType;
}

void MetricsData::setACSType(int val)
{
    ACSType = val;
}

int MetricsData::getMotion2DType()
{
    return Motion2DType;
}

void MetricsData::setMotion2DType(int val)
{
    Motion2DType = val;
}

Result MetricsData::createCsvResultFile()
{
    Result ret = MetricsData::instance()->writeImageInfoCsv(
        "Name,moduleName,serialNumber,ND_Filter,color,colorLight,exposure,vid,aperture");
    if (!ret.success)
    {
        return ret;
    }
}

Result MetricsData::writeImageInfoCsv(QString message)
{
    QString fileName = MetricsData::instance()->getMTFImgsDir() + "\\" + m_imageInfoFileName;

    return writeToCSV(fileName, message, "Write image info csv failed.");
}

void MetricsData::setDutEyeType(int eyeType)
{
    m_dutEyeType = eyeType;

    ROTATION_MIRROR_TYPE type = eyeType == 0 ? RMT_DUT_LEFT : RMT_DUT_RIGHT;
    m_rotationMirrorType = type;
}
int MetricsData::getDutEyeType()
{
    return m_dutEyeType;
}
void MetricsData::setReticleEyeType(int eyeType)
{
    m_reticleEyeType = eyeType;
}
int MetricsData::getReticleEyeType()
{
    return m_dutEyeType;
    //return m_reticleEyeType;
}
void MetricsData::setRotationMirrorType(ROTATION_MIRROR_TYPE type)
{
    m_rotationMirrorType = type;
}
ROTATION_MIRROR_TYPE MetricsData::getRotationMirrorType()
{
    return m_rotationMirrorType;
}

Result MetricsData::writeToCSV(QString filename, QString msg, QString errorMsg)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << msg << Qt::endl;
        return Result();
    }
    return Result(false, errorMsg.toStdString());
}

QString MetricsData::getEyeboxQueue()
{
    return m_eyeboxQueue;
}
void MetricsData::setEyeboxQueue(QString val)
{
    m_eyeboxQueue = val;
}

void MetricsData::setDutAngle(float dutAngle)
{
    m_dutAngle = dutAngle;
}

float MetricsData::getDutAngle()
{
    if (!GetTestState().IsDut)
    {
        return 0.0;
    }
    return m_dutAngle;
}

void MetricsData::setAlignImageDir(QString dir)
{
    m_alignImageDir = dir;
}

QString MetricsData::getAlignImageDir()
{
    return m_alignImageDir;
}

void MetricsData::setEyeboxIndexCurrent(int index)
{
    m_eyeboxIndexCurrent = index;
}

int MetricsData::getEyeboxIndexCurrent()
{
    return m_eyeboxIndexCurrent;
}

void MetricsData::setLuminanceFile(QString file)
{
    m_luminanceFile = file;
}

QString MetricsData::getLuminanceFile()
{
    return m_luminanceFile;
}

void MetricsData::setCsvPath(QString csv,QString allcsv)
{
    m_allcsvpath = allcsv;
    m_csvpath = csv;
}

QString MetricsData::getAllCsvPath()
{
    return m_allcsvpath;
}

QString MetricsData::getCsvPath()
{
    return m_csvpath;
}

void MetricsData::setDutTestSeqName(QString name)
{
    m_dutTestSeqName = name;
}

QString MetricsData::getDutTestSeqName()
{
    return m_dutTestSeqName;
}

void MetricsData::setSeqImageDir(QString dir)
{
    m_seqImgDir = dir;
}

QString MetricsData::getSeqImageDir()
{
    return m_seqImgDir;
}

void MetricsData::SetTestState(MLUtils::TestState state)
{
    m_TestState = state;
}

MLUtils::TestState MetricsData::GetTestState()
{
    return m_TestState;
}