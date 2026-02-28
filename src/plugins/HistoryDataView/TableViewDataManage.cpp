#include "TableViewDataManage.h"
#include "iqmetricconfig.h"
#include <QString>
#include <map>
#include <qstringlist.h>
#include "HistoryDataBase.h"
#include <algorithm>
#include "mtfmodelconfig.h"

MetricTableArray* TableViewDataManage::m_metricTableArray = nullptr;
StringArray TableViewDataManage::headersList;
unsigned int TableViewDataManage::index = 0;
TableViewDataManage::TableViewDataManage()
{
    
}

TableViewDataManage::~TableViewDataManage()
{
    if (m_metricTableArray != nullptr)
    {
        delete m_metricTableArray;
        m_metricTableArray = nullptr;
    }
}

void TableViewDataManage::UpdateMetricsTableHeads()
{
    //init Metrics
    headersList.clear();
    IQMetricConfig::instance()->load("config/IQMetricConfig_new.json");
    std::vector<MetricDescription*>* desVec = IQMetricConfig::instance()->getMetricDescripts();

    if (m_metricTableArray == nullptr)
    {
        m_metricTableArray = new MetricTableArray;
    }

    HeaderInfo m_headerInfo;
    m_headerInfo.colIndex = 2;
    for (int i = 0; i < desVec->size(); i++)
    {
        MetricTableShow* metricTableShow = new MetricTableShow();
        MetricDescription* item = desVec->at(i);
        std::string name = item->name;
        std::string logHeader = item->logheader;
        const std::vector<std::string>& colors = item->colors;
        const std::vector<std::string>& suffixes = item->suffix;
        std::vector<std::string>& eyeboxs = item->eyeboxId;

        for each (std::string color in colors)
        {
            QString qLogHeader = QString::fromStdString(logHeader);
            QStringList qLogHeaderList = qLogHeader.split(",");
            for each (QString itemStr in qLogHeaderList)
            {
                QString headerT = QString(itemStr);
                QStringList qHeadList = headerT.split("_");
                QString header;
                int i = 0;
                foreach(const QString & str, qHeadList)
                {
                    i++;
                    if (!(str.contains("$eyeboxId$")))
                    {
                        header += str;
                        if (i < qHeadList.size())
                        {
                            header += "_";
                        }
                    }
                }
                if (header.contains("$color$"))
                {
                    header = header.replace("$color$", QString::fromStdString(color));
                }
                if (header.contains("$suffix$"))
                {
                    for each (std::string suf in suffixes)
                    {
                        QString heads2 = QString(header);
                        heads2 = heads2.replace("$suffix$", QString::fromStdString(suf));

                        m_headerInfo.headerStr = heads2.toStdString();
                        m_headerInfo.colIndex++;
                        index = m_headerInfo.colIndex;
                        m_headerInfo.color = color;
                        metricTableShow->headerInfo.push_back(m_headerInfo);
                        headersList.push_back(m_headerInfo.headerStr);
                    }

                }
                else
                {

                    m_headerInfo.headerStr = header.toStdString();
                    m_headerInfo.colIndex++;
                    index = m_headerInfo.colIndex;
                    m_headerInfo.color = color;
                    metricTableShow->headerInfo.push_back(m_headerInfo);
                    headersList.push_back(m_headerInfo.headerStr);

                }
            }
        }
        if (colors.size() == 0)
        {
            QString qLogHeader = QString::fromStdString(logHeader);
            QStringList qLogHeaderList = qLogHeader.split(",");
            for each (QString itemStr in qLogHeaderList)
            {
                QString header = QString(itemStr);
                if (header.contains("$suffix$"))
                {
                    for each (std::string suf in suffixes)
                    {
                        QString heads2 = QString(header);
                        heads2 = heads2.replace("$suffix$", QString::fromStdString(suf));
                        m_headerInfo.headerStr = heads2.toStdString();

                        m_headerInfo.colIndex++;
                        index = m_headerInfo.colIndex;
                        m_headerInfo.color = "";
                        metricTableShow->headerInfo.push_back(m_headerInfo);
                        headersList.push_back(m_headerInfo.headerStr);
                    }
                }
                else
                {
                    m_headerInfo.headerStr = header.toStdString();

                    m_headerInfo.colIndex++;
                    index = m_headerInfo.colIndex;
                    m_headerInfo.color = "";
                    metricTableShow->headerInfo.push_back(m_headerInfo);
                    headersList.push_back(m_headerInfo.headerStr);
                }
            }

        }
        metricTableShow->name = item->name;
        metricTableShow->colors = item->colors;
        metricTableShow->eyeboxId = item->eyeboxId;
        m_metricTableArray->push_back(*metricTableShow);

        if (metricTableShow != nullptr)
        {
            delete metricTableShow;
            metricTableShow = nullptr;
        }
    }
}

