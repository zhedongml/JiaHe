#include "metricsdata.h"
#include <QFile>
#include <QMutex>
#include <QTextStream>
#include <QRandomGenerator>
#include "LogPlus.h"
#include "TipsDialog.h"
#include <QThread>
#include "CsvDequeRecorder.h"

template<typename T>
std::optional<std::reference_wrapper<T>> peek_back_ref_mutable(std::deque<T>& dq) {
    if (dq.empty()) {
        return std::nullopt;
    }
    return std::ref(dq.back());
}
//template<typename T>
//std::optional<T> pop_front_ref_mutable(std::deque<T>& dq) {
//    if (dq.empty()) {
//        return std::nullopt;
//    }
//    T value = dq.front();
//    dp.pop_front();
//    return value;
//}

template<typename T>
std::optional<T> pop_front_safe(std::deque<T>& dq) {
    if (dq.empty()) {
        return std::nullopt;
    }
    T value = std::move(dq.front());
    dq.pop_front();
    return value;
}

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

QString MetricsData::getDutName()
{
    return dutName;
}

void MetricsData::setDutName(QString val)
{
    dutName = val;
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

QString MetricsData::getDutSN()
{
    //QString DutSN = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
    //return DutSN;
    return dutSN;
}

void MetricsData::setDutSN(QString val)
{
    dutSN = val;
}

//DUT Name
void MetricsData::setDutType(QString val)
{
    dutType = val;
}

QString MetricsData::getDutType()
{
    return dutType;
}

void MetricsData::setAllCsvDir(QString allcsvDir)
{
    m_allcsvDir = allcsvDir;
}

QString MetricsData::getAllCsvDir()
{
    return m_allcsvDir;
}

void MetricsData::resetDutMeasureHash()
{
    m_DutMeasureHash.clear();
}

void MetricsData::insertDutMeasureHash()
{
    m_DutMeasureHash.clear();
}

QHash<QString, bool> MetricsData::getDutMeasureHash()
{
    return m_DutMeasureHash;
}

void MetricsData::setDutMeasureResult(bool succ)
{
    m_MeasureResult = succ;
}

bool MetricsData::getDutMeasureResult()
{
    return m_MeasureResult;
}

//void MetricsData::updateDutMeasureResult()
//{
 //   bool success = true;// getDutMeasureResult();
	//QString DutSN = getDutSN();

 //   appendDutTestResult({ qMakePair(DutSN, success) });
//}

//void MetricsData::updateDutPickResult()
//{
//    auto dut_ref = popMetricsQueueFront();
//
//    if (!dut_ref.has_value()) {
//        return ;
//    }
//
//    DutMeasureInfo _dut = dut_ref.value();
//    updateDutPickResult(_dut);
//}

void MetricsData::updateDutAutoDPResult(QString ErrorMsg)
{
    auto dut_ref = popAdpHistoryQueueFront();

    if (!dut_ref.has_value()) {
        return;
    }

    DutMeasureInfo _dut = dut_ref.value();

    _dut.ErrorMsg += ErrorMsg;

    updateDutProcessResult(_dut);
}

//void MetricsData::updateDutAutoDPResult(QString SN,bool pass)
//{
//    std::string _sn = SN.toStdString();
//
//    auto dut_ref = popMetricsQueueFront();
//
//    if (!dut_ref.has_value()) {
//        return;
//    }
//
//    DutMeasureInfo _dut = dut_ref.value();
//
//    if (!pass)
//    {
//        _dut.ErrorMsg += "Metrics_ERR";
//    }
//
//    updateDutProcessResult(_dut);
//}


//void MetricsData::updateDutProcessResult(QString SN)
//{
//    std::string _sn = SN.toStdString();
//
//    if (m_DutAutoDPQueue.find(_sn) != m_DutAutoDPQueue.end())
//    {
//        m_DutAutoDPQueue[_sn].Metrics_OK = pass;
//
//        if (!pass)
//        {
//            m_DutAutoDPQueue[_sn].ErrorMsg = "Metrics_Fail";
//        }
//
//        DutMeasureInfo _dut = m_DutAutoDPQueue[_sn];
//
//        updateDutProcessResult(_dut);
//
//        //删除数据
//        size_t numRemoved = m_DutAutoDPQueue.erase(_sn);
//
//        //QString message = QString::fromStdString("Recipe Node [ IQ_UpdateFlipRotation ] run error, update Flip_Rotate.json error.");
//        //LoggingWrapper::instance()->error(message);
//
//        std::cout << "remove " << numRemoved << " dut info from AutoDP queue\n";
//    }
//
//    emit sig_dutProcessEnd(info);
//}

void MetricsData::updateDutProcessResult(DutMeasureInfo info)
{
    appendDutTestResult({QVector<QString>{info.SN, QString::number(info.LayerIndex), 
        QString::number(info.DutIndex), info.ErrorMsg}});
}

void MetricsData::updateDutPickResult(DutMeasureInfo info)
{
    emit sig_dutPickEnd(info);
    updateNowLayer(info.LayerIndex);

    appendDutPickResult({ QVector<QString>{info.SN, QString::number(info.LayerIndex),
    QString::number(info.DutIndex), info.ErrorMsg} });

}

void MetricsData::appendDutTestResult(const QList<QVector<QString>> list)
{
    emit sig_appendDutTestResult(list);
}

void MetricsData::appendDutPickResult(const QList<QVector<QString>> list)
{
    for (const auto& vector : list) {
        QHash<QString, QVariant> hash;
        QString posMsg = QString("%1.%2").arg(vector[1]).arg(vector[2]);
        QDateTime dateTime = QDateTime::currentDateTime();
        QString str = dateTime.toString("yyyy-MM-dd hh:mm:ss");

        hash.insert("SN", vector[0]);
        hash.insert("Pos", posMsg);
        hash.insert("Status", vector[3]);
        hash.insert("Time", str);

        emit sig_appendDutPickResult({ hash });
    }
}

void MetricsData::updateMetricsProgressBar(int value)
{
    emit sig_updateMetricsProgressBar(value,true);
}

void MetricsData::setAutoTaskDutIndex(int value)
{
    m_AutoTaskDutIndex = value;

	//QHash<QString, int> dataMap;
 //   dataMap["Tray.DoneIndex"] = 0;
	//dataMap["History.Index"] = done;
	//dataMap["History.Done"] = done;
	//dataMap["History.Fail"] = fail;

	//notifyListeners(dataMap);
//
//
//    emit sig_updateMetricsProgressBar(value, true);
}

int MetricsData::getAutoTaskDutIndex()
{
    return m_AutoTaskDutIndex;
}

void MetricsData::setAutoTaskDutCount(int value)
{
    m_AutoTaskDutCount = value;
}

int MetricsData::getAutoTaskDutCount()
{
    return m_AutoTaskDutCount;
}

void MetricsData::pushDutPutQueue(DutMeasureInfo _dut)
{
    m_DutPutQueue.push_back(_dut);
}

void MetricsData::pushDutMeasureQueue(DutMeasureInfo _dut)
{
    m_DutMeasureQueue.push_back(_dut);
}

void MetricsData::pushDutPickQueue(DutMeasureInfo _dut)
{
    m_DutPickQueue.push_back(_dut);
}

void MetricsData::pushDutMetricsQueue(DutMeasureInfo _dut)
{
    m_DutMetricsQueue.push_back(_dut);
}

bool MetricsData::pushDutAdpHistoryQueue(DutMeasureInfo _dut)
{
    return CsvDequeRecorder::GetInstance()->push_back(_dut);
}

const int MetricsData::getDutMeasureSize()
{
    return m_DutMeasureQueue.size();
}

const int MetricsData::getDutPickSize()
{
    return m_DutPickQueue.size();
}
//const int MetricsData::getDutMeasureTrayIndex(DutMeasureInfo& _dut)
//{
//    if (m_DutMeasureQueue.size() > 0) {
//        _dut = m_DutMeasureQueue.front();
//        m_DutMeasureQueue.pop_front();
//        //_dut = m_DutMeasureQueue.dequeue();
//        //std::string msg = printAllQueue();
//        //LOG4CPLUS_INFO(LogPlus::getInstance()->logger, msg.c_str());
//        return _dut.DutIndex;
//    }
//    //LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, printAllQueue().c_str());
//    return -1;
//}
//
//const int MetricsData::getDutPickTrayIndex(DutMeasureInfo& _dut)
//{
//    if (m_DutPickQueue.size() > 0) {
//        _dut = m_DutPickQueue.front();
//        m_DutPickQueue.pop_front();
//        //_dut = m_DutPickQueue.pop_front();// .dequeue();
//        //std::string msg = printAllQueue();
//        //LOG4CPLUS_INFO(LogPlus::getInstance()->logger, msg.c_str());
//        return _dut.DutIndex;
//    }
//    //LOG4CPLUS_INFO(LogPlus::getInstance()->logger, printAllQueue().c_str());
//    return -1;
//}

std::optional<DutMeasureInfo> MetricsData::popPutQueueFront()
{
    if (m_DutPutQueue.empty()) {
        return std::nullopt;
    }

    DutMeasureInfo value = m_DutPutQueue.front();
    m_DutPutQueue.pop_front();
    return value;

    //return pop_front_ref_mutable(m_DutPutQueue);
}

std::optional<DutMeasureInfo> MetricsData::popMeasureQueueFront()
{
    if (m_DutMeasureQueue.empty()) {
        return std::nullopt;
    }

    DutMeasureInfo value = m_DutMeasureQueue.front();
    m_DutMeasureQueue.pop_front();
    return value;

    //return pop_front_ref_mutable(m_DutMeasureQueue);
}

std::optional<DutMeasureInfo> MetricsData::popPickQueueFront()
{
    if (m_DutPickQueue.empty()) {
        return std::nullopt;
    }

    DutMeasureInfo value = m_DutPickQueue.front();
    m_DutPickQueue.pop_front();
    return value;
}

std::optional<DutMeasureInfo> MetricsData::popMetricsQueueFront()
{
    if (m_DutMetricsQueue.empty()) {
        return std::nullopt;
    }

    DutMeasureInfo value = m_DutMetricsQueue.front();
    m_DutMetricsQueue.pop_front();
    return value;
}

std::optional<DutMeasureInfo> MetricsData::popAdpHistoryQueueFront()
{
    return CsvDequeRecorder::GetInstance()->pop_front();
    //if (m_DutAutoDPQueue.empty()) {
    //    return std::nullopt;
    //}

    //DutMeasureInfo value = m_DutAutoDPQueue.front();
    //m_DutAutoDPQueue.pop_front();
    //return value;

    //return pop_front_ref_mutable(m_DutMeasureQueue);
}

//std::optional<DutMeasureInfo> MetricsData::popMetricsQueueBackSN(QString _SN)
//{
//    //return CsvDequeRecorder::GetInstance()->pop_back();
//    if(CsvDequeRecorder::GetInstance()->empty()){// (m_DutAutoDPQueue.empty()) {
//        return std::nullopt;
//    }
//
//    std::deque<DutMeasureInfo> temp;
//    DutMeasureInfo lastElement;
//
//    while (!m_DutAutoDPQueue.empty()) {
//        DutMeasureInfo current = m_DutAutoDPQueue.front();
//        m_DutAutoDPQueue.pop_front();
//
//        if (m_DutAutoDPQueue.empty()) {
//            lastElement = current;
//        }
//        else {
//            temp.push_back(std::move(current));
//        }
//    }
//
//    m_DutAutoDPQueue = std::move(temp);
//
//    std::cout << "queue last SN: " << lastElement.SN.toStdString() << " Input SN:"<< _SN.toStdString()<<"\n";
//
//    lastElement.SN;
//
//    return lastElement;
//
//    //std::deque<DutMeasureInfo> tempQueue;
//    //    while (!m_DutAutoDPQueue.empty()) {
//    //        DutMeasureInfo element = std::move(m_DutAutoDPQueue.front());
//    //        m_DutAutoDPQueue.pop();
//
//    //        if (predicate(element)) {
//    //            DutMeasureInfo.push(std::move(element));
//    //        }
//    //    }
//
//    //bool find = false;
//
//    //DutMeasureInfo result;
//
//    //std::deque<DutMeasureInfo> tempQueue;
//
//    //while (!m_DutAutoDPQueue.empty()) {
//    //    DutMeasureInfo _dut = m_DutAutoDPQueue.front();
//    //    m_DutAutoDPQueue.pop_front();
//
//    //    if (!find && _dut.SN == _SN)
//    //    {
//    //        result = _dut;
//    //        find = true;
//    //        continue;
//    //    }
//    //    else
//    //    {
//    //        tempQueue.push_back(_dut);
//    //    }
//    //}
//
//    //while (!tempQueue.empty()) {
//    //    DutMeasureInfo _dut = tempQueue.front();
//    //    tempQueue.pop_front();
//    //    m_DutAutoDPQueue.push_back(_dut);
//    //}
//
//    //return result;
//}

std::optional<std::reference_wrapper<DutMeasureInfo>> MetricsData::queryPutQueueFront()
{
    return peek_front_ref_mutable(m_DutPutQueue);
}

std::optional<std::reference_wrapper<DutMeasureInfo>> MetricsData::queryMeasureQueueFront()
{
    return peek_front_ref_mutable(m_DutMeasureQueue);
}

std::optional<std::reference_wrapper<DutMeasureInfo>> MetricsData::queryPickQueueFront()
{
    return peek_front_ref_mutable(m_DutPickQueue);
}

std::optional<std::reference_wrapper<DutMeasureInfo>> MetricsData::queryMetricsQueueFront()
{
    return peek_front_ref_mutable(m_DutMetricsQueue);
}

std::optional<std::reference_wrapper<DutMeasureInfo>> MetricsData::queryAdpHistoryQueueFront()
{
    return CsvDequeRecorder::GetInstance()->query_front();
    //return peek_front_ref_mutable(m_DutAutoDPQueue);
}

bool MetricsData::updatePutMeasureQueue()
{
    QMutexLocker locker(&m_PutMeasureMutex);

    auto _dut = popPutQueueFront();
    if (_dut.has_value()) {
        pushDutMeasureQueue(_dut.value());
        return true;
    }
    else
    {
        return false;
    }
}

bool MetricsData::updateMeasurePickQueue()
{
    QMutexLocker locker(&m_MeasurePickMutex);

    //std::optional<DutMeasureInfo> popPutQueueFront();
    //std::optional<DutMeasureInfo> popMeasureQueueFront();
    //std::optional<DutMeasureInfo> popPickQueueFront();

    auto _dut = popMeasureQueueFront();
    if (_dut.has_value()) {
        //DutMeasureInfo _dut_copy = _dut.value();
        pushDutPickQueue(_dut.value());
        //pushDutAutoDPQueue(_dut_copy);
        //m_DutAutoDPQueue.push_back(_dut_copy);
        //std::string _SN = _dut_copy.SN.toStdString();
        //m_DutAutoDPQueue.insert(std::make_pair(_SN,_dut_copy));
        return true;
    }
    else
    {
        return false;
    }
}

//bool MetricsData::updatePickMetricsQueue()
//{
//    QMutexLocker locker(&m_MeasurePickMutex);
//
//    //std::optional<DutMeasureInfo> popPutQueueFront();
//    //std::optional<DutMeasureInfo> popMeasureQueueFront();
//    //std::optional<DutMeasureInfo> popPickQueueFront();
//
//    auto _dut = popMeasureQueueFront();
//    if (_dut.has_value()) {
//        DutMeasureInfo _dut_copy = _dut.value();
//        pushDutPickQueue(_dut.value());
//        std::string _SN = _dut_copy.SN.toStdString();
//        m_DutAutoDPQueue.insert(std::make_pair(_SN, _dut_copy));
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}

