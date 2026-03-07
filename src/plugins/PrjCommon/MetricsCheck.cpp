#include "MetricsCheck.h"
#include <QFile>
#include <QTextCodec>
#include <QDebug>

MetricsCheck& MetricsCheck::instance()
{
    static MetricsCheck self;
    return self;
}

bool MetricsCheck::limitUpdate()
{
    return m_limitUpdate && m_checkInfo.checkEnabled;
}

void MetricsCheck::setHeaderList(QStringList headerList, QString eyeboxQueue)
{
    m_headerListCurrent = headerList;
    m_headerMap[eyeboxQueue] = headerList;
}

QStringList MetricsCheck::getHeaderList()
{
    return m_headerListCurrent;
}

bool MetricsCheck::existHeaderList(QString eyeboxQueue)
{
    if(m_headerMap.contains(eyeboxQueue))
    {
        m_headerListCurrent = m_headerMap[eyeboxQueue];
        return true;
    }
    return false;
}

Result MetricsCheck::setModelName(QString modelName, bool liveRefresh)
{
    m_checkInfo.modelName = modelName;

    if(limitUpdate()){
        if(modelName.isEmpty()){
            return Result(false, "Metrics limit model name is empty.");
        }

        if(liveRefresh && m_limitsMap.contains(modelName)){
            m_limitsMap.remove(modelName);
        }

        QMap<QString, McMetricsLimit> limits;
        Result ret = readLimit(modelName, limits);
        if(!ret.success){
            return ret;
        }
    }
    return Result();
}

void MetricsCheck::setCheckEnabled(bool enabled)
{
    m_checkInfo.checkEnabled = enabled;
}

QString MetricsCheck::getModelName()
{
    return m_checkInfo.modelName;
}

Result MetricsCheck::setModelName_autoDP(QString modelName, bool liveRefresh)
{
    m_checkInfo_autoDP.modelName = modelName;

    if (m_checkInfo_autoDP.checkEnabled) {
        if (modelName.isEmpty()) {
            return Result(false, "Metrics limit model name is empty.");
        }

        if (liveRefresh && m_limitsMap.contains(modelName)) {
            m_limitsMap.remove(modelName);
        }

        QMap<QString, McMetricsLimit> limits;
        Result ret = readLimit(modelName, limits);
        if (!ret.success) {
            return ret;
        }
    }
    return Result();
}

void MetricsCheck::setCheckEnabled_autoDP(bool enabled)
{
    m_checkInfo_autoDP.checkEnabled = enabled;
}

QString MetricsCheck::getModelName_autoDP()
{
    return m_checkInfo_autoDP.modelName;
}

Result MetricsCheck::getLimitList(const QString& modelName, const QStringList& headerList, QStringList& upList, QStringList& lowList)
{
    QMap<QString, McMetricsLimit> limitMap;
    Result ret = readLimit(modelName, limitMap);
    if(!ret.success){
        return ret;
    }

    upList.append("uplimit");
    lowList.append("lowlimit");
    for(int i = 1; i < headerList.size(); ++i){
        QString key;
        QString name = headerList[i];
        Result ret = getKey(name, key);
        if (!ret.success) {
            return ret;
        }

        if (!limitMap.contains(key)) {
            upList.append("");
            lowList.append("");
        }else{
            float up = limitMap[key].up;
            float low = limitMap[key].low;
            if(qAbs(up - NULL_VALUE) < 1e-6 || qAbs(low - NULL_VALUE) < 1e-6){
                upList.append("NULL");
                lowList.append("NULL");
            }else{
                qAbs(up - INVALID_VALUE) > 1e-6 ? upList.append(QString::number(up, 'f', DECIMAL_NUMBER)) : upList.append("");
                qAbs(low - INVALID_VALUE) > 1e-6 ? lowList.append(QString::number(low, 'f', DECIMAL_NUMBER)) : lowList.append("");
            }
        }
    }
    return Result();
}

