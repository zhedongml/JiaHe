

#include "QDFData.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <chrono>

#include <QFile>
#include <QTextCodec>
#include "PrjCommon/MetricsCheck.h"
#include "PrjCommon/logindata.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>

QDFData::QDFData() {

    setCurrentTimeAsStart();
    getAutoDPCsvDelete(m_autoDPCsvDelete);
}

Result QDFData::getQDFData(QdpCsvHeader& csvHeader)
{
    if (!loadConfig()) {
        return Result(false, QString("Read config file error, %1.").arg(m_configPath).toStdString());
    }

    csvHeader.cm = QString::fromStdString(m_json["qdf_data"]["cm"].get<std::string>());
    csvHeader.factory_id = QString::fromStdString(m_json["qdf_data"]["factory_id"].get<std::string>());
    csvHeader.product_name = QString::fromStdString(m_json["qdf_data"]["product_name"].get<std::string>());
    csvHeader.build_config = QString::fromStdString(m_json["qdf_data"]["build_config"].get<std::string>());
    csvHeader.assembly_phase = QString::fromStdString(m_json["qdf_data"]["assembly_phase"].get<std::string>());
    csvHeader.build_phase = QString::fromStdString(m_json["qdf_data"]["build_phase"].get<std::string>());
    csvHeader.line_id = m_json["qdf_data"]["line_id"].get<int>();
    csvHeader.station_type = QString::fromStdString(m_json["qdf_data"]["station_type"].get<std::string>());
    csvHeader.station_id = QString::fromStdString(m_json["qdf_data"]["station_id"].get<std::string>());
    csvHeader.station_sequence = m_json["qdf_data"]["station_sequence"].get<int>();
    csvHeader.slot = m_json["qdf_data"]["slot"].get<int>();
    csvHeader.fixture = QString::fromStdString(m_json["qdf_data"]["fixture"].get<std::string>());
    //csvHeader.mes_status = m_json["qdf_data"]["mes_status"].get<int>();
    csvHeader.hostname = QString::fromStdString(m_json["qdf_data"]["hostname"].get<std::string>());
    csvHeader.operator_id = QString::fromStdString(m_json["qdf_data"]["operator_id"].get<std::string>());
    csvHeader.diags_version = QString::fromStdString(m_json["qdf_data"]["diags_version"].get<std::string>());
    csvHeader.firmware_version = QString::fromStdString(m_json["qdf_data"]["firmware_version"].get<std::string>());
    csvHeader.os_version = QString::fromStdString(m_json["qdf_data"]["os_version"].get<std::string>());

    //setCurrentTimeAsStart();

    // TODO: MES
    // csvHeader.line_id = 
    // csvHeader.station_id = 
    // csvHeader.station_sequence 
    // csvHeader.test_count 

    csvHeader.start_time = MetricsData::instance()->getRecipeStartDate();

    QDateTime endTime = QDateTime::currentDateTime();
    int secs = MetricsData::instance()->getStartEyeboxTime().secsTo(endTime);
    csvHeader.duration = secs;

    //csvHeader.operator_id = LoginData::instance()->getUserName();
    csvHeader.serial_number = MetricsData::instance()->getDutSN();

    csvHeader.vid = MetricsData::instance()->getDiopter();

    //TODO: to be done
    //csvHeader.zip_files = 

    //TODO: 
    //TODO: test
    QList<QStringList> listHeader;
    listHeader << QStringList(csvHeader.cm) << QStringList(csvHeader.serial_number);
    csvHeader.header_hash = calculateSHA256(listHeader);
    listHeader << QStringList(csvHeader.station_type) << QStringList(csvHeader.start_time);
    csvHeader.program_hash = calculateSHA256(listHeader);

    csvHeader.start_time = setCurrentTimeAsStart();
    return Result();
}

