#include "GenCSVHeader.h"
#include "metricsdatabase.h"
#include "iqmetricconfig.h"
#include "limitconfig.h"
#include "mtfmodelconfig.h"

QStringList GenCSVHeader::headersList;
void GenCSVHeader::writeCsvHeadersToDBAll(int metricTableNum)
{
 std::vector<MetricDescription*>*  desVec =IQMetricConfig::instance()->getMetricDescripts();
    for (int idNum = 0; idNum < metricTableNum; idNum++)
    {
        QString id = QString::number(idNum + 1);
        for (int i = 0; i < desVec->size(); i++)
        {
            MetricDescription *item = desVec->at(i);
            std::string name = item->name;
            std::string logHeader = item->logheader;
            const std::vector<std::string> &colors = item->colors;
            const std::vector<std::string> &suffixes = item->suffix;
            std::vector<std::string> &eyeboxs = item->eyeboxId;
            LimitsInfos limits = LimitConfig::instance()->getLimitInfos(logHeader);

            if (!eyeboxs.empty() && std::find(eyeboxs.begin(), eyeboxs.end(), id.toStdString()) != eyeboxs.end())
            {
                for each (std::string color in colors)
                {
                    QString qLogHeader = QString::fromStdString(logHeader);
                    QStringList qLogHeaderList = qLogHeader.split(",");
                    for each (QString itemStr in qLogHeaderList)
                    {
                        QString header = QString(itemStr);
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
                                if (heads2.contains("$eyeboxId$"))
                                {
                                    if (std::find(eyeboxs.begin(), eyeboxs.end(), id.toStdString()) != eyeboxs.end())
                                    {
                                        QString heads3 = QString(heads2);
                                        heads3 = heads3.replace("$eyeboxId$", id);
                                        MetricsDataBase::getInstance()->insertMetricsItem(heads3, limits.uplimit, limits.lowlimit, id,
                                            QString::fromStdString(item->unit));
                                    }
                                }
                                else MetricsDataBase::getInstance()->insertMetricsItem(
                                    heads2, limits.uplimit, limits.lowlimit, id, QString::fromStdString(item->unit));
                            }
                        }
                        else
                        {
                            if (header.contains("$eyeboxId$"))
                            {
                                if (std::find(eyeboxs.begin(), eyeboxs.end(), id.toStdString()) != eyeboxs.end())
                                //for each (std::string eid in eyeboxs)
                                {
                                    QString heads2 = QString(header);
                                    heads2 = heads2.replace("$eyeboxId$", id);
                                    MetricsDataBase::getInstance()->insertMetricsItem(heads2, limits.uplimit, limits.lowlimit, id,
                                             QString::fromStdString(item->unit));
                                }
                            } 
                            else
                                MetricsDataBase::getInstance()->insertMetricsItem(
                                    header, limits.uplimit, limits.lowlimit, id, QString::fromStdString(item->unit));
                        }
                    }

                }
                if (colors.size() == 0)
                {
                    for each (std::string suf in suffixes)
                    {
                        QString qLogHeader = QString::fromStdString(logHeader);
                        qLogHeader = qLogHeader.replace("$suffix$", QString::fromStdString(suf));

                        if (qLogHeader.contains("$eyeboxId$"))
                        {
                            if (std::find(eyeboxs.begin(), eyeboxs.end(), id.toStdString()) != eyeboxs.end())
                            {
                                QString heads = QString(qLogHeader);
                                heads = heads.replace("$eyeboxId$", id);
                                MetricsDataBase::getInstance()->insertMetricsItem(heads, limits.uplimit, limits.lowlimit,
                                                                            id, QString::fromStdString(item->unit));
                            }
                        }

                        else MetricsDataBase::getInstance()->insertMetricsItem(qLogHeader, limits.uplimit, limits.lowlimit,
                                                                          id, QString::fromStdString(item->unit));
                    }
                    if (suffixes.size() == 0)
                    {
                        QString qLogHeader = QString::fromStdString(logHeader);
                        if (qLogHeader.contains("$eyeboxId$"))
                        {
                            if (std::find(eyeboxs.begin(), eyeboxs.end(), id.toStdString()) != eyeboxs.end())
                            {
                                qLogHeader = qLogHeader.replace("$eyeboxId$", id);
                                MetricsDataBase::getInstance()->insertMetricsItem(qLogHeader, limits.uplimit,
                                                                                  limits.lowlimit, id,
                                                                                  QString::fromStdString(item->unit));
                            }
                        }
                        else MetricsDataBase::getInstance()->insertMetricsItem(qLogHeader, limits.uplimit, limits.lowlimit,
                                                                          id, QString::fromStdString(item->unit));
                    }
                }
            }
        }

    }

}