Result MetricsCheck::getMetricsResult(const QString& modelName, const QStringList& headerList, QStringList& valueList, McMetricsResult& mcResult)
{
    if(!limitUpdate()){
        return Result();
    }

    QStringList valueListTmp = valueList;

    if(headerList.size() != valueListTmp.size()){
        return Result(false, "Check metrics result error, header number and value number is not equal.");
    }

    QMap<QString, McMetricsLimit> limitMap;
    Result ret = readLimit(modelName, limitMap);
    if (!ret.success) {
        return ret;
    }

    mcResult = McMetricsResult();

    QMap<QString, bool> resultMap;
    for(int i = 1; i < headerList.size(); ++i){
        QString key;
        QString name = headerList[i];
        ret = getKey(name, key);
        if (!ret.success) {
            return ret;
        }

        if (!limitMap.contains(key)) {
            resultMap[name] = true;
        }
        else {
            float up = limitMap[key].up;
            float low = limitMap[key].low;

            if(valueListTmp[i].isEmpty()){
                if (qAbs(up - NULL_VALUE) < 1e-6  || qAbs(low - NULL_VALUE) < 1e-6) {
                    resultMap[name] = true;
                }else{
                    mcResult.failList.append(name);
                }
                continue;
            }

            bool result = true;
            if (qAbs(up - INVALID_VALUE) > 1e-6) {
                result = valueListTmp[i].toFloat() <= up;
            }

            if (result) {
                if (qAbs(low - INVALID_VALUE) > 1e-6) {
                    result = valueListTmp[i].toFloat() >= low;
                }
            }

            if(!result){
                mcResult.failList.append(name);
            }
        }
    }

    int testResultNumber = -1;
    int testFailItemNumber = -1;
    for (int i = 1; i < headerList.size(); ++i) {
        QString name = headerList[i];
        if (name == TEST_RESULT) {
            testResultNumber = i;
        }
        else if (name == TEST_FAIL_ITEM)
        {
            testFailItemNumber = i;
        }

        if(testResultNumber > 0 && testFailItemNumber > 0){
            break;
        }
    }

    if(mcResult.failList.size() == 0){
        mcResult.result = METRICS_PASS;
        
        valueList[testResultNumber] = "Pass";
        valueList[testFailItemNumber] = "NONE";
        qWarning() << "Metric limit check passed.";
        //LoggingWrapper::instance()->info("Metric limit check passed.");
    }else{
        mcResult.result = METRICS_FAIL;
        //mcResult.failStr = mcResult.failList.join(";");
        valueList[testResultNumber] = "Fail";
        valueList[testFailItemNumber] = mcResult.failList.join(";");
        qCritical() << "Metric limit check failed: " + mcResult.failList.join(";");
        //LoggingWrapper::instance()->error("Metric limit check failed: " + mcResult.failStr);
    }

    MetricsData::instance()->setMetricsResult(mcResult);
    return Result();
}

Result MetricsCheck::getLimitListAutoDP(const QString& modelName, const QStringList& headerList, QStringList& upList, QStringList& lowList)
{
    if (modelName.isEmpty()) {
        return Result(false, "Read limit error, modelName is empty.");
    }

    QMap<QString, McMetricsLimit> limitMap;
    Result ret = readLimit(modelName, limitMap);
    if (!ret.success) {
        return ret;
    }

    upList.append("HighLimit");
    lowList.append("LowLimit");
    for (int i = 1; i < headerList.size(); ++i) {
        QString name = headerList[i];
        QString key = name;
        //Result ret = getKey(name, key);
        //if (!ret.success) {
        //    return ret;
        //}

        if (!limitMap.contains(key)) {
            upList.append("");
            lowList.append("");
        }
        else {
            float up = limitMap[key].up;
            float low = limitMap[key].low;
            if (qAbs(up - NULL_VALUE) < 1e-6 || qAbs(low - NULL_VALUE) < 1e-6) {
                upList.append("NULL");
                lowList.append("NULL");
            }
            else {
                qAbs(up - INVALID_VALUE) > 1e-6 ? upList.append(QString::number(up, 'f', DECIMAL_NUMBER)) : upList.append("");
                qAbs(low - INVALID_VALUE) > 1e-6 ? lowList.append(QString::number(low, 'f', DECIMAL_NUMBER)) : lowList.append("");
            }
        }
    }
    return Result();
}