Result QDFData::getAutoDpCsv(QString& filePath, const QString& dirPath)
{
    QDir dir(dirPath);

    QStringList filters;
    filters << "*.csv";

    dir.setNameFilters(filters);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList fileList = dir.entryInfoList();

    if (fileList.size() == 0) {
        return Result(false, QString("Get AutoDp Csv error, dir is %1").arg(dirPath).toStdString());
    }

    //QFileInfoList fileList = dir.entryInfoList(
    //    filters,
    //    QDir::Files,
    //    QDir::Time | QDir::Reversed
    //);

    //if(fileList.size() < 3){
    //    return Result(false, QString("Get AutoDP csv error, csv file count is less than 3, dir is %1.").arg(dirPath).toStdString());
    //}

    //if (fileList.size() > 3) {
    //    return Result(false, QString("Get AutoDP csv error, csv file count is greater than 3, dir is %1.").arg(dirPath).toStdString());
    //}

    QFileInfo latestFile = fileList.first(); 
    for (const QFileInfo& fileInfo : fileList) {
        if (fileInfo.lastModified() > latestFile.lastModified()) {
        //if (fileInfo.birthTime() > latestFile.birthTime()) {
            latestFile = fileInfo;
        }
    }
    QFileInfo newest = latestFile;

    //QFileInfo newest = fileList.first();
    filePath = newest.absoluteFilePath();
    return Result();
}

Result QDFData::updateCsv(const QString& filePath, QString& newFilePath, QdpCsvHeader qdpCsvHeader, QList<QStringList>& initData_, McMetricsResult& mcResult)
{
    m_csvHeader = qdpCsvHeader;

    QList<QStringList> initData;
    Result ret = readCsv(filePath, initData);
    if(!ret.success){
        return ret;
    }

    if(initData.size() < 2){
        return Result(false, "AutoDP export csv info error, row count is less than 2.");
    }
    initData_ = initData;

    QStringList dpTestTime;
    if(initData.size() >=3){
        dpTestTime = initData[2];
    }

    // delete fields
    QStringList dpHeader = initData[0];
    QStringList dpValue = initData[1];

    QString dut_SN = spiltSN(dpValue[3]);

    ret = autoDPCsvDelete(dpHeader, dpValue, dpTestTime);

    if (!ret.success) {
        return Result(false, "Delete AutoDP fields error, " + ret.errorMsg);
    }

    initData_[0] = dpHeader;
    initData_[1] = dpValue;

    // check result
    //McMetricsResult mcResult;
    //metricsResult
    ret = MetricsCheck::instance().getMetricsResultAutoDP(MetricsCheck::instance().getModelName_autoDP(), dpHeader, dpValue, mcResult);
    if (!ret.success) {
        return Result(false, "Check AutoDP limits error, " + ret.errorMsg);
    }
    m_csvHeader.test_result = mcResult.result == METRICS_PASS ? 1 : 2;
    m_csvHeader.failures = mcResult.failStr;

    //bool pass = (mcResult.result == 1);
    //QString errorMsg = (mcResult.result == 1)? "":"Metrics_ERR";

    //MetricsData::instance()->updateDutAutoDPResult(pass);

    bool isEP = MetricsData::instance()->getHolderID().toUpper().endsWith("EP");

    // 1 row: Header
    QStringList newHeader = m_csvHeader.getHeaderNameList(isEP);
    QStringList headerRow = newHeader + dpHeader;

    // limit: 2 row HighLimit, 3 row LowLimit
    QStringList upList;
    QStringList lowList;
    ret = MetricsCheck::instance().getLimitListAutoDP(MetricsCheck::instance().getModelName_autoDP(), headerRow, upList, lowList);
    if(!ret.success){
        return Result(false, "Get AutoDP limits error, " + ret.errorMsg);
    }

    // 4 row: value
    QStringList newValues = m_csvHeader.getValueList(isEP);
    QStringList valueRow = newValues + dpValue;

    newFilePath = getNewCsvName(filePath);
    QList<QStringList> dataAll;
    //dataAll << headerRow << upList << lowList << valueRow;
    QStringList unit;
    unit << "Unit";
    QStringList TestTime;
    newValues.removeAt(0);
    TestTime << "TestTime" << newValues << dpTestTime;
    dataAll << headerRow << upList << lowList << unit << valueRow << TestTime;

    if(false){
        // Hash
        m_csvHeader.header_hash = calculateSHA256(dataAll);
        dataAll.clear();

        // 4 reset row: value
        QStringList newValues = m_csvHeader.getValueList(isEP);
        QStringList valueRow = newValues + dpValue;
        dataAll << headerRow << upList << lowList << unit << valueRow << TestTime;
    }

    ret = writeCsv(newFilePath, dataAll);

   
    if (!ret.success) {
        return Result(false, "Write AutoDP csv error, " + ret.errorMsg);
    }
    
    //bool pass = (mcResult.result == 1);
    ////QString errorMsg = (mcResult.result == 1)? "":"Metrics_ERR";

    ////MetricsData::instance()->appendDutTestResult({ qMakePair(dut_SN, errorMsg) });
    ////MetricsData::instance()->updateDutAutoDPResult(dut_SN, pass);
    //MetricsData::instance()->updateDutAutoDPResult(pass);
    return Result();
}

