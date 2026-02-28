#include "resultprocessor.h"
#include <QDir>
#include "mtfmodelconfig.h"
#include "mtfresultwidget.h"
#include "PrjCommon/metricsdata.h"
ResultProcessor::ResultProcessor(QObject *parent)
	: QObject(parent)
{
}

ResultProcessor::ResultProcessor()
{
}

ResultProcessor::~ResultProcessor()
{
}

void ResultProcessor::saveTOCVSData(QString filePath, QString dutId)
{
	//QString csvFileName = filePath + QDir::separator() + "result_mtf.csv";
	//QFile resultFile(csvFileName);
	QFile resultFile(filePath);
	resultFile.open(QIODevice::Append | QIODevice::Text);
	if (resultFile.size() > 1) {
		genMTFCsvHeaders();
	}
	else {
		genMTFCsvHeaders();
		resultFile.write(mtfHeadersList.join(",").toUtf8());
		resultFile.write("\n");
		QString upLimit = genMTFUpLimitRow();
		resultFile.write(upLimit.toUtf8());
		QString lowLimit = genMTFLowLimitRow();
		resultFile.write(lowLimit.toUtf8());
	}
	const QString& itemRow = genMTFItemRow();
	resultFile.write(itemRow.toUtf8());
	resultFile.write("\n");
	resultFile.close();
	MTFResultWidget::getInstance()->updateTableWidget(metricVec);
	metricVec.clear();
}

void ResultProcessor::addMetricItem(MetricItem item)
{
	metricVec.append(item);
}

void ResultProcessor::clearMetricVector()
{
	metricVec.clear();
}

void ResultProcessor::setVid(float val)
{
	vid = val;
}

void ResultProcessor::getMTFData(QStringList& mtfData)
{
	mtfData.clear();
	QString dutBarStr = MetricsData::instance()->getDutBarCode();
	QStringList dytBarList = dutBarStr.split("_");
	QString ctime = "";
	QString color = "";
	QString eyebox = "";
	int i = 0;
	do
	{
		i++;
		if ((dytBarList.at(i) == "w") || (dytBarList.at(i) == "r") || (dytBarList.at(i) == "g") || (dytBarList.at(i) == "b"))
		{
			color = dytBarList.at(i);
		}
		if (dytBarList.at(i).contains("EB",Qt::CaseInsensitive))
		{
			eyebox = dytBarList.at(i).right(1);
		}
		if (dytBarList.at(i).contains("T", Qt::CaseInsensitive))
		{
			QString yearStr = dytBarList.at(i).mid(0, 4);
			QString monthStr = dytBarList.at(i).mid(4, 2);
			QString dayStr = dytBarList.at(i).mid(6, 2);
			QString hourStr = dytBarList.at(i).mid(dytBarList.at(i).indexOf("T") + 1, 2);
			QString minStr = dytBarList.at(i).mid(dytBarList.at(i).indexOf("T") + 3, 2);
			QString secStr = dytBarList.at(i).mid(dytBarList.at(i).indexOf("T") + 5, 2);
			ctime = yearStr + monthStr + dayStr + hourStr + minStr + secStr;
		}

	} while (i < dytBarList.count()-1);

	mtfData.push_back(ctime);
	mtfData.push_back(eyebox);
	mtfData.push_back(color);

	for each (MetricItem data in metricVec)
	{
		double value = data.value;
		if (std::isnan(value))
		{
			value = 0.0;
		}
		mtfData.push_back(QString::number(value));
	}
	mtfData.push_back(QString::number(vid));
}

void ResultProcessor::genMTFCsvHeaders()
{
	mtfHeadersList.clear();
	mtfHeadersList<< "Serial Number"<<"Test Total Time"<<"Test Result";
	std::vector<std::string> freqs= MTFModelConfig::instance()->getSpecifedFreq();
	for (int n = 0; n < mtfAOINum; n++) {
		QString prefix = "MTF" + QString::number(n)+"/";
		for (int i = 0; i < freqs.size(); i++) {
			std::string frq = freqs.at(i);
			//QString mtfitem = prefix + QString::number(frq, 'f', 3);
			QString mtfitem = prefix + QString::fromStdString(frq);
			mtfHeadersList.append("V"+mtfitem);
			mtfHeadersList.append("H" + mtfitem);
		}
	}
	mtfHeadersList << "vid(m)";
}

QString ResultProcessor::genMTFUpLimitRow()
{
	QString upLimitStr = "Upper Limit, , ";
	for (int i = 0; i < mtfAOINum; i++) {
		std::vector<MTFLimitConfig> limit =  MTFModelConfig::instance()->getMTFLimit(i);
		for (int n = 0; n < limit.size(); n++) {
			MTFLimitConfig item = limit.at(n);
			QString str = QString::number(item.upperLimit, 'f', 3);
			QString VandHStr = str + "," + str;
			upLimitStr = upLimitStr + "," + VandHStr;
		}
	}
	return upLimitStr + "\n";
}

QString ResultProcessor::genMTFLowLimitRow()
{
	QString lowLimitStr = "Lower Limit, , ";
	for (int i = 0; i < mtfAOINum; i++) {
		std::vector<MTFLimitConfig> limit = MTFModelConfig::instance()->getMTFLimit(i);
		for (int n = 0; n < limit.size(); n++) {
			MTFLimitConfig item = limit.at(n);
			QString str = QString::number(item.lowerLimit, 'f', 3);
			QString VandHStr = str + "," + str;
			lowLimitStr = lowLimitStr + "," + VandHStr;
		}
	}
	return lowLimitStr + "\n";
}

QString ResultProcessor::genMTFItemRow()
{
	QStringList rowItemLst;
	rowItemLst << "deviceId" << "totaltime" << "passOrFail";
	bool isPass = true;
	for (int i = 3; i < mtfHeadersList.size(); i++) {
		QString field = mtfHeadersList.at(i);
		bool isNull = true;
		for each (MetricItem data in metricVec)
		{
			if (data.metricName.toLower().trimmed() == field.toLower().trimmed()) {
				isNull = false;
				double val = data.value;
				if (val > data.upLimit || val < data.lowLimit) {
					isPass = false;
				}
				rowItemLst.append(QString::number(val, 'f', 3));
			}
		}
		if (isNull) {
			if (!field.contains("vid"))
			{
				rowItemLst.append(" ");
			}
		}
	}
	if (isPass) {
		rowItemLst[2] = "Pass";
	}
	else {
		rowItemLst[2] = "Fail";
	}
	rowItemLst[0] = MetricsData::instance()->getDutBarCode();
	QDateTime endTime = QDateTime::currentDateTime();
	qint64 endStamp = endTime.toTime_t();
	qint64 startStamp =MetricsData::instance()->getMTFStartTime();
	QString rangeStr = tr("%1s").arg(endStamp - startStamp);
	rowItemLst[1] = rangeStr;
	rowItemLst.append(QString::number(vid,'f',4));
	return rowItemLst.join(",");
}