void TableViewDataManage::UpdateMTFHeaders()
{
    if (m_metricTableArray == nullptr)
    {
        m_metricTableArray = new MetricTableArray;
    }
    MTFModelConfig::instance()->load("config/mtfconfig.json");
    StringArray freqs = MTFModelConfig::instance()->getSpecifedFreq();
    StringArray colors = MTFModelConfig::instance()->getMeasureMTFColor();

    MetricTableShow* metricTableShow = new MetricTableShow();
    metricTableShow->name = "MTF";
    metricTableShow->colors = colors;

    HeaderInfo m_headerInfo;
    m_headerInfo.colIndex = index;

    for (int m = 0; m < colors.size(); m++)
    {
        std::string m_color = colors.at(m);
        for (int n = 0; n < 9; n++)
        {
            std::string prefix = "MTF" + std::to_string(n) +"/";
            for (int i = 0; i < freqs.size(); i++)
            {
                std::string frq = freqs.at(i);
                std::string mtfitem = prefix + frq;
                m_headerInfo.color = m_color;
                m_headerInfo.headerStr = "V" + mtfitem + "_" + m_color;
                m_headerInfo.colIndex++;
                headersList.push_back(m_headerInfo.headerStr);
                metricTableShow->headerInfo.push_back(m_headerInfo);
                m_headerInfo.headerStr = "H" + mtfitem + "_" + m_color;
                m_headerInfo.colIndex++;
                headersList.push_back(m_headerInfo.headerStr);
                metricTableShow->headerInfo.push_back(m_headerInfo);
            }
        }
        m_headerInfo.headerStr = "vid(m)_" + m_color;
        m_headerInfo.colIndex++;
        headersList.push_back(m_headerInfo.headerStr);
        metricTableShow->headerInfo.push_back(m_headerInfo);
    }
    m_metricTableArray->push_back(*metricTableShow);

    if (metricTableShow != nullptr)
    {
        delete metricTableShow;
        metricTableShow = nullptr;
    }
}

bool TableViewDataManage::CompareConfigDBTableFormat(StringArray& iqTableName)
{
    bool ret = true;
    IQTableFormatArray* iqTableFormats = new IQTableFormatArray();
    HistoryDataBase::getInstance()->queryTableFormat(iqTableFormats);
    for (int i = 0; i < iqTableFormats->size(); i++)
    {
        if (iqTableFormats->at(i).headers.size() != headersList.size())
        {
            ret = false;
            iqTableName.push_back(iqTableFormats->at(i).tableName);
            continue;
        }
       bool result = std::equal(headersList.begin(), headersList.end(), iqTableFormats->at(i).headers.begin());
       if (!result)
       {
            iqTableName.push_back(iqTableFormats->at(i).tableName);
       }
       ret &= result;
    }
    return ret;
}

void TableViewDataManage::DelDifferentHistoryTable(StringArray iqTableName)
{
    HistoryDataBase::getInstance()->DelDiffFormatTable(iqTableName);
}

void TableViewDataManage::DelTimeOutHistoryData(QString oldTime)
{
    HistoryDataBase::getInstance()->DelTimeOutDBData(oldTime);
}

MetricTableArray* TableViewDataManage::GetMetricTableArray()
{
    return m_metricTableArray;
}

void TableViewDataManage::GetDutShowData(DutInfoShowArray& dutIndoArray)
{
    HistoryDataBase::getInstance()->queryHistoryTableName(dutIndoArray);
}

void TableViewDataManage::GetImgDutShowData(DutInfoShowArray &dutIndoArray)
{
    HistoryDataBase::getInstance()->loadHistoryImgPathTableName(dutIndoArray);
}
void TableViewDataManage::GetMetricTableShowData(QString startTime, QString endTime, QString dutSn, QString eyebox, QString metricType, QString color, QStringList& headsShowList, std::vector<StringArray>& rowsAllData)
{
    UpdateAllEyeboxMetricShowTable(startTime, endTime, dutSn, eyebox, metricType, color, headsShowList, rowsAllData);
}