Result GenCSVHeader::writeCsvHeadersToDB()
{
    try {
        std::vector<MetricDescription*>* desVec = IQMetricConfig::instance()->getMetricDescripts();
        for (int i = 0; i < desVec->size(); i++)
        {
            MetricDescription* item = desVec->at(i);
            std::string name = item->name;
            std::string logHeader = item->logheader;
            const std::vector<std::string>& colors = item->colors;
            const std::vector<std::string>& suffixes = item->suffix;
            std::vector<std::string>& eyeboxs = item->eyeboxId;
            {
                for (int index = 0; index < eyeboxs.size(); index++)
                {
                    QString id = QString::fromStdString(eyeboxs.at(index));
                    for each (std::string color in colors)
                    {
                        QString qLogHeader = QString::fromStdString(logHeader);
                        QStringList qLogHeaderList = qLogHeader.split(",");
                        for each (QString itemStr in qLogHeaderList)
                        {
                            QString header = QString(itemStr);
                            LimitsInfos limits = LimitConfig::instance()->getLimitInfos(header.toStdString());
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
                                    limits = LimitConfig::instance()->getLimitInfos(heads2.toStdString());
                                    if (heads2.contains("$eyeboxId$"))
                                    {
                                        if (std::find(eyeboxs.begin(), eyeboxs.end(), id.toStdString()) !=
                                            eyeboxs.end())
                                        {
                                            QString heads3 = QString(heads2);
                                            heads3 = heads3.replace("$eyeboxId$", id);
                                            limits = LimitConfig::instance()->getLimitInfos(heads3.toStdString());
                                            MetricsDataBase::getInstance()->insertMetricsItem(heads3, limits.uplimit, limits.lowlimit, id,
                                                QString::fromStdString(item->unit));
                                        }
                                    }
                                    else
                                        MetricsDataBase::getInstance()->insertMetricsItem(heads2, limits.uplimit, limits.lowlimit, id,
                                            QString::fromStdString(item->unit));
                                }
                            }
                            else
                            {
                                if (header.contains("$eyeboxId$"))
                                {
                                    if (std::find(eyeboxs.begin(), eyeboxs.end(), id.toStdString()) != eyeboxs.end())
                                    {
                                        QString heads2 = QString(header);
                                        heads2 = heads2.replace("$eyeboxId$", id);
                                        limits = LimitConfig::instance()->getLimitInfos(heads2.toStdString());
                                        MetricsDataBase::getInstance()->insertMetricsItem(heads2, limits.uplimit, limits.lowlimit, id,
                                            QString::fromStdString(item->unit));
                                    }
                                }
                                else
                                    MetricsDataBase::getInstance()->insertMetricsItem(header, limits.uplimit, limits.lowlimit, id, QString::fromStdString(item->unit));
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
                            LimitsInfos limits = LimitConfig::instance()->getLimitInfos(header.toStdString());
                            if (header.contains("$suffix$"))
                            {
                                for each (std::string suf in suffixes)
                                {
                                    QString heads2 = QString(header);
                                    heads2 = heads2.replace("$suffix$", QString::fromStdString(suf));
                                    if (heads2.contains("$eyeboxId$"))
                                    {
                                        if (std::find(eyeboxs.begin(), eyeboxs.end(), id.toStdString()) != eyeboxs.end())
                                        {
                                            QString heads = QString(heads2);
                                            heads = heads.replace("$eyeboxId$", id);
                                            MetricsDataBase::getInstance()->insertMetricsItem(heads, limits.uplimit, limits.lowlimit, id,
                                                QString::fromStdString(item->unit));
                                        }
                                    }

                                    else
                                        MetricsDataBase::getInstance()->insertMetricsItem(heads2, limits.uplimit, limits.lowlimit, id,
                                            QString::fromStdString(item->unit));
                                }
                            }
                            else
                            {
                                if (header.contains("$eyeboxId$"))
                                {
                                    if (std::find(eyeboxs.begin(), eyeboxs.end(), id.toStdString()) != eyeboxs.end())
                                    {
                                        header = header.replace("$eyeboxId$", id);
                                        MetricsDataBase::getInstance()->insertMetricsItem(header, limits.uplimit, limits.lowlimit, id,
                                            QString::fromStdString(item->unit));
                                    }
                                }
                                else
                                    MetricsDataBase::getInstance()->insertMetricsItem(header, limits.uplimit, limits.lowlimit, id,
                                        QString::fromStdString(item->unit));
                            }
                        }
                    }
                }
            }
        }
        return Result();
    }
    catch (std::exception e)
    {
        //LoggingWrapper::instance()->error("IQ recipe error," + QString::fromStdString(e.what()));
        return Result(false, "Write csv headers to db error, "+ QString(e.what()).toStdString());
    }
}
bool GenCSVHeader::crateMetricsTable(QStringList eyeboxList)
{
    if (eyeboxList.empty())
    {
        return false;
    }
    bool ret = MetricsDataBase::getInstance()->createMultiMetricsTable(eyeboxList);

    if(!ret){
        return false;
    }
    return true;

	//return MetricsDataBase::getInstance()->createMetricsTable();
    //return MetricsDataBase::getInstance()->createMultiMetricsTable(n);
}