Result MetricsCheck::getMetricsResultAutoDP(const QString& modelName, const QStringList& headerList, QStringList& valueList, McMetricsResult& mcResult)
{
    if (!m_checkInfo_autoDP.checkEnabled) {
        return Result();
    }

    QStringList valueListTmp = valueList;

    if (headerList.size() != valueListTmp.size()) {
        return Result(false, "Check metrics result error, header number and value number is not equal.");
    }

    QMap<QString, McMetricsLimit> limitMap;
    Result ret = readLimit(modelName, limitMap);
    if (!ret.success) {
        return ret;
    }

    mcResult = McMetricsResult();

    QMap<QString, bool> resultMap;
    for (int i = 1; i < headerList.size(); ++i) {
        QString name = headerList[i];
        QString key = name;
        //ret = getKey(name, key);
        //if (!ret.success) {s
        //    return ret;
        //}

        if (!limitMap.contains(key)) {
            resultMap[name] = true;
        }
        else {
            float up = limitMap[key].up;
            float low = limitMap[key].low;

            if (valueListTmp[i].isEmpty()) {
                if (qAbs(up - NULL_VALUE) < 1e-6 || qAbs(low - NULL_VALUE) < 1e-6) {
                    resultMap[name] = true;
                }
                else {
                    mcResult.failList.append(name);
                }
                continue;
            }

            bool result = true;
            if (qAbs(up - INVALID_VALUE) > 1e-6) {
                result = valueListTmp[i].toFloat() <= up;
            }

            if (result) {
                if (qAbs(low - INVALID_VALUE) > 1e-6) {
                    result = valueListTmp[i].toFloat() >= low;
                }
            }

            if (!result) {
                mcResult.failList.append(name);
            }
        }
    }

    int testResultNumber = -1;
    int testFailItemNumber = -1;
    for (int i = 1; i < headerList.size(); ++i) {
        QString name = headerList[i];
        if (name == TEST_RESULT) {
            testResultNumber = i;
        }
        else if (name == TEST_FAIL_ITEM)
        {
            testFailItemNumber = i;
        }

        if (testResultNumber > 0 && testFailItemNumber > 0) {
            break;
        }
    }

    if (mcResult.failList.size() == 0) {
        mcResult.result = METRICS_PASS;
        mcResult.failStr = "";
        //valueList[testResultNumber] = "Pass";
        //valueList[testFailItemNumber] = "NONE";
        qWarning() << "Metric limit check passed.";
        //LoggingWrapper::instance()->info("Metric limit check passed.");
    }
    else {
        mcResult.result = METRICS_FAIL;
        mcResult.failStr = mcResult.failList.join(";");
        //valueList[testResultNumber] = "Fail";
        //valueList[testFailItemNumber] = mcResult.failList.join(";");
        qWarning() << "Metric limit check failed: " + mcResult.failStr;
        //LoggingWrapper::instance()->error("Metric limit check failed: " + mcResult.failStr);
    }

    MetricsData::instance()->setMetricsResult_autoDP(mcResult);
    return Result();
}