QString MetricsData::printQueueInfo(QString type, std::deque<DutMeasureInfo> DataBuffer)
{
    QString msg = type +" :{";
    for (const DutMeasureInfo& value : DataBuffer) {
        msg += QString::number(value.DutIndex) + ",";
    }
    msg += "}";

    return msg;
}

std::string MetricsData::swapPutMeasurePickQueue()
{
    updateMeasurePickQueue();

    updatePutMeasureQueue();

    //std::string msg = printAllQueue();
    //LOG4CPLUS_INFO(LogPlus::getInstance()->logger, msg.c_str());
    //QString msg1 = printQueueInfo("Put", m_DutPutQueue);
    //QString msg2 = printQueueInfo("Measure", m_DutMeasureQueue);
    //QString msg3 = printQueueInfo("Pick", m_DutPickQueue);

    //std::string msg = (msg1 + msg2 + msg3).toStdString();
    return "";
}

std::string MetricsData::printAllQueue()
{
    QString msg1 = printQueueInfo("Put", m_DutPutQueue);
    QString msg2 = printQueueInfo("Measure", m_DutMeasureQueue);
    QString msg3 = printQueueInfo("Pick", m_DutPickQueue);
    QString msg4 = printQueueInfo("Metrics", CsvDequeRecorder::GetInstance()->getDeque());

    std::string msg = ("DUT process queue: " + msg1 + "  " + msg2 + "  " + msg3 + "  || " + msg4).toStdString();
    //std::string msg = ("DUT process queue: " + msg1 + "  " + msg2 + "  " + msg3).toStdString();

    return msg;
}

