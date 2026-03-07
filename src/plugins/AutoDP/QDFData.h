#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <json.hpp>
#include <fstream>

#include <iostream>
#include <QObject>
#include <QCryptographicHash>
#include <QStringList>
#include <QFile>
#include <QTextStream>

#include "Result.h"
#include "PrjCommon/metricsdata.h"
#include "autodp_global.h"

using Json = nlohmann::json;

struct SummaryCsvHeader {
    QString SerialNumber;
    QString TesterName;
    QString Appraiser;
    QString date;
    QString Time;
    QString TestResult;
    QString first_fail_item;
    QString all_fail_items;
    QString total_time;
    QString mes_status = "";
    QString sw_version = "V1.0.0";
    QString work_order;
    QString build_config;
    QString build_phase;
    QString checkroute_status;
    QString checkroute_result;
    QString keyparts_sn;

    //VID
    double vid = 0.0f;

    QStringList getValueList(bool isEP) {
        QStringList valueList;
        valueList << SerialNumber << TesterName << Appraiser << date;
        valueList << Time << TestResult << first_fail_item << all_fail_items;
        valueList << total_time << mes_status << sw_version << work_order << build_config;
        valueList << build_phase << checkroute_status << checkroute_result << keyparts_sn;

        if (isEP) {
            valueList.append(QString::number(vid, 'f', 3));
        }

        valueList.append("");
        return valueList;
    }

    QStringList getHeaderNameList(bool isEP) {
        QStringList headerNames = {
            "SerialNumber", "TesterName", "Appraiser", "date", "Time",
            "TestResult", "first_fail_item", "all_fail_items", "total_time",
            "mes_status", "sw_version", "work_order", "build_config", "build_phase",
            "checkroute_status", "checkroute_result", "keyparts_sn"
        };

        if (isEP) {
            headerNames.append("VID(diopter)");
        }

        headerNames.append("");
        return headerNames;
    }

    QString getHeaderNames(bool isEP) {
        QStringList headerNames = getHeaderNameList(isEP);
        return headerNames.join(",");
    }

};

struct QdpCsvHeader{
    QString Parameter;

    // Hash information
    QString header_hash;
    QString program_hash;

    // Basic information (32-character limit
    QString cm = "sunny_omnilight";
    QString factory_id = "yuyao";
    QString product_name = "Plaid";
    QString build_config;
    QString assembly_phase = "MODULE";
    QString build_phase = "P1";

    // Work station information
    int line_id = 1;
    QString station_type = "DIQ-01-WG-L";
    QString station_id = "1";
    int station_sequence = 1;
    int slot = 1;
    QString fixture;

    // Test result
    int test_count = 0;
    int test_result = 1;  // 1-PASS, 2-FAIL, 3-ERROR, 4-ABORT, 5-COF
    int test_status = 1;  // 1-PRIME, 2-FA, 3-REWORK, 4-GR&R, 5-REL, 6-GOLDEN, 7-DOE, 8-DEBUG
    int mes_status = 1;   // 1-online, 0-offline
    QString failures;
    QString errors;

    // Time
    QString start_time;
    float duration = 0.0f;

    // System information (64 character limit)
    QString hostname;
    QString operator_id;
    QString serial_number;

    // file info
    QStringList zip_files;

    // Version information (32-character limit)
    QString diags_version;
    QString firmware_version;
    QString os_version;

    //VID
    double vid = 0.0f;

    QStringList getValueList(bool isEP) {
        QStringList valueList;
        valueList << "Value" << header_hash << program_hash << cm << factory_id;
        valueList << product_name << build_config << assembly_phase << build_phase;
        valueList << QString::number(line_id) << station_type << station_id << QString::number(station_sequence) << QString::number(slot);
        valueList << fixture << QString::number(test_count) << QString::number(test_result) << QString::number(test_status) << QString::number(mes_status);
        valueList << failures << errors << start_time << QString::number(duration, 'f', 3) << hostname;
        valueList << operator_id << serial_number << zip_files.join(";") << diags_version;
        valueList << firmware_version << os_version;

        if(isEP){
            // valueList.append(QString::number(MetricsData::instance()->getDiopter(), 'f', 3));
            valueList.append(QString::number(vid, 'f', 3));
        }

        return valueList;
    }

    QStringList getHeaderNameList(bool isEP) {
        QStringList headerNames = { 
            "Parameter", "header_hash", "program_hash", "cm", "factory_id",
            "product_name", "build_config", "assembly_phase", "build_phase",
            "line_id", "station_type", "station_id", "station_sequence", "slot",
            "fixture", "test_count", "test_result", "test_status", "mes_status",
            "failures", "errors", "start_time", "duration", "hostname",
            "operator_id", "serial_number", "zip_files", "diags_version",
            "firmware_version", "os_version"
        };

        if (isEP) {
            headerNames.append("VID(diopter)");
        }

        return headerNames;
    }