Result QDFData::appendToCsv(const QString& filePath, const QStringList& valueRow)
{
    if (valueRow.isEmpty()) {
        return Result(false, "Append csv error: Data row is empty");
    }

    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        return Result(false, QString("Append csv error: Open file failed, %1").arg(filePath).toStdString());
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");

    if (file.size() > 0) {
        //stream << "\n";
    }

    try {
        QStringList processedRow;
        for (const QString& field : valueRow) {
            QString processedField = field;

            if (field.contains(',') || field.contains('"') || field.contains('\n')) {
                processedField.replace('"', "\"\"");
                processedField = '"' + processedField + '"';
            }
            processedRow.append(processedField);
        }

        QString line = processedRow.join(",");
        stream << line;

        if (stream.status() != QTextStream::Ok) {
            throw std::runtime_error("Write data failed");
        }

        stream.flush(); 

    }
    catch (const std::exception& e) {
        file.close();
        return Result(false, QString("Append csv error: %1").arg(e.what()).toStdString());
    }

    file.close();
    return Result(true, "Append to csv successfully");
}

Result QDFData::getSummaryData(SummaryCsvHeader& summaryCsvHeader)
{
    if (!loadSummaryConfig()) {
        return Result(false, QString("Read summary config file error, %1.").arg(m_summaryConfigPath).toStdString());
    }

    QString SN = "Unknow";
    auto dut_ref = MetricsData::instance()->queryAdpHistoryQueueFront();
    if (dut_ref) {
        SN = dut_ref.value().get().SN;
    }

    summaryCsvHeader.SerialNumber = SN;// MetricsData::instance()->getDutSN();
    summaryCsvHeader.TesterName = QString::fromStdString(m_summaryJson["SummaryHeader"]["TesterName"].get<std::string>());
    summaryCsvHeader.Appraiser = QString::fromStdString(m_summaryJson["SummaryHeader"]["Appraiser"].get<std::string>());
    QDateTime current = QDateTime::currentDateTime();
    QString time = current.toString("yyyyMMddhhmmss");
    summaryCsvHeader.date = time;
    summaryCsvHeader.Time = setCurrentTimeAsStart();
    summaryCsvHeader.TestResult = getStatusString(m_csvHeader.test_result);
    summaryCsvHeader.first_fail_item = m_csvHeader.failures.split(';').first();
    summaryCsvHeader.all_fail_items = m_csvHeader.failures;

    QDateTime endTime = QDateTime::currentDateTime();
    int secs = MetricsData::instance()->getStartEyeboxTime().secsTo(endTime);
    summaryCsvHeader.total_time = QString::number(secs);

    summaryCsvHeader.mes_status = "";    
    summaryCsvHeader.sw_version = QString::fromStdString(m_summaryJson["SummaryHeader"]["sw_version"].get<std::string>());
    summaryCsvHeader.work_order = QString::fromStdString(m_summaryJson["SummaryHeader"]["work_order"].get<std::string>());
    summaryCsvHeader.build_config = m_csvHeader.build_config;
    summaryCsvHeader.build_phase = m_csvHeader.build_phase;
    summaryCsvHeader.checkroute_status = QString::fromStdString(m_summaryJson["SummaryHeader"]["checkroute_status"].get<std::string>());
    summaryCsvHeader.checkroute_result = QString::fromStdString(m_summaryJson["SummaryHeader"]["checkroute_result"].get<std::string>());
    summaryCsvHeader.keyparts_sn = QString::fromStdString(m_summaryJson["SummaryHeader"]["keyparts_sn"].get<std::string>());
  
    return Result();
}