int MetricsData::getDutPendingCount()
{
    return m_DutPutQueue.size() + m_DutMeasureQueue.size() + m_DutPickQueue.size();
}

int MetricsData::getDutPutPendingCount()
{
    return m_DutPutQueue.size();
}

int MetricsData::getDutUnpickCount()
{
    return m_DutPutQueue.size() + m_DutMeasureQueue.size();
}

bool MetricsData::resetPutMeasurePickQueue()
{
    m_DutPutQueue.clear();
    m_DutMeasureQueue.clear();
    m_DutPickQueue.clear();
    m_DutMetricsQueue.clear();
    return true;
}

bool MetricsData::resetAdpHistoryQueue()
{
    CsvDequeRecorder::GetInstance()->clear();
    return true;
}


//void MetricsData::updateDutIndex(int value)
//{
//    QHash<QString, int> dataMap;
//    dataMap["History.Index"] = done;
//    dataMap["History.Done"] = done;
//    dataMap["History.Fail"] = fail;
//
//    notifyListeners(dataMap);
//
//
//    emit sig_updateMetricsProgressBar(value, true);
//}

void MetricsData::updateNowStatCount(int value)
{
    emit sig_updateMetricsProgressBar(value, true);
}

const int MetricsData::getAutoTaskTrayGridCount()
{
    return 15;
}