void TableViewDataManage::GetMetricTableImgShowDir(QString dutSn, QString eyebox, QStringList &headsShowList,std::vector<StringArray> &rowsAllData,QString startTime, QString endTime)
{
    UpdateMetricShowDirTable(dutSn, eyebox, headsShowList, rowsAllData,startTime, endTime);
}
void TableViewDataManage::UpdateAllEyeboxMetricShowTable(QString startTime, QString endTime, QString dutSn, QString eyebox, QString metricType, QString color, QStringList& headsShowList, std::vector<StringArray>& rowsAllData)
{
    MetricTableArray::iterator metricTableIter;
    HeadInfoArray::iterator headInfoIter;

    GDataMap::iterator dataMapIter;
    GColumMap::iterator columDataIter;

    rowsAllData.clear();
    StringArray m_rowData;

    headsShowList.clear();
    headsShowList.push_back("Time");
    headsShowList.push_back("eyebox");
    IntArray selIndexList;
    selIndexList.push_back(1);
    selIndexList.push_back(2);

    if (m_metricTableArray->empty())
    {
        return;
    }

    for (metricTableIter = m_metricTableArray->begin(); metricTableIter != m_metricTableArray->end(); metricTableIter++)
    {
        if ((metricType == "All") || metricTableIter->name == metricType.toStdString())
        {
            for (headInfoIter = metricTableIter->headerInfo.begin(); headInfoIter != metricTableIter->headerInfo.end(); headInfoIter++)
            {
                if ((color == "All") || (color == "") || (headInfoIter->color == color.toStdString()))
                {
                    selIndexList.push_back(headInfoIter->colIndex);
                    headsShowList.push_back(QString::fromStdString(headInfoIter->headerStr));
                }
            }

        }
    }

    GDataMap dataMap = HistoryDataBase::getInstance()->UpdateMetricTableShowInfo(startTime, endTime, dutSn, eyebox);

    for (dataMapIter = dataMap.begin(); dataMapIter != dataMap.end(); dataMapIter++)
    {
        m_rowData.clear();
        for (columDataIter = dataMapIter->second.begin(); columDataIter != dataMapIter->second.end(); columDataIter++)
        {
            for (int m = 0; m < selIndexList.size(); m++)
            {
                std::string value = dataMapIter->second[selIndexList[m]];
                m_rowData.push_back(value);
            }
        }
        rowsAllData.push_back(m_rowData);
    }
}

void TableViewDataManage::UpdateMetricShowDirTable(QString dutSn, QString eyebox, QStringList &headsShowList,
                                                   std::vector<StringArray> &rowsAllData, QString startTime,
                                                   QString endTime)
{
    MetricTableArray::iterator metricTableIter;
    HeadInfoArray::iterator headInfoIter;

    GDataMap::iterator dataMapIter;
    GColumMap::iterator columDataIter;

    rowsAllData.clear();
    StringArray m_rowData;

    headsShowList.clear();
    headsShowList.push_back("Time");
    headsShowList.push_back("eyebox");
    headsShowList.push_back("path");
    IntArray selIndexList;
    selIndexList.push_back(1);
    selIndexList.push_back(2);
    selIndexList.push_back(3);

    GDataMap dataMap = HistoryDataBase::getInstance()->UpdateImgPathDirInfo(dutSn, eyebox, startTime, endTime);
    for (dataMapIter = dataMap.begin(); dataMapIter != dataMap.end(); dataMapIter++)
    {
        m_rowData.clear();
        //for (columDataIter = dataMapIter->second.begin(); columDataIter != dataMapIter->second.end(); columDataIter++)
        
            for (int m = 0; m < selIndexList.size(); m++)
            {
                std::string value = dataMapIter->second[selIndexList[m]];
                m_rowData.push_back(value);
            }
        
        rowsAllData.push_back(m_rowData);
    }
}

void TableViewDataManage::GetMTFTableShowData(QString startTime, QString endTime, QString dutSn, QString eyebox, QString metricType, QString color, QStringList& headsShowList, std::vector<StringArray>& rowsAllData)
{
    rowsAllData.clear();
    headsShowList.clear(); //include time,eyebox,color....
    StringArray m_rowData;
    GDataMap::iterator dataMapIter;
    GColumMap::iterator columDataIter;

    GDataMap dataMap = HistoryDataBase::getInstance()->UpdateMTFTableShowInfo(startTime, endTime, dutSn, eyebox, color, headsShowList);
    
    for (dataMapIter = dataMap.begin(); dataMapIter != dataMap.end(); dataMapIter++)
    {
        m_rowData.clear();
        for (columDataIter = dataMapIter->second.begin(); columDataIter != dataMapIter->second.end(); columDataIter++)
        {
            m_rowData.push_back(columDataIter->second);
        }
        rowsAllData.push_back(m_rowData);
    }
}