Result QDFData::updateSummaryCsv(const QString & filePath, QString & newFilePath, SummaryCsvHeader summaryCsvHeader, QList<QStringList> &initData)
{
    m_summaryHeader = summaryCsvHeader;

    //QList<QStringList> initData;
    //Result ret = readCsv(filePath, initData);
    //if (!ret.success) {
    //    return ret;
    //}

    if (initData.size() < 2) {
        return Result(false, "AutoDP export csv info error, row count is less than 2.");
    }

    QStringList dpHeader = initData[0];
    QStringList dpValue = initData[1];

    // check result
    McMetricsResult mcResult;
    Result ret = MetricsCheck::instance().getMetricsResultAutoDP(MetricsCheck::instance().getModelName_autoDP(), dpHeader, dpValue, mcResult);
    if (!ret.success) {
        return Result(false, "Check AutoDP limits error, " + ret.errorMsg);
    }
    m_summaryHeader.TestResult = mcResult.result == METRICS_PASS ? 1 : 2;

    bool isEP = MetricsData::instance()->getHolderID().toUpper().endsWith("EP");

    // 1 row: Header
    QStringList newHeader = m_summaryHeader.getHeaderNameList(isEP);
    QStringList headerRow = newHeader + dpHeader;

    // limit: 2 row HighLimit, 3 row LowLimit
    QStringList upList;
    QStringList lowList;
    ret = MetricsCheck::instance().getLimitListAutoDP(MetricsCheck::instance().getModelName_autoDP(), headerRow, upList, lowList);
    if (!ret.success) {
        return Result(false, "Get AutoDP limits error, " + ret.errorMsg);
    }

    // 4 row: value
    QStringList newValues = m_summaryHeader.getValueList(isEP);
    QStringList valueRow = newValues + dpValue;

    QList<QStringList> dataAll;

    QStringList unit;
    unit << "Unit";

    //summary csv
    QString summaryFilePath = getNewSummaryCsvName(filePath);

    //QFileInfo dirInfo(summaryFilePath);

    if (QFile::exists(summaryFilePath))
    {
        ret = appendToCsv(summaryFilePath, valueRow);
    }
    else
    {
        QList<QStringList> summaryAll;
        summaryAll << headerRow << unit << upList << lowList << valueRow;
        ret = writeCsv(summaryFilePath, summaryAll);
    }

    if (!ret.success) {
        return Result(false, "Write AutoDP csv error, " + ret.errorMsg);
    }
    return Result();
}

QString QDFData::setCurrentTimeAsStart() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count() << "Z";

    m_csvHeader.start_time = QString::fromStdString(ss.str());
    return m_csvHeader.start_time;
}

void QDFData::addDynamicParameter(const std::string& param_name,
    const std::string& value,
    const std::string& low_limit,
    const std::string& high_limit,
    const std::string& unit,
    const std::string& test_time) {
    DynamicParameter param;
    param.value = value;
    param.low_limit = low_limit;
    param.high_limit = high_limit;
    param.unit = unit;
    param.test_time = test_time;
    dynamic_params[param_name] = param;
}

bool QDFData::insertDataToCSV(const std::string& csv_path) {
    auto new_rows = generateCSVRows();
    if (new_rows.empty()) {
        return false;
    }

    std::vector<std::vector<std::string>> existing_data;
    if (!readCSVFile(csv_path, existing_data)) {
        return writeCSVFile(csv_path, new_rows);
    }

    std::vector<std::vector<std::string>> merged_data;
    size_t target_columns = new_rows[0].size();

    for (auto& row : existing_data) {
        if (row.size() < target_columns) {
            row.resize(target_columns, "");
        }
        else if (row.size() > target_columns) {
            row.resize(target_columns);
        }
    }

    for (auto& row : new_rows) {
        if (row.size() < target_columns) {
            row.resize(target_columns, "");
        }
    }

    merged_data.insert(merged_data.end(), new_rows.begin(), new_rows.end());
    merged_data.insert(merged_data.end(), existing_data.begin(), existing_data.end());

    return writeCSVFile(csv_path, merged_data);
}