void MetricsData::setAutoTaskLayerCount(int value)
{
    m_AutoTaskLayerCount = value;
}

int MetricsData::getAutoTaskLayerCount()
{
    return m_AutoTaskLayerCount;
}

int MetricsData::getAutoTaskDoneCount()
{
    return m_AutoTaskDoneCount;
}

void MetricsData::updateAutoTaskDoneCount()
{
    m_AutoTaskDoneCount++;
}

void MetricsData::resetAutoTaskDoneCount()
{
    m_AutoTaskDoneCount = 0;
}

void MetricsData::setMetricsResult(McMetricsResult metricsResult)
{
    m_metricsResult = metricsResult;
}

void MetricsData::setRecipeResultState(METRICS_STATE retState)
{
    m_metricsResult.result = retState;
}

McMetricsResult MetricsData::getMetricsResult()
{
    return m_metricsResult;
}

void MetricsData::setMetricsResult_autoDP(McMetricsResult metricsResult)
{
    m_metricsResult_autoDP = metricsResult;
}

void MetricsData::setRecipeResultState_autoDP(METRICS_STATE retState)
{
    m_metricsResult_autoDP.result = retState;
}

McMetricsResult MetricsData::getMetricsResult_autoDP()
{
    return m_metricsResult_autoDP;
}