    QString getHeaderNames(bool isEP) {
        QStringList headerNames = getHeaderNameList(isEP);
        return headerNames.join(",");
    }

};

class AUTODP_EXPORT QDFData {
private:
    struct DynamicParameter {
        std::string value;
        std::string low_limit;
        std::string high_limit;
        std::string unit;
        std::string test_time;
    };

    std::unordered_map<std::string, DynamicParameter> dynamic_params;

public:
    QDFData();

    Result getQDFData(QdpCsvHeader &csvHeader);
    Result getAutoDpCsv(QString & filePath, const QString &dirPath);
    Result updateCsv(const QString &filePath, QString& newFilePath, QdpCsvHeader qdpCsvHeader, QList<QStringList>& initData, McMetricsResult& metricsResult);
    
    Result appendToCsv(const QString& filePath, const QStringList& valueRow);
    Result getSummaryData(SummaryCsvHeader& summaryCsvHeader);
    Result updateSummaryCsv(const QString& filePath, QString& newFilePath, SummaryCsvHeader summaryCsvHeader, QList<QStringList>& initData);

    QString setCurrentTimeAsStart();
     
    void addDynamicParameter(const std::string& param_name,
        const std::string& value = "",
        const std::string& low_limit = "",
        const std::string& high_limit = "",
        const std::string& unit = "",
        const std::string& test_time = "");

    bool insertDataToCSV(const std::string& csv_path);

    bool generateNewCSV(const std::string& csv_path);

    bool updateFromExternalCSV(const std::string& external_csv_path, const std::string& output_csv_path);

    std::string checkOutOfLimitParameters(const std::string& external_csv_path);

    void setHeaderHash(const QString& hash) { m_csvHeader.header_hash = hash; }
    void setProgramHash(const QString& hash) { m_csvHeader.program_hash = hash; }
    void setTestCount(int count) { m_csvHeader.test_count = count; }
    void setTestResult(int result) { m_csvHeader.test_result = result; }
    void setFailures(const QString& failure_list) { m_csvHeader.failures = failure_list; }
    void setErrors(const QString& error_list) { m_csvHeader.errors = error_list; }
    void setSerialNumber(const QString& sn) { m_csvHeader.serial_number = sn; }
    void addZipFile(const QString& zip_file) { m_csvHeader.zip_files.push_back(zip_file); }
    void setVid(double vid_) { m_csvHeader.vid = vid_; }

private:

    QString calculateSHA256(const QList<QStringList>& list);

    QString calculateFileSHA256(const QString& filePath);

    QString calculateAllFilesSHA256(const std::string& filePath, const std::vector<std::string>& volumeFiles);

    std::vector<std::vector<std::string>> generateCSVRows();

    static QString joinStringVector(const std::vector<std::string>& vec,
        const std::string& delimiter);

    static bool readCSVFile(const std::string& file_path,
        std::vector<std::vector<std::string>>& data);

    static bool writeCSVFile(const std::string& file_path,
        const std::vector<std::vector<std::string>>& data);

    static std::string escapeCSVField(const std::string& field);

    bool isValueInLimit(const std::string& value, const std::string& low_limit, const std::string& high_limit);

    bool loadConfig();

    bool loadSummaryConfig();

    QString getStatusString(int status);

    QString getNewCsvName(const QString& filePath);
    QString getNewSummaryCsvName(const QString& filePath);
    Result readCsv(QString filePath, QList<QStringList>& data);
    Result writeCsv(QString filePath,const QList<QStringList>& data);

    QString spiltSN(QString line);

    Result setQDFData();
    Result autoDPCsvDelete(QStringList &dpHeader, QStringList &dpValue, QStringList& dpTestTime);
    Result getAutoDPCsvDelete(QStringList &deleteFields);
    Result setSummaryData();

private:
    const QString m_autoDPCsvDeletePath = ".\\config\\QdpMes\\AutoDPCsvDelete.csv";
    const QString m_configPath = ".\\config\\QdpMes\\QDFConfig.json";

    Json m_json;

    const QString m_summaryConfigPath = ".\\config\\QdpMes\\SummaryConfig.json";
    Json m_summaryJson;

    QdpCsvHeader m_csvHeader;
    SummaryCsvHeader m_summaryHeader;

    QStringList m_autoDPCsvDelete;
};