bool QDFData::generateNewCSV(const std::string& csv_path) {
    auto rows = generateCSVRows();
    return writeCSVFile(csv_path, rows);
}

bool QDFData::updateFromExternalCSV(const std::string& external_csv_path, const std::string& output_csv_path) {
    std::vector<std::vector<std::string>> external_data;
    if (!readCSVFile(external_csv_path, external_data)) {
        std::cerr << "Failed to read external CSV file: " << external_csv_path << std::endl;
        return false;
    }

    if (external_data.size() < 4) {
        std::cerr << "External CSV must have at least 4 rows" << std::endl;
        return false;
    }

    // 1 row: name
    const std::vector<std::string>& param_names_row = external_data[0];
    std::vector<std::string> param_names;
    if (param_names_row.size() > 1) {
        param_names.assign(param_names_row.begin() + 1, param_names_row.end());
    }

    // 2 row: LowLimit
    const std::vector<std::string>& low_limits_row = external_data[1];
    std::vector<std::string> low_limits;
    if (low_limits_row.size() > 1) {
        low_limits.assign(low_limits_row.begin() + 1, low_limits_row.end());
    }

    // 3 row: HighLimit
    const std::vector<std::string>& high_limits_row = external_data[2];
    std::vector<std::string> high_limits;
    if (high_limits_row.size() > 1) {
        high_limits.assign(high_limits_row.begin() + 1, high_limits_row.end());
    }

    // 4 row：Value
    const std::vector<std::string>& values_row = external_data[3];
    std::vector<std::string> values;
    if (values_row.size() > 1) {
        values.assign(values_row.begin() + 1, values_row.end());
    }

    // Check the consistency of the column number
    size_t column_count = param_names.size();
    if (low_limits.size() != column_count ||
        high_limits.size() != column_count ||
        values.size() != column_count) {
        std::cerr << "Column count mismatch in external CSV" << std::endl;
        std::cerr << "Parameters: " << column_count
            << ", LowLimits: " << low_limits.size()
            << ", HighLimits: " << high_limits.size()
            << ", Values: " << values.size() << std::endl;
        return false;
    }

    dynamic_params.clear();

    //add vid info
    if (1)
    {
        addDynamicParameter("vid", QString::number(m_csvHeader.vid, 'f', 3).toStdString(), "", "", "", "");
    }

    for (size_t i = 0; i < column_count; ++i) {
        const std::string& param_name = param_names[i];
        const std::string& value = values[i];
        const std::string& low_limit = low_limits[i];
        const std::string& high_limit = high_limits[i];

        if (!param_name.empty()) {
            addDynamicParameter(param_name, value, low_limit, high_limit, "", "");
        }
    }

    return generateNewCSV(output_csv_path);
}

std::string QDFData::checkOutOfLimitParameters(const std::string& external_csv_path) {
    std::vector<std::vector<std::string>> external_data;
    if (!readCSVFile(external_csv_path, external_data)) {
        std::cerr << "Failed to read external CSV file: " << external_csv_path << std::endl;
        return "";
    }

    if (external_data.size() < 4) {
        std::cerr << "External CSV must have at least 4 rows" << std::endl;
        return "";
    }

    std::vector<std::string> param_names;
    if (external_data[0].size() > 1) {
        param_names.assign(external_data[0].begin() + 1, external_data[0].end());
    }

    std::vector<std::string> low_limits;
    if (external_data[1].size() > 1) {
        low_limits.assign(external_data[1].begin() + 1, external_data[1].end());
    }

    std::vector<std::string> high_limits;
    if (external_data[2].size() > 1) {
        high_limits.assign(external_data[2].begin() + 1, external_data[2].end());
    }

    std::vector<std::string> values;
    if (external_data[3].size() > 1) {
        values.assign(external_data[3].begin() + 1, external_data[3].end());
    }

    size_t column_count = param_names.size();
    if (low_limits.size() != column_count ||
        high_limits.size() != column_count ||
        values.size() != column_count) {
        std::cerr << "Column count mismatch in external CSV" << std::endl;
        return "";
    }

    std::vector<std::string> out_of_limit_params;

    for (size_t i = 0; i < column_count; ++i) {
        const std::string& param_name = param_names[i];
        const std::string& value = values[i];
        const std::string& low_limit = low_limits[i];
        const std::string& high_limit = high_limits[i];

        if (param_name.empty()) {
            continue;
        }

        if (!isValueInLimit(value, low_limit, high_limit)) {
            out_of_limit_params.push_back(param_name);
        }
    }

    return joinStringVector(out_of_limit_params, ";").toStdString();
}