void MetricsData::setDiopter(float diopter)
{
    m_imageNameInfo.diopter = diopter;
}

float MetricsData::getDiopter()
{
    return m_imageNameInfo.diopter;
}

void MetricsData::setHolderID(const QString& holderID)
{
    m_holderID = holderID;
}

QString MetricsData::getHolderID()
{
    return m_holderID;
}

QString MetricsData::getRecipeStartTime()
{
    return recipeStartTime;
}

void MetricsData::setRecipeStartTime(QString val)
{
    recipeStartTime = val;
}

QString MetricsData::getRecipeStartDate()
{
    return recipeStartDate;
}

void MetricsData::setRecipeStartDate(QString val)
{
    recipeStartDate = val;
}

void MetricsData::setRecipeTotalSeconds(int seconds)
{
    m_recipeTotalSeconds = seconds;
}

int MetricsData::getRecipeTotalSeconds()
{
    return m_recipeTotalSeconds;
}

QDateTime MetricsData::getStartEyeboxTime()
{
    return startTime;
}

void MetricsData::setStartEyeboxTime(QDateTime time)
{
    startTime = time;
}

void MetricsData::updateNowLayer(int value)
{
    emit sig_updateNowLayer(value);
    m_NowLayerNum = value;
}

int MetricsData::getNowLayer()
{
    return m_NowLayerNum;
}