void GenCSVHeader::writeHeadersToHistoryDB()
{
    //TODO: not used
    return;

    std::vector<MetricDescription*>* desVec = IQMetricConfig::instance()->getMetricDescripts();
    headersList.clear();

    for (int i = 0; i < desVec->size(); i++)
    {
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
                        headersList.push_back(heads2);
                    }
                }
                else
                {
                    headersList.push_back(header);
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
                        headersList.push_back(heads2);
                    }
                }
                else
                {
                    headersList.push_back(header);
                }
            }

        }
    }
}

bool GenCSVHeader::createMetricsHistoryTable(QString dutId)
{
    // TODO: not used
    return true;

    QStringList mtfTableHeadersList;
    createMTFHeaders(mtfTableHeadersList);
    headersList.append(mtfTableHeadersList);
    return MetricsDataBase::getInstance()->createMultiMetricsHistoryTable(dutId,&headersList);
}

bool GenCSVHeader::createMetricsHistoryPathTable(QString dutId,QString path)
{
    // TODO: not used
    return true;

    return MetricsDataBase::getInstance()->createIQMetricsDirectoryHistoryTable(dutId, &headersList);
}
void GenCSVHeader::SetMetricsHistoryTableTime(QString time)
{
    // TODO: not used
    return;

    MetricsDataBase::getInstance()->SetMetricsHistoryTableTime(time);
}

void GenCSVHeader::createMTFHeaders(QStringList & mtfTableHeadersList)
{
    mtfTableHeadersList.clear();
    std::vector<std::string> freqs = MTFModelConfig::instance()->getSpecifedFreq();
    std::vector<std::string> colors = MTFModelConfig::instance()->getMeasureMTFColor();
    for (int m = 0; m < colors.size(); m++)
    {
        QString m_color = QString::fromStdString(colors.at(m));
        for (int n = 0; n < 9; n++)
        {
            QString prefix = "MTF" + QString::number(n) + "/";
            for (int i = 0; i < freqs.size(); i++)
            {
                std::string frq = freqs.at(i);
                QString mtfitem = prefix + QString::fromStdString(frq);

                mtfTableHeadersList.append("V" + mtfitem + "_" + m_color);
                mtfTableHeadersList.append("H" + mtfitem + "_" + m_color);

            }
        }
        mtfTableHeadersList << "vid(m)_" + m_color;
    }
}

QList<QString> GenCSVHeader::getMetricsEyeboxIds()
{
    std::vector<MetricDescription *> *desVec = IQMetricConfig::instance()->getMetricDescripts();
    QList<QString> eyeboxIds;
    for (int i = 0; i < desVec->size(); i++)
    {
        std::vector<std::string> ids = desVec->at(i)->eyeboxId;
        for (int j = 0; j < ids.size(); ++j)
        {
            QString id = QString::fromStdString(ids.at(j));
            if (!eyeboxIds.contains(id))
            {
                eyeboxIds.push_back(id);
            }
        }
    }
    return eyeboxIds;
}

bool GenCSVHeader::createMTFTable(QString dutId)
{
    bool ret = false;
    QStringList mtfTableHeadersList;
    mtfTableHeadersList.clear();
    mtfTableHeadersList << "Time" << "eyebox" << "color";
    std::vector<std::string> freqs = MTFModelConfig::instance()->getSpecifedFreq();
    for (int n = 0; n < 9; n++)
    {
        QString prefix = "MTF" + QString::number(n) + "/";
        for (int i = 0; i < freqs.size(); i++)
        {
            std::string frq = freqs.at(i);
            //QString mtfitem = prefix + QString::number(frq, 'f', 3);
            QString mtfitem = prefix + QString::fromStdString(frq);
            mtfTableHeadersList.append("V" + mtfitem);
            mtfTableHeadersList.append("H" + mtfitem);
        }
    }
    mtfTableHeadersList << "vid(m)";

    ret = MetricsDataBase::getInstance()->createMTFHistoryTable(dutId, &mtfTableHeadersList);

    return ret;
}