bool QDFData::isValueInLimit(const std::string& value, const std::string& low_limit, const std::string& high_limit) {
    if (low_limit.empty() || high_limit.empty()) {
        return true;
    }

    try {
        double val = std::stod(value);
        double low = std::stod(low_limit);
        double high = std::stod(high_limit);

        return (val >= low) && (val <= high);
    }
    catch (const std::exception&) {
        return (value >= low_limit) && (value <= high_limit);
    }
}

QString QDFData::calculateSHA256(const QList<QStringList>& list)
{
    QStringList strList;
    for(QStringList listOne: list){
        strList.append(listOne);
    }

    QString concatenated = strList.join("");

    QByteArray data = concatenated.toUtf8();

    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

QString QDFData::calculateFileSHA256(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Sha256);
    if (hash.addData(&file)) {
        return QString(hash.result().toHex());
    }

    return QString();
}

QString QDFData::calculateAllFilesSHA256(const std::string& filePath, const std::vector<std::string>& volumeFiles)
{
    QCryptographicHash totalHash(QCryptographicHash::Sha256);

    QString csvPath = QString::fromStdString(filePath);
    QFile csvFile(csvPath);
    if (csvFile.open(QIODevice::ReadOnly)) {
        totalHash.addData(&csvFile);
        csvFile.close();
    }

    for (const auto& volumeFile : volumeFiles) {
        QString volPath = QString::fromStdString(volumeFile);
        QFile file(volPath);
        if (file.open(QIODevice::ReadOnly)) {
            totalHash.addData(&file);
            file.close();
        }
    }

    return QString(totalHash.result().toHex());
}