void MetricsData::updateSoftRunningState()
{
    static int count = 0;

    if (count % 4 == 0)
    {
        emit sig_updateSoftRunningState();
        count = 1;
    }
    else
        count++;
}

bool MetricsData::showTipsDialog(QString title, QString msg, QString yesInfo, QString noInfo)
{
    TipsDialog dialog(title, msg, yesInfo, noInfo);
    int nResult = dialog.exec();
    if (nResult == QDialog::Accepted) //点击了确定按钮
    {
    	return true;
    }
    else
    {
    	return false;
    }
}

bool MetricsData::popTipsDialog(QString title, QString info, QString yesInfo, QString noInfo)
{
    bool ok = false;

    if (QThread::currentThread() != QCoreApplication::instance()->thread()) {
        QMetaObject::invokeMethod(qApp, [&]() {
            ok = showTipsDialog(title, info, yesInfo, noInfo);
            }, Qt::BlockingQueuedConnection);
    }
    else {
        ok = showTipsDialog(title, info, yesInfo, noInfo);
    }

    return ok;
}

int MetricsData::getExposureMode()
{
    return m_ExposureMode;
}

void MetricsData::setExposureMode(int mode)
{
    m_ExposureMode = mode;
}

void MetricsData::clearDutGridStatus()
{
    emit sig_resetGridStatus();
}

void MetricsData::clearHistoryDoneList()
{
    emit sig_clearHistoryDoneList();
}

//void MetricsData::setAutoTaskDutCount(int value)
//{
//    m_AutoTaskDutCount = value;
//
//    QHash<QString, int> dataMap;
//
//    dataMap["Tray.DoneIndex"] = 0;
//    dataMap["Tray.GridNum"] = getAutoTaskTrayGridCount();
//    dataMap["Tray.LayerNum"] = getAutoTaskLayerCount();
//    dataMap["Tray.TaskNum"] = value;
//    dataMap["Tray.Progress"] = 0;
//
//    dataMap["Progress.Alignment"] = 0;
//    dataMap["Progress.Capture"] = 0;
//    dataMap["Progress.Measure"] = 0;
//    dataMap["Progress.Entirety"] = 0;
//    dataMap["Tray.Progress"] = 0;
//    dataMap["Tray.Progress"] = 0;
//
//    notifyListeners(dataMap);
//}

IDataChangeListener::IDataChangeListener() {

}

IDataChangeListener::~IDataChangeListener() {
}