Result MetricsCheck::readLimit(QString modelName, QMap<QString, McMetricsLimit>& limits)
{
    if (modelName.isEmpty()) {
        return Result(false, "Read limit error, modelName is empty.");
    }

    if(m_limitsMap.contains(modelName)){
        limits = m_limitsMap[modelName];
        return Result();
    }

    QString filePath = getFileName(modelName);
    QList<QStringList> datas;
    Result ret = readCsv(filePath, datas);
    if(!ret.success){
        return ret;
    }

    if(datas.size() <= 1){
        return Result(false, QString("Metrics limit is empty, " + filePath).toStdString());
    }

    limits.clear();
    for(int i = 1; i < datas.size(); ++i){
        QStringList row = datas[i];
        if (row.size() < 3) {
            return Result(false, QString("Read Metrics limit file error, filePath is %1, row info is less than 3 is missing, %2.")
                .arg(filePath).arg(row.join(",")).toStdString());
        }

        QString key;
        ret = getKey(row[0].trimmed(), key);
        if(!ret.success){
            return Result(false, QString("Read Metrics limit file error, filePath is %1, %2.")
                .arg(filePath).arg(QString::fromStdString(ret.errorMsg)).toStdString());
        }

        McMetricsLimit limit;
        limit.name = key;

        if(row[1].trimmed() == "NULL" || row[2].trimmed() == "NULL"){
            limit.up = NULL_VALUE;
            limit.low = NULL_VALUE;
        }else{
            limit.up = row[1].trimmed().isEmpty() ? INVALID_VALUE : row[1].trimmed().toFloat();
            limit.low = row[2].trimmed().isEmpty() ? INVALID_VALUE : row[2].trimmed().toFloat();
        }

        limits[limit.name] = limit;
    }

    m_limitsMap[modelName] = limits;
	return Result();
}

Result MetricsCheck::checkMetrics(const QMap<QString, McMetricsLimit>& limits, const QList<QString>& metricsNames, const QMap<QString, float>& metricsMap, QMap<QString, int>& checkResults)
{
    for(QString name: metricsNames){
        QString key;
        Result ret = getKey(name, key);
        if(!ret.success){
            return ret;
        }

        if(!limits.contains(key)){

            continue;
        }

        float up = limits[key].up;
        float low = limits[key].low;

        if(qAbs(up - INVALID_VALUE) < 1e-6 && qAbs(low - INVALID_VALUE) < 1e-6){
            checkResults[name] = 0;
        }
        else{
            bool result = true;
            if (qAbs(up - INVALID_VALUE) > 1e-6) {
                result = metricsMap[name] <= up;
            }

            if (result) {
                if (qAbs(low - INVALID_VALUE) > 1e-6) {
                    result = metricsMap[name] >= low;
                }
            }
            checkResults[name] = result ? 1 : -1;
        }
    }

	return Result();
}

Result MetricsCheck::writeResult(const QList<QString>& metricsNames, QMap<QString, int>& checkResults)
{
	return Result();
}

Result MetricsCheck::readCsv(QString filePath, QList<QStringList>& data)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return Result(false, QString("Read metrics limit file error, Open file failed, %1").arg(filePath).toStdString());

    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
    QByteArray content = file.readAll();
    QString text = codec->toUnicode(content);
    QStringList lines = text.split('\n');

    for (int i = 0; i < lines.size(); i++)
    {
        if (!lines.at(i).isEmpty())
        {
            QStringList row = lines.at(i).split(',');
            data.append(row);
        }
    }

    file.close();
    return Result();
}

Result MetricsCheck::writeCsv(QString filePath, QList<QStringList>& data)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
        return Result(false, QString("Write csv error, Open file failed, %1").arg(filePath).toStdString());

    QTextCodec* codec = QTextCodec::codecForName("UTF-8");

    for (int i = 0; i < data.size(); i++)
    {
        QStringList row = data.at(i);
        QString line = row.join(",");
        line += "\n";
        QByteArray encodedLine = codec->fromUnicode(line);
        file.write(encodedLine);
    }

    file.close();
    return Result();
}

QString MetricsCheck::getFileName(const QString& modelName)
{
    QString filePath = FILE_DIR + "metricsLimit_" + modelName + ".csv";
    return filePath;
}

Result MetricsCheck::getKey(const QString& metricsName, QString& key)
{
    if (metricsName.contains("Eyebox")) {
        int pos = metricsName.indexOf("Eyebox");
        if (pos <= 3) {
            return Result(false, "Check metrics error code 1.");
        }
        key = metricsName.left(pos - 3);
    }
    else {
        key = metricsName;
    }
    return Result();
}