std::vector<std::vector<std::string>> QDFData::generateCSVRows() {
    std::vector<std::vector<std::string>> rows;

    //std::vector<std::string> param_names;
    //for (const auto& pair : dynamic_params) {
    //    param_names.push_back(pair.first);
    //}
    ////std::sort(param_names.begin(), param_names.end());

    //std::vector<std::string> header_row = {
    //    "Parameter", "header_hash", "program_hash", "cm", "factory_id",
    //    "product_name", "build_config", "assembly_phase", "build_phase",
    //    "line_id", "station_type", "station_id", "station_sequence", "slot",
    //    "fixture", "test_count", "test_result", "test_status", "mes_status",
    //    "failures", "errors", "start_time", "duration", "hostname",
    //    "operator_id", "serial_number", "zip_files", "diags_version",
    //    "firmware_version", "os_version", "vid"
    //};

    //for (const auto& param_name : param_names) {
    //    header_row.push_back(param_name);
    //}

    //rows.push_back(header_row);

    //// LowLimit
    //std::vector<std::string> low_limit_row = { "LowLimit" };
    //low_limit_row.insert(low_limit_row.end(), 30, ""); // 固定字段占位符
    //for (const auto& param_name : param_names) {
    //    low_limit_row.push_back(dynamic_params[param_name].low_limit);
    //}
    //rows.push_back(low_limit_row);

    //// HighLimit
    //std::vector<std::string> high_limit_row = { "HighLimit" };
    //high_limit_row.insert(high_limit_row.end(), 30, "");
    //for (const auto& param_name : param_names) {
    //    high_limit_row.push_back(dynamic_params[param_name].high_limit);
    //}
    //rows.push_back(high_limit_row);

    //// Unit 行
    //std::vector<std::string> unit_row = { "Unit" };
    //unit_row.insert(unit_row.end(), 30, "");
    //for (const auto& param_name : param_names) {
    //    unit_row.push_back(dynamic_params[param_name].unit);
    //}
    //rows.push_back(unit_row);

    //// Value row
    //std::vector<QString> value_row = { "Value" };
    //value_row.push_back(m_csvHeader.header_hash);
    //value_row.push_back(m_csvHeader.program_hash);
    //value_row.push_back(m_csvHeader.cm);
    //value_row.push_back(m_csvHeader.factory_id);
    //value_row.push_back(m_csvHeader.product_name);
    //value_row.push_back(m_csvHeader.build_config);
    //value_row.push_back(m_csvHeader.assembly_phase);
    //value_row.push_back(m_csvHeader.build_phase);
    //value_row.push_back(QString::number(m_csvHeader.line_id));
    //value_row.push_back(m_csvHeader.station_type);
    //value_row.push_back(m_csvHeader.station_id);
    //value_row.push_back(QString::number(m_csvHeader.station_sequence));
    //value_row.push_back(QString::number(m_csvHeader.slot));
    //value_row.push_back(m_csvHeader.fixture);
    //value_row.push_back(QString::number(m_csvHeader.test_count));
    //value_row.push_back(QString::number(m_csvHeader.test_result));
    //value_row.push_back(QString::number(m_csvHeader.test_status));
    //value_row.push_back(QString::number(m_csvHeader.mes_status));
    //value_row.push_back(m_csvHeader.failures);
    //value_row.push_back(m_csvHeader.errors);
    //value_row.push_back(m_csvHeader.start_time);
    //value_row.push_back(QString::number(m_csvHeader.duration));
    //value_row.push_back(m_csvHeader.hostname);
    //value_row.push_back(m_csvHeader.operator_id);
    //value_row.push_back(m_csvHeader.serial_number);
    //value_row.push_back(joinStringVector(m_csvHeader.zip_files, ";"));
    //value_row.push_back(m_csvHeader.diags_version);
    //value_row.push_back(m_csvHeader.firmware_version);
    //value_row.push_back(m_csvHeader.os_version);
    //value_row.push_back(QString::number(m_csvHeader.vid));

    //// 添加参数值
    //for (const auto& param_name : param_names) {
    //    value_row.push_back(dynamic_params[param_name].value);
    //}

    //rows.push_back(value_row);

    //// TestTime 行
    //std::vector<std::string> test_time_row = { "TestTime" };
    //test_time_row.insert(test_time_row.end(), 30, ""); // 固定字段占位符
    //for (const auto& param_name : param_names) {
    //    test_time_row.push_back(dynamic_params[param_name].test_time);
    //}
    //rows.push_back(test_time_row);

    return rows;
}

QString QDFData::joinStringVector(const std::vector<std::string>& vec,
    const std::string& delimiter) {
    std::string result;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) result += delimiter;
        result += vec[i];
    }
    return QString::fromStdString(result);
}

bool QDFData::readCSVFile(const std::string& file_path,
    std::vector<std::vector<std::string>>& data) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            if (!cell.empty() && cell.front() == '"' && cell.back() == '"') {
                cell = cell.substr(1, cell.length() - 2);
            }
            row.push_back(cell);
        }
        data.push_back(row);
    }

    file.close();
    return true;
}

bool QDFData::writeCSVFile(const std::string& file_path,
    const std::vector<std::vector<std::string>>& data) {
    std::ofstream file(file_path);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << escapeCSVField(row[i]);
            if (i < row.size() - 1) {
                file << ",";
            }
        }
        file << "\n";
    }

    file.close();
    return true;
}

std::string QDFData::escapeCSVField(const std::string& field) {
    if (field.find(',') != std::string::npos ||
        field.find('"') != std::string::npos ||
        field.find('\n') != std::string::npos) {
        std::string escaped = "\"";
        for (char c : field) {
            if (c == '"') escaped += "\"\"";
            else escaped += c;
        }
        escaped += "\"";
        return escaped;
    }
    return field;
}

bool QDFData::loadConfig()
{
    std::ifstream jsonFile(m_configPath.toStdString());
    if (jsonFile.is_open())
    {
        std::string contents =
            std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
        jsonFile.close();
        m_json = Json::parse(contents);
    }
    else
    {
        return false;
    }

    return true;
}

bool QDFData::loadSummaryConfig()
{
    std::ifstream jsonFile(m_summaryConfigPath.toStdString());
    if (jsonFile.is_open())
    {
        std::string contents =
            std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
        jsonFile.close();
        m_summaryJson = Json::parse(contents);
    }
    else
    {
        return false;
    }

    return true;
}

QString QDFData::getStatusString(int status) {
    switch (status) {
    case 1: return "PASS";
    case 2: return "FAIL";
    case 3: return "ERROR";
    case 4: return "ABORT";
    case 5: return "COF";
    default: return "UNKNOWN";
    }
}

QString QDFData::getNewCsvName(const QString& filePath)
{
    QFileInfo info(filePath);
    QString dir = info.absolutePath();
    QDateTime current = QDateTime::currentDateTime();
    QString time = current.toString("yyyyMMddhhmmss");
    //QString newName = info.baseName() + "_autoDP.csv";
    QString newName = m_csvHeader.cm + "_" + m_csvHeader.product_name + "_" + m_csvHeader.station_type + "_" + m_csvHeader.station_id + "_" + m_csvHeader.serial_number + "_" + getStatusString(m_csvHeader.test_result) + "_" + time + ".qdf.csv";
    QString newFilePath = dir + "/" + newName;
    return newFilePath;
}
QString QDFData::getNewSummaryCsvName(const QString& filePath)
{
    QFileInfo info(filePath);
    QString dir = info.absolutePath();
    QDateTime current = QDateTime::currentDateTime();
    QString time = current.toString("yyyyMMdd");

    QString newName = m_csvHeader.cm + "_" + m_csvHeader.product_name + "_" + m_csvHeader.station_id + "_" + m_summaryHeader.TesterName + "_" + m_summaryHeader.Appraiser + "_Summary_" + time + ".csv";
    QString summaryFilePath = dir + "/" + newName;
    return summaryFilePath;
}

Result QDFData::readCsv(QString filePath, QList<QStringList>& data)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return Result(false, QString("Read Auto DP metrics limit file error, Open file failed, %1").arg(filePath).toStdString());

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

Result QDFData::writeCsv(QString filePath, const QList<QStringList>& data)
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

Result QDFData::setQDFData()
{
    Result ret = getQDFData(m_csvHeader);
    return ret;
}

Result QDFData::autoDPCsvDelete(QStringList& dpHeader, QStringList& dpValue, QStringList &dpTestTime)
{
    QStringList deleteFields;
    Result ret = getAutoDPCsvDelete(deleteFields);
    if(!ret.success){
        return ret;
    }

    if(dpTestTime.size() != dpHeader.size()){
        for (const QString& field : deleteFields) {
            int index = dpHeader.indexOf(field);
            if (index != -1) {
                dpHeader.removeAt(index);
                dpValue.removeAt(index);
            }
        }
    }else{
        for (const QString& field : deleteFields) {
            int index = dpHeader.indexOf(field);
            if (index != -1) {
                dpHeader.removeAt(index);
                dpValue.removeAt(index);
                dpTestTime.removeAt(index);
            }
        }
    }

    return Result();
}

Result QDFData::getAutoDPCsvDelete(QStringList& deleteFields)
{
    deleteFields.clear();
    QList<QStringList> dataDelete;
    Result ret = readCsv(m_autoDPCsvDeletePath, dataDelete);
    if(!ret.success){
        QString msg = QString("Read file error, %1, %2.").arg(m_autoDPCsvDeletePath);
        qCritical() << msg;
        return Result(false, msg.toStdString());
    }

    QStringList result;
    for (auto& lst : dataDelete) deleteFields += lst;
    return Result();
}

Result QDFData::setSummaryData()
{
    Result ret = getSummaryData(m_summaryHeader);
    return ret;
}

QString QDFData::spiltSN(QString line)
{
    //3N2QG01HBJ0013.20251227T153449
    if (line.contains("."))
    {
        QStringList list = line.split(".");
        return list[0];
    }
    else
        return line;
